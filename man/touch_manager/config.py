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
    DEFAULT_INGRESS_ID = os.getenv('DEFAULT_INGRESS_ID', 'local-127.0.0.1')
    DEFAULT_INGRESS_HOST = os.getenv('DEFAULT_INGRESS_HOST', '127.0.0.1')
    DEFAULT_INGRESS_PORT = int(os.getenv('DEFAULT_INGRESS_PORT', 54321))

    # WebSocket ping/pong configuration
    # ping_timeout should be 1/3 to 1/2 of ping_interval for reliable detection
    # Reference: RFC 6455 and industry best practices
    WEBSOCKET_PING_INTERVAL = int(os.getenv('WEBSOCKET_PING_INTERVAL', 15))  # seconds
    WEBSOCKET_PING_TIMEOUT = int(os.getenv('WEBSOCKET_PING_TIMEOUT', 5))  # seconds (1/3 of interval)

    # Edge list query
    EDGE_LIST_BATCH_SIZE = int(os.getenv('EDGE_LIST_BATCH_SIZE', 50))  # 每批次获取的边缘数量
    EDGE_LIST_QUERY_TIMEOUT = int(os.getenv('EDGE_LIST_QUERY_TIMEOUT', 10))  # 查询超时时间（秒）

    # ------------------------------------------------------------------
    # 配置下发"确认超时"阈值 (CONFIG_CONFIRM_TIMEOUT, 单位: 秒)
    #  作用: edge 收到配置后仍处于 configuring(未 confirmed)时, 再次 POST 同一条配置,
    #       允许重推前必须等待的间隔。用于避免在 edge 处理完成前反复重推(节流)。
    #  判定(见 api/edges.py): 距上次下发超过本阈值 -> 允许重试(重新分配版本并重推);
    #       未超过 -> 返回 busy(50008 "Config update in progress, please try later")。
    #  取值:
    #    1~3600 : 有效范围(秒)。默认 10。
    #    0      : 关闭此节流逻辑 —— 此时每次 POST 都直接重推, 不等待、不返回 busy。
    #    其他(超范围/空值/非法值) : 回退到默认值 10。
    #  可通过环境变量 CONFIG_CONFIRM_TIMEOUT 覆盖。
    # ------------------------------------------------------------------
    try:
        _cfg_confirm_timeout = int(os.getenv('CONFIG_CONFIRM_TIMEOUT', 10))
    except (TypeError, ValueError):
        _cfg_confirm_timeout = 10  # 空值/非法值回退默认
    if _cfg_confirm_timeout == 0:
        CONFIG_CONFIRM_TIMEOUT = 0
    elif 1 <= _cfg_confirm_timeout <= 3600:
        CONFIG_CONFIRM_TIMEOUT = _cfg_confirm_timeout
    else:
        CONFIG_CONFIRM_TIMEOUT = 10  # 超范围回退默认

    # Logging configuration
    LOG_LEVEL = os.getenv('LOG_LEVEL', 'DEBUG')  # DEBUG, INFO, WARNING, ERROR, CRITICAL
    # 文件与 stdout 可分别设级: 未设置(空)时跟随 LOG_LEVEL, 保持现有行为一致。
    #  用法示例: LOG_LEVEL=DEBUG LOG_LEVEL_CONSOLE=INFO —— 文件记全量DEBUG, 终端只看INFO(不刷屏)。
    LOG_LEVEL_FILE = os.getenv('LOG_LEVEL_FILE', '')
    LOG_LEVEL_CONSOLE = os.getenv('LOG_LEVEL_CONSOLE', 'INFO')
    LOG_PATH = os.getenv('LOG_PATH', str('/tmp/logs'))
    LOG_FILE = os.getenv('LOG_FILE', 'touch_manager.log')
    LOG_MAX_BYTES = int(os.getenv('LOG_MAX_BYTES', 1 * 1024 * 1024))  # 10MB
    LOG_BACKUP_COUNT = int(os.getenv('LOG_BACKUP_COUNT', 3))  # 保留 3 个备份文件
    LOG_FORMAT = os.getenv('LOG_FORMAT', 'nginx')  # nginx, detailed, simple
