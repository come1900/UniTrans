# Touch WebSocket 协议规范 (JSON-RPC 2.0)

## 概述

本文档定义 Touch 系统中所有通过 WebSocket 通信的接口，遵循 **JSON-RPC 2.0** 规范。

### JSON-RPC 2.0 规范要点

- **请求格式**：
  ```json
  {
    "jsonrpc": "2.0",
    "method": "method_name",
    "params": {...},
    "id": 1
  }
  ```

- **响应格式**（成功）：
  ```json
  {
    "jsonrpc": "2.0",
    "result": {...},
    "id": 1
  }
  ```

- **响应格式**（错误）：
  ```json
  {
    "jsonrpc": "2.0",
    "error": {
      "code": -32600,
      "message": "Invalid Request"
    },
    "id": 1
  }
  ```

- **通知格式**（无响应）：
  ```json
  {
    "jsonrpc": "2.0",
    "method": "method_name",
    "params": {...}
  }
  ```
  注意：通知消息**不包含 `id` 字段**，服务器不会返回响应。

### 错误码定义

**JSON-RPC 2.0 规范预定义错误码**（必须按规范使用）：
- `-32700`: Parse error（解析错误）- JSON 格式错误
- `-32600`: Invalid Request（无效请求）- 请求格式不符合规范
- `-32601`: Method not found（方法不存在）- 请求的方法不存在
- `-32602`: Invalid params（无效参数）- 参数类型或值错误
- `-32603`: Internal error（内部错误）- 服务器内部处理错误
- `-32000` to `-32099`: Server error（服务器错误）- 保留用于实现定义

**业务错误码**（使用正整数，建议从 40000 开始）：
- `40001`: 边缘验证失败
- `40002`: Manager 未连接
- `40003`: 边缘已连接（重复注册）
- `40004`: 边缘被拒绝（黑名单）


---

## Edge ↔ Ingress 通信

### 1. edge.online - 边缘上线（请求/响应）

边缘首次连接 Ingress 时发送上线请求。

**请求**：
```json
{
  "jsonrpc": "2.0",
  "method": "edge.online",
  "params": {
    "id": "edge001",
    "key": "key001",
    "type": "touch",
    "nonce": "random_nonce_string",
    "token": ""
  },
  "id": 1
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| id | string | M | 边缘唯一标识 |
| key | string | M | 边缘认证密钥 |
| type | string | O | 边缘类型 |
| nonce | string | O | 随机数，用于OAuth2风格认证 |
| token | string | O | 从 Manager 获取的认证 token（用于与 Ingress 认证） |

**成功响应**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "success": true,
    "access_token": "ingress_generated_token",
    "token_type": "Bearer",
    "expires_in": 3600
  },
  "id": 1
}
```

**错误响应**：
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": 40001,
    "message": "Device verification failed"
  },
  "id": 1
}
```

**错误码**：

**JSON-RPC 2.0 规范预定义错误码**：
- `-32602`: Invalid params（无效参数）- 当 device_id 或 device_key 为空时
- `-32603`: Internal error（内部错误）- 服务器内部处理错误

**业务错误码（正整数）**：
- `40001`: 边缘验证失败
- `40002`: Manager 未连接
- `40003`: 边缘已连接（重复注册）
- `40004`: 边缘被拒绝（黑名单）

---

### 2. edge.heartbeat - 心跳（通知）

边缘定时发送心跳，保持连接活跃。**这是通知消息，无响应**。

**通知**：
```json
{
  "jsonrpc": "2.0",
  "method": "edge.heartbeat",
  "params": {
    "id": "edge001",
    "access_token": "token_from_registration",
    "timestamp": "2026-03-01T01:27:23Z"
  }
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| id | string | M | 边缘ID |
| access_token | string | O | 注册时获取的访问令牌 |
| timestamp | string | O | 心跳时间戳（ISO 8601） |

**说明**：
- 心跳是通知消息，Ingress 不返回响应
- 默认间隔：30 秒
- Ingress 在本地更新心跳时间，不通知 Manager

---

## Manager ↔ Ingress 通信

### 1. manager.connect - Manager 连接（通知）

Manager 连接 Ingress 时发送连接通知。**这是通知消息，无响应**。

**通知**：
```json
{
  "jsonrpc": "2.0",
  "method": "manager.connect",
  "params": {
    "manager_id": "touch_manager",
    "ingress_id": "ingress1",
    "timestamp": "2026-03-01T01:27:23Z"
  }
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| manager_id | string | M | Manager 实例ID |
| ingress_id | string | O | Ingress 实例ID |
| timestamp | string | O | 连接时间戳 |

**说明**：
- Manager 启动时主动连接 Ingress
- 连接成功后发送此通知
- Ingress 记录 Manager 连接状态

---

### 2. ingress.edge.online - 边缘上线通知（通知）

Ingress 通知 Manager 边缘已上线。**这是通知消息，无响应**。

**通知**：
```json
{
  "jsonrpc": "2.0",
  "method": "ingress.edge.online",
  "params": {
    "edge_id": "edge001",
    "ingress_id": "ingress1",
    "online_since": 1709251200
  }
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘ID |
| ingress_id | string | O | Ingress 实例ID |
| online_since | integer | O | 上线时间（UTC Unix 时间戳，秒） |

**说明**：
- Ingress 在边缘成功上线后发送
- Manager 收到后通过 HTTP API 更新边缘状态

---

### 3. edge.offline - 边缘下线通知（通知）

Ingress 通知 Manager 边缘已下线。**这是通知消息，无响应**。

**通知**：
```json
{
  "jsonrpc": "2.0",
  "method": "ingress.edge.offline",
  "params": {
    "edge_id": "edge001",
    "ingress_id": "ingress1"
  }
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘ID |
| ingress_id | string | O | Ingress 实例ID |

**说明**：
- Ingress 在边缘断开连接时发送
- Manager 收到后通过 HTTP API 更新边缘状态为离线

---

### 4. manager.edge.list - 查询边缘列表（请求/响应）

Manager 查询 Ingress 上的边缘列表。

**请求**：
```json
{
  "jsonrpc": "2.0",
  "method": "manager.edge.list",
  "params": {
    "offset": 0,
    "limit": 50
  },
  "id": 1
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| offset | integer | O | 偏移量，默认 0 |
| limit | integer | O | 每页数量，默认 50，最大 1000 |

**成功响应**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "devices": [
      {
        "edge_id": "edge001",
        "client_id": 1,
        "online_since": 1709251200,
        "last_heartbeat": 1709251500
      },
      {
        "edge_id": "edge002",
        "client_id": 2,
        "online_since": 1709251300,
        "last_heartbeat": 1709251600
      }
    ],
    "total": 2,
    "offset": 0,
    "limit": 50,
    "count": 2
  },
  "id": 1
}
```

**响应字段说明**：

| 字段 | 类型 | 描述 |
|------|------|------|
| devices | array | 边缘对象数组，每个对象包含 edge_id、client_id、online_since、last_heartbeat |
| total | integer | 总边缘数 |
| offset | integer | 请求的偏移量 |
| limit | integer | 请求的每页数量 |
| count | integer | 返回的边缘数量 |

**错误响应**：
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": -32603,
    "message": "Internal error"
  },
  "id": 1
}
```

---

### 5. manager.edge.kick - 踢边缘下线（请求/响应）

Manager 强制边缘下线。

**请求**：
```json
{
  "jsonrpc": "2.0",
  "method": "manager.edge.kick",
  "params": {
    "edge_id": "edge001",
    "reason": "Edge rejected by admin"
  },
  "id": 1
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| edge_id | string | M | 边缘ID |
| reason | string | O | 踢下线原因 |

**成功响应**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "success": true,
    "edge_id": "edge001",
    "message": "Edge kicked successfully"
  },
  "id": 1
}
```

**错误响应**：
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": -32602,
    "message": "Invalid params",
    "data": {
      "details": "edge_id is required"
    }
  },
  "id": 1
}
```

**错误码**：
- `-32602`: Invalid params（无效参数）- 当 edge_id 为空时
- `-32603`: Internal error（内部错误）- 服务器内部处理错误

---

### 6. manager.edge.offline.list - 查询离线边缘列表（请求/响应）

Manager 查询 Ingress 上缓存的离线边缘列表。

**请求**：
```json
{
  "jsonrpc": "2.0",
  "method": "manager.edge.offline.list",
  "params": {
    "offset": 0,
    "limit": 50
  },
  "id": 1
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| offset | integer | O | 偏移量，默认 0 |
| limit | integer | O | 每页数量，默认 50，最大 1000 |

**成功响应**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "offline_devices": [
      {
        "edge_id": "edge001",
        "offline_time": 1709251200,
        "pending_confirm": false,
        "edge_type": "touch"
      },
      {
        "edge_id": "edge002",
        "offline_time": 1709251300,
        "pending_confirm": true,
        "edge_type": "touch"
      }
    ],
    "total": 2,
    "offset": 0,
    "limit": 50,
    "count": 2
  },
  "id": 1
}
```

**响应字段说明**：

| 字段 | 类型 | 描述 |
|------|------|------|
| offline_devices | array | 离线边缘对象数组 |
| offline_devices[].edge_id | string | 边缘 ID |
| offline_devices[].offline_time | integer | 离线时间（UTC Unix 时间戳，秒） |
| offline_devices[].pending_confirm | boolean | 是否等待 Manager 确认 |
| offline_devices[].edge_type | string | 边缘类型 |
| total | integer | 总离线边缘数 |
| offset | integer | 请求的偏移量 |
| limit | integer | 请求的每页数量 |
| count | integer | 返回的离线边缘数量 |

**错误响应**：
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": -32603,
    "message": "Internal error"
  },
  "id": 1
}
```

---

## 消息流程示例

### 边缘上线流程

```
1. Edge → Ingress: edge.online (请求)
   ↓
2. Ingress 验证边缘，通知 Manager
   ↓
3. Ingress → Manager: ingress.edge.online (请求，等待响应)
   ↓
4. Manager 返回 confirmed 状态
   ↓
5. Ingress → Edge: edge.online (响应，success=true)
   ↓
6. Ingress → Manager: ingress.edge.online (通知，边缘已上线)
   ↓
7. Edge 启动心跳定时器
```

### 心跳流程

```
1. Edge → Ingress: edge.heartbeat (通知)
   ↓
2. Ingress 更新本地心跳时间（不通知 Manager）
```

### 边缘下线流程

```
1. Edge 断开连接
   ↓
2. Ingress → Manager: ingress.edge.offline (通知)
   ↓
3. Manager 更新边缘状态为离线（HTTP API）
```

### Manager 查询边缘列表

```
1. Manager → Ingress: manager.edge.list (请求)
   ↓
2. Ingress → Manager: manager.edge.list (响应，包含边缘对象列表)
```

### Manager 踢边缘下线

```
1. Manager → Ingress: manager.edge.kick (请求)
   ↓
2. Ingress 关闭边缘连接，发送拒绝消息给边缘
   ↓
3. Ingress → Manager: manager.edge.kick (响应，success=true)
   ↓
4. Ingress → Manager: ingress.edge.offline (通知)
```

---

## 兼容性说明

### 向后兼容

- 新协议支持旧格式消息（通过 `jsonrpc` 字段判断）
- 旧格式消息自动转换为 JSON-RPC 2.0 格式处理

### 迁移策略

1. **第一阶段**：同时支持旧格式和 JSON-RPC 2.0 格式
2. **第二阶段**：逐步迁移到 JSON-RPC 2.0
3. **第三阶段**：移除旧格式支持（提前通知）

---

## 实现建议

### 消息解析

1. 检查消息是否包含 `jsonrpc: "2.0"` 字段
2. 如果包含，按 JSON-RPC 2.0 规范解析
3. 如果不包含，按旧格式解析（兼容模式）

### 错误处理

- 所有错误必须符合 JSON-RPC 2.0 错误格式
- **JSON-RPC 2.0 规范预定义错误码**（`-32700` 到 `-32603`，`-32000` 到 `-32099`）必须按规范使用
- **业务错误码**使用正整数（建议从 `40000` 开始），如 `40001`、`40002` 等

### 通知消息

- 通知消息不包含 `id` 字段
- 服务器不返回响应
- 客户端不应等待响应

---

## 参考

- [JSON-RPC 2.0 Specification](https://www.jsonrpc.org/specification)
- [JSON-RPC 2.0 Examples](https://www.jsonrpc.org/specification#examples)

