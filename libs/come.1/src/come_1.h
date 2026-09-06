/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * come_1.h - come protocol message definitions
 *
 * Copyright (C) 2025 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: come_1.h $
 *
 *  Explain:
 *     Define message classes for come protocol, including device online,
 *     device report, server command and their acknowledgment messages.
 *     Support OAuth2-style authentication with access_token.
 *
 *  Update:
 *     2024-01-01 00:00:00 Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef COME_1_H
#define COME_1_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

// 基础消息类（可选，用于统一接口）
class MsgCome {
public:
    std::string access_token;   // 访问令牌（用于身份验证）

    MsgCome() : access_token("") {}
    virtual ~MsgCome() = default;
};

class AckMsgCome : public MsgCome{
public:
    int32_t code;           // 返回码
    std::string msg;        // 返回消息

    AckMsgCome() : code(0), msg("") {}
    AckMsgCome(int32_t _code, const std::string& _msg = "") 
        : code(_code), msg(_msg) {}
    virtual ~AckMsgCome() = default;
};

// ==============================
// Method 常量定义（用于 JSON-RPC 2.0）
// 命名规则：<发起方>.<操作> 或 <发起方>.<目标>.<操作>
// 网元角色：edge（边缘设备）、ingress（接入网关）、manager（管理中心）
// ==============================

// Edge 发起的消息（Edge → Manager/Ingress）
#define COME_METHOD_EDGE_ONLINE         "edge.online"       // Edge 上线请求
#define COME_METHOD_EDGE_HEARTBEAT      "edge.heartbeat"    // Edge 心跳通知
#define COME_METHOD_EDGE_OFFLINE        "edge.offline"      // Edge 下线通知

// Manager 发起的消息（Manager → Ingress）
#define COME_METHOD_MANAGER_CONNECT         "manager.connect"           // Manager 连接通知
#define COME_METHOD_MANAGER_EDGE_LIST       "manager.edge.list"         // Manager 查询边缘列表
#define COME_METHOD_MANAGER_EDGE_OFFLINE_LIST "manager.edge.offline.list" // Manager 查询离线边缘列表
#define COME_METHOD_MANAGER_EDGE_KICK       "manager.edge.kick"         // Manager 踢边缘

// Ingress 发起的消息（Ingress → Manager）
#define COME_METHOD_INGRESS_EDGE_ONLINE     "ingress.edge.online"       // Ingress 通知边缘上线
#define COME_METHOD_INGRESS_EDGE_OFFLINE    "ingress.edge.offline"      // Ingress 通知边缘下线

// ==============================
// 边缘上线消息
// ==============================

class EdgeOnline : public MsgCome {
public:
    std::string id;          // 边缘唯一标识
    std::string key;         // 边缘认证密钥
    std::string type;        // 边缘类型
    std::string nonce;       // 随机数，用于OAuth2风格认证
    std::string token;       // 从 Manager 获取的认证 token（用于与 Ingress 认证）

    EdgeOnline() = default;
    EdgeOnline(const std::string& _id, const std::string& _key,
                 const std::string& _type = "", const std::string& _nonce = "",
                 const std::string& _token = "")
        : id(_id), key(_key), type(_type), nonce(_nonce), token(_token) {}
};

class AckEdgeOnline : public AckMsgCome {
public:
    bool success;               // 是否成功上线
    std::string access_token;   // 访问令牌（OAuth2风格认证）
    std::string token_type;     // 令牌类型（如 "Bearer"）
    int32_t expires_in;         // 令牌过期时间（秒）

    AckEdgeOnline() : AckMsgCome(), success(false), token_type("Bearer"), expires_in(0) {}
    AckEdgeOnline(int32_t _code, bool _success, const std::string& _msg = "",
                    const std::string& _access_token = "", const std::string& _token_type = "Bearer",
                    int32_t _expires_in = 0)
        : AckMsgCome(_code, _msg), success(_success), access_token(_access_token),
          token_type(_token_type), expires_in(_expires_in) {}
};

// ==============================
// 边缘上报消息（通用数据上报）
// ==============================

class EdgeReport : public MsgCome {
public:
    std::string id;                         // 设备ID（用于路由/验证）
    std::map<std::string, std::string> data; // 上报的键值对数据（可扩展）

    EdgeReport() = default;
    EdgeReport(const std::string& _id, const std::map<std::string, std::string>& _data,
                 const std::string& _access_token = "")
        : MsgCome(), id(_id), data(_data) {
        access_token = _access_token;
    }
};

class AckEdgeReport : public AckMsgCome {
public:
    bool received;              // 是否成功接收
    std::string id;             // 对应设备ID
    std::string timestamp;      // 服务器接收时间（可选）

    AckEdgeReport() : AckMsgCome(), received(false) {}
    AckEdgeReport(int32_t _code, bool _received, const std::string& _id, 
                    const std::string& _msg = "", const std::string& _ts = "")
        : AckMsgCome(_code, _msg), received(_received), id(_id), timestamp(_ts) {}
};

// ==============================
// 服务器下发消息（指令/配置等）
// ==============================

class ServerCommand : public MsgCome {
public:
    std::string id;                         // 目标设备ID
    std::string command_type;               // 指令类型（如 "reboot", "config_update"）
    std::map<std::string, std::string> payload; // 指令参数

    ServerCommand() = default;
    ServerCommand(const std::string& _id, const std::string& _cmd,
                  const std::map<std::string, std::string>& _payload = {})
        : id(_id), command_type(_cmd), payload(_payload) {}
};

class AckServerCommand : public AckMsgCome {
public:
    std::string id;             // 设备ID
    std::string command_type;   // 回应的指令类型
    bool handled;               // 是否成功处理
    std::string result;         // 处理结果描述

    AckServerCommand() : AckMsgCome(), handled(false) {}
    AckServerCommand(int32_t _code, const std::string& _id, const std::string& _cmd,
                     bool _handled, const std::string& _msg = "", const std::string& _result = "")
        : AckMsgCome(_code, _msg), id(_id), command_type(_cmd), handled(_handled), result(_result) {}
};

// ==============================
// 边缘心跳消息
// ==============================

class EdgeHeartbeat : public MsgCome {
public:
    std::string id;             // 设备ID
    std::string timestamp;       // 心跳时间戳（可选）

    EdgeHeartbeat() = default;
    EdgeHeartbeat(const std::string& _id, const std::string& _access_token = "",
                    const std::string& _timestamp = "")
        : MsgCome(), id(_id), timestamp(_timestamp) {
        access_token = _access_token;
    }
};

class AckEdgeHeartbeat : public AckMsgCome {
public:
    std::string id;             // 设备ID
    std::string timestamp;      // 服务器响应时间戳

    AckEdgeHeartbeat() : AckMsgCome() {}
    AckEdgeHeartbeat(int32_t _code, const std::string& _id, const std::string& _msg = "",
                       const std::string& _timestamp = "")
        : AckMsgCome(_code, _msg), id(_id), timestamp(_timestamp) {}
};

// ==============================
// 配置更新消息
// ==============================

class ConfigUpdate : public MsgCome {
public:
    std::string edge_id;      // 目标设备ID
    std::string config_type;    // 配置类型（如 "frpc", "system"）
    std::string config_content; // 配置内容（JSON 字符串或配置文本）
    int32_t version;            // 配置版本号

    ConfigUpdate() : version(0) {}
    ConfigUpdate(const std::string& _edge_id, const std::string& _config_type,
                 const std::string& _config_content, int32_t _version = 0,
                 const std::string& _access_token = "")
        : edge_id(_edge_id), config_type(_config_type), config_content(_config_content),
          version(_version) {
        access_token = _access_token;
    }
};

class AckConfigUpdate : public AckMsgCome {
public:
    std::string edge_id;      // 设备ID
    std::string config_type;    // 配置类型
    bool applied;               // 是否成功应用
    int32_t version;            // 应用的配置版本号
    std::string result;         // 应用结果描述

    AckConfigUpdate() : AckMsgCome(), applied(false), version(0) {}
    AckConfigUpdate(int32_t _code, const std::string& _edge_id, const std::string& _config_type,
                    bool _applied, int32_t _version, const std::string& _msg = "",
                    const std::string& _result = "")
        : AckMsgCome(_code, _msg), edge_id(_edge_id), config_type(_config_type),
          applied(_applied), version(_version), result(_result) {}
};

// ==============================
// TunnelService 配置相关结构（嵌套在 config_content 中）
// ==============================

// 端点配置（嵌套在 tunnelService 中）
struct CEndpoint {
    std::string host;           // 主机地址
    int32_t port;               // 端口号

    CEndpoint() : port(0) {}
    CEndpoint(const std::string& _host, int32_t _port)
        : host(_host), port(_port) {}
};

// 安全配置（嵌套在 tunnelService 中）
struct CSecurity {
    std::string authMethod;     // 认证方法（如 "token"）
    std::string credential;     // 认证凭证
    bool enableTls;             // 是否启用 TLS

    CSecurity() : enableTls(false) {}
    CSecurity(const std::string& _authMethod, const std::string& _credential, bool _enableTls)
        : authMethod(_authMethod), credential(_credential), enableTls(_enableTls) {}
};

// 本地管理配置（嵌套在 tunnelService 中）
struct CLocalManagement {
    std::string bindAddress;    // 本地管理绑定地址
    int32_t bindPort;           // 本地管理绑定端口

    CLocalManagement() : bindPort(0) {}
    CLocalManagement(const std::string& _bindAddr, int32_t _bindPort)
        : bindAddress(_bindAddr), bindPort(_bindPort) {}
};

// 目标服务配置（嵌套在 accessPolicies 中）
struct CTargetService {
    std::string ip;             // 目标服务 IP
    int32_t port;               // 目标服务端口

    CTargetService() : port(0) {}
    CTargetService(const std::string& _ip, int32_t _port)
        : ip(_ip), port(_port) {}
};

// 单个访问策略
struct CAccessPolicy {
    std::string policyId;       // 策略 ID
    std::string protocol;       // 协议（如 "tcp", "udp"）
    CTargetService targetService;  // 目标服务配置：{ip, port}
    int32_t exposedPort;        // 暴露的端口
    std::string description;    // 策略描述

    CAccessPolicy() : exposedPort(0) {}
    CAccessPolicy(const std::string& _policyId, const std::string& _protocol,
                  const CTargetService& _ts, int32_t _exposedPort, const std::string& _desc = "")
        : policyId(_policyId), protocol(_protocol), targetService(_ts),
          exposedPort(_exposedPort), description(_desc) {}
};

// 隧道服务配置（使用嵌套对象结构）
struct CtunnelServiceCfg {
    std::string version;        // 配置版本号（如 "1.0"）
    CEndpoint endpoint;         // 端点配置：{host, port}
    CSecurity security;         // 安全配置：{authMethod, credential, enableTls}
    CLocalManagement localManagement;  // 本地管理配置：{bindAddress, bindPort}

    CtunnelServiceCfg() = default;
    CtunnelServiceCfg(const std::string& _ver, const CEndpoint& _ep,
                      const CSecurity& _sec, const CLocalManagement& _lm)
        : version(_ver), endpoint(_ep), security(_sec), localManagement(_lm) {}
};

// 单个服务配置项（用于 config_content 数组中的元素）
// 每个配置项包含：一个隧道服务配置（带名称）和它的 accessPolicies 数组
struct CServiceConfig {
    std::string serviceName;          // 服务名称（如 "tunnelService", "tunnelService2"）
    CtunnelServiceCfg tunnelService;  // 隧道服务配置
    std::vector<CAccessPolicy> accessPolicies;  // 该服务的访问策略数组

    CServiceConfig() = default;
    CServiceConfig(const std::string& _name, const CtunnelServiceCfg& _ts,
                   const std::vector<CAccessPolicy>& _ap)
        : serviceName(_name), tunnelService(_ts), accessPolicies(_ap) {}
};

// 配置内容（嵌套在 config_content 字段中）
// config_content 是数组，包含多个并列的服务配置项
struct CConfigContent {
    std::vector<CServiceConfig> services;  // 多个并列的服务配置项

    CConfigContent() = default;
    CConfigContent(const std::vector<CServiceConfig>& _sv)
        : services(_sv) {}
};

// ==============================
// ConfigUpdate_tunnelService - 隧道服务配置更新消息
// ==============================

class ConfigUpdate_tunnelService : public ConfigUpdate {
public:
    CConfigContent configContent;   // 配置内容（支持多个隧道服务和访问策略数组）

    ConfigUpdate_tunnelService() : ConfigUpdate() {}
    ConfigUpdate_tunnelService(const std::string& _edge_id, const CConfigContent& _content,
                               int32_t _version = 0, const std::string& _access_token = "")
        : ConfigUpdate(_edge_id, "tunnelService", "", _version, _access_token),
          configContent(_content) {}
};

// ==============================
// FrpcConfig - frpc 配置结构（嵌套对象设计）
// ==============================

// frpc 代理配置
struct FrpcProxy {
    std::string name;       // 代理名称
    std::string type;       // 代理类型（如 "tcp", "udp", "http", "https"）
    std::string localIP;    // 本地 IP
    int32_t localPort;      // 本地端口
    int32_t remotePort;     // 远程端口

    FrpcProxy() : localPort(0), remotePort(0) {}
    FrpcProxy(const std::string& _name, const std::string& _type,
              const std::string& _localIP, int32_t _localPort, int32_t _remotePort)
        : name(_name), type(_type), localIP(_localIP),
          localPort(_localPort), remotePort(_remotePort) {}
};

// frpc 配置（扁平结构）
struct FrpcConfig {
    std::string serverAddr;     // 服务器地址
    int32_t serverPort;         // 服务器端口
    std::string authMethod;     // 认证方法（如 "token"）
    std::string token;          // 认证令牌
    bool tlsEnable;             // 是否启用 TLS
    std::string webServerAddr;  // Web 服务器地址
    int32_t webServerPort;      // Web 服务器端口
    std::vector<FrpcProxy> proxies;  // 代理列表

    FrpcConfig() : serverPort(0), authMethod(), token(), tlsEnable(false),
                   webServerAddr(), webServerPort(0) {}
};

// AckConfigUpdate_tunnelService 复用 AckConfigUpdate 即可

// ==============================
// Ingress 负载状态上报（Manager ↔ Ingress）
// ==============================

class IngressLoadReport : public MsgCome {
public:
    std::string ingress_id;     // Ingress 实例ID
    int32_t connection_count;   // 当前连接数
    double cpu_usage;           // CPU 使用率（百分比）
    double memory_usage;        // 内存使用率（百分比）
    std::string timestamp;      // 上报时间戳

    IngressLoadReport() : connection_count(0), cpu_usage(0.0), memory_usage(0.0) {}
    IngressLoadReport(const std::string& _ingress_id, int32_t _conn_count,
                      double _cpu, double _memory, const std::string& _timestamp = "")
        : ingress_id(_ingress_id), connection_count(_conn_count),
          cpu_usage(_cpu), memory_usage(_memory), timestamp(_timestamp) {}
};

class AckIngressLoadReport : public AckMsgCome {
public:
    std::string ingress_id;     // Ingress 实例ID
    std::string timestamp;      // 服务器接收时间戳

    AckIngressLoadReport() : AckMsgCome() {}
    AckIngressLoadReport(int32_t _code, const std::string& _ingress_id, const std::string& _msg = "",
                         const std::string& _timestamp = "")
        : AckMsgCome(_code, _msg), ingress_id(_ingress_id), timestamp(_timestamp) {}
};

// ==============================
// Manager 管理指令（Manager ↔ Ingress）
// ==============================

class ManagerCommand : public MsgCome {
public:
    std::string command_type;   // 指令类型（如 "update_config", "device_command", "ingress_control"）
    std::string target_id;      // 目标ID（设备ID或Ingress ID）
    std::map<std::string, std::string> params; // 指令参数

    ManagerCommand() = default;
    ManagerCommand(const std::string& _cmd_type, const std::string& _target_id,
                   const std::map<std::string, std::string>& _params = {},
                   const std::string& _access_token = "")
        : command_type(_cmd_type), target_id(_target_id), params(_params) {
        access_token = _access_token;
    }
};

class AckManagerCommand : public AckMsgCome {
public:
    std::string command_type;   // 指令类型
    std::string target_id;      // 目标ID
    bool executed;              // 是否成功执行
    std::string result;         // 执行结果描述

    AckManagerCommand() : AckMsgCome(), executed(false) {}
    AckManagerCommand(int32_t _code, const std::string& _cmd_type, const std::string& _target_id,
                      bool _executed, const std::string& _msg = "", const std::string& _result = "")
        : AckMsgCome(_code, _msg), command_type(_cmd_type), target_id(_target_id),
          executed(_executed), result(_result) {}
};

// ==============================
// Manager 设备注册消息（Edge ↔ Manager）
// ==============================

class EdgeRegisterRequest : public MsgCome {
public:
    std::string edge_id;      // 边缘唯一标识
    std::string edge_type;    // 边缘类型
    std::string edge_key;     // 设备密钥
    std::string local_ip;       // 设备本地IP（可选）

    EdgeRegisterRequest() = default;
    EdgeRegisterRequest(const std::string& _edge_id, const std::string& _edge_type,
                         const std::string& _edge_key, const std::string& _local_ip = "")
        : edge_id(_edge_id), edge_type(_edge_type), edge_key(_edge_key), local_ip(_local_ip) {}
};

// Ingress 信息结构
struct IngressInfo {
    std::string host;           // Ingress 主机地址
    int32_t port;               // Ingress 端口

    IngressInfo() : port(0) {}
    IngressInfo(const std::string& _host, int32_t _port) : host(_host), port(_port) {}
};

class AckEdgeRegister : public AckMsgCome {
public:
    IngressInfo ingress;        // 分配的 Ingress 信息
    std::string token;          // 认证令牌
    std::string edge_public_ip; // 设备公网IP

    AckEdgeRegister() : AckMsgCome() {}
    AckEdgeRegister(int32_t _code, const std::string& _msg = "",
                      const std::string& _ingress_host = "", int32_t _ingress_port = 0,
                      const std::string& _token = "", const std::string& _edge_public_ip = "")
        : AckMsgCome(_code, _msg), ingress(_ingress_host, _ingress_port),
          token(_token), edge_public_ip(_edge_public_ip) {}
};

// ==============================
// Manager ↔ Ingress 通信消息
// ==============================

// Manager 连接通知（Manager → Ingress）
class ManagerConnect : public MsgCome {
public:
    std::string manager_id;     // Manager 实例 ID
    std::string ingress_id;     // Ingress 实例 ID
    std::string timestamp;      // 连接时间戳

    ManagerConnect() = default;
    ManagerConnect(const std::string& _manager_id, const std::string& _ingress_id,
                   const std::string& _timestamp = "")
        : manager_id(_manager_id), ingress_id(_ingress_id), timestamp(_timestamp) {}
};

// Ingress 通知 Manager 边缘上线（Ingress → Manager）
class IngressEdgeOnline : public MsgCome {
public:
    std::string edge_id;        // 边缘 ID
    std::string edge_type;      // 边缘类型
    std::string local_ip;       // 边缘本地 IP
    std::string public_ip;      // 边缘公网 IP
    std::string ingress_id;     // Ingress 实例 ID
    int64_t online_since;       // 上线时间（UTC Unix 时间戳，秒）

    IngressEdgeOnline() : online_since(0) {}
    IngressEdgeOnline(const std::string& _edge_id, const std::string& _ingress_id,
                      int64_t _online_since, const std::string& _edge_type = "",
                      const std::string& _local_ip = "", const std::string& _public_ip = "")
        : edge_id(_edge_id), edge_type(_edge_type), local_ip(_local_ip), public_ip(_public_ip),
          ingress_id(_ingress_id), online_since(_online_since) {}
};

// Ingress 通知 Manager 边缘下线（Ingress → Manager）
class IngressEdgeOffline : public MsgCome {
public:
    std::string edge_id;        // 边缘 ID
    std::string ingress_id;     // Ingress 实例 ID

    IngressEdgeOffline() = default;
    IngressEdgeOffline(const std::string& _edge_id, const std::string& _ingress_id = "")
        : edge_id(_edge_id), ingress_id(_ingress_id) {}
};

// Manager 查询边缘列表（Manager → Ingress）
class ManagerEdgeList : public MsgCome {
public:
    int32_t offset;             // 偏移量
    int32_t limit;              // 每页数量

    ManagerEdgeList() : offset(0), limit(50) {}
    ManagerEdgeList(int32_t _offset, int32_t _limit)
        : offset(_offset), limit(_limit) {}
};

// 边缘列表响应（Ingress → Manager）
class AckManagerEdgeList : public AckMsgCome {
public:
    std::vector<std::string> edge_ids;  // 边缘 ID 列表
    int32_t total;              // 总边缘数
    int32_t offset;             // 请求的偏移量
    int32_t limit;              // 请求的每页数量
    int32_t count;              // 返回的边缘数量

    AckManagerEdgeList() : total(0), offset(0), limit(50), count(0) {}
};

// Manager 查询离线边缘列表（Manager → Ingress）
class ManagerEdgeOfflineList : public MsgCome {
public:
    int32_t offset;             // 偏移量
    int32_t limit;              // 每页数量

    ManagerEdgeOfflineList() : offset(0), limit(50) {}
    ManagerEdgeOfflineList(int32_t _offset, int32_t _limit)
        : offset(_offset), limit(_limit) {}
};

// 离线边缘信息
struct OfflineEdgeInfo {
    std::string edge_id;        // 边缘 ID
    int64_t offline_time;       // 离线时间（UTC Unix 时间戳，秒）
    bool pending_confirm;       // 是否等待 Manager 确认
    std::string edge_type;      // 边缘类型

    OfflineEdgeInfo() : offline_time(0), pending_confirm(false) {}
    OfflineEdgeInfo(const std::string& _edge_id, int64_t _offline_time,
                    bool _pending_confirm, const std::string& _edge_type = "")
        : edge_id(_edge_id), offline_time(_offline_time), pending_confirm(_pending_confirm),
          edge_type(_edge_type) {}
};

// 离线边缘列表响应（Ingress → Manager）
class AckManagerEdgeOfflineList : public AckMsgCome {
public:
    std::vector<OfflineEdgeInfo> offline_devices;  // 离线设备列表
    int32_t total;              // 总离线边缘数
    int32_t offset;             // 请求的偏移量
    int32_t limit;              // 请求的每页数量
    int32_t count;              // 返回的离线边缘数量

    AckManagerEdgeOfflineList() : total(0), offset(0), limit(50), count(0) {}
};

// Manager 踢边缘（Manager → Ingress）
class ManagerEdgeKick : public MsgCome {
public:
    std::string edge_id;        // 边缘 ID
    std::string reason;         // 踢下线原因

    ManagerEdgeKick() = default;
    ManagerEdgeKick(const std::string& _edge_id, const std::string& _reason = "")
        : edge_id(_edge_id), reason(_reason) {}
};

// 踢边缘响应（Ingress → Manager）
class AckManagerEdgeKick : public AckMsgCome {
public:
    std::string edge_id;        // 边缘 ID
    bool success;               // 是否成功

    AckManagerEdgeKick() : success(false) {}
    AckManagerEdgeKick(const std::string& _edge_id, bool _success,
                       int32_t _code = 0, const std::string& _msg = "")
        : AckMsgCome(_code, _msg), edge_id(_edge_id), success(_success) {}
};

#endif // COME_1_H
