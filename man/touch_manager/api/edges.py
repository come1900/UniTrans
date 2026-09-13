#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Edge management API endpoints (v0.1 MVP - device registration and online).
"""

import logging
import json
import time
import requests
from datetime import datetime
from flask import request, jsonify, current_app

from api import api_bp
from database import (
    get_db, get_db_lock, Edge, Ingress, EdgeConfig,
    EDGE_STATUS_OFFLINE, EDGE_STATUS_ONLINE
)
from config import Config

logger = logging.getLogger(__name__)

@api_bp.route('/edges/register', methods=['POST'])
def register_edge():
    """Edge registration endpoint (v0.1 MVP - simplified)."""
    try:
        data = request.get_json()
        if not data:
            return jsonify({'code': 40006, 'message': 'Invalid request body'}), 400

        edge_id = data.get('edge_id')
        edge_type = data.get('edge_type')
        edge_key = data.get('edge_key')
        local_ip = data.get('local_ip', '')

        # Validate required fields
        if not edge_id or not edge_type or not edge_key:
            return jsonify({'code': 40006, 'message': 'Missing required fields'}), 400

        # Get client IP (public IP)
        edge_public_ip = request.remote_addr
        if request.headers.get('X-Forwarded-For'):
            edge_public_ip = request.headers.get('X-Forwarded-For').split(',')[0].strip()

        db = get_db()

        try:
            # Get or create edge
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
            if edge:
                # Update edge info
                edge.edge_type = edge_type
                device.local_ip = local_ip
                device.public_ip = edge_public_ip
                edge.updated_at = datetime.utcnow()
            else:
                # Create new edge
                edge = Edge(
                    edge_id=edge_id,
                    edge_type=edge_type,
                    edge_key=edge_key,
                    local_ip=local_ip,
                    public_ip=edge_public_ip,
                    status=EDGE_STATUS_OFFLINE
                )
                db.add(edge)

            # Simple load balancing: select first enabled ingress
            ingress = db.query(Ingress).filter(Ingress.enabled == True).first()

            if not ingress:
                return jsonify({'code': 50003, 'message': 'No available ingress'}), 503

            # Update edge.s ingress
            edge.ingress_id = ingress.ingress_id
            edge.updated_at = datetime.utcnow()

            # Generate simple token (v0.1 MVP - just use edge_id for now)
            token = edge_id  # Simplified token for v0.1

            db.commit()

            # Return response
            return jsonify({
                'code': 0,
                'message': 'success',
                'ingress': {
                    'host': ingress.host,
                    'port': ingress.port
                },
                'token': token,
                'edge_public_ip': edge_public_ip
            })

        except Exception as e:
            db.rollback()
            logger.error(f"Database error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Registration error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/list', methods=['POST'])
def list_edges():
    """List edges (v0.1 MVP) - from database.

    边缘列表默认从数据库读取；与 ingress 的全量同步通过单独的"手动同步"接口完成。
    所有时间戳使用 UTC ISO 8601 格式，浏览器会自动转换为本地时区显示。

    注意：link_duration（链接时长）是动态计算的瞬时数据，不在数据库存储。
    数据库只存储 last_online_time（上线时间），API 根据当前时间动态计算链接时长。
    这样设计避免数据库频繁写入，提高效率。

    优化：减少数据库锁持有时间，提高并发性能。
    """
    try:
        # 获取请求参数（从 JSON 消息体）
        data = request.get_json() or {}
        page = data.get('page', 1)
        page_size = data.get('page_size', 20)  # 默认 20
        edge_type = data.get('edge_type', '')
        status = data.get('status', '')
        keyword = data.get('keyword', '')

        db = get_db()

        try:
            # 构建查询
            query = db.query(Edge)
            
            # 应用过滤条件
            if edge_type:
                query = query.filter(Edge.edge_type == edge_type)
            if status:
                status_val = 1 if status == 'online' else 0
                query = query.filter(Edge.status == status_val)
            if keyword:
                query = query.filter(
                    (Edge.edge_id.contains(keyword)) | 
                    (Edge.local_ip.contains(keyword)) | 
                    (Edge.public_ip.contains(keyword))
                )
            
            # 获取总数
            total = query.count()
            
            # 数据库级别分页
            offset = (page - 1) * page_size
            edges = query.offset(offset).limit(page_size).all()
            
            # 查询全局统计数据（不受过滤条件影响）
            total_online = db.query(Edge).filter(Edge.status == EDGE_STATUS_ONLINE).count()
            total_confirmed = db.query(Edge).filter(Edge.confirmed == 1).count()
            total_rejected = db.query(Edge).filter(Edge.confirmed == 0).count()
        finally:
            db.close()

        # 在锁外计算在线时长和链接时长，避免阻塞其他数据库操作
        edge_list = []
        now = datetime.utcnow()
        for edge in edges:
            # 动态计算链接时长（秒）= 当前连接持续时间
            link_duration = 0
            if edge.status == EDGE_STATUS_ONLINE and edge.last_online_time:
                link_duration = int((now - edge.last_online_time).total_seconds())

            edge_list.append({
                'edge_id': edge.edge_id,
                'edge_type': edge.edge_type,
                'local_ip': edge.local_ip,
                'public_ip': edge.public_ip,
                'status': int(edge.status),  # 整数：0=离线，1=在线
                'confirmed': edge.confirmed,  # 99=pending, 1=confirmed, 0=rejected
                'ingress_id': edge.ingress_id,
                'last_online_time': edge.last_online_time.isoformat() + 'Z' if edge.last_online_time else None,
                'last_offline_time': edge.last_offline_time.isoformat() + 'Z' if edge.last_offline_time else None,
                'link_duration': link_duration,  # 秒（动态计算，当前连接持续时间）
                'created_at': edge.created_at.isoformat() + 'Z' if edge.created_at else None
            })

        return jsonify({
            'code': 0,
            'message': 'success',
            'total': total,
            'page': page,
            'page_size': page_size,
            'edges': edge_list,
            'stats': {
                'total_online': total_online,
                'total_confirmed': total_confirmed,
                'total_rejected': total_rejected
            }
        })

    except Exception as e:
        logger.error(f"List edges error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/get', methods=['POST'])
def get_edge():
    """Get edge details (v0.1 MVP)."""
    try:
        data = request.get_json() or {}
        edge_id = data.get('edge_id', '')
        
        db = get_db()
        try:
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
        finally:
            db.close()

        if not edge:
            return jsonify({'code': 40002, 'message': 'Edge not found'}), 404

        now = datetime.utcnow()
        link_duration = 0
        if edge.status == EDGE_STATUS_ONLINE and edge.last_online_time:
            link_duration = int((now - edge.last_online_time).total_seconds())

        return jsonify({
            'code': 0,
            'message': 'success',
            'edge_id': edge.edge_id,
            'edge_type': edge.edge_type,
            'local_ip': edge.local_ip,
            'public_ip': edge.public_ip,
            'status': int(edge.status),
            'confirmed': edge.confirmed,
            'ingress_id': edge.ingress_id,
            'last_online_time': edge.last_online_time.isoformat() + 'Z' if edge.last_online_time else None,
            'last_offline_time': edge.last_offline_time.isoformat() + 'Z' if edge.last_offline_time else None,
            'link_duration': link_duration,
            'created_at': edge.created_at.isoformat() + 'Z' if edge.created_at else None
        })

    except Exception as e:
        logger.error(f"Get edge error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/confirm', methods=['POST'])
def confirm_edge():
    """Confirm a pending edge (v0.1 MVP)."""
    try:
        data = request.get_json() or {}
        edge_id = data.get('edge_id', '')
        
        db = get_db()
        try:
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

            if not edge:
                return jsonify({'code': 40002, 'message': 'Edge not found'}), 404

            edge.confirmed = 1  # 已确认 (白名单)
            edge.updated_at = datetime.utcnow()
            db.commit()

            logger.info(f"Edge {edge_id} confirmed (confirmed=1, whitelist), status unchanged (status={edge.status})")
            return jsonify({'code': 0, 'message': 'success'})

        except Exception as e:
            db.rollback()
            logger.error(f"Confirm edge error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Confirm edge error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/reject', methods=['POST'])
def reject_edge():
    """Reject a pending edge (v0.1 MVP)."""
    try:
        data = request.get_json() or {}
        edge_id = data.get('edge_id', '')
        
        db = get_db()
        try:
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

            if not edge:
                return jsonify({'code': 40002, 'message': 'Edge not found'}), 404

            edge.confirmed = 0  # 黑名单 (拒绝)
            edge.updated_at = datetime.utcnow()
            db.commit()

            from ingress_manager import get_ingress_manager
            ingress_manager = get_ingress_manager()
            ingress_manager.kick_device(edge_id)
            logger.info(f"Edge {edge_id} rejected (confirmed=0), kick command sent to ingress")

            return jsonify({'code': 0, 'message': 'success'})

        except Exception as e:
            db.rollback()
            logger.error(f"Reject edge error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Reject edge error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/delete', methods=['POST'])
def delete_edge():
    """Delete an edge from manager (v0.1 MVP)."""
    try:
        data = request.get_json() or {}
        edge_id = data.get('edge_id', '')
        
        db = get_db()
        try:
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()

            if not edge:
                return jsonify({'code': 40002, 'message': 'Edge not found'}), 404

            # If edge is online, kick it first
            if edge.status == EDGE_STATUS_ONLINE:
                from ingress_manager import get_ingress_manager
                ingress_manager = get_ingress_manager()
                ingress_manager.kick_device(edge_id)
                logger.debug(f"Edge {edge_id} delete: kick command sent to ingress")

            # Delete from database
            db.delete(edge)
            db.commit()

            logger.info(f"Edge {edge_id} deleted from manager")
            return jsonify({'code': 0, 'message': 'success'})

        except Exception as e:
            db.rollback()
            logger.error(f"Delete edge error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Delete edge error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/batch-confirm', methods=['POST'])
def batch_confirm_edges():
    """Batch confirm multiple edges (v0.1 MVP).
    
    Note: Uses database lock to protect concurrent updates.
    """
    try:
        data = request.get_json()
        if not data:
            return jsonify({'code': 40006, 'message': 'Invalid request body'}), 400

        edge_ids = data.get('edge_ids', [])
        if not edge_ids:
            return jsonify({'code': 40006, 'message': 'No edge IDs provided'}), 400

        # 使用数据库锁保护并发更新
        from database import get_db_lock
        with get_db_lock():
            db = get_db()
            try:
                confirmed_count = 0
                for edge_id in edge_ids:
                    edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
                    if edge:
                        edge.confirmed = 1  # 已确认 (白名单)
                        # 注意：不修改 status，边缘在线状态由 ingress 上报的上线/下线消息决定
                        edge.updated_at = datetime.utcnow()
                        confirmed_count += 1

                db.commit()

                logger.info(f"Batch confirmed {confirmed_count} edge(s)")
                return jsonify({
                    'code': 0,
                    'message': 'success',
                    'confirmed_count': confirmed_count
                })

            finally:
                db.close()

    except Exception as e:
        logger.error(f"Batch confirm error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/edges/batch-reject', methods=['POST'])
def batch_reject_edges():
    """Batch reject multiple edges (v0.1 MVP).

    Rejected edges will be marked as confirmed=0 (blacklist).
    When a rejected device tries to come online, it will be denied.

    Note: Do NOT set status=offline here. Wait for ingress to report device.offline.
    Note: Uses database lock to protect concurrent updates.
    Note: Kick commands are sent asynchronously to avoid blocking.
    """
    try:
        data = request.get_json()
        if not data:
            return jsonify({'code': 40006, 'message': 'Invalid request body'}), 400

        edge_ids = data.get('edge_ids', [])
        if not edge_ids:
            return jsonify({'code': 40006, 'message': 'No edge IDs provided'}), 400

        # 使用数据库锁保护并发更新
        from database import get_db_lock
        with get_db_lock():
            db = get_db()
            try:
                rejected_count = 0
                edges_to_kick = []  # 记录需要踢的边缘

                for edge_id in edge_ids:
                    edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
                    if edge:
                        edge.confirmed = 0  # 黑名单
                        edge.updated_at = datetime.utcnow()
                        rejected_count += 1
                        
                        # 如果边缘在线，记录需要踢
                        if edge.status == EDGE_STATUS_ONLINE:
                            edges_to_kick.append(edge_id)

                db.commit()
                logger.info(f"Batch rejected {rejected_count} edge(s) (confirmed=0)")

            finally:
                db.close()

        # 在锁外异步发送 kick 命令，避免阻塞
        if edges_to_kick:
            from ingress_manager import get_ingress_manager
            ingress_manager = get_ingress_manager()
            
            for edge_id in edges_to_kick:
                # 异步发送 kick 命令，不等待结果
                ingress_manager.kick_device(edge_id)

            logger.info(f"Batch rejected {rejected_count} edge(s) (confirmed=0, kick commands sent)")
            
        return jsonify({
            'code': 0,
            'message': 'success',
            'rejected_count': rejected_count
        })

    except Exception as e:
        logger.error(f"Batch reject error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500


# ==============================
# frpc 远程配置 API（第一阶段实现）
# ==============================

def _canonical_config(cfg):
    """将任意结构配置统一为 frpc 嵌套权威结构（与 edge 实际运行文件/edge.config.query 回读一致）。

    支持两类输入：
      - 扁平结构: {serverAddr, serverPort, authMethod, token, tlsEnable,
                   webServerAddr, webServerPort, proxies[{name,type,localIP,localPort,remotePort,description}]}
      - frpc 嵌套结构: {serverAddr, serverPort, auth:{method,token}, transport:{tls:{enable}},
                        webServer:{addr,port}, proxies[...]}
    统一输出 frpc 嵌套结构，DB 与 GET 返回以此为准，消除 schema 漂移。
    """
    if not isinstance(cfg, dict):
        return {}

    auth = cfg.get('auth')
    if isinstance(auth, dict):
        auth_method = auth.get('method', 'token')
        token = auth.get('token', '')
    else:
        auth_method = cfg.get('authMethod', 'token')
        token = cfg.get('token', '')

    transport = cfg.get('transport')
    if isinstance(transport, dict) and isinstance(transport.get('tls'), dict):
        tls_enable = transport['tls'].get('enable', False)
    else:
        tls_enable = cfg.get('tlsEnable', False)

    web_server = cfg.get('webServer')
    if isinstance(web_server, dict):
        ws_addr = web_server.get('addr', '127.0.0.1')
        ws_port = web_server.get('port', 0)
    else:
        ws_addr = cfg.get('webServerAddr', '127.0.0.1')
        ws_port = cfg.get('webServerPort', 0)

    proxies = []
    for p in cfg.get('proxies') or []:
        if not isinstance(p, dict):
            continue
        entry = {
            'name': p.get('name', ''),
            'type': p.get('type', 'tcp'),
            'localIP': p.get('localIP', '127.0.0.1'),
            'localPort': p.get('localPort', 0),
            'remotePort': p.get('remotePort', 0),
        }
        if p.get('description'):
            entry['description'] = p['description']
        proxies.append(entry)

    return {
        'serverAddr': cfg.get('serverAddr', ''),
        'serverPort': cfg.get('serverPort', 0),
        'auth': {'method': auth_method, 'token': token},
        'transport': {'tls': {'enable': tls_enable}},
        'webServer': {'addr': ws_addr, 'port': ws_port},
        'proxies': proxies,
    }


def _set_edge_config_status(edge_id, status, version=None):
    """短事务更新 edge 配置状态。

    在进程内互斥锁内获取/释放连接，立即归还连接池，
    避免在并发推送下长时间占用 SQLAlchemy QueuePool 连接导致耗尽超时。
    """
    with get_db_lock():
        db = get_db()
        try:
            db.expire_all()
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
            if not edge:
                return
            edge.config_status = status
            if version is not None:
                edge.config_version = version
                ec = db.query(EdgeConfig).filter(EdgeConfig.edge_id == edge_id).first()
                if ec:
                    ec.version = version
            edge.updated_at = datetime.utcnow()
            db.commit()
        except Exception as e:
            db.rollback()
            logger.error(f"set_edge_config_status error: {e}")
        finally:
            db.close()


@api_bp.route('/edges/config/update', methods=['POST'])
def update_edge_config():
    """Update frpc config for an edge (v0.2 - frpc remote config).

    保存配置到数据库，并下发给在线的 Edge。
    如果 Edge 离线，配置暂存数据库，待 Edge 上线后下发。
    """
    try:
        data = request.get_json() or {}

        # 0. 从 body 提取并校验 edge_id（遵循全系统约定：边缘 ID 放 body，不放 URL 路径）
        edge_id = data.get('edge_id', '')
        if not edge_id:
            return jsonify({'code': 40001, 'message': "Missing 'edge_id' field"}), 400

        # 1. 验证 edge 是否存在
        db = get_db()
        try:
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
            if not edge:
                return jsonify({'code': 40002, 'message': 'Edge not found'}), 404

            # 2. 获取/创建 edge_config 记录
            edge_config = db.query(EdgeConfig).filter(EdgeConfig.edge_id == edge_id).first()
            if not edge_config:
                edge_config = EdgeConfig(edge_id=edge_id)
                db.add(edge_config)

            # 3. 验证输入格式并提取配置数据
            # 检查 config_type 字段
            config_type = data.get('config_type')
            if not config_type:
                logger.error(f"Edge {edge_id} config: Missing 'config_type' field")
                return jsonify({
                    'code': 40001,
                    'message': "Missing 'config_type' field, expected 'tunnelService'"
                }), 400
            
            if config_type != 'tunnelService':
                logger.error(f"Edge {edge_id} config: Unsupported config_type '{config_type}', expected 'tunnelService'")
                return jsonify({
                    'code': 40001,
                    'message': f"Unsupported config_type '{config_type}', expected 'tunnelService'"
                }), 400
            
            # 检查 config_content 字段
            config_content = data.get('config_content')
            if not config_content:
                logger.error(f"Edge {edge_id} config: Missing 'config_content' field")
                return jsonify({
                    'code': 40001,
                    'message': "Missing 'config_content' field"
                }), 400
            
            # 检查 services 数组
            services = config_content.get('services', [])
            if not services or not isinstance(services, list) or len(services) == 0:
                logger.error(f"Edge {edge_id} config: 'config_content.services' must be a non-empty array")
                return jsonify({
                    'code': 40001,
                    'message': "'config_content.services' must be a non-empty array"
                }), 400
            
            # 检查第一个服务的 tunnelService 字段
            first_service = services[0]
            tunnel_service = first_service.get('tunnelService')
            if not tunnel_service or not isinstance(tunnel_service, dict):
                logger.error(f"Edge {edge_id} config: Missing 'tunnelService' in first service")
                return jsonify({
                    'code': 40001,
                    'message': "Missing 'tunnelService' in first service"
                }), 400
            
            # 从 config_content.services[0].tunnelService 中提取参数
            endpoint = tunnel_service.get('endpoint', {})
            security = tunnel_service.get('security', {})
            local_management = tunnel_service.get('localManagement', {})
            
            config_data = {
                'serverAddr': endpoint.get('host', ''),
                'serverPort': endpoint.get('port', 0),
                'authMethod': security.get('authMethod', 'token'),
                'token': security.get('credential', ''),
                'tlsEnable': security.get('enableTls', False),
                'webServerAddr': local_management.get('bindAddress', '127.0.0.1'),
                'webServerPort': local_management.get('bindPort', 0),
            }

            # 从 config_content.services[0].accessPolicies 转换为 proxies（frpc 扁平结构）
            access_policies = first_service.get('accessPolicies', [])
            if not isinstance(access_policies, list):
                access_policies = []
            proxies = []
            for ap in access_policies:
                tgt = ap.get('targetService', {}) if isinstance(ap.get('targetService'), dict) else {}
                proxies.append({
                    'name': ap.get('policyId', ''),
                    'type': ap.get('protocol', 'tcp'),
                    'localIP': tgt.get('ip', '127.0.0.1'),
                    'localPort': tgt.get('port', 0),
                    'remotePort': ap.get('exposedPort', 0),
                    'description': ap.get('description', '')
                })
            config_data['proxies'] = proxies
            import json
            # 统一为 frpc 嵌套权威结构存储（消除扁平/嵌套 schema 漂移）
            new_config_json = json.dumps(
                _canonical_config(config_data), ensure_ascii=False, sort_keys=True)
            
            # 4-7. 版本号分配与 DB 持久化：在进程内互斥锁内串行完成读-改-写，
            #      保证并发推送下 version 严格唯一递增、content 不被互相覆盖。
            #      （单进程多线程前提下，应用锁即保证原子；网络发送放到锁外进行。）
            import time
            dedup_hit = False   # 内容与已确认配置相同 -> 不重新分配版本
            busy_version = None # 非 None 时表示节流拒绝返回
            new_version = None
            edge_online = False
            with get_db_lock():
                # 锁内串行：先刷新本文session缓存，读到其他线程最近提交的最新值
                db.expire_all()
                edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
                edge_config = db.query(EdgeConfig).filter(EdgeConfig.edge_id == edge_id).first()
                if not edge_config:
                    edge_config = EdgeConfig(edge_id=edge_id)
                    db.add(edge_config)

                if (edge.config_status in ('confirmed', 'configuring')
                        and edge_config.config_json == new_config_json):
                    # 内容与数据库现有配置相同
                    if edge.config_status == 'confirmed':
                        logger.info(f"Edge {edge_id} config: Config already confirmed and unchanged, skipping update")
                        dedup_hit = True
                        new_version = edge_config.version
                    else:
                        # 仍在配置中 -> 按可配置的确认超时阈值判断允许重试或返回 busy
                        #  (Config.CONFIG_CONFIRM_TIMEOUT: 1~3600 秒生效, 0=每次都直接重推)
                        cfg_timeout = Config.CONFIG_CONFIRM_TIMEOUT
                        configuring_since = edge_config.configuring_since
                        elapsed_sec = (int(time.time() * 1000) - configuring_since) / 1000.0 if configuring_since else 0.0
                        timeout_ok = cfg_timeout == 0 or (configuring_since and elapsed_sec > cfg_timeout)
                        if timeout_ok:
                            logger.info(f"Edge {edge_id} config: Config update timeout ({elapsed_sec:.1f}s > {cfg_timeout}s), allowing retry")
                        else:
                            logger.info(f"Edge {edge_id} config: Config update already in progress, returning busy")
                            busy_version = edge_config.version
                else:
                    # 内容不同 -> 锁内串行分配新版本（绝不复用）
                    current_version = edge_config.version or 0
                    new_version = current_version + 1
                    edge_config.version = new_version
                    edge_config.config_json = new_config_json
                    edge_config.updated_at = datetime.utcnow()
                    edge_config.configuring_since = int(time.time() * 1000)

                    edge.config_status = 'configuring'
                    edge.config_version = new_version
                    edge.updated_at = datetime.utcnow()
                    db.commit()
                    edge_online = (edge.status == EDGE_STATUS_ONLINE)

            if busy_version is not None:
                return jsonify({
                    'code': 50008,
                    'message': 'Config update in progress, please try later',
                    'config_version': busy_version
                }), 503

            if dedup_hit:
                return jsonify({
                    'code': 0,
                    'message': 'Config already confirmed and unchanged',
                    'config_version': new_version
                })

            if not edge_online:
                # Edge 离线，配置暂存数据库
                edge.config_status = 'pending'
                edge.config_version = new_version
                edge.updated_at = datetime.utcnow()
                db.commit()
                logger.info(f"Edge {edge_id} is offline, config saved to database (pending)")
                return jsonify({
                    'code': 0,
                    'message': 'Config saved (edge offline, will be applied when online)',
                    'config_version': new_version
                })

            # 与 DB 最新状态同步（后续步骤仅当需要引用 edge/edge_config 属性时才访问）
            db.expire_all()
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
            edge_config = db.query(EdgeConfig).filter(EdgeConfig.edge_id == edge_id).first()

            # 8. 获取 edge 当前连接的 ingress client
            # 默认 ingress（由 config.py 管理，不在数据库）与数据库管理的 ingress 都通过
            # ingress_manager 按 edge.ingress_id 直接查找连接，避免默认接入查询不到 Ingress 记录
            from ingress_manager import get_ingress_manager
            ingress_manager = get_ingress_manager()
            ingress_client = ingress_manager.get_client(edge.ingress_id)
            if not ingress_client or not ingress_client.is_connected():
                edge.config_status = 'config_failed'
                db.commit()
                return jsonify({'code': 50004, 'message': 'Ingress not connected'}), 503

            # 8. 构造 ConfigUpdate_tunnelService 格式的消息（符合 come.1 库的编解码格式）
            # 注意：edge_id、config_type、version、access_token、config_content 都在顶层，不在 params 中
            import json
            logger.info(f"Edge {edge_id} config: Building ConfigUpdate_tunnelService message")
            logger.info(f"  - config_data: {json.dumps(config_data, ensure_ascii=False)}")

            # 将 proxies 转换为 accessPolicies
            proxies_list = config_data.get('proxies', [])
            logger.info(f"  - Converting {len(proxies_list)} proxies to accessPolicies")
            
            access_policies = []
            for idx, proxy in enumerate(proxies_list):
                access_policy = {
                    'policyId': proxy.get('name', ''),
                    'protocol': proxy.get('type', 'tcp'),
                    'targetService': {
                        'ip': proxy.get('localIP', '127.0.0.1'),
                        'port': proxy.get('localPort', 0)
                    },
                    'exposedPort': proxy.get('remotePort', 0),
                    'description': proxy.get('description', '')
                }
                logger.info(f"    - proxy[{idx}]: {json.dumps(proxy, ensure_ascii=False)} -> accessPolicy: {json.dumps(access_policy, ensure_ascii=False)}")
                access_policies.append(access_policy)

            config_msg = {
                'jsonrpc': '2.0',
                'method': 'edge.config.update',
                'edge_id': edge_id,  # 顶层字段！
                'config_type': 'tunnelService',
                'version': new_version,  # 使用锁内原子分配的唯一版本号（避免 ORM 对象被并发刷新读到过期值）
                'access_token': '',  # TODO: 从注册时获取
                'config_content': [  # 注意：是 config_content 不是 configContent
                    {
                        'serviceName': 'tunnelService',
                        'tunnelService': {
                            'version': '1.0',
                            'endpoint': {
                                'host': config_data.get('serverAddr', ''),
                                'port': config_data.get('serverPort', 0)
                            },
                            'localManagement': {
                                'bindAddress': config_data.get('webServerAddr', '127.0.0.1'),
                                'bindPort': config_data.get('webServerPort', 0)
                            },
                            'security': {
                                'authMethod': config_data.get('authMethod', 'token'),
                                'credential': config_data.get('token', ''),
                                'enableTls': config_data.get('tlsEnable', False)
                            }
                        },
                        'accessPolicies': access_policies  # accessPolicies 与 tunnelService 同级！
                    }
                ],
                'id': ingress_client.generate_request_id()  # 添加 id 字段
            }

            logger.info(f"  - Final config_msg: {json.dumps(config_msg, ensure_ascii=False)}")
            
            # 9. 异步发送配置消息（不等待响应）
            # Manager 只负责转发指令给 Ingress，Ingress 和 Edge 的处理都是异步的
            # 使用线程安全的方式发送消息
            ingress_client = ingress_manager.get_client(edge.ingress_id)
            if not ingress_client or not ingress_client.is_connected():
                edge.config_status = 'config_failed'
                db.commit()
                return jsonify({'code': 50004, 'message': 'Ingress not connected'}), 503

            try:
                # 使用线程安全的方式发送消息（不等待响应）
                sent = ingress_client.send_message_threadsafe(config_msg)
                
                if not sent:
                    logger.error(f"Failed to send config message to ingress {self.ingress_id}")
                    edge.config_status = 'config_failed'
                    db.commit()
                    return jsonify({
                        'code': 50007,
                        'message': 'Failed to send config message to ingress'
                    }), 500
            except Exception as e:
                logger.error(f"Error sending config message: {e}")
                edge.config_status = 'config_failed'
                db.commit()
                return jsonify({
                    'code': 50007,
                    'message': f'Error sending config message: {str(e)}'
                }), 500

            # 10. 指令已发送，返回成功（异步处理，不等 Edge 响应）
            edge.config_status = 'configuring'  # 配置指令已发送，等待 Edge 反馈
            edge.config_version = new_version
            db.commit()
            logger.info(f"Edge {edge_id} config message sent to ingress (version={new_version}, async wait for edge ack)")
            return jsonify({
                'code': 0,
                'message': 'Config message sent, waiting for edge confirmation',
                'config_version': new_version
            })
            
        except Exception as e:
            db.rollback()
            logger.error(f"Database error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()
            
    except Exception as e:
        logger.error(f"Update edge config error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500


def _default_edge_config():
    """返回后端定义的空模板默认 frpc 配置。

    当数据库中没有任何配置(从未保存/下发过)且 Edge 不可在线回读时使用，
    保证 `config/get` 总能返回一套可编辑的初始配置(frpc 嵌套权威结构)。
    默认值在后端统一管理，前端不再硬编码空值。
    """
    return {
        'serverAddr': '127.0.0.1',
        'serverPort': 17400,
        'auth': {'method': 'token', 'token': ''},
        'transport': {'tls': {'enable': True}},
        'webServer': {'addr': '127.0.0.1', 'port': 17401},
        'proxies': []
    }


@api_bp.route('/edges/config/get', methods=['POST'])
def get_edge_config():
    """Get frpc config for an edge (v0.2 - frpc remote config).

    返回优先级：数据库配置 -> (Edge 在线时) edge 前端配置回填 -> 后端默认配置。
    保证接口总能返回一套可用的配置(非空)，供前端"加载配置"直接回填表单。
    如果数据库中没有配置且 Edge 离线，返回后端默认配置。
    """
    try:
        # 0. 从 body 提取并校验 edge_id（遵循全系统约定：边缘 ID 放 body，不放 URL 路径）
        data = request.get_json() or {}
        edge_id = data.get('edge_id', '')
        if not edge_id:
            return jsonify({'code': 40001, 'message': "Missing 'edge_id' field"}), 400

        db = get_db()
        try:
            # 1. 验证 edge 是否存在
            edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
            if not edge:
                return jsonify({'code': 40002, 'message': 'Edge not found'}), 404
            
            # 2. 查询数据库中的配置
            edge_config = db.query(EdgeConfig).filter(EdgeConfig.edge_id == edge_id).first()
            
            config_data = {}
            if edge_config and edge_config.config_json:
                import json
                config_data = json.loads(edge_config.config_json)
            
            # 3. 如果 Edge 在线，后台异步刷新配置
            # 通过 ingress_manager 按 edge.ingress_id 直接查找连接，兼容默认(config)与数据库管理的 ingress
            # config_matched_val: True=一致 / False=不一致 / None=无法核对(离线)
            # config_sync_val: 'matched' / 'mismatch' / 'missing' / None=无法核对
            config_matched_val = None
            config_sync_val = None
            final_status = edge.config_status
            if edge.status == EDGE_STATUS_ONLINE and edge.ingress_id:
                from ingress_manager import get_ingress_manager
                ingress_manager = get_ingress_manager()
                ingress_client = ingress_manager.get_client(edge.ingress_id)

                if ingress_client and ingress_client.is_connected():
                        # 以 edge 前端配置为准: 同步读一次 edge 本地真实配置。
                        # 使用线程安全方式避免跨事件循环挂起(旧实现 new_event_loop 回填延迟数秒~30s)。
                        # 区分两种不可读情况: missing(文件不存在/无法打开, 顶层 error)
                        #                     / bad(文件存在但解析失败, result.config_error)
                        import json
                        edge_online_cfg = None
                        edge_unreadable = False
                        edge_local_missing = False
                        edge_local_bad = False
                        response = ingress_client.query_edge_config(edge_id, timeout=10)
                        if isinstance(response, dict):
                            result = response.get('result') if 'result' in response else None
                            err = response.get('error') or (result.get('error') if isinstance(result, dict) else None)
                            if isinstance(result, dict) and 'config' in result:
                                edge_online_cfg = result['config']
                                # 若 DB 尚无配置, 以 edge 前端配置回填 DB & 返回
                                if not config_data:
                                    config_data = edge_online_cfg
                                    if not edge_config:
                                        edge_config = EdgeConfig(edge_id=edge_id)
                                        db.add(edge_config)
                                    edge_config.config_json = json.dumps(edge_online_cfg, ensure_ascii=False)
                                    edge_config.updated_at = datetime.utcnow()
                                    db.commit()
                                    logger.info(f"Edge {edge_id} config synced from device (db empty)")
                            elif isinstance(result, dict) and result.get('config_error'):
                                # edge 本地配置文件存在但解析失败
                                edge_local_bad = True
                                edge_unreadable = True
                                logger.warning(f"Edge {edge_id} config query error: "
                                               f"{result.get('config_error')}")
                            elif err is not None:
                                # edge 返回 JSON-RPC error (典型: 本地配置文件缺失/无法打开)
                                edge_local_missing = True
                                edge_unreadable = True
                                logger.warning(f"Edge {edge_id} config query error: {err}")

                        # 一致性核对(以 edge 前端为准): 归一化比较 manager(DB) 与 edge 本地配置。
                        # 区分: 一致 / 不一致 / edge 无可用配置(missing) / 配置格式错(bad)。
                        config_matched = None
                        if edge_online_cfg is not None:
                            db_canon = _canonical_config(config_data)
                            edge_canon = _canonical_config(edge_online_cfg)
                            if db_canon == edge_canon:
                                config_matched = True
                                config_sync_val = 'matched'
                                if edge.config_status != 'confirmed':
                                    _set_edge_config_status(edge_id, 'confirmed')
                                    edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
                            else:
                                config_matched = False
                                config_sync_val = 'mismatch'
                        elif edge_local_missing:
                            # edge 在线但本地配置文件不存在/无法打开
                            config_matched = False
                            config_sync_val = 'missing'
                        elif edge_local_bad:
                            # edge 在线但本地配置文件存在却无法解析
                            config_matched = False
                            config_sync_val = 'bad'
                        # 其它情况(查询超时/无响应): config_sync_val 保持 None, 不误判为缺失

                        config_matched_val = config_matched
                        final_status = 'confirmed' if (config_matched is True) else edge.config_status
                else:
                        config_matched_val = None
                        config_sync_val = None
                        final_status = edge.config_status
            
            # 4. 返回配置（保证非空）：数据库配置 -> (Edge 在线回填) -> 后端默认配置
            # 统一输出 frpc 嵌套权威结构，兼容历史扁平数据，消除 schema 漂移
            final_config = config_data if config_data else _default_edge_config()
            return jsonify({
                'code': 0,
                'message': 'success',
                'config': _canonical_config(final_config),
                'config_version': edge_config.version if edge_config else 0,
                'config_status': final_status,
                'config_matched': config_matched_val,
                'config_sync': config_sync_val
            })
            
        except Exception as e:
            db.rollback()
            logger.error(f"Database error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()
            
    except Exception as e:
        logger.error(f"Get edge config error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500


@api_bp.route('/edges/config/sync', methods=['POST'])
def sync_edge_config():
    """从 Edge 前端获取真实配置，并与数据库(manager)配置核对。

    语义：区别于 config/get(从数据库加载)。
    - config/get  = 加载配置   -> 以数据库为准返回
    - config/sync = 同步配置   -> 以 edge 前端为准，展示 edge 的真实配置
    返回：
      config       = edge 前端真实配置（frpc 权威结构）
      differ       = 与数据库配置是否不一致 (None=无法核对)
      sync_at      = 本次同步核对的 UTC 毫秒时间戳
      config_status/version 沿用数据库记录
    """
    try:
        data = request.get_json(silent=True) or {}
        edge_id = data.get('edge_id')
        if not edge_id:
            return jsonify({'code': 40006, 'message': 'edge_id is required'}), 400

        db = get_db()
        edge = db.query(Edge).filter(Edge.edge_id == edge_id).first()
        if not edge:
            db.close()
            return jsonify({'code': 40004, 'message': f'Edge {edge_id} not found'}), 404

        edge_config = db.query(EdgeConfig).filter(EdgeConfig.edge_id == edge_id).first()
        db_cfg_raw = None
        if edge_config and edge_config.config_json:
            try:
                db_cfg_raw = json.loads(edge_config.config_json)
            except Exception:
                db_cfg_raw = None

        sync_at_ms = int(time.time() * 1000)
        sync_at_dt = datetime.utcnow()

        # 从 edge 前端同步读取真实配置
        # 三种结果区分（供前端准确提示）：
        #   edge_reachable + 拿到 config  -> 正常，可比较 differ
        #   edge 在线但返回 error (文件缺失/解析失败) -> unreadable=True, reachable=True
        #   ingress 未连接/超时 -> reachable=False (离线或无法通信)
        edge_online_cfg = None
        edge_reachable = False
        edge_unreadable = False
        edge_local_missing = False      # 配置文件不存在/无法打开（顶层 error）
        edge_local_bad = False          # 文件存在但解析失败（result.config_error）
        if edge.status == EDGE_STATUS_ONLINE and edge.ingress_id:
            from ingress_manager import get_ingress_manager
            im = get_ingress_manager()
            ic = im.get_client(edge.ingress_id)
            if ic and ic.is_connected():
                resp = ic.query_edge_config(edge_id, timeout=10)
                if isinstance(resp, dict):
                    edge_reachable = True
                    result = resp.get('result') if 'result' in resp else None
                    err = resp.get('error') or (result.get('error') if isinstance(result, dict) else None)
                    if isinstance(result, dict) and 'config' in result:
                        edge_online_cfg = result['config']
                    elif isinstance(result, dict) and result.get('config_error'):
                        # 文件存在但 JSON 解析失败
                        edge_local_bad = True
                        edge_unreadable = True
                    elif err is not None:
                        # JSON-RPC 顶层 error -> 文件不存在/无法打开
                        edge_local_missing = True
                        edge_unreadable = True
                # resp=None 或非 dict -> 无法通信 (超时/连接异常)，不置 unreadable
        # else: edge 未在线或无 ingress -> reachable 保持 False (离线)

        # 一致性核对：edge 前端 vs 数据库
        differ = None
        if edge_online_cfg is not None:
            db_canon = _canonical_config(db_cfg_raw) if db_cfg_raw else None
            edge_canon = _canonical_config(edge_online_cfg)
            if db_canon is not None and edge_canon is not None:
                differ = (db_canon != edge_canon)
            else:
                differ = None
        if edge_reachable and edge_online_cfg is not None:
            sync_state = 'differ' if differ else 'same'
        elif edge_reachable and edge_local_missing:
            sync_state = 'missing'      # edge 在线但本地配置文件不存在/无法打开
        elif edge_reachable and edge_local_bad:
            sync_state = 'bad'          # edge 在线但本地配置解析失败
        elif edge_reachable:
            sync_state = 'unreadable'   # edge 在线但配置不可读（兜底）
        else:
            sync_state = 'unreachable'  # edge 离线或无法通信

        # 记录同步毫秒时间戳 (DB 与 edge 最近一次同步核对)
        if edge_config is None:
            edge_config = EdgeConfig(edge_id=edge_id)
            db.add(edge_config)
        edge_config.synced_at = sync_at_ms
        edge_config.updated_at = sync_at_dt
        # 仅当数据库无配置时才回填 edge 配置，避免用 edge 值覆盖用户已保存(manager 为准)的配置
        if edge_config.config_json is None:
            if edge_online_cfg is not None:
                edge_config.config_json = json.dumps(edge_online_cfg, ensure_ascii=False)
        db.commit()

        # 在 db.close() 之前取出所需字段，避免 detached 实例属性刷新报错
        cfg_status = edge.config_status if edge else 'unknown'
        cfg_version = edge_config.version if edge_config else 0
        db.close()
        return jsonify({
            'code': 0,
            'message': 'success',
            'config': _canonical_config(edge_online_cfg) if edge_online_cfg is not None else None,
            'config_status': cfg_status,
            'config_version': cfg_version,
            'differ': differ,
            'sync_state': sync_state,
            'sync_at': sync_at_ms,
            'edge_unreadable': edge_unreadable,
            'edge_reachable': edge_reachable,
            'edge_local_missing': edge_local_missing,
            'edge_local_bad': edge_local_bad
        })

    except Exception as e:
        logger.error(f"Sync edge config error: {e}")
        try:
            db.rollback()
            db.close()
        except Exception:
            pass
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500
