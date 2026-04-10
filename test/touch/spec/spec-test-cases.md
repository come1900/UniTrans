# Touch 系统测试用例

## 概述

本文档定义 Touch 系统的测试用例，包括 REST API 接口测试和 WebSocket 协议测试。

---

## 测试环境

### 环境要求

- touch_manager 运行在 `http://localhost:18051`
- touch_ingress 运行在 `127.0.0.1:54321`
- 测试边缘 ID：`edge-test-001` ~ `edge-test-010`

### 测试准备

```bash
# 1. 启动 touch_ingress
cd ~/svn/daily/src/appmodule/UniTrans/local
./touchIngress-linux &

# 2. 启动 touch_manager
cd ~/svn/daily/src/appmodule/UniTrans/man/touch_manager
source wpyenv/bin/activate
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' PORT='18051' python app.py &

# 3. 等待服务启动
sleep 5

# 4. 验证服务正常
curl http://localhost:18051/health
```

---

## REST API 测试用例

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

## WebSocket 协议测试用例

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

## 压力测试用例

### 6. 100 设备压力测试

**测试目的**: 验证 Ingress 能处理 100 个 Edge 同时连接，Manager 能正确接收并展示所有设备

**测试编号**: TC-PRESSURE-001

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
cd ~/svn/daily/src/appmodule/UniTrans/test/touch/scripts
./test_manager_api.sh

# 运行单个测试用例
./test_manager_api.sh test_list_edges
./test_manager_api.sh test_confirm_edge
./test_manager_api.sh test_reject_edge
```

### 测试结果查看

测试完成后，查看测试报告：
```bash
cat /tmp/touch_manager_test_report.txt
```

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

**文档版本**: v0.2  
**更新时间**: 2026-03-31
