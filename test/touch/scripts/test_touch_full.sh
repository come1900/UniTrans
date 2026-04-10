#!/bin/bash
# Touch 系统完整测试脚本
# 用法: ./test_touch_full.sh

set -e

BASEDIR="/home/wujj/svn/daily/src/appmodule/UniTrans"
MANAGER_URL="http://localhost:18051"
PASS=0
FAIL=0
TOTAL=0

log() { echo "[$(date +%H:%M:%S)] $1"; }

pass() { PASS=$((PASS+1)); TOTAL=$((TOTAL+1)); log "✅ PASS: $1"; }
fail() { FAIL=$((FAIL+1)); TOTAL=$((TOTAL+1)); log "❌ FAIL: $1"; }

cleanup() {
    log "清理环境..."
    pkill -9 touchIngress-linux 2>/dev/null || true
    pkill -9 touchEdge-linux 2>/dev/null || true
    pkill -9 touch_edge-linux 2>/dev/null || true
    pkill -9 -f "python app.py" 2>/dev/null || true
    sleep 2
    rm -f /tmp/logs/log-touchIngress.log /tmp/logs/log-touchEdge.log /tmp/logs/touch_manager.log
    rm -f $BASEDIR/man/touch_manager/t-touch_manager.db
    mkdir -p /tmp/logs
}

start_ingress() {
    log "启动 Ingress..."
    cd $BASEDIR/build/local
    cp ../../touchIngress-linux . 2>/dev/null || true
    setsid ./touchIngress-linux > /tmp/ingress_out.log 2>&1 < /dev/null &
    disown
    sleep 3
    if netstat -tlnp 2>/dev/null | grep -q 54321; then
        pass "Ingress 启动成功"
    else
        fail "Ingress 启动失败"
        return 1
    fi
}

start_manager() {
    log "启动 Manager..."
    cd $BASEDIR/man/touch_manager
    source wpyenv/bin/activate
    setsid env DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' DEFAULT_INGRESS_PORT='54321' DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py > /tmp/manager_out.log 2>&1 < /dev/null &
    disown
    sleep 5
    HEALTH=$(curl -s $MANAGER_URL/health 2>/dev/null)
    if echo "$HEALTH" | grep -q "ok"; then
        pass "Manager 启动成功"
    else
        fail "Manager 启动失败"
        return 1
    fi
}

start_edge() {
    local edge_id=$1
    local edge_key=$2
    log "启动测试 Edge: $edge_id..."
    cd $BASEDIR/test/touch/touch_edge
    setsid ./touch_edge-linux --host 127.0.0.1 --port 54321 --edge-id "$edge_id" --edge-key "$edge_key" > /tmp/edge_${edge_id}.log 2>&1 < /dev/null &
    disown
    sleep 3
}

stop_edge() {
    local edge_id=$1
    log "停止 Edge: $edge_id..."
    pkill -9 -f "touch_edge-linux.*--edge-id $edge_id" 2>/dev/null || true
    sleep 2
}

check_edge_status() {
    local edge_id=$1
    local expected_status=$2
    sleep 2
    local status=$(curl -s -X POST $MANAGER_URL/api/v1/edges/list \
        -H "Content-Type: application/json" -d '{}' | \
        python3 -c "
import sys, json
d = json.load(sys.stdin)
for e in d.get('edges', []):
    if e['edge_id'] == '$edge_id':
        print(e['status'])
        break
" 2>/dev/null)
    if [ "$status" = "$expected_status" ]; then
        pass "Edge $edge_id 状态正确 (status=$status, expected=$expected_status)"
    else
        fail "Edge $edge_id 状态错误 (status=$status, expected=$expected_status)"
    fi
}

# ===== 开始测试 =====
cleanup

echo ""
echo "========================================="
echo "Touch 系统完整测试"
echo "========================================="
echo ""

# 1. 启动 Ingress 和 Manager
start_ingress
start_manager

# 2. 测试 Manager 连接 Ingress
log "测试 Manager-Ingress 连接..."
sleep 3
CONN=$(netstat -an 2>/dev/null | grep 54321 | grep ESTABLISHED | wc -l)
if [ "$CONN" -ge 2 ]; then
    pass "Manager 已连接 Ingress (连接数=$CONN)"
else
    fail "Manager 未连接 Ingress (连接数=$CONN)"
fi

# 3. 测试 Edge 上线
start_edge "test-001" "key001"
sleep 3
check_edge_status "test-001" "1"

# 4. 测试 Edge 心跳（保持 10 秒观察，touch_edge reconnect_max_retries=1 会退出）
log "观察 Edge 心跳 (10 秒)..."
sleep 10
# touch_edge 可能已退出，检查最后状态

# 5. 测试 Edge 下线
stop_edge "test-001"
check_edge_status "test-001" "0"

# 6. 测试多个 Edge
start_edge "test-002" "key002"
start_edge "test-003" "key003"
sleep 3
check_edge_status "test-002" "1"
check_edge_status "test-003" "1"

# 7. 测试 Kick Edge（拒绝 Edge）
log "测试踢出 Edge test-002..."
REJECT=$(curl -s -X POST $MANAGER_URL/api/v1/edges/reject \
    -H "Content-Type: application/json" \
    -d '{"edge_id": "test-002"}')
if echo "$REJECT" | grep -q '"code": 0\|"code":0'; then
    pass "拒绝 Edge 请求成功"
else
    fail "拒绝 Edge 请求失败: $REJECT"
fi
sleep 3
check_edge_status "test-002" "0"

# 8. 测试 Ingress 日志
log "检查 Ingress 日志..."
CONN_LOG=$(grep -c "\[CONNECT\]" /tmp/logs/log-touchIngress.log 2>/dev/null || echo 0)
MANAGER_LOG=$(grep -c "\[MANAGER.CONNECT\]" /tmp/logs/log-touchIngress.log 2>/dev/null || echo 0)
EDGE_ONLINE_LOG=$(grep -c "\[EDGE.ONLINE\]" /tmp/logs/log-touchIngress.log 2>/dev/null || echo 0)
if [ "$CONN_LOG" -gt 0 ] && [ "$MANAGER_LOG" -gt 0 ] && [ "$EDGE_ONLINE_LOG" -gt 0 ]; then
    pass "Ingress 日志完整 (CONNECT=$CONN_LOG, MANAGER=$MANAGER_LOG, EDGE_ONLINE=$EDGE_ONLINE_LOG)"
else
    fail "Ingress 日志不完整 (CONNECT=$CONN_LOG, MANAGER=$MANAGER_LOG, EDGE_ONLINE=$EDGE_ONLINE_LOG)"
fi

# 9. 清理
cleanup

echo ""
echo "========================================="
echo "测试结果: $PASS/$TOTAL 通过, $FAIL 失败"
echo "========================================="
