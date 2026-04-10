#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
touch_manager - Management Service for Touch System (v0.2)

Main application entry point.
"""

import os
import sys
import logging
from flask import Flask, render_template
from flask_cors import CORS

from config import Config
from database import init_db
from api import api_bp
from ingress_manager import get_ingress_manager
from logger import setup_logging, get_logger

# Initialize logging with rotating file handler
setup_logging()
logger = get_logger(__name__)

def create_app(config_class=Config):
    """Create and configure Flask application."""
    app = Flask(__name__)
    app.config.from_object(config_class)
    
    # Enable CORS
    CORS(app)
    
    # Initialize database
    init_db()
    
    # Start ingress manager (connect to all enabled ingresses)
    ingress_manager = get_ingress_manager()
    ingress_manager.start()
    
    # Register blueprints
    app.register_blueprint(api_bp, url_prefix='/api/v1')
    
    @app.route('/')
    def index():
        """主页"""
        return render_template('index.html')
    
    @app.route('/api')
    def api_info():
        """API信息（JSON格式）"""
        return {'status': 'ok', 'service': 'touch_manager', 'version': '0.1'}
    
    @app.route('/health')
    def health():
        return {'status': 'ok'}
    
    @app.route('/edges')
    def edges_page():
        """边缘列表页面"""
        return render_template('edges.html')
    
    @app.route('/ingresses')
    def ingresses_page():
        """Ingress列表页面"""
        return render_template('ingresses.html')
    
    return app

if __name__ == '__main__':
    import signal
    
    app = create_app()
    ingress_manager = get_ingress_manager()
    
    # 信号处理函数
    def signal_handler(signum, frame):
        logger.info(f"Received signal {signum}, shutting down...")
        ingress_manager.stop()
        logger.info("touch_manager stopped")
        sys.exit(0)
    
    # 注册信号处理器
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    try:
        # Get port from environment or use default
        from config import Config
        port = Config.PORT
        host = os.getenv('HOST', '0.0.0.0')

        logger.info(f"Starting touch_manager on {host}:{port}")
        # 禁用 reloader 以避免 ingress_manager 被初始化两次
        # https://flask.palletsprojects.com/en/2.3.x/api/#flask.Flask.run
        app.run(host=host, port=port, debug=False)
    except KeyboardInterrupt:
        logger.info("KeyboardInterrupt received, shutting down...")
    finally:
        ingress_manager.stop()
        logger.info("touch_manager stopped")
