#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Ingress WebSocket client for touch_manager (v0.1 MVP).

Connects to touch_ingress instances via WebSocket using come.1 JSON protocol.
"""

import asyncio
import json
import logging
import os
import time
import websockets
from datetime import datetime
from typing import Optional, Dict, Callable
from threading import Thread, Event
from config import Config
from protocol import (
    MANAGER_CONNECT,
    MANAGER_EDGE_LIST,
    MANAGER_EDGE_KICK,
    INGRESS_EDGE_ONLINE,
    INGRESS_EDGE_OFFLINE,
)

logger = logging.getLogger(__name__)

# 修复：Python urllib 不识别 no_proxy 中的 CIDR 格式（如 127.0.0.0/8），
# 导致 127.0.0.1 的连接被错误地路由到系统代理。
# 在此显式补充 127.0.0.1 和 localhost 到 no_proxy，确保本地连接不走代理。
def _fix_no_proxy():
    for var in ('no_proxy', 'NO_PROXY'):
        current = os.environ.get(var, '')
        entries = set(e.strip() for e in current.split(',') if e.strip())
        entries.update({'127.0.0.1', 'localhost', '::1'})
        os.environ[var] = ','.join(sorted(entries))

_fix_no_proxy()

class IngressClient:
    """WebSocket client for connecting to touch_ingress."""

    def __init__(self, ingress_id: str, host: str, port: int):
        self.ingress_id = ingress_id
        self.host = host
        self.port = port
        self.ws_url = f"ws://{host}:{port}/come"
        self.websocket = None
        self.connected = False
        self.running = False
        self._loop = None
        self._thread = None
        self._stop_event = Event()
        self._reconnect_interval = 10  # seconds
        self._edge_list_futures = {}  # 存储边缘列表查询的 Future 对象
        self._query_id_counter = 0  # 查询 ID 计数器

    def start(self):
        """Start the WebSocket client in a background thread."""
        if self.running:
            logger.warning(f"Ingress {self.ingress_id} client already running")
            return

        self.running = True
        self._stop_event.clear()
        self._thread = Thread(target=self._run_loop, daemon=True)
        self._thread.start()
        logger.info(f"Ingress {self.ingress_id} client thread started")

    def stop(self):
        """Stop the WebSocket client."""
        if not self.running and not self._thread:
            return

        self.running = False
        self._stop_event.set()

        # 关闭 WebSocket 连接
        if self.websocket:
            asyncio.run_coroutine_threadsafe(self.websocket.close(), self._loop)

        # 停止事件循环
        if self._loop and self._loop.is_running():
            asyncio.run_coroutine_threadsafe(self._close(), self._loop)
            # 给事件循环一点时间处理关闭
            time.sleep(0.1)

        # 等待线程结束（缩短超时时间）
        if self._thread:
            self._thread.join(timeout=2)
            self._thread = None

        logger.info(f"Ingress {self.ingress_id} client stopped")

    def _run_loop(self):
        """Run the asyncio event loop in a separate thread."""
        self._loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self._loop)
        self._loop.run_until_complete(self._connect_loop())

    async def _connect_loop(self):
        """Main connection loop with reconnection logic."""
        while self.running:
            try:
                logger.info(f"Connecting to ingress {self.ingress_id} at {self.ws_url} (running={self.running})")
                # Note: subprotocols may not be required for initial connection
                # The "come.1" protocol is used in the WebSocket server config but may not be required in client
                try:
                    # websockets 11.0+ 使用 additional_headers 而不是 extra_headers
                    # 2026-02-25: ezsocket 库已修复 pong payload 问题（符合 RFC 6455）
                    # - 现在服务端会原样返回 ping 的 payload
                    # - 启用客户端 ping（15 秒）作为服务端 ping（30 秒）的补充检测
                    # 配置说明：
                    # - ping_timeout 应该是 ping_interval 的 1/3 到 1/2（RFC 6455 最佳实践）
                    # - interval=15s, timeout=5s 可以在 20 秒内检测到连接问题
                    websocket = await websockets.connect(
                        self.ws_url,
                        subprotocols=["come.1"],  # Try with subprotocol first
                        ping_interval=Config.WEBSOCKET_PING_INTERVAL,
                        ping_timeout=Config.WEBSOCKET_PING_TIMEOUT,
                        close_timeout=5,
                        additional_headers={"User-Agent": "touch_manager/0.1"}
                    )
                    logger.info(f"Connected to ingress {self.ingress_id} with subprotocol 'come.1'")
                except (websockets.exceptions.InvalidStatusCode, websockets.exceptions.InvalidHandshake) as e:
                    # If subprotocol fails, try without it
                    logger.warning(f"Connection with subprotocol failed, retrying without: {e}")
                    websocket = await websockets.connect(
                        self.ws_url,
                        ping_interval=Config.WEBSOCKET_PING_INTERVAL,
                        ping_timeout=Config.WEBSOCKET_PING_TIMEOUT,
                        close_timeout=5,
                        additional_headers={"User-Agent": "touch_manager/0.1"}
                    )
                    logger.info(f"Connected to ingress {self.ingress_id} without subprotocol")

                async with websocket:
                    self.websocket = websocket
                    self.connected = True
                    logger.info(f"WebSocket context entered for ingress {self.ingress_id}")

                    # Send initial handshake (v0.1 MVP - simple connection)
                    await self._send_handshake()

                    # Start receiving messages
                    await self._receive_messages()

            except websockets.exceptions.ConnectionClosed as e:
                logger.warning(f"Connection to ingress {self.ingress_id} closed: code={e.code}, reason={e.reason}")
                self.connected = False
                # Ingress 离线时，将该 ingress 上的所有设备状态更新为离线
                await self._notify_ingress_offline()
            except websockets.exceptions.InvalidStatusCode as e:
                logger.error(f"Invalid HTTP status code from ingress {self.ingress_id}: {e.status_code} - {e.headers if hasattr(e, 'headers') else ''}")
                self.connected = False
            except websockets.exceptions.InvalidHandshake as e:
                logger.error(f"WebSocket handshake failed for ingress {self.ingress_id}: {e}")
                self.connected = False
            except websockets.exceptions.InvalidURI as e:
                logger.error(f"Invalid URI for ingress {self.ingress_id}: {e}")
                self.connected = False
            except ConnectionRefusedError:
                logger.warning(f"Connection refused to ingress {self.ingress_id} at {self.host}:{self.port} - is touch_ingress running?")
                self.connected = False
                # 如果无法连接，说明 ingress 离线，更新该 ingress 上的所有设备状态
                await self._notify_ingress_offline()
            except OSError as e:
                error_msg = str(e)
                if "did not receive a valid HTTP response" in error_msg:
                    logger.warning(f"Cannot connect to ingress {self.ingress_id} at {self.host}:{self.port} - WebSocket handshake failed. Is touch_ingress running and accepting WebSocket connections? Error: {e}")
                elif "Connection refused" in error_msg:
                    logger.warning(f"Connection refused to ingress {self.ingress_id} at {self.host}:{self.port} - is touch_ingress running?")
                else:
                    logger.error(f"Network error connecting to ingress {self.ingress_id}: {e}")
                self.connected = False
                # 如果无法连接，说明 ingress 离线，更新该 ingress 上的所有设备状态
                await self._notify_ingress_offline()
            except Exception as e:
                logger.error(f"Error connecting to ingress {self.ingress_id}: {type(e).__name__}: {e}", exc_info=True)
                self.connected = False
                # 连接错误时，也更新设备状态
                await self._notify_ingress_offline()

            if not self.running:
                logger.info(f"Exiting connection loop for ingress {self.ingress_id} (running={self.running})")
                break

            # Wait before reconnecting
            logger.info(f"Reconnecting to ingress {self.ingress_id} in {self._reconnect_interval} seconds...")
            await asyncio.sleep(self._reconnect_interval)

    async def _send_handshake(self):
        """Send initial handshake message to ingress."""
        # 阶段 2：发送 JSON-RPC 2.0 通知：manager.connect
        handshake = {
            "jsonrpc": "2.0",
            "method": MANAGER_CONNECT,
            "params": {
                "manager_id": "touch_manager",
                "ingress_id": self.ingress_id,
                "timestamp": datetime.utcnow().isoformat() + "Z"
            }
        }
        logger.info(f"Sending manager.connect notification to ingress {self.ingress_id}: {handshake}")
        await self.send_message(handshake)

    async def _receive_messages(self):
        """Receive and process messages from ingress."""
        try:
            logger.info(f"Starting to receive messages from ingress {self.ingress_id}")
            async for message in self.websocket:
                try:
                    data = json.loads(message)
                    logger.debug(f"Received raw message from ingress {self.ingress_id}: {data}")
                    await self._handle_message(data)
                except json.JSONDecodeError as e:
                    logger.error(f"Invalid JSON from ingress {self.ingress_id}: {e}, raw message: {message[:200]}")
                except Exception as e:
                    logger.error(f"Error handling message from ingress {self.ingress_id}: {type(e).__name__}: {e}", exc_info=True)
        except websockets.exceptions.ConnectionClosed as e:
            logger.warning(f"WebSocket connection to ingress {self.ingress_id} closed: code={e.code}, reason={e.reason}")
            self.connected = False  # 确保连接状态及时更新
            # Ingress 离线时，将该 ingress 上的所有设备状态更新为离线
            await self._notify_ingress_offline()
        except Exception as e:
            logger.error(f"Error receiving messages from ingress {self.ingress_id}: {type(e).__name__}: {e}", exc_info=True)
            self.connected = False  # 确保连接状态及时更新
            # 如果连接错误导致断开，也需要更新设备状态
            await self._notify_ingress_offline()

    async def _handle_message(self, data: Dict):
        """Handle incoming message from ingress."""
        # 阶段 2：优先判断是否为 JSON-RPC 2.0 格式
        if data.get("jsonrpc") == "2.0":
            # JSON-RPC 2.0 格式
            if "method" in data and "id" not in data:
                # 通知消息（无 id）
                method = data.get("method", "")
                params = data.get("params", {})
                if method == INGRESS_EDGE_ONLINE:
                    # 阶段 2：处理 edge.online 通知（Ingress 主动上报边缘上线，如 Manager 重启后连接 Ingress）
                    edge_id = params.get("edge_id")
                    edge_type = params.get("edge_type", "unknown")
                    local_ip = params.get("local_ip", "")
                    public_ip = params.get("public_ip", "")
                    online_since = params.get("online_since")  # UTC 时间戳
                    if edge_id:
                        logger.info(f"Edge online_since={online_since}")
                        # 通知格式不需要返回响应，直接更新边缘状态
                        await self._handle_edge_online_notification(edge_id, edge_type, local_ip, public_ip, online_since)
                    else:
                        logger.warning(f"Received edge.online notification without edge_id from ingress {self.ingress_id}: {data}")
                elif method == INGRESS_EDGE_OFFLINE:
                    # 阶段 2：处理 edge.offline 通知
                    edge_id = params.get("edge_id")
                    if edge_id:
                        logger.info(f"Edge offline on ingress {self.ingress_id}")
                        await self._notify_edge_offline(edge_id)
                    else:
                        logger.warning(f"Received edge.offline notification without edge_id from ingress {self.ingress_id}: {data}")
                else:
                    logger.info(f"Received unknown JSON-RPC 2.0 notification method '{method}' from ingress {self.ingress_id}: {data}")
            elif "method" in data and "id" in data:
                # 请求消息（有 id）- 需要返回响应
                method = data.get("method", "")
                params = data.get("params", {})
                req_id = data.get("id")

                if method == INGRESS_EDGE_ONLINE:
                    # 阶段 2：处理 edge.online 请求
                    edge_id = params.get("edge_id")
                    edge_type = params.get("edge_type", "unknown")
                    local_ip = params.get("local_ip", "")
                    public_ip = params.get("public_ip", "")
                    online_since = params.get("online_since")  # UTC 时间戳
                    if edge_id:
                        logger.info(f"Edge online_since={online_since}")
                        await self._handle_edge_online_request(req_id, edge_id, edge_type, local_ip, public_ip, online_since)
                    else:
                        logger.warning(f"Received edge.online request without edge_id from ingress {self.ingress_id}: {data}")
                        # 返回错误响应
                        error_response = {
                            "jsonrpc": "2.0",
                            "error": {"code": -32602, "message": "Invalid params: missing edge_id"},
                            "id": req_id
                        }
                        await self.send_message(error_response)
                elif method == INGRESS_EDGE_OFFLINE:
                    # 阶段 2：处理 edge.offline 请求（需要返回确认响应）
                    edge_id = params.get("edge_id")
                    if edge_id:
                        logger.info(f"Edge offline request on ingress {self.ingress_id} (req_id={req_id})")
                        await self._handle_edge_offline_request(req_id, edge_id)
                    else:
                        logger.warning(f"Received edge.offline request without edge_id from ingress {self.ingress_id}: {data}")
                        error_response = {
                            "jsonrpc": "2.0",
                            "error": {"code": -32602, "message": "Invalid params: missing edge_id"},
                            "id": req_id
                        }
                        await self.send_message(error_response)
                else:
                    logger.info(f"Received unknown JSON-RPC 2.0 request method '{method}' from ingress {self.ingress_id}: {data}")
            elif "result" in data or "error" in data:
                # 响应消息
                req_id = data.get("id")
                if req_id and hasattr(self, '_req_id_to_query_id') and req_id in self._req_id_to_query_id:
                    # 通过 req_id 查找对应的 query_id 和 Future
                    query_id = self._req_id_to_query_id.pop(req_id)
                    if hasattr(self, '_edge_list_futures') and query_id in self._edge_list_futures:
                        future = self._edge_list_futures.pop(query_id)
                        if not future.done():
                            future.set_result(data)
                            logger.debug(f"Edge list response (id={req_id}, query_id={query_id}) from ingress {self.ingress_id}")
                        else:
                            logger.warning(f"Future for query_id={query_id} already done")
                    else:
                        logger.warning(f"Received edge.list response for unknown query_id={query_id} from ingress {self.ingress_id}")
                else:
                    logger.info(f"Received JSON-RPC 2.0 response (id={req_id}) from ingress {self.ingress_id}: {data}")
            else:
                logger.info(f"Received unknown JSON-RPC 2.0 message from ingress {self.ingress_id}: {data}")
        else:
            # 旧格式兼容（如果还有的话）
            msg_type = data.get("type", "")
            logger.debug(f"Handling old format message type '{msg_type}' from ingress {self.ingress_id}: {data}")

            if msg_type == "load_report":
                logger.info(f"Received load report from ingress {self.ingress_id}: {data}")
            elif msg_type == "ping":
                logger.debug(f"Received ping from ingress {self.ingress_id}, sending pong")
                await self.send_message({"type": "pong"})
            else:
                logger.info(f"Received unknown old format message type '{msg_type}' from ingress {self.ingress_id}: {data}")

    async def _handle_edge_online_request(self, req_id, edge_id, edge_type, local_ip, public_ip, online_since):
        """处理 ingress 发来的 edge.online 请求，返回 confirmed 状态"""
        try:
            # 通过内部方法更新边缘状态（不通过 HTTP，直接调用数据库）
            from database import get_db, Edge, EDGE_STATUS_OFFLINE
            from datetime import datetime

            now = datetime.utcnow()
            if online_since:
                now = datetime.utcfromtimestamp(online_since)

            db = get_db()
            try:
                edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

                if not edge:
                    # 新边缘自动注册
                    edge = Edge(
                        edge_id=edge_id,
                        edge_type=edge_type,
                        edge_key='',
                        local_ip=local_ip,
                        public_ip=public_ip,
                        status=1,  # online
                        confirmed=99,  # pending
                        ingress_id=self.ingress_id,
                        last_online_time=now,
                        last_offline_time=now
                    )
                    db.add(edge)
                    confirmed = 99
                else:
                    # 检查是否被拒绝
                    if edge.confirmed == 0:
                        confirmed = 0  # 黑名单
                        # 黑名单边缘尝试上线，设置为离线状态
                        edge.status = EDGE_STATUS_OFFLINE
                        edge.updated_at = datetime.utcnow()
                    else:
                        confirmed = edge.confirmed
                        # 更新状态
                        edge.status = 1
                        edge.last_online_time = now
                        edge.last_offline_time = now
                        edge.ingress_id = self.ingress_id
                        edge.updated_at = datetime.utcnow()

                db.commit()

                # 如果边缘在黑名单中（confirmed=0），发送 kick 命令给 ingress
                if confirmed == 0:
                    logger.warning(f"Edge {edge_id} is in blacklist (confirmed=0), sending kick command to ingress")
                    kick_command = {
                        "jsonrpc": "2.0",
                        "method": MANAGER_EDGE_KICK,
                        "params": {
                            "edge_id": edge_id,
                            "reason": "Edge is in blacklist"
                        },
                        "id": int(time.time() * 1000) % (2**31)
                    }
                    await self.send_message(kick_command)

                # 返回成功响应
                response = {
                    "jsonrpc": "2.0",
                    "result": {
                        "edge_id": edge_id,
                        "success": True,
                        "confirmed": confirmed,
                        "message": "Edge online confirmed"
                    },
                    "id": req_id
                }
                await self.send_message(response)
                logger.info(f"Sent edge.online confirmation response to ingress {self.ingress_id} (req_id={req_id}, edge_id={edge_id}, confirmed={confirmed})")

            finally:
                db.close()

        except Exception as e:
            logger.error(f"Error handling edge.online request: {e}")
            # 返回错误响应
            error_response = {
                "jsonrpc": "2.0",
                "error": {"code": 50001, "message": str(e)},
                "id": req_id
            }
            await self.send_message(error_response)

    async def _handle_edge_online_notification(self, edge_id, edge_type, local_ip, public_ip, online_since):
        """处理 ingress 发来的 edge.online 通知（不需要返回响应）"""
        try:
            # 通过内部方法更新设备状态（不通过 HTTP，直接调用数据库）
            from database import get_db, Edge
            from datetime import datetime

            now = datetime.utcnow()
            if online_since:
                now = datetime.utcfromtimestamp(online_since)

            db = get_db()
            try:
                edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

                if not edge:
                    # 新边缘自动注册
                    edge = Edge(
                        edge_id=edge_id,
                        edge_type=edge_type,
                        edge_key='',
                        local_ip=local_ip,
                        public_ip=public_ip,
                        status=1,  # online
                        confirmed=99,  # pending
                        ingress_id=self.ingress_id,
                        last_online_time=now,
                        last_offline_time=now
                    )
                    db.add(edge)
                    logger.info(f"New edge {edge_id} auto-registered from ingress notification: status=online, confirmed=99 (pending), ingress_id={self.ingress_id}")
                else:
                    # 已存在边缘，更新状态
                    # 检查是否被拒绝
                    if edge.confirmed == 0:
                        # 边缘在黑名单中，但 ingress 仍然上报在线，需要踢掉设备
                        logger.warning(f"Edge {edge_id} is in blacklist (confirmed=0) but ingress reported online, sending kick command")
                        kick_command = {
                            "jsonrpc": "2.0",
                            "method": MANAGER_EDGE_KICK,
                            "params": {
                                "edge_id": edge_id,
                                "reason": "Edge is in blacklist"
                            },
                            "id": int(time.time() * 1000) % (2**31)
                        }
                        await self.send_message(kick_command)
                    else:
                        # 更新状态
                        edge.status = 1
                        edge.last_online_time = now
                        edge.last_offline_time = now
                        edge.ingress_id = self.ingress_id
                        logger.info(f"Edge {edge_id} status updated from ingress notification (status=online, confirmed={edge.confirmed}, ingress_id={self.ingress_id})")

                db.commit()

            finally:
                db.close()

        except Exception as e:
            logger.error(f"Error handling edge.online notification: {e}")

    async def _notify_edge_online(self, edge_id: str, edge_type: str = 'unknown', local_ip: str = '', public_ip: str = '', online_since: int = None):
        """Notify manager that a edge is online (called by ingress).

        直接更新数据库，不通过 HTTP API。
        使用数据库锁保护并发更新。
        """
        try:
            from database import get_db, get_db_lock, Edge, EDGE_STATUS_ONLINE

            now = datetime.utcnow()
            if online_since:
                now = datetime.utcfromtimestamp(online_since)

            # 使用数据库锁保护并发更新
            with get_db_lock():
                db = get_db()
                try:
                    edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

                    if not edge:
                        # 新边缘自动注册
                        edge = Edge(
                            edge_id=edge_id,
                            edge_type=edge_type,
                            edge_key='',
                            local_ip=local_ip,
                            public_ip=public_ip,
                            status=EDGE_STATUS_ONLINE,
                            confirmed=99,
                            ingress_id=self.ingress_id,
                            last_online_time=now,
                            last_offline_time=now
                        )
                        db.add(edge)
                        logger.info(f"New edge {edge_id} auto-registered: status=online, confirmed=99 (pending)")
                    else:
                        # 已存在边缘，更新状态
                        edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
                        if edge and edge.confirmed != 0:  # 非黑名单边缘才更新
                            edge.status = EDGE_STATUS_ONLINE
                            edge.last_online_time = now
                            edge.last_offline_time = now
                            edge.ingress_id = self.ingress_id
                            logger.info(f"Updated edge {edge_id} status to online (confirmed={edge.confirmed})")

                    db.commit()
                finally:
                    db.close()

        except Exception as e:
            logger.error(f"Error updating edge online for {edge_id}: {type(e).__name__}: {e}", exc_info=True)

    async def _handle_edge_offline_request(self, req_id: int, edge_id: str):
        """Handle edge.offline request from ingress (with confirmation response).

        更新数据库中的边缘状态为离线，并发送确认响应给 Ingress。
        使用数据库锁保护并发更新。
        """
        try:
            from database import get_db, get_db_lock, Edge, EDGE_STATUS_OFFLINE

            # 使用数据库锁保护并发更新
            with get_db_lock():
                db = get_db()
                try:
                    edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

                    if edge:
                        edge.status = EDGE_STATUS_OFFLINE
                        edge.last_offline_time = datetime.utcnow()
                        edge.updated_at = datetime.utcnow()
                        db.commit()
                        logger.info(f"Updated edge {edge_id} status to offline")
                    else:
                        logger.warning(f"Edge {edge_id} not found in database")
                finally:
                    db.close()

            # 发送确认响应给 Ingress
            response = {
                "jsonrpc": "2.0",
                "result": {
                    "edge_id": edge_id,
                    "success": True,
                    "message": "Edge offline confirmed"
                },
                "id": req_id
            }
            await self.send_message(response)
            logger.info(f"Sent edge.offline confirmation response to ingress {self.ingress_id} (req_id={req_id})")

        except Exception as e:
            logger.error(f"Error handling edge offline request for {edge_id}: {type(e).__name__}: {e}", exc_info=True)
            # 发送错误响应
            error_response = {
                "jsonrpc": "2.0",
                "error": {"code": -32603, "message": f"Internal error: {str(e)}"},
                "id": req_id
            }
            await self.send_message(error_response)

    async def _notify_edge_offline(self, edge_id: str):
        """Notify manager that a edge is offline (called by ingress).

        直接更新数据库，不通过 HTTP API。
        使用数据库锁保护并发更新。
        """
        try:
            from database import get_db, get_db_lock, Edge, EDGE_STATUS_OFFLINE

            # 使用数据库锁保护并发更新
            with get_db_lock():
                db = get_db()
                try:
                    edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

                    if edge:
                        edge.status = EDGE_STATUS_OFFLINE
                        edge.last_offline_time = datetime.utcnow()
                        edge.updated_at = datetime.utcnow()
                        db.commit()
                        logger.info(f"Updated edge {edge_id} status to offline")
                finally:
                    db.close()

        except Exception as e:
            logger.error(f"Error updating edge offline for {edge_id}: {type(e).__name__}: {e}", exc_info=True)

    async def _notify_ingress_offline(self):
        """Notify manager that an ingress is offline, update all its edges to offline.

        当 ingress 离线时，该 ingress 上的所有边缘都应该被标记为离线。
        直接更新数据库，不通过 HTTP API。
        使用数据库锁保护并发更新。
        """
        logger.info(f"Notifying manager that ingress {self.ingress_id} is offline, updating all edges")
        try:
            from database import get_db, get_db_lock, Edge, EDGE_STATUS_OFFLINE, EDGE_STATUS_ONLINE

            # 使用数据库锁保护并发更新
            with get_db_lock():
                db = get_db()
                try:
                    # 批量更新该 ingress 上的所有在线边缘为离线
                    edges = db.query(Edge).filter(
                        Edge.ingress_id == self.ingress_id,
                        Edge.status == EDGE_STATUS_ONLINE
                    ).all()

                    count = 0
                    now = datetime.utcnow()
                    for edge in edges:
                        edge.status = EDGE_STATUS_OFFLINE
                        edge.last_offline_time = now
                        edge.updated_at = now
                        count += 1

                    db.commit()
                    logger.info(f"Updated {count} edge(s) on ingress {self.ingress_id} to offline (ingress offline)")
                finally:
                    db.close()

        except Exception as e:
            logger.error(f"Error updating ingress offline for {self.ingress_id}: {type(e).__name__}: {e}", exc_info=True)

    async def send_message(self, message: Dict) -> bool:
        """Send a message to the ingress."""
        if not self.connected or not self.websocket:
            logger.warning(f"Cannot send message to ingress {self.ingress_id}: not connected")
            return False

        try:
            await self.websocket.send(json.dumps(message))
            return True
        except Exception as e:
            logger.error(f"Error sending message to ingress {self.ingress_id}: {e}")
            self.connected = False
            return False

    async def _close(self):
        """Close the WebSocket connection."""
        if self.websocket:
            try:
                if not self.websocket.closed:
                    await self.websocket.close()
            except:
                pass
        self.connected = False

    def is_connected(self) -> bool:
        """Check if connected to ingress."""
        # 实时检查连接状态：不仅检查标志，还检查 websocket 对象是否有效
        if not self.connected:
            return False

        # 如果 websocket 对象存在，检查其实际状态
        if self.websocket:
            try:
                # websockets 16.0+ 使用 closed 属性
                if hasattr(self.websocket, 'closed') and self.websocket.closed:
                    self.connected = False
                    return False
            except:
                # 如果检查失败，认为连接已断开
                self.connected = False
                return False

        return self.connected

    def query_edge_list(self, offset: int = 0, limit: int = None) -> Optional[Dict]:
        """Query edge list from ingress (synchronous wrapper for async method).

        Args:
            offset: Starting offset for pagination
            limit: Maximum number of edges to return (uses Config.EDGE_LIST_BATCH_SIZE if None)

        Returns:
            Dict with edge list response, or None if query failed
        """
        if limit is None:
            limit = Config.EDGE_LIST_BATCH_SIZE

        if not self.connected or not self._loop:
            logger.warning(f"Cannot query edge list from ingress {self.ingress_id}: not connected")
            return None

        try:
            # 在事件循环中运行异步查询
            future = asyncio.run_coroutine_threadsafe(
                self._query_edge_list_async(offset, limit),
                self._loop
            )
            result = future.result(timeout=Config.EDGE_LIST_QUERY_TIMEOUT)
            return result
        except asyncio.TimeoutError:
            logger.error(f"Edge list query timeout for ingress {self.ingress_id}")
            return None
        except Exception as e:
            logger.error(f"Error querying edge list from ingress {self.ingress_id}: {e}", exc_info=True)
            return None

    async def _query_edge_list_async(self, offset: int, limit: int) -> Optional[Dict]:
        """Query edge list from ingress (async method).

        Args:
            offset: Starting offset for pagination
            limit: Maximum number of edges to return

        Returns:
            Dict with edge list response, or None if query failed
        """
        if not self.connected or not self.websocket:
            logger.warning(f"Cannot query edge list from ingress {self.ingress_id}: not connected")
            return None

        # 生成查询 ID
        self._query_id_counter += 1
        query_id = f"{self.ingress_id}_{self._query_id_counter}"

        # 创建 Future 对象用于接收响应
        future = asyncio.Future()
        self._edge_list_futures[query_id] = future

        try:
            # 阶段 2：发送 JSON-RPC 2.0 请求：edge.list
            # 生成请求 ID（使用查询 ID 的哈希值作为数字 ID）
            import hashlib
            req_id = int(hashlib.md5(query_id.encode()).hexdigest()[:8], 16) % (2**31)

            # 维护 req_id 到 query_id 的映射
            if not hasattr(self, '_req_id_to_query_id'):
                self._req_id_to_query_id = {}
            self._req_id_to_query_id[req_id] = query_id

            query_msg = {
                "jsonrpc": "2.0",
                "method": MANAGER_EDGE_LIST,
                "params": {
                    "offset": offset,
                    "limit": limit
                },
                "id": req_id
            }

            if not await self.send_message(query_msg):
                self._edge_list_futures.pop(query_id, None)
                self._req_id_to_query_id.pop(req_id, None)
                return None

            # 等待响应（带超时）
            try:
                response = await asyncio.wait_for(future, timeout=Config.EDGE_LIST_QUERY_TIMEOUT)
                return response
            except asyncio.TimeoutError:
                logger.warning(f"Edge list query timeout for ingress {self.ingress_id}, query_id={query_id}")
                self._edge_list_futures.pop(query_id, None)
                return None
        except Exception as e:
            logger.error(f"Error querying edge list from ingress {self.ingress_id}: {e}", exc_info=True)
            self._edge_list_futures.pop(query_id, None)
            return None

