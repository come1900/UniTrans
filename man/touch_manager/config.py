#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Configuration settings for touch_manager (v0.2).
"""

import os
from pathlib import Path

class Config:
    """Application configuration."""

    # Base directory
    BASE_DIR = Path(__file__).parent

    # Database
    # DATABASE_PATH: 完整数据库文件路径（优先级最高）
    # DATABASE_NAME: 数据库文件名（相对于 BASE_DIR），用于运行多套 manager 实例
    _db_name = os.getenv('DATABASE_NAME', 'touch_manager.db')
    DATABASE_PATH = os.getenv('DATABASE_PATH', str(BASE_DIR / _db_name))

    # Server port
    PORT = int(os.getenv('PORT', 18050))

    # Ingress connection
    INGRESS_CONNECT_TIMEOUT = int(os.getenv('INGRESS_CONNECT_TIMEOUT', 5))  # seconds
    INGRESS_RECONNECT_INTERVAL = int(os.getenv('INGRESS_RECONNECT_INTERVAL', 10))  # seconds

    # Default ingress server configuration
    DEFAULT_INGRESS_ID = os.getenv('DEFAULT_INGRESS_ID', 'lili-47.100.49.48')
    DEFAULT_INGRESS_HOST = os.getenv('DEFAULT_INGRESS_HOST', '47.100.49.48')
    DEFAULT_INGRESS_PORT = int(os.getenv('DEFAULT_INGRESS_PORT', 54321))

    # WebSocket ping/pong configuration
    # ping_timeout should be 1/3 to 1/2 of ping_interval for reliable detection
    # Reference: RFC 6455 and industry best practices
    WEBSOCKET_PING_INTERVAL = int(os.getenv('WEBSOCKET_PING_INTERVAL', 15))  # seconds
    WEBSOCKET_PING_TIMEOUT = int(os.getenv('WEBSOCKET_PING_TIMEOUT', 5))  # seconds (1/3 of interval)

    # Edge list query
    EDGE_LIST_BATCH_SIZE = int(os.getenv('EDGE_LIST_BATCH_SIZE', 50))  # 每批次获取的边缘数量
    EDGE_LIST_QUERY_TIMEOUT = int(os.getenv('EDGE_LIST_QUERY_TIMEOUT', 10))  # 查询超时时间（秒）

    # Logging configuration
    LOG_LEVEL = os.getenv('LOG_LEVEL', 'DEBUG')  # DEBUG, INFO, WARNING, ERROR, CRITICAL
    LOG_PATH = os.getenv('LOG_PATH', str('/tmp/logs'))
    LOG_FILE = os.getenv('LOG_FILE', 'touch_manager.log')
    LOG_MAX_BYTES = int(os.getenv('LOG_MAX_BYTES', 1 * 1024 * 1024))  # 10MB
    LOG_BACKUP_COUNT = int(os.getenv('LOG_BACKUP_COUNT', 3))  # 保留 3 个备份文件
    LOG_FORMAT = os.getenv('LOG_FORMAT', 'nginx')  # nginx, detailed, simple
