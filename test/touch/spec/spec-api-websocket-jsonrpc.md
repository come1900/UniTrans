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
    "sign": ""
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
| sign | string | O | 校验数据（签名）：根据 `key`、`id`、`nonce` 等计算得出（原 `token`，语义为上线认证校验数据） |

**成功响应**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "success": true,
    "touch_token": "9f2b4d8a1c3e6f0a2b5c7d9e1f3a4b6c8d0e2f4a6b8c0d2e4f6a8b0c2d4e6f8a",
    "token_type": "Bearer",
    "expires_in": 3600
  },
  "id": 1
}
```

> **Edge 段会话 token**：`touch_token` 由 Ingress 在 `edge.online` 时签发（为 `edge_id` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex 摘要），Edge 保存并于本段后续消息携带（发送格式见 `edge.heartbeat`）。

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
    "touch_token": "9f2b4d8a1c3e6f0a2b5c7d9e1f3a4b6c8d0e2f4a6b8c0d2e4f6a8b0c2d4e6f8a",
    "timestamp": "2026-03-01T01:27:23Z"
  }
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| id | string | M | 边缘ID |
| touch_token | string | O | 本段（Edge↔Ingress）会话 token：edge.online 时 Ingress 签发、Edge 持有并回传 |
| timestamp | string | O | 心跳时间戳（ISO 8601） |

**说明**：
- 心跳是通知消息，Ingress 不返回响应
- 默认间隔：30 秒
- Ingress 在本地更新心跳时间，不通知 Manager

---

## Manager ↔ Ingress 通信

### 1. manager.connect - Manager 连接（请求 / Ingress 签发 Manager 段 token）

Manager 连接 Ingress 时发送连接请求（带 `id`），Ingress 递增其持有号并**签发 Manager 段 `touch_token`** 返回。**这是请求-响应消息**。

**请求**：
```json
{
  "jsonrpc": "2.0",
  "method": "manager.connect",
  "params": {
    "manager_id": "touch_manager",
    "ingress_id": "ingress1",
    "timestamp": "2026-03-01T01:27:23Z"
  },
  "id": 1
}
```

**参数说明**：

| 参数 | 类型 | 可选 | 描述 |
|------|------|------|------|
| manager_id | string | M | Manager 实例ID |
| ingress_id | string | O | Ingress 实例ID |
| timestamp | string | O | 连接时间戳 |
| id | number | M | JSON-RPC 2.0 请求 ID（Ingress 用其关联返回 token） |

**成功响应**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "code": 0,
    "message": "Manager connected",
    "manager_id": "touch_manager",
    "touch_token": "3c7e9b1d4f6a8c2e5f7a9b1c3d5e7f9ab1c3d5e7f9ab1c3d5e7f9ab1c3d5e7f9",
    "token_type": "Bearer",
    "expires_in": 3600
  },
  "id": 1
}
```

> **Manager 段会话 token**：`touch_token` 由 Ingress 在 `manager.connect` 时签发（为 `"mgr"` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex 摘要），Manager 保存并于本段（Manager↔Ingress）后续消息携带（见 `edge.config.update`）。

**说明**：
- Manager 启动时主动连接 Ingress（带 `id` 的请求），成功后保存 Ingress 签发的 `touch_token`。
- **分段解耦**：Manager 段 token 与 Edge 段 token 值不同、互不共享（都由对 `prefix` 字节 + 时间计数字节 + 8 字节随机数做 SHA256 生成）；Ingress 为唯一签发方 + 分段网关。
- Ingress 记录 Manager 连接状态；已有 Manager 连接时拒绝新 Manager。

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

## frpc 远程配置通信（Edge 配置）

> frpc 远程配置专题的 WebSocket 协议方法。配置消息由 **Manager 发起、经 Ingress 透传、由 Edge 处理**；Edge 路径：`edge.config.update`（下发）与 `edge.config.query`（查询）。请求为带 `id` 的 JSON-RPC 请求，响应以 `id` 关联。
>
> frpc 配置文件路径：`./shpc/shpc.<edge_id>.json`（相对 Edge 工作目录，`CFunRegisterCli::get_frpc_config_path()`）。

### edge.config.update - 配置下发（请求 / AckConfigUpdate 响应）

Manager 构造 `edge.config.update` 请求，经 Ingress 透传给 Edge；Edge 解析为 `ConfigUpdate_tunnelService`，转换为 `FrpcConfig` 写入本地文件，并回复 `AckConfigUpdate`。

**请求（Manager → Ingress → Edge）**：
```json
{
  "jsonrpc": "2.0",
  "method": "edge.config.update",
  "edge_id": "edge001",
  "config_type": "tunnelService",
  "version": 2,
  "touch_token": "3c7e9b1d4f6a8c2e5f7a9b1c3d5e7f9ab1c3d5e7f9ab1c3d5e7f9ab1c3d5e7f9",
  "config_content": [
    {
      "serviceName": "tunnelService",
      "tunnelService": {
        "version": "1.0",
        "endpoint": {"host": "10.220.42.139", "port": 50400},
        "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
        "security": {"authMethod": "token", "credential": "test-token", "enableTls": true}
      },
      "accessPolicies": [
        {"policyId": "w-tcp-51422", "protocol": "tcp",
         "exposedPort": 51422, "description": "pss service",
         "targetService": {"ip": "127.0.0.1", "port": 55555}}
      ]
    }
  ],
  "id": 100
}
```

**字段说明**：
- `edge_id`、`config_type`、`touch_token` 为**顶层字段**
- `touch_token`（**分段点对点**）：Manager 侧在 **Manager↔Ingress 段** 携带其持有的 Manager 段 token（`"mgr"` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex，manager.connect 时 Ingress 签发）；Ingress 收到后**校验**该值等于其为该 Manager 签发的 token（不符则拒绝），并在透传前**换成**目标 Edge 的 **Edge 段 token**（`edge_id` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex）再转发给 Edge。故 Edge 收到的 `touch_token` 是 Edge 段 token，与 Manager 段值不同。
- `version`（**顶层**，`int32_t`）：**Manager ↔ Ingress ↔ Edge 三个组件通讯**用的配置版本号。由 Manager 端数据库 `edge_configs.version` 在互斥锁内原子 +1 生成；Edge 回复 `AckConfigUpdate` 时回显同名顶层 `version`（`AckConfigUpdate.version`，亦为 `int32_t`），Manager 据此配对确认。**界面展示的 `(version=1)` 即指该值**。
  > ⚠ 与 `config_content` 内的 `tunnelService.version`（`“1.0”`，`std::string`）是**两个不同的字段**：后者是给 **Edge 自己用**的配置体版本标注，随配置体写盘，与顶层版本号无关，二者当前互不传递。
- `config_content` 是**数组**，其元素为对象：`serviceName` + `tunnelService` + **`accessPolicies`（与 `tunnelService` 同级）**
- `id`：JSON-RPC 请求 ID，用于关联 Edge 的确认响应

**Edge 处理**：
1. `ComeJsonCodec::decode` 解析为 `ConfigUpdate_tunnelService`
2. `toFrpcConfig()` 转换为 `FrpcConfig`
3. 写入 `./shpc/shpc.<edge_id>.json`
4. 回复 `AckConfigUpdate`

**响应（AckConfigUpdate，Edge 写盘成功）**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "code": 0,
    "message": "success",
    "applied": true,
    "edge_id": "edge001"
  },
  "id": 100
}
```

**Manager 侧确认逻辑**（`ingress_client` 收到 Ack 后）：
- `result.applied == true` → 置 `edge.config_status = 'confirmed'`，并发起一致性核对（`config_sync=matched`）
- `applied == false` → 仅转发确认，继续等待 Edge 的最终落盘确认

**写盘失败**：Edge 回复 `AckConfigUpdate` 且 `applied=false`/`code!=0`，Manager 置 `config_status = 'config_failed'`。

---

### edge.config.query - 配置查询（请求 / 响应）

Manager 查询 Edge 本地 frpc 配置，经 Ingress 透传，Edge 读取文件后返回。

**请求（Manager → Ingress → Edge）**：
```json
{
  "jsonrpc": "2.0",
  "method": "edge.config.query",
  "params": {"edge_id": "edge001"},
  "id": 200
}
```

**响应（成功，文件存在且可解析）**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config": {
      "serverAddr": "10.220.42.139",
      "serverPort": 50400,
      "auth": {"method": "token", "token": "test-token"},
      "transport": {"tls": {"enable": true}},
      "webServer": {"addr": "127.0.0.1", "port": 17400},
      "proxies": [
        {"name": "w-tcp-51422", "type": "tcp",
         "localIP": "127.0.0.1", "localPort": 55555, "remotePort": 51422}
      ]
    }
  },
  "id": 200
}
```

**`result.config`**：Edge 直接返回本地 frpc 配置 JSON（嵌套权威结构）。

**响应（文件存在但解析失败）**：
```json
{
  "jsonrpc": "2.0",
  "result": {
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config_error": "Failed to parse config file"
  },
  "id": 200
}
```
Manager 判 `config_sync = 'bad'` / `sync_state = 'bad'`。

**响应（文件不存在/打不开，顶层 JSON-RPC error）**：
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": -1,
    "message": "Failed to read config file"
  },
  "id": 200
}
```
Manager 判 `config_sync = 'missing'` / `sync_state = 'missing'`。

> **一致性判定（Manager）**：`edge.config.query` 的响应中，**必须在 `result` 与顶层 `error` 两处都解析**——顶层 `error`（文件缺失）与 `result.config_error`（解析失败）分别对应 `missing` 与 `bad`，不可混淆（历史上 format-error 曾被误判为 missing）。

---

## 消息流程示例

### 边缘上线流程

```text
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

