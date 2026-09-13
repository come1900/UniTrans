#!/bin/bash
# test_remote_cfg_shpc.sh - Test remote config deployment

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="$SCRIPT_DIR/../touch_edge/shpc/shpc.edge001.json"

MANAGER_URL="http://localhost:18050"
EDGE_ID="edge001"

echo "=== Test Remote Config Deployment ==="
echo ""

# 记录部署前配置文件的初始状态（存在与否 + 最后修改时间），用于判断部署后文件是否真的被更新
if [ -f "$CONFIG_FILE" ]; then
    PREV_MTIME=$(stat -c %Y "$CONFIG_FILE")
    echo "部署前配置文件已存在: $CONFIG_FILE (mtime=$(date -d @$PREV_MTIME '+%H:%M:%S'))"
else
    PREV_MTIME=0
    echo "部署前配置文件不存在: $CONFIG_FILE"
fi
echo ""
echo "Sending config to manager..."
echo ""

# Send config via REST API (nested format: config_type + config_content.services)
RESPONSE=$(curl -s -X POST "${MANAGER_URL}/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d '{
    "edge_id": "'"${EDGE_ID}"'",
    "config_type": "tunnelService",
    "config_content": {
      "services": [{
        "serviceName": "http-tunnel",
        "tunnelService": {
          "endpoint": {
            "host": "10.220.42.139",
            "port": 50400
          },
          "localManagement": {
            "bindAddress": "127.0.0.1",
            "bindPort": 17400
          },
          "security": {
            "authMethod": "token",
            "credential": "737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc",
            "enableTls": true
          }
        },
        "accessPolicies": [{
          "policyId": "w-tcp-55555",
          "protocol": "tcp",
          "targetService": {
            "ip": "127.0.0.1",
            "port": 55555
          },
          "exposedPort": 51422
        }]
      }]
    }
  }')

echo "Response from manager:"
echo "$RESPONSE" | python3 -m json.tool
echo ""

# 若 manager 返回"配置已确认且未变化"，则配置内容未改变，edge 不会重写文件，
# 文件 mtime 自然不变 —— 直接收工，无需等待/校验/确认文件。
if echo "$RESPONSE" | grep -q "already confirmed and unchanged"; then
    echo "配置未变化（Config already confirmed and unchanged）—— 文件不会被更新，无需确认文件 ✅"
    exit 0
fi

# 部署是异步的，间隔性查 2 次；每次检查前先等 1s，给 edge 留出写入时间。
# 判定"文件被本次部署更新"用 mtime 晚于部署前记录，不傻等 30s。
echo "Waiting for config file to be written by edge (async deploy)..."
WAITED=0
for i in 1 2; do
    sleep 1; WAITED=$((WAITED+1))  # 先稍等，让 edge 写好
    if [ -f "$CONFIG_FILE" ] && [ "$(stat -c %Y "$CONFIG_FILE" 2>/dev/null)" -gt "$PREV_MTIME" ]; then
        UPDATED=1
        break
    fi
    sleep 1; WAITED=$((WAITED+1))
done
echo "  (checked ${WAITED}s)"
echo ""

# Verify the config file written by edge
UPDATED=${UPDATED:-0}
NEW_MTIME=$(stat -c %Y "$CONFIG_FILE" 2>/dev/null || echo 0)
echo "Verifying edge config file..."
if [ "$UPDATED" -eq 1 ]; then
    echo "配置文件已更新: $CONFIG_FILE (mtime=$(date -d @$NEW_MTIME '+%H:%M:%S'), 部署前=$PREV_MTIME)"
    echo "Content:"
    cat "$CONFIG_FILE" | python3 -m json.tool
    echo ""
    python3 -c "
import json, sys
with open('$CONFIG_FILE') as f:
    cfg = json.load(f)
ok = True
checks = {
    'serverAddr': cfg.get('serverAddr') == '10.220.42.139',
    'serverPort': cfg.get('serverPort') == 50400,
    'auth.method': cfg.get('auth', {}).get('method') == 'token',
    'transport.tls.enable': cfg.get('transport', {}).get('tls', {}).get('enable') is True,
    'webServer.addr': cfg.get('webServer', {}).get('addr') == '127.0.0.1',
    'webServer.port': cfg.get('webServer', {}).get('port') == 17400,
}
proxies = cfg.get('proxies', [])
checks['proxies.count'] = len(proxies) == 1
if proxies:
    checks['proxies[0].name'] = proxies[0].get('name') == 'w-tcp-55555'
    checks['proxies[0].localPort'] = proxies[0].get('localPort') == 55555
    checks['proxies[0].remotePort'] = proxies[0].get('remotePort') == 51422
for k, v in checks.items():
    print(('  PASS ' if v else '  FAIL ') + k)
    ok = ok and v
sys.exit(0 if ok else 1)
"
    RC=$?
    echo ""
    if [ "$RC" -eq 0 ]; then
        echo "配置内容校验通过 ✅"
    else
        echo "配置内容校验存在 FAIL 项 ❌"
    fi
else
    if [ -f "$CONFIG_FILE" ]; then
        echo "WARN: 配置文件已存在($CONFIG_FILE)但 mtime 未更新(仍=$(date -d @$NEW_MTIME '+%H:%M:%S')) —— 本次部署可能未生效/未写入"
    else
        echo "FAIL: config file not found: $CONFIG_FILE"
    fi
fi
echo ""
