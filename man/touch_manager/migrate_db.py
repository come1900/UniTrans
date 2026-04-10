#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Database migration script to add 'confirmed' column to edges table.
"""

import sqlite3
import os

DATABASE_PATH = os.path.expanduser('~/svn/daily/src/appmodule/touch/touch_manager/touch_manager.db')

def migrate():
    """Add 'confirmed' column to edges table.

    Note: link_duration is NOT stored in database - it's calculated dynamically
    from last_online_time to avoid frequent database writes.
    """
    conn = sqlite3.connect(DATABASE_PATH)
    cursor = conn.cursor()

    try:
        # Check if column already exists
        cursor.execute("PRAGMA table_info(devices)")
        columns = [row[1] for row in cursor.fetchall()]

        if 'confirmed' not in columns:
            # Add confirmed column with default value 99 (pending)
            cursor.execute('''
                ALTER TABLE devices ADD COLUMN confirmed INTEGER DEFAULT 99
            ''')
            # Update existing devices to confirmed=99 (pending)
            cursor.execute('''
                UPDATE devices SET confirmed = 99
            ''')
            print("Added 'confirmed' column to edges table")
        else:
            print("Column 'confirmed' already exists")

        conn.commit()
        print("Migration completed successfully!")

    except Exception as e:
        conn.rollback()
        print(f"Migration failed: {e}")
        raise
    finally:
        conn.close()

if __name__ == '__main__':
    migrate()
