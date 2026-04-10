#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Ingress connection manager for touch_manager (v0.1 MVP).

Manages WebSocket connections to all configured touch_ingress instances.
"""

import logging
from typing import Dict, Optional
from database import get_db, Ingress
from ingress_client import IngressClient
from config import Config
from protocol import MANAGER_EDGE_KICK

logger = logging.getLogger(__name__)

class IngressManager:
    """Manages connections to all ingress instances."""

    def __init__(self):
        self.clients: Dict[str, IngressClient] = {}
        self.running = False

    def start(self):
        """Start the ingress manager and connect to all enabled ingresses."""
        if self.running:
            logger.warning("Ingress manager already running")
            return

        self.running = True
        logger.info("Starting ingress manager...")
        self._connect_all_ingresses()

    def stop(self):
        """Stop the ingress manager and disconnect from all ingresses."""
        if not self.running:
            return

        self.running = False
        logger.info("Stopping ingress manager...")

        for ingress_id, client in self.clients.items():
            client.stop()

        self.clients.clear()
        logger.info("Ingress manager stopped")

    def _connect_all_ingresses(self):
        """Connect to default ingress from config and other ingresses from database."""
        # 1. Connect to default ingress from config.py
        self._connect_default_ingress_from_config()

        # 2. Connect to other ingresses from database (excluding the default one)
        db = get_db()
        try:
            ingresses = db.query(Ingress).filter(Ingress.enabled == True).all()

            for ingress in ingresses:
                # Skip if this is the default ingress (already connected from config)
                if ingress.ingress_id == Config.DEFAULT_INGRESS_ID:
                    continue

                if ingress.ingress_id not in self.clients:
                    self._connect_ingress(ingress)
                else:
                    # Update existing client if host/port changed
                    client = self.clients[ingress.ingress_id]
                    if client.host != ingress.host or client.port != ingress.port:
                        logger.info(f"Updating ingress {ingress.ingress_id} connection")
                        client.stop()
                        self._connect_ingress(ingress)
        finally:
            db.close()

    def _connect_default_ingress_from_config(self):
        """Connect to the default ingress defined in config.py."""
        try:
            client = IngressClient(
                Config.DEFAULT_INGRESS_ID,
                Config.DEFAULT_INGRESS_HOST,
                Config.DEFAULT_INGRESS_PORT
            )
            client.start()
            self.clients[Config.DEFAULT_INGRESS_ID] = client
            logger.info(f"Started connection to default ingress {Config.DEFAULT_INGRESS_ID} at {Config.DEFAULT_INGRESS_HOST}:{Config.DEFAULT_INGRESS_PORT}")
        except Exception as e:
            logger.error(f"Failed to start connection to default ingress {Config.DEFAULT_INGRESS_ID}: {e}")

    def _connect_ingress(self, ingress: Ingress):
        """Connect to a specific ingress."""
        try:
            client = IngressClient(ingress.ingress_id, ingress.host, ingress.port)
            client.start()
            self.clients[ingress.ingress_id] = client
            logger.info(f"Started connection to ingress {ingress.ingress_id} at {ingress.host}:{ingress.port}")
        except Exception as e:
            logger.error(f"Failed to start connection to ingress {ingress.ingress_id}: {e}")
    
    def add_ingress(self, ingress: Ingress):
        """Add and connect to a new ingress."""
        if ingress.ingress_id in self.clients:
            logger.warning(f"Ingress {ingress.ingress_id} already connected")
            return
        
        if ingress.enabled:
            self._connect_ingress(ingress)
    
    def remove_ingress(self, ingress_id: str):
        """Remove and disconnect from an ingress."""
        if ingress_id in self.clients:
            self.clients[ingress_id].stop()
            del self.clients[ingress_id]
            logger.info(f"Removed connection to ingress {ingress_id}")
    
    def update_ingress(self, ingress: Ingress):
        """Update connection to an ingress (if host/port changed)."""
        if ingress.ingress_id in self.clients:
            client = self.clients[ingress.ingress_id]
            if client.host != ingress.host or client.port != ingress.port:
                logger.info(f"Updating ingress {ingress.ingress_id} connection")
                client.stop()
                if ingress.enabled:
                    self._connect_ingress(ingress)
                else:
                    del self.clients[ingress.ingress_id]
        elif ingress.enabled:
            self._connect_ingress(ingress)
    
    def get_client(self, ingress_id: str) -> Optional[IngressClient]:
        """Get the client for a specific ingress."""
        return self.clients.get(ingress_id)
    
    def is_connected(self, ingress_id: str) -> bool:
        """Check if connected to a specific ingress."""
        client = self.clients.get(ingress_id)
        return client.is_connected() if client else False

    def kick_device(self, device_id: str) -> bool:
        """Kick a device from all connected ingresses (fire-and-forget).
        
        This method sends kick commands asynchronously without waiting for responses.
        Always returns True (success is not guaranteed but command is sent).
        
        Note: This is now non-blocking. Use kick_device_async() alias for clarity.
        """
        # 直接调用异步版本，不等待结果
        self.kick_device_async(device_id)
        return True  # 总是返回 True，因为命令已发送（不保证送达）

    def kick_device_async(self, device_id: str):
        """Kick a device from all connected ingresses (asynchronous, non-blocking).
        
        This method sends kick commands without waiting for responses.
        Use this for batch operations to avoid blocking.
        
        Note: Uses fire-and-forget approach, no response handling.
        """
        import asyncio
        import hashlib
        import time

        for ingress_id, client in self.clients.items():
            if client.is_connected():
                try:
                    # 生成请求 ID
                    req_id = int(hashlib.md5(f"{device_id}_{time.time()}".encode()).hexdigest()[:8], 16) % (2**31)

                    kick_msg = {
                        "jsonrpc": "2.0",
                        "method": MANAGER_EDGE_KICK,
                        "params": {
                            "edge_id": device_id,
                            "reason": "Rejected by admin"
                        },
                        "id": req_id
                    }

                    client_loop = getattr(client, '_loop', None)
                    if client_loop and client_loop.is_running():
                        # 异步发送，不等待结果，不处理响应
                        asyncio.run_coroutine_threadsafe(
                            client.send_message(kick_msg),
                            client_loop
                        )
                        logger.debug(f"Sent async kick command for device {device_id} to ingress {ingress_id}")
                except Exception as e:
                    logger.debug(f"Failed to send async kick command for device {device_id}: {e}")

    def refresh_connections(self):
        """Refresh connections based on config and database state."""
        if not self.running:
            return

        # 1. Refresh default ingress from config
        self._refresh_default_ingress_from_config()

        # 2. Refresh other ingresses from database
        db = get_db()
        try:
            # Get all enabled ingresses from database (excluding default ingress)
            db_ingresses = {}
            for ing in db.query(Ingress).filter(Ingress.enabled == True).all():
                if ing.ingress_id != Config.DEFAULT_INGRESS_ID:
                    db_ingresses[ing.ingress_id] = ing

            # Remove clients for ingresses that are disabled or deleted (excluding default)
            to_remove = []
            for ingress_id, client in self.clients.items():
                if ingress_id != Config.DEFAULT_INGRESS_ID and ingress_id not in db_ingresses:
                    to_remove.append(ingress_id)

            for ingress_id in to_remove:
                self.remove_ingress(ingress_id)

            # Add or update clients for enabled ingresses (excluding default)
            for ingress in db_ingresses.values():
                if ingress.ingress_id not in self.clients:
                    self._connect_ingress(ingress)
                else:
                    self.update_ingress(ingress)
        finally:
            db.close()

    def _refresh_default_ingress_from_config(self):
        """Refresh the default ingress connection based on config.py settings."""
        if Config.DEFAULT_INGRESS_ID in self.clients:
            # Check if config changed
            client = self.clients[Config.DEFAULT_INGRESS_ID]
            if client.host != Config.DEFAULT_INGRESS_HOST or client.port != Config.DEFAULT_INGRESS_PORT:
                logger.info(f"Default ingress config changed, reconnecting...")
                client.stop()
                del self.clients[Config.DEFAULT_INGRESS_ID]
                self._connect_default_ingress_from_config()
        else:
            # Not connected yet, connect now
            self._connect_default_ingress_from_config()

# Global ingress manager instance
_ingress_manager: Optional[IngressManager] = None

def get_ingress_manager() -> IngressManager:
    """Get the global ingress manager instance."""
    global _ingress_manager
    if _ingress_manager is None:
        _ingress_manager = IngressManager()
    return _ingress_manager

