## v0.1（MVP）
- [ ] 完成三类网元的架构
- [ ] 三个网元的基本通讯联通
- [ ] touch_edge ：向touch_ingress网元接入：连接 + 心跳 + 上线（发送自己的边缘信息：ID、类型等）
- [ ] touch_ingress ：边缘接入
- [ ] touch_manager ：提供接口和页面配置管理touch_ingress； 

## v0.2
- [ ] touch_manager的Web UI（边缘列表 + 编辑器）
- [ ] **Edge 上报模式优化**（第二步）
  - [ ] touchIngress 配置增加 `EdgeReportMode` 参数
    - `0` - 不上报：Manager 连接后不上报任何 Edge
    - `1` - 上报所有（默认）：Manager 连接后上报所有已连接的 Edge
    - `2` - 增量上报：Manager 连接后仅上报有变化的 Edge
  - [ ] touchIngress 增加 Edge 离线缓存特性
    - 记录没有 Manager 连接时下线的 Edge
    - Manager 上线后根据上报模式上报这些 Edge 的离线状态
    - 确保 Manager 能够获取完整的 Edge 状态变化历史
  - [ ] 减少 Manager 重启时对 Ingress 的压力（大量 Edge 场景）
- [ ] 支持touch_ingress

## v0.3（并发优化）
- [ ] **Manager 处理大量并发 edge.online 消息优化**
  - 问题：Manager 的 `_handle_edge_online_request` 在 asyncio 事件循环中执行同步 `db.commit()`
  - 影响：事件循环阻塞 → WebSocket 心跳超时 → Manager 断开连接 → 进程退出
  - 测试结果：
    - 10 设备：Manager 稳定 ✅
    - 100 设备（分批启动）：Edge 进程全部启动（100个），但 Manager 在处理约 20+ 并发 edge.online 时崩溃 ❌
    - 501 设备：Edge 进程启动后部分退出（剩 297），Manager 同样崩溃 ❌
    - Ingress：所有测试中均稳定运行 ✅
  - 解决方案（三选一）：
    - [ ] **方案 A**：改用异步数据库（aiosqlite），避免阻塞事件循环
    - [ ] **方案 B**：增加消息队列（asyncio.Queue），串行处理数据库写入，避免并发锁竞争
    - [ ] **方案 C**：批量处理 edge.online 请求，攒批后一次 commit
- [ ] **SQLite 数据库连接池优化**
  - 启用 WAL 模式（已完成）
  - 增加 `busy_timeout=5000`（已完成）
  - 考虑连接池或每个协程独立 session
- [ ] **Edge 启动分批控制**
  - 测试脚本支持分批启动（如每批 100 个，间隔 5 秒）
  - 避免瞬时 500+ 并发连接冲击 Ingress 和 Manager

## v1.0
- [ ] 多用户 RBAC
- [ ] Prometheus 指标导出
- [ ] OTA 升级 agent
- [ ] **Edge 频繁上下线防抖机制**
  - [ ] 设备下线后又上线时，优先使用 Ingress 已记录的设备信息
  - [ ] 如果 Ingress 已有该设备的记录数据（离线缓存未上报），则不向 Manager 发送上线消息
  - [ ] 避免设备频繁上下线对 Manager 和数据库造成冲击
  - [ ] 可配置防抖时间窗口（默认 60 秒）

## v2.0（Ingress 性能优化 - 十万连接目标）

### P0 致命问题（必须解决）

- [ ] **Manager 重连 O(n²) 操作优化**
  - 问题：`handle_manager_connect_jsonrpc2` 遍历所有 Edge，`notify_device_online` 内部又遍历一次
  - 影响：十万连接 → 阻塞 100+ 秒，心跳超时引发雪崩断开
  - 解决方案：
    - [ ] 改为**批量上报**：收集所有 edge_id 到数组，一次性发送给 Manager
    - [ ] 消除 `notify_device_online` 内部的 O(n) 遍历

- [ ] **单线程事件循环优化**
  - 问题：`ez_ws_server_service_exec` 单线程处理所有连接
  - 影响：无法利用多核 CPU，耗时操作阻塞所有连接
  - 解决方案：
    - [ ] WebSocket 事件循环保持单线程（epoll 线程安全）
    - [ ] 业务逻辑处理移到**线程池**
    - [ ] 使用无锁队列传递消息

### P1 严重问题（应该解决）

- [ ] **消除 O(n) 线性扫描（5 处）**
  - [ ] Edge 上线检查重复 edge_id → 反向索引 `unordered_map<string, int>`
  - [ ] Manager 确认响应查找 → 反向索引
  - [ ] 踢出 Edge 查找 → 反向索引
  - [ ] 查询列表分页遍历 → 反向索引 + 分页缓存
  - [ ] 通知 Manager 查找公网 IP → 反向索引

- [ ] **std::map → std::unordered_map**
  - 问题：`std::map` 查找 O(log n) ≈ 17 次节点跳转，缓存不友好
  - 解决方案：
    - [ ] `m_online_edges` 改为 `unordered_map<int, EdgeDeviceInfo>`
    - [ ] `m_offline_edges_cache` 改为 `unordered_map<string, EdgeDeviceInfo>`

- [ ] **JSON 重复解析优化**
  - 问题：同一条消息被解析 5 次（isJsonRpc2、isResponse、extractMethod、extractId、decode）
  - 解决方案：
    - [ ] 一次解析，缓存 method 和 id
    - [ ] 考虑使用 SIMD JSON 库（如 simdjson）

### P2 中等问题（建议解决）

- [ ] **日志异步化**
  - 问题：十万 Edge 每秒 3,333 次心跳，日志同步 I/O 阻塞事件循环
  - 解决方案：
    - [ ] 关闭生产环境 DEBUG 日志
    - [ ] INFO 日志改为异步写入
    - [ ] 高频日志（如心跳）采样输出（每 1000 次输出一次）

- [ ] **内存拷贝优化**
  - 问题：每条消息经历 5+ 次内存拷贝（buffer → string → JsonValue → 结构体 → 响应 JSON → 发送 buffer）
  - 解决方案：
    - [ ] 使用 `std::string_view` 传递字符串
    - [ ] 预分配 JSON 编码 buffer
    - [ ] 考虑零拷贝序列化（如 flatbuffers）

### 性能目标

| 指标 | v0.1 当前 | v2.0 目标 |
|------|----------|----------|
| 单实例连接数 | 1,000 | 100,000 |
| Manager 重连恢复 | 100s+ | < 1s |
| Edge 上线延迟 | 秒级 | < 100ms |
| 心跳处理吞吐 | 单线程 | 多线程池 |
| CPU 利用率 | 单核 | 多核 |

### 实施计划

- **第一阶段（1-2 天）**：反向索引 + unordered_map + 批量上报 + 关 DEBUG 日志
- **第二阶段（1-2 周）**：JSON 一次解析 + string_view + 日志异步
- **第三阶段（1-2 月）**：业务逻辑线程池 + vector 自由列表 + 性能测试框架