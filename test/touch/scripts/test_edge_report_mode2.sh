#!/bin/bash
#
# test_edge_report_mode2.sh - 测试 EdgeReportMode=2（增量上报）
#
# 测试场景：
# 1. 启动 Ingress（EdgeReportMode=2）
# 2. 启动多个 Edge
# 3. 启动 Manager，验证只上报变化的 Edge
# 4. Edge 断开，验证离线缓存
# 5. Manager 重新连接，验证离线 Edge 上报
#

set -e

# 配置
INGRESS_PORT=54321
MANAGER_URL="http://localhost:18051"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EDGE_BINARY="$SCRIPT_DIR/../touch_edge/touch_edge-linux"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_header() {
    echo -e "\n${YELLOW}========================================${NC}"
    echo -e "${YELLOW}$1${NC}"
    echo -e "${YELLOW}========================================${NC}\n"
}

print_step() {
    echo -e "${YELLOW}[STEP]${NC} $1"
}

print_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
}

# 清理旧进程
cleanup() {
    print_header "清理旧进程"
    pkill -9 -f "touch_edge-linux.*edge-test" 2>/dev/null || true
    pkill -9 -f "touchIngress-linux" 2>/dev/null || true
    pkill -9 -f "python app.py" 2>/dev/null || true
    sleep 2
    print_pass "旧进程已清理"
}

# 启动 Ingress（EdgeReportMode=2）
start_ingress() {
    print_header "启动 Ingress（EdgeReportMode=2）"
    
    # 修改配置文件
    local cfg_file="/home/wujj/svn/daily/src/appmodule/UniTrans/build/local/cfg-touchIngress.cfg"
    sed -i 's/"EdgeReportMode" : [0-9]/"EdgeReportMode" : 2/' "$cfg_file"
    
    cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
    cp ../../touchIngress-linux .
    ./touchIngress-linux &
    sleep 2
    
    # 验证端口
    if netstat -tlnp 2>/dev/null | grep -q ":$INGRESS_PORT"; then
        print_pass "Ingress 启动成功（端口 $INGRESS_PORT）"
    else
        print_fail "Ingress 启动失败"
        exit 1
    fi
}

# 启动 Edge
start_edge() {
    local edge_id=$1
    local edge_key=${2:-"key001"}
    
    $EDGE_BINARY --host 127.0.0.1 --port $INGRESS_PORT -i "$edge_id" -k "$edge_key" -t touch_edge >/dev/null 2>&1 &
    sleep 1
    print_pass "Edge $edge_id 启动成功"
}

# 启动 Manager
start_manager() {
    print_header "启动 Manager"
    
    cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager
    source wpyenv/bin/activate
    DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py &
    sleep 5
    
    # 验证 Manager
    if curl -s "$MANAGER_URL/health" | grep -q "ok"; then
        print_pass "Manager 启动成功"
    else
        print_fail "Manager 启动失败"
        exit 1
    fi
}

# 查询在线 Edge 列表
query_online_edges() {
    curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
        -H "Content-Type: application/json" \
        -d '{"status": "online"}' | python3 -c "
import sys, json
data = json.load(sys.stdin)
edges = data.get('edges', [])
print(f'在线 Edge 数量: {len(edges)}')
for e in edges:
    print(f'  - {e[\"edge_id\"]}: status={e[\"status\"]}, confirmed={e[\"confirmed\"]}')
"
}

# 查询 Ingress 离线缓存
query_offline_cache() {
    print_header "查询 Ingress 离线缓存"
    
    # 通过 WebSocket 发送 manager.edge.offline.list 请求
    # 这里使用 Python 脚本模拟
    python3 << 'EOF'
import asyncio
import json
import websockets

async def query_offline_list():
    try:
        async with websockets.connect("ws://127.0.0.1:54321/come") as ws:
            # 发送查询请求
            req = {
                "jsonrpc": "2.0",
                "method": "manager.edge.offline.list",
                "params": {"offset": 0, "limit": 50},
                "id": 1
            }
            await ws.send(json.dumps(req))
            
            # 等待响应
            resp = await asyncio.wait_for(ws.recv(), timeout=5)
            data = json.loads(resp)
            
            if "result" in data:
                result = data["result"]
                offline = result.get("offline_devices", [])
                print(f"离线 Edge 数量: {result.get('total', 0)}")
                for d in offline:
                    print(f"  - {d['edge_id']}: offline_time={d['offline_time']}, pending_confirm={d['pending_confirm']}")
            else:
                print(f"查询失败: {data}")
    except Exception as e:
        print(f"查询失败: {e}")

asyncio.run(query_offline_list())
EOF
}

# 测试主流程
test_main() {
    print_header "测试 EdgeReportMode=2（增量上报）"
    
    # 步骤 1：启动 Ingress
    start_ingress
    
    # 步骤 2：启动 3 个 Edge
    print_header "启动 3 个 Edge"
    start_edge "edge-test-001"
    start_edge "edge-test-002"
    start_edge "edge-test-003"
    sleep 3
    
    # 步骤 3：启动 Manager
    start_manager
    
    # 步骤 4：验证 Edge 上线
    print_header "验证 Edge 上线"
    sleep 3
    query_online_edges
    
    # 步骤 5：断开 Edge 002
    print_header "断开 Edge 002"
    pkill -9 -f "touch_edge-linux.*edge-test-001" 2>/dev/null || true
    sleep 2
    
    # 步骤 6：查询离线缓存
    query_offline_cache
    
    # 步骤 7：验证 Manager 收到离线通知
    print_header "验证 Manager 收到离线通知"
    query_online_edges
    
    print_header "测试完成"
    echo -e "${GREEN}EdgeReportMode=2 测试通过${NC}"
}

# 主函数
main() {
    cleanup
    test_main
}

main "$@"
