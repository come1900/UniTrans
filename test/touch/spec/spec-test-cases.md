# Touch 系统测试用例

## 概述

本文档是 **Touch 系统的统一测试用例库**，整合了全部测试用例，按主题分类维护：

| 章节 | 覆盖范围 | 用例标识 |
|------|---------|---------|
| 一、REST API 接口测试 | 边缘管理、Ingress 管理、系统状态 | `TC-API-xxx` |
| 二、WebSocket 协议测试 | Edge ↔ Ingress、Manager ↔ Ingress | `TC-WS-xxx` |
| 三、边缘生命周期与管理测试 | 边缘上线/下线、黑白名单、心跳、Manager 离线、边界条件、增量上报 | `TC-xxx` |
| 四、frpc 远程配置测试 | come.1 编解码单元测试、配置下发/查询端到端测试 | `TF-UT-xxx` / `TF-IT-xxx` |
| 五、压力测试 | 大规模并发连接 | `TC-PRESSURE-xxx` |

> 本文档为测试用例的**唯一权威出处**。设计解析见 `spec-design.md`；迁移自该文档的用例在此集中维护，设计文档中仅保留用例引用。

---

## 测试环境

### 环境要求

- touch_manager 运行在 `http://localhost:18051`（默认端口 18050，测试用 `PORT=18051` 覆盖）
- touch_ingress 运行在 `127.0.0.1:54321`
- 测试边缘 ID：`edge001` ~ `edge010`（统一三位 edge id）；亦可用 `edge-test-001` ~ `edge-test-010`
- Edge 侧配置文件名：`./shpc/shpc.<edge_id>.json`（相对 edge 工作目录，由 `FunRegisterCli::get_frpc_config_path()` 生成）

### 测试准备

```bash
# 1. 启动 touch_ingress
cd <repo>/test/touch/touch_ingress
./touch_ingress-linux -p 54321 &

# 2. 启动 touch_manager
cd <repo>/man/touch_manager
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' PORT='18051' wpyenv/bin/python app.py &

# 3. 启动 edge（连本地 ingress，edge001）
cd <repo>/test/touch/touch_edge
./touch_edge-linux --host 127.0.0.1 --port 54321 --edge-id edge001 --edge-key key001 &

# 4. 等待服务启动
sleep 5

# 5. 验证服务正常
curl http://localhost:18051/health
```

---

## 一、REST API 接口测试

### 1. 边缘管理接口测试

#### TC-API-001: 查询边缘列表

**测试目的**: 验证获取边缘列表接口正常工作

**前置条件**:
- touch_manager 正常运行
- 数据库中至少有 1 个边缘记录

**测试步骤**:
```bash
curl -X GET "http://localhost:18051/api/v1/edges"
```

**预期结果**:
```json
{
    "code": 0,
    "edges": [
        {
            "edge_id": "xxx",
            "edge_type": "touch",
            "status": 1,
            "confirmed": 99,
            ...
        }
    ],
    "message": "success"
}
```

**通过标准**:
- 返回码 `code` 为 0
- `edges` 数组包含边缘数据
- 每个边缘包含必需字段：`edge_id`, `edge_type`, `status`, `confirmed`

---

#### TC-API-002: 查询边缘列表（带分页）

**测试目的**: 验证分页参数正常工作

**测试步骤**:
```bash
curl -X GET "http://localhost:18051/api/v1/edges?page=1&page_size=10"
```

**预期结果**:
- 返回最多 10 个边缘
- 包含分页信息

---

#### TC-API-003: 确认边缘（白名单）

**测试目的**: 验证将边缘加入白名单功能

**前置条件**:
- 边缘 `edge-test-001` 存在且状态为 pending (confirmed=99)

**测试步骤**:
```bash
curl -X POST "http://localhost:18051/api/v1/edges/edge-test-001/confirm"
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success"
}
```

**验证步骤**:
```bash
curl "http://localhost:18051/api/v1/edges/edge-test-001"
# 检查 confirmed 字段应为 1
```

---

#### TC-API-004: 拒绝边缘（黑名单）

**测试目的**: 验证将边缘加入黑名单功能

**前置条件**:
- 边缘 `edge-test-002` 存在

**测试步骤**:
```bash
curl -X POST "http://localhost:18051/api/v1/edges/edge-test-002/reject"
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success"
}
```

**验证步骤**:
```bash
curl "http://localhost:18051/api/v1/edges/edge-test-002"
# 检查 confirmed 字段应为 0
```

---

#### TC-API-005: 删除边缘

**测试目的**: 验证删除边缘功能

**前置条件**:
- 边缘 `edge-test-003` 存在

**测试步骤**:
```bash
curl -X DELETE "http://localhost:18051/api/v1/edges/edge-test-003"
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success"
}
```

**验证步骤**:
```bash
curl "http://localhost:18051/api/v1/edges/edge-test-003"
# 应返回 404 或 edge not found
```

---

#### TC-API-006: 批量确认边缘

**测试目的**: 验证批量确认功能

**前置条件**:
- 边缘 `edge-test-004`, `edge-test-005`, `edge-test-006` 存在且状态为 pending

**测试步骤**:
```bash
curl -X POST "http://localhost:18051/api/v1/edges/batch-confirm" \
  -H "Content-Type: application/json" \
  -d '{"edge_ids": ["edge-test-004", "edge-test-005", "edge-test-006"]}'
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success",
    "confirmed_count": 3
}
```

---

#### TC-API-007: 批量拒绝边缘

**测试目的**: 验证批量拒绝功能

**前置条件**:
- 边缘 `edge-test-007`, `edge-test-008`, `edge-test-009` 存在

**测试步骤**:
```bash
curl -X POST "http://localhost:18051/api/v1/edges/batch-reject" \
  -H "Content-Type: application/json" \
  -d '{"edge_ids": ["edge-test-007", "edge-test-008", "edge-test-009"]}'
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success",
    "rejected_count": 3
}
```

---

#### TC-API-008: 查询不存在的边缘

**测试目的**: 验证错误处理

**测试步骤**:
```bash
curl "http://localhost:18051/api/v1/edges/non-existent-edge"
```

**预期结果**:
```json
{
    "code": 40002,
    "message": "Edge not found"
}
```

---

### 2. Ingress 管理接口测试

#### TC-API-101: 查询 Ingress 列表

**测试目的**: 验证获取 Ingress 列表接口正常工作

**测试步骤**:
```bash
curl -X GET "http://localhost:18051/api/v1/ingresses"
```

**预期结果**:
```json
{
    "code": 0,
    "ingresses": [
        {
            "ingress_id": "local-127.0.0.1",
            "host": "127.0.0.1",
            "port": 54321,
            "status": 1,
            "enabled": true
        }
    ],
    "message": "success"
}
```

---

#### TC-API-102: 添加 Ingress

**测试目的**: 验证添加新 Ingress 功能

**测试步骤**:
```bash
curl -X POST "http://localhost:18051/api/v1/ingresses" \
  -H "Content-Type: application/json" \
  -d '{
    "ingress_id": "ingress-test-001",
    "host": "192.168.1.100",
    "port": 54321,
    "enabled": true
  }'
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success"
}
```

---

#### TC-API-103: 更新 Ingress

**测试目的**: 验证更新 Ingress 配置功能

**测试步骤**:
```bash
curl -X PUT "http://localhost:18051/api/v1/ingresses/ingress-test-001" \
  -H "Content-Type: application/json" \
  -d '{
    "host": "192.168.1.101",
    "enabled": false
  }'
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success"
}
```

---

#### TC-API-104: 删除 Ingress

**测试目的**: 验证删除 Ingress 功能

**前置条件**:
- Ingress `ingress-test-001` 存在

**测试步骤**:
```bash
curl -X DELETE "http://localhost:18051/api/v1/ingresses/ingress-test-001"
```

**预期结果**:
```json
{
    "code": 0,
    "message": "success"
}
```

---

### 3. 系统状态接口测试

#### TC-API-201: 健康检查

**测试目的**: 验证健康检查接口

**测试步骤**:
```bash
curl "http://localhost:18051/health"
```

**预期结果**:
```json
{
    "status": "ok"
}
```

---

#### TC-API-202: API 信息

**测试目的**: 验证 API 信息接口

**测试步骤**:
```bash
curl "http://localhost:18051/api"
```

**预期结果**:
```json
{
    "status": "ok",
    "service": "touch_manager",
    "version": "0.2"
}
```

---

## 二、WebSocket 协议测试

### 4. Edge ↔ Ingress 通信测试

#### TC-WS-001: Edge 上线请求

**测试目的**: 验证边缘上线流程

**前置条件**:
- touch_ingress 正常运行
- touch_manager 已连接到 ingress

**测试步骤**:
1. 启动 touch_edge，连接到 ingress
2. 发送 `edge.online` 请求

**预期结果**:
- ingress 返回 `success=true`
- manager 收到 `ingress.edge.online` 通知
- 边缘状态更新为 online

---

#### TC-WS-002: Edge 心跳

**测试目的**: 验证心跳机制

**前置条件**:
- 边缘已成功上线

**测试步骤**:
1. 等待 30 秒（默认心跳间隔）
2. 检查 ingress 日志

**预期结果**:
- ingress 收到 `edge.heartbeat` 通知
- 心跳正常，连接保持

---

#### TC-WS-003: Edge 下线检测

**测试目的**: 验证边缘下线检测

**前置条件**:
- 边缘已上线

**测试步骤**:
1. 停止 touch_edge 进程
2. 等待 ingress 检测断开
3. 检查 manager 日志

**预期结果**:
- ingress 发送 `ingress.edge.offline` 通知
- manager 更新边缘状态为 offline

---

### 5. Manager ↔ Ingress 通信测试

#### TC-WS-101: Manager 连接 Ingress

**测试目的**: 验证 manager 连接 ingress 流程

**前置条件**:
- touch_ingress 正常运行
- touch_manager 配置了 ingress 信息

**测试步骤**:
1. 启动 touch_manager
2. 检查 ingress 日志

**预期结果**:
- manager 发送 `manager.connect` 通知
- ingress 接受连接

---

#### TC-WS-102: Manager 踢边缘

**测试目的**: 验证 manager 踢边缘功能

**前置条件**:
- 边缘 `edge-test-010` 已上线
- 边缘在黑名单中 (confirmed=0)

**测试步骤**:
1. 在 manager 中调用拒绝接口
2. 检查 ingress 日志

**预期结果**:
- manager 发送 `manager.edge.kick` 命令
- ingress 关闭边缘连接
- ingress 发送 `ingress.edge.offline` 通知

---

#### TC-WS-103: Manager 查询边缘列表

**测试目的**: 验证 manager 查询 ingress 上的边缘列表

**前置条件**:
- ingress 上有多个边缘连接

**测试步骤**:
1. manager 发送 `manager.edge.list` 请求
2. 检查响应

**预期结果**:
- ingress 返回边缘列表
- 包含边缘 ID 和上线时间

---

#### TC-WS-104: 单 Manager 连接守护（TC-019）

**测试目的**: 验证 ingress 只接受一个 Manager 连接，多余的 Manager 会被立即拒绝

**设计原则**: 每个 touchIngress 实例只能有一个 Manager 连接，新 Manager 连接时如果已有 Manager，直接拒绝新连接

**前置条件**:
- touch_ingress 正常运行在 `127.0.0.1:54321`
- 准备两个 Manager 实例（Manager-A 和 Manager-B）

**测试步骤**:
1. 启动 Manager-A，连接到 ingress 并发送 `manager.connect` 消息
2. 等待 3 秒，验证 Manager-A 连接成功
3. 启动 Manager-B，连接到 ingress 并发送 `manager.connect` 消息
4. 验证 Manager-B 被 ingress 拒绝
5. 验证 Manager-A 仍保持连接
6. 查询边缘列表，验证只有 Manager-A 能接收数据

**Manager-A 连接消息**:
```json
{
    "jsonrpc": "2.0",
    "method": "manager.connect",
    "params": {
        "manager_id": "manager-a",
        "ingress_id": "local-127.0.0.1"
    }
}
```

**Manager-B 连接消息**:
```json
{
    "jsonrpc": "2.0",
    "method": "manager.connect",
    "params": {
        "manager_id": "manager-b",
        "ingress_id": "local-127.0.0.1"
    }
}
```

**预期结果**:
1. Manager-A 连接成功，ingress 日志显示：
   ```
   touch_ingress: Received manager.connect notification from client_id=X, manager_id=manager-a
   touch_ingress: Manager connected successfully [client_id=X, manager_id=manager-a]
   ```
2. Manager-B 连接时，ingress 日志显示：
   ```
   touch_ingress: Manager already connected [client_id=X], rejecting new manager [client_id=Y, manager_id=manager-b]
   ```
3. Manager-B 连接被拒绝，连接关闭
4. Manager-A 仍保持连接，能接收边缘状态通知

**通过标准**:
- Manager-A 连接成功后，`m_manager_client_id` 设置为 Manager-A 的 client_id
- Manager-B 连接时，ingress 直接拒绝新连接，`m_manager_client_id` 不变
- Manager-A 仍保持连接，能接收边缘状态通知和管理指令
- ingress 日志中明确显示 "Manager already connected" 信息

**自动化测试脚本**:
```bash
#!/bin/bash
# test_single_manager.sh

INGRESS_HOST="127.0.0.1"
INGRESS_PORT="54321"

echo "=== TC-WS-104: 单 Manager 连接守护测试 ==="

# 1. 启动 Manager-A
echo "[1/6] 启动 Manager-A..."
python3 -c "
import asyncio, json, websockets

async def test_manager_a():
    async with websockets.connect('ws://$INGRESS_HOST:$INGRESS_PORT/come') as ws:
        msg = {
            'jsonrpc': '2.0',
            'method': 'manager.connect',
            'params': {'manager_id': 'manager-a', 'ingress_id': 'local-127.0.0.1'}
        }
        await ws.send(json.dumps(msg))
        print('Manager-A: 连接成功，等待 5 秒...')
        await asyncio.sleep(5)
        print('Manager-A: 连接仍保持，测试通过')

asyncio.run(test_manager_a())
" &
MANAGER_A_PID=$!

sleep 3

# 2. 启动 Manager-B
echo "[2/6] 启动 Manager-B..."
python3 -c "
import asyncio, json, websockets

async def test_manager_b():
    async with websockets.connect('ws://$INGRESS_HOST:$INGRESS_PORT/come') as ws:
        msg = {
            'jsonrpc': '2.0',
            'method': 'manager.connect',
            'params': {'manager_id': 'manager-b', 'ingress_id': 'local-127.0.0.1'}
        }
        await ws.send(json.dumps(msg))
        print('Manager-B: 连接成功')
        await asyncio.sleep(2)
        print('Manager-B: 连接仍保持，测试通过')

asyncio.run(test_manager_b())
" &
MANAGER_B_PID=$!

sleep 3

# 3. 验证 Manager-A 被断开
echo "[3/6] 验证 Manager-A 被断开..."
if kill -0 $MANAGER_A_PID 2>/dev/null; then
    echo "FAIL: Manager-A 仍在运行"
    kill $MANAGER_A_PID 2>/dev/null
    kill $MANAGER_B_PID 2>/dev/null
    exit 1
else
    echo "PASS: Manager-A 已被断开"
fi

# 4. 验证 Manager-B 仍连接
echo "[4/6] 验证 Manager-B 仍连接..."
if kill -0 $MANAGER_B_PID 2>/dev/null; then
    echo "PASS: Manager-B 仍保持连接"
else
    echo "FAIL: Manager-B 被意外断开"
    kill $MANAGER_B_PID 2>/dev/null
    exit 1
fi

# 5. 检查 ingress 日志
echo "[5/6] 检查 ingress 日志..."
if grep -q "Disconnecting old manager" /tmp/logs/log-touchIngress.log; then
    echo "PASS: ingress 日志显示断开旧 Manager"
else
    echo "FAIL: ingress 日志未显示断开旧 Manager"
    kill $MANAGER_B_PID 2>/dev/null
    exit 1
fi

# 6. 清理
echo "[6/6] 清理测试环境..."
kill $MANAGER_B_PID 2>/dev/null

echo ""
echo "=== TC-WS-104 测试通过 ==="
```

---

## 三、边缘生命周期与管理测试

### 测试概述

**测试分类逻辑**：按功能模块和业务流程分类，覆盖系统的主要特性和关键场景：

| 分类 | 测试目的 | 覆盖场景 |
|------|---------|---------|
| **边缘上线** | 验证边缘正常上线流程 | 新边缘注册、Manager 确认、Ingress 接受 |
| **边缘下线** | 验证边缘正常下线流程 | 边缘断开、Ingress 通知、状态更新 |
| **黑名单管理** | 验证边缘黑名单功能 | 加入黑名单、拒绝上线、移出黑名单 |
| **心跳机制** | 验证心跳保活功能 | 定时心跳、心跳应答、断线重连 |
| **Manager 离线** | 验证 Manager 离线场景 | 已在线边缘保持、新边缘拒绝 |
| **边界条件** | 验证异常和边界情况 | 重复注册、边缘验证失败、网络超时 |
| **增量上报** | 验证 EdgeReportMode=2 增量上报 | 只上报变化边缘、离线缓存 |

### 测试用例总表

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

### 详细测试用例

#### TC-001: 新边缘正常上线

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

#### TC-002: 黑名单边缘上线

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

#### TC-006: 加入黑名单

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

#### TC-007: 移出黑名单

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

#### TC-011: Manager 离线时已在线边缘保持

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

#### TC-016: 拉黑在线边缘后连接断开

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

#### TC-017: 增量上报（EdgeReportMode=2）

**测试目的**：验证 EdgeReportMode=2 时，Manager 连接后只上报变化的 Edge

**前置条件**：
- touchIngress 配置 `EdgeReportMode=2`
- touchManager 未启动
- 测试边缘 edge-test-001、edge-test-002、edge-test-003

**测试步骤**：
```bash
# 1. 启动 Ingress（EdgeReportMode=2）
cd <repo>/build/local
./touchIngress-linux &

# 2. 启动 3 个 Edge
./test_edge-linux --host 127.0.0.1 --port 54321 -i edge-test-001 -k key001 &
./test_edge-linux --host 127.0.0.1 --port 54321 -i edge-test-002 -k key002 &
./test_edge-linux --host 127.0.0.1 --port 54321 -i edge-test-003 -k key003 &
sleep 3

# 3. 启动 Manager
cd <repo>/man/touch_manager
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

#### TC-018: 查询离线缓存

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

## 四、frpc 远程配置测试

frpc 远程配置（Edge 配置）专题的测试。设计解析见 `spec-design.md` 的 `## Edge frpc 远程配置` 子章节。

> **迁移说明**：本专题用例取自 spec-design.md 的 `### 验收测试`。原 `UT-xxx` / `IT-xxx` 编号在本文档中统一加 `TF-` 前缀（`TF-UT-xxx` / `TF-IT-xxx`），以避免与全局 `TC-xxx` 冲突。

### 测试准备

**环境要求**：
- touch_manager 运行在 `http://localhost:18051`
- touch_ingress 运行在 `127.0.0.1:54321`
- 测试边缘 ID：`edge001` 等（edge 侧配置文件名 `./shpc/shpc.edge001.json`，相对 edge 工作目录）
- 测试 frpc 配置文件路径：edge 工作目录下 `shpc/shpc.<edge_id>.json`

**启动脚本**：
```bash
# 1. 启动 touch_ingress
cd <repo>/test/touch/touch_ingress
./touch_ingress-linux -p 54321 &

# 2. 启动 touch_manager
cd <repo>/man/touch_manager
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' PORT='18051' wpyenv/bin/python app.py &

# 3. 启动 edge（连本地 ingress，edge001）
cd <repo>/test/touch/touch_edge
./touch_edge-linux --host 127.0.0.1 --port 54321 --edge-id edge001 --edge-key key001 &

# 4. 等待服务启动
sleep 5

# 5. 验证服务正常
curl http://localhost:18051/health
```

### 单元测试（TF-UT-xxx）

> 单元测试在 `libs/come.1/ut/ut-come.1.json.cpp` 中实现，使用现有的单元测试框架。

#### TF-UT-001: ConfigUpdate_tunnelService 编解码测试

**测试目的**: 验证 `ConfigUpdate_tunnelService` 的 encode/decode 功能

**测试步骤**（在 `ut-come.1.json.cpp` 中新增）：
```cpp
// 1. 构造 ConfigUpdate_tunnelService 消息
ConfigUpdate_tunnelService msg;
msg.edge_id = "edge-test-001";
msg.config_type = "tunnelService";
msg.version = 1;
msg.access_token = "test-token-123";
msg.configContent.services.push_back(createTestTunnelService());

// 2. encode 为 JSON 字符串
std::string jsonStr = ComeJsonCodec::encode(msg);

// 3. decode 回消息对象
ConfigUpdate_tunnelService decodedMsg;
bool success = ComeJsonCodec::decode(jsonStr, decodedMsg);

// 4. 验证字段一致性
ASSERT_EQ(msg.edge_id, decodedMsg.edge_id);
ASSERT_EQ(msg.config_type, decodedMsg.config_type);
ASSERT_EQ(msg.version, decodedMsg.version);
ASSERT_EQ(msg.access_token, decodedMsg.access_token);
```

**预期结果**：
- encode 输出有效的 JSON-RPC 2.0 格式
- decode 成功返回 true
- 所有字段值保持一致

**优先级**: P0（核心功能，必须测试）

---

#### TF-UT-002: FrpcConfig 编解码测试

**测试目的**: 验证 `FrpcConfig` 的 encode/decode 功能

**测试步骤**：
```cpp
// 1. 构造 FrpcConfig 对象
FrpcConfig frpcCfg;
frpcCfg.serverAddr = "10.220.42.139";
frpcCfg.serverPort = 50400;
frpcCfg.authMethod = "token";
frpcCfg.token = "test-token";
frpcCfg.tlsEnable = true;
frpcCfg.webServerAddr = "127.0.0.1";
frpcCfg.webServerPort = 17400;
frpcCfg.proxies.push_back(createTestProxy());

// 2. encode 为 JSON 字符串
std::string jsonStr = ComeJsonCodec::encode(frpcCfg);

// 3. decode 回 FrpcConfig 对象
FrpcConfig decodedCfg;
bool success = ComeJsonCodec::decode(jsonStr, decodedCfg);

// 4. 验证字段一致性
ASSERT_EQ(frpcCfg.serverAddr, decodedCfg.serverAddr);
ASSERT_EQ(frpcCfg.serverPort, decodedCfg.serverPort);
ASSERT_EQ(frpcCfg.token, decodedCfg.token);
```

**预期结果**：
- encode 输出标准 frpc JSON 格式
- decode 成功返回 true
- 所有字段值保持一致

**优先级**: P0

---

#### TF-UT-003: ConfigUpdate ↔ FrpcConfig 转换测试

**测试目的**: 验证 `toFrpcConfig()` 和 `fromFrpcConfig()` 转换功能

**测试步骤**：
```cpp
// 1. 构造完整的 ConfigUpdate_tunnelService 消息（使用 TunnelService 格式）
ConfigUpdate_tunnelService configMsg;
configMsg.edge_id = "edge001";
configMsg.config_type = "tunnelService";
configMsg.version = 1;
configMsg.access_token = "test_access_token";

// 构造 tunnelService
TunnelService tunnelSvc;
tunnelSvc.version = "1.0";
tunnelSvc.endpoint.host = "10.220.42.139";
tunnelSvc.endpoint.port = 50400;
tunnelSvc.localManagement.bindAddress = "127.0.0.1";
tunnelSvc.localManagement.bindPort = 17400;
tunnelSvc.security.authMethod = "token";
tunnelSvc.security.credential = "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc";
tunnelSvc.security.enableTls = true;

// 构造 accessPolicy
AccessPolicy policy;
policy.policyId = "w-tcp-51422";
policy.protocol = "tcp";
policy.description = "pss service";
policy.exposedPort = 51422;
policy.targetService.ip = "127.0.0.1";
policy.targetService.port = 55555;

// 添加到消息中
tunnelSvc.accessPolicies.push_back(policy);

ServiceConfig serviceCfg;
serviceCfg.serviceName = "tunnelService";
serviceCfg.tunnelService = tunnelSvc;
configMsg.configContent.services.push_back(serviceCfg);

// 2. 转换为 FrpcConfig
FrpcConfig frpcCfg;
bool toSuccess = ComeJsonCodec::toFrpcConfig(configMsg, frpcCfg);

// 3. 验证 FrpcConfig 字段（转换后的扁平结构）
ASSERT_TRUE(toSuccess);
ASSERT_EQ(frpcCfg.serverAddr, "10.220.42.139");
ASSERT_EQ(frpcCfg.serverPort, 50400);
ASSERT_EQ(frpcCfg.authMethod, "token");
ASSERT_EQ(frpcCfg.token, "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc");
ASSERT_EQ(frpcCfg.tlsEnable, true);
ASSERT_EQ(frpcCfg.webServerAddr, "127.0.0.1");
ASSERT_EQ(frpcCfg.webServerPort, 17400);
ASSERT_EQ(frpcCfg.proxies.size(), 1);
ASSERT_EQ(frpcCfg.proxies[0].name, "w-tcp-51422");
ASSERT_EQ(frpcCfg.proxies[0].type, "tcp");
ASSERT_EQ(frpcCfg.proxies[0].localIP, "127.0.0.1");
ASSERT_EQ(frpcCfg.proxies[0].localPort, 55555);
ASSERT_EQ(frpcCfg.proxies[0].remotePort, 51422);

// 4. 反向转换：FrpcConfig → ConfigUpdate_tunnelService
ConfigUpdate_tunnelService backMsg;
bool fromSuccess = ComeJsonCodec::fromFrpcConfig(frpcCfg, "edge001", 1, "test_access_token", backMsg);

// 5. 验证往返一致性
ASSERT_TRUE(toSuccess && fromSuccess);
ASSERT_EQ(configMsg.configContent.services[0].tunnelService.endpoint.host,
          backMsg.configContent.services[0].tunnelService.endpoint.host);
ASSERT_EQ(configMsg.configContent.services[0].tunnelService.endpoint.port,
          backMsg.configContent.services[0].tunnelService.endpoint.port);
ASSERT_EQ(configMsg.configContent.services[0].accessPolicies[0].policyId,
          backMsg.configContent.services[0].accessPolicies[0].policyId);
```

**预期结果**：
- `toFrpcConfig()` 正确从 TunnelService 格式转换为 frpc 扁平结构
- `fromFrpcConfig()` 正确从 frpc 扁平结构转换回 TunnelService 格式
- 往返转换保持数据一致性

**优先级**: P0

---

#### TF-UT-004: AckConfigUpdate 编解码测试

**测试目的**: 验证 `AckConfigUpdate` 的 encode/decode 功能

**测试步骤**：
```cpp
// 1. 构造 AckConfigUpdate 消息（成功）
AckConfigUpdate ackMsg;
ackMsg.id = 12345;
ackMsg.code = 0;
ackMsg.message = "success";

// 2. encode 为 JSON 字符串
std::string jsonStr = ComeJsonCodec::encode(ackMsg);

// 3. decode 回消息对象
AckConfigUpdate decodedAck;
bool success = ComeJsonCodec::decode(jsonStr, decodedAck);

// 4. 验证字段
ASSERT_EQ(ackMsg.id, decodedAck.id);
ASSERT_EQ(ackMsg.code, decodedAck.code);
ASSERT_EQ(ackMsg.message, decodedAck.message);
```

**预期结果**：
- encode 输出 `{"jsonrpc":"2.0","result":{"code":0,"message":"success"},"id":12345}`
- decode 成功返回 true
- 所有字段值保持一致

**优先级**: P0

---

### 端到端测试（TF-IT-xxx）

> 联合测试在完整系统环境中进行，验证 Edge ↔ Ingress ↔ Manager↔（Web UI）的端到端功能。Edge 侧无独立接口，需在完整系统环境中测试：
> - 方案1：使用真实 Edge 设备
> - 方案2：编写 Edge 模拟器（mock Edge），模拟 ConfigUpdate 处理和文件写入
> - 方案3：在测试脚本中直接操作 Edge 进程和文件系统

#### TF-IT-001: 配置下发成功场景

**测试目的**: 验证配置从 Web UI 下发到 Edge 的完整流程

**前置条件**：
- touch_manager、touch_ingress 正常运行
- Edge 在线并已连接（`edge001`）
- 数据库中存在边缘记录

**测试步骤**：
```bash
# 1. 调用 REST API 下发配置（REST 接受嵌套 TunnelService 结构）
curl -X POST "http://localhost:18051/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config_content": {
      "services": [
        {
          "serviceName": "tunnelService",
          "tunnelService": {
            "endpoint": {"host": "10.220.42.139", "port": 50400},
            "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
            "security": {"authMethod": "token", "credential": "test-token-123", "enableTls": true},
            "accessPolicies": [
              {"policyId": "w-tcp-51422", "protocol": "tcp",
               "exposedPort": 51422,
               "targetService": {"ip": "127.0.0.1", "port": 55555}}
            ]
          }
        }
      ]
    }
  }'

# 2. 检查 Edge 侧配置文件（edge 按自身 ID 写盘）
cat ./shpc/shpc.edge001.json   # 需在 edge 工作目录下

# 3. 查询数据库中的配置状态（表名 edges）
sqlite3 <manager>/t-touch_manager.db "SELECT config_status FROM edges WHERE edge_id='edge001';"
```

**预期结果**：
1. REST API 返回 `{"code":0,...}`（成功下发，等待 edge 确认）
2. `./shpc/shpc.edge001.json` 文件存在且内容正确（扁平 frpc 格式）：
   ```json
   {
     "serverAddr": "10.220.42.139",
     "serverPort": 50400,
     "auth": {"method": "token", "token": "test-token-123"},
     "transport": {"tls": {"enable": true}},
     "webServer": {"addr": "127.0.0.1", "port": 17400},
     "proxies": [{"name": "w-tcp-51422", "type": "tcp",
                  "localIP": "127.0.0.1", "localPort": 55555, "remotePort": 51422}]
   }
   ```
3. 数据库中 `config_status = 'confirmed'`
4. Edge 回复 `AckConfigUpdate(code=0, message="success")`

**优先级**: P0

---

#### TF-IT-002: 配置下发失败场景（Edge 离线）

**测试目的**: 验证 Edge 离线时配置暂存数据库的逻辑

**前置条件**：
- touch_manager、touch_ingress 正常运行
- Edge 离线（未连接或已断开）

**测试步骤**：
```bash
# 1. 调用 REST API 下发配置（REST 接受嵌套 TunnelService 结构，离线时暂存数据库）
curl -X POST "http://localhost:18051/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config_content": {
      "services": [
        {
          "serviceName": "tunnelService",
          "tunnelService": {
            "endpoint": {"host": "10.220.42.139", "port": 50400},
            "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
            "security": {"authMethod": "token", "credential": "test-token-123", "enableTls": true},
            "accessPolicies": [
              {"policyId": "w-tcp-51422", "protocol": "tcp",
               "exposedPort": 51422, "description": "pss service",
               "targetService": {"ip": "127.0.0.1", "port": 55555}}
            ]
          }
        }
      ]
    }
  }'

# 2. 查询数据库中的配置状态和配置内容（表名 edge_configs）
sqlite3 <manager>/t-touch_manager.db \
  "SELECT config_status, config_json FROM edge_configs WHERE edge_id='edge001';"
```

**预期结果**：
1. REST API 返回 `{"code":40003,"message":"Edge is offline"}` 或类似错误
2. 数据库中配置已保存：`config_json` 暂存下发的配置数据
3. `config_status` 标记为 'pending'（离线暂存待下发）

**优先级**: P1

---

#### TF-IT-003: 配置查询成功场景

**测试目的**: 验证配置查询（config/get）的同步返回逻辑

**前置条件**：
- Edge 在线并已连接
- 数据库中已有配置（或为空）

**测试步骤**：
```bash
# 1. 调用 REST API 查询配置（DB 有配置 → 直接同步返回）
curl -X POST "http://localhost:18051/api/v1/edges/config/get" \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'

# 2. 清空数据库配置后再次查询（DB 无配置且 Edge 在线 → 同步读 edge 本地配置回填）
# 3. 再次查询，验证配置已回填
curl -X POST "http://localhost:18051/api/v1/edges/config/get" \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'

# 4. 检查 Edge 侧配置文件
cat ./shpc/shpc.edge001.json
```

**预期结果**：
1. 第一次查询：返回数据库中的配置；若 DB 为空且 Edge 在线，同步读 edge 本地真实配置并回填 DB 后返回
2. `config_sync` 反映一致性核对结果（matched / mismatch）
3. 数据库更新，`config_json` 刷新为 Edge 的实际配置
4. 第二次查询：返回回填/核对后的配置

**优先级**: P1

---

#### TF-IT-004: 配置查询失败场景（配置文件不存在）

**测试目的**: 验证 Edge 侧配置文件不存在时的错误处理

**前置条件**：
- Edge 在线并已连接
- `./shpc/shpc.edge001.json` 文件不存在

**测试步骤**：
```bash
# 1. 删除 Edge 侧配置文件（如果存在）
rm -f ./shpc/shpc.edge001.json

# 2. 调用 REST API 查询配置
curl -X POST "http://localhost:18051/api/v1/edges/config/get" \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001"}'
```

**预期结果**：
- Edge 在线但本地配置文件缺失：`config/get` 返回 `config_sync='missing'`，配置回退为后端默认
- （历史表述曾为返回错误 `{"code":50006,"message":"Config file not found"}` 或空配置，现以 `config_sync='missing'` 区分）

**优先级**: P2

---

#### TF-IT-005: 配置下发失败场景（文件写入失败）

**测试目的**: 验证 Edge 侧文件写入失败时的错误处理

**前置条件**：
- Edge 在线并已连接
- Edge 配置目录 `./shpc/` 目录权限设置为只读（模拟写入失败）

**测试步骤**：
```bash
# 1. 设置 Edge 配置目录权限为只读（模拟；需在 edge 工作目录下）
chmod 555 ./shpc

# 2. 调用 REST API 下发配置（REST 接受嵌套 TunnelService 结构）
curl -X POST "http://localhost:18051/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config_content": {
      "services": [
        {
          "serviceName": "tunnelService",
          "tunnelService": {
            "endpoint": {"host": "10.220.42.139", "port": 50400},
            "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
            "security": {"authMethod": "token", "credential": "test-token-123", "enableTls": true},
            "accessPolicies": [
              {"policyId": "w-tcp-51422", "protocol": "tcp",
               "exposedPort": 51422,
               "targetService": {"ip": "127.0.0.1", "port": 55555}}
            ]
          }
        }
      ]
    }
  }'

# 3. 恢复权限
chmod 755 ./shpc
```

**预期结果**：
- Edge 回复 `AckConfigUpdate(code=-2, message="Failed to write config file")`
- REST API 返回 `{"code":50005,"message":"Config update failed: Failed to write config file"}`
- 数据库 `config_status = 'config_failed'`

**优先级**: P2

---

#### TF-IT-006: 配置确认超时重推场景

**测试目的**: 验证 `CONFIG_CONFIRM_TIMEOUT` 超时后允许重新下发

**前置条件**：
- Edge 在线但未确认配置（模拟延迟/不确认）
- 将 `CONFIG_CONFIRM_TIMEOUT` 设置为较短值（如 2 秒）便于测试

**测试步骤**：
```bash
# 1. 调用 REST API 下发配置（首次下发后 edge 不确认，config_status 停留 configuring）
curl -X POST "http://localhost:18051/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config_content": {
      "services": [
        {
          "serviceName": "tunnelService",
          "tunnelService": {
            "endpoint": {"host": "10.220.42.139", "port": 50400},
            "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
            "security": {"authMethod": "token", "credential": "test-token-123", "enableTls": true},
            "accessPolicies": [
              {"policyId": "w-tcp-51422", "protocol": "tcp",
               "exposedPort": 51422,
               "targetService": {"ip": "127.0.0.1", "port": 55555}}
            ]
          }
        }
      ]
    }
  }'

# 2. 等待超过 CONFIG_CONFIRM_TIMEOUT（如 2s）
# 3. 再次调用 REST API 下发（因超时，允许越过节流直接重推）
curl -X POST "http://localhost:18051/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{"edge_id": "edge001", "config_type": "tunnelService", "config_content": {}}'
```

**预期结果**：
- 首次下发后 edge 未确认，`config_status = 'configuring'`（保持配置中状态）
- 超过 `CONFIG_CONFIRM_TIMEOUT` 后再次下发：不再被节流拒绝，manager 重新推送配置
- 若 edge 确认成功，`config_status` 更新为 'confirmed'

**优先级**: P2

---

### frpc 测试用例总表

| 编号 | 分类 | 测试用例 | 优先级 | 可测性说明 |
|------|------|----------|--------|------------|
| TF-UT-001 | 单元测试 | ConfigUpdate_tunnelService 编解码 | P0 | libs/come.1 已有单元测试框架，可直接扩展 |
| TF-UT-002 | 单元测试 | FrpcConfig 编解码 | P0 | libs/come.1 已有单元测试框架 |
| TF-UT-003 | 单元测试 | ConfigUpdate ↔ FrpcConfig 转换 | P0 | libs/come.1 单元测试 |
| TF-UT-004 | 单元测试 | AckConfigUpdate 编解码 | P0 | libs/come.1 单元测试 |
| TF-IT-001 | 端到端 | 配置下发成功 | P0 | 需要 Edge 模拟或真实环境 |
| TF-IT-002 | 端到端 | 配置下发失败（Edge离线） | P1 | 可通过断开 Edge 连接测试 |
| TF-IT-003 | 端到端 | 配置查询成功 | P1 | 需要 Edge 模拟或真实环境 |
| TF-IT-004 | 端到端 | 配置查询失败（文件不存在） | P2 | 需要 Edge 模拟环境 |
| TF-IT-005 | 端到端 | 配置下发失败（文件写入失败） | P2 | 需要 Edge 模拟环境，权限控制 |
| TF-IT-006 | 端到端 | 配置下发超时 | P2 | 需要 Edge 模拟延迟 |

### frpc 测试脚本示例

**TF-IT-001 测试脚本**（bash）：
```bash
#!/bin/bash
# test_config_update.sh - 测试配置下发成功场景

set -e

MANAGER_URL="http://localhost:18051"
EDGE_ID="edge001"
# edge 工作目录（edge 按自身 ID 写盘）
EDGE_WORKDIR="/path/to/touch_edge"
TEST_FRPC_FILE="$EDGE_WORKDIR/shpc/shpc.$EDGE_ID.json"

echo "=== 测试配置下发成功场景 ==="

# 1. 准备测试环境（清理旧配置）
rm -f "$TEST_FRPC_FILE"

# 2. 调用 REST API 下发配置（REST 接受嵌套 TunnelService 结构）
echo "步骤1: 下发配置..."
RESPONSE=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{
    "edge_id": "edge001",
    "config_type": "tunnelService",
    "config_content": {
      "services": [
        {
          "serviceName": "tunnelService",
          "tunnelService": {
            "endpoint": {"host": "10.220.42.139", "port": 50400},
            "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
            "security": {"authMethod": "token", "credential": "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc", "enableTls": true},
            "accessPolicies": [
              {"policyId": "w-tcp-51422", "protocol": "tcp",
               "exposedPort": 51422, "description": "pss service",
               "targetService": {"ip": "127.0.0.1", "port": 55555}}
            ]
          }
        }
      ]
    }
  }')

echo "REST API 响应: $RESPONSE"

# 3. 检查配置文件（应该是转换后的 frpc JSON 格式）
echo "步骤2: 检查配置文件..."
if [ -f "$TEST_FRPC_FILE" ]; then
    echo "✓ 配置文件存在"
    echo "配置文件内容:"
    cat "$TEST_FRPC_FILE" | python3 -m json.tool
    # 验证关键字段
    echo ""
    echo "验证关键字段:"
    python3 -c "
import json
with open('$TEST_FRPC_FILE') as f:
    cfg = json.load(f)
    assert cfg.get('serverAddr') == '10.220.42.139', 'serverAddr mismatch'
    assert cfg.get('serverPort') == 50400, 'serverPort mismatch'
    assert cfg.get('auth', {}).get('method') == 'token', 'auth method mismatch'
    assert cfg.get('auth', {}).get('token') == '737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc', 'token mismatch'
    assert cfg.get('transport', {}).get('tls', {}).get('enable') == True, 'tls enable mismatch'
    assert cfg.get('webServer', {}).get('addr') == '127.0.0.1', 'webServer addr mismatch'
    assert cfg.get('webServer', {}).get('port') == 17400, 'webServer port mismatch'
    assert len(cfg.get('proxies', [])) == 1, 'proxies count mismatch'
    proxy = cfg['proxies'][0]
    assert proxy.get('name') == 'w-tcp-51422', 'proxy name mismatch'
    assert proxy.get('type') == 'tcp', 'proxy type mismatch'
    assert proxy.get('localPort') == 55555, 'proxy localPort mismatch'
    assert proxy.get('remotePort') == 51422, 'proxy remotePort mismatch'
    print('✓ 所有字段验证通过')
"
else
    echo "✗ 配置文件不存在"
    exit 1
fi

# 4. 检查数据库状态
echo "步骤3: 检查数据库状态..."
# sqlite3 <manager>/t-touch_manager.db "SELECT config_status FROM edges WHERE edge_id='$EDGE_ID';"

echo "=== 测试通过 ==="
```

---


---

## 五、压力测试

### TC-PRESSURE-001: 100 设备压力测试

**测试目的**: 验证 Ingress 能处理 100 个 Edge 同时连接，Manager 能正确接收并展示所有设备

**前置条件**:
- touch_ingress 正常运行在 `127.0.0.1:54321`
- touch_manager 正常运行在 `http://localhost:18051`
- Manager 已通过 WebSocket 连接到 Ingress
- 测试程序 `test/touch/touch_edge/touch_edge-linux` 已编译

**测试步骤**:

```bash
# 1. 清理环境
pkill -9 touchIngress-linux 2>/dev/null
pkill -9 touchEdge-linux 2>/dev/null
pkill -9 touch_edge-linux 2>/dev/null
pkill -9 -f "python app.py" 2>/dev/null
sleep 2
rm -f /tmp/logs/log-touchIngress.log /tmp/logs/touch_manager.log
rm -f man/touch_manager/t-touch_manager.db

# 2. 启动 Ingress
cd build/local && ./touchIngress-linux &
sleep 2

# 3. 启动 Manager
cd man/touch_manager && source wpyenv/bin/activate
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' \
  DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py &
sleep 6

# 4. 验证 Ingress 和 Manager 正常运行
curl http://localhost:18051/health  # 应返回 {"status":"ok"}

# 5. 启动 100 个测试 Edge
bash test/touch/scripts/start_edges_quick.sh 100
sleep 20

# 6. 验证 API 返回全部 100 个设备
curl -s -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" -d '{}' | python3 -c "
import sys, json
data = json.load(sys.stdin)
edges = data.get('edges', [])
total = data.get('total', 0)
print(f'API returned: {len(edges)} edges, total in DB: {total}')
assert total >= 100, f'Expected >=100 edges, got {total}'
print('PASS: 100 edges registered')
"

# 7. 验证 Ingress 未崩溃
ps aux | grep touchIngress | grep -v grep  # 应显示 Ingress 进程
```

**预期结果**:
- 100 个 Edge 进程全部启动
- Manager 数据库记录 >= 100 个边缘
- API `/api/v1/edges/list` 返回全部 100 个边缘（`page_size` 默认 1000）
- Ingress 进程未崩溃
- Web UI 页面显示所有设备

**通过标准**:
- 数据库边缘数 >= 100
- API 返回边缘数 >= 100
- Ingress 进程存活
- 无 Segmentation fault

**常见问题**:

| 问题 | 原因 | 解决方法 |
|------|------|---------|
| API 只返回 20 个设备 | `page_size` 默认 20 | 已修复为 1000 |
| Manager 崩溃 | debug=True 时大量并发消息导致 | 已修复为 debug=False |
| Ingress 崩溃 | kick_device 竞态条件 | 已修复，不再重复通知 |
| 设备全离线 | 测试程序 `reconnect_max_retries=1` | 已修复为 -1（无限重连） |
| 501 设备测试失败 | Manager 在大量并发 edge.online 消息时不稳定 | 建议分批启动，每批 100 个 |

---

## 测试脚本

### 自动化测试脚本

```bash
# 运行完整测试套件
cd <repo>/test/touch/scripts
./test_manager_api.sh

# 运行单个测试用例
./test_manager_api.sh test_list_edges
./test_manager_api.sh test_confirm_edge
./test_manager_api.sh test_reject_edge
```

### 边缘生命周期测试脚本目录结构

```
st/                          # 测试脚本目录
├── README.md               # 测试说明
├── common.sh               # 公共函数库
├── test_blacklist.sh       # 黑名单测试
├── test_edge_online.sh   # 边缘上线测试
└── run_all_tests.sh        # 运行所有测试
```

### 测试结果查看

测试完成后，查看测试报告：
```bash
cat /tmp/touch_manager_test_report.txt
```

---

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

---

## 测试通过标准

| 测试类别 | 用例数 | 通过标准 |
|----------|--------|----------|
| REST API - 边缘管理 | 8 | 全部通过 |
| REST API - Ingress 管理 | 4 | 全部通过 |
| REST API - 系统状态 | 2 | 全部通过 |
| WebSocket - Edge 通信 | 3 | 全部通过 |
| WebSocket - Manager 通信 | 4 | 全部通过 |
| 压力测试 - 100 设备 | 1 | API 返回 >= 100 设备，Ingress 不崩溃 |
| **总计** | **22** | **100% 通过** |

---

## 常见问题

### Q1: API 返回 500 错误

**原因**: 数据库连接问题或内部错误

**解决方法**:
```bash
# 检查 manager 日志
tail -f /tmp/logs/touch_manager.log

# 重启 manager
pkill -f "python app.py"
python app.py
```

### Q2: WebSocket 连接失败

**原因**: ingress 未启动或端口被占用

**解决方法**:
```bash
# 检查 ingress 进程
ps aux | grep touchIngress

# 检查端口监听
netstat -tlnp | grep 54321

# 重启 ingress
pkill touchIngress-linux
./touchIngress-linux &
```

---

**文档版本**: v1.0
**更新时间**: 2026-09-14

**变更说明**：
- v1.0（2026-09-14）：整合统一测试用例库。保留原有 REST API / WebSocket / 压力测试用例；迁入 spec-design.md 中的边缘生命周期与管理测试（TC-001~018）与 frpc 远程配置测试（frpc 单元 TF-UT-001~004、端到端 TF-IT-001~006），统一路径与 edge id 约定（三位 edge id `edge001`）。spec-design.md 中不再重复展开整套用例，仅保留引用。
- v0.2（2026-03-31）：初版，含 REST API、WebSocket、压力测试用例。
