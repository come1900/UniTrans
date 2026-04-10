# Touch System - 开发说明

## 概述

本文档说明 Touch 系统的编译方法和测试方法。

**重要说明**：
- **正式产出物**：`touchEdge-linux`、`touchIngress-linux`、`man/touch_manager`
- **测试程序**：`test/touch/touch_edge`（仅用于模拟 Edge 进行测试）
- **原型代码**：`test/touch/touch_ingress` 已废弃，不要使用

---

## 系统架构

**详见**: [spec-design.md](spec-design.md) - 架构设计文档

**程序说明**：

| 程序 | 类型 | 位置 | 说明 |
|------|------|------|------|
| **touchEdge-linux** | ✅ 正式 | `build/` | Edge 正式程序 |
| **touchIngress-linux** | ✅ 正式 | `build/` | Ingress 正式程序 |
| **touch_manager** | ✅ 正式 | `man/touch_manager/` | Manager 正式程序（Python） |
| touch_edge-linux | 🧪 测试 | `test/touch/touch_edge/` | Edge 模拟程序（仅用于测试） |
| ~~touch_ingress-linux~~ | ❌ 废弃 | `test/touch/touch_ingress/` | 原型程序，不要使用 |

---

## 编译说明

### 环境准备

所有依赖库位于 `$HOME/libs/` 目录下：
- `$HOME/libs/include/` - 头文件
- `$HOME/libs/lib/` - 静态库

### come.1 库

**位置**: `libs/come.1/`

come.1 库定义了协议数据结构和 JSON 编解码器。

**编译和安装**:
```bash
cd $HOME/svn/daily/src/appmodule/UniTrans/libs/come.1
make          # 编译库
make install  # 安装到 $HOME/libs/
```

**重要提示**: come.1 库的任何代码修改后，必须：
1. 重新编译：`make && make install`
2. 重新编译 touchEdge 和 touchIngress

**安装位置**:
- 静态库：`$HOME/libs/lib/libcome1-$(PLATFORM).a`
- 头文件：`$HOME/libs/include/come1/come_1.h`、`$HOME/libs/include/come1/come.1.json.h`

---

## 正式程序编译与启动

### touchEdge-linux（正式程序）

**编译**:
```bash
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build
make -f 93.Makefile.Touch.Edge rebuild
```

**启动**:
```bash
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
cp ../../touchEdge-linux .
./touchEdge-linux &
```

**说明**:
- 默认配置：连接 127.0.0.1:54321
- EdgeId：根据设备硬件唯一标识（HUID）自动生成，格式为 `G1` + HUID（如 `G1509057776`）
  - 如果配置文件中 `EdgeId` 长度不足，程序会自动生成并保存回配置文件
  - 生成逻辑：`"G1" + g_Solar.GetHuid()`（`AgentTouchEdge.cpp`）
- 配置文件：`build/local/cfg-touchEdge.cfg`（从启动时的当前目录读取）
- 日志路径：`/tmp/logs/log-touchEdge.log`（由配置文件中 `13_levelLog`、`14_LogFilePath` + `15_LogFileName` 决定）
- **重要**：
  - 程序从当前工作目录读取配置文件，必须在 `build/local/` 目录下启动
  - 测试前请确保日志配置为 `37`（调试等级），日志目录已创建：`mkdir -p /tmp/logs`

**停止服务**:
```bash
# 方法 1：使用 pkill -9（推荐）
pkill -9 touchEdge-linux

# 方法 2：使用 kill -9
ps aux | grep "touchEdge-linux" | grep -v grep | awk '{print $2}' | xargs kill -9
```

---

### touchIngress-linux（正式程序）

**编译**:
```bash
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build
make -f 93.Makefile.Touch.Ingress rebuild
```

**启动**:
```bash
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
cp ../../touchIngress-linux .
./touchIngress-linux &
```

**说明**:
- 默认配置：监听 0.0.0.0:54321
- 配置文件：`build/local/cfg-touchIngress.cfg`（从启动时的当前目录读取）
- 日志路径：`/tmp/logs/log-touchIngress.log`（由配置文件中 `13_levelLog`、`14_LogFilePath` + `15_LogFileName` 决定）
- **重要**：
  - 程序从当前工作目录读取配置文件，必须在 `build/local/` 目录下启动
  - 测试前请确保日志配置为 `37`（调试等级），日志目录已创建：`mkdir -p /tmp/logs`

**停止服务**:
```bash
# 方法 1：使用 pkill -9（推荐）
pkill -9 touchIngress-linux

# 方法 2：使用 kill -9
ps aux | grep "touchIngress-linux" | grep -v grep | awk '{print $2}' | xargs kill -9
```

---

### touch_manager（正式程序）

**首次运行 - 准备虚拟环境**:

如果是首次运行或虚拟环境不存在，需要先创建虚拟环境并安装依赖：

```bash
cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager

# 1. 确保系统有 pip 和 venv（Ubuntu/Debian）
sudo apt install -y python3-pip python3-venv

# 2. 创建虚拟环境（如果不存在）
if [ ! -d "wpyenv" ]; then
    python3 -m venv wpyenv
    echo "虚拟环境创建完成"
fi

# 3. 激活虚拟环境
source wpyenv/bin/activate

# 4. 安装依赖（使用阿里云镜像加速）
pip install -i https://mirrors.aliyun.com/pypi/simple/ -r requirements.txt

# 5. （可选）验证依赖
pip list | grep -E "Flask|websockets|SQLAlchemy"

# 6. 退出虚拟环境（稍后启动时会重新激活）
deactivate
```

**备选方案**（如果没有 venv 权限）:
```bash
# 直接在用户目录安装（不推荐，但可以工作）
cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager
pip3 install -i https://mirrors.aliyun.com/pypi/simple/ --user -r requirements.txt
```

**依赖说明** (`requirements.txt`):
- `Flask>=2.0.0` - Web 框架
- `Flask-CORS>=3.0.0` - 跨域支持
- `SQLAlchemy>=1.4.0` - 数据库 ORM
- `websockets>=12.0` - WebSocket 客户端
- `requests>=2.25.0` - HTTP 客户端

**启动**:

```bash
cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager

# 激活虚拟环境
source wpyenv/bin/activate

# 启动 touch_manager（测试用）
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py
```

**快速启动**（推荐 - 自动处理虚拟环境和依赖）:

```bash
cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager

# 使用一键启动脚本（自动：创建虚拟环境、安装依赖、数据库迁移、启动服务）
./start_manager.sh

# 或使用自定义配置
DATABASE_NAME='t-touch_manager.db' PORT='18051' ./start_manager.sh
```

**完整启动命令**（包含数据库迁移）:
```bash
cd $HOME/svn/daily/src/appmodule/UniTrans/man/touch_manager
source wpyenv/bin/activate

# 首次运行或重置数据库时，先执行迁移
if [ ! -f "t-touch_manager.db" ]; then
    python migrate_db.py
    echo "数据库迁移完成"
fi

# 启动服务
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py
```

**环境变量配置**:

| 变量 | 默认值 | 说明 |
|------|--------|------|
| `PORT` | `18050` | HTTP 服务端口 |
| `DATABASE_NAME` | `touch_manager.db` | 数据库文件名（相对于 `man/touch_manager/` 目录） |
| `DATABASE_PATH` | 无 | 数据库完整路径（优先级高于 `DATABASE_NAME`） |
| `DEFAULT_INGRESS_ID` | `lili-47.100.49.48` | 默认 Ingress 实例 ID |
| `DEFAULT_INGRESS_HOST` | `47.100.49.48` | 默认 Ingress 主机地址 |
| `DEFAULT_INGRESS_PORT` | `54321` | 默认 Ingress 端口 |

**运行多套 Manager 实例**:
通过设置不同的 `DATABASE_NAME` 和 `PORT`，可以同时运行多套 Manager 实例，便于测试：
```bash
# 实例 1（测试用）
DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py

# 实例 2（正式用）
DATABASE_NAME='touch_manager.db' PORT='18050' python app.py
```

**测试用数据库**: `t-touch_manager.db`（位于 `man/touch_manager/` 目录下）
- 用途：开发和测试时使用独立的数据库，避免与正式数据混淆
- 启动方式：`DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py`
- 数据库迁移：如需重置测试数据库，删除后重新运行迁移脚本即可
  ```bash
  rm -f t-touch_manager.db
  python migrate_db.py
  ```

**停止服务**:
```bash
# 方法 1：使用 pkill
pkill -f "python app.py"

# 方法 2：使用 kill -9
ps aux | grep "python app.py" | grep -v grep | awk '{print $2}' | xargs kill -9
```

---

## 运行目录和日志

**统一运行目录**: `build/local/`

```bash
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
./touchIngress-linux &
./touchEdge-linux &
```

**配置文件**:
- `build/local/cfg-touchEdge.cfg`
- `build/local/cfg-touchIngress.cfg`

**日志配置** (重要):

测试时请确保配置文件中的日志参数为：
```json
{
   "General" : [
      {
         "13_levelLog" : 37,
         "14_LogFilePath" : "/tmp/logs",
         "15_LogFileName" : "log-touchEdge.log"
      }
   ]
}
```

**13_levelLog = 37 含义**:
- `3` (十位) = 同时输出到文件和标准输出
- `7` (个位) = 调试等级（最详细）

**日志路径** (由配置文件中 `14_LogFilePath` + `15_LogFileName` 决定):
- Edge: `/tmp/logs/log-touchEdge.log`
- Ingress: `/tmp/logs/log-touchIngress.log`

**重要**:
- 程序从**当前工作目录**读取配置文件
- 必须在 `build/local/` 目录下启动
- 测试前必须创建日志目录：`mkdir -p /tmp/logs`
- 详见 [spec-config.md](spec-config.md) - 完整日志配置说明

---

## 测试程序

### touch_edge（Edge 模拟器）

**程序类型**: 🧪 测试程序（仅用于开发测试）

**编译并启动**:
```bash
cd /home/wujj/svn/daily/src/appmodule/UniTrans/test/touch/touch_edge
make clean
make
./touch_edge-linux --host 127.0.0.1 --port 54321 --edge-id myedge --edge-key mykey
```

**停止服务**:
```bash
# 方法 1：前台运行时按 Ctrl+C
# 方法 2：使用 pkill
pkill touch_edge-linux

# 方法 3：使用 kill -9
ps aux | grep "touch_edge-linux" | grep -v grep | awk '{print $2}' | xargs kill -9
```

**重要特性**:
- **连接行为**：`reconnect_max_retries=1`（只重连 1 次后不再尝试）
- **设计目的**：故意设定为不持续重连，便于测试断开连接、踢出设备等场景
- **与正式程序区别**：正式 `touchEdge-linux` 会无限重连（`ReconnectMaxRetries=-1`），而测试程序连接失败后会退出

---

## 完整测试流程

### 0. 准备日志目录

**重要**: 测试前必须创建日志目录，并配置日志参数为 `37`（输出到文件+标准输出，调试等级）：

```bash
# 创建日志目录
mkdir -p /tmp/logs

# 验证目录存在
ls -ld /tmp/logs
```

**日志配置说明** (配置文件 `General` 部分):
```json
{
   "General" : [
      {
         "13_levelLog" : 37,
         "14_LogFilePath" : "/tmp/logs",
         "15_LogFileName" : "log-touchEdge.log"
      }
   ]
}
```

**13_levelLog = 37 含义**:
- `3` (十位) = `EZ_LOG_TO_FILE_AND_STDOUT` (同时输出到文件和标准输出)
- `7` (个位) = `EZ_LOG_LEVEL_DEBUG` (调试等级)

**详见**: [spec-config.md](spec-config.md) - 完整日志配置说明

---

### 1. 启动所有正式程序

**重要**: 必须在 `build/local/` 目录下启动，程序从当前工作目录读取配置文件！

```bash
# 终端 1：启动 touchIngress
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
./touchIngress-linux &

# 终端 2：启动 touchEdge
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build/local
./touchEdge-linux &

# 终端 3：启动 touch_manager
cd /home/wujj/svn/daily/src/appmodule/UniTrans/man/touch_manager

# 首次运行时，先准备虚拟环境（仅第一次需要）
if [ ! -d "wpyenv" ]; then
    python3 -m venv wpyenv
    source wpyenv/bin/activate
    pip install -r requirements.txt
    deactivate
    echo "虚拟环境准备完成"
fi

# 激活虚拟环境
source wpyenv/bin/activate

# （仅首次运行或数据库不存在时）执行数据库迁移
if [ ! -f "t-touch_manager.db" ]; then
    python migrate_db.py
    echo "数据库迁移完成"
fi

# 启动 touch_manager（测试用）
DEFAULT_INGRESS_ID='local-127.0.0.1' DEFAULT_INGRESS_HOST='127.0.0.1' DATABASE_NAME='t-touch_manager.db' PORT='18051' python app.py &
```

**日志路径** (由配置文件中 `14_LogFilePath` + `15_LogFileName` 决定):
- Edge: `/tmp/logs/log-touchEdge.log`
- Ingress: `/tmp/logs/log-touchIngress.log`

### 2. 启动测试 Edge 模拟器

```bash
# 终端 4：启动测试 edge
cd /home/wujj/svn/daily/src/appmodule/UniTrans/test/touch/touch_edge
./touch_edge-linux --host 127.0.0.1 --port 54321 --edge-id myedge --edge-key mykey
```

### 3. 验证测试

```bash
# 查询边缘列表
curl -X POST http://localhost:18051/api/v1/edges/list \
  -H "Content-Type: application/json" \
  -d '{}'

# 运行自动化测试
cd /home/wujj/svn/daily/src/appmodule/UniTrans/test/touch/scripts
./test_manager_api.sh
```

### 4. 停止所有服务

```bash
# 停止所有 touch 相关进程
ps aux | grep -E "touchIngress|touchEdge|touch_edge|python app.py" | grep -v grep | awk '{print $2}' | xargs kill -9

# 验证所有进程已停止
ps aux | grep -E "touchIngress|touchEdge|touch_edge|python app.py" | grep -v grep
# 应该无输出
```

---

## 测试前环境清理（重要）

**每次测试前必须执行**，否则旧进程、旧日志、旧数据库会干扰调试：

### 标准清理流程

```bash
# 1. 停止所有 touch 相关进程（使用 kill -9 确保强制停止）
pkill -9 touchIngress-linux 2>/dev/null
pkill -9 touchEdge-linux 2>/dev/null
pkill -9 touch_edge-linux 2>/dev/null
pkill -9 -f "python app.py" 2>/dev/null
sleep 2

# 2. 验证所有进程已停止
ps aux | grep -E "touchIngress|touchEdge|touch_edge|python app.py" | grep -v grep
# 应该无输出，如果有残留进程，手动 kill -9 <PID>

# 3. 验证端口已释放
netstat -tlnp 2>/dev/null | grep -E "54321|18051"
# 应该无输出

# 4. 清理日志文件
rm -f /tmp/logs/log-touchIngress.log
rm -f /tmp/logs/log-touchEdge.log
rm -f /tmp/logs/touch_manager.log
# 确保日志目录存在
mkdir -p /tmp/logs

# 5. 清理测试数据库
rm -f /home/wujj/svn/daily/src/appmodule/UniTrans/man/touch_manager/t-touch_manager.db

# 6. 清理编译产物（如果需要重新编译）
cd /home/wujj/svn/daily/src/appmodule/UniTrans/build
make -f 93.Makefile.Touch.Ingress clean
```

### 常见问题

**问题：端口 54321 仍被占用**
```bash
# 查找占用进程
fuser 54321/tcp 2>/dev/null
# 或
netstat -tlnp 2>/dev/null | grep 54321

# 强制停止
kill -9 $(fuser 54321/tcp 2>/dev/null) 2>/dev/null
```

**问题：旧日志干扰调试**
- Ingress 日志可能写入多个位置，测试前确认配置文件中的日志路径
- 使用 `find /tmp -name "*touchIngress*" -mmin -5` 查找最新日志文件

**问题：旧数据库残留数据**
- 测试数据库：`t-touch_manager.db`（位于 `man/touch_manager/` 目录）
- 如果修改了数据库结构，必须删除旧数据库并重新运行 `python migrate_db.py`

---

## 调试

### 开启调试日志

**touch_manager**:
```bash
# 在 app.py 中设置 debug=True
app.run(host=host, port=port, debug=True, use_reloader=False)
```

### 常见问题

1. **找不到 come.1 库**:
   ```bash
   cd $HOME/svn/daily/src/appmodule/UniTrans/libs/come.1
   make && make install
   ```

2. **端口已被占用**:
   ```bash
   # 检查端口是否被占用
   netstat -tlnp | grep 54321

   # 杀死占用端口的进程
   kill -9 $(lsof -t -i:54321)
   ```

3. **数据库被锁定** (touch_manager):
   ```bash
   rm -f touch_manager.db
   python migrate_db.py
   ```

---

## 清理

```bash
# 清理测试程序
cd test/touch/touch_edge
make clean

# 清理 come.1 库
cd ../../libs/come.1
make clean
```

---

## 开发流程

1. **修改 come.1 库**:
   ```bash
   cd libs/come.1
   # 编辑代码
   make && make install
   ```

2. **重新编译正式程序**:
   ```bash
   cd build
   make -f 93.Makefile.Touch.Edge rebuild
   make -f 93.Makefile.Touch.Ingress rebuild
   ```

3. **测试修改**:
   ```bash
   # 启动服务并测试
   ```

4. **提交修改**:
   ```bash
   svn add <新文件>
   ```

---

## 文件约定

- **源文件**: `.cpp`、`.h`（C++）；`.py`（Python）
- **目标文件**: `.o`（在 `obj/` 目录中）
- **可执行文件**: `*-linux`（Linux 平台）
- **注释**: 代码中使用英文（不使用 emoji）
- **编码**: UTF-8

---

## 参考文档

**重要提示**：下次使用时请先阅读以下两个文档了解 Touch 系统整体情况：

1. **[spec-design.md](spec-design.md)** - 架构设计文档
   - 三层架构设计（Edge ↔ Ingress ↔ Manager）
   - 各层职责和通信模式
   - 核心流程（边缘上线、心跳机制）
   - come.1 协议库设计

2. **[spec-api.md](spec-api.md)** - 接口规范文档
   - **整体接口规范**：REST API、WebSocket (come.1 JSON-RPC 2.0)
   - **边缘管理接口**：查询、确认、拒绝、删除
   - **Ingress 管理接口**：配置、状态查询
   - **系统状态接口**：健康检查、API 信息
   - **WebSocket 消息格式**：edge.online、edge.heartbeat、manager.edge.kick 等
   - **请求/响应格式规范**

其他文档：
- [spec-touch.md](spec-touch.md) - 整体结构
- [spec-api-websocket-jsonrpc.md](spec-api-websocket-jsonrpc.md) - WebSocket 协议详情

---

## 更新记录

### v0.2 (2026-03-31)

**术语统一**：
- 所有"设备"改为"边缘"（英文：edge）
- REST API 路径从 `/devices` 改为 `/edges`
- WebSocket 方法名从 `manager.device.*` 改为 `manager.edge.*`
- WebSocket 方法名从 `ingress.device.*` 改为 `ingress.edge.*`

**come.1 协议库**：
- `COME_METHOD_MANAGER_DEVICE_*` → `COME_METHOD_MANAGER_EDGE_*`
- `COME_METHOD_INGRESS_DEVICE_*` → `COME_METHOD_INGRESS_EDGE_*`

**touch_manager**：
- 文件重命名：`api/edges.py` → `api/edges.py`
- 文件重命名：`templates/edges.html` → `templates/edges.html`
- 数据库字段：`last_heartbeat` → `last_offline_time`
- 新增环境变量：`DEFAULT_INGRESS_ID`、`DEFAULT_INGRESS_HOST`
- 默认端口：`18050` → 可通过 `PORT` 环境变量配置

**touch_edge**：
- 命令行参数支持 `getopt_long`（长短参数名）
- 新增参数：`--host`、`--port`、`--edge-id`、`--edge-key`、`--edge-type`、`--local-ip`

**touch_ingress**：
- 命令行参数支持 `getopt_long`（长短参数名）
- 新增参数：`--port`、`--bind`
- 边缘离线时自动通知 manager（`ingress.edge.offline`）

**Makefile**：
- 新增 `rebuild` 命令（先 clean 后 all）

**文档更新**：
- `spec-api.md` - 接口规范更新
- `spec-devel.md` - 开发说明更新
- `spec-design.md` - 架构设计更新
- `spec-api-websocket-jsonrpc.md` - WebSocket 协议更新

---

### v0.1 MVP (2026-02-24)

- 初始版本
- 基础边缘注册功能
- WebSocket 通信（JSON-RPC 2.0）
- REST API 接口
- Web UI 管理界面
