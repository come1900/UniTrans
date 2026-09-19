# 整体结构

## 文档体系

`touch/spec/` 目录下维护 Touch 系统的全部规范文档，按职责划分：

| 文档 | 作用 | 面向 |
|------|------|------|
| `spec-touch.md` | 整体结构说明（本文档） | 通用 |
| `spec-design.md` | 三层架构设计文档（含 frpc 远程配置子章节、安全设计章节） | 设计 |
| `spec-api.md` | 系统对外接口文档（REST API：Manager ↔ Web UI、Edge 首次注册） | 对外 |
| `spec-api-websocket-jsonrpc.md` | 系统内部接口文档（Edge ↔ Ingress ↔ Manager，come.1 JSON-RPC 2.0 协议） | 对内 |
| `spec-config.md` | 各组件配置情况说明 | 对外 |
| `spec-devel.md` | 开发指导 | 对内 |
| `spec-roadmap.md` | 开发路线图 | 规划 |
| `spec-test-cases.md` | 统一测试用例文档（REST / WebSocket / 边缘生命周期 / frpc 远程配置 / 压力） | 测试 |

```
touch/
├── README.md
├── spec/
│   ├── spec-touch.md                  # 整体结构说明（本文档）
│   ├── spec-design.md                 # 三层架构设计文档（含 frpc 远程配置、安全设计章节）
│   ├── spec-api.md                    # 系统对外接口文档（REST API）
│   ├── spec-api-websocket-jsonrpc.md  # 系统内部接口文档（WebSocket JSON-RPC 2.0）
│   ├── spec-config.md                 # 各组件配置情况说明（对外）
│   ├── spec-devel.md                  # 开发指导（对内）
│   ├── spec-roadmap.md                # 开发路线图
│   └── spec-test-cases.md             # 统一测试用例文档
│
└── src/                      # 源代码目录
    ├── touch_edge/           # Edge (C++，嵌入式，WebSocket 客户端)
    ├── touch_ingress/        # Ingress Service (C++，无状态 WebSocket 接入层)
    ├── touch_manager/        # Management Service (Python/Flask + SQLite，有状态 + Web UI)
    ├── devWsRegister/        # WebSocket 通信层
    ├── funcRegister/         # 功能注册模块
    └── Makefile
```

> 说明：测试用例统一维护在 `spec-test-cases.md`，设计文档不再展开整套用例，仅保留引用。
