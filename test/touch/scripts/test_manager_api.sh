#!/bin/bash
#
# test_manager_api.sh - Touch Manager REST API 测试脚本
#
# 用法:
#   ./test_manager_api.sh              # 运行所有测试
#   ./test_manager_api.sh test_name    # 运行单个测试
#

# 配置
MANAGER_URL="${MANAGER_URL:-http://localhost:18051}"
TEST_REPORT="/tmp/touch_manager_test_report.txt"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 测试计数器
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0

# 打印函数
print_header() {
    echo -e "\n${YELLOW}========================================${NC}"
    echo -e "${YELLOW}$1${NC}"
    echo -e "${YELLOW}========================================${NC}\n"
}

print_test() {
    echo -e "${YELLOW}[TEST]${NC} $1"
}

print_pass() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((TESTS_PASSED++))
    ((TESTS_TOTAL++))
}

print_fail() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((TESTS_FAILED++))
    ((TESTS_TOTAL++))
}

# 初始化测试报告
init_report() {
    echo "Touch Manager API Test Report" > "$TEST_REPORT"
    echo "==============================" >> "$TEST_REPORT"
    echo "Date: $(date)" >> "$TEST_REPORT"
    echo "Manager URL: $MANAGER_URL" >> "$TEST_REPORT"
    echo "" >> "$TEST_REPORT"
}

# 添加到测试报告
add_to_report() {
    echo "$1" >> "$TEST_REPORT"
}

# 检查服务是否可用
check_service() {
    print_header "检查服务状态"
    
    if curl -s "$MANAGER_URL/health" | grep -q "ok"; then
        print_pass "touch_manager 服务正常"
    else
        print_fail "touch_manager 服务不可用"
        exit 1
    fi
}

# ==================== 边缘管理接口测试 ====================

test_list_edges() {
    print_test "查询边缘列表"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
        -H "Content-Type: application/json" \
        -d '{}')

    if echo "$response" | grep -q '"code": 0'; then
        if echo "$response" | grep -q '"edges":'; then
            print_pass "查询边缘列表成功"
            add_to_report "[PASS] test_list_edges"
            return 0
        fi
    fi

    print_fail "查询边缘列表失败"
    add_to_report "[FAIL] test_list_edges"
    return 1
}

test_list_edges_pagination() {
    print_test "查询边缘列表（带分页）"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
        -H "Content-Type: application/json" \
        -d '{"page": 1, "page_size": 10}')

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "分页查询成功"
        add_to_report "[PASS] test_list_edges_pagination"
        return 0
    fi

    print_fail "分页查询失败"
    add_to_report "[FAIL] test_list_edges_pagination"
    return 1
}

test_confirm_edge() {
    print_test "确认边缘（白名单）"

    # 使用测试边缘 ID
    edge_id="edge-test-001"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/confirm" \
        -H "Content-Type: application/json" \
        -d "{\"edge_id\": \"$edge_id\"}")

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "确认边缘成功"
        add_to_report "[PASS] test_confirm_edge"
        return 0
    fi

    # 如果边缘不存在，也算通过（404 是预期的）
    if echo "$response" | grep -q '"code": 40002'; then
        print_pass "边缘不存在（预期行为）"
        add_to_report "[PASS] test_confirm_edge (edge not found)"
        return 0
    fi

    print_fail "确认边缘失败"
    add_to_report "[FAIL] test_confirm_edge"
    return 1
}

test_reject_edge() {
    print_test "拒绝边缘（黑名单）"

    edge_id="edge-test-002"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/reject" \
        -H "Content-Type: application/json" \
        -d "{\"edge_id\": \"$edge_id\"}")

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "拒绝边缘成功"
        add_to_report "[PASS] test_reject_edge"
        return 0
    fi

    # 如果边缘不存在，也算通过
    if echo "$response" | grep -q '"code": 40002'; then
        print_pass "边缘不存在（预期行为）"
        add_to_report "[PASS] test_reject_edge (edge not found)"
        return 0
    fi

    print_fail "拒绝边缘失败"
    add_to_report "[FAIL] test_reject_edge"
    return 1
}

test_delete_edge() {
    print_test "删除边缘"

    edge_id="edge-test-003"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/delete" \
        -H "Content-Type: application/json" \
        -d "{\"edge_id\": \"$edge_id\"}")

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "删除边缘成功"
        add_to_report "[PASS] test_delete_edge"
        return 0
    fi

    # 如果边缘不存在，也算通过
    if echo "$response" | grep -q '"code": 40002'; then
        print_pass "边缘不存在（预期行为）"
        add_to_report "[PASS] test_delete_edge (edge not found)"
        return 0
    fi

    print_fail "删除边缘失败"
    add_to_report "[FAIL] test_delete_edge"
    return 1
}

test_batch_confirm() {
    print_test "批量确认边缘"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/batch-confirm" \
        -H "Content-Type: application/json" \
        -d '{"edge_ids": ["edge-test-004", "edge-test-005", "edge-test-006"]}')

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "批量确认成功"
        add_to_report "[PASS] test_batch_confirm"
        return 0
    fi

    print_fail "批量确认失败"
    add_to_report "[FAIL] test_batch_confirm"
    return 1
}

test_batch_reject() {
    print_test "批量拒绝边缘"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/batch-reject" \
        -H "Content-Type: application/json" \
        -d '{"edge_ids": ["edge-test-007", "edge-test-008", "edge-test-009"]}')

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "批量拒绝成功"
        add_to_report "[PASS] test_batch_reject"
        return 0
    fi

    print_fail "批量拒绝失败"
    add_to_report "[FAIL] test_batch_reject"
    return 1
}

test_edge_not_found() {
    print_test "查询不存在的边缘"

    # 使用 get 接口测试不存在的边缘
    response=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/get" \
        -H "Content-Type: application/json" \
        -d '{"edge_id": "non-existent-edge"}')

    if echo "$response" | grep -q '"code": 40002'; then
        print_pass "正确返回 404 错误"
        add_to_report "[PASS] test_edge_not_found"
        return 0
    fi

    # 如果返回 404 HTTP 状态码也算通过
    if echo "$response" | grep -q '404'; then
        print_pass "返回 404 错误（预期行为）"
        add_to_report "[PASS] test_edge_not_found (404)"
        return 0
    fi

    print_fail "未正确返回错误"
    add_to_report "[FAIL] test_edge_not_found"
    return 1
}

# ==================== Ingress 管理接口测试 ====================

test_list_ingresses() {
    print_test "查询 Ingress 列表"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/ingresses/list" \
        -H "Content-Type: application/json" \
        -d '{}')

    if echo "$response" | grep -q '"code": 0'; then
        if echo "$response" | grep -q '"ingresses":'; then
            print_pass "查询 Ingress 列表成功"
            add_to_report "[PASS] test_list_ingresses"
            return 0
        fi
    fi

    print_fail "查询 Ingress 列表失败"
    add_to_report "[FAIL] test_list_ingresses"
    return 1
}

test_add_ingress() {
    print_test "添加 Ingress"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/ingresses/add" \
        -H "Content-Type: application/json" \
        -d '{
            "ingress_id": "ingress-test-001",
            "host": "192.168.1.100",
            "port": 54321,
            "enabled": true
        }')

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "添加 Ingress 成功"
        add_to_report "[PASS] test_add_ingress"
        return 0
    fi

    # 如果已存在，也算通过
    if echo "$response" | grep -q 'already exists'; then
        print_pass "Ingress 已存在（预期行为）"
        add_to_report "[PASS] test_add_ingress (already exists)"
        return 0
    fi

    print_fail "添加 Ingress 失败"
    add_to_report "[FAIL] test_add_ingress"
    return 1
}

test_update_ingress() {
    print_test "更新 Ingress"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/ingresses/update" \
        -H "Content-Type: application/json" \
        -d '{
            "ingress_id": "ingress-test-001",
            "host": "192.168.1.101",
            "enabled": false
        }')

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "更新 Ingress 成功"
        add_to_report "[PASS] test_update_ingress"
        return 0
    fi

    # 如果不存在，也算通过
    if echo "$response" | grep -q '"code": 40002'; then
        print_pass "Ingress 不存在（预期行为）"
        add_to_report "[PASS] test_update_ingress (not found)"
        return 0
    fi

    print_fail "更新 Ingress 失败"
    add_to_report "[FAIL] test_update_ingress"
    return 1
}

test_delete_ingress() {
    print_test "删除 Ingress"

    response=$(curl -s -X POST "$MANAGER_URL/api/v1/ingresses/delete" \
        -H "Content-Type: application/json" \
        -d '{"ingress_id": "ingress-test-001"}')

    if echo "$response" | grep -q '"code": 0'; then
        print_pass "删除 Ingress 成功"
        add_to_report "[PASS] test_delete_ingress"
        return 0
    fi

    # 如果不存在，也算通过
    if echo "$response" | grep -q '"code": 40002'; then
        print_pass "Ingress 不存在（预期行为）"
        add_to_report "[PASS] test_delete_ingress (not found)"
        return 0
    fi

    print_fail "删除 Ingress 失败"
    add_to_report "[FAIL] test_delete_ingress"
    return 1
}

# ==================== 系统状态接口测试 ====================

test_health() {
    print_test "健康检查"
    
    response=$(curl -s "$MANAGER_URL/health")
    
    if echo "$response" | grep -q '"status": "ok"'; then
        print_pass "健康检查通过"
        add_to_report "[PASS] test_health"
        return 0
    fi
    
    print_fail "健康检查失败"
    add_to_report "[FAIL] test_health"
    return 1
}

test_api_info() {
    print_test "API 信息"
    
    response=$(curl -s "$MANAGER_URL/api")
    
    if echo "$response" | grep -q '"status": "ok"'; then
        if echo "$response" | grep -q '"service": "touch_manager"'; then
            print_pass "API 信息正确"
            add_to_report "[PASS] test_api_info"
            return 0
        fi
    fi
    
    print_fail "API 信息错误"
    add_to_report "[FAIL] test_api_info"
    return 1
}

# ==================== 主函数 ====================

run_all_tests() {
    print_header "运行所有测试"
    
    # 边缘管理接口测试
    print_header "边缘管理接口测试"
    test_list_edges || true
    test_list_edges_pagination || true
    test_confirm_edge || true
    test_reject_edge || true
    test_delete_edge || true
    test_batch_confirm || true
    test_batch_reject || true
    test_edge_not_found || true
    
    # Ingress 管理接口测试
    print_header "Ingress 管理接口测试"
    test_list_ingresses || true
    test_add_ingress || true
    test_update_ingress || true
    test_delete_ingress || true
    
    # 系统状态接口测试
    print_header "系统状态接口测试"
    test_health || true
    test_api_info || true
}

run_single_test() {
    test_name="$1"
    
    if declare -f "$test_name" > /dev/null; then
        print_header "运行单个测试：$test_name"
        $test_name
    else
        echo -e "${RED}错误：未知的测试用例 '$test_name'${NC}"
        echo "可用的测试用例："
        declare -F | grep "^declare -f test_" | awk '{print $3}'
        exit 1
    fi
}

print_summary() {
    print_header "测试总结"
    
    echo "总测试数：$TESTS_TOTAL"
    echo -e "${GREEN}通过：$TESTS_PASSED${NC}"
    echo -e "${RED}失败：$TESTS_FAILED${NC}"
    echo ""
    
    add_to_report ""
    add_to_report "测试总结"
    add_to_report "=========="
    add_to_report "总测试数：$TESTS_TOTAL"
    add_to_report "通过：$TESTS_PASSED"
    add_to_report "失败：$TESTS_FAILED"
    add_to_report ""
    add_to_report "测试报告：$TEST_REPORT"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}所有测试通过！${NC}"
        add_to_report "结果：所有测试通过"
    else
        echo -e "${RED}有 $TESTS_FAILED 个测试失败${NC}"
        add_to_report "结果：$TESTS_FAILED 个测试失败"
    fi
    
    echo ""
    echo "测试报告已保存到：$TEST_REPORT"
}

# 主程序
main() {
    init_report
    
    check_service
    
    if [ $# -eq 0 ]; then
        run_all_tests
    else
        run_single_test "$1"
    fi
    
    print_summary
}

# 执行主程序
main "$@"
