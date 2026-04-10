
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
    - 接收返回：access_token、expires_in 等
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
   │   └─> 返回 AckEdgeOnline(code=0, success=true, access_token, ...)
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
cd $HOME/svn/daily/src/appmodule/touch/come.1
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

# 测试用例

## 测试概述

### 测试分类逻辑

测试用例按功能模块和业务流程分类，确保覆盖系统的主要特性和关键场景：

| 分类 | 测试目的 | 覆盖场景 |
|------|---------|---------|
| **边缘上线** | 验证边缘正常上线流程 | 新边缘注册、Manager 确认、Ingress 接受 |
| **边缘下线** | 验证边缘正常下线流程 | 边缘断开、Ingress 通知、状态更新 |
| **黑名单管理** | 验证边缘黑名单功能 | 加入黑名单、拒绝上线、移出黑名单 |
| **心跳机制** | 验证心跳保活功能 | 定时心跳、心跳应答、断线重连 |
| **Manager 离线** | 验证 Manager 离线场景 | 已在线边缘保持、新边缘拒绝 |
| **边界条件** | 验证异常和边界情况 | 重复注册、边缘验证失败、网络超时 |

### 测试环境要求

- touchIngress 运行在 127.0.0.1:54321
- touchManager 运行在 127.0.0.1:18050
- 测试边缘：edge001 ~ edge010
- 辅助脚本目录：`st/`

---

## 测试用例总表

| 编号 | 分类 | 测试用例 | 前置条件 | 测试步骤 | 预期结果 | 优先级 |
|------|------|---------|---------|---------|---------|--------|
| TC-001 | 边缘上线 | 新边缘正常上线 | Manager 和 Ingress 运行 | 1. 启动 edge 边缘<br>2. 发送 EdgeOnline<br>3. 等待 Manager 确认 | 1. 收到 code=0<br>2. 边缘状态 online | P0 |
| TC-002 | 边缘上线 | 黑名单边缘上线 | 边缘已被列入黑名单 | 1. 启动 edge 边缘<br>2. 发送 EdgeOnline | 1. 收到 code=40001<br>2. 连接关闭 | P0 |
| TC-003 | 边缘上线 | Manager 离线时上线 | Manager 停止运行 | 1. 停止 Manager<br>2. 启动 edge 边缘 | 1. 收到 code=-2<br>2. 连接关闭 | P1 |
| TC-004 | 边缘下线 | 边缘正常下线 | 边缘已在线 | 1. 停止 edge 进程<br>2. 等待 Ingress 检测 | 1. 边缘状态 offline<br>2. 通知 Manager | P0 |
| TC-005 | 边缘下线 | 边缘重连 | 边缘已在线 | 1. 重启 edge 进程<br>2. 重新连接 | 1. 旧连接关闭<br>2. 新连接建立 | P1 |
| TC-006 | 黑名单 | 加入黑名单 | 边缘已在线 | 1. Web UI 点击拒绝<br>2. 调用 reject API | 1. rejected=True<br>2. 边缘被踢掉 | P0 |
| TC-007 | 黑名单 | 移出黑名单 | 边缘在黑名单 | 1. Web UI 点击确认<br>2. 调用 confirm API | 1. rejected=False<br>2. confirmed=True | P0 |
| TC-008 | 黑名单 | 黑名单边缘无法上线 | 边缘在黑名单 | 1. 启动 edge 边缘<br>2. 尝试上线 | 1. 返回 403<br>2. 连接关闭 | P0 |
| TC-009 | 心跳 | 定时心跳发送 | 边缘已上线 | 1. 等待 30 秒<br>2. 查看日志 | 1. 每秒发送心跳<br>2. 收到应答 | P1 |
| TC-010 | 心跳 | 心跳超时处理 | 边缘已上线 | 1. 停止 Ingress<br>2. 观察 edge 行为 | 1. 心跳失败<br>2. 尝试重连 | P2 |
| TC-011 | Manager 离线 | 已在线边缘保持 | 边缘已上线 | 1. 停止 Manager<br>2. 观察边缘状态 | 1. 边缘保持 online<br>2. 心跳正常 | P0 |
| TC-012 | Manager 离线 | 新边缘拒绝 | Manager 已停止 | 1. 启动新 edge<br>2. 尝试上线 | 1. 返回失败<br>2. 连接关闭 | P0 |
| TC-013 | 边界条件 | 重复注册 | 边缘已在线 | 1. 启动相同 edge_id<br>2. 尝试上线 | 1. 返回 code=-3<br>2. 拒绝连接 | P1 |
| TC-014 | 边界条件 | 边缘验证失败 | 边缘 key 错误 | 1. 使用错误 key<br>2. 尝试上线 | 1. 返回 code=-1<br>2. 连接关闭 | P1 |
| TC-015 | 边界条件 | HTTP 超时 | Manager 响应慢 | 1. 模拟 Manager 延迟<br>2. 尝试上线 | 1. 超时断开<br>2. 返回失败 | P2 |
| TC-016 | 黑名单 | 拉黑在线边缘后连接断开 | 边缘已在线 | 1. 调用 reject API<br>2. 等待 3 秒<br>3. 检查边缘进程 | 1. confirmed=0<br>2. 边缘连接被 Ingress 断开<br>3. 边缘进程退出或尝试重连 | P0 |
| TC-017 | EdgeReportMode | 增量上报（模式 2） | Ingress EdgeReportMode=2 | 1. 启动 Ingress（模式 2）<br>2. 启动 3 个 Edge<br>3. 启动 Manager<br>4. 查询在线 Edge<br>5. 断开 Edge 002<br>6. 查询离线缓存<br>7. 验证 Manager 收到离线通知 | 1. Manager 连接后只上报变化的 Edge<br>2. 离线 Edge 被缓存<br>3. Manager 收到离线通知<br>4. 缓存确认后删除 | P0 |
| TC-018 | EdgeReportMode | 查询离线缓存 | Ingress 有离线缓存 | 1. 发送 manager.edge.offline.list 请求<br>2. 验证响应格式 | 1. 返回离线设备列表<br>2. 包含 edge_id、offline_time、pending_confirm | P1 |

---

## 详细测试用例

### TC-001: 新边缘正常上线

**测试目的**：验证新边缘能够正常注册并上线

**前置条件**：
- touchIngress 运行在 127.0.0.1:54321
- touchManager 运行在 127.0.0.1:18050
- 测试边缘 edge001 未在黑名单中

**测试步骤**：
```bash
cd st/
./test_edge_online.sh edge001 key001
```

**预期结果**：
1. edge 成功连接 ingress
2. 收到 `AckEdgeOnline(code=0, success=true)`
3. Manager 中边缘状态为 `status=online, confirmed=False, rejected=False`
4. 边缘开始发送心跳

**通过标准**：
- 边缘成功上线
- 心跳正常发送

---

### TC-002: 黑名单边缘上线

**测试目的**：验证被列入黑名单的边缘无法上线

**前置条件**：
- touchIngress 和 touchManager 运行正常
- 边缘 edge002 已被列入黑名单（rejected=True）

**测试步骤**：
```bash
# 1. 将边缘加入黑名单
curl -X POST http://localhost:18050/api/v1/edges/reject \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge002"}'

# 2. 启动边缘尝试上线
./test_edge_online.sh edge002 key002
```

**预期结果**：
1. ingress 调用 Manager API
2. Manager 返回 `code=40001, rejected=True`
3. ingress 发送 `AckEdgeOnline(code=40001, success=false)`
4. ingress 关闭边缘连接

**通过标准**：
- 边缘被拒绝
- 连接被关闭

---

### TC-006: 加入黑名单

**测试目的**：验证管理员可以将边缘加入黑名单

**前置条件**：
- 边缘 edge003 已在线
- Web UI 可访问

**测试步骤**：
```bash
# 1. 查看边缘当前状态
curl -X POST http://localhost:18050/api/v1/edges/get \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge003"}'

# 2. 将边缘加入黑名单
curl -X POST http://localhost:18050/api/v1/edges/reject \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge003"}'

# 3. 查看边缘状态
curl -X POST http://localhost:18050/api/v1/edges/get \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge003"}'

# 4. 观察边缘是否被踢掉
./test_edge_status.sh edge003
```

**预期结果**：
1. 边缘状态变为 `rejected=True, status=offline`
2. ingress 关闭边缘连接
3. 边缘尝试重连时被拒绝

**通过标准**：
- rejected=True
- 边缘被踢掉
- 无法重新上线

---

### TC-007: 移出黑名单

**测试目的**：验证管理员可以将边缘从黑名单移出

**前置条件**：
- 边缘 edge003 在黑名单中（rejected=True）

**测试步骤**：
```bash
# 1. 将边缘移出黑名单
curl -X POST http://localhost:18050/api/v1/edges/confirm \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge003"}'

# 2. 查看边缘状态
curl -X POST http://localhost:18050/api/v1/edges/get \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge003"}'

# 3. 启动边缘
./test_edge_online.sh edge003 key003
```

**预期结果**：
1. 边缘状态变为 `rejected=False, confirmed=True`
2. 边缘可以正常上线
3. 收到 `code=0, success=true`

**通过标准**：
- rejected=False
- confirmed=True
- 边缘成功上线

---

### TC-011: Manager 离线时已在线边缘保持

**测试目的**：验证 Manager 离线不影响已在线边缘

**前置条件**：
- 10 个边缘已在线
- Manager 和 Ingress 运行正常

**测试步骤**：
```bash
# 1. 记录当前在线边缘数
curl -X POST http://localhost:18050/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"status": "online"}' | python3 count_online.py

# 2. 停止 Manager
pkill -f "python app.py"

# 3. 等待 60 秒
sleep 60

# 4. 查看边缘状态
curl -X POST http://localhost:18050/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"status": "online"}' | python3 count_online.py

# 5. 启动 Manager
./start_manager.sh
```

**预期结果**：
1. Manager 离线后边缘保持 online
2. 心跳正常（Ingress 本地处理）
3. Manager 恢复后边缘状态同步

**通过标准**：
- 边缘保持在线
- 心跳正常

---

### TC-016: 拉黑在线边缘后连接断开

**测试目的**：验证将已在线的边缘加入黑名单后，Ingress 会主动断开与该边缘的连接

**前置条件**：
- touchIngress 运行在 127.0.0.1:54321
- touchManager 运行在 127.0.0.1:18051
- 测试边缘 testedge001 已在线且 confirmed=99（待确认状态）

**测试步骤**：
```bash
# 1. 确认边缘当前在线状态
curl -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "testedge001"}'

# 2. 确认边缘进程正在运行
ps aux | grep "touch_edge-linux.*testedge001" | grep -v grep

# 3. 将边缘加入黑名单
curl -X POST http://localhost:18051/api/v1/edges/reject \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "testedge001"}'

# 4. 等待 3 秒让 Ingress 处理踢出命令
sleep 3

# 5. 检查边缘进程是否已断开连接
ps aux | grep "touch_edge-linux.*testedge001" | grep -v grep

# 6. 查询边缘状态，确认 confirmed=0 且 status=offline
curl -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{}' | python3 -c "import sys,json; data=json.load(sys.stdin); edge=[e for e in data['edges'] if e['edge_id']=='testedge001'][0]; print(f'confirmed={edge[\"confirmed\"]}, status={edge[\"status\"]}')"
```

**预期结果**：
1. 步骤 1 返回边缘状态 `status=1`（在线），`confirmed=99`（待确认）
2. 步骤 2 显示边缘进程正在运行
3. 步骤 3 返回 `{"code": 0, "message": "success"}`
4. 步骤 5 边缘进程已退出或正在尝试重连（进程参数变化）
5. 步骤 6 返回 `confirmed=0`（黑名单），`status=0`（离线）

**通过标准**：
- 边缘被成功拉黑（confirmed=0）
- Ingress 主动断开与边缘的 WebSocket 连接
- 边缘进程退出或检测到连接断开并尝试重连时被拒绝

**注意事项**：
- 此测试验证 Manager → Ingress 的 `manager.edge.kick` 命令是否正确传递 `edge_id` 参数
- 历史上曾因参数名错误（使用 `device_id` 而非 `edge_id`）导致踢出命令失效
- 必须确保 Ingress 收到 kick 命令后调用 `kick_device(edge_id)` 断开连接

---

### TC-017: 增量上报（EdgeReportMode=2）

**测试目的**：验证 EdgeReportMode=2 时，Manager 连接后只上报变化的 Edge

**前置条件**：
- touchIngress 配置 `EdgeReportMode=2`
- touchManager 未启动
- 测试边缘 edge-test-001、edge-test-002、edge-test-003

**测试步骤**：
```bash
# 1. 启动 Ingress（EdgeReportMode=2）
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
./touchIngress-linux &

# 2. 启动 3 个 Edge
./test_edge-linux --host 127.0.0.1 --port 54321 -i edge-test-001 -k key001 &
./test_edge-linux --host 127.0.0.1 --port 54321 -i edge-test-002 -k key002 &
./test_edge-linux --host 127.0.0.1 --port 54321 -i edge-test-003 -k key003 &
sleep 3

# 3. 启动 Manager
cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager
source wpyenv/bin/activate
DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py &
sleep 5

# 4. 查询在线 Edge
curl -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"status": "online"}'

# 5. 断开 Edge 002
pkill -f "test_edge-linux.*edge-test-002"
sleep 2

# 6. 查询离线缓存（通过 WebSocket）
python3 -c "
import asyncio, json, websockets
async def query():
    async with websockets.connect('ws://127.0.0.1:54321/come') as ws:
        req = {'jsonrpc': '2.0', 'method': 'manager.edge.offline.list', 'params': {'offset': 0, 'limit': 50}, 'id': 1}
        await ws.send(json.dumps(req))
        resp = await asyncio.wait_for(ws.recv(), timeout=5)
        print(json.dumps(json.loads(resp), indent=2))
asyncio.run(query())
"

# 7. 验证 Manager 收到离线通知
curl -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{}'
```

**预期结果**：
1. Manager 连接后，Ingress 上报所有在线 Edge（edge-test-001、002、003）
2. Edge 002 断开后，Ingress 将其移动到离线缓存
3. Manager 收到 ingress.edge.offline 通知
4. 查询离线缓存返回 edge-test-002，pending_confirm=true
5. Manager 确认后，离线缓存删除 edge-test-002

**通过标准**：
- Manager 连接后只上报变化的 Edge
- 离线 Edge 被正确缓存
- Manager 收到离线通知并确认
- 缓存确认后删除

---

### TC-018: 查询离线缓存

**测试目的**：验证 manager.edge.offline.list 接口返回正确的离线设备列表

**前置条件**：
- touchIngress 有离线缓存（Edge 断开但 Manager 未确认）
- touchManager 已连接

**测试步骤**：
```bash
# 1. 发送查询请求
python3 -c "
import asyncio, json, websockets
async def query():
    async with websockets.connect('ws://127.0.0.1:54321/come') as ws:
        req = {'jsonrpc': '2.0', 'method': 'manager.edge.offline.list', 'params': {'offset': 0, 'limit': 50}, 'id': 1}
        await ws.send(json.dumps(req))
        resp = await asyncio.wait_for(ws.recv(), timeout=5)
        data = json.loads(resp)
        print(f\"离线设备数量: {data['result']['total']}\")
        for d in data['result']['offline_devices']:
            print(f\"  - {d['edge_id']}: offline_time={d['offline_time']}, pending_confirm={d['pending_confirm']}\")
asyncio.run(query())
"
```

**预期结果**：
1. 返回 JSON-RPC 2.0 响应
2. 包含 offline_devices 数组
3. 每个设备包含：edge_id、offline_time、pending_confirm、edge_type
4. total 字段等于离线设备总数

**通过标准**：
- 接口返回正确的离线设备列表
- 响应格式符合 JSON-RPC 2.0 规范

---

## 测试脚本目录结构

```
st/                          # 测试脚本目录
├── README.md               # 测试说明
├── common.sh               # 公共函数库
├── test_blacklist.sh       # 黑名单测试
├── test_edge_online.sh   # 边缘上线测试
└── run_all_tests.sh        # 运行所有测试
```

## 测试执行结果

### 测试环境
- 日期：2026-02-27
- touchIngress: 127.0.0.1:54321
- touchManager: 127.0.0.1:18050

### TC-006: 加入黑名单测试

**测试步骤**：
1. 启动 edge003
2. 验证边缘在线
3. 调用 reject API
4. 验证边缘状态

**测试结果**：
```
边缘状态：edge003: status=online, confirmed=True, rejected=False
PASS: 边缘上线成功

加入黑名单后状态：edge003: status=offline, confirmed=False, rejected=True
PASS: 已加入黑名单
```

**结论**：✅ 通过

### TC-007: 移出黑名单测试

**测试步骤**：
1. 验证边缘在黑名单中
2. 调用 confirm API
3. 验证边缘状态

**测试结果**：
```
当前状态：edge003: status=offline, confirmed=False, rejected=True
移出后状态：edge003: status=online, confirmed=True, rejected=False
PASS: 已移出黑名单
```

**结论**：✅ 通过

### 测试总结

| 测试用例 | 结果 | 说明 |
|---------|------|------|
| TC-006: 加入黑名单 | ✅ PASS | 边缘被正确加入黑名单，状态更新为 rejected=True |
| TC-007: 移出黑名单 | ✅ PASS | 边缘被正确移出黑名单，状态更新为 rejected=False, confirmed=True |

所有执行的测试均通过，黑名单功能工作正常。

# 接口文档

详细接口文档请参考：`spec-api.md`

# 开发计划

开发计划和路线图请参考：`spec-roadmap.md`
