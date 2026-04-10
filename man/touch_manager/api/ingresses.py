#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Ingress management API endpoints (v0.1 MVP - only configuration management).
"""

import logging
from datetime import datetime
from flask import request, jsonify

from api import api_bp
from database import (
    get_db, Ingress, Edge,
    EDGE_STATUS_OFFLINE, EDGE_STATUS_ONLINE,
    INGRESS_STATUS_OFFLINE, INGRESS_STATUS_ONLINE
)
from ingress_manager import get_ingress_manager
from config import Config

logger = logging.getLogger(__name__)

@api_bp.route('/ingresses/list', methods=['POST'])
def list_ingresses():
    """List ingresses (from both config.py and database)."""
    try:
        data = request.get_json() or {}
        
        db = get_db()

        try:
            # Get connection status from ingress manager
            ingress_manager = get_ingress_manager()

            ingress_list = []

            # 1. Add default ingress from config.py
            default_ingress = {
                'ingress_id': Config.DEFAULT_INGRESS_ID,
                'host': Config.DEFAULT_INGRESS_HOST,
                'port': Config.DEFAULT_INGRESS_PORT,
                'enabled': True,
                'status': INGRESS_STATUS_ONLINE if ingress_manager.is_connected(Config.DEFAULT_INGRESS_ID) else INGRESS_STATUS_OFFLINE,
                'created_at': None,
                'from_config': True  # Mark as from config
            }
            ingress_list.append(default_ingress)

            # 2. Add other ingresses from database (excluding default ingress)
            ingresses = db.query(Ingress).filter(
                Ingress.ingress_id != Config.DEFAULT_INGRESS_ID
            ).all()

            for ingress in ingresses:
                # Update status based on actual connection
                connection_status_int = INGRESS_STATUS_ONLINE if ingress_manager.is_connected(ingress.ingress_id) else INGRESS_STATUS_OFFLINE

                ingress_list.append({
                    'ingress_id': ingress.ingress_id,
                    'host': ingress.host,
                    'port': ingress.port,
                    'status': int(connection_status_int),  # 整数：0=离线，1=在线（前端自行转换显示，强制转换为整数）
                    'enabled': ingress.enabled,
                    'created_at': ingress.created_at.isoformat() if ingress.created_at else None,
                    'from_config': False
                })

            return jsonify({
                'code': 0,
                'message': 'success',
                'ingresses': ingress_list
            })

        finally:
            db.close()

    except Exception as e:
        logger.error(f"List ingresses error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/ingresses/add', methods=['POST'])
def add_ingress():
    """Add ingress configuration (cannot add default ingress from config)."""
    try:
        data = request.get_json() or {}
        if not data:
            return jsonify({'code': 40006, 'message': 'Invalid request body'}), 400

        ingress_id = data.get('ingress_id')
        host = data.get('host')
        port = data.get('port')

        if not ingress_id or not host or not port:
            return jsonify({'code': 40006, 'message': 'Missing required fields'}), 400

        # Cannot add default ingress from config
        if ingress_id == Config.DEFAULT_INGRESS_ID:
            return jsonify({'code': 40006, 'message': 'Cannot add default ingress from config, modify config.py instead'}), 400

        db = get_db()

        try:
            # Check if ingress already exists
            existing = db.query(Ingress).filter(Ingress.ingress_id == ingress_id).first()
            if existing:
                return jsonify({'code': 40006, 'message': 'Ingress already exists'}), 400

            ingress = Ingress(
                ingress_id=ingress_id,
                host=host,
                port=port,
                enabled=data.get('enabled', True)
            )
            db.add(ingress)
            db.commit()

            # Notify ingress manager to connect to new ingress
            ingress_manager = get_ingress_manager()
            ingress_manager.add_ingress(ingress)

            return jsonify({'code': 0, 'message': 'success'})

        except Exception as e:
            db.rollback()
            logger.error(f"Add ingress error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Add ingress error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/ingresses/update', methods=['POST'])
def update_ingress():
    """Update ingress configuration (cannot update default ingress from config)."""
    try:
        data = request.get_json() or {}
        ingress_id = data.get('ingress_id', '')
        
        # Cannot update default ingress from config
        if ingress_id == Config.DEFAULT_INGRESS_ID:
            return jsonify({'code': 40006, 'message': 'Cannot update default ingress from config, modify config.py instead'}), 400

        if not data:
            return jsonify({'code': 40006, 'message': 'Invalid request body'}), 400

        db = get_db()

        try:
            ingress = db.query(Ingress).filter(Ingress.ingress_id == ingress_id).first()

            if not ingress:
                return jsonify({'code': 40004, 'message': 'Ingress not found'}), 404

            # Update fields
            if 'host' in data:
                ingress.host = data['host']
            if 'port' in data:
                ingress.port = data['port']
            if 'enabled' in data:
                ingress.enabled = data['enabled']

            ingress.updated_at = datetime.utcnow()
            db.commit()

            # Notify ingress manager to update connection
            ingress_manager = get_ingress_manager()
            ingress_manager.update_ingress(ingress)

            return jsonify({'code': 0, 'message': 'success'})

        except Exception as e:
            db.rollback()
            logger.error(f"Update ingress error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Update ingress error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500

@api_bp.route('/ingresses/delete', methods=['POST'])
def delete_ingress():
    """Delete ingress configuration (cannot delete default ingress from config)."""
    try:
        data = request.get_json() or {}
        ingress_id = data.get('ingress_id', '')
        
        # Cannot delete default ingress from config
        if ingress_id == Config.DEFAULT_INGRESS_ID:
            return jsonify({'code': 40006, 'message': 'Cannot delete default ingress from config, modify config.py instead'}), 400

        db = get_db()

        try:
            ingress = db.query(Ingress).filter(Ingress.ingress_id == ingress_id).first()

            if not ingress:
                return jsonify({'code': 40004, 'message': 'Ingress not found'}), 404

            ingress_id = ingress.ingress_id
            db.delete(ingress)
            db.commit()

            # Notify ingress manager to disconnect
            ingress_manager = get_ingress_manager()
            ingress_manager.remove_ingress(ingress_id)

            return jsonify({'code': 0, 'message': 'success'})

        except Exception as e:
            db.rollback()
            logger.error(f"Delete ingress error: {e}")
            return jsonify({'code': 50002, 'message': 'Database error'}), 500
        finally:
            db.close()

    except Exception as e:
        logger.error(f"Delete ingress error: {e}")
        return jsonify({'code': 50001, 'message': 'Internal error'}), 500



