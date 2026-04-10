#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Database models and initialization (v0.1 MVP).
"""

from datetime import datetime
import logging
from sqlalchemy import create_engine, Column, Integer, String, Boolean, DateTime, ForeignKey, text, inspect
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, relationship
from config import Config

Base = declarative_base()

# 边缘状态常量（使用整数，便于快速处理）
EDGE_STATUS_OFFLINE = 0  # 离线
EDGE_STATUS_ONLINE = 1   # 在线

# Ingress 状态常量
INGRESS_STATUS_OFFLINE = 0  # 离线
INGRESS_STATUS_ONLINE = 1   # 在线

def edge_status_to_str(status):
    """将设备状态整数转换为字符串（用于显示）
    
    兼容处理：SQLite 可能将整数存储为字符串，需要兼容处理
    """
    # 兼容处理：如果传入的是字符串，先转换为整数
    if isinstance(status, str):
        if status == 'online':
            return 'online'
        elif status == 'offline':
            return 'offline'
        elif status in ('0', '1'):
            status = int(status)
        else:
            return 'unknown'
    
    # 整数状态转换
    if status == EDGE_STATUS_ONLINE or status == 1:
        return 'online'
    elif status == EDGE_STATUS_OFFLINE or status == 0:
        return 'offline'
    else:
        return 'unknown'

def edge_status_from_str(status_str):
    """将设备状态字符串转换为整数（用于存储）"""
    if status_str == 'online':
        return EDGE_STATUS_ONLINE
    elif status_str == 'offline':
        return EDGE_STATUS_OFFLINE
    else:
        return EDGE_STATUS_OFFLINE  # 默认离线

def ingress_status_to_str(status):
    """将 Ingress 状态整数转换为字符串（用于显示）
    
    兼容处理：SQLite 可能将整数存储为字符串，需要兼容处理
    """
    # 兼容处理：如果传入的是字符串，先转换为整数
    if isinstance(status, str):
        if status == 'online':
            return 'online'
        elif status == 'offline':
            return 'offline'
        elif status in ('0', '1'):
            status = int(status)
        else:
            return 'unknown'
    
    # 整数状态转换
    if status == INGRESS_STATUS_ONLINE or status == 1:
        return 'online'
    elif status == INGRESS_STATUS_OFFLINE or status == 0:
        return 'offline'
    else:
        return 'unknown'

class Edge(Base):
    """Device model (v0.1 MVP)."""
    __tablename__ = "edges"

    edge_id = Column(String(64), primary_key=True)
    edge_type = Column(String(32), nullable=False)
    edge_key = Column(String(128), nullable=False)
    local_ip = Column(String(45))  # Support IPv6
    public_ip = Column(String(45))
    status = Column(Integer, default=EDGE_STATUS_OFFLINE)  # 0=offline, 1=online
    ingress_id = Column(String(64), ForeignKey('ingresses.ingress_id'))
    last_online_time = Column(DateTime)  # 设备上次上线时间戳（用于计算链接时长）
    last_offline_time = Column(DateTime)  # 设备上次离线时间戳（用于统计）
    # confirmed: 99=待确认，1=已确认 (白名单)，0=黑名单 (拒绝)
    confirmed = Column(Integer, default=99)  # SmallInteger: 99=pending, 1=confirmed, 0=rejected
    created_at = Column(DateTime, default=datetime.utcnow)
    updated_at = Column(DateTime, default=datetime.utcnow, onupdate=datetime.utcnow)

    # Relationships
    ingress = relationship('Ingress', back_populates="edges")

class Ingress(Base):
    """Ingress model (v0.1 MVP)."""
    __tablename__ = 'ingresses'
    
    ingress_id = Column(String(64), primary_key=True)
    host = Column(String(255), nullable=False)
    port = Column(Integer, nullable=False)
    enabled = Column(Boolean, default=True)
    status = Column(Integer, default=INGRESS_STATUS_OFFLINE)  # 0=offline, 1=online
    created_at = Column(DateTime, default=datetime.utcnow)
    updated_at = Column(DateTime, default=datetime.utcnow, onupdate=datetime.utcnow)
    
    # Relationships
    edges = relationship('Edge', back_populates='ingress')

# Database engine and session
_engine = None
_SessionLocal = None

def init_db():
    """Initialize database and create default data if needed."""
    global _engine, _SessionLocal

    if _engine is None:
        # 使用 WAL 模式提高并发写入性能
        _engine = create_engine(f'sqlite:///{Config.DATABASE_PATH}', echo=False)
        # 启用 WAL 模式和 busy_timeout
        with _engine.connect() as conn:
            from sqlalchemy import text
            conn.execute(text("PRAGMA journal_mode=WAL"))
            conn.execute(text("PRAGMA busy_timeout=5000"))
            conn.commit()

        Base.metadata.create_all(_engine)
        _SessionLocal = sessionmaker(bind=_engine)

        # 迁移现有数据（将字符串状态转换为整数）
        _migrate_status_to_int()

        # 迁移添加新列（link_duration 等）
        _migrate_add_columns()

        # 初始化默认数据（如果数据库为空）
        _init_default_data()

    return _engine

def _migrate_status_to_int():
    """迁移数据库：将字符串状态字段转换为整数类型。

    SQLite 不支持直接修改列类型，但可以通过以下方式处理：
    1. 检查现有数据，如果是字符串则转换为整数存储
    2. 新表创建时已经是整数类型，无需迁移
    """
    if _SessionLocal is None:
        return

    db = _SessionLocal()
    logger = logging.getLogger(__name__)
    try:
        from sqlalchemy import inspect, text

        # 检查 devices 表是否存在且需要迁移
        try:
            inspector = inspect(_engine)
            if 'devices' in inspector.get_table_names():
                # 尝试查询一条记录，检查 status 是否为字符串
                result = db.execute(text("SELECT status FROM devices LIMIT 1")).fetchone()
                if result and result[0] in ('online', 'offline'):
                    logger.info("Migrating device status from string to integer...")
                    # 转换数据：'online' -> 1, 'offline' -> 0
                    db.execute(text("UPDATE devices SET status = CASE WHEN status = 'online' THEN 1 ELSE 0 END"))
                    db.commit()
                    logger.info("Device status migration completed")
        except Exception as e:
            # 如果表不存在或已经是整数类型，忽略
            logger.debug(f"Device status migration check: {e}")

        # 检查 ingresses 表
        try:
            if 'ingresses' in inspector.get_table_names():
                result = db.execute(text("SELECT status FROM ingresses LIMIT 1")).fetchone()
                if result and result[0] in ('online', 'offline'):
                    logger.info("Migrating ingress status from string to integer...")
                    db.execute(text("UPDATE ingresses SET status = CASE WHEN status = 'online' THEN 1 ELSE 0 END"))
                    db.commit()
                    logger.info("Ingress status migration completed")
        except Exception as e:
            logger.debug(f"Ingress status migration check: {e}")

    except Exception as e:
        db.rollback()
        logger.debug(f"Status migration: {e}")
    finally:
        db.close()

def _migrate_add_columns():
    """迁移数据库：添加新列（如 confirmed, last_offline_time 等）。

    SQLite 不支持直接修改列，但可以使用 ALTER TABLE ADD COLUMN。
    注意：瞬时数据（如 link_duration）不在数据库存储，动态计算。
    """
    if _SessionLocal is None:
        return

    db = _SessionLocal()
    logger = logging.getLogger(__name__)
    try:
        from sqlalchemy import inspect, text

        inspector = inspect(_engine)
        if 'devices' not in inspector.get_table_names():
            return

        # 检查 devices 表的列
        columns = [col['name'] for col in inspector.get_columns('devices')]

        # 添加 confirmed 列（如果不存在）
        if 'confirmed' not in columns:
            logger.info("Adding confirmed column to devices table...")
            db.execute(text("ALTER TABLE edges ADD COLUMN confirmed INTEGER DEFAULT 99"))
            db.execute(text("UPDATE devices SET confirmed = 99"))
            db.commit()
            logger.info("confirmed column added")
        else:
            logger.debug("confirmed column already exists")

        # 添加 last_offline_time 列（如果不存在）
        if 'last_offline_time' not in columns:
            logger.info("Adding last_offline_time column to devices table...")
            db.execute(text("ALTER TABLE edges ADD COLUMN last_offline_time DATETIME"))
            # 将现有的 last_online_time 复制到 last_offline_time（作为默认值）
            db.execute(text("UPDATE devices SET last_offline_time = last_online_time WHERE last_online_time IS NOT NULL"))
            db.commit()
            logger.info("last_offline_time column added")
        else:
            logger.debug("last_offline_time column already exists")

        # 注意：last_heartbeat 列不再使用，但保留兼容性
        # 新系统不再写入 last_heartbeat

    except Exception as e:
        db.rollback()
        logger.debug(f"Add columns migration: {e}")
    finally:
        db.close()

def _init_default_data():
    """Initialize default data (deprecated - default ingress now managed by config.py)."""
    # 默认 ingress 现在由 config.py 管理，不再自动创建数据库记录
    # 此函数保留但不再创建默认 ingress，以兼容旧代码
    pass

def get_db():
    """Get database session.
    
    注意：每次调用返回新的 session，调用者负责 close()。
    在并发场景下，每个线程/协程应该持有自己的 session。
    """
    if _SessionLocal is None:
        init_db()
    return _SessionLocal()


# 线程安全的数据库操作锁（用于保护并发更新）
import threading
_db_lock = threading.Lock()

def get_db_lock():
    """Get database lock for thread-safe operations.
    
    用于保护需要事务隔离的并发数据库操作。
    使用方式：
        with get_db_lock():
            db = get_db()
            try:
                # 数据库操作
                db.commit()
            finally:
                db.close()
    """
    return _db_lock

