
# 功能/目的

支持万级前端（嵌入式边缘）接入，接入层可弹性伸缩，管理服务专注业务与 UI。

三层架构设计

## 架构图

```text
                                    +-----------------------+
                                    | Web UI / 测试工具     |
                                    +-----------------------+
                                             ▲
                                             │ REST API (HTTP/JSON)
                                             │
+------------------+               +------------------+
| touchEdge        |               | Touch Manager    |
| (C++)            |               | (Python, 正式)    |
+------------------+               +------------------+
        │                                ▲
        │                                │
        │ WebSocket                      │ WebSocket
        │ (come.1 JSON-RPC 2.0)          │ (come.1 JSON-RPC 2.0)
        │                                │
        └───────────────┬────────────────┘
                        │
                        ▼
               +------------------+
               | touchIngress     |
               | (C++, 正式)       |
               +------------------+
```

**架构说明**：
- **通信模式**：
  - **touchEdge ↔ touchIngress**：WebSocket (come.1 JSON-RPC 2.0)，边缘注册、心跳、业务消息
  - **touchIngress ↔ touchManager**：WebSocket (come.1 JSON-RPC 2.0)，边缘状态通知、管理指令
  - **touchManager ↔ Web UI**：REST API (HTTP/JSON)
  - **touchEdge 与 touchManager 不直接通信**：所有业务通讯都通过 **touchIngress** 转发
- **touchIngress 作为业务纽带**：
  - touchManager 启动后通过 **WebSocket (come.1 JSON-RPC 2.0)** 主动连接 touchIngress
  - touchIngress 的 IP、端口信息通过 touchManager 的 Web UI 配置
  - **边缘状态通知**：
    - 边缘在 ingress 上线成功后，ingress 通过 WebSocket 通知 manager 边缘上线
    - 边缘从 ingress 断开后，ingress 通过 WebSocket 通知 manager 边缘下线
  - **管理指令下发**：
    - manager 通过 WebSocket 向 ingress 发送管理指令（如踢边缘、查询边缘列表等）
    - ingress 执行指令并返回响应
- **touchManager 功能**：
  - 提供 **REST API** 供 Web UI 调用，用于管理和维护整个系统
  - 通过 WebSocket 与 ingress 通信，获取边缘状态和边缘列表
  - 不直接与 edge 通信，所有边缘管理都通过 ingress 转发

**接口规范**：详见 [spec-api.md](spec-api.md) 和 [spec-api-websocket-jsonrpc.md](spec-api-websocket-jsonrpc.md)

---

## 各层职责

### 1. touchEdge（Edge，终端）
- **实现状态**：已实现
- **语言**：C++（libezsocket）
- **功能**：
  - **首次上线/注册**：直接连接 touchIngress 并发送 EdgeOnline 消息
    - 发送边缘信息：edge_id、edge_type、edge_key、nonce 等
    - 接收返回：touch_token、expires_in 等
  - **后续所有通信**：通过 WebSocket 与 touchIngress 通信
    - **定时心跳**：EdgeHeartbeat（包含边缘 ID、时间戳）
    - **心跳应答**：AckEdgeHeartbeat
    - **业务消息**：通过 ingress 转发
  - **失败重试机制**：
    - **WebSocket 断开重连机制**：
      - WebSocket 连接断开后，自动尝试重连
      - 重连间隔：3 秒，支持退避策略
- **通信架构**：
  - **与 manager**：不直接通信
  - **与 ingress**：所有通信都通过 WebSocket (come.1 JSON-RPC 2.0)
- **特性**：
  - 基于 ezThread 自驱动实体
  - 使用 come.1 JSON 协议进行消息交换（与 ingress 通信）
  - 资源占用低，适合嵌入式边缘
- **依赖库**：
  - libezsocket（WebSocket 通信、HTTP 客户端功能）
  - libezThread（线程管理）
  - come.1（协议解析和封装库，详见下方"come.1 库设计"章节）

### 2. touchIngress（Ingress Service，接入层）
- **实现状态**：已实现
- **语言**：C++（libezsocket）
- **无状态**：不存储边缘数据
- **核心能力**：
  - **作为业务纽带**：touchIngress 是 edge 和 manager 之间的业务纽带，负责转发所有业务消息
  - **与 manager 的连接**：touchManager 启动后主动连接 touchIngress（WebSocket (come.1 JSON-RPC 2.0)）
  - **与 edge 的连接**：维护 WebSocket 连接（服务端），接收 touchEdge 的连接
  - **边缘注册处理**：
    - 处理边缘注册请求（EdgeOnline）
    - 通过 WebSocket 向 manager 发送 edge.online 请求（带 id，需要等待响应）
    - 根据 manager 返回的 confirmed 状态决定是否接受边缘：
      - confirmed=99 或 1：接受边缘上线
      - confirmed=0（黑名单）：拒绝边缘上线并关闭连接
    - 如果没有 manager 连接，拒绝边缘上线
  - **边缘状态通知**：
    - 边缘在 ingress 上线成功后，通过 WebSocket 通知 manager 边缘上线
    - 边缘从 ingress 断开后，通过 WebSocket 通知 manager 边缘下线
    - **Edge 上报模式**（v0.2 新增）：
      - 通过 `EdgeReportMode` 配置项控制 Manager 连接后的上报行为
      - `0` - 不上报：Manager 连接后不上报任何 Edge
      - `1` - 上报所有（默认）：Manager 连接后上报所有已连接的 Edge
      - `2` - 增量上报：Manager 连接后仅上报有变化的 Edge
    - **Edge 离线缓存**（v0.2 新增）：
      - 当没有 Manager 连接时，Ingress 会记录下线的 Edge
      - Manager 上线后根据上报模式上报这些 Edge 的离线状态
      - 确保 Manager 能够获取完整的 Edge 状态变化历史，避免状态丢失
  - **管理指令处理**：
    - 接收 manager 通过 WebSocket 发送的管理指令（如 manager.edge.kick、manager.edge.list 等）
    - 执行指令并返回响应
- **通信架构**：
  - **与 manager**：manager 主动连接，通过 WebSocket (come.1 JSON-RPC 2.0) 双向通信
  - **与 edge**：edge 连接 ingress，通过 WebSocket (come.1 JSON-RPC 2.0) 双向通信
  - **业务转发**：所有 edge 和 manager 之间的业务消息都通过 ingress 转发
- **特性**：
  - 基于 ezThread 自驱动实体
  - 使用 come.1 JSON 协议
  - 可部署多实例 + 负载均衡
  - 水平扩展：加机器即可扩容连接数
  - IP 和端口信息通过 touchManager 的配置页面配置
- **依赖库**：
  - libezsocket（WebSocket 通信）
  - libezThread（线程管理）
  - libezutil（工具库）
  - come.1（协议解析和封装库，详见下方"come.1 库设计"章节）

### 3. touchManager（Management Service，管理服务）
- **实现状态**：已实现（v0.1 MVP）
- **语言**：Python
- **有状态**：维护边缘注册表、配置版本、用户操作记录、ingress 服务配置表、边缘历史连接记录
- **通信架构**：
  - **与 edge**：不直接通信
  - **与 ingress**：启动后主动连接所有配置的 touchIngress 实例（WebSocket (come.1 JSON-RPC 2.0)）
- **功能模块**：
  - **边缘注册中心**：
    - 通过 WebSocket 接收 ingress 转发的边缘上线请求（edge.online）
    - 查询数据库，返回 confirmed 状态（99=pending, 1=confirmed, 0=rejected）
    - 如果边缘在黑名单中（confirmed=0），发送 manager.edge.kick 命令给 ingress
  - **边缘状态管理**：
    - 通过 WebSocket 接收 ingress 的边缘上线/下线通知
    - 直接更新数据库中的边缘状态
    - 提供 REST API 供 Web UI 查询边缘状态
  - **配置引擎**：生成 frpc.toml，支持模板变量（如 `${edge_id}`）
  - **命令下发队列**：对离线边缘缓存指令
  - **Web 控制台**：
    - **touchIngress 配置管理**：
      - IP、端口等信息配置
      - 管理 ingress 上下线（是否提供服务）
      - 查看 ingress 状态和负载情况
    - **edge 边缘管理**：
      - edge 的 id、IP、类别等信息管理
      - 边缘权限设置（允许/禁止上线）
      - 展示全局的 edge 在线状态
      - 边缘列表（在线/离线、最后心跳）
    - **配置管理**：
      - 可视化编辑 proxy 配置（remote_port, local_port 等）
      - 向 edge 下发配置
    - **批量操作**：
      - 批量操作（如"全部重启 SSH 代理"）
      - 批量配置下发
  - **REST API**：
    - 供 WebUI 调用
    - 支持自测和调试
    - 提供边缘管理、配置管理、状态查询等接口
- **计划存储**：SQLite（单机）或 PostgreSQL（集群）
- **与 touchIngress 通信**：WebSocket (come.1 JSON-RPC 2.0)，touchManager 主动连接配置的 touchIngress

---

## 设计原则

### Ingress 单 Manager 连接原则

**核心原则**：每个 touchIngress 实例**只能有一个 Manager 连接**。

- **设计理由**：
  - Ingress 是无状态接入层，不需要处理多个 Manager 的并发控制
  - 单一 Manager 连接简化了状态管理和消息路由逻辑
  - 避免多 Manager 连接导致的消息重复、状态不一致等问题
  - 降低系统复杂度，便于调试和维护

- **实现方式**：
  - Ingress 使用 `m_manager_client_id` 成员变量保存唯一的 Manager 连接 client_id
  - 当新 Manager 发送 `manager.connect` 消息时，Ingress 会检查是否已有 Manager 连接
  - 如果已有 Manager 连接（`m_manager_client_id > 0`），Ingress 会**直接拒绝**新 Manager 连接
  - 所有通知 Manager 的接口（如 `notify_device_online`、`notify_device_offline`）直接使用 `m_manager_client_id`，无需遍历查找

- **Manager 高可用**：
  - 如果需要高可用，应使用主从切换机制，而非多连接
  - 主 Manager 断开后，备用 Manager 可以重新连接
  - Ingress 检测到 Manager 断开后，会缓存离线事件，等待新 Manager 连接后上报

- **测试用例（TC-019）**：
  - **测试目的**：验证 Ingress 只接受一个 Manager 连接，多余的 Manager 会被立即断开
  - **测试步骤**：
    1. 启动 Ingress
    2. 启动 Manager-A，发送 `manager.connect` 消息
    3. 验证 Manager-A 连接成功
    4. 启动 Manager-B，发送 `manager.connect` 消息
    5. 验证 Manager-B 连接成功，Manager-A 被断开
    6. 查询边缘列表，验证只有 Manager-B 能接收数据
  - **预期结果**：
    1. Manager-A 连接成功，`m_manager_client_id` 设置为 Manager-A 的 client_id
    2. Manager-B 连接后，Ingress 断开 Manager-A，`m_manager_client_id` 更新为 Manager-B 的 client_id
    3. Manager-A 收到断开通知，连接关闭
    4. 只有 Manager-B 能接收边缘状态通知和管理指令

### 无数据库设计（Edge 和 Ingress）

**核心原则**：touchEdge 和 touchIngress **不使用数据库**，所有状态数据保存在内存数据结构中。

- **touchEdge**：
  - 无状态设计，仅保存当前连接状态和配置信息
  - 所有数据存储在内存结构体中（如 `EdgeOnline`、`EdgeHeartbeat` 等）
  - 配置信息从配置文件读取，不持久化到数据库

- **touchIngress**：
  - 无状态设计，不持久化任何业务数据
  - 连接状态、设备信息、离线事件缓存等全部保存在内存数据结构中（如 `std::map<int, DeviceInfo>`、`std::map<std::string, EdgeOfflineEvent>`）
  - Manager 下线后，Ingress 缓存的离线事件在进程重启后会丢失
  - 配置信息从配置文件读取，不持久化到数据库

- **touchManager**：
  - **唯一使用数据库的网元**
  - 使用 SQLite（单机）或 PostgreSQL（集群）存储边缘注册表、配置版本、用户操作记录等
  - 负责持久化所有业务状态

**设计优势**：
- Ingress 可弹性伸缩，无状态设计便于水平扩展
- 加机器即可扩容连接数，无需考虑数据同步
- 降低部署复杂度，Ingress 和 Edge 无需依赖数据库服务

### 边缘上线流程
```
1. touchEdge 首次启动
   └─> 连接 touchIngress（WebSocket）
       └─> 发送 EdgeOnline 消息
           └─> 参数：edge_id, edge_key, edge_type, nonce

2. touchIngress 处理注册请求
   ├─> 检查是否有 manager 连接
   │   └─> 如果没有 manager，返回失败并关闭连接
   ├─> 验证边缘身份（edge_id, edge_key）
   ├─> 通过 WebSocket 向 manager 发送 edge.online 请求（JSON-RPC 2.0）
   │   └─> 请求参数：edge_id, edge_type, local_ip, public_ip, online_since
   └─> 等待 manager 响应

3. touchManager 处理 edge.online 请求
   ├─> 查询数据库中的边缘记录
   ├─> 新边缘自动注册（confirmed=99，pending 状态）
   ├─> 已存在边缘检查 confirmed 状态：
   │   ├─> confirmed=0：黑名单
   │   └─> confirmed=1：已确认（白名单）
   │   └─> confirmed=99：待确认
   ├─> 更新边缘状态（status=online, last_online_time, ingress_id）
   ├─> 如果 confirmed=0，发送 manager.edge.kick 命令给 ingress
   └─> 返回响应：{"confirmed": xx}

4. touchIngress 接收 manager 响应
   ├─> confirmed=99 或 1：接受边缘上线
   │   └─> 返回 AckEdgeOnline(code=0, success=true, touch_token, ...)
   └─> confirmed=0：拒绝边缘上线
       └─> 返回 AckEdgeOnline(code!=0, success=false)
           └─> 关闭 WebSocket 连接

5. touchEdge 接收应答
   ├─> code=0：边缘上线成功，开始心跳和业务通讯
   └─> code!=0：边缘上线失败，等待重连
```

### 边缘下线流程
```
1. touchEdge 断开连接（网络异常、主动退出等）
   └─> WebSocket 连接断开

2. touchIngress 检测到断开
   ├─> 从 m_online_devices 中移除该设备
   └─> 将设备信息移动到 m_offline_devices_cache（离线缓存）
       └─> 记录 offline_time（离线时间）
       └─> 设置 pending_offline_confirm = false（等待 Manager 确认）

3. touchIngress 检查 Manager 连接状态
   ├─> Manager 在线：
   │   ├─> 立即通过 WebSocket 发送 ingress.edge.offline 通知
   │   │   └─> 参数：edge_id, ingress_id
   │   └─> 设置 pending_offline_confirm = true（等待确认）
   └─> Manager 不在线：
       └─> 保留在离线缓存中，等待 Manager 连接后上报

4. touchManager 接收 offline 通知
   ├─> 查询数据库中的边缘记录
   ├─> 更新边缘状态：
   │   ├─> status = offline（离线）
   │   ├─> last_offline_time = 当前时间
   │   └─> last_online_time = NULL（清除在线时间）
   └─> 返回确认响应：{"edge_id": "xxx"}

5. touchIngress 接收 Manager 确认
   ├─> 根据 edge_id 查找离线缓存
   ├─> 设置 pending_offline_confirm = false
   └─> 从 m_offline_devices_cache 中删除该设备记录

6. 特殊情况：Manager 连接后上报
   └─> Manager 连接时，touchIngress 遍历 m_offline_devices_cache
       ├─> 对每个 pending_offline_confirm=false 的设备
       │   ├─> 发送 ingress.edge.offline 通知
       │   └─> 设置 pending_offline_confirm = true
       └─> 等待 Manager 逐个确认后删除缓存
```

### 查询离线边缘流程
```
1. touchManager 发送查询请求
   └─> 通过 WebSocket 发送 manager.edge.offline.list 请求
       └─> 参数：offset, limit

2. touchIngress 接收请求
   └─> 遍历 m_offline_devices_cache
       └─> 构建离线设备列表响应
           └─> 包含：edge_id, offline_time, pending_confirm, edge_type

3. touchIngress 返回响应
   └─> 返回 JSON-RPC 2.0 响应
       └─> 包含：offline_devices 数组、total、offset、limit、count

4. touchManager 接收响应
   └─> 解析离线设备列表
       └─> 可用于调试和监控
```

### touchManager 连接 touchIngress 流程

```
1. touchManager 启动
   └─> 读取配置的 touchIngress 列表（通过 Web UI 配置的 IP、端口信息）

2. touchManager 主动连接 touchIngress
   └─> 对每个配置的 touchIngress，建立 WebSocket 连接
       └─> 使用 come.1 JSON 协议进行通信（WebSocket (come.1 JSON)）

3. touchManager 与 touchIngress 建立连接后
   ├─> touchManager 可以接收 touchIngress 的负载状态上报
   ├─> touchManager 可以向 touchIngress 下发管理指令
   └─> touchIngress 定期上报负载状态（连接数、资源使用等）
```

### 配置下发流程（待实现）

```
1. 用户在 Web UI 修改边缘 A 的 remote_port → 提交
   └─> 通过 REST API 调用 touchManager

2. touchManager 处理配置变更
   ├─> 生成新 frpc.toml
   ├─> 查询边缘 A 是否在线
   │   └─> 查询边缘 A 当前连接的 touchIngress
   └─> 若在线 → 通过 WebSocket 通知对应的 touchIngress 实例

3. touchIngress 接收配置更新指令
   └─> 通过已有 WebSocket 连接向 touchEdge 发送 `update_frpc_config`（come.1 JSON 协议）

4. touchEdge 执行 reload 并上报结果
   └─> 通过 touchIngress 转发结果给 touchManager

5. touchManager 记录操作日志 + 配置版本
```

> **离线边缘处理**：若边缘离线，指令存入数据库，待边缘下次上线时通过分配的 touchIngress 下发。
>
> **当前实现**：仅支持边缘注册流程，负载分配和服务发现功能待 touchManager 实现后完成。

---

## 通信协议设计

### Edge ↔ Ingress 通信

- **协议**：WebSocket
- **消息格式**：come.1 JSON-RPC 2.0 协议
- **认证**：使用 edge_key 进行身份认证
- **功能**：
  - 边缘注册（EdgeOnline 请求/响应）
  - 心跳消息（EdgeHeartbeat 通知）
  - 命令下发（配置更新、远程控制等）
  - 状态上报（边缘状态、执行结果等）
- **特点**：长连接，双向通信

### Manager ↔ Ingress 通信

- **协议**：WebSocket (come.1 JSON-RPC 2.0)
- **连接方向**：touchManager 主动连接 touchIngress
- **功能**：
  - **连接建立**：
    - touchManager 启动后主动连接配置的 touchIngress
    - touchIngress 的 IP、端口信息通过 Web UI 配置在 touchManager 中
  - **边缘状态通知**：
    - ingress 通过 WebSocket 发送 edge.online 请求（带 id，需要响应）
    - ingress 通过 WebSocket 发送 edge.offline 通知（无 id，不需要响应）
    - manager 返回 confirmed 状态给 ingress
  - **管理指令下发**：
    - manager 通过 WebSocket 发送 manager.edge.kick 命令
    - manager 通过 WebSocket 发送 manager.edge.list 查询请求
    - ingress 执行指令并返回响应
- **特点**：长连接，双向通信，JSON-RPC 2.0 格式


### Manager ↔ Web UI / 测试工具通信
- **协议**：REST API（HTTP/HTTPS）
- **功能**：
  - **touchIngress 配置管理**：
    - 添加、修改、删除 ingress 配置
    - 管理 ingress 上下线（启用/禁用服务）
    - 查询 ingress 状态和负载情况
  - **edge 边缘管理**：
    - 查询边缘列表、边缘详情
    - 管理边缘信息（ID、IP、类别等）
    - 设置边缘权限（允许/禁止上线）
    - 查询全局 edge 在线状态
  - **配置管理**：
    - 创建、修改、删除配置
    - 向 edge 下发配置
  - **状态查询**：
    - 边缘在线状态
    - ingress 负载状态
  - **操作执行**：
    - 下发命令
    - 批量操作
- **特点**：同步请求/响应，便于 Web UI 和自测工具调用

---

## 分布式部署支持

- **touchIngress**：
  - 可部署在多个区域（如北京、上海）
  - 边缘边缘通过 touchManager 的负载分配就近连接
  - 无状态设计，易于水平扩展
  - IP 和端口信息通过 touchManager 的 Web UI 配置
  - touchManager 启动后主动连接配置的 touchIngress 实例
- **touchManager**（待实现）：
  - 单一实例（或主从 HA）
  - 主动连接配置的 touchIngress 实例（通过 Web UI 配置）
  - 维护全局的 ingress 服务状态表和负载状态
  - 管理 ingress 上下线（是否提供服务）
  - 展示全局的 edge 的在线状态； 
  - 向edge下发配置
  - 提供 REST API 供 Web UI 和 touchEdge 注册访问
- **负载均衡策略**：
  - **优先策略**：优先选择边缘此前使用的 touchIngress（如果在线且负载正常）
  - **重新分配策略**：如果此前使用的 ingress 不在线，根据以下因素重新分配：
    - 边缘 ID、类别、边缘自身IP、公网IP
    - ingress 当前负载情况（连接数、资源使用等）
  - **一致性保证**：同一边缘在相同条件下总是分配到相同的 ingress
- **配置管理**：
  - touchIngress 的 IP、端口等信息通过 Web UI 配置在 touchManager 中
  - 支持动态添加、修改、删除 touchIngress 配置
  - touchManager 根据配置主动连接 touchIngress
  - edge 的 id、IP、类别等信息管理在 touchManager 中； 可以根据边缘id设置是否容许边缘上线， 默认使用黑名单机制， 即接受边缘的自动注册；
- **数据一致性**（待实现）：
  - 边缘状态最终一致（容忍短暂延迟）
  - 关键操作（如配置变更）强一致
  - ingress 服务状态表实时更新

---

## 实现状态总结

| 组件 | 实现状态 | 技术栈 | 主要功能 |
|------|---------|--------|---------|
| **touchEdge** | 已实现 | C++ (libezsocket, libezutil) | REST API 客户端（首次注册），WebSocket 客户端（连接 ingress），token 认证，失败重试 |
| **touchIngress** | 已实现 | C++ (libezsocket) | WebSocket 服务端（接收 manager 和 edge 连接），连接管理，消息转发，负载状态上报 |
| **touchManager** | 待实现 | Python (计划) | REST API 服务端（边缘注册、Web UI），主动连接 ingress（WebSocket (come.1 JSON)），负载均衡（优先策略），token 生成，边缘权限管理（黑名单/白名单），ingress 上下线管理，全局 edge 在线状态展示，配置下发 |

# 功能说明

[列出项目的主要功能模块，每个功能包含参数说明和功能描述]

1. **边缘自动注册**
   - 参数：edge_id（必需）、edge_type（必需）、edge_key（必需）、local_ip（可选，边缘自身IP）
   - 说明：边缘通过 REST API 向 touchManager 请求上线/注册，touchManager 根据边缘信息、历史连接记录和 ingress 负载情况分配最优的 touchIngress。Manager 从请求中获取对端IP（边缘的公网IP），返回 ingress 接入信息、token 和边缘的公网IP（edge_public_ip）

2. **负载均衡分配**
   - 参数：边缘ID、类别、边缘自身IP、上次连接记录、ingress 负载状态
   - 说明：touchManager 优先选择边缘此前使用的 touchIngress（如果在线），否则根据边缘信息和 ingress 负载情况重新分配

3. **失败重试机制**
   - 参数：无
   - 说明：边缘在 touchIngress 上线失败（连接失败、认证失败、交互失败）时，自动重新向 touchManager 发起上线/注册流程

4. **touchIngress 配置管理**
   - 参数：ingress_id、IP、端口、描述
   - 说明：通过 Web UI 配置 touchIngress 的 IP、端口等信息，touchManager 根据配置主动连接 touchIngress

5. **token 认证机制**
   - 参数：token（由 touchManager 生成并返回给边缘）
   - 说明：边缘使用 token 与 touchIngress 建立连接并进行身份认证，确保安全性

6. **边缘权限管理**
   - 参数：edge_id、权限设置（允许/禁止上线）
   - 说明：touchManager 管理 edge 的 id、IP、类别等信息，可以根据边缘 ID 设置是否允许边缘上线。默认使用黑名单机制（即接受所有边缘的自动注册），支持白名单机制（仅允许列表中的边缘上线）

7. **ingress 上下线管理**
   - 参数：ingress_id、服务状态（启用/禁用）
   - 说明：通过 Web UI 管理 touchIngress 的上下线状态，控制是否提供服务。touchManager 根据 ingress 的服务状态进行负载均衡分配

8. **全局 edge 在线状态展示**
   - 参数：无
   - 说明：touchManager 展示全局的 edge 在线状态，包括边缘 ID、IP、类别、在线/离线状态、最后心跳时间等信息，便于运维管理

9. **配置下发**
   - 参数：edge_id、配置内容
   - 说明：touchManager 向 edge 下发配置（如 frpc.json 等），支持单个边缘配置下发和批量配置下发。对于离线边缘，配置会缓存到队列中，待边缘上线时自动下发

# 非功能说明

## 架构和规范

项目遵循业界最佳实践，采用标准化的通信协议和架构模式：

- **REST API**：
  - Edge ↔ Manager 首次通信采用 REST API（HTTP/HTTPS）
  - Manager ↔ Web UI 通信采用 REST API
  - 使用 JSON 格式进行数据交换
  - 遵循 RESTful 设计原则，提供标准的 HTTP 方法（GET、POST、PUT、DELETE）

- **WebSocket (JSON over WebSocket)**：
  - Edge ↔ Ingress 后续通信采用 WebSocket 协议
  - Manager ↔ Ingress 通信采用 WebSocket (come.1 JSON)
  - 使用 come.1 JSON 协议进行消息统一定义
  - 支持长连接、双向通信，适合实时消息推送和状态同步

- **分层架构**：
  - 采用三层架构设计（Edge、Ingress、Manager）
  - 职责清晰分离，便于扩展和维护
  - Ingress 层无状态设计，支持水平扩展

- **安全机制**：
  - 使用 token 进行身份认证
  - 支持边缘权限管理（黑名单/白名单机制）
  - 通信支持 HTTPS/WSS 加密传输

## 性能要求

touchEdge: 单链路， 最大1Mbps流量， 一般、平时极小的信令流量。
touchIngress ： 每实例，单核4g内存满足万级edge接入
touchManager ： 满足百万级edge管理， 百级 touchIngress 管理

## 运维要求

touchIngress ： touchEdge(终端)接入总数、按时间段活跃情况等业务参数支持Prometheus采集
touchManager ： touchIngress接入总数、按时间段活跃情况等业务参数支持Prometheus采集

## 安全要求

业务接口安全
传输链路安全

## 可靠性要求

所有网元满足7*24小时运行邀请

# 运行环境

所有网元运行环境为Linux
WebUI支持chrome等主流浏览器

```bash
# 环境配置示例
source /path/to/environment/bin/activate
```

# 技术栈

## touchEdge（Edge，终端）
- **语言**：C++
- **核心库**：
  - libezsocket：WebSocket 通信、HTTP 客户端功能（使用 `ez_tcp_client`、`ez_http_post` 等接口向 touchManager 发送 REST API 注册请求）
  - libezThread：线程管理
  - come.1：JSON 协议封装

## touchIngress（Ingress Service，接入层）
- **语言**：C++
- **核心库**：
  - libezsocket：WebSocket 通信
  - libezThread：线程管理
  - libezutil：工具库
  - come.1：JSON 协议封装

## touchManager（Management Service，管理服务）
- **语言**：Python
- **核心框架/库**：
  - Web 框架：Flask/FastAPI（REST API 服务）
  - WebSocket 客户端：websockets/websocket-client（连接 touchIngress）
  - 数据库：SQLite（单机）或 PostgreSQL（集群）
  - 模板引擎：Jinja2（配置生成）
  - 认证/加密：JWT（token 生成）

# 依赖项

[列出项目依赖的外部库和工具]

## 核心依赖

### touchEdge 依赖
- **libezsocket**：WebSocket 通信和 HTTP 客户端库
  - 安装位置：`$(HOME)/libs/lib/libezsocket-$(PLATFORM).a`（静态库）
  - 头文件：`$(HOME)/libs/include/ezsocket/ez_socket.h`
  - HTTP 客户端接口：
    - `ez_tcp_client` / `ez_tcp_client_with_timeout`：TCP 客户端连接
    - `ez_http_post`：HTTP POST 请求
    - `ez_http_post_to_host`：直接向主机发送 HTTP POST 请求（封装了连接和发送）
  - 用途：用于向 touchManager 发送 REST API 注册请求
- **libezThread**：线程管理库
- **come.1**：JSON 协议封装库

## 开发依赖

- 开发依赖1：用途说明
- 开发依赖2：用途说明

## 特殊依赖说明

[对于依赖项较多的项目（如 C/C++ 项目需要 websocket 库、JSON 解析库等），在此处详细说明]

### C/C++ 项目依赖说明

- **WebSocket 和 HTTP 客户端库 (libezsocket)**: 用于 WebSocket 通信和 REST API 通信
  - **libezsocket**: 
    - 安装位置：`$(HOME)/libs/lib/libezsocket-$(PLATFORM).a`（静态库）、`$(HOME)/libs/include/ezsocket/ez_socket.h`（头文件）
    - HTTP 客户端接口：
      - `ez_tcp_client(const char *hname, const char *sname)`：创建 TCP 客户端连接
      - `ez_tcp_client_with_timeout(const char *hname, const char *sname, const long usec_timeout)`：带超时的 TCP 客户端连接
      - `ez_http_post(ez_socket_t sockfd, const char *host, const char *page, const char *poststr, char *recvline, int recvbuflen, int iFastMode)`：HTTP POST 请求
      - `ez_http_post_to_host(const char *p_host, const char *p_port, const char *p_page, const char *p_post_string, char *recvline, int recvbuflen, int iFastMode)`：直接向主机发送 HTTP POST 请求（封装了连接和发送）
    - 使用示例：
      ```c
      #include "ez_socket.h"
      // 方式1：使用 ez_http_post_to_host（推荐，更简单）
      char recvline[4096];
      int ret = ez_http_post_to_host("manager.example.com", "80", "/api/v1/edges/register", 
                                      json_data, recvline, sizeof(recvline), 0);
      // 方式2：手动连接和发送
      ez_socket_t sock = ez_tcp_client_with_timeout("manager.example.com", "80", 5000000);
      if (sock > 0) {
          ret = ez_http_post(sock, "manager.example.com", "/api/v1/edges/register", 
                            json_data, recvline, sizeof(recvline), 0);
          ez_close(sock);
      }
      ```
    - 用途：touchEdge 使用 libezsocket 的 HTTP 客户端功能向 touchManager 发送边缘注册 REST API 请求，同时用于 WebSocket 通信

---

## come.1 库设计

### 概述

**come.1** 是一个用于 C++ 单元的协议解析和封装库，专门用于 Touch 系统中 Edge 和 Ingress 之间的消息通信。该库的设计目标是：

- **解耦协议实现**：Edge 和 Ingress 只需关注业务数据结构，无需直接依赖底层 JSON 库
- **统一协议处理**：所有 JSON 协议封装和解封装都通过 come.1 库实现，保持协议处理的统一性和可维护性
- **可扩展性**：库设计为可对外提供，未来可发展为独立的协议库

### 架构设计

come.1 库采用**分层设计**，将数据结构定义和 JSON 编解码分离：

```
┌─────────────────────────────────────────┐
│         Edge / Ingress 应用层            │
│  （只关注数据结构，调用 encode/decode）   │
└─────────────────────────────────────────┘
                    │
                    │ 使用
                    ▼
┌─────────────────────────────────────────┐
│         come.1.json.h / cpp              │
│  （提供 encode/decode 接口）             │
│  - ComeJsonCodec::encode()              │
│  - ComeJsonCodec::decode()               │
│  - JsonValue（封装 JSON 操作）           │
│  - JsonRpcRequest/Response（JSON-RPC 2.0）│
└─────────────────────────────────────────┘
                    │
                    │ 依赖
                    ▼
┌─────────────────────────────────────────┐
│         come_1.h                        │
│  （定义协议数据结构）                     │
│  - EdgeOnline / AckEdgeOnline       │
│  - EdgeHeartbeat / AckEdgeHeartbeat │
│  - 其他协议消息结构                       │
└─────────────────────────────────────────┘
                    │
                    │ 内部实现
                    ▼
┌─────────────────────────────────────────┐
│         nlohmann::json（内部实现）        │
│  （不对外暴露，仅库内部使用）              │
└─────────────────────────────────────────┘
```

### 核心组件

#### 1. come_1.h - 数据结构定义

**职责**：定义所有协议消息的数据结构，不涉及任何 JSON 序列化逻辑。

**主要数据结构**：
- `EdgeOnline`：边缘上线消息
- `AckEdgeOnline`：边缘上线应答
- `EdgeHeartbeat`：边缘心跳消息
- `AckEdgeHeartbeat`：边缘心跳应答
- `JsonRpcRequest`：JSON-RPC 2.0 请求
- `JsonRpcResponse`：JSON-RPC 2.0 响应
- `JsonValue`：JSON 值封装（用于参数传递）

**设计原则**：
- 纯数据结构，无业务逻辑
- 不依赖任何 JSON 库
- 可独立使用和测试

#### 2. come.1.json.h / cpp - JSON 编解码实现

**职责**：提供数据结构和 JSON 字符串之间的编解码功能。

**核心接口**：
```cpp
class ComeJsonCodec {
public:
    // 编码：将消息对象转换为 JSON 字符串
    static std::string encode(const EdgeOnline& msg);
    static std::string encode(const AckEdgeOnline& msg);
    // ... 其他消息类型的 encode 方法
    
    // 解码：将 JSON 字符串解析为消息对象
    static bool decode(const std::string& jsonStr, EdgeOnline& msg);
    static bool decode(const std::string& jsonStr, AckEdgeOnline& msg);
    // ... 其他消息类型的 decode 方法
    
    // JSON-RPC 2.0 支持
    static bool isJsonRpc2(const std::string& jsonStr);
    static bool isRequest(const std::string& jsonStr);
    static bool isResponse(const std::string& jsonStr);
};

// JSON 值封装类（避免外部直接使用 nlohmann::json）
class JsonValue {
public:
    static JsonValue parse(const std::string& jsonStr);
    std::string dump(int indent = -1) const;
    
    // 类型检查和值获取
    bool isObject() const;
    bool isArray() const;
    std::string getString(const std::string& key, const std::string& defaultVal = "") const;
    int getInt(const std::string& key, int defaultVal = 0) const;
    // ... 其他类型的方法
    
    // 值设置
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    // ... 其他类型的方法
};
```

**设计原则**：
- 封装底层 JSON 库（nlohmann::json），不对外暴露
- 提供类型安全的接口
- 统一的错误处理机制

### 使用方式

#### Edge / Ingress 中的使用示例

```cpp
#include "come_1.h"           // 数据结构定义
#include "come.1.json.h"      // JSON 编解码接口

// 1. 创建消息对象（只关注数据结构）
EdgeOnline msg;
msg.id = "edge001";
msg.key = "key001";
msg.type = "touch";

// 2. 编码为 JSON 字符串（调用库接口）
std::string json_str = ComeJsonCodec::encode(msg);

// 3. 发送 JSON 字符串（通过 WebSocket）
websocket.send(json_str);

// 4. 接收 JSON 字符串（从 WebSocket）
std::string recv_json = websocket.receive();

// 5. 解码为消息对象（调用库接口）
AckEdgeOnline ack;
if (ComeJsonCodec::decode(recv_json, ack)) {
    if (ack.code == 0) {
        // 处理成功响应
    }
}
```

**关键优势**：
- Edge/Ingress 代码**不直接使用 nlohmann::json**
- 只需关注业务数据结构（`EdgeOnline`、`AckEdgeOnline` 等）
- 所有 JSON 操作都通过 `ComeJsonCodec` 和 `JsonValue` 接口完成
- 如果未来需要更换 JSON 库，只需修改 come.1 库内部实现

### 安装和使用

#### 编译和安装

```bash
cd $HOME/svn/come1900/UniTrans/libs/come.1
make          # 编译库
make install  # 安装到 $HOME/libs/
```

**安装位置**：
- 静态库：`$HOME/libs/lib/libcome1-$(PLATFORM).a`
- 头文件：`$HOME/libs/include/come1/come_1.h`、`$HOME/libs/include/come1/come.1.json.h`

#### 在 Edge / Ingress 中使用

**Makefile 配置**：
```makefile
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/come1
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lcome1-$(PLATFORM)
```

**代码包含**：
```cpp
#include "come_1.h"           // 数据结构
#include "come.1.json.h"      // JSON 编解码
```

### 日志级别规范

**核心原则**：`ez_printf_info` 用于业务跟踪主线，`ez_printf_debug` 用于问题定位细节。

**日志级别使用规则**：

| 级别 | 用途 | 说明 |
|------|------|------|
| **INFO** (`-6-`) | 业务流程关键节点 | 可用于跟踪完整的业务流程，如连接、上线、离线、心跳、踢除等 |
| **DEBUG** (`-7-`) | 内部处理细节 | 用于定位问题时需要的详细信息，如缓存操作、确认响应、中间状态等 |
| **WARNING** (`-4-`) | 异常但可恢复 | 参数错误、设备验证失败、重连等异常情况 |
| **ERROR** (`-3-`) | 严重错误 | 启动失败、资源创建失败等 |

**业务流程 INFO 日志主线**（Ingress）：

一个完整的 Edge 上线→心跳→离线业务流程，INFO 级别日志应能清晰跟踪：

```
[CONNECT]        client_id=8, ip=127.0.0.1, port=xxxxx        ← 连接建立
[EDGE.ONLINE]    Edge registered successfully [client_id=8]    ← 注册成功
[MANAGER.CONNECT] Manager connected successfully               ← Manager 连接
[DISCONNECT]     Edge disconnected [client_id=8]               ← 断开连接
[KICK]           Kicked edge xxx (client_id=8)                 ← 被踢出（如适用）
```

**INFO 级别事件清单**：

| 事件 | 日志标签 | 触发条件 |
|------|---------|---------|
| 服务启动/停止 | `Starting/Started/Stopping/Stopped` | 生命周期管理 |
| 客户端连接 | `[CONNECT]` | WebSocket 连接建立 |
| Manager 连接成功 | `[MANAGER.CONNECT] Manager connected successfully` | Manager 认证通过 |
| Edge 注册成功 | `[EDGE.ONLINE] Edge registered successfully` | Edge 完成上线流程 |
| Edge 断开 | `[DISCONNECT] Edge disconnected` | 已注册 Edge 断开 |
| Manager 断开 | `[DISCONNECT] Manager disconnected` | Manager 断开连接 |
| 踢除 Edge | `[KICK] Kicked edge` | 主动踢除 Edge |
| 心跳异常 | `Heartbeat received from unregistered/mismatch` | 心跳验证失败 |

**DEBUG 级别事件清单**：

| 事件 | 说明 |
|------|------|
| 消息接收详情 | 收到的 edge.online、manager.connect 等消息的详细内容 |
| 缓存操作 | `Cached offline event`、`Cleared offline cache` |
| 确认响应 | `Manager confirmed online/offline` |
| 上报状态 | `Reporting all N existing edges`、`No cached offline events` |
| 通知发送详情 | `[NOTIFY.ONLINE/OFFLINE] Sent to manager` 的详细参数 |
| 未知客户端断开 | 非 Edge/Manager 的连接断开 |
| 未注册 Edge 断开 | 连接建立但尚未完成注册就断开 |
| 踢除时 Edge 不在列表 | `Edge not found in online list` |

### 日志级别规范

**核心原则**：`ez_printf_info` 用于业务跟踪主线，`ez_printf_debug` 用于问题定位细节。

**设计理念**：
- **INFO 级别**：一个完整的业务流程（连接→上线→心跳→离线），仅通过 INFO 日志就能清晰跟踪全流程
- **DEBUG 级别**：定位具体问题时才需要的内部处理细节

#### INFO 级别事件清单（业务跟踪主线）

一个完整的 Edge 上线→心跳→离线业务流程，INFO 级别日志应能清晰跟踪：

```
[CONNECT]        client_id=8, ip=127.0.0.1, port=xxxxx        ← 连接建立
[EDGE.ONLINE]    Edge registered successfully [client_id=8]    ← 注册成功
[MANAGER.CONNECT] Manager connected successfully               ← Manager 连接
[DISCONNECT]     Edge disconnected [client_id=8]               ← 断开连接
[KICK]           Kicked edge xxx (client_id=8)                 ← 被踢出（如适用）
```

| 事件 | 日志标签 | 触发条件 | 说明 |
|------|---------|---------|------|
| 服务启动 | `Starting on port` | 服务启动时 | 记录监听端口和协议 |
| 服务就绪 | `Started successfully` | WebSocket 服务端就绪 | 服务可用 |
| 服务停止 | `Stopping...` / `Stopped successfully` | 服务关闭 | 生命周期管理 |
| 客户端连接 | `[CONNECT]` | WebSocket 连接建立 | 记录 client_id、IP、端口 |
| Manager 连接成功 | `[MANAGER.CONNECT] Manager connected successfully` | Manager 认证通过 | 关键业务节点 |
| Edge 注册成功 | `[EDGE.ONLINE] Edge registered successfully` | Edge 完成上线流程 | 关键业务节点 |
| Edge 断开 | `[DISCONNECT] Edge disconnected` | 已注册 Edge 断开 | 关键业务节点 |
| Manager 断开 | `[DISCONNECT] Manager disconnected` | Manager 断开连接 | 关键业务节点 |
| 踢除 Edge | `[KICK] Kicked edge` | 主动踢除 Edge | 关键业务节点 |

#### DEBUG 级别事件清单（问题定位细节）

| 事件类别 | 说明 |
|---------|------|
| 消息接收详情 | 收到的 edge.online、manager.connect 等消息的详细内容 |
| 缓存操作 | `Cached offline event`、`Cleared offline cache` |
| 确认响应 | `Manager confirmed online/offline` |
| 上报状态 | `Reporting all N existing edges`、`No cached offline events` |
| 通知发送详情 | `[NOTIFY.ONLINE/OFFLINE] Sent to manager` 的详细参数 |
| 未知客户端断开 | 非 Edge/Manager 的连接断开 |
| 未注册 Edge 断开 | 连接建立但尚未完成注册就断开 |
| 踢除时 Edge 不在列表 | `Edge not found in online list` |
| 心跳处理 | 收到心跳、等待确认时重通知等中间状态 |
| 上报模式 | `EdgeReportMode`、`Reporting changed edges` 等配置细节 |

#### WARNING/ERROR 级别

| 级别 | 事件 | 说明 |
|------|------|------|
| **WARNING** | 参数错误、设备验证失败、重连、未知方法 | 异常但可恢复 |
| **ERROR** | 启动失败、WebSocket 服务端创建失败 | 严重错误，服务不可用 |

### 开发阶段说明

**当前状态**：come.1 库处于开发阶段，因此放在 Touch 项目中一起发展。

**未来规划**：
- 库稳定后，可独立发布为外部库
- 其他项目也可以使用 come.1 库进行协议处理
- 保持向后兼容性，确保现有代码无需修改

### 设计原则总结

1. **数据结构与编解码分离**：`come_1.h` 定义数据结构，`come.1.json.h/cpp` 实现编解码
2. **隐藏实现细节**：Edge/Ingress 不直接依赖底层 JSON 库（nlohmann::json）
3. **统一接口**：所有协议处理都通过 come.1 库的统一接口
4. **类型安全**：提供类型安全的 JSON 操作接口（`JsonValue`）
5. **可扩展性**：支持 JSON-RPC 2.0 等标准协议

### 协议构造原则

**核心原则**：`FunRegisterSvr.cpp`（以及所有 Edge/Ingress 业务代码）**不应该直接使用 `JsonValue` 构造或解析协议消息**。所有协议构造和解析都应该通过 `come.1` 库完成。

**正确做法**：
```cpp
// ✅ 正确：使用 come.1 库的消息类型
#include "come_1.h"
#include "come.1.json.h"

// 构造消息
IngressEdgeOnline msg;
msg.edge_id = edge_id;
msg.ingress_id = ingress_id;
msg.online_since = online_since;
std::string json = ComeJsonCodec::encodeJsonRpcRequest(msg, COME_METHOD_INGRESS_EDGE_ONLINE, req_id);

// 解析消息
IngressEdgeOnline msg;
if (ComeJsonCodec::decode(json_str, msg)) {
    // 使用 msg.edge_id, msg.ingress_id 等字段
}
```

**错误做法**：
```cpp
// ❌ 错误：直接使用 JsonValue 构造协议消息
#include "come.1.json.h"  // 不应该在业务代码中直接使用 JsonValue

JsonValue params = JsonValue::createObject();
params.setString("edge_id", edge_id);
params.setString("ingress_id", ingress_id);
params.setInt64("online_since", online_since);
JsonRpcRequest req(COME_METHOD_INGRESS_EDGE_ONLINE, params);
```

**设计优势**：
- **协议一致性**：所有消息格式由 come.1 库统一管理，避免字段名不一致
- **易于维护**：协议变更只需修改 come.1 库，业务代码无需修改
- **类型安全**：编译期检查字段类型，避免运行时错误
- **可测试性**：消息结构可独立测试，不依赖 JSON 库

---

# 功能设计

---
## Edge frpc 远程配置

**版本**: 1.2
**日期**: 2026-09-11
**状态**: 已实现 · 功能与并发正确性通过实测

> **v1.2 按代码同步说明**（2026-09-11）：本文档此前部分内容与实际实现存在漂移，本次按源码核对并修正，关键差异：
> - `config_status` 合法取值实为 `unknown / configuring / confirmed / pending / config_failed`（**无 `configured`**；"下发成功已确认"用 `confirmed`，"离线暂存待下发"用 `pending`）。
> - Edge 另有 `confirmed` 字段表示设备白/黑名单：`99=pending`、`1=confirmed(白名单)`、`0=rejected(黑名单)`。`confirmed=0` 的 edge 每次上线会被 manager 下发 `manager.edge.kick` 踢下线（详见"异常处理"）。
> - Edge 侧配置文件名按边缘 ID 生成：`./shpc/shpc.<edge_id>.json`（如 `shpc.edge001.json`），不再写死单个 `shpc.a1.json`。
> - touch_manager 默认监听 `18050`（`PORT` 可覆盖，测试脚本用 `18051`）。
> - 配置下发支持 manager↔ingress 断线自动重连；未确认(confirmed)配置的重复推送节流阈值可在 `config.py` 用 `CONFIG_CONFIRM_TIMEOUT` 配置(范围 1~3600 秒，0=关闭节流)。

### 功能/目的

实现 touch_manager 对 Edge 设备 frpc 配置的远程管理能力：
- 远程配置下发（写配置）
- 配置结果反馈
- 配置内容查询

---

### 各层职责

#### 1. touchEdge（Edge，终端）
- **新增功能**：
  - 处理 `ConfigUpdate_tunnelService` 消息
  - 将 `ConfigUpdate_tunnelService` 转换为 `FrpcConfig`
  - 将 `FrpcConfig` 编码为 JSON 字符串
  - 写入配置文件（路径按边缘 ID 生成：`./shpc/shpc.<edge_id>.json`，由 `FunRegisterCli::get_frpc_config_path()` 提供）
  - 回复 `AckConfigUpdate`（成功/失败）
  - 处理配置查询请求，读取并返回配置文件内容

#### 2. touchIngress（Ingress Service，接入层）
- **新增功能**：
  - 透传 `ConfigUpdate_tunnelService` 消息（Manager → Edge）
  - 透传 `AckConfigUpdate` 消息（Edge → Manager）
  - 透传配置查询请求和响应

#### 3. touchManager（Management Service，管理服务）
- **新增功能**：
  - **REST API**（遵循全系统约定：边缘 ID 一律放在请求 body，不放 URL 路径；操作名用 RPC 风格的 `POST /api/v1/edges/<action>`）：
    - `POST /api/v1/edges/config/update` - body `{edge_id, config_type, config_content}`，保存并下发 frpc 配置
    - `POST /api/v1/edges/config/get` - body `{edge_id}`，平台加载配置：DB 有配置则直接返回；DB 无配置且 Edge 在线则同步读一次 edge 本地配置并回填 DB；均无则返回后端默认
    - 说明：早期版本曾用 `POST/GET /api/v1/edges/<edge_id>/config` 路径式接口，与约定冲突，**已废弃**，统一迁移到上述 body 式接口（见「第三阶段」接口约定）。
  - **数据库扩展**：
    - `edge_configs` 表（EdgeConfig 模型）：存储边缘配置（edge_id, config_json, version, updated_at, configuring_since）
    - `Edge` 模型新增字段：
      - `config_status`：'unknown'（未知）, 'configuring'（下发中）, 'confirmed'（已确认）, 'pending'（离线暂存待下发）, 'config_failed'（下发失败）
      - `config_version`：配置版本号
      - `confirmed`：设备白/黑名单标记 `99=pending, 1=confirmed(白名单), 0=rejected(黑名单)`
  - **配置持久化**：
    - 用户保存配置时，先写入数据库，再下发给在线 Edge
    - 离线 Edge 的配置暂存数据库，待上线后下发
    - 查询配置时（config/get）：DB 有配置则直接返回；DB 无配置且 Edge 在线则同步读一次 edge 本地配置并回填 DB（区分 missing/bad）；Edge 离线则返回后端默认

注：Web UI 由 touchManager 统一提供，详见 [spec-design.md](../test/touch/spec/spec-design.md)

---

### 设计原则

#### 配置存储原则

**核心原则**：配置在 touchManager 数据库中持久化存储，Edge 本地为运行时副本。

- **设计理由**：
  - touchManager 需要持久化保存配置，支持离线边缘的配置缓存
  - 用户首次打开配置界面时，从数据库加载配置（无需等待 Edge 响应）
  - Edge 离线时，配置可以暂存，待 Edge 上线后下发
  - 查询时先展示数据库中的配置；DB 无配置且 Edge 在线时同步读一次 edge 本地配置（区分 missing/bad）

- **实现方式**：
  - touchManager 数据库存储配置的完整 JSON 表示
  - 用户保存配置时，先写入数据库，再下发给 Edge
  - 查询配置时（config/get）：
    1. DB 有配置：直接返回数据库配置
    2. DB 无配置且 Edge 在线：同步读一次 edge 本地配置并回填 DB 后返回（区分 missing/bad）
    3. DB 无配置且 Edge 离线：返回后端默认配置
  - 配置文件权限设置为 0644
  - Edge 侧配置文件路径按边缘 ID 生成：`./shpc/shpc.<edge_id>.json`（`FunRegisterCli::get_frpc_config_path()`），多 Edge 各自独立、互不覆盖，便于横向扩展

---

### 详细设计

#### 消息格式

**ConfigUpdate_tunnelService**（Manager → Edge）:
```json
{
  "jsonrpc": "2.0",
  "method": "edge.config.update",
  "params": {
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "version": 1,
    "touch_token": "edge001_test_token",
    "configContent": {
      "services": [
        {
          "serviceName": "tunnelService",
          "tunnelService": {
            "version": "1.0",
            "endpoint": {
              "host": "10.220.42.139",
              "port": 50400
            },
            "localManagement": {
              "bindAddress": "127.0.0.1",
              "bindPort": 17400
            },
            "security": {
              "authMethod": "token",
              "credential": "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc",
              "enableTls": true
            },
            "accessPolicies": [
              {
                "policyId": "w-tcp-51422",
                "protocol": "tcp",
                "description": "pss service",
                "exposedPort": 51422,
                "targetService": {
                  "ip": "127.0.0.1",
                  "port": 55555
                }
              }
            ]
          }
        }
      ]
    }
  },
  "id": 12345
}
```

**AckConfigUpdate**（Edge → Manager）:
```json
{
  "jsonrpc": "2.0",
  "result": {
    "code": 0,
    "message": "success"
  },
  "id": 12345
}
```

---

### 流程设计

#### 1. 远程配置下发流程

```
Web UI → touch_manager（写数据库） → touchIngress → touchEdge → 写入文件 → 反馈结果
```

**步骤**：
1. 用户在 Web UI 填写 frpc 配置，点击保存
2. touch_manager 将配置写入数据库（edge_config 表）
3. 检查 Edge 是否在线：
   - Edge 在线：构造 `ConfigUpdate_tunnelService` 消息，通过 WebSocket 发送
   - Edge 离线：配置暂存数据库，等待 Edge 上线后下发
4. touchIngress 透传给 touchEdge
5. touchEdge 处理：
   - decode JSON → `ConfigUpdate_tunnelService`
   - `toFrpcConfig()` 转换为 `FrpcConfig`
   - `encode` 为 JSON 字符串
   - 写入配置文件（路径 `./shpc/shpc.<edge_id>.json`）
   - 回复 `AckConfigUpdate(code=0, message="success")`
6. touchIngress 将 `AckConfigUpdate` 转发给 touch_manager
7. touch_manager 更新数据库状态：`config_status = 'confirmed'`（并递增 `config_version`）
8. Web UI 显示配置成功

**异常处理**：
- Edge 离线：配置写入数据库，`config_status='pending'`，等待 Edge 上线后下发
- Edge 为黑名单（`confirmed=0`）：manager 下发 `manager.edge.kick`，ingress 踢掉并回 ack `"Edge rejected by admin" (40001)`——该 edge 会持续"上线→被踢→重连"，除非把 `confirmed` 修正为非 0 值
- Ingress 未连接：REST API 返回 50004 "Ingress not connected"
- 文件写入失败：Edge 回复 `AckConfigUpdate(code=-2, message="Failed to write config file")`，manager 置 `config_status='config_failed'`
- 消息超时：REST API 返回 50007 "Config update timeout"，数据库标记为"configuring"
- 重复推送节流：edge 仍处 `configuring` 时再次 POST 同一条配置，距上次下发超过 `CONFIG_CONFIRM_TIMEOUT`(默认 10s，范围 1~3600，0=不做节流每次都推) 才允许重推；否则返回 50008 "Config update in progress"
- manager↔ingress 断线：manager 自动重连（默认每 10s 重试），断线期间该 ingress 下所有 edge 标记 offline，重连后恢复

---

#### 2. 配置查询流程

```
Web UI → touch_manager（config/get） → （DB 有配置→直接返回；DB 无配置且 Edge 在线→同步读 edge 本地配置并回填） → touchIngress → touchEdge
```

**步骤**（`config/get`，无后台异步线程）：
1. 用户在 Web UI 点击「从平台加载」
2. touch_manager 从数据库查询配置（edge_config 表）
   - DB 有配置：直接返回配置（同步）
   - DB 无配置且 Edge 在线：同步发起一次配置查询，读 edge 本地真实配置并回填 DB 后返回
   - DB 无配置且 Edge 离线：返回后端默认配置
3. Web UI 展示返回的配置（JSON 表单回填）

**Edge 在线时的同步查询（一次）**：
- touch_manager 发送配置查询请求给 touchIngress → 透传 touchEdge
- touchEdge 处理：
  - 读取 `./shpc/shpc.<edge_id>.json`（按自身边缘 ID 定位）
  - parse JSON → `FrpcConfig`
  - `fromFrpcConfig()` 转换为 `ConfigUpdate_tunnelService` 格式
  - 回复配置数据
- touchIngress 将响应转发给 touch_manager
- touch_manager 更新数据库：写入/刷新 config_json（若 DB 原为空）；同时做一致性核对（matched/mismatch/missing/bad）

**异常处理**：
- DB 无配置且 Edge 离线：返回后端默认配置
- Edge 离线：返回数据库配置，标记"设备离线，无法刷新"
- 配置文件不存在：Edge 返回 JSON-RPC error（顶层 `error`），`config/get` 判为 `missing`
- 配置文件存在但解析失败：Edge 返回 `result.config_error`，`config/get` 判为 `bad`

---

### 实现计划

> 状态：三个阶段均已实现并通过实测，含第三阶段 frpc 配置编辑 Web UI。

#### 数据库设计（对应 `database.py` 实际模型，已实现）

**`edge_configs`（EdgeConfig 模型，存储配置）**：
```python
edge_id     : PK+FK→edges, unique   # 边缘 ID（唯一）
config_json : Text, nullable        # frpc 配置 JSON 字符串
version     : Integer, default 0    # 配置版本号
updated_at  : DateTime              # 更新时间
configuring_since : BigInteger      # UTC 毫秒时间戳，用于"未确认超时重推"判断
```

**`edges`（Edge 模型）新增字段**：
```python
config_status  : String(20), default 'unknown'  # unknown/configuring/confirmed/pending/config_failed
config_version : Integer, default 0             # 配置版本号
confirmed      : Integer, default 99            # 99=待确认, 1=已确认(白名单), 0=黑名单(拒绝)
```

#### 第一阶段：基础功能（✅ 已实现）
- [x] Edge 侧：CFunRegisterCli 实现 `ConfigUpdate_tunnelService` 处理
- [x] Edge 侧：实现 `write_frpc_config_file()` 函数（按 `shpc/shpc.<edge_id>.json` 写盘）
- [x] Ingress 侧：透传 `ConfigUpdate/AckConfigUpdate` 消息
- [x] touch_manager 侧：
  - [x] 新增 REST API：`POST /api/v1/edges/config/update`（body 带 edge_id）✅ 已迁移
  - [x] 数据库扩展：创建 `edge_configs` 表，给 `edges` 添加 `config_status`、`config_version`、`confirmed` 字段
  - [x] 配置保存逻辑：写数据库 → 检查在线 → 下发配置

#### 第二阶段：查询功能（✅ 已实现）
- [x] Edge 侧：实现配置查询处理（`edge.config.query`）与 `read_frpc_config_file()`
- [x] Ingress 侧：透传查询请求/响应
- [x] touch_manager 侧：
  - [x] 新增 REST API：`POST /api/v1/edges/config/get`（body 带 edge_id）✅ 已迁移
  - [x] 配置查询逻辑：先返回数据库 → 在线时线程安全地 `edge.config.query` 刷新（同步/异步，见「异步回填 bug 修复」）

#### 第三阶段：用户界面（frpc 配置编辑，✅ 已实现）
> 已实现独立的 frpc 配置编辑页（`templates/edge_config.html` + 路由 `GET /edges/config`），并在 `edges.html` 设备列表每行「操作」列新增「配置」按钮跳转进入。配置按类型分组折叠展示（当前为「SHPC 配置」面板，未来多种配置类型可平铺并列）。以下为实际实现与约定。


**接口约定**（遵循全系统 REST 约定：边缘 ID 放 body，操作名用 RPC 风格 `POST /api/v1/edges/<action>`；`config_content` 采用嵌套 `services[].tunnelService` 结构）：
- **保存并下发** `POST /api/v1/edges/config/update`
  - body：
    ```json
    {
      "edge_id": "edge001",
      "config_type": "tunnelService",
      "config_content": {
        "services": [
          {
            "serviceName": "tunnelService",
            "tunnelService": {
              "endpoint": {"host": "10.220.42.139", "port": 50400},
              "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
              "security": {"authMethod": "token", "credential": "...", "enableTls": true},
              "accessPolicies": [
                {"policyId": "w-tcp-51422", "protocol": "tcp",
                 "exposedPort": 51422, "description": "pss service",
                 "targetService": {"ip": "127.0.0.1", "port": 55555}}
              ]
            }
          }
        ]
      }
    }
    ```
  - 响应：在线 `{"code":0,...}`；离线暂存 `config_status='pending'`；参数错误 `40001`；Edge 不存在 `40002`
- **查询配置** `POST /api/v1/edges/config/get`
  - body：`{"edge_id": "edge001"}`
  - 响应：返回配置（含 `config_status`/`config_version`）。
    - DB 已有配置 → 直接返回 DB 快照，后台线程安全地 `edge.config.query` 刷新一次；
    - **DB 无配置且 Edge 在线** → **同步**查询一次 edge 本地配置并返回（消除"首次打开 token 空、需等异步回填"的问题），并回填 DB。
    - 详见「异步回填 bug 修复」小节。
- **同步配置** `POST /api/v1/edges/config/sync`（✅ 2026-09-12 新增：以 **edge 前端**为准）
  - body：`{"edge_id": "edge001"}`
  - 语义：与 `config/get`（以**数据库**为准）相反，`config/sync` **以 edge 前端的本地真实配置为准**，展示 edge 实际运行的配置，方便用户在"数据库版本"与"edge 实际版本"间选择。
  - 响应字段：
    - `config`：edge 前端真实配置（frpc 权威结构）；取不到则为 `null`。
    - `differ`：与数据库配置归一化比较结果——`true`(不一致)/`false`(一致)/`None`(无法比较)。
    - `sync_state`：`same`(一致)/`differ`(不一致)/`unreadable`(edge 在线但本地配置不可读或缺失)/`unreachable`(edge 离线或无法通信)。
    - `edge_reachable`：是否成功建立通信（edge 是否"在线可达"）。
    - `edge_unreadable`：edge 在线但本地配置文件缺失/解析失败。
    - `sync_at`：本次同步核对的 UTC **毫秒时间戳**，持久化到 `edge_configs.synced_at`（BigInteger 毫秒），供前端展示"最后同步时间"。
  - 一致性核对逻辑：edge 在线且拿到 config → 与 DB 比较得 `differ`；edge 在线但返回 error（文件缺失）→ `unreadable`；ingress 未连接/超时 → `unreachable`。**注意区分"离线"与"在线但配置不可读"**——二者前端提示不同。
- **迁移说明**：早期版本 `POST/GET /api/v1/edges/<edge_id>/config` 路径式接口与约定冲突，已废弃；代码需迁移到上述 body 式接口后再接入 UI。

**UI 实现**（✅ 已落地）：
- [x] `edges.html` 设备列表每行「操作」列新增「配置」按钮（`settings` 图标），点击跳转 `GET /edges/config?edge_id=<id>`（页面路由，edge_id 走 URL 查询参数——页面导航非 API，不违反"edge_id 放 body"约定）
- [x] 新增独立配置页 `templates/edge_config.html`：配置类型按需分组折叠（当前「SHPC 配置」面板，默认折叠；未来多种类型可平铺并列）
- [x] 表单字段：服务端连接（`endpoint.host/port`）、本地管理（`localManagement.bindAddress/bindPort`）、安全（`security.authMethod`/`credential`/`enableTls`，Token 输入框带明/密文眼睛切换）、访问策略/代理（`accessPolicies[]`，可动态增删行）
- [x] 配置状态显示：顶部 meta 徽章展示 `config_status`/`version`；加载时按状态给不同提示
- [x] 异步刷新：保存后 1s 短轮询 `config/get` 直到 `confirmed` 或 `config_failed`
- [x] **三个动作按钮（每个配置面板内各自独立）**：「从平台加载」(从**数据库**) /「从设备同步」(从 **edge 前端**) /「保存并下发」，遵循"每类配置独立"理念，未来新增配置类型各自带操作按钮：
  - **从平台加载** = `config/get`，以数据库为准（DB→edge 在线回填→后端默认模板），表单展示 DB 值；后端默认模板由 `_default_edge_config()` 定义（serverPort=17400、无 token/proxy、tls 开、webServer.port=17401）。
  - **从设备同步** = `config/sync`，以 edge 前端为准，点击后按钮倒计时 3s（"获取中 (3s)→…"）自动获取一次并展示 edge 真实配置；按 `sync_state` 提示"一致/不一致/格式异常(bad)/配置不存在(missing)/离线不可达"，末尾附最后同步时间。**离线设备不显示此按钮**（由页面路由 `edge_online` 决定，Jinja `{% if edge_online %}` 渲染时隐藏）——离线本就无法同步，故不提供该功能。
  - **保存并下发** = `config/update`。
  - 已移除原「重置」(清空表单) 按钮——"从平台加载"即承担"取回一份配置"的语义。

**交互流程（实际实现）**：
1. 设备列表「配置」按钮 → 跳转独立配置页
2. 页面打开自动 `POST /api/v1/edges/config/get` 加载当前配置；若 DB 空，后端同步查询 edge 本地配置返回、前端兜底重试
3. 用户编辑后点「保存并下发」→ `POST /api/v1/edges/config/update`，显示返回 `code`/`version`
4. 保存后 `config_status` 经历 `configuring → confirmed`，前端轮询直至落下

**关键语义：`config_status` 的 `unknown` 与「以 edge 前端为准」的一致性核对**
- `unknown` = 该 Edge **从未经 manager 下发并确认**（配置可能是从 edge 本地回读/同步的），是正常初始状态。
- **以 edge 前端为准**：`config/get` 加载时会做一致性核对——同时读 DB 配置与 edge 在线本地配置，归一化后比较。核对结果以 `config_sync` 字段返回（见下）。
- `config/get` 响应的一致性核对字段 `config_sync`，取值：
  - `matched` — 在线且 DB 与 edge 本地**一致** → `config_status` 置 `confirmed`，`config_matched=true`。
  - `mismatch` — 在线且 DB 与 edge 本地**不一致** → 保持原状态（`unknown` 仍 `unknown`），`config_matched=false`，前端提示"待同步"。
  - `missing` — 在线但 edge 本地配置文件**不存在/无法打开**（JSON-RPC 顶层 error）→ `config_matched=false`。
  - `bad` — 在线且 edge 本地配置文件存在但**解析失败**（JSON 格式异常，`result.config_error`）→ `config_matched=false`，前端提示"格式异常"。
  - `None` — 离线或查询超时/无响应（无法核对），保持原状态。
- **DB 空**（从未下发过）→ 回填 edge 本地配置并直接返回真实配置，不改 confirmed。
- 「保存并下发」仍走 `下发 → edge 写盘 → AckConfigUpdate → confirmed` 流程（实测：edge007 下发后 `unknown → confirmed`，version 0→1）。
- 页面在 `unknown` 且已有配置时给出明确提示，避免误导。

**一致性核对实现（`edges.py::get_edge_config`）**：
- 初始化 `config_matched_val=None; final_status=edge.config_status`；仅当 Edge 在线且已连接时同步 `query_edge_config(edge_id, timeout=10)`。
- DB 空 → 回填：`if not config_data: config_data = edge_online_cfg ... db.commit()`。
- 比较用归一化权威结构 `_canonical_config()`（两侧都转成 frpc 权威结构再判等）。
- 一致 → `config_matched=True`，若当前非 `confirmed` 则 `_set_edge_config_status(edge_id,'confirmed')`，`final_status='confirmed'`；否则 `config_matched=False`，`final_status` 保持 DB 原状态。
- **区分 edge 本地"缺失"与"格式错"两种不可读情况**：edge 返回 **JSON-RPC 顶层 error**（文件不存在/无法打开）→ `missing`；edge 返回 **success 响应但 `result.config_error`**（文件存在但解析失败）→ `bad`。二者都置 `config_matched=false`、`config_sync` 分别取 `missing`/`bad`，前端据此给出不同提示（此前统一归为 missing，导致 edge002 这类"格式错"被误报为"配置不存在"）。
- **已删除原后台 `refresh_config` 线程**：它曾用线程安全方式（或旧版 `new_event_loop`）无条件用 edge 值覆盖 DB，会在不一致时把 DB 污染成 edge 本地值。删除后 DB 保持权威，不一致时不被覆盖。
- 响应字段：`config_status`（最终状态）、`config_matched`（是否一致）、`config_sync`（matched/mismatch/missing/bad/None）。


**字段映射（配置页表单 → config_content.services[].tunnelService）**：
- `服务器地址/端口` → `endpoint.host / endpoint.port`
- `认证方式/Token` → `security.authMethod / security.credential`
- `启用 TLS` → `security.enableTls`
- `本地管理地址/端口` → `localManagement.bindAddress / bindPort`
- 代理列表（每行：`协议/公网端口/目标 ip:port/策略 ID/描述`）→ `accessPolicies[]`（`protocol / exposedPort / targetService.ip / targetService.port / policyId / description`）

#### 异步回填 bug 修复（2026-09-11）

**现象**：配置页首次打开时，Edge 磁盘上明明有 `shpc.<edge_id>.json`（含 token），但页面 token 显示为空。

**根因（两层）**：
1. **前端时序**：`config/get` 返回的是 **DB 快照**；DB 无记录时返回空表单。Edge 本地配置是通过后台 `edge.config.query` 异步回填 DB 的，首次打开常读到空 DB → token 空。
2. **后端异步 bug（更深）**：原 `refresh_config` 线程用 `asyncio.new_event_loop()` + `run_until_complete`，在**独立新事件循环**里 `await` 一个**绑定在另一个事件循环（`self._loop`）上的 websocket**——**跨事件循环错配**，导致回填延迟数秒到 30+ 秒（实测 edge004 ≈25s、edge005 >40s），页面根本等不到。

**修复**：
- `ingress_client.py` 新增线程安全方法 `query_edge_config(edge_id, timeout)`，用正确的 `asyncio.run_coroutine_threadsafe(..., self._loop)` 模式（与 `query_edge_list` 一致）投递到连接所属事件循环。
- `edges.py::get_edge_config` 改为：**DB 无配置且 Edge 在线 → 同步 `query_edge_config` 一次并直接返回真实配置**（同时回填 DB）；后台异步刷新也改用线程安全方式。
- 前端 `edge_config.html`：加载时若返回空配置，800ms 间隔最多重试 8 次兜底。

**验证**：DB 无配置的 edge008/009/010/011/012 **首次 GET 即返回真实 token**（修复前需等 25–40s）；并发总首次 GET 全部正确；edge001（已有配置 ver 105）不受影响仍正常。

---

### 附录

#### FrpcConfig 结构示例（转换后的扁平格式）

**从 TunnelService 转换后的 frpc JSON 格式**（edge 写入 `./shpc/shpc.<edge_id>.json`）:
```json
{
  "serverAddr": "10.220.42.139",
  "serverPort": 50400,
  "auth": {
    "method": "token",
    "token": "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc"
  },
  "transport": {
    "tls": {
      "enable": true
    }
  },
  "webServer": {
    "addr": "127.0.0.1",
    "port": 17400
  },
  "proxies": [
    {
      "name": "w-tcp-51422",
      "type": "tcp",
      "localIP": "127.0.0.1",
      "localPort": 55555,
      "remotePort": 51422
    }
  ]
}
```

**注意**：这是 Edge 侧写入文件的最终格式。`toFrpcConfig()` 会将 TunnelService 的嵌套结构转换为上述扁平结构。

#### 相关文件
- `libs/come.1/src/come_1.h` - `ConfigUpdate_tunnelService` 定义
- `libs/come.1/src/come.1.json.cpp` - 编解码实现
- `libs/come.1/ut/ut-come.1.json.cpp` - 单元测试
- `src/Function/Touch/Edge/FunRegisterCli.h` - Edge 客户端
- `src/Function/Touch/Ingress/FunRegisterSvr.h` - Ingress 服务端
- `man/touch_manager/api/edges.py` - Edge REST API
- `man/touch_manager/ingress_client.py` - Ingress WebSocket 客户端

---


frpc 远程配置的测试用例（单元测试 `TF-UT-001`~`004`、端到端测试 `TF-IT-001`~`006`、用例总表）详见 [spec-test-cases.md](spec-test-cases.md)「四、frpc 远程配置测试」。

# 安全设计

Touch 系统在 Edge（嵌入式设备）与 Ingress / Manager 之间建立**安全可信的通讯体系**，分两个阶段：

1. **上线认证阶段（连接建立时）**：Edge 出示凭据，Ingress 验证 Edge 身份。
2. **会话通讯阶段（连接建立后）**：通过服务端签发的**会话 token** 维持后续所有消息的安全认证，无需每条消息重复出示原始凭据。

**核心原则（系统原则，简单化）**：

> **所有 token 都由服务端（Ingress）签发**——Edge、Manager 自身都不生成 token，只持有 Ingress 签发的 token。Ingress 是唯一签发方，也是**分段网关**。

**核心模型（touch_token 是分段、点对点的 token）**：

> **`touch_token` 不要求全链路一致**。它是按**通讯段**划分的点对点会话令牌，每段只有该段两侧共享自己的 token，由 Ingress 在段间负责换段（validate 上一段、换成下一段）。即：**分段，解耦**。

```
Manager ───────────────── Ingress ───────────────── Edge
   段① Manager↔Ingress          段② Ingress↔Edge
  touch_token = SHA256("mgr"||t)   touch_token = SHA256(edge_id||t)
  (对 prefix 字节 + 毫秒时间计数字节 + 8 字节随机数求 SHA256 的 hex 摘要)（Ingress connect 时签发 / edge.online 时签发）
            └──────── Ingress = 分段网关：校验段①、换段② ────────┘
```

> **token 格式**（`gen_touch_token(type, prefix)`，`FunRegisterSvr.cpp`）：对 **`prefix` 字节 + 原始毫秒时间计数（`uint64_t`，`SystemGetMSCount()`）字节 + 8 字节随机数（`ez_rand_buf`，libezutil）**做 **SHA256**，输出 64 位 hex 摘要（sha256.h，libezutil）；数据分次置入 `sha256_update`，不拼接中间字符串。Edge 段 prefix = `edge_id`，Manager 段 prefix = `"mgr"`。随机数使 token 每次签发不可预测，**防重放/防破解**；且 token 是**摘要**，不直接暴露前缀/时间/随机信息。

## 一、上线认证：key + nonce + sign

### 字段与角色

| 字段 | 类型 | 角色 | 说明 |
|------|------|------|------|
| `key` | string | 认证密钥 | Edge 的身份凭证（`EdgeOnline.key`） |
| `nonce` | string | 挑战随机数 | 防重放/挑战值，参与认证计算 |
| `sign` | string | 校验数据（签名） | 根据 `key`、`id`、`nonce` 等计算得到的校验数据（`EdgeOnline.sign`，原 `EdgeOnline.token` 更名） |

**设计意图**：Edge 上线时携带 `key + nonce + sign` 三要素，构成**挑战-响应式**认证，使 Ingress 能：
- 用 `key` 识别并验证 Edge 身份；
- 用 `nonce` 防止重放攻击（每次连接用新鲜随机数）；
- 用 `sign`（由 `key`、`id`、`nonce` 计算得出）作为**校验数据/签名**，供服务端验证请求的完整性与真实性。

> 注：`sign` 的语义是"根据 `key`、`id`、`nonce` 等推导出的校验数据"（签名），与**分段会话 token（`touch_token`）**是不同概念：`sign` 用于上线认证时的即时校验，`touch_token` 用于认证后的会话保持。完整落地时还需考虑时间戳、签名算法等组合。

## 二、会话保持：服务端签发的分段 token

### 核心思想

认证通过后，由**服务端（Ingress）签发一个会话 token** 返回给该段对端（Edge 或 Manager）。对端在**该段后续所有消息**中携带，Ingress 据此校验会话合法性，**无需每条消息重复出示 `key`/`token`**。

### token 的承载字段：`MsgCome::touch_token`（分段、点对点）

> **定名**：会话 token 的承载字段统一命名为 **`touch_token`**（当前代码已全栈 rename 为该名，`MsgCome` 基类，come_1.h:28）。

`MsgCome` 是 come 协议栈的**基类**，几乎所有 come 消息都继承它（come_1.h:28）：

```cpp
class MsgCome {
public:
    std::string touch_token;   // 会话令牌（服务端签发）；分段点对点，各段值不同
    ...
};
```

**`MsgCome::touch_token` 是分段的会话 token 承载字段**，理由：

1. **全局携带**：作为基类字段，`touch_token` 天然出现在每条 come 消息上（心跳、配置、上报…），完全符合"后续消息统一携带会话凭据"的特性。
2. **服务端（Ingress）签发**：Ingress 在每段对端连接建立时生成并返回——
   - **Edge 段**：`edge.online` 时生成（`FunRegisterSvr.cpp`，为 `edge_id` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex 摘要），经 `AckEdgeOnline` 返回给 Edge；
   - **Manager 段**：`manager.connect` 时生成（为 `"mgr"` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex 摘要），存 `m_manager_token` 并经 `manager.connect` 响应返回给 Manager。
3. **分段、点对点**：**不要求全链路一致**。Manager→Ingress 段携带 Manager 段 token；Ingress 透传下行消息（如 `config.update`）前，将该字段**换为**目标 Edge 的 Edge 段 token 再转发——两端值不同、互不共享。
4. **Ingress = 唯一签发方 + 分段网关**：校验上一段 token（如收到 `config.update` 时校验其 touch_token == `m_manager_token`），再换成下一段 token。

### 命名建议（最终定名：`touch_token`）

**`touch_token` 的语义定义**：

> **`touch_token`（Touch 系统会话令牌，分段点对点）**：由服务端（Ingress）在每段对端连接建立时签发（Edge 段经 `AckEdgeOnline`、Manager 段经 `manager.connect` 响应返回）、由对端在**该段此后所有 come 消息**中携带、由 Ingress 校验并在段间换段的**点对点会话令牌**。其作用等价于 Web 会话中的 cookie：对端无需在每条消息重复出示原始凭据，Ingress 据此校验该段会话合法性。**各段 token 值不同，不要求全链路一致（分段解耦）**。

- `touch_token` 命名来源：`touch` 前缀表明其属 **Touch 系统 / come 协议栈**的通用会话凭据，与既有各 `token`（上线认证 `token`、frpc `auth.token`）明确区分。
- 此字段为**最终定名**；备选名 `session_token` / `conn_token` 语义相近，均能体现"会话"定位，但不采用。
- **不宜简化为 `token`**：会与上线认证用的 `token`、配置体内的 `auth.token` 撞名混淆（详见「命名冲突风险」）。

## 三、两阶段的一体化流程（分段）

### 3.1 段① Edge↔Ingress 上线认证 + 会话保持

```
Edge                          Ingress
 │  ① edge.online             │
 │  key + nonce + sign ──────► │  ② verify_device(id,key,sign)
 │                            │  用 sign 校验 key+nonce 等
 │                            │  ③ 签发 Edge 段 touch_token
 │  ◄── AckEdgeOnline ─────── │    (SHA256 hex)
 │    {touch_token}           │
 │  ④ 保存 touch_token        │
 │  ⑤ 后续消息                │  ⑥ 校验本段 touch_token
 │  edge.heartbeat/config     │    == 签发值
 │  touch_token ────────────► │
```

### 3.2 段② Manager↔Ingress 会话保持 + config 下行换段

```
Manager                        Ingress
 │  ① manager.connect (请求,id) │
 │  (manager_id/ingress_id) ──► │  ② 签发 Manager 段 touch_token
 │                             │    (SHA256 hex)，存 m_manager_token
 │  ◄── manager.connect 响应 ── │  返回 {touch_token}
 │    {touch_token}            │
 │  ③ 保存 manager_token       │
 │  ④ 下行 config.update       │  ⑤ 校验本段 touch_token
 │  touch_token ─────────────► │    touch_token == m_manager_token
 │                             │  ⑥ 换段：touch_token = <edge_id> 段 token
 │                             │  ──────────────────────────────► Edge
```

> 图中 `touch_token` 为**分段点对点**：段① Edge 用 Edge 段 token、段② Manager 用 Manager 段 token，两值不同、互不共享；Ingress 在段②→段①转发（如 `config.update`）时**校验 Manager 段 token 并换成 Edge 段 token**。

## 四、现状与差距（2026-09-18 核对源码）

| 环节 | 当前实现 | 达标度 |
|------|---------|--------|
| `key` 认证 | Ingress 用 `key` 调 `verify_device`，但**仅检查非空即通过**（`FunRegisterSvr.cpp` 无密码学校验；待实现，见缺口清单 2） | ⚠️ 名义实现 |
| `nonce` 生成 | Edge `RegisterEdge` 用 **`ez_rand_buf` 生成 16 字节 CSPRNG nonce**（hex 32 字符）；随机源失败退回时间戳保证非空、注册不阻塞 | ✅ 已实现 |
| `nonce` 防重放 | Edge 生成新鲜 nonce 且**参与 sign 计算**（Ingress 在 sign 复核时消费 nonce）；但**尚未做一次性/留存防重放校验**（真正挑战-响应待实现，见缺口清单 3） | ⚠️ 部分（消费 nonce 算 sign，未做防重放留存） |
| `sign` 认证（原 `token`） | `EdgeOnline.sign` 已 settle 字段名；**Edge 已按 `SHA256(key+id+nonce)` 计算 sign 随上线发送，Ingress 用消息内 `key` 重算复核**（不符返回 40001 并关闭连接）——**弱自洽**校验 | ✅ 已实现（弱自洽；权威校验待 `verify_device`） |
| Edge 段 `touch_token` 签发 | Ingress 在 `edge.online` 时生成（`edge_id` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex）经 `AckEdgeOnline` 返回，并存入 `m_online_edges[client_id].touch_token` | ✅ 已实现 |
| Edge 段 `touch_token` 携带 | Edge 存 `m_token`，心跳带着走（`FunRegisterCli.cpp`） | ✅ 已实现 |
| Edge 段 `touch_token` 校验 | Ingress 收 Edge 心跳时**校验 msg.touch_token == 该 edge 的签发值**（不符则断开）；Edge 收下行 `config.update` 时**校验 touch_token == 本 Edge 会话 token**（不符则拒收） | ✅ 已实现 |
| **Manager 段 `touch_token` 签发** | Ingress 在 `manager.connect` 时生成（`"mgr"` 字节 + 时间计数 + 8 字节随机数的 SHA256 hex）存 `m_manager_token`，经 connect 响应返回给 Manager；Manager 缓存于 `ingress_client.manager_token` | ✅ 已实现 |
| **Manager 段 `touch_token` 校验 + 换段** | Ingress 收 `config.update` 时**校验其 touch_token == `m_manager_token`**（不符则拒绝），透传前**换成**目标 Edge 的 Edge 段 token | ✅ 已实现 |

> **注**：`access_token → touch_token` rename 已全栈落地（come.1 / Edge / Ingress / Manager），本表直接使用 `touch_token` 名。

### 缺口清单（待开发）

1. ~~**落地 Edge 段 `touch_token` 校验**~~（✅ 2026-09-18 已实现）：Ingress 收 Edge 心跳校验 `msg.touch_token == 签发值`（不符断开）；Edge 收下行 `config.update` 校验 `touch_token == 本 Edge 会话 token`（不符拒收）。`ez_rand_buf` 随机源失败时**跳过随机盐**（仅用 prefix+时间）并记 err 日志，仍正常签发 token（`gen_touch_token` 恒返回非空，避免阻塞）。
2. **补全 `verify_device` 的密码学校验**（待实现）：当前 `verify_device(edge_id, device_key)` 仅"非空即过"（名义实现）。真正落地需建立**权威 edge→key 存储**（当前 Ingress 无此表，Manager DB `Edge.edge_key` 列在 WS 上线路径写入为空，且 Ingress→Manager 消息未携带 key/nonce/sign）——方案可以是 Ingress 本地共享密钥表 / 配置，或由 Manager 权威校验后回传。**在此之前，`sign` 只能做"弱自洽"（用消息内 `key` 复核），证明发送者能算 hash，非共享密钥认证**。
3. **落地 `nonce` 的挑战-响应 / 一次性防重放**（待实现）：当前 nonce 由 Edge 自生成并参与 sign 计算；Ingress 尚未做 nonce 一次性使用 / 留存校验（防同一 nonce+sign 重放）。
4. ~~**落地 `sign` 校验数据**~~（✅ 2026-09-18 已实现，弱自洽）：Edge 已按 `SHA256(key+id+nonce)` 计算 `sign` 随上线发送（`FunRegisterCli.cpp`，分段 `sha256_update`，无中间拼接串）；Ingress 在 `handle_device_online_jsonrpc2` 中，若 `msg.sign` 非空则用消息内 `key` 重算复核，不符返回 error 40001 并 `CloseClient`。**待 `verify_device` 权威 key 存储就位后升级为共享密钥认证**。

> **验证记录（2026-09-18 功能 + 性能实测）**——本轮新增 sign 上线认证 + Edge 段 token 校验闭环后的全面验证：
>
> **功能 ✓**
> - **Edge 上线（含 sign 校验）**：edge001 用正确 sign 上线成功，Manager DB 落库 `status=1(online)`、`confirmed=99`，Ingress 日志 `[EDGE.ONLINE] Edge registered successfully`，Edge 收到 `Register success` + 下发的 Edge 段 touch_token，心跳正常运行。
> - **远程配置全链路**：`POST /api/v1/edges/config/update` → Ingress 校验 Manager 段 token + 注入 Edge 段 token → Edge `[CONFIG.UPDATE.RECV]` + `Verified edge touch_token in config.update OK` + 转 PresetFrpcConfig 写盘 `./shpc/shpc.edge001.json`（`auth.token`/proxies 正确）→ ack → Manager `config_status=confirmed`（version=1）。
> - **配置查询**：`config/get` → `config_matched=true`、`config_status=confirmed`、`config_sync=matched`、`config_version=1`。
> - **sign 负向（自洽性）**：正确 sign → `Register success`（`code:0`）；篡改 sign（全 0）→ **error 40001 "Device sign verification failed"** 并关闭连接（1006）；sign 截断（长度不符）→ 同样 40001 拒绝。三例全部符合预期。
> - **Manager 端段 token**：manager.connect 时 Ingress 签发 Manager 段 token，Manager 缓存，用于下行 config.update。
>
> **性能 ✓（无 sign 引入的可观测开销）**
> - **单次配置推送全链路（POST→confirmed）**：10 轮实测 150–228 ms（均值 ~175 ms）；POST→Manager API 响应 49–95 ms（均值 ~61 ms）。
> - **连续 10 次推送（丢/乱序）**：服务端成功分配 10 个版本，Edge 功能日志 **RECV 10 次 + WRITE 10 次**，RECV 时间戳严格单调 → **0 丢失、0 乱序**；最终 `config_status=confirmed`、`config_version=11`。
> - **转发及时性**：Ingress `handle_config_update_jsonrpc2` 处理 0–2 ms、`handle_config_ack_jsonrpc2` 转发 0–1 ms。
> - **sign/nonce 计算开销**：`SHA256(key+id+nonce)` + hex 单次 ~1.67 µs，相对全链路 ~150 ms 可忽略不计。
> - **配置查询往返**：`config/get` ~35 ms。
>
> **⚠️ 已知限制（2026-09-18 记录，已修正）：日志中 token/sign 曾显示截断——真实原因是代码 `substr`，非框架**
> - **现象**：`_ingress.log` 的 `[HEARTBEAT] ... touch_token=a9cc52024e4708a5f5d8]` 只落盘 token 前 20 字符。
> - **根因（已定位并修正）**：`FunRegisterSvr.cpp` 心跳解码成功日志里用了 `msg.touch_token.substr(0, 20).c_str()` 主动截断，**并非 ezlog 框架限制**。此前误判为"ezlog 文件写档 ~190 字节上限"，已证伪：`ez_log.c` 中 `EZ_LOG_MAX_LINE_SIZE=1024*128`（128 KB）是所有行的 `logstr` 静态缓冲上限，`_vsnprintf` 每行可写足 128 KB，与 20 字符截断无关。
> - **修正**：该处已改为 `%s` 直接打印完整 `msg.touch_token`，其余 token/sign/nonce 日志已全部用完整 `.c_str()`。
> - **说明**：Touch 代码中剩余的 `%.100s`/`%.200s` 仅用于**原始 JSON 报文**日志（`json=`/decode 失败），是有意的安全宽度上限（防 dump 任意超大消息），不作用于长度可知的 sign/token，符合"长度可知的都打全"要求。
> - **关于 `EZ_LOG_MAX_LINE_SIZE` 的取值**：`#define EZ_LOG_MAX_LINE_SIZE 1024*128`（注释掉的 `1024*1024*2`）是单行日志缓冲上限，128 KB 对普通单行足够且 `logstr` 为进程级全局静态、内存占用小；扩到 2 MB 会永久占用 2 MB 且无实际收益，故保留 128 KB。与 token 截断无因果关系。

## 五、命名冲突风险

come 协议栈中已存在多个 `token`，语义不同，须避免撞名：

| 字段 | 位置 | 语义 | 状态 |
|------|------|------|------|
| `EdgeOnline.sign`（原 `EdgeOnline.token`） | come_1.h:78 | 上线认证校验数据（由 `key`/`id`/`nonce` 计算） | 未使用（原 `token` 恒发 `""`） |
| `FrpcConfig.token` / `auth.token` | come_1.h:360 | frpc 服务端认证令牌 | 已使用 |
| `SvConfig.token` | come_1.h:464 | 配置认证令牌 | 已使用 |
| `AckEdgeRegister.token` | come.1.json.cpp:1214/1229 | Edge↔Manager 独立注册应答返回的 token | 独立消息，与上线认证/会话无关 |

**上线认证字段重命名：`EdgeOnline.token` → `EdgeOnline.sign`**

`EdgeOnline` 中用于上线认证的字段由 `token` 更名为 **`sign`**，语义为"根据 `key`、`id`、`nonce` 等得到**校验数据**（签名）"。更名后该字段**不再是 `token` 一族**，从而与 `FrpcConfig.token`、`SvConfig.token`、`AckEdgeRegister.token`、`touch_token` 彻底区分，避免撞名。

**`EdgeOnline.token → sign` 的 rename 影响面（上线消息，wire 协议变更，需 Edge+Ingress 同步升级）**：
- `libs/come.1/src/come_1.h`：`EdgeOnline.token` 字段（L78）与构造函数形参（L82-83，`_token`/`token(_token)`）→ `sign`
- `libs/come.1/src/come.1.json.cpp`：`EdgeOnline` 编解码中 JSON key `"token"`（L117 encode、L126 decode）→ `"sign"`
- `src/Function/Touch/Edge/FunRegisterCli.cpp`：L142 `edge_online.token = "";` → `edge_online.sign = <由 key/id/nonce 计算>;`
- **不含** `AckEdgeRegister.token`、`FrpcConfig.token`（两者本就与 `EdgeOnline.token` 无关）

**命名结论（最终定名）：`MsgCome::access_token` → `MsgCome::touch_token`**

代表 **会话 cookie** 的 `MsgCome::access_token` **不更名为 `token`**（避免与 `auth.token` 等撞名混淆），最终定名为 **`touch_token`**。该名以 `touch` 前缀区分于既有各 `token`，明示其属 Touch 系统的会话令牌定位（语义定义见「命名建议（最终定名：touch_token）」）。

**rename 影响面（`access_token` → `touch_token`，全栈 wire 协议变更，三组件需同步升级）**：
- `libs/come.1/src/come_1.h`：`MsgCome.access_token` 及所有派生消息字段
- `libs/come.1/src/come.1.json.cpp`：JSON key `"access_token"` 的编解码（约 8 处）
- `libs/come.1/ut/ut-come.1.json.cpp`：单测断言（约 10 处）
- `src/Function/Touch/Edge/FunRegisterCli.cpp`：Edge 持有/发送
- `src/Function/Touch/Ingress/FunRegisterSvr.cpp`：Ingress 签发/校验
- `man/touch_manager/api/edges.py`、`man/touch_manager/ingress_client.py`：Manager 侧
- 文档（spec-api*.md、spec-test-cases.md 等协议示例）与测试脚本中的 wire 字段

# 文件列表

项目结构（架构层）：
```text
touch/
├── spec/                 # 规范文档目录
│   ├── spec-design.md   # 架构设计文档
│   ├── spec-api.md      # 接口文档
│   ├── spec-roadmap.md  # 开发路线图
│   └── spec-touch.md    # 整体结构说明
├── src/                 # 源代码目录， 每层的代码分别放置， 不要交叉
│   ├── touchEdge/      # Edge（边缘边缘）
│   ├── touchIngress/   # Ingress Service（接入服务）
│   └── touchManager/   # Management Service（管理服务）
└── README.md            # 项目说明文档
```

# 一般约定

1. 代码中非注释内容使用英文，全文不要出现 emoji 符号
2. README.md 只包括项目功能介绍、接口介绍、使用介绍等，不要介绍细节，保持简洁明的特点
3. 生成依赖项管理文件（requirements.txt、package.json、CMakeLists.txt 等）
4. 遵循项目的代码规范和最佳实践


> 全部测试用例（REST API / WebSocket / 边缘生命周期与管理 / frpc 远程配置 / 压力）统一维护在 [spec-test-cases.md](spec-test-cases.md)，此处不再重复展开。
# 接口文档

详细接口文档请参考：`spec-api.md`

# 开发计划

开发计划和路线图请参考：`spec-roadmap.md`
