#!/bin/bash
# Test remote config update frpc config localPort (nested format)

MANAGER_URL="http://localhost:18050"
EDGE_ID="edge001"

TOKEN="737da3d24390c0f8601198eb5568e361046d92224266f44dcc2bc7fb13a2f4cc"
POLICY_ID="w-tcp-51422"

BODY() {
  local local_port="$1"
  cat <<EOF
{
  "edge_id": "${EDGE_ID}",
  "config_type": "tunnelService",
  "config_content": {
    "services": [{
      "serviceName": "http-tunnel",
      "tunnelService": {
        "endpoint": {"host": "10.220.42.139", "port": 50400},
        "localManagement": {"bindAddress": "127.0.0.1", "bindPort": 17400},
        "security": {"authMethod": "token", "credential": "${TOKEN}", "enableTls": true}
      },
      "accessPolicies": [{
        "policyId": "${POLICY_ID}",
        "protocol": "tcp",
        "targetService": {"ip": "127.0.0.1", "port": ${local_port}},
        "exposedPort": 51422
      }]
    }]
  }
}
EOF
}

check_local_port() {
  local expected="$1"
  if [ ! -f "./shpc/shpc.edge001.json" ]; then
    echo "FAIL: config file not found"
    return 1
  fi
  python3 -c "
import json, sys
with open('./shpc/shpc.edge001.json') as f:
    cfg = json.load(f)
proxies = cfg.get('proxies', [])
actual = proxies[0].get('localPort') if proxies else None
if actual != $expected:
    print('FAIL: localPort expected=$expected got=%s' % actual)
    sys.exit(1)
print('PASS: localPort=%s' % actual)
"
}

# 部署是异步的，轮询等待文件中的 localPort 变为期望值（最长 30 秒）
wait_local_port() {
  local expected="$1"
  local waited=0
  while [ "$waited" -lt 30 ]; do
    if [ -f "./shpc/shpc.edge001.json" ] && \
       python3 -c "
import json,sys
try:
    cfg=json.load(open('./shpc/shpc.edge001.json'))
    p=cfg.get('proxies',[])
    sys.exit(0 if (p and p[0].get('localPort')==$expected) else 1)
except Exception:
    sys.exit(1)
" 2>/dev/null; then
      break
    fi
    sleep 2
    waited=$((waited+2))
  done
  echo "  (waited ${waited}s)"
}

echo "=== Test Update frpc Config (localPort) ==="
echo ""

# Test 1: Send initial config with localPort=55555
echo "1. Sending initial config (localPort=55555) to manager..."
RESPONSE=$(curl -s -X POST "${MANAGER_URL}/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d "$(BODY 55555)")
echo "Response: $(echo "$RESPONSE" | python3 -m json.tool)"
echo "Waiting for localPort=55555 to be applied (async deploy)..."
wait_local_port 55555
echo ""
echo "2. Checking edge config file after initial config..."
cat ./shpc/shpc.edge001.json 2>/dev/null | python3 -m json.tool
check_local_port 55555
echo ""

# Test 2: Update localPort from 55555 to 66666
echo "3. Updating localPort from 55555 to 66666..."
RESPONSE=$(curl -s -X POST "${MANAGER_URL}/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d "$(BODY 66666)")
echo "Response: $(echo "$RESPONSE" | python3 -m json.tool)"
echo "Waiting for localPort=66666 to be applied (async deploy)..."
wait_local_port 66666
echo ""
echo "4. Checking edge config file after update..."
cat ./shpc/shpc.edge001.json 2>/dev/null | python3 -m json.tool
check_local_port 66666
echo ""

# Test 3: Revert localPort back to 55555
echo "5. Updating localPort from 66666 back to 55555..."
RESPONSE=$(curl -s -X POST "${MANAGER_URL}/api/v1/edges/config/update" \
  -H "Content-Type: application/json" \
  -d "$(BODY 55555)")
echo "Response: $(echo "$RESPONSE" | python3 -m json.tool)"
echo "Waiting for localPort=55555 to be applied (async deploy)..."
wait_local_port 55555
echo ""
echo "6. Checking edge config file after reverting..."
cat ./shpc/shpc.edge001.json 2>/dev/null | python3 -m json.tool
check_local_port 55555
echo ""
