#!/bin/bash
# Test remote config query and update
# 使用 nested 格式（config_type + config_content.services[].tunnelService）

MANAGER_URL="http://localhost:18050"
EDGE_ID="edge001"

echo "=== Test Remote Config Query & Update ==="
echo ""

# Test 1: GET config (query)
echo "1. Query current config from manager..."
echo ""

RESPONSE=$(curl -s -X POST "${MANAGER_URL}/api/v1/edges/config/get" \
  -H "Content-Type: application/json" \
  -d "{\"edge_id\": \"${EDGE_ID}\"}")
echo "Response from POST /api/v1/edges/config/get:"
echo "$RESPONSE" | python3 -m json.tool
echo ""

# Test 2: POST config (update, nested format)
echo "2. Send new config to manager..."
echo ""

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
            "credential": "test-token-456",
            "enableTls": true
          }
        },
        "accessPolicies": [{
          "policyId": "w-tcp-51422",
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

echo "Response from POST /api/v1/edges/config/update:"
echo "$RESPONSE" | python3 -m json.tool
echo ""

# 部署是异步的，轮询等待配置文件更新（最长 30 秒）
echo "Waiting for config to be applied (async deploy)..."
WAITED=0
while [ "$WAITED" -lt 30 ]; do
    if [ -f "./shpc/shpc.edge001.json" ] && grep -q "test-token-456" ./shpc/shpc.edge001.json 2>/dev/null; then
        break
    fi
    sleep 2
    WAITED=$((WAITED+2))
done
echo "  (waited ${WAITED}s)"
echo ""

# Test 3: GET config again (verify update)
echo "3. Query updated config from manager..."
echo ""

RESPONSE=$(curl -s -X POST "${MANAGER_URL}/api/v1/edges/config/get" \
  -H "Content-Type: application/json" \
  -d "{\"edge_id\": \"${EDGE_ID}\"}")
echo "Response from POST /api/v1/edges/config/get (after update):"
echo "$RESPONSE" | python3 -m json.tool
echo ""

# Test 4: Check the actual config file on edge (verify proxies are written)
echo "4. Check actual config file written by edge..."
echo ""

if [ -f "./shpc/shpc.edge001.json" ]; then
    echo "Config file exists: ./shpc/shpc.edge001.json"
    echo "Content:"
    cat ./shpc/shpc.edge001.json | python3 -m json.tool
    echo ""
    echo "Proxy verification:"
    python3 -c "
import json, sys
with open('./shpc/shpc.edge001.json') as f:
    cfg = json.load(f)
ok = True
if cfg.get('serverAddr') != '10.220.42.139':
    print('  FAIL: serverAddr'); ok = False
if cfg.get('auth', {}).get('token') != 'test-token-456':
    print('  FAIL: token'); ok = False
proxies = cfg.get('proxies', [])
if len(proxies) != 1:
    print('  FAIL: expect 1 proxy, got', len(proxies)); ok = False
else:
    p = proxies[0]
    if p.get('name') != 'w-tcp-51422': print('  FAIL: proxy name'); ok = False
    if p.get('localPort') != 55555: print('  FAIL: proxy localPort'); ok = False
    if p.get('remotePort') != 51422: print('  FAIL: proxy remotePort'); ok = False
print('  PASS' if ok else '  FAIL')
sys.exit(0 if ok else 1)
"
else
    echo "Config file not found: ./shpc/shpc.edge001.json"
    echo "FAIL"
fi
echo ""
