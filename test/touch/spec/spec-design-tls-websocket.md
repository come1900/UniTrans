# Touch WebSocket 链路 TLS 改造设计

## 目标与原则

Touch 三层（touchEdge ↔ touchIngress ↔ touch_manager）之间的 WebSocket 链路当前为明文，需引入 TLS 加密。核心约束有两条：

* 加密能力由 **libezsocket**（WS 库，代码在 `/home/10312200/svn/come1900/libezsocket/src`）以**编译宏**方式开关，保证库在不需要 TLS 时保持小巧（不引入 OpenSSL 依赖、不占代码与运行时开销）。
* TLS 实现使用**源码编译的 OpenSSL 静态库**（OpenSSL 3.4.1，安装前缀 `$(HOME)/libs/openssl-linux`，非系统动态库），不做自研算法；构建方式与 API 用法参考 `3rd-openssl/tutorials/ssl-echo`（该示例含 Makefile 与阻塞式 server/client 代码，可与本设计的非阻塞插入点对照）。

本设计只描述方案，不改动任何实现代码。

## 范围界定

TLS 改造落在 **WebSocket 传输层**，即 edge↔ingress 这条点对点加密链路。**不改动原有明文 WS 实现**，新增并行的 wss（WS over TLS）组件：

* `ez_wss-server-native.c/.h`（服务器，Ingress 侧，wss 版）
* `ez_wss-client-native.c/.h`（客户端，Edge 侧，wss 版）

原有 `ez_wsserver-native.c/.h`、`ez_wsclient-native.c/.h` 保持纯明文不动，继续作为轻量/非 TLS 路径。底层 `ez_socket.c`、`ez_tcp.c` 是普通 TCP socket 封装，TLS 层构建于 wss 组件内部的 WS 层（不复用、不修改原 WS 主循环）；WS 帧解析复用共享组件 `http_parser` + `ez_websocket_parser`（回调式，无环状态）。调研确认当前 libezsocket 内**无任何 SSL/TLS 代码**，TLS 为全新插入。

**并行实现取舍**：代价是帧构造器、握手响应生成、发送队列与 epoll 主循环有一份轻量复制；换来原实现零改动、TLS 非阻塞握手状态机与 WANT_READ/WRITE 逻辑可独立演进，无需在原文件里散点包裹 `ssl ? SSL_read : recv`。

## TLS 宏开关设计（EZ_WS_ENABLE_OPENSSLTLS）

宏归属 **WS 层**，定义于 `ez_websocket.h` 顶部（ws 共用头，Server/Client 共用，不改动 `ez_config.h` 等底层层级头文件）。**默认启用**：默认即加密链路、依赖 OpenSSL；把宏定义行注释掉即回到目前纯明文，不引入 OpenSSL 依赖。

```c
/* ez_websocket.h 顶部
 * 默认启用 TLS（加密+内置信任锚互认，依赖 OpenSSL）。
 * 注释本行即回到纯明文：编译器不进 SSL 分支，库不依赖 OpenSSL。 */
#define EZ_WS_ENABLE_OPENSSLTLS 1
```

* 与库内 `EZ_WS_*_ENABLE_STATS` 同类功能开关，但**不走**其 `#ifndef X` + `#define X 1` 的回退写法：TLS 采用**默认开、注释即关**，原因是 Touch 把加密作为默认形态，免去构建期 `-D` 参数，且源码级一眼可见是否开启。
* 所有 OpenSSL 头文件 `#include <openssl/ssl.h>` 与 SSL 相关代码均以 `#if defined(EZ_WS_ENABLE_OPENSSLTLS) && (EZ_WS_ENABLE_OPENSSLTLS == 1)` 包裹；宏被注释时未定义 → 条件不满足 → 零 TLS 代码、零 OpenSSL 依赖。
* 平台宏沿用现有约定（`ez_config_linux.h` 等按 `LINUX/WIN32/...` 选入），`EZ_WS_ENABLE_OPENSSLTLS` 是独立于平台的功能开关，定义于 WS 头，不受平台选入影响。
* 构建期不再以 `-D` 控制取值（宏在头文件内已定）。wss 两个源文件（`ez_wss-server-native.c`、`ez_wss-client-native.c`）以 `#if defined(EZ_WS_ENABLE_OPENSSLTLS) && (EZ_WS_ENABLE_OPENSSLTLS == 1)` **整文件包裹**：宏被注释 → 编译为空对象 → 不引入任何 OpenSSL 符号与依赖；原 `ez_wsserver-native`/`ez_wsclient-native` 永远不依赖该宏。

## 配置结构扩展

在不改动原有 WS 句柄与配置结构的前提下，wss 组件在**新头文件**中定义带 TLS 字段的公开配置结构，便于 Touch 应用层直接设置。

**两级开关关系**：`EZ_WS_ENABLE_OPENSSLTLS` 是编译期能力开关（默认开，决定进不进 SSL 分支/链不链 OpenSSL）；`tls_enable` 是**每实例/每条连接**的运行期开关——仅当库带 TLS 能力、本端 `tls_enable=1` 且对端也启用时，该条连接才真正协商 TLS；`tls_enable=0` 的连接即使库带 TLS 能力也走纯明文。**`tls_enable` 默认 1（开箱即加密）**：Touch 链路默认强制加密，未升级 TLS 的明文对端将无法接入；需放行旧对端时由应用层显式置 0。注意不对称：服务器侧 `tls_enable` 实为**监听级**（一个 listen socket 的新连接继承同一取值，同一端口无法 ws/wss 混跑，edge 与 manager 共用 `/come` 时整体生效），客户端侧才是真正每连接字段。

* `struct ez_wss_server_config`（`ez_wss-server-native.h`，镜像原 `ez_ws_server_config` 全部字段 + 新增如下 TLS 字段）：
  * `int tls_enable;`（0/1，监听级、新连接继承；默认 1，未配置证书时走内置 CA 互认兜底）
  * `const char *tls_cert_path;`（证书链，可为空）
  * `const char *tls_key_path;`（私钥，可为空）
  * `const char *tls_ca_path;`（可选，双向认证 client CA，可为空）
* `struct ez_wss_client_config`（`ez_wss-client-native.h`，镜像原 `ez_ws_client_config` + 新增如下）：
  * `int tls_enable;`（0/1，每实例/每条连接；默认 1）
  * `int tls_verify_peer;`（默认 1，校验服务端证书；跨信任域可配 0）
  * `const char *tls_ca_path;`（校验服务端所用 CA，可为空表示用系统 CA）

每个连接的 `SSL *` 指针存入 wss 连接结构：服务器存入 wss 版 `struct client_connection`（`ez_wss-server-native.c`），客户端存入 wss 版客户端句柄（`ez_wss-client-native.c`），紧邻 `sockfd`，随每条连接创建/销毁并携带各自的 tls 状态，保证「每条连接独立持有 `tls_enable` 与 TLS 会话」。

## 证书处理（frp 风格：内置信任锚兜底）

对齐 frp 的做法：证书可配置，也可完全不配置；未配置时自动使用**库内置的信任锚**，保证加密+互认能力开箱即用。缺省档位为「内置 CA 互认」。

* **可配置（正式证书）**：用户指定 `tls_cert_path` + `tls_key_path`（服务器提供正式证书），或指定 `tls_ca_path`（客户端校验指定 CA）时，TLS 使用指定证书，走标准 OpenSSL 加载流程（`SSL_CTX_use_certificate_chain_file` / `SSL_CTX_use_PrivateKey_file` / `SSL_CTX_load_verify_locations`）。
* **不配置（内置 CA 互认，默认档）**：库内置一把 CA 证书+私钥（编译期内嵌、随库分发）。当 `tls_enable=1` 且服务器证书/私钥路径均为空时，服务端 `SSL_CTX` 用该 CA 实时签发一张叶子证书；客户端 `tls_verify_peer=1` 并 trust 同一把内置 CA。两端天然互认，**零配置即实现加密+身份互认**，同信任域内可防主动 MITM，对齐 frp 两端共用同一内置证书/密钥的互认做法。
* **降级档（仅加密、不认证）**：跨信任域或第三方临时连入、无法 trust 内置 CA 时，客户端显式设 `tls_verify_peer=0`，走「纯加密不认证」；**不作为默认**，文档需明确其定位。

内置信任锚的生成与缓存（实现定稿）：

* 私钥生成用 `EVP_PKEY_keygen`，建议 **EC P-256**（生成快、握手快、TLS 1.2/1.3 通用）。
* 证书用 `X509_*` 标准 API 全内存构造：版本 3、随机 serial、`X509_gmtime_adj` 设定固定有效期、subject=issuer、`basicConstraints` 按用途设 `CA:TRUE`（作 CA 互认）或 `CA:FALSE`（叶子）、可填 SAN 备将来按 IP/域名校验。
* **CA 固定内嵌**（编译期生成一次、以 PEM 常量随库分发，`pthread_once` 加载，保证两次端到端互认一致）；仅**叶子证书**在服务器建 `SSL_CTX` 时运行时签发一次并缓存于 `SSL_CTX` 内，不落盘实体文件，不改变应用加载路径。
* 内置 CA 公钥证书需提供**导出到调用方缓冲区**的能力（如 `ez_ws_export_builtin_ca_pem()`，C 库侧不落盘），供 Python manager 等非 C 消费者落地为 CA 文件以信任 ingress；内置 CA 私钥永不导出。

## wss 并行实现（ez_wss-server-native.c / ez_wss-client-native.c）

两个新源文件各自实现一套与明文版平行的 WS 栈：复用 `http_parser` + `ez_websocket_parser`（回调式）做 HTTP/WS 帧解析，自持 epoll 主循环、发送队列、心跳与 TLS。**TLS 握手在任何 HTTP/WS 字节进入解析器之前必须完成**。

服务器（`ez_wss-server-native.c`）：

* **句柄级**（`ez_wss_server_handle_create`）：按 `config.tls_enable` 创建全局 `SSL_CTX`（懒初始化，库首次用时执行一次 OpenSSL 初始化）——配置 `tls_cert_path/tls_key_path` 走正式证书；两者皆空走内置 CA 互认（运行时签发叶子证书）；`SSL_CTX` 缓存于句柄，所有连接共享。
* **连接级**（accept 后、注册 epoll 前）：每条连接 `SSL_new(server->ssl_ctx)` + `SSL_set_fd`，进入 TLS 握手状态；epoll 在握手阶段以 **level-triggered + 显式掩码**注册（`WANT_READ → EPOLLIN`、`WANT_WRITE → EPOLLIN|EPOLLOUT`，规避 EPOLLET 下 EPOLLOUT 无跃迁不触发的握手死锁，见「实现结论」），**握手完成后切回 `EPOLLIN|EPOLLOUT|EPOLLET` 数据面、才进入 HTTP/WS 握手**。
* **数据面**：读写统一走 `SSL_read`/`SSL_write`，WANT_READ/WRITE 返回并驱动事件切换；`SSL_pending` 处理一次事件里 SSL 内部仍有多余应用数据的情况。
* **关闭清理**：`close` 前先 `SSL_shutdown`（尽力）再 `SSL_free`；半关闭/重连时 SSL 状态随连接重建。

客户端（`ez_wss-client-native.c`）：

* **连接级**：TCP connect 成功后、发送 WS 握手请求前，按 `config.tls_enable` 建 `SSL_CTX`（客户端 CTX 仅做校验与初始握手）+ `SSL_new` + `SSL_set_fd` + `SSL_connect`（非阻塞拆为事件驱动）；`tls_verify_peer=1` 时 `SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, ...)` 并用 `tls_ca_path` 或内置 CA 校验，`0` 时 `SSL_VERIFY_NONE`。
* **数据面**：与服务器同模式（SSL_read/SSL_write + WANT 处理）。
* **重置/清理**：`ez_wss_client_handle_reset`/cleanup 释放 SSL 资源，重连时整链重建。

公共 TLS 工具：

* 内置 CA 公钥证书**导出**函数：**`ez_ws_export_builtin_ca_pem(char *buf, size_t len)`**，返回内置 CA 的 PEM（私钥永不出），供 manager 等非 C 消费者落地为 CA 文件。
* 内置 CA/叶子证书运行时生成细节与参数见「证书处理」。

## Touch 集成点

* **Ingress（服务器，发力点）**：`DevWsRegisterSvr.cpp` `Start(port, protocol, path_prefix)`（L31-76）由 `ez_ws_server_handle_create` 换成 `ez_wss_server_handle_create`，填充 `struct ez_wss_server_config`（镜像原 `ez_ws_server_config` 字段 + 新增 `tls_enable/cert/key/ca`，未配证书默认走内置 CA 互认）；`FunRegisterSvr.cpp` `CFunRegisterSvr::Start`（L48）为最上层入口，需向上（应用配置层）透传 TLS 参数。`DevWsRegisterSvr.h` `Start` 签名扩展（可选，缺省即内置 CA 兜底）。
* **Edge（客户端）**：`DevWsRegisterCli.cpp` `Start(...)`（L30-85）换成 `ez_wss_client_*` API，填充 `struct ez_wss_client_config`（镜像原 `ez_ws_client_config` + 新增 `tls_enable/verify_peer/ca`，默认 `verify_peer=1` + 内置 CA）；`FunRegisterCli.cpp` `CFunRegisterCli::Start`（L75）为最上层入口，同样透传。`DevWsRegisterCli.h` `Start` 签名扩展。

## 编译与链接

* OpenSSL 采用**源码编译的静态库**（OpenSSL 3.4.1，前缀 `$(HOME)/libs/openssl-linux`，配套 zlib 静态库 `$(HOME)/libs/zlib-linux`），不以 `-lssl -lcrypto` 动态链系统库；include/链接行参考 `3rd-openssl/tutorials/ssl-echo` 的 Makefile：

```makefile
OPENSSL_INSTALL_PREFIX = $(HOME)/libs/openssl-linux
CFLAGS += -I$(OPENSSL_INSTALL_PREFIX)/include -I$(HOME)/libs/zlib-linux/include
LIBS = $(OPENSSL_INSTALL_PREFIX)/lib64/libssl.a \
       $(OPENSSL_INSTALL_PREFIX)/lib64/libcrypto.a \
       $(HOME)/libs/zlib-linux/lib/libz.a -lpthread -ldl
```

* `EZ_WS_ENABLE_OPENSSLTLS` 默认开，Touch 项目（如 `build/93.Makefile.Touch.Edge`、`build/93.Makefile.Touch.Ingress`）**无需 `-D` 定义宏**，仅在 Makefile 追加上述 include/静态 `LIBS` 即可；静态链接自检 `ldd <bin> | grep -E "ssl|crypto"` 应无输出。
* `libezsocket` 的 `Makefile.SrcLists` **新增** `ez_wss-server-native.o`、`ez_wss-client-native.o`（源码已被宏整文件包裹，宏注释 → 空对象）；原 `ez_wsserver-native.o`/`ez_wsclient-native.o` 不变。TLS 产物 = 含 wss 对象、链接上述 OpenSSL/zlib 静态库；非 TLS 产物 = 不含 wss 对象，保持轻量、零 OpenSSL 依赖。
* 既有的非 TLS 项目因宏默认开，需在 `ez_websocket.h` 中注释该宏并选用非 TLS 产物，行为与链接体积不变；这是相对以往唯一需要动的一处，换来默认开箱即加密。

## 实现参考（3rd-openssl ssl-echo）

示例位于 `3rd-openssl/tutorials/ssl-echo`（OpenSSL 3.4.1 静态库；编译/链接见上文「编译与链接」），其 API 用法与本设计对应关系：

| 环节 | 示例 API（ssl-echo） | 本设计落点 |
|------|----------------------|------------|
| 库初始化 | `OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS \| OPENSSL_INIT_LOAD_CRYPTO_STRINGS)` | WS 层首次使用 TLS 时执行一次 |
| 上下文 | `SSL_CTX_new(TLS_server_method()/TLS_client_method())` + `SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)` | 服务器/客户端连接初始化 |
| 证书加载 | `SSL_CTX_use_certificate_file`/`SSL_CTX_use_PrivateKey_file` + `SSL_CTX_check_private_key` | 配置证书路径时使用（正式证书链走 `SSL_CTX_use_certificate_chain_file`） |
| 客户端校验 | `SSL_CTX_load_verify_locations(ctx, CA_FILE, NULL)` + `SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL)`，`ENABLE_CA_VERIFY` 宏开关 | 对应 `tls_verify_peer`；内置 CA 互认为默认档 |
| 握手/读写 | `SSL_accept`/`SSL_connect`/`SSL_read`/`SSL_write`（**阻塞式**） | 本设计按 epoll 非阻塞拆分：`SSL_ERROR_WANT_READ/WRITE` 驱动，见 wss 并行实现 |

注意：ssl-echo 是阻塞式演示（`SSL_set_fd` 后阻塞握手与读写），仅供构建链接、`SSL_CTX` 配置、证书加载与校验 API 参考；本设计的握手与数据面必须按非阻塞（epoll + WANT_READ/WRITE）实现，不能直接照搬其主循环。

## 风险与附注

* 非阻塞 TLS 常见坑是 `WANT_READ/WANT_WRITE` 与 EPOLL 事件的切换（读就绪但需写、写就绪但需读），需在 epoll 事件驱动循环中正确维护；含半关闭、重连场景下 `SSL` 状态需随连接重置。
* 内置信任锚的私钥随库公开，属「防君子不防小人」的信任边界：对纯内网链路（防被动嗅探与轻度主动注入）足够；面向公网/多租户或可能提取私钥的强对手时，应配置正式可信证书。降级档 `tls_verify_peer=0` 仅加密、不认证，不可作默认。
* 本设计聚焦 edge↔ingress；manager↔ingress 复用同一**信任锚**而非同一宏（`EZ_WS_ENABLE_OPENSSLTLS` 活在 C 库，manager 用不上）：
  * manager 为 Python WS 客户端（`man/touch_manager/ingress_client.py`，连接 `ws://host:port/come`、每 10s 重连、应用层 `manager.connect` 换 token）。libezsocket 需提供内置 CA 公钥证书导出（如 `ez_ws_export_builtin_ca_pem()`，导出到调用方缓冲区），manager 落地为 `ca.crt`，以 `ssl.create_default_context(cafile=ca.crt)` + `wss://` 连接并验证 ingress 证书 → 与 edge 同入内置 CA 信任域，`verify_peer` 不降级。
  * manager 配置统一为**单 URL**（`DEFAULT_INGRESS_URL`，如 `ws://ip:port` / `wss://ip:port`），scheme 即 TLS 开关，不再拆 HOST/PORT 与独立 tls 布尔项；`wss://` 时以 `INGRESS_CA_FILE` 指定内置 CA 文件校验（`INGRESS_SSL_VERIFY=0` 可跳过）。
  * **翻切节奏（实现后一起翻）**：`DEFAULT_INGRESS_URL` 默认暂保持 `ws://`（兼容明文）；待 ingress 侧 TLS 能力（`EZ_WS_ENABLE_OPENSSLTLS` 落地、libezsocket 内置 CA 导出可用）就绪后，与 ingress 侧**一起**翻为默认 `wss://`（并设 `INGRESS_CA_FILE`），避免 manager 先行 wss 而 ingress 未支持导致反复断连重试。
  * **服务器侧 `tls_enable` 是监听级**：ingress 单个监听同时服务 edge 与 manager，同端口不能 ws/wss 混跑；过渡期对旧对端放明文需另开明文监听端口（双端口灰度），或一次性直切 TLS。
  * manager 为管理面（持配置库、可踢设备），TLS 是纵深防御，身份认证由应用层 token 承担；默认内置 CA 互认即可，外向部署可升级为配置证书。

## 实现结论（原「待确认」项已定稿）

原「待确认」两项已在 libezsocket wss 实现与教程例 `wss-svr-native`/`wss-cli-native` 中定稿并实测：

### 1. 内置 CA / 叶子证书参数（定稿）

* **信任锚固定内嵌**：内置 CA 证书+私钥以 PEM 常量**编译期内嵌**于 `ez_wss-server-native.c`，`pthread_once` 加载成 `X509*`/`EVP_PKEY*` 缓存。这同时修正了上面「证书处理」中「运行时生成并缓存于 SSL_CTX」的措辞：CA 必须是**固定的**（若随进程/句柄随机生成，两端/多次运行的信任锚不一致，互认断链），落点为固定内嵌；真正在运行时生成的是**叶子证书**。
* **CA 证书**：密钥算法 **EC P-256**（`NID_X9_62_prime256v1`）；`X509` v3、`basicConstraints = critical, CA:TRUE`；subject=issuer=`CN=ez-wss-builtin-ca, O=EZLIBS Touch`；一次性生成后固化为 PEM 常量随库分发。
* **叶子证书**（服务器建 `SSL_CTX` 时每句柄运行签发一次，`wss_issue_leaf_cert`）：密钥 EC P-256、serial 随机（BIGNUM 随机数）、有效期 `X509_gmtime_adj` 1 年、subject 独立（`CN=ez-wss-server`）、issuer=CA subject、`basicConstraints = critical, CA:FALSE`、`X509_sign` 用 `EVP_sha256`；并把内置 CA 以 `SSL_CTX_add_extra_chain_cert` 附入发送链，客户端可回链内置 CA。
* **导出函数**（供 Python manager 等非 C 消费者落地为 CA 文件）：**`ez_ws_export_builtin_ca_pem(char *buf, size_t size)`**，只导出 CA 公钥证书 PEM，私钥永不导出。上面「公共 TLS 工具」中的 `ez_ws_builtin_ca_pem` 为早期笔误，以本函数名为准。
* **TLS 版本实测**：`SSL_CTX_set_min_proto_version(TLS1_2)`、上限默认，两端同为静态 OpenSSL 3.4.1 时实测协商 **TLS 1.3**；抓包可见 ClientHello/ServerHello 后全为 `17 03 03` 密文（TLS 1.3 下证书等握手消息亦加密、包内不可见，属预期）。

### 2. 零配置内置 CA 互认（实测）

* 教程例默认配置（svr `tls_enable=1` 不配证书 + cli `tls_verify_peer=1` trust 内置 CA）实测通过：
  * TLS 1.3 握手 → HTTP/WS 握手 → 客户端 `=== Connection established ===` 与 `on_connected`/`on_receive` 等回调（服务器侧 `on_connected` 在 TLS 握手完成后才触发，与设计一致）；
  * 消息往返：服务端日志 `Received text from client #6: 'hello wss from client'` ↔ 客户端 `SEND[21]: …`；
  * 心跳 PING/PONG 正常；握手在默认 `connect_timeout_ms=1000` 内稳定完成；
  * 抓包确认为 TLS 密文（非明文 ws）；`ldd` 无 `libssl/libcrypto` 动态依赖（全静态内嵌）。
* **尚未专门实测**：断线重连 / 退避重连路径（重连宏沿用明文客户端，TLS 资源随连接整链重建）留待 Touch 接入联调覆盖。

### 3. 命中并修复的风险（原「风险与附注」首条）

* 实际踩中的坑是 **EPOLLET 握手死锁**：非阻塞 `SSL_connect/SSL_accept` 返回 WANT_WRITE 后，socket 已写就绪但**无状态跃迁**，EPOLLET 下 EPOLLOUT 不再触发 → 握手卡死，直至 `connect_timeout_ms`（默认 1s）超时重置。与「风险与附注」预判一致。
* **修复结论**：握手阶段改用 **level-triggered + 显式事件掩码**——`WANT_READ → MOD EPOLLIN`（仅）、`WANT_WRITE → MOD EPOLLIN|EPOLLOUT`；**握手完成后** `MOD EPOLLIN|EPOLLOUT|EPOLLET` 切回数据面。服务端 `wss_set_handshake_events`/`do_tls_handshake`、客户端 `wss_client_set_handshake_events`/`wss_client_tls_handshake` 均按此模式；客户端 `ez_wss_connect` 在 `tls_enable=1` 时以电平事件注册、`tls_enable=0` 保持原 EPOLLET。

---

## Ingress 双实例接入（明文 ws + TLS wss 并存）

上文「风险与附注」指出：服务器侧 `tls_enable` 是**监听级**，同一端口无法 ws/wss 混跑，明文+加密并存需**双端口灰度**。本节把这一结论落地为 Ingress 的**接口化双实例**设计，并对齐"HTTP 与 HTTPS 并存"的思路：明文 ws 与 TLS wss 是 Ingress 上两个并列的接入服务。

### 设计目标

* Ingress 同时监听两个端口：**明文 ws（端口 54321）+ TLS wss（端口 54443，默认双开）**，各自独立事件循环与线程，互不拖累。
* 灰度平滑：旧对端（未升级 TLS 的 edge / manager）走明文端口，新对端走 wss 端口，无一次性直切风险。
* 为将来接入更多协议（如 MQTT 接入 MQTT edge）预留同一抽象接缝：每个"接入服务"= 一个传输实例。

### 传输抽象：`CWsTransportSvc`

在 Ingress 接入层（`DevWsRegisterSvr` 之上）引入抽象基类，每个实例封装"一个监听端口 + 一条事件循环 + 一组连接 + client_id 合并"：

```
CWsTransportSvc（抽象）
 ├── CWsPlainTransport   —— 封装 ez_ws_server_*（明文，端口 54321）
 └── CWssTransport       —— 封装 ez_wss_server_*（TLS，端口 54443）
```

* **接口**（每个实例实现）：`Start()/Stop()/IsReady()/GetClientCount()`、`SendText(gid, ...)/SendBinary(gid, ...)/CloseClient(gid)`，回调 `on_receive(gid, data, len, is_binary)`、`on_connected(gid, ip, port)`、`on_disconnected(gid, code)`。
* **每实例一线程**：各自 `service_exec` 事件循环，明文与 wss 的 epoll 互不阻塞。
* **协议细节收敛到各自实现内部**：抽象只关心"接入服务"语义（连接生命周期 + 收发字节流 + 上抛/下发），不暴露 WS 帧或（未来）MQTT 报文细节。明文 ws 与 wss 共享同一 WS 帧协议、仅加密变体不同；未来 MQTT 是另一个实现，但上抛给业务层的仍是统一的 come.1 载荷。

### client_id 合并到单一 id 空间

明文与 wss 两个 `libezsocket` 句柄各自从 1 开始编号 `client_id`，会冲突。解决：每个实例持 `id_base`，上抛给业务层的**全局 id = id_base + 实例内原生 id**；下行按全局 id 路由回正确实例（`ToLocalId(gid) = gid - id_base`，超出范围返回 -1 拒绝）。`CFunRegisterSvr` 只看到全局唯一 id，`m_manager_client_id`/各 handler 无需感知传输来自明文还是 wss。

* 明文实例 `id_base = 0`；wss 实例 `id_base = 1_000_000`（预留充足增长空间）。
* 工厂 `WsTransportCreate(cfg)`：按 `cfg.tls_enable` 选明文或 TLS 实现。

### 装配与配置

* `CDevWsRegisterSvr` 从"单句柄"改为持有**两个 `CWsTransportSvc*`**（明文 + wss），两个实例的回调汇聚到同一 `m_SigNotify`，业务层零改动。
* 配置层（`ConfigTouchIngress` / `cfg-touchIngress.cfg`）新增：`WssPort=54443`、`WssEnable=1`（默认双开）、`TlsCertPath/KeyPath/CaPath`（默认空 → 内置 CA 互认）。
* 构建：`test/touch/touch_ingress/Makefile` 追加 OpenSSL include + 静态链接（见「编译与链接」）。

#### 实现状态（2026-09-30，A 双实例已落地并实测）

* **`WsTransport.h/.cpp`**：`CWsTransportSvc` 抽象 + `CWsPlainTransport`/`CWssTransport` 实现，工厂 `WsTransportCreate` 按 `tls_enable` 选实现；每实例 `Start()` 内部 `pthread_create` 独立线程跑 `service_exec`（明文 50ms、wss 50ms 轮询）。回调上抛全局 id（`ToGlobalId`）。
* **`DevWsRegisterSvr`**：改为持有 `m_plain`/`m_wss` 两个 `CWsTransportSvc*`；`Start(port,protocol,path)` 建明文 + 可选 wss；`SetWssConfig(enable,port,cert,key,ca)` 在 Start 前注入 wss 配置；`RouteByGlobalId` 按 id_base 段路由 `SendText/CloseClient`（明文 id_base=0，wss id_base=1_000_000，广播归明文）；信号槽 `m_SigNotify` 不变，业务层 `CFunRegisterSvr` 零改动。两个实例各自独立线程，`DevWsRegisterSvr` 自身不再跑事件循环。
* **`CFunRegisterSvr::Start`** 扩展签名：新增 `wss_enable/wss_port/tls_cert_path/tls_key_path/tls_ca_path`（默认关闭），透传给 `g_DevWsRegisterSvr`。
* **`Main.cpp`（test/touch/touch_ingress 本地入口）**：新增 `-w/--wss-enable`、`-W/--wss-port`（默认 54443）、`-c/--tls-cert`、`-k/--tls-key`、`-a/--tls-ca`。
* **实测**：`touch_ingress-linux --port 54321 --wss-enable --wss-port 54443` 双端口同时 LISTEN（54321 明文 + 54443 wss，同一进程）、进程稳定驻留；wss 客户端（`t-wss-cli-bench`，协议 come.0）对 54443 完成 **TCP + TLS 握手**，仅因子协议 come.0≠come.1 在 WS 升级阶段被服务端关闭（协议不匹配属预期，TLS 链路本身已打通）；`ldd` 无动态 libssl/libcrypto（全静态内嵌）。
* **构建**：`Makefile` 追加 `WsTransport.o`、OpenSSL 静态库（`libssl.a`/`libcrypto.a`/`zlib.a`）与 `-ldl`（OpenSSL 静态链接必需）。`make clean && make` 通过。

> 说明：edge 侧（`touchEdge` / `DevWsRegisterCli`）与 manager 侧（`touch_manager` 单 URL + wss://）的 wss 翻切**已完成并实测**，见下节「edge / manager 侧 wss 翻切」。

---

## edge / manager 侧 wss 翻切（2026-09-30 落地并实测）

### Edge 侧（客户端，连 ingress wss 端口）

* **`DevWsRegisterCli.h/.cpp`**：`Start(server_addr, port, url_path="/come", protocol="come.1", reconnect_max_retries=0, tls_enable=1, tls_verify_peer=1, tls_ca_path=NULL)`。按 `tls_enable` 分支选 `ez_wss_client_*`（TLS）或 `ez_ws_client_*`（明文）；`tls_verify_peer=1` 且 `tls_ca_path` 空时 trust 内置 CA（与 ingress 同信任域互认）。
* **`FunRegisterCli.h/.cpp`**：`Start(...)` 透传 `tls_enable/tls_verify_peer/tls_ca_path` 到 `g_DevWsRegisterCli`。
* **正式配置层**：`ConfigTouchEdge` 新增 `WssEnable`（默认 1）、`WssPort`（默认 54443）、`TlsVerifyPeer`（默认 1）、`TlsCaPath`（默认空→内置 CA）；`AgentTouchEdge::SetConfig()` 填充并 `Start()` 按 `iWssEnable` 选 wss 端口 + 透传 TLS 参数；`cfg-touchEdge.cfg` 同步新增字段。
* **本地入口**：`test/touch/touch_edge/Main.cpp` 新增 `-w/--wss`、`-n/--no-verify`、`-a/--tls-ca`；`Makefile` 追加 OpenSSL 静态库 + `-ldl`。
* **实测**：`touch_edge-linux --host 127.0.0.1 --port 54443 --edge-id <id> --edge-key <key> -w` 对 wss ingress 完成 **TCP + TLS 握手 + wss WS 升级**（日志 `WebSocket connected to ingress`），随后发 `edge.online`（无 manager 时仅被业务层拒绝，TLS 链路已打通）。

### Manager 侧（Python websockets 客户端）

* **`man/touch_manager/config.py`**：`DEFAULT_INGRESS_URL` 默认翻为 `wss://<host>:54443`（scheme 即 TLS 开关）；`INGRESS_CA_FILE`（内置 CA 导出文件）与 `INGRESS_SSL_VERIFY`（默认 1）沿用。
* **`man/touch_manager/ingress_client.py` `_client_ssl_context`**：wss 时用 `ssl.create_default_context(cafile=INGRESS_CA_FILE)` 做 CA 校验（防伪造/MITM），并 `check_hostname=False` —— 内置 CA 叶子证书 `CN=ez-wss-server` 无 SAN，IP 随部署变化，与 edge 侧「内置 CA 互认」口径一致，不做主机名绑定；`INGRESS_SSL_VERIFY=0` 时降为仅加密。
* **CA 导出工具**：新增 `test/touch/touch_ca_export/`（C 程序，链接 libezsocket + OpenSSL），调 `ez_ws_export_builtin_ca_pem()` 把内置 CA 公钥证书导出为 PEM 文件（如 `ca.crt`，私钥永不出）。manager 以 `INGRESS_CA_FILE=ca.crt` 信任 ingress。因内置 CA 不在系统 CA store，`INGRESS_SSL_VERIFY=1` 时必须用该文件（或系统 CA），否则校验失败。
* **实测**：manager 以 `INGRESS_CA_FILE=/tmp/touch_ca.crt` 连 `wss://127.0.0.1:54443/come`，日志 `Connected ... with subprotocol 'come.1'` + `Received Manager segment touch_token`（应用层 `manager.connect` 走通），TLS + wss 全链路 OK。

### 翻切节奏与回退

* 默认已一起翻为 `wss://`（edge 默认 `WssEnable=1`、manager 默认 `wss://`），两端同入内置 CA 信任域。
* **回退/明文兼容**：edge 设 `WssEnable=0`（或 `-w` 不传）连 ingress 明文端口；manager 设 `DEFAULT_INGRESS_URL=ws://host:54321` 回明文。`INGRESS_SSL_VERIFY=0` 可在无 CA 文件时降为仅加密（不作为默认）。

---

## 设备身份索引（edge_id 反查）

### 动机

`CFunRegisterSvr` 的在线表 `m_online_edges` 以 **client_id** 为 key（收消息/断连等场景天然拿到 client_id，直查快）。但业务上常**只拿到 edge_id**（config.update/query、notify online/offline、kick、manager 确认），现实现为**对全表 `for(...) if(edge_id==...)` 线性扫描 O(n)**。在线 edge 上千量级时，O(n) 反查成为真实瓶颈。

### 结论：保留 client_id 主表 + 新增 edge_id 辅助反查索引

不把主表改成 edge_id（那会让"收→转发"热路径多一次字符串查找，且 manager/暂态连接无 edge_id 需特判，逻辑更复杂）。改为：

* **主表不变**：`m_online_edges`（`client_id → EdgeDeviceInfo`），热路径（收消息）仍 O(1) 直查。
* **新增辅助索引**：`m_edge_id_to_client`（`std::unordered_map<std::string,int>`，edge_id → client_id），O(1) 平均。
* 把 5+ 处 O(n) 线性扫描替换为 `m_edge_id_to_client.find(edge_id)`。

### 索引同步规则（唯一需要维护的地方）

| 事件 | 主表 `m_online_edges` | 辅助索引 `m_edge_id_to_client` |
|------|----------------------|-------------------------------|
| edge 上线（edge.online 成功，edge_id 确定） | `[client_id] = info` | `[edge_id] = client_id` |
| edge 断连 | `erase(client_id)` | `erase(对应 edge_id)` |
| 查 edge（config/notify/kick） | —（按索引得 client_id 后再查主表） | `find(edge_id)` |

### 映射关系一句话总结

> **client_id 是"这条连接在哪"（传输句柄），edge_id 是"这台设备是谁"（稳定身份）。**
> 一台 edge 以后可用 ws 或 MQTT 接入，连接（client_id）会变，但设备身份（edge_id）不变。
> 业务层以 edge_id 为稳定索引管理设备归属，client_id 仅用于把消息投递给当前承载该设备的连接。

这也正是"将来接入 MQTT edge"的架构准备：跨协议（ws/wss/mqtt）的同一台设备，统一按 edge_id 对齐。
