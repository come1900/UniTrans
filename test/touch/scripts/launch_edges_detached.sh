#!/bin/bash
# launch_edges_detached.sh - 用 setsid 分离启动 N 个本地 edge，使其在脚本退出后仍存活
# 用法: ./launch_edges_detached.sh COUNT START_INDEX
#   COUNT=100, START_INDEX=1  -> edge001..edge100
set -u
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
EDGE_BIN="$REPO/test/touch/touch_edge/touch_edge-linux"
COUNT="${1:-100}"
START="${2:-1}"

for ((i=START; i<=COUNT; i++)); do
    eid=$(printf "edge%03d" "$i")
    key=$(printf "key%03d" "$i")
    lip="192.168.9.$((93 + i % 256))"
    setsid "$EDGE_BIN" \
        --host 127.0.0.1 --port 54321 \
        --edge-id "$eid" --edge-key "$key" --edge-type touch --local-ip "$lip" \
        >"$REPO/test/touch/touch_edge/_edge_$eid.log" 2>&1 < /dev/null &
done
echo "Launched $((COUNT-START+1)) detached edges ($START..$COUNT)"
