#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Logging utility for touch_manager (v0.1 MVP).

Supports:
- Rotating file handler (size-based rotation)
- Multiple log formats (nginx-like, detailed, simple)
- Configurable log path, max file size, backup count
"""

import logging
import os
from logging.handlers import RotatingFileHandler
from datetime import datetime
from config import Config


class NginxFormatter(logging.Formatter):
    """Nginx-style log formatter.
    
    Format example:
    127.0.0.1 - - [01/Mar/2026:22:00:00 +0000] "POST /api/v1/edges HTTP/1.1" 200 123 "-" "python-requests/2.28.0"
    """
    
    def format(self, record):
        # Get client IP if available (try multiple sources)
        remote_addr = '-'
        try:
            # Try to get from record created by Flask
            if hasattr(record, 'remote_addr') and record.remote_addr:
                remote_addr = record.remote_addr
            # Try to get from message
            elif '127.0.0.1' in record.getMessage():
                remote_addr = '127.0.0.1'
        except:
            pass
        
        # Format timestamp like nginx
        timestamp = datetime.fromtimestamp(record.created).strftime('%d/%b/%Y:%H:%M:%S %z')
        
        # Map logging level to HTTP status (for reference)
        level_map = {
            'DEBUG': 'DEBUG',
            'INFO': 'INFO',
            'WARNING': 'WARN',
            'ERROR': 'ERROR',
            'CRITICAL': 'CRIT'
        }
        level = level_map.get(record.levelname, record.levelname)
        
        # Build nginx-like log line
        # Format: IP - - [timestamp] "METHOD PATH PROTOCOL" status size "referer" "user-agent" level logger_name message
        log_line = (
            f'{remote_addr} - - [{timestamp}] '
            f'"{self._get_request_info(record)}" '
            f'- - '
            f'"{level}" '
            f'"{record.name}" '
            f'"{record.getMessage()}"'
        )
        
        # Add exception info if present
        if record.exc_info:
            log_line += f'\n{self.formatException(record.exc_info)}'
        
        return log_line
    
    def _get_request_info(self, record):
        """Extract HTTP request info from log record."""
        try:
            from flask import request
            method = request.method
            path = request.path
            return f'{method} {path} HTTP/{request.environ.get("SERVER_PROTOCOL", "1.1")}'
        except:
            return '-'


class DetailedFormatter(logging.Formatter):
    """Detailed log formatter with full context."""
    
    def format(self, record):
        timestamp = datetime.fromtimestamp(record.created).strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        return (
            f'{timestamp} - {record.levelname:<8} - {record.name:<30} - '
            f'{record.filename}:{record.lineno} - {record.getMessage()}'
        )


class SimpleFormatter(logging.Formatter):
    """Simple log formatter."""
    
    def format(self, record):
        timestamp = datetime.fromtimestamp(record.created).strftime('%Y-%m-%d %H:%M:%S')
        return f'{timestamp} [{record.levelname}] {record.name}: {record.getMessage()}'


def get_formatter(format_type='nginx'):
    """Get log formatter by type."""
    if format_type == 'nginx':
        return NginxFormatter()
    elif format_type == 'detailed':
        return DetailedFormatter()
    else:
        return SimpleFormatter()


def setup_logging():
    """Setup logging with rotating file handler.
    
    Configuration:
    - Log path: LOG_PATH (default: ./logs)
    - Log file: LOG_FILE (default: touch_manager.log)
    - Max file size: LOG_MAX_BYTES (default: 10MB)
    - Backup count: LOG_BACKUP_COUNT (default: 5)
    - Log level: LOG_LEVEL (default: INFO)
    - Log format: LOG_FORMAT (default: nginx)
    """
    # Create log directory
    log_path = Config.LOG_PATH
    os.makedirs(log_path, exist_ok=True)
    
    # Log file path
    log_file = os.path.join(log_path, Config.LOG_FILE)
    
    # Get root logger
    root_logger = logging.getLogger()
    root_logger.setLevel(getattr(logging, Config.LOG_LEVEL.upper(), logging.INFO))
    
    # Clear existing handlers
    root_logger.handlers.clear()
    
    # Create rotating file handler
    # - Rotates when file reaches LOG_MAX_BYTES
    # - Keeps LOG_BACKUP_COUNT backup files
    # - Total disk usage: LOG_MAX_BYTES * (LOG_BACKUP_COUNT + 1)
    file_handler = RotatingFileHandler(
        filename=log_file,
        maxBytes=Config.LOG_MAX_BYTES,
        backupCount=Config.LOG_BACKUP_COUNT,
        encoding='utf-8',
        delay=False
    )
    file_handler.setLevel(getattr(logging, Config.LOG_LEVEL.upper(), logging.INFO))
    file_handler.setFormatter(get_formatter(Config.LOG_FORMAT))
    root_logger.addHandler(file_handler)
    
    # Create console handler (for development)
    console_handler = logging.StreamHandler()
    console_handler.setLevel(getattr(logging, Config.LOG_LEVEL.upper(), logging.INFO))
    console_handler.setFormatter(SimpleFormatter())
    root_logger.addHandler(console_handler)
    
    # Log configuration info
    logger = logging.getLogger(__name__)
    logger.info(f"Logging initialized: path={log_path}, file={Config.LOG_FILE}, "
                f"max_bytes={Config.LOG_MAX_BYTES}, backup_count={Config.LOG_BACKUP_COUNT}, "
                f"level={Config.LOG_LEVEL}, format={Config.LOG_FORMAT}")
    
    return root_logger


def get_logger(name=None):
    """Get a logger instance.
    
    Args:
        name: Logger name (default: None for root logger)
    
    Returns:
        logging.Logger instance
    """
    return logging.getLogger(name)
