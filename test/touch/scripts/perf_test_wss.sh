#!/bin/bash
# perf_test_wss.sh - WSS 链路性能测试（连接规模）
#
# 用法: ./perf_test_wss.sh [EDGE_COUNT] [BATCH]
#   默认: 100 个 edge, 每批 20（建议分批，勿整批一次轰 50+，易触发并发 TLS 握手尖峰）
#
# 前置: touch_ingress(wss 54443) + touch_manager(18051) 已启动
# 说明: 按批启动 touch_edge-linux (--wss 连 54443)，逐批统计上线数/耗时。
#       明文 ws 对比可用 perf_test_local.sh。
# 吞吐/时延/数据完整性: 见 libezsocket/test/t-wss-*bench（raw wss 传输层）。

set -u

EDGE_COUNT="${1:-100}"
BATCH="${2:-20}"
WSS_PORT="${WSS_PORT:-54443}"

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
EDGE_BIN="$REPO/test/touch/touch_edge/touch_edge-linux"
MANAGER_URL="${MANAGER_URL:-http://localhost:18051}"

# 用 manager 的 venv python 做 JSON 解析（系统 python3 版本过旧可能报错）
PY="${PY:-$REPO/man/touch_manager/wpyenv/bin/python}"
[ -x "$PY" ] || PY="python3"
JSTAT() { "$PY" -c "import sys,json
d=json.load(sys.stdin)
edges=d.get('edges',[])
print(d.get('total',0))
print(sum(1 for e in edges if e.get('status')==1))"; }

if [ ! -f "$EDGE_BIN" ]; then
    echo "错误: edge 二进制不存在 $EDGE_BIN"
    exit 1
fi

echo "=============================================="
echo " Touch WSS 性能测试 (本地 127.0.0.1:${WSS_PORT}, TLS)"
echo " 目标边缘数: $EDGE_COUNT, 批次: $BATCH"
echo "=============================================="

pkill -f "touch_edge-linux.*--edge-id edge" 2>/dev/null
sleep 1

START_TOTAL=$(date +%s%N)
LAUNCHED=0

for ((start=1; start<=EDGE_COUNT; start+=BATCH)); do
    end=$((start+BATCH-1))
    [ $end -gt $EDGE_COUNT ] && end=$EDGE_COUNT

    echo ""
    echo "[批] 启动 edge$start ~ edge$end (wss) ..."
    BATCH_START=$(date +%s%N)

    for i in $(seq $start $end); do
        eid=$(printf "edge%03d" "$i")
        key=$(printf "key%03d" "$i")
        lip="192.168.9.$((93 + i % 256))"
        ( cd "$REPO/test/touch/touch_edge" && "$EDGE_BIN" \
            --host 127.0.0.1 --port "$WSS_PORT" --wss \
            --edge-id "$eid" --edge-key "$key" --edge-type touch --local-ip "$lip" \
            >/dev/null 2>&1 ) &
        LAUNCHED=$((LAUNCHED+1))
    done

    sleep 3

    read TOTAL ONLINE < <(curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
        -H "Content-Type: application/json" -d '{"page_size":1000}' | JSTAT 2>/dev/null)
    TOTAL="${TOTAL:-?}"; ONLINE="${ONLINE:-?}"

    BATCH_END=$(date +%s%N)
    BATCH_MS=$(( (BATCH_END-BATCH_START)/1000000 ))
    echo "  [批完成] 累计启动=$LAUNCHED, DB总记录=$TOTAL, 在线=$ONLINE, 本批耗时=${BATCH_MS}ms"
done

END_TOTAL=$(date +%s%N)
TOTAL_MS=$(( (END_TOTAL-START_TOTAL)/1000000 ))

echo ""
echo "=============================================="
echo " 最终统计 (WSS)"
echo " 启动边缘数: $LAUNCHED"
echo " 总耗时: ${TOTAL_MS}ms ($((TOTAL_MS/1000))s)"
echo "=============================================="

read TOTAL ONLINE < <(curl -s -X POST "$MANAGER_URL/api/v1/edges/list" \
    -H "Content-Type: application/json" -d '{"page_size":1000}' | JSTAT 2>/dev/null)
echo "DB total=$TOTAL, online=$ONLINE"
if [ "${TOTAL:-0}" -ge "$EDGE_COUNT" ] && [ "${ONLINE:-0}" -ge "$EDGE_COUNT" ]; then
    echo "PASS: $EDGE_COUNT 个 edge 经 wss 全部在线"
else
    echo "CHECK: total=$TOTAL online=$ONLINE（建议减小批次 BATCH 再试）"
fi
