#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
COME protocol message method constants.

Naming convention: <initiator>.<target>.<action> or <initiator>.<action>
Network elements: edge (edge device), ingress (access gateway), manager (management center)

Message flow:
- Edge → Manager/Ingress: edge.online, edge.heartbeat, edge.offline
- Manager → Ingress: manager.connect, manager.edge.list, manager.edge.kick, manager.edge.online, manager.edge.offline
- Ingress → Manager: ingress.edge.online, ingress.edge.offline
"""

# Edge initiated messages (Edge → Manager/Ingress)
EDGE_ONLINE = "edge.online"           # Edge 上线请求
EDGE_HEARTBEAT = "edge.heartbeat"     # Edge 心跳通知
EDGE_OFFLINE = "edge.offline"         # Edge 下线通知

# Manager initiated messages (Manager → Ingress)
MANAGER_CONNECT = "manager.connect"           # Manager 连接通知
MANAGER_EDGE_LIST = "manager.edge.list"       # Manager 查询边缘列表
MANAGER_EDGE_OFFLINE_LIST = "manager.edge.offline.list"  # Manager 查询离线边缘列表
MANAGER_EDGE_KICK = "manager.edge.kick"       # Manager 踢边缘

# Ingress initiated messages (Ingress → Manager)
INGRESS_EDGE_ONLINE = "ingress.edge.online"   # Ingress 通知边缘上线
INGRESS_EDGE_OFFLINE = "ingress.edge.offline" # Ingress 通知边缘下线
