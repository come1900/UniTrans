#!/bin/bash
# touch_manager 启动脚本
# 自动处理虚拟环境、依赖安装和数据库迁移

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "========================================"
echo "  touch_manager 启动脚本"
echo "========================================"
echo ""

# 配置参数
DATABASE_NAME="${DATABASE_NAME:-t-touch_manager.db}"
PORT="${PORT:-18051}"
DEFAULT_INGRESS_ID="${DEFAULT_INGRESS_ID:-local-127.0.0.1}"
DEFAULT_INGRESS_HOST="${DEFAULT_INGRESS_HOST:-127.0.0.1}"

# 1. 检查并创建虚拟环境
if [ ! -d "wpyenv" ]; then
    echo "[1/4] 创建虚拟环境..."
    python3 -m venv wpyenv
    echo "      ✓ 虚拟环境创建成功"
else
    echo "[1/4] 虚拟环境已存在"
fi

# 2. 激活虚拟环境
echo "[2/4] 激活虚拟环境..."
source wpyenv/bin/activate
echo "      ✓ 虚拟环境已激活: $(python3 --version)"

# 3. 安装依赖
echo "[3/4] 检查依赖..."
if [ -f "requirements.txt" ]; then
    # 检查是否需要安装依赖
    NEED_INSTALL=false
    while IFS= read -r package; do
        # 提取包名（去掉版本信息）
        pkg_name=$(echo "$package" | cut -d'>' -f1 | cut -d'=' -f1 | cut -d'<' -f1 | tr -d '[:space:]')
        if [ -n "$pkg_name" ] && ! pip show "$pkg_name" &>/dev/null; then
            NEED_INSTALL=true
            break
        fi
    done < requirements.txt
    
    if [ "$NEED_INSTALL" = true ]; then
        echo "      安装依赖中（使用阿里云镜像）..."
        pip install -i https://mirrors.aliyun.com/pypi/simple/ -r requirements.txt -q
        echo "      ✓ 依赖安装完成"
    else
        echo "      ✓ 依赖已安装，跳过"
    fi
else
    echo "      ⚠ 未找到 requirements.txt"
fi

# 4. 数据库迁移
echo "[4/4] 检查数据库..."
if [ ! -f "$DATABASE_NAME" ]; then
    echo "      数据库不存在，执行迁移..."
    if [ -f "migrate_db.py" ]; then
        python migrate_db.py
        echo "      ✓ 数据库迁移完成"
    else
        echo "      ⚠ 未找到 migrate_db.py，跳过迁移"
    fi
else
    echo "      ✓ 数据库已存在: $DATABASE_NAME"
fi

echo ""
echo "========================================"
echo "  启动 touch_manager"
echo "========================================"
echo ""
echo "配置信息:"
echo "  - 数据库: $DATABASE_NAME"
echo "  - 端口: $PORT"
echo "  - Ingress ID: $DEFAULT_INGRESS_ID"
echo "  - Ingress Host: $DEFAULT_INGRESS_HOST"
echo ""
echo "访问地址:"
echo "  - Web UI: http://localhost:$PORT"
echo "  - API: http://localhost:$PORT/api/v1"
echo "  - Health: http://localhost:$PORT/health"
echo ""
echo "按 Ctrl+C 停止服务"
echo "========================================"
echo ""

# 启动服务
exec DEFAULT_INGRESS_ID="$DEFAULT_INGRESS_ID" \
     DEFAULT_INGRESS_HOST="$DEFAULT_INGRESS_HOST" \
     DATABASE_NAME="$DATABASE_NAME" \
     PORT="$PORT" \
     python app.py
