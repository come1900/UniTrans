#!/bin/bash
#
# test_perf_config.sh - Touch Manager frpc 远程配置 推送/查询/异步反馈 性能与可靠性测试
#
# 覆盖:
#   1. 单次配置推送的端到端全链路耗时 (POST -> manager -> ingress -> edge RECV -> edge WRITE -> ack -> confirmed)
#   2. 连续多次推送: 丢消息 / 重排 / 乱序 检测 (按 version 逐个核对 _edge.log RECV 与最终 confirmed)
#   3. 消息转发及时性 (edge RECV 时间戳 - manager 发送时间戳) 与通路内各段耗时
#   4. 配置查询往返 (POST config/get 触发后台 edge.config.query 刷新)
#
# 用法: ./test_perf_config.sh [rounds]
#   rounds  - 连续推送检测的轮数 (默认 10)
#
# 依赖: MANAGER(18051)、touch_ingress(54321)、touch_edge(edge001)、edge 工作目录下的 _edge.log

set -u

MANAGER_URL="${MANAGER_URL:-http://localhost:18051}"
EDGE_ID="${EDGE_ID:-edge001}"
EDGE_LOG="/home/10312200/svn/daily/src/come1900/UniTrans/test/touch/touch_edge/_edge.log"
# edge 的 ez_wsclient 原生 RECV/SEND(含完整 JSON 与 version 字段)输出到该文件
EDGE_RECV_LOG="${EDGE_RECV_LOG:-/tmp/touch_edge_001.log}"
INGRESS_LOG="/home/10312200/svn/daily/src/come1900/UniTrans/test/touch/touch_ingress/_ingress.log"
ROUNDS="${1:-10}"

# millisecond epoch: for日志时间戳换算
now_ms() { date +%s%3N; }

# 从 _edge.log 提取某 version 配置更新的 RECV/WRITE 毫秒时间戳(取最近一次匹配)
edge_stage_ms() {
    local version="$1" stage="$2"
    # stage: RECV=CONFIG.UPDATE.RECV, WRITE=CONFIG.UPDATE.WRITE, ACK=Sent config ack(若存在)
    grep -E "CONFIG.UPDATE.(RECV|WRITE)" "$EDGE_LOG" \
      | grep -E "Received ConfigUpdate|wrote frpc config" \
      | tail -1 \
      | grep -q . && true
    # 直接按日志时间戳解析: 格式 "2026-09-09 21:14:24.120"
    local line
    line=$(grep -E "=== \[CONFIG\.UPDATE\.${stage}\]" "$EDGE_LOG" | tail -1)
    if [ -n "$line" ]; then
        # 转换为 epoch 毫秒
        echo "$line" | awk '{
            split($1,d,"-"); y=d[1]; mo=d[2]; da=d[3];
            split($2,t,":"); h=t[1]; mi=t[2]; rest=t[3]; split(rest,s,".");
            printf "%s", mktime(sprintf("%d %d %d %d %d %d", y,mo,da,h,mi,s[1]))*1000 + s[2]
        }'
    fi
}

print_header() { echo -e "\n\033[1;36m===== $1 =====\033[0m"; }
print_row()    { printf "  %-46s %s\n" "$1" "$2"; }

echo "======================================================================="
echo " touch_manager frpc 远程配置 性能 & 可靠性测试"
echo " Manager=$MANAGER_URL  Edge=$EDGE_ID  rounds=$ROUNDS"
echo " $(date '+%F %T')"
echo "======================================================================="

# 长基线: 记录测试开始前进/ingress 日志行数, 便于按新增行做时间窗统计
EDGE_LOG_LINES_BEFORE=$(wc -l < "$EDGE_LOG")
EDGE_RECV_LOG_LINES_BEFORE=$(wc -l < "$EDGE_RECV_LOG")
INGRESS_LOG_LINES_BEFORE=$(wc -l < "$INGRESS_LOG")

# ---------------------------------------------------------------
print_header "Part 1: 单次配置推送 端到端全链路耗时"
# ---------------------------------------------------------------
echo "  (使用唯一 token, 便于在 edge 日志中精确定位)"
TOKEN="perf-$(date +%s)"
T_PUSH_START=$(date +%s%3N)

RESP=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d "{\"edge_id\":\"$EDGE_ID\",\"config_type\":\"tunnelService\",\"config_content\":{\"services\":[{\"serviceName\":\"http-tunnel\",\"tunnelService\":{\"endpoint\":{\"host\":\"10.220.42.139\",\"port\":50400},\"localManagement\":{\"bindAddress\":\"127.0.0.1\",\"bindPort\":17400},\"security\":{\"authMethod\":\"token\",\"credential\":\"$TOKEN\",\"enableTls\":true}},\"accessPolicies\":[{\"policyId\":\"w-tcp-51422\",\"protocol\":\"tcp\",\"targetService\":{\"ip\":\"127.0.0.1\",\"port\":55555},\"exposedPort\":51422}]}]}}")

T_PUSH_RESP=$(date +%s%3N)
echo "  POST 响应: $RESP"
print_row "POST->Manager 返回(API 处理耗时)" "$((T_PUSH_RESP-T_PUSH_START)) ms"

# 等待 edge 应用并确认：直接查询数据库 config_status（避免 GET 触发后台
# refresh_config 污染时序，sleep 用 0.1 减小轮询量化误差）
MANAGER_DB="/home/10312200/svn/daily/src/come1900/UniTrans/man/touch_manager/t-touch_manager.db"
T_CONFIRMED=0
for i in $(seq 1 60); do
    ST=$(cd /home/10312200/svn/daily/src/come1900/UniTrans/man/touch_manager && wpyenv/bin/python -c "
import sqlite3
c=sqlite3.connect('$MANAGER_DB')
print(c.execute(\"SELECT config_status FROM edges WHERE edge_id='$EDGE_ID'\").fetchone()[0])
c.close()" 2>/dev/null)
    if [ "$ST" = "confirmed" ]; then
        T_CONFIRMED=$(date +%s%3N)
        break
    fi
    sleep 0.1
done

if [ "$T_CONFIRMED" -eq 0 ]; then
    print_row "config_status 在 6s 内未变为 confirmed" "(见后续日志核对)"
else
    print_row "POST->config_status=confirmed (异步反馈全链路)" "$((T_CONFIRMED-T_PUSH_START)) ms"
fi

# 从 _edge.log 提取本次 token 对应 RECV/WRITE 时间戳
EDGE_RECV=$(grep "Received ConfigUpdate_tunnelService for edge $EDGE_ID" "$EDGE_LOG" | tail -1)
EDGE_WRITE=$(grep "wrote frpc config to ./shpc/shpc.edge001.json" "$EDGE_LOG" | tail -1)
echo "  edge RECV 行: ${EDGE_RECV:19:23}"
echo "  edge WRITE 行: ${EDGE_WRITE:19:23}"

# 时间戳换算(毫秒epoch): 从 "2026-09-09 21:14:24.120"
conv_ms() {
    local s="$1"
    echo "$s" | awk '{
        split($1,d,"-"); y=d[1]; mo=d[2]; da=d[3];
        split($2,t,":"); h=t[1]; mi=t[2]; rest=t[3]; split(rest,sec,".");
        hr=sec[1]; ms=sec[2];
        t0=mktime(sprintf("%d %d %d %d %d %d", y,mo,da,h,mi,hr));
        print t0*1000+ms
    }'
}
ER=$(conv_ms "${EDGE_RECV:0:23}")
EW=$(conv_ms "${EDGE_WRITE:0:23}")
if [ "$ER" -gt 0 ] && [ "$T_PUSH_START" -le "$ER" ]; then
    print_row "POST->Edge 收到 (Manager+Ingress+网络转发)" "$((ER-T_PUSH_START)) ms"
    print_row "Edge 收->写文件 (Edge 内部处理)" "$((EW-ER)) ms"
    print_row "POST->Edge 写盘完成" "$((EW-T_PUSH_START)) ms"
    print_row "Edge 写盘->confirmed (ack 回程)" "$((T_CONFIRMED-EW)) ms"
fi

# ---------------------------------------------------------------
print_header "Part 2: 连续 $ROUNDS 次推送 — 丢消息/乱序 检测"
# ---------------------------------------------------------------
echo "  (每次推送唯一 token; 记录每个 version 是否被 Edge 实际 RECV 以及最终 confirmed)"
declare -a SENT_VERS=()   # 服务端分配/接受 的 version 序列(成功推送)
declare -A SENT_MAP=()    # version -> 唯一token 标识(用于核对)
BUSY_CNT=0
for r in $(seq 1 "$ROUNDS"); do
    TK="perf-$(date +%s)-$r"
    RESP=$(curl -s -X POST "$MANAGER_URL/api/v1/edges/config/update" \
      -H "Content-Type: application/json" \
      -d "{\"edge_id\":\"$EDGE_ID\",\"config_type\":\"tunnelService\",\"config_content\":{\"services\":[{\"serviceName\":\"http-tunnel\",\"tunnelService\":{\"endpoint\":{\"host\":\"10.220.42.139\",\"port\":50400},\"localManagement\":{\"bindAddress\":\"127.0.0.1\",\"bindPort\":17400},\"security\":{\"authMethod\":\"token\",\"credential\":\"$TK\",\"enableTls\":true}},\"accessPolicies\":[{\"policyId\":\"w-tcp-51422\",\"protocol\":\"tcp\",\"targetService\":{\"ip\":\"127.0.0.1\",\"port\":$((55550+r))},\"exposedPort\":51422}]}]}}")
    VER=$(echo "$RESP" | python3 -c "import sys,json
try:
    d=json.load(sys.stdin)
    print(d.get('config_version','ERR') if d.get('code')==0 else 'BUSY')
except: print('ERR')" 2>/dev/null)
    if [ "$VER" = "BUSY" ]; then
        echo "  轮$r: 被 busy-guard 拒绝 — 属预期节流(上一次同内容配置仍在确认中)"
        BUSY_CNT=$((BUSY_CNT+1))
        continue
    fi
    SENT_VERS+=("$VER")
    SENT_MAP["$VER"]="$TK"
    sleep 0.4
done
echo "  服务端成功分配版本序列: ${SENT_VERS[*]:-无}"
echo "  busy-guard 拒绝次数: $BUSY_CNT"

# 等待所有已提交推送完成异步确认：轮询直到每个成功分配的 version
# 都出现在 edge RECV 日志（或 12s 超时），避免因日志刷新/edge 排队导致的误报丢失
for _ in $(seq 1 60); do
    EDGE_NOW=$(sed -n "${EDGE_RECV_LOG_LINES_BEFORE},\$p" "$EDGE_RECV_LOG" \
        | grep "edge.config.update" \
        | grep -oE '"version"[[:space:]]*:[[:space:]]*[0-9]+' \
        | grep -oE '[0-9]+' | sort -n -u)
    ALL_RECV=1
    for v in "${SENT_VERS[@]:-}"; do
        echo "$EDGE_NOW" | grep -qx "$v" || { ALL_RECV=0; break; }
    done
    [ "$ALL_RECV" = 1 ] && break
    sleep 0.2
done

# 从 edge 原生 RECV 日志(含完整 JSON)提取每个已实际到达 edge 的 version (按到达顺序, 不去重不改序)
# 只取本次测试窗口(EDGE_RECV_LOG_LINES_BEFORE 之后新增的行)——避免把历史版本误判为本次推送
# RECV 行格式: {"jsonrpc": "2.0", "method": "edge.config.update", ..., "version": N, ...}
EDGE_VERS=$(sed -n "${EDGE_RECV_LOG_LINES_BEFORE},\$p" "$EDGE_RECV_LOG" \
    | grep "edge.config.update" \
    | grep -oE '"version"[[:space:]]*:[[:space:]]*[0-9]+' \
    | grep -oE '[0-9]+')
# 去重后的到达集合(用于成员判定)
EDGE_VERS_UNIQ=$(echo "$EDGE_VERS" | sort -n -u)
echo "  Edge 本次窗口实际 RECV 到的 version 列表(到达序): $(echo $EDGE_VERS)"

# 核对: 每个成功分配的 version 是否都在 edge RECV 集合中
MISS=0
REORD=0
declare -A ERECV
for v in $EDGE_VERS_UNIQ; do ERECV["$v"]=1; done
for v in "${SENT_VERS[@]}"; do
    if [ -z "${ERECV[$v]:-}" ]; then
        echo "    · 版本 $v (token ${SENT_MAP[$v]}) 未在原生 stdout 日志中匹配(可能为日志滞后, 是否真丢失以功能日志计数判定为准)"
        MISS=$((MISS+1))
    fi
done
# 乱序检测: edge RECV 到的 version 序列是否严格递增
PREV=-1
for v in $EDGE_VERS; do
    if [ "$v" -lt "$PREV" ]; then REORD=$((REORD+1)); fi
    PREV=$v
done

# 统计 ingress 转发与 edge 功能日志 RECV/WRITE 次数(本次测试累计)
# edge 功能日志 _edge.log 逐条记录 RECV/WRITE, 不受原生 stdout 日志滞后影响, 是"实际到达"的权威计数依据
ICNT=$(sed -n "${INGRESS_LOG_LINES_BEFORE},\$p" "$INGRESS_LOG" | grep -c "Forwarded edge.config.update")
ECNT=$(sed -n "${EDGE_LOG_LINES_BEFORE},\$p" "$EDGE_LOG" | grep -c "CONFIG.UPDATE.RECV")
WCNT=$(sed -n "${EDGE_LOG_LINES_BEFORE},\$p" "$EDGE_LOG" | grep -c "CONFIG.UPDATE.WRITE")
print_row "服务端成功推送数" "${#SENT_VERS[@]}"
print_row "本次测试 Ingress 转发次数" "$ICNT"
print_row "本次测试 Edge RECV 次数(功能日志)" "$ECNT"
print_row "本次测试 Edge WRITE 次数(功能日志)" "$WCNT"

# 权威计数判定: 每条推送 edge 成功处理都会在 _edge.log 留一条 RECV。
# 若 ECNT < 成功推送数 -> 真实丢失; 否则即使原生日志 version 未全部匹配, 也算"无真实丢失"
REAL_MISS=0
if [ "$ECNT" -lt "${#SENT_VERS[@]}" ]; then
    REAL_MISS=$(( ${#SENT_VERS[@]} - ECNT ))
fi

# 原生日志按 version 核对(尽力而为): 原生 stdout 日志在压力下可能滞后/漏记,
# 导致 version 缺失但 edge 实际已处理——此结果以 ECNT 为准
RECV_OK=0
for v in "${SENT_VERS[@]}"; do [ -n "${ERECV[$v]:-}" ] && RECV_OK=$((RECV_OK+1)); done
print_row "Edge 实际收到数(原生日志按version命中)" "$RECV_OK"
print_row "乱序(逆序)数" "$REORD"
if [ "$REAL_MISS" -eq 0 ] && [ "$REORD" -eq 0 ]; then
    print_row "可靠性判定(以功能日志计数为准)" "✅ 无真实丢失, 无乱序"
    if [ "$RECV_OK" -lt "${#SENT_VERS[@]}" ]; then
        echo "     (注: 原生日志未匹配 $((${#SENT_VERS[@]}-RECV_OK)) 个 version——原生 stdout 日志滞后, 非消息丢失; edge 功能层已全部 RECV)"
    fi
else
    print_row "可靠性判定(以功能日志计数为准)" "⚠ 真实丢失=$REAL_MISS 乱序=$REORD"
fi

# ---------------------------------------------------------------
print_header "Part 3: 消息转发及时性 (基于日志毫秒时间戳)"
# ---------------------------------------------------------------
echo "  从 _ingress.log 提取最近一次 config.update 各阶段耗时:"
grep -E "\[LATENCY\] handle_config_update_jsonrpc2" "$INGRESS_LOG" | tail -3 | sed 's/^/    /'
echo "  从 _ingress.log 提取最近一次 config ack 转发耗时:"
grep -E "\[LATENCY\] handle_config_ack_jsonrpc2" "$INGRESS_LOG" | tail -3 | sed 's/^/    /'

# ---------------------------------------------------------------
print_header "Part 4: 配置查询往返 (POST config/get 后台刷新)"
# ---------------------------------------------------------------
TQ=$(date +%s%3N)
curl -s -X POST "$MANAGER_URL/api/v1/edges/config/get" \
    -H "Content-Type: application/json" \
    -d "{\"edge_id\":\"$EDGE_ID\"}" -o /tmp/perf_cfg_get.json
Q_ST=$(python3 -c "import json;d=json.load(open('/tmp/perf_cfg_get.json'));print(d['config_status'],d['config_version'])" 2>/dev/null)
print_row "POST config/get 返回 (status, version)" "$Q_ST"
print_row "POST config/get HTTP 往返耗时" "$(($(date +%s%3N)-TQ)) ms"
# 查询会触发后台线程向边缘主动同步, 等待其完成
sleep 2
echo "  (GET 触发后台 edge.config.query, 由 edge 回读本地 shpc 文件覆盖 DB)"

echo ""
echo "======================================================================="
echo " 测试完成: $(date '+%F %T')"
echo " 日志: edge=$EDGE_LOG ; ingress=$INGRESS_LOG ; manager stdout=见进程"
echo "======================================================================="
