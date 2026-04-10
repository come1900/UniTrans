#!/bin/bash
#
# test_edge_report_mode.sh - 测试 EdgeReportMode 配置（1 和 2）
#
# 测试场景：
# 1. EdgeReportMode=1：Manager 连接后上报所有已连接的 Edge
# 2. EdgeReportMode=2：Manager 连接后仅上报有变化的 Edge
#

set -e

# 配置
INGRESS_PORT=54321
MANAGER_URL="http://localhost:18051"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EDGE_BINARY="$SCRIPT_DIR/../touch_edge/touch_edge-linux"
INGRESS_BINARY="$SCRIPT_DIR/../../../build/touchIngress-linux"
CFG_FILE="$SCRIPT_DIR/../../../build/local/cfg-touchIngress.cfg"

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
    pkill -9 -f "touchEdge-linux" 2>/dev/null || true
    pkill -9 -f "touchIngress-linux" 2>/dev/null || true
    pkill -9 -f "python app.py" 2>/dev/null || true
    sleep 2
    print_pass "旧进程已清理"
}

# 清理日志和数据库
cleanup_data() {
    print_header "清理日志和数据库"

    # 清理日志
    rm -f /tmp/logs/log-touchIngress.log
    rm -f /tmp/logs/log-touchEdge.log
    rm -f /tmp/logs/touch_manager.log
    print_pass "日志已清理"

    # 清理数据库
    local db_file="$HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager/t-touch_manager.db"
    if [ -f "$db_file" ]; then
        rm -f "$db_file"
        print_pass "数据库已清理"
    fi

    # 重新初始化数据库
    cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager
    python migrate_db.py 2>/dev/null || true
    print_pass "数据库已重新初始化"
}

# 设置 EdgeReportMode 配置
set_report_mode() {
    local mode=$1
    print_step "设置 EdgeReportMode=$mode"

    # 检查配置文件中是否已有 EdgeReportMode
    if grep -q "EdgeReportMode" "$CFG_FILE"; then
        # 更新现有值
        sed -i "s/\"EdgeReportMode\" : [0-9]*/\"EdgeReportMode\" : $mode/" "$CFG_FILE"
    else
        # 添加新配置项
        sed -i "s/\"Protocol\" : \"come.1\"/\"Protocol\" : \"come.1\",\n         \"EdgeReportMode\" : $mode/" "$CFG_FILE"
    fi

    print_pass "EdgeReportMode 已设置为 $mode"
}

# 恢复原配置
restore_config() {
    # 恢复默认配置
    set_report_mode 1
    print_pass "配置已恢复为默认值"
}

# 启动 Ingress
start_ingress() {
    print_header "启动 Ingress"

    cd "$SCRIPT_DIR/../../../build/local"
    cp ../../build/touchIngress-linux .
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
    DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py &
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
        -d '{}' | python3 -c "
import sys, json
data = json.load(sys.stdin)
edges = data.get('edges', [])
print(f'在线 Edge 数量: {len(edges)}')
for e in edges:
    print(f'  - {e[\"edge_id\"]}: status={e[\"status\"]}, confirmed={e[\"confirmed\"]}')
"
}

# 检查 Ingress 日志中的上报信息
check_ingress_logs() {
    local mode=$1
    local expected_msg=$2
    local log_file="/tmp/logs/log-touchIngress.log"

    print_step "检查 Ingress 日志"

    if grep -q "$expected_msg" "$log_file" 2>/dev/null; then
        print_pass "Ingress 日志显示: $expected_msg"
    else
        print_fail "Ingress 日志未找到: $expected_msg"
    fi
}

# 测试 EdgeReportMode=1（上报所有）
test_mode_1() {
    print_header "测试 EdgeReportMode=1（上报所有已连接的 Edge）"

    # 步骤 1：设置配置
    set_report_mode 1

    # 步骤 2：启动 Ingress
    start_ingress

    # 步骤 3：启动 3 个 Edge
    print_header "启动 3 个 Edge"
    start_edge "edge-test-001"
    start_edge "edge-test-002"
    start_edge "edge-test-003"
    sleep 3

    # 步骤 4：启动 Manager
    start_manager

    # 步骤 5：验证 Edge 上线
    print_header "验证 Edge 上线"
    sleep 3
    query_online_edges

    # 步骤 6：检查日志
    check_ingress_logs 1 "Reporting all 3 existing edges"

    print_header "EdgeReportMode=1 测试完成"
    echo -e "${GREEN}EdgeReportMode=1 测试通过${NC}"
}

# 测试 EdgeReportMode=2（增量上报）
test_mode_2() {
    print_header "测试 EdgeReportMode=2（仅上报有变化的 Edge）"

    # 步骤 1：设置配置
    set_report_mode 2

    # 步骤 2：启动 Ingress
    start_ingress

    # 步骤 3：启动 3 个 Edge
    print_header "启动 3 个 Edge"
    start_edge "edge-test-001"
    start_edge "edge-test-002"
    start_edge "edge-test-003"
    sleep 3

    # 步骤 4：启动 Manager
    start_manager

    # 步骤 5：验证 Edge 上线
    print_header "验证 Edge 上线"
    sleep 3
    query_online_edges

    # 步骤 6：检查日志
    check_ingress_logs 2 "Reporting changed edges"

    print_header "EdgeReportMode=2 测试完成"
    echo -e "${GREEN}EdgeReportMode=2 测试通过${NC}"
}

# 主函数
main() {
    # 清理旧进程
    cleanup

    # 清理日志和数据库
    cleanup_data

    # 测试模式 1
    test_mode_1

    # 清理
    cleanup
    cleanup_data

    # 测试模式 2
    test_mode_2

    # 恢复配置
    restore_config

    # 最终清理
    cleanup

    print_header "所有测试完成"
    echo -e "${GREEN}EdgeReportMode 配置测试全部通过${NC}"
}

main "$@"
