#!/bin/bash
# 快速版本遍历测试 - 只测试 Ingress 是否能稳定运行
set -e

BASEDIR="/home/wujj/svn/daily/src/appmodule/UniTrans"
START_REV=${1:-4316}
END_REV=${2:-4319}

echo "========================================="
echo "版本遍历测试: r${START_REV} -> r${END_REV}"
echo "========================================="

for REV in $(seq $START_REV $END_REV); do
    echo ""
    echo "--------- 测试 r${REV} ---------"
    
    # 1. 停止所有进程
    pkill -9 touchIngress-linux 2>/dev/null || true
    pkill -9 -f "python app.py" 2>/dev/null || true
    sleep 1
    
    # 2. 切换版本
    cd $BASEDIR
    svn up -r ${REV} --quiet 2>&1 | grep -v "^正在" || true
    
    # 3. 编译 come.1
    cd $BASEDIR/libs/come.1
    make clean > /dev/null 2>&1
    make > /dev/null 2>&1
    make install > /dev/null 2>&1
    
    # 4. 编译 Ingress
    cd $BASEDIR/build
    BUILD_OUT=$(make -f 93.Makefile.Touch.Ingress rebuild 2>&1)
    if [ $? -ne 0 ]; then
        echo "r${REV}: ❌ 编译失败"
        echo "  错误: $(echo "$BUILD_OUT" | grep -i 'error:' | head -1)"
        continue
    fi
    
    # 5. 清理日志
    rm -f /tmp/logs/log-touchIngress.log
    
    # 6. 启动 Ingress
    cd $BASEDIR/build/local
    cp ../../touchIngress-linux . 2>/dev/null || true
    ./touchIngress-linux &
    INGRESS_PID=$!
    
    # 7. 等待 12 秒，观察是否存活
    sleep 12
    
    if kill -0 $INGRESS_PID 2>/dev/null; then
        echo "r${REV}: ✅ 运行正常 (PID=$INGRESS_PID)"
        kill -9 $INGRESS_PID 2>/dev/null || true
    else
        echo "r${REV}: ❌ 已退出"
        LOG_LINE=$(tail -5 /tmp/logs/log-touchIngress.log 2>/dev/null | grep -E "haveException|exit signal|Stopped" | head -1)
        if [ -n "$LOG_LINE" ]; then
            echo "  原因: $LOG_LINE"
        fi
    fi
    
    sleep 1
done

echo ""
echo "========================================="
echo "测试完成"
echo "========================================="
