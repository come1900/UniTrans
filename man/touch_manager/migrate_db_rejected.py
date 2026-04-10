#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Database migration script to add 'rejected' column to devices table.
"""

import sqlite3
import os

DATABASE_PATH = os.path.expanduser('~/svn/daily/src/appmodule/touch/touch_manager/touch_manager.db')

def migrate():
    """Add 'rejected' column to devices table."""
    conn = sqlite3.connect(DATABASE_PATH)
    cursor = conn.cursor()
    
    try:
        # Check if column already exists
        cursor.execute("PRAGMA table_info(devices)")
        columns = [row[1] for row in cursor.fetchall()]
        
        if 'rejected' in columns:
            print("Column 'rejected' already exists")
            return
        
        # Add rejected column with default value FALSE
        cursor.execute('''
            ALTER TABLE devices ADD COLUMN rejected BOOLEAN DEFAULT FALSE
        ''')
        
        conn.commit()
        print("Migration completed successfully!")
        print("Added 'rejected' column to devices table")
        
    except Exception as e:
        conn.rollback()
        print(f"Migration failed: {e}")
        raise
    finally:
        conn.close()

if __name__ == '__main__':
    migrate()
