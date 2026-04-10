#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Edge management API endpoints (v0.1 MVP - device registration and online).
"""

import logging
import requests
from datetime import datetime
from flask import request, jsonify, current_app

from api import api_bp
from database import (
    get_db, Edge, Ingress,
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
