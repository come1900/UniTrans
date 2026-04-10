#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Database migration script to update confirmed field type.
confirmed: 99=pending, 1=confirmed (whitelist), 0=rejected (blacklist)
"""

import sqlite3
import os

DATABASE_PATH = os.path.expanduser('~/svn/daily/src/appmodule/touch/touch_manager/touch_manager.db')

def migrate():
    """Migrate confirmed field from boolean to integer."""
    conn = sqlite3.connect(DATABASE_PATH)
    cursor = conn.cursor()
    
    try:
        # Check current schema
        cursor.execute("PRAGMA table_info(devices)")
        columns = {row[1]: row[2] for row in cursor.fetchall()}
        
        if 'confirmed' not in columns:
            print("Error: 'confirmed' column not found")
            return
        
        print(f"Current confirmed type: {columns['confirmed']}")
        
        # Drop rejected column if exists
        if 'rejected' in columns:
            print("Dropping 'rejected' column...")
            # SQLite doesn't support DROP COLUMN directly, need to recreate table
            cursor.execute("""
                CREATE TABLE devices_new (
                    device_id VARCHAR(64) PRIMARY KEY,
                    device_type VARCHAR(32) NOT NULL,
                    device_key VARCHAR(128) NOT NULL,
                    local_ip VARCHAR(45),
                    public_ip VARCHAR(45),
                    status VARCHAR(16) DEFAULT 'offline',
                    ingress_id VARCHAR(64),
                    last_online_time DATETIME,
                    last_offline_time DATETIME,
                    confirmed INTEGER DEFAULT 99,
                    created_at DATETIME,
                    updated_at DATETIME,
                    FOREIGN KEY (ingress_id) REFERENCES ingresses(ingress_id)
                )
            """)

            # Copy data, converting boolean to integer
            # Note: last_heartbeat is removed, use last_offline_time instead
            cursor.execute("""
                INSERT INTO devices_new
                SELECT device_id, device_type, device_key, local_ip, public_ip,
                       status, ingress_id, last_online_time, 
                       last_online_time as last_offline_time,  -- Use last_online_time as default for last_offline_time
                       CASE
                           WHEN rejected = 1 THEN 0  -- rejected=True -> confirmed=0 (blacklist)
                           WHEN confirmed = 1 THEN 1  -- confirmed=True -> confirmed=1 (whitelist)
                           ELSE 99  -- default pending
                       END as confirmed,
                       created_at, updated_at
                FROM devices
            """)
            
            # Drop old table and rename new one
            cursor.execute("DROP TABLE devices")
            cursor.execute("ALTER TABLE devices_new RENAME TO devices")
            
            print("Migration completed successfully!")
        else:
            # Just update values if column is already INTEGER
            print("Updating confirmed values...")
            # No action needed if already using new format
        
        conn.commit()
        print("Database migrated successfully!")
        
    except Exception as e:
        conn.rollback()
        print(f"Migration failed: {e}")
        raise
    finally:
        conn.close()

if __name__ == '__main__':
    migrate()
