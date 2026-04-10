# Touch System - 接口规范文档

# 概述

本文档定义 Touch 系统的**所有接口规范**，包括：
- **REST API 接口**：Manager ↔ Web UI、Edge 首次注册
- **WebSocket 接口**：Edge ↔ Ingress、Ingress ↔ Manager（come.1 JSON-RPC 2.0 协议）

---

## REST API 接口规范

所有 REST API 接口遵循以下规范：
- **所有接口使用 POST 方法**
- **所有参数放在 JSON 消息体中**
- **URL 不包含变量**

### 传输方式

- **协议**: REST API (HTTP/HTTPS)
- **方法**: POST（所有接口）
- **数据格式**: JSON
- **字符编码**: UTF-8
- **基础路径**: `/api/v1`
- **Content-Type**: `application/json`

---

# REST API 接口 (Manager ↔ Web UI)

## 边缘管理接口

### POST /api/v1/edges/list - 查询边缘列表

查询所有边缘的信息列表，支持分页和过滤。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| page | integer | O | 页码，从 1 开始，默认 1 |
| page_size | integer | O | 每页数量，默认 20，最大 100 |
| edge_type | string | O | 边缘类型过滤 |
| status | string | O | 边缘状态过滤（"online"/"offline"） |
| keyword | string | O | 关键词搜索（边缘 ID、IP 等） |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"page": 1, "page_size": 20, "status": "online"}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success",
  "total": 100,
  "page": 1,
  "page_size": 20,
  "edges": [
    {
      "edge_id": "edge001",
      "edge_type": "touch",
      "local_ip": "192.168.1.100",
      "public_ip": "203.0.113.45",
      "status": 1,
      "confirmed": 1,
      "ingress_id": "ingress001",
      "last_online_time": "2025-01-15T10:30:00Z",
      "last_offline_time": "2025-01-15T11:30:00Z",
      "link_duration": 3600,
      "created_at": "2025-01-10T08:00:00Z"
    }
  ]
}
```

**响应字段说明：**

| 字段 | 类型 | 描述 |
|------|------|------|
| code | integer | 返回码，0 表示成功 |
| message | string | 返回消息 |
| total | integer | 总边缘数 |
| page | integer | 当前页码 |
| page_size | integer | 每页数量 |
| edges | array | 边缘列表 |
| edges[].edge_id | string | 边缘唯一标识符 |
| edges[].edge_type | string | 边缘类型 |
| edges[].local_ip | string | 边缘本地 IP 地址 |
| edges[].public_ip | string | 边缘公网 IP 地址 |
| edges[].status | integer | 边缘状态：0=离线，1=在线 |
| edges[].confirmed | integer | 确认状态：99=待确认，1=已确认，0=已拒绝（黑名单） |
| edges[].ingress_id | string | 所属 ingress ID |
| edges[].last_online_time | string | 最后上线时间（UTC ISO 8601 格式，带 Z） |
| edges[].last_offline_time | string | 最后离线时间（UTC ISO 8601 格式，带 Z） |
| edges[].link_duration | integer | 链接时长（秒，动态计算）= 当前时间 - last_online_time，离线时为 0 |
| edges[].created_at | string | 创建时间（UTC ISO 8601 格式，带 Z） |

---

### POST /api/v1/edges/get - 查询边缘详情

查询指定边缘的详细信息。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘 ID |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/get \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success",
  "edge_id": "edge001",
  "edge_type": "touch",
  "local_ip": "192.168.1.100",
  "public_ip": "203.0.113.45",
  "status": 1,
  "confirmed": 1,
  "ingress_id": "ingress001",
  "last_online_time": "2025-01-15T10:30:00Z",
  "last_offline_time": "2025-01-15T11:30:00Z",
  "link_duration": 3600,
  "created_at": "2025-01-10T08:00:00Z"
}
```

---

### POST /api/v1/edges/confirm - 确认边缘

确认待处理边缘（加入白名单）。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘 ID |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/confirm \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success"
}
```

---

### POST /api/v1/edges/reject - 拒绝边缘

拒绝边缘（加入黑名单）。边缘如果在线将被踢掉。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘 ID |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/reject \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success"
}
```

---

### POST /api/v1/edges/delete - 删除边缘

从数据库中删除边缘记录。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘 ID |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/delete \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success"
}
```

---

### POST /api/v1/edges/batch-confirm - 批量确认边缘

批量确认多个边缘。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_ids | array | M | 边缘 ID 列表 |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/batch-confirm \
  -H "Content-Type: application/json" \
  -d '{"edge_ids": ["edge001", "edge002", "edge003"]}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success",
  "confirmed_count": 3
}
```

---

### POST /api/v1/edges/batch-reject - 批量拒绝边缘

批量拒绝多个边缘。边缘如果在线将被踢掉。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_ids | array | M | 边缘 ID 列表 |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/edges/batch-reject \
  -H "Content-Type: application/json" \
  -d '{"edge_ids": ["edge001", "edge002", "edge003"]}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success",
  "rejected_count": 3
}
```

---

## Ingress 管理接口

### POST /api/v1/ingresses/list - 查询 Ingress 列表

查询所有 ingress 实例的信息。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| 无 | - | - | 空对象即可 |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/ingresses/list \
  -H "Content-Type: application/json" \
  -d '{}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success",
  "ingresses": [
    {
      "ingress_id": "ingress001",
      "host": "192.168.1.100",
      "port": 54321,
      "status": 1,
      "enabled": true,
      "created_at": "2025-01-10T08:00:00Z"
    }
  ]
}
```

---

### POST /api/v1/ingresses/add - 添加 Ingress

添加新的 ingress 配置。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| ingress_id | string | M | Ingress 唯一标识符 |
| host | string | M | Ingress 主机地址 |
| port | integer | M | Ingress 端口号 |
| enabled | boolean | O | 是否启用，默认 true |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/ingresses/add \
  -H "Content-Type: application/json" \
  -d '{
    "ingress_id": "ingress002",
    "host": "192.168.1.101",
    "port": 54321,
    "enabled": true
  }'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success"
}
```

---

### POST /api/v1/ingresses/update - 更新 Ingress

更新 ingress 配置。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| ingress_id | string | M | Ingress ID |
| host | string | O | Ingress 主机地址 |
| port | integer | O | Ingress 端口号 |
| enabled | boolean | O | 是否启用 |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/ingresses/update \
  -H "Content-Type: application/json" \
  -d '{
    "ingress_id": "ingress001",
    "host": "192.168.1.101",
    "port": 54321,
    "enabled": false
  }'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success"
}
```

---

### POST /api/v1/ingresses/delete - 删除 Ingress

删除 ingress 配置。

**请求参数（JSON 消息体）：**

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| ingress_id | string | M | Ingress ID |

**请求示例：**
```bash
curl -X POST http://localhost:18051/api/v1/ingresses/delete \
  -H "Content-Type: application/json" \
  -d '{"ingress_id": "ingress001"}'
```

**响应示例：**
```json
{
  "code": 0,
  "message": "success"
}
```

---

# 返回码说明

| 返回码 | 说明 |
|--------|------|
| 0 | 成功 |
| 40001 | 边缘不允许上线（验证失败或黑名单） |
| 40002 | 边缘不存在 |
| 40003 | 边缘密钥无效 |
| 40004 | Ingress 不存在 |
| 40005 | Ingress 不在线 |
| 40006 | 参数错误 |
| 50001 | 内部错误 |
| 50002 | 数据库错误 |
| 50003 | 服务不可用 |

**错误响应示例：**
```json
{
  "code": 40002,
  "message": "Edge not found"
}
```

---

# 接口设计原则

## 1. 统一使用 POST 方法

**优势**：
- 简化客户端实现（只需处理一种 HTTP 方法）
- 参数在消息体中，不受 URL 长度限制
- 更安全，参数不在 URL 中暴露
- 易于扩展和版本控制

## 2. 参数放在消息体中

**优势**：
- 支持复杂数据结构（嵌套对象、数组等）
- 参数类型更清晰（JSON 类型）
- 易于文档化和自动化测试
- 支持可选参数和默认值

## 3. URL 简洁清晰

**格式**：`/api/v1/{resource}/{action}`

**示例**：
- `/api/v1/edges/list` - 查询边缘列表
- `/api/v1/edges/get` - 查询边缘详情
- `/api/v1/edges/confirm` - 确认边缘
- `/api/v1/edges/delete` - 删除边缘

---

# 安全认证

## 边缘认证

- **边缘注册**：使用 `edge_id` 和 `edge_key` 进行身份验证
- **Token 认证**：边缘注册成功后获得 `access_token`，用于与 ingress 建立 WebSocket 连接

## Web UI 认证

- **认证方式**：待实现（计划使用 JWT Token 或 Session）
- **授权机制**：基于角色的访问控制（RBAC）
- **Token 管理**：Token 有效期和刷新机制待实现

---

# 版本管理

- **版本号规则**：使用 `/api/v1` 作为当前版本路径
- **兼容性策略**：
  - 向后兼容：新版本保持旧版本接口可用
  - 废弃策略：废弃接口提前 3 个月通知
  - 版本升级：通过路径版本号进行版本控制

---

## WebSocket 接口规范（come.1 JSON-RPC 2.0）

**详见**: [spec-api-websocket-jsonrpc.md](spec-api-websocket-jsonrpc.md) - WebSocket 协议详情

### 传输方式

- **协议**: WebSocket (come.1 JSON-RPC 2.0)
- **数据格式**: JSON-RPC 2.0
- **字符编码**: UTF-8

### Edge ↔ Ingress 通信

| 消息类型 | 方法名 | 方向 | 说明 |
|----------|--------|------|------|
| 边缘上线 | `edge.online` | Edge → Ingress | 边缘注册/上线请求 |
| 边缘上线应答 | `AckEdgeOnline` | Ingress → Edge | 边缘上线响应 |
| 边缘心跳 | `edge.heartbeat` | Edge → Ingress | 心跳消息 |
| 边缘下线 | `edge.offline` | Edge → Ingress | 边缘断开通知 |

### Ingress ↔ Manager 通信

| 消息类型 | 方法名 | 方向 | 说明 |
|----------|--------|------|------|
| 边缘上线通知 | `ingress.edge.online` | Ingress → Manager | Ingress 通知 Manager 边缘上线 |
| 边缘下线通知 | `ingress.edge.offline` | Ingress → Manager | Ingress 通知 Manager 边缘下线 |
| 踢边缘命令 | `manager.edge.kick` | Manager → Ingress | Manager 命令 Ingress 踢掉边缘 |
| 查询边缘列表 | `manager.edge.list` | Manager → Ingress | Manager 查询 Ingress 上的边缘列表 |

### JSON-RPC 2.0 消息格式

**请求格式**：
```json
{
  "jsonrpc": "2.0",
  "method": "edge.online",
  "params": {
    "edge_id": "edge001",
    "edge_type": "touch",
    "edge_key": "key001",
    "nonce": "xxx",
    "token": ""
  },
  "id": 12345
}
```

**响应格式**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "success": true,
    "access_token": "xxx",
    "expires_in": 3600
  },
  "id": 12345
}
```

**通知格式**（无 id，不需要响应）：
```json
{
  "jsonrpc": "2.0",
  "method": "ingress.edge.offline",
  "params": {
    "edge_id": "edge001"
  }
}
```

---

# 参考文档

- [spec-design.md](spec-design.md) - 架构设计文档
- [spec-api-websocket-jsonrpc.md](spec-api-websocket-jsonrpc.md) - WebSocket JSON-RPC 2.0 协议详情
- [JSON-RPC 2.0 Specification](https://www.jsonrpc.org/specification)
- [REST API Design Best Practices](https://restfulapi.net/)
