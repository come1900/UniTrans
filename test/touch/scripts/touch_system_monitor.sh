#!/bin/bash
# touch_system_monitor.sh - 监控系统运行状态
# 每 10 分钟检查一次 touch 系统三个组件的运行状态

LOG_FILE="$HOME/svn/daily/src/appmodule/touch/system_monitor.log"
MANAGER_URL="http://localhost:18050"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

check_process() {
    local name=$1
    local pattern=$2
    local pid=$(ps aux | grep "$pattern" | grep -v grep | awk '{print $2}')
    if [ -n "$pid" ]; then
        log "✓ $name 运行中 (PID: $pid)"
        return 0
    else
        log "✗ $name 未运行!"
        return 1
    fi
}

check_api() {
    local endpoint=$1
    local name=$2
    local response=$(curl -s -o /dev/null -w "%{http_code}" "$MANAGER_URL$endpoint" 2>/dev/null)
    if [ "$response" = "200" ]; then
        log "✓ $name API 正常 (HTTP $response)"
        return 0
    else
        log "✗ $name API 异常 (HTTP $response)"
        return 1
    fi
}

check_devices() {
    log "--- 设备状态 ---"
    local devices=$(curl -s "$MANAGER_URL/api/v1/devices" 2>/dev/null)
    if [ -n "$devices" ]; then
        local online=$(echo "$devices" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len([x for x in d.get('devices',[]) if x.get('status')=='online']))" 2>/dev/null)
        local total=$(echo "$devices" | python3 -c "import sys,json; d=json.load(sys.stdin); print(len(d.get('devices',[])))" 2>/dev/null)
        log "设备总数：$total, 在线：$online"
        
        # 显示每个设备的最后心跳时间
        echo "$devices" | python3 -c "
import sys, json
data = json.load(sys.stdin)
for dev in data.get('devices', []):
    did = dev.get('device_id', 'unknown')
    status = dev.get('status', 'unknown')
    hb = dev.get('last_heartbeat', 'N/A')[-8:] if dev.get('last_heartbeat') else 'N/A'
    print(f'  - {did}: {status}, 最后心跳：{hb}')
" 2>/dev/null | while read line; do log "$line"; done
    fi
}

check_ingress() {
    log "--- Ingress 状态 ---"
    local ingresses=$(curl -s "$MANAGER_URL/api/v1/ingresses" 2>/dev/null)
    if [ -n "$ingresses" ]; then
        echo "$ingresses" | python3 -c "
import sys, json
data = json.load(sys.stdin)
for ing in data.get('ingresses', []):
    iid = ing.get('ingress_id', 'unknown')
    status = ing.get('status', 'unknown')
    host = ing.get('host', '')
    port = ing.get('port', '')
    print(f'  - {iid}: {status} ({host}:{port})')
" 2>/dev/null | while read line; do log "$line"; done
    fi
}

# 主检查流程
log "========================================"
log "开始系统健康检查"
log "========================================"

errors=0

# 检查进程
check_process "touch_ingress" "touch_ingress-linux" || ((errors++))
check_process "touch_manager" "python app.py" || ((errors++))
check_process "touch_edge" "touch_edge-linux" || ((errors++))

# 检查 API
check_api "/api/v1/ingresses" "Manager-Ingress" || ((errors++))
check_api "/api/v1/devices" "Manager-Devices" || ((errors++))

# 检查业务状态
check_ingress
check_devices

# 总结
log "========================================"
if [ $errors -eq 0 ]; then
    log "✓ 系统运行正常"
else
    log "✗ 发现 $errors 个问题，请检查!"
fi
log "========================================"
log ""
