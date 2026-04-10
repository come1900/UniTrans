#!/bin/bash
# Touch Ingress & Manager 连接测试脚本
# 用法: ./test_ingress_manager.sh <svn_revision>

set -e

REVISION=$1
BASEDIR="/home/wujj/svn/daily/src/appmodule/UniTrans"

echo "========================================="
echo "测试 SVN 版本: r${REVISION}"
echo "========================================="

# 1. 停止所有相关进程
echo "[1/8] 停止所有相关进程..."
pkill -9 touchIngress-linux 2>/dev/null || true
pkill -9 touch_edge-linux 2>/dev/null || true
pkill -f "python app.py" 2>/dev/null || true
sleep 2

# 2. 回退到指定版本
echo "[2/8] 回退到 SVN r${REVISION}..."
cd $BASEDIR
svn up -r ${REVISION} --quiet

# 3. 清理日志和数据库
echo "[3/8] 清理日志和数据库..."
rm -f /tmp/logs/log-touchIngress.log
rm -f /home/wujj/logs/log-touchIngress.log
rm -f $BASEDIR/man/touch_manager/t-touch_manager.db
rm -f /tmp/ingress_test_output.log

# 4. 编译 Ingress
echo "[4/8] 编译 touchIngress..."
cd $BASEDIR/build
make -f 93.Makefile.Touch.Ingress rebuild > /tmp/build_ingress.log 2>&1
if [ $? -ne 0 ]; then
    echo "❌ 编译失败!"
    tail -20 /tmp/build_ingress.log
    exit 1
fi
echo "✅ 编译成功"

# 5. 启动 Manager
echo "[5/8] 启动 touchManager..."
cd $BASEDIR/man/touch_manager
source wpyenv/bin/activate
DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py &
MANAGER_PID=$!
sleep 3

# 检查 Manager 是否启动成功
if ! kill -0 $MANAGER_PID 2>/dev/null; then
    echo "❌ Manager 启动失败!"
    exit 1
fi

# 验证 Manager 健康状态
HEALTH=$(curl -s http://localhost:18051/health 2>/dev/null)
if [ -z "$HEALTH" ]; then
    echo "❌ Manager 健康检查失败!"
    kill -9 $MANAGER_PID 2>/dev/null || true
    exit 1
fi
echo "✅ Manager 启动成功: $HEALTH"

# 6. 启动 Ingress
echo "[6/8] 启动 touchIngress..."
cd $BASEDIR/build/local
cp ../../touchIngress-linux .
./touchIngress-linux &
INGRESS_PID=$!
sleep 3

# 检查 Ingress 是否启动成功
if ! kill -0 $INGRESS_PID 2>/dev/null; then
    echo "❌ Ingress 启动失败,进程已退出!"
    echo "--- Ingress 日志 ---"
    cat /tmp/logs/log-touchIngress.log 2>/dev/null || echo "无日志文件"
    kill -9 $MANAGER_PID 2>/dev/null || true
    exit 1
fi

# 检查端口监听
if ! netstat -tlnp 2>/dev/null | grep -q 54321; then
    echo "❌ Ingress 端口 54321 未监听!"
    kill -9 $INGRESS_PID $MANAGER_PID 2>/dev/null || true
    exit 1
fi
echo "✅ Ingress 启动成功,端口监听中"

# 7. 启动测试 Edge
echo "[7/8] 启动测试 Edge 模拟器..."
cd $BASEDIR/test/touch/touch_edge
make > /tmp/build_edge.log 2>&1
./touch_edge-linux --host 127.0.0.1 --port 54321 --edge-id test-edge-001 --edge-key testkey001 &
EDGE_PID=$!
sleep 5

# 8. 检查通讯状态
echo "[8/8] 检查 Ingress 和 Manager 通讯状态..."

# 检查 Edge 是否连接到 Ingress
EDGE_CONNECTED=$(netstat -an 2>/dev/null | grep 54321 | grep ESTABLISHED | wc -l)
echo "  - Edge 到 Ingress 连接数: $EDGE_CONNECTED"

# 检查 Manager 是否收到 Edge 上线通知
EDGE_LIST=$(curl -s -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{}' 2>/dev/null)

EDGE_COUNT=$(echo "$EDGE_LIST" | python3 -c "import sys, json; print(json.load(sys.stdin).get('total', 0))" 2>/dev/null || echo "0")
echo "  - Manager 边缘列表总数: $EDGE_COUNT"

# 检查 Ingress 日志中的 Manager 连接
MANAGER_CONNECT_LOG=$(grep -c "manager.connect\|CLIENT_TYPE_MANAGER" /tmp/logs/log-touchIngress.log 2>/dev/null || echo "0")
echo "  - Ingress 日志中 Manager 连接记录: $MANAGER_CONNECT_LOG"

# 检查 Ingress 日志中的 edge.online 通知
EDGE_ONLINE_LOG=$(grep -c "Sent edge.online request\|notify_device_online" /tmp/logs/log-touchIngress.log 2>/dev/null || echo "0")
echo "  - Ingress 日志中 Edge 上线通知: $EDGE_ONLINE_LOG"

# 最终判定
echo ""
echo "========================================="
echo "测试结果 (r${REVISION}):"
echo "========================================="

if [ "$EDGE_COUNT" -gt 0 ] 2>/dev/null; then
    echo "✅ 通讯正常: Edge 已上线,Manager 已收到通知"
    echo "$EDGE_LIST" | python3 -m json.tool 2>/dev/null | head -20
    RESULT="SUCCESS"
elif [ "$MANAGER_CONNECT_LOG" -gt 0 ] 2>/dev/null && [ "$EDGE_ONLINE_LOG" -gt 0 ] 2>/dev/null; then
    echo "⚠️  部分正常: Manager 已连接,Edge 上线通知已发送,但 Manager 未确认"
    RESULT="PARTIAL"
elif [ "$MANAGER_CONNECT_LOG" -gt 0 ] 2>/dev/null; then
    echo "⚠️  Manager 已连接 Ingress,但 Edge 上线失败"
    RESULT="MANAGER_CONNECTED"
else
    echo "❌ 通讯失败: Manager 未连接到 Ingress 或 Edge 未上线"
    RESULT="FAILED"
fi

echo ""
echo "--- Ingress 最新日志 (最后30行) ---"
tail -30 /tmp/logs/log-touchIngress.log 2>/dev/null || echo "无日志"

echo ""
echo "--- 进程状态 ---"
echo "Manager PID: $MANAGER_PID (运行: $(kill -0 $MANAGER_PID 2>/dev/null && echo '是' || echo '否'))"
echo "Ingress PID: $INGRESS_PID (运行: $(kill -0 $INGRESS_PID 2>/dev/null && echo '是' || echo '否'))"
echo "Edge PID: $EDGE_PID (运行: $(kill -0 $EDGE_PID 2>/dev/null && echo '是' || echo '否'))"

# 清理
echo ""
echo "清理测试环境..."
kill -9 $EDGE_PID $INGRESS_PID $MANAGER_PID 2>/dev/null || true
sleep 1

echo ""
echo "测试结果: $RESULT"
echo "========================================="

# 返回结果
if [ "$RESULT" = "SUCCESS" ]; then
    exit 0
else
    exit 1
fi
