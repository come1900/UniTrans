#!/bin/bash
# perf_test_local.sh - 本地三组件性能测试
#
# 用法: ./perf_test_local.sh [EDGE_COUNT] [START_INDEX] [BATCH]
#   默认: 100 个 edge, 从 1 开始, 每批 50
#
# 前置: touch_ingress(54321) + touch_manager(18051) 已启动
# 说明: 按批启动 touch_edge-linux 测试模拟器，逐批等待注册后统计上线数/耗时。

set -u

EDGE_COUNT="${1:-100}"
START_INDEX="${2:-1}"
BATCH="${3:-50}"

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
EDGE_BIN="$REPO/test/touch/touch_edge/touch_edge-linux"
MANAGER_URL="${MANAGER_URL:-http://localhost:18051}"

if [ ! -f "$EDGE_BIN" ]; then
    echo "错误: edge 二进制不存在 $EDGE_BIN"
    exit 1
fi

echo "=============================================="
echo " Touch 三组件性能测试 (本地 127.0.0.1:54321)"
echo " 目标边缘数: $EDGE_COUNT, 批次: $BATCH"
echo "=============================================="

# 清理之前启动的本地 edge(仅限本脚本启动的 edgeXXX, 保留核心 edge001)
pkill -f "touch_edge-linux.*--edge-id edge" 2>/dev/null
sleep 1

START_TOTAL=$(date +%s%N)
LAUNCHED=0

for ((start=START_INDEX; start<=EDGE_COUNT; start+=BATCH)); do
    end=$((start+BATCH-1))
    [ $end -gt $EDGE_COUNT ] && end=$EDGE_COUNT

    echo ""
    echo "[批] 启动 edge$start ~ edge$end ..."
    BATCH_START=$(date +%s%N)

    for i in $(seq $start $end); do
        eid=$(printf "edge%03d" "$i")
        key=$(printf "key%03d" "$i")
        lip="192.168.9.$((93 + i % 256))"
        ( cd "$REPO/test/touch/touch_edge" && "$EDGE_BIN" \
            --host 127.0.0.1 --port 54321 \
            --edge-id "$eid" --edge-key "$key" --edge-type touch --local-ip "$lip" \
            >/dev/null 2>&1 ) &
        LAUNCHED=$((LAUNCHED+1))
    done

    # 等待本批注册
    sleep 3

    # 统计已上线边缘数(通过 manager API)
    TOTAL=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
        -H "Content-Type: application/json" -d '{"page_size":1000}' \
        | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('total',0))" 2>/dev/null)
    ONLINE=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
        -H "Content-Type: application/json" -d '{"page_size":1000}' \
        | python3 -c "import sys,json; d=json.load(sys.stdin); 
on=sum(1 for e in d.get('edges',[]) if e.get('status')==1); print(on)" 2>/dev/null)

    BATCH_END=$(date +%s%N)
    BATCH_MS=$(( (BATCH_END-BATCH_START)/1000000 ))
    echo "  [批完成] 累计启动=$LAUNCHED, DB总记录=$TOTAL, 在线=$ONLINE, 本批耗时=${BATCH_MS}ms"
done

END_TOTAL=$(date +%s%N)
TOTAL_MS=$(( (END_TOTAL-START_TOTAL)/1000000 ))

echo ""
echo "=============================================="
echo " 最终统计"
echo " 启动边缘数: $LAUNCHED"
echo " 总耗时: ${TOTAL_MS}ms ($((TOTAL_MS/1000))s)"
echo "=============================================="

# 最终状态核对
curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
    -H "Content-Type: application/json" -d '{"page_size":1000}' \
    | python3 -c "
import sys,json
d=json.load(sys.stdin)
edges=d.get('edges',[])
total=d.get('total',0)
online=sum(1 for e in edges if e.get('status')==1)
print(f'DB total={total}, online={online}')
print('PASS' if total>=int('$EDGE_COUNT') else 'CHECK')
"
