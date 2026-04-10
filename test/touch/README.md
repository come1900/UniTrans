# Touch System - Test/Touch

命令行式快速测试环境，复用 `src/Function/Touch/` 的正式实现代码。

## 目录结构

```
test/touch/
├── touch_edge/          # Edge（边缘设备）- 复用 src/Function/Touch/Edge
│   ├── Main.cpp         # 测试用 main
│   └── Makefile
├── touch_ingress/       # Ingress Service（接入服务）- 复用 src/Function/Touch/Ingress
│   ├── Main.cpp         # 测试用 main
│   └── Makefile
├── scripts/             # 辅助脚本
├── spec/                # 设计文档
├── Makefile             # 顶层 Makefile
└── README.md            # 本文件
```

## 编译

```bash
# 编译所有组件
make

# 或单独编译
make touch_edge
make touch_ingress
```

## 运行

### 启动 touch_ingress

```bash
cd touch_ingress
./touch_ingress-linux [-p|--port PORT]
```

示例：
```bash
# 默认端口 54321
./touch_ingress-linux

# 指定端口
./touch_ingress-linux -p 54321
```

### 启动 touch_edge

```bash
cd touch_edge
./touch_edge-linux [ingress_host] [ingress_port] [device_id] [device_key] [device_type]
```

示例：
```bash
# 连接到 ingress (127.0.0.1:54321)，设备 ID: edge001
./touch_edge-linux 127.0.0.1 54321 edge001 key001 touch

# 使用默认参数（127.0.0.1:54321, edge001）
./touch_edge-linux
```

## 清理

```bash
make clean
```

## 说明

- `touch_edge` 和 `touch_ingress` 复用 `src/Function/Touch/` 下的正式代码
- 仅在本地维护 `Main.cpp` 和 `Makefile` 用于快速测试
- 正式代码修改后，重新编译即可生效
