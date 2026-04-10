#整体结构

touch/
├── README.md
├── spec/
│   ├── spec-design.md        # 三层架构设计文档
│   ├── spec-api.md           # 接口契约文档
│   ├── spec-roadmap.md       # 开发路线图
│   └── spec-touch.md         # 整体结构说明（本文档）
│
└── src/                      # 源代码目录
    ├── touch_edge/           # Edge (C++, 嵌入式)
    │   └── Main.cpp
    ├── touch_ingress/        # Ingress Service (C++, 无状态 WebSocket 接入层)
    │   └── Main.cpp
    ├── touch_manager/        # Management Service (待实现, Go, 有状态 + Web UI)
    ├── devWsRegister/        # WebSocket 通信层
    ├── funcRegister/         # 功能注册模块
    └── Makefile