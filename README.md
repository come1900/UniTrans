# UniTrans

UniTrans 是一个面向多项目共享共创的 C++ 通信与服务框架平台。它以一套统一的源码树为底座，通过 `build` 目录下的逐项目 Makefile 与编译宏，产出面向不同业务场景的独立可执行程序；各项目共享通用基础库、通用模块与公共开发规范，避免重复造轮子，从而"共享一套代码、共同迭代、分别交付各业务项目"。

## 平台定位

* **一套源码，多项目交付**：`src` 为所有项目共用的源码树，`build` 下每个 `<序号>.Makefile.<ProjectName>` 从这份源码编出对应项目的可执行文件。
* **编译期功能裁剪**：通过编译宏（如 `-D_FUNC_TouchEdge`、`-D_FUNC_TouchIngress`）按项目启用所需功能模块，实现"同一框架、按需组合"。
* **通用基础库**：`libs/come.1` 提供跨项目复用的基础能力（消息编码、通信、公共工具等），是共享共创的核心资产。
* **统一构建与配置**：`build` 下统一提供构建脚本与 `cfg-<project>.cfg` 运行配置模板，各项目按统一规范组织。

## 目录结构

```
UniTrans/
├── build/          # 多项目构建入口：<序号>.Makefile.<ProjectName> + 构建脚本 + cfg 模板
├── src/            # 各项目共享的源码树（Main / Function / Configs / 通用模块）
├── libs/           # 跨项目复用的基础库（come.1 等）
├── man/            # 配套服务（Python 侧，如 touch_manager）
├── test/           # 测试与测试文档（按业务组织，如 touch）
├── doc/            # 文档目录
└── release/        # 发布信息
```

`build` 目录是观察项目组成的关键入口：当前包含 29 份项目 Makefile，覆盖短信（SMS.ASS）、网络推流/拉流（PuOverHttp / CuOverHttp / LiveStreamming / LiveRecord）、音视频交互（LiveDialog / LiveCast / InterVideo）、IoT（H2 / InterUtcs）、安全锁（gminiLock）、服务型组件（NvpRegisterServer / KeyManagementService / MinerMtService）、Pandora 矩阵与节点（Pandora.Matrix / Pandora.Node）以及 Touch 触控接入（Touch.Edge / Touch.Ingress）等多个领域项目。各子项目既可独立构建，也遵循同一套源码共享规范。本文档以下以 Touch 为例说明典型子项目的组织方式。

## Touch 子项目

Touch 是 UniTrans 下的一个典型接入业务，采用三层架构：边缘设备（Edge）经接入层（Ingress）接入统一管理端（Manager）。

| 组件 | 类型 | 说明 |
|------|------|------|
| touchEdge | C++ | 边缘端，WebSocket 客户端，周期心跳上报 |
| touchIngress | C++ | 无状态接入层，WebSocket 服务端，负责签名校验、令牌签发、消息转发 |
| touch_manager | Python | 管理服务（Flask + SQLite），有状态，提供 REST API 与 Web UI |

三者的源码分属 `src/Function/Touch/Edge`、`src/Function/Touch/Ingress` 与 `man/touch_manager`，构建入口分别为 `build/93.Makefile.Touch.Edge`、`build/93.Makefile.Touch.Ingress`，运行配置模板为 `build/cfg-touchEdge.cfg`、`build/cfg-touchIngress.cfg`。

Touch 的设计、接口、开发指导与测试用例统一维护在 `test/touch/spec/` 下的系列规范文档中：

* `spec-touch.md`：整体结构说明
* `spec-design.md`：三层架构设计（含 frpc 远程配置、安全设计）
* `spec-api.md`：对外 REST 接口
* `spec-api-websocket-jsonrpc.md`：内部 WebSocket JSON-RPC 2.0 接口
* `spec-config.md`：各组件配置说明
* `spec-devel.md`：开发指导
* `spec-test-cases.md`：统一测试用例库

## 构建

项目从 `build` 目录按 Makefile 统一构建。以 Touch 为例：

```
cd build
make -f 93.Makefile.Touch.Edge       # 构建 touchEdge
make -f 93.Makefile.Touch.Ingress    # 构建 touchIngress
```

各子项目 Makefile 均依赖共享的 `src` 源码树与 `libs/come.1` 基础库，构建前先构建/安装 `libs/come.1`。

## 文档约定

主文档见仓库根 `README.md`；业务文档按"顶层总述 + 分层设计 + 接口 + 开发指导 + 测试用例"的组织方式维护在对应业务目录（如 `test/touch/spec`）下，保证文档自洽、可独立阅读。
