#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
API blueprint for touch_manager (v0.1 MVP).
"""

from flask import Blueprint

api_bp = Blueprint('api', __name__)

# Import routes
from . import ingresses, edges

