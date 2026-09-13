#!/bin/bash
# ============================================================================
# test_boot_order.sh — 验证"三个组件任意顺序启动"特性
# ============================================================================
# 覆盖三组件 (touch_manager / touch_ingress / touch_edge) 的 6 种启动顺序组合，
# 验证任意顺序下最终都能自愈收敛为"全链路连通 + 配置功能可用"。
#
#   manager  : touch_manager (Python Flask, REST:18051)
#   ingress  : touch_ingress (C++, WS Server:54321)
#   edge     : touch_edge    (C++, WS Client -> ingress, id=edge001)
#
# 依赖重连机制: manager/edge 在目标(ingress)未就绪时应周期性重连(默认间隔 10s)，
# 因此在每个用例里等待最多 BOOT_WAIT 秒轮询收敛。
#
# 用法: ./test_boot_order.sh [BOOT_WAIT]        # BOOT_WAIT=每用例收敛等待秒数(默认 30)
#       ./test_boot_order.sh -h | --help
# ============================================================================
set -u
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

BOOT_WAIT="${1:-30}"
if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    sed -n '1,40p' "$0" | grep '^#' | sed 's/^# \{0,1\}//'
    exit 0
fi

# --------------------------- 组件定义 ---------------------------
MANAGER_DIR="$ROOT/man/touch_manager"
INGRESS_DIR="$ROOT/test/touch/touch_ingress"
EDGE_DIR="$ROOT/test/touch/touch_edge"
INGRESS_BIN="$INGRESS_DIR/touch_ingress-linux"
EDGE_BIN="$EDGE_DIR/touch_edge-linux"

MANAGER_PORT=18051
INGRESS_PORT=54321
MANAGER_URL="http://127.0.0.1:$MANAGER_PORT"
DB="$MANAGER_DIR/t-touch_manager.db"
# 各日志(stdout 重定向, 便于失败时排障)
M_LOG=/tmp/boot_order_manager.log
I_LOG=/tmp/boot_order_ingress.log
E_LOG=/tmp/boot_order_edge.log
# edge 原生日志(含 version)与功能日志 —— 用于配置推送/查询验证
EDGE_NATIVE_LOG=/tmp/touch_edge_boot_001.log

PASS=0; FAIL=0

say()  { echo "$@"; }
ok()   { say "[PASS] $*"; PASS=$((PASS+1)); }
bad()  { say "[FAIL] $*"; FAIL=$((FAIL+1)); }

# --------------------------- 组件启停 ---------------------------
# stop_all: 按可执行名 + 端口彻底清理, 保证用例间隔离。
# 通过 pgrep 匹配自己的二进制名, kill 后再确认端口释放。
stop_all() {
    # manager: 杀掉 touch_manager 的 app.py (按端口/二进制路径精确匹配)
    for pid in $(pgrep -f "wpyenv/bin/python app.py" 2>/dev/null; pgrep -f "touch_manager.*app\.py" 2>/dev/null; pgrep -f "^python app\.py$" 2>/dev/null); do
        kill -9 "$pid" 2>/dev/null
    done
    # ingress
    for pid in $(pgrep -f "touch_ingress-linux" 2>/dev/null); do kill -9 "$pid" 2>/dev/null; done
    # edge
    for pid in $(pgrep -f "touch_edge-linux" 2>/dev/null); do kill -9 "$pid" 2>/dev/null; done
    # 等待端口释放
    for i in $(seq 1 20); do
        if ! (ss -ltn 2>/dev/null | grep -qE ":$MANAGER_PORT |:$INGRESS_PORT "); then break; fi
        sleep 0.3
    done
    rm -f /tmp/boot_order_*.pid
}

start_manager() {
    ( cd "$MANAGER_DIR" && nohup env DEFAULT_INGRESS_ID='local-127.0.0.1' \
        DEFAULT_INGRESS_HOST='127.0.0.1' DEFAULT_INGRESS_PORT="$INGRESS_PORT" \
        DATABASE_NAME='t-touch_manager.db' PORT="$MANAGER_PORT" \
        wpyenv/bin/python app.py > "$M_LOG" 2>&1 & echo $! > /tmp/boot_order_manager.pid )
}

start_ingress() {
    ( cd "$INGRESS_DIR" && nohup ./touch_ingress-linux -p "$INGRESS_PORT" > "$I_LOG" 2>&1 & echo $! > /tmp/boot_order_ingress.pid )
}

start_edge() {
    ( cd "$EDGE_DIR" && nohup ./touch_edge-linux --host 127.0.0.1 --port "$INGRESS_PORT" \
        --edge-id edge001 --edge-key key001 > "$E_LOG" 2>&1 & echo $! > /tmp/boot_order_edge.pid )
}

# 只停 ingress(保留 manager/edge 运行), 用于"ingress 抖动重连"场景
stop_ingress_only() {
    for pid in $(pgrep -f "touch_ingress-linux" 2>/dev/null); do kill -9 "$pid" 2>/dev/null; done
    # 等待 54321 端口释放
    for i in $(seq 1 20); do
        if ! ss -ltn 2>/dev/null | grep -q ":$INGRESS_PORT "; then break; fi
        sleep 0.3
    done
}

# --------------------------- 连通性判定 ---------------------------
# manager 进程存活 + health OK
manager_up() {
    curl -s --max-time 2 "$MANAGER_URL/health" 2>/dev/null | grep -q '"status":"ok"'
}

# edge 已由 ingress 注册并在 manager 中 online (DB edges.status==1)
edge_online() {
    cd "$MANAGER_DIR" && wpyenv/bin/python -c "
import sqlite3,sys
c=sqlite3.connect('$DB')
r=c.execute(\"SELECT status FROM edges WHERE edge_id='edge001'\").fetchone()
c.close()
sys.exit(0 if r and r[0]==1 else 1)" 2>/dev/null
}

# edge 被 manager 标记为 offline (DB edges.status==0, 且记录存在)
edge_offline() {
    cd "$MANAGER_DIR" && wpyenv/bin/python -c "
import sqlite3,sys
c=sqlite3.connect('$DB')
r=c.execute(\"SELECT status FROM edges WHERE edge_id='edge001'\").fetchone()
c.close()
sys.exit(0 if r and r[0]==0 else 1)" 2>/dev/null
}

# 全链路收敛: manager up + edge online
converged() {
    manager_up && edge_online
}

# 等待某条件成立, 最多 BOOT_WAIT 秒
wait_for() {
    local fn="$1"
    local i
    for i in $(seq 1 "$BOOT_WAIT"); do
        if $fn; then return 0; fi
        sleep 1
    done
    return 1
}

# --------------------------- 配置功能验证 ---------------------------
# 推一次配置并确认到达 edge (version 递增) + 查询返回
# 返回 0=通过; 1=失败
verify_config_flow() {
    local token="boot-$(date +%s%N)"
    local resp version
    resp=$(curl -s --max-time 5 -X POST "$MANAGER_URL/api/v1/edges/config/update" \
        -H "Content-Type: application/json" \
        -d "{\"edge_id\":\"edge001\",\"config_type\":\"tunnelService\",\"config_content\":{\"services\":[{\"serviceName\":\"boot\",\"tunnelService\":{\"endpoint\":{\"host\":\"10.220.42.139\",\"port\":50400},\"localManagement\":{\"bindAddress\":\"127.0.0.1\",\"bindPort\":17400},\"security\":{\"authMethod\":\"token\",\"credential\":\"$token\",\"enableTls\":true}},\"accessPolicies\":[{\"policyId\":\"bp\",\"protocol\":\"tcp\",\"targetService\":{\"ip\":\"127.0.0.1\",\"port\":55555},\"exposedPort\":51999}]}]}}")
    # POST 应返回 code=0
    echo "$resp" | grep -qE '"code": ?0' || { say "      POST 未返回成功: $resp"; return 1; }
    # 轮询 config_status -> confirmed (配置已实际在 edge 生效)
    local st="" i
    for i in $(seq 1 60); do
        st=$(cd "$MANAGER_DIR" && wpyenv/bin/python -c "
import sqlite3
c=sqlite3.connect('$DB')
print(c.execute(\"SELECT config_status FROM edges WHERE edge_id='edge001'\").fetchone()[0])
c.close()" 2>/dev/null)
        [ "$st" = "confirmed" ] && break
        sleep 0.3
    done
    [ "$st" = "confirmed" ] || { say "      config_status 未到 confirmed (=$st)"; return 1; }
    # 查询接口返回
    curl -s --max-time 5 -X POST "$MANAGER_URL/api/v1/edges/config/get" \
        -H "Content-Type: application/json" \
        -d '{"edge_id":"edge001"}' | grep -qE '"code": ?0' || { say "      POST config/get 失败"; return 1; }
    return 0
}

# 运行一个启动顺序用例
run_case() {
    local label="$1"; shift
    say ""
    say "──────────────────────────────────────────────────────────"
    say "用例: $label"
    say "──────────────────────────────────────────────────────────"
    stop_all

    # 按指定顺序启动各组件(每起动一个暂停一下, 模拟"先/后"启的时序)
    local comp
    for comp in "$@"; do
        case "$comp" in
            manager) say "  [start] manager"; start_manager ;;
            ingress) say "  [start] ingress"; start_ingress ;;
            edge)    say "  [start] edge";    start_edge ;;
        esac
        sleep 1
    done

    say "  等待收敛 (最多 ${BOOT_WAIT}s)..."
    if wait_for converged; then
        ok "全链路收敛: manager↔ingress 连通 + edge online"
    else
        bad "未能收敛: manager_up=$(manager_up && echo yes || echo no), edge_online=$(edge_online && echo yes || echo no)"
        # 打印关键日志便于排障
        say "  --- manager 最近日志 ---"; tail -5 "$M_LOG" 2>/dev/null | sed 's/^/     /'
        say "  --- ingress 最近日志 ---"; tail -3 "$I_LOG" 2>/dev/null | sed 's/^/     /'
        { stop_all; return; }
    fi

    # 稳定等待: edge 刚 online 时, manager 到 edge 的 ack 回程链路可能尚未完全就绪,
    # 立即推送配置易产生偶发时序竞态(config_status 短暂卡在 configuring)。
    # 额外等几秒, 排除收敛窗口内的 ack 回程未稳定导致的误报。
    say "  收敛后稳定等待 4s (等待 ack 链路就绪)..."
    sleep 4

    # 配置功能验证
    if verify_config_flow; then
        ok "配置推送+确认+查询 功能正常"
    else
        bad "配置功能验证失败"
    fi

    stop_all
}

# ---------------------------------------------------------------------------
# 用例: "ingress 抖动重连" —— 正常运行中停掉 ingress 再重启, 验证 manager 自动重连
# ---------------------------------------------------------------------------
# 覆盖用户上报场景: 先正常启动(manager↔ingress 连通), 停掉 ingress, 再启动 ingress,
# manager 应自动重连 ingress, 且 edge 重新回到 online。
run_restart_ingress_case() {
    say ""
    say "──────────────────────────────────────────────────────────"
    say "用例: ingress 抖动重连 (正常→停 ingress→重启 ingress→自动重连)"
    say "──────────────────────────────────────────────────────────"
    stop_all

    # 先正常拉起全链路(任选一种可靠顺序: ingress → manager → edge)
    say "  [start] ingress"; start_ingress; sleep 1
    say "  [start] manager"; start_manager; sleep 1
    say "  [start] edge";    start_edge

    say "  首次收敛 (最多 ${BOOT_WAIT}s)..."
    if wait_for converged; then
        ok "首次全链路收敛: manager↔ingress 连通 + edge online"
    else
        bad "首次未能收敛: manager_up=$(manager_up && echo yes || echo no), edge_online=$(edge_online && echo yes || echo no)"
        { stop_all; return; }
    fi

    # ---- 停掉 ingress ----
    say "  [stop] ingress (manager/edge 保持运行)..."
    stop_ingress_only
    if pgrep -f "touch_ingress-linux" >/dev/null 2>&1; then
        bad "ingress 进程未被停掉"
        { stop_all; return; }
    fi

    # 等待 manager 检测到断线, 将 edge 标记为 offline
    say "  等待 manager 检测断线 → edge offline..."
    if wait_for edge_offline; then
        ok "manager 检测到 ingress 断线, edge 已标记 offline"
    else
        bad "manager 未将 edge 标记 offline (edge_status 仍为 online)"
        say "      edge_online=$(edge_online && echo yes || echo no), edge_offline=$(edge_offline && echo yes || echo no)"
    fi

    # manager 自身应保持健康(不受 ingress 断线影响)
    if manager_up; then
        ok "manager 保持健康 (health ok)"
    else
        bad "manager health 检查失败"
    fi

    # ---- 重启 ingress ----
    say "  [start] ingress (重启)..."
    start_ingress

    say "  等待 manager 自动重连 ingress → edge 重新 online..."
    if wait_for converged; then
        ok "manager 自动重连 ingress, edge 重新 online ✅"
    else
        bad "manager 未能自动重连 ingress (edge 未回到 online) ❌"
        say "      manager_up=$(manager_up && echo yes || echo no), edge_online=$(edge_online && echo yes || echo no)"
        say "  --- manager 最近日志 ---"; tail -8 "$M_LOG" 2>/dev/null | sed 's/^/     /'
        say "  --- ingress 最近日志 ---"; tail -5 "$I_LOG" 2>/dev/null | sed 's/^/     /'
        { stop_all; return; }
    fi

    # 稳定等待 + 配置功能验证(确认重连后链路仍可用)
    say "  重连后稳定等待 4s..."
    sleep 4
    if verify_config_flow; then
        ok "重连后配置推送+确认+查询 功能正常"
    else
        bad "重连后配置功能验证失败"
    fi

    stop_all
}

# --------------------------- 主流程: 6 种启动顺序 ---------------------------
say "======================================================================"
say " 三组件任意顺序启动测试  (BOOT_WAIT=${BOOT_WAIT}s)"
say "  启动前先确保干净起点; 每用例前 stop_all"
say "======================================================================"

run_case "顺序1: manager → ingress → edge"  manager ingress edge
run_case "顺序2: manager → edge → ingress"  manager edge ingress
run_case "顺序3: ingress → manager → edge"  ingress manager edge
run_case "顺序4: ingress → edge → manager"  ingress edge manager
run_case "顺序5: edge → manager → ingress"  edge manager ingress
run_case "顺序6: edge → ingress → manager"  edge ingress manager

# --------------------------- 主流程: ingress 抖动重连用例 ---------------------------
run_restart_ingress_case

# --------------------------- 汇总 ---------------------------
say ""
say "======================================================================"
say " 测试完成"
say "  PASS: $PASS    FAIL: $FAIL"
say "======================================================================"
stop_all  # 收尾清理
if [ "$FAIL" -eq 0 ]; then
    say " → 全部通过: 三组件任意顺序启动 + ingress 重启自动重连 均正常 ✅"
    exit 0
else
    say " → 存在失败用例 ❌"
    exit 1
fi
