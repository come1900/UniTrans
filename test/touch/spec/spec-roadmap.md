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