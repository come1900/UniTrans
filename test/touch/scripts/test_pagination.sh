#!/bin/bash
# 测试设备列表分页功能

# 默认测试 18050 端口的 Manager
MANAGER_PORT=${1:-18050}
BASE_URL="http://localhost:${MANAGER_PORT}"

echo "========================================="
echo "测试设备列表分页功能"
echo "Manager URL: ${BASE_URL}"
echo "========================================="
echo ""

# 测试 1: 查询第 1 页 (page_size=20)
echo "【测试 1】查询第 1 页 (page_size=20)"
echo "-----------------------------------------"
RESP1=$(curl -s -X POST ${BASE_URL}/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"page": 1, "page_size": 20}')

TOTAL1=$(echo "$RESP1" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('total', 'N/A'))")
COUNT1=$(echo "$RESP1" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('edges', [])))")
PAGE1=$(echo "$RESP1" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('page', 'N/A'))")

echo "总数: ${TOTAL1}"
echo "当前页: ${PAGE1}"
echo "返回设备数: ${COUNT1}"
echo "预期: 第1页应该返回 20 条设备"
if [ "$COUNT1" -le 20 ]; then
    echo "✅ PASS: 第1页数据量正确"
else
    echo "❌ FAIL: 第1页返回了 ${COUNT1} 条，预期最多 20 条"
fi
echo ""

# 测试 2: 查询第 2 页 (page_size=20)
echo "【测试 2】查询第 2 页 (page_size=20)"
echo "-----------------------------------------"
RESP2=$(curl -s -X POST ${BASE_URL}/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"page": 2, "page_size": 20}')

TOTAL2=$(echo "$RESP2" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('total', 'N/A'))")
COUNT2=$(echo "$RESP2" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('edges', [])))")
PAGE2=$(echo "$RESP2" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('page', 'N/A'))")

echo "总数: ${TOTAL2}"
echo "当前页: ${PAGE2}"
echo "返回设备数: ${COUNT2}"

EXPECTED_PAGE2=$(( (TOTAL2 + 19) / 20 ))  # 向上取整
if [ "$TOTAL2" -gt 20 ]; then
    EXPECTED_COUNT2=20
    if [ "$TOTAL2" -le 40 ]; then
        EXPECTED_COUNT2=$((TOTAL2 - 20))
    fi
    
    if [ "$COUNT2" -eq "$EXPECTED_COUNT2" ]; then
        echo "✅ PASS: 第2页数据量正确 (预期 ${EXPECTED_COUNT2} 条)"
    else
        echo "❌ FAIL: 第2页返回了 ${COUNT2} 条，预期 ${EXPECTED_COUNT2} 条"
    fi
else
    echo "⚠️  总数不超过20，第2页应该为空"
    if [ "$COUNT2" -eq 0 ]; then
        echo "✅ PASS: 第2页为空"
    else
        echo "❌ FAIL: 第2页不应该有数据"
    fi
fi
echo ""

# 测试 3: 验证第1页和第2页的设备ID不重复
echo "【测试 3】验证分页数据不重复"
echo "-----------------------------------------"
IDS1=$(echo "$RESP1" | python3 -c "import sys,json; d=json.load(sys.stdin); print(' '.join([e['edge_id'] for e in d.get('edges', [])]))")
IDS2=$(echo "$RESP2" | python3 -c "import sys,json; d=json.load(sys.stdin); print(' '.join([e['edge_id'] for e in d.get('edges', [])]))")

if [ -z "$IDS1" ] || [ -z "$IDS2" ]; then
    echo "⚠️  无法验证（某一页为空）"
else
    # 检查是否有重复
    OVERLAP=$(echo "$IDS1 $IDS2" | tr ' ' '\n' | sort | uniq -d | wc -l)
    if [ "$OVERLAP" -eq 0 ]; then
        echo "✅ PASS: 第1页和第2页没有重复设备"
    else
        echo "❌ FAIL: 发现 ${OVERLAP} 个重复设备ID"
        echo "重复设备: $(echo "$IDS1 $IDS2" | tr ' ' '\n' | sort | uniq -d)"
    fi
fi
echo ""

# 测试 4: 查询第 3 页 (page_size=20)
if [ "$TOTAL1" -gt 40 ]; then
    echo "【测试 4】查询第 3 页 (page_size=20)"
    echo "-----------------------------------------"
    RESP3=$(curl -s -X POST ${BASE_URL}/api/v1/edges/list \
      -H "Content-Type: application/json" \
      -d '{"page": 3, "page_size": 20}')
    
    TOTAL3=$(echo "$RESP3" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('total', 'N/A'))")
    COUNT3=$(echo "$RESP3" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('edges', [])))")
    PAGE3=$(echo "$RESP3" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('page', 'N/A'))")
    
    echo "总数: ${TOTAL3}"
    echo "当前页: ${PAGE3}"
    echo "返回设备数: ${COUNT3}"
    echo "✅ 第3页查询成功"
    echo ""
fi

# 测试 5: 测试不同的 page_size
echo "【测试 5】测试 page_size=50"
echo "-----------------------------------------"
RESP50=$(curl -s -X POST ${BASE_URL}/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{"page": 1, "page_size": 50}')

TOTAL50=$(echo "$RESP50" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('total', 'N/A'))")
COUNT50=$(echo "$RESP50" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('edges', [])))")

echo "总数: ${TOTAL50}"
echo "返回设备数: ${COUNT50}"
if [ "$COUNT50" -le 50 ]; then
    echo "✅ PASS: page_size=50 时数据量正确"
else
    echo "❌ FAIL: 返回了 ${COUNT50} 条，预期最多 50 条"
fi
echo ""

# 总结
echo "========================================="
echo "测试总结"
echo "========================================="
echo "设备总数: ${TOTAL1}"
echo "第1页设备数: ${COUNT1}"
echo "第2页设备数: ${COUNT2}"
echo ""
echo "前端显示验证:"
echo "  - 打开浏览器访问: ${BASE_URL}/edges"
echo "  - 验证第1页显示 20 条设备"
echo "  - 点击下一页，验证第2页显示剩余设备"
echo "  - 检查分页信息: '显示 X - Y 共 ${TOTAL1} 条'"
echo "========================================="
