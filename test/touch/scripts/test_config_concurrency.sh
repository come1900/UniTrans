#!/bin/bash
# 严格并发验证: 版本唯一性(按edge RECV新增窗口) + schema统一 + 最终一致性
set -u
M=http://localhost:18051; EID=edge001
EL=/tmp/touch_edge_001.log
N=${1:-40}

# 基线
L0=$(wc -l < "$EL")
V0=$(cd /home/10312200/svn/daily/src/come1900/UniTrans/man/touch_manager && wpyenv/bin/python -c "import sqlite3;c=sqlite3.connect('t-touch_manager.db');print(c.execute('SELECT version FROM edge_configs WHERE edge_id=?',('$EID',)).fetchone()[0]);c.close()")

START=$(date +%s%3N)
for i in $(seq 1 $N); do
  TK="vfy-$(date +%s%N)-$i"
  curl -s -X POST "$M/api/v1/edges/config/update" -H "Content-Type: application/json" \
    -d "{\"edge_id\":\"$EID\",\"config_type\":\"tunnelService\",\"config_content\":{\"services\":[{\"serviceName\":\"s\",\"tunnelService\":{\"endpoint\":{\"host\":\"10.220.42.139\",\"port\":50400},\"localManagement\":{\"bindAddress\":\"127.0.0.1\",\"bindPort\":17400},\"security\":{\"authMethod\":\"token\",\"credential\":\"$TK\",\"enableTls\":true}},\"accessPolicies\":[{\"policyId\":\"p$i\",\"protocol\":\"tcp\",\"targetService\":{\"ip\":\"127.0.0.1\",\"port\":$((80000+i))},\"exposedPort\":51422}]}]}}" \
    > /dev/null &
done
wait
END=$(date +%s%3N)
sleep 5   # 等待 edge ack 全部回
L1=$(wc -l < "$EL")

echo "=== 并发推送: $N 个, 总耗时 $((END-START)) ms, 平均 $(( (END-START)/N )) ms ==="

# 本次窗口新增 RECV version
NEW=$(sed -n "${L0},${L1}p" "$EL" | grep "edge.config.update" | grep -oE '"version": ?[0-9]+' | grep -oE '[0-9]+')
echo "--- 本次 edge 新增 RECV version 数: $(echo "$NEW" | wc -w) ---"
echo "序列: $(echo $NEW | tr '\n' ' ')"
DUP=$(echo "$NEW" | sort -n | uniq -d)
[ -z "$DUP" ] && echo ">>> 版本唯一性: 通过 (无复用)" || echo ">>> 版本唯一性: ⚠ 有复用: $DUP"

# 最终版本
V1=$(cd /home/10312200/svn/daily/src/come1900/UniTrans/man/touch_manager && wpyenv/bin/python -c "import sqlite3;c=sqlite3.connect('t-touch_manager.db');print(c.execute('SELECT version FROM edge_configs WHERE edge_id=?',('$EID',)).fetchone()[0]);c.close()")
echo "--- version: 起点=$V0  终点(DB)=$V1  期望终点=$((V0+N)) ---"
[ "$V1" -eq $((V0+N)) ] && echo ">>> DB 版本连续性: 正确 ($V0 -> $V1, +$N)" || echo ">>> DB 版本连续性: 异常"

# 最终一致性: GET / DB / edge文件
echo ""
echo "=== 最终一致性: GET 查询 vs edge 实际文件 ==="
G=$(curl -s -X POST "$M/api/v1/edges/config/get" -H "Content-Type: application/json" \
    -d "{\"edge_id\":\"$EID\"}" | python3 -c "
import sys,json
d=json.load(sys.stdin)
c=d['config']
print('GET version='+str(d['config_version']),'status='+str(d['config_status']))
print('  GET token=', c.get('auth',{}).get('token','NONE'), ' localPort=', c.get('proxies',[{}])[0].get('localPort','NONE'))
print('  GET keys=', sorted(c.keys()))
")
echo "$G"
EFILE=/home/10312200/svn/daily/src/come1900/UniTrans/test/touch/touch_edge/shpc/shpc.edge001.json
FT=$(grep -oE '"token":"[^"]*"' "$EFILE" | sed 's/"token":"//;s/"//;')
FL=$(grep -oE '"localPort":[0-9]*' "$EFILE" | sed 's/"localPort"://')
echo "  edge文件 token=$FT localPort=$FL"
DBJ=$(cd /home/10312200/svn/daily/src/come1900/UniTrans/man/touch_manager && wpyenv/bin/python -c "
import sqlite3,json
c=sqlite3.connect('t-touch_manager.db')
r=c.execute('SELECT config_json FROM edge_configs WHERE edge_id=?',('$EID',)).fetchone()[0]
d=json.loads(r)
print(d.get('auth',{}).get('token',''), d.get('proxies',[{}])[0].get('localPort',''))
c.close()")
DBT=$(echo $DBJ | awk '{print $1}'); DBL=$(echo $DBJ | awk '{print $2}')
echo "  DB 内容 token=$DBT localPort=$DBL"
echo ""
if [ "$FL" = "$DBL" ] && [ "$FL" = "$(echo "$G"|grep -oE 'localPort= [0-9]+'|grep -oE '[0-9]+'|head -1)" ]; then
    echo ">>> 一致性: edge文件 localPort=$FL 与 DB localPort=$DBL 一致"
else
    echo ">>> 一致性: ⚠ edge文件=$FL / DB=$DBL"
    echo "   (edge文件是最新到达的配置, DB可能被后台query回读覆盖——对比token核实)"
fi
