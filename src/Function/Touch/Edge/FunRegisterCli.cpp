/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunRegisterCli.cpp - device registration function layer implementation (client)
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: FunRegisterCli.cpp $
 *
 *  Explain:
 *     Implementation of device registration function layer for client side.
 *
 *  Update:
 *     2013-08-22 20:15:30 Create
 *     2026-02-25 Add heartbeat support
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "FunRegisterCli.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>  // for chmod
#include <fstream>     // for file reading

#include "../../../Logs.h"

// 静态成员变量定义
const std::string CFunRegisterCli::FRPC_CONFIG_DIR = "./shpc";

// 该 edge 的 frpc 配置文件路径, 按 edge_id 命名以免多 edge 互相覆盖
//   例如 edge_id=edge001 -> ./shpc/shpc.edge001.json
std::string CFunRegisterCli::get_frpc_config_path() const
{
    return FRPC_CONFIG_DIR + "/shpc." + m_edge_id + ".json";
}

CFunRegisterCli::CFunRegisterCli(const std::string& edge_id, const std::string& edge_key,
                                 const std::string& edge_type)
    : m_edge_id(edge_id)
    , m_edge_key(edge_key)
    , m_edge_type(edge_type)
    , m_token("")
    , m_started(false)
    , m_on_register_result(NULL)
    , m_on_reconnect_failed(NULL)
    , m_user_data(NULL)
    , m_need_reregister(false)
    , m_registered(false)
    , m_heartbeat_timer("HeartbeatTimer")
{
}

CFunRegisterCli::~CFunRegisterCli()
{
    Stop();
}

void CFunRegisterCli::UpdateEdgeParams(const std::string& edge_id, const std::string& edge_key,
                                         const std::string& edge_type)
{
    m_edge_id = edge_id;
    m_edge_key = edge_key;
    if (!edge_type.empty()) {
        m_edge_type = edge_type;
    }
    // 重置注册状态和 token，下次注册时使用新参数
    m_registered = false;
    m_token = "";
}

void CFunRegisterCli::Start(const char *server_addr, unsigned short port, int reconnect_max_retries)
{
    if (m_started) {
        return;
    }

    // 启动 WebSocket 客户端
    if (!g_DevWsRegisterCli.Start(server_addr, port, "/come", "come.1", reconnect_max_retries)) {
        return;
    }

    // 注册 WebSocket 统一信号槽
    g_DevWsRegisterCli.Start(this, (CDevWsRegisterCli::DevWsRegisterCliSignalProc_t)&CFunRegisterCli::OnWebsocketNotify);

    m_started = true;
}

void CFunRegisterCli::Stop()
{
    if (!m_started) {
        return;
    }

    // 停止心跳
    StopHeartbeat();

    // 注销 WebSocket 统一信号槽
    g_DevWsRegisterCli.Stop(this, (CDevWsRegisterCli::DevWsRegisterCliSignalProc_t)&CFunRegisterCli::OnWebsocketNotify);

    g_DevWsRegisterCli.Stop();

    m_started = false;
}

void CFunRegisterCli::SetRegisterCallback(OnRegisterResultProc_t proc, void *user_data)
{
    m_on_register_result = proc;
    m_user_data = user_data;
}

void CFunRegisterCli::SetReconnectFailedCallback(OnReconnectFailedProc_t proc, void *user_data)
{
    m_on_reconnect_failed = proc;
    m_user_data = user_data;
}

bool CFunRegisterCli::NeedReregister() const
{
    return m_need_reregister;
}

bool CFunRegisterCli::RegisterEdge(bool is_power_on)
{
    if (!m_started || !g_DevWsRegisterCli.IsConnected()) {
        return false;
    }

    // 生成随机 nonce
    char nonce_buf[33];
    srand(time(NULL));
    snprintf(nonce_buf, sizeof(nonce_buf), "%08x%08x%08x%08x",
             rand(), rand(), rand(), rand());
    std::string nonce(nonce_buf);

    // 阶段1：使用 EdgeOnline 数据结构构造 JSON-RPC 2.0 请求
    EdgeOnline edge_online;
    edge_online.id = m_edge_id;
    edge_online.key = m_edge_key;
    edge_online.type = m_edge_type;
    edge_online.nonce = nonce;
    edge_online.token = "";  // token 从 Manager 获取，这里为空
    
    // 生成请求 ID（使用时间戳）
    time_t now = time(NULL);
    int64_t req_id = static_cast<int64_t>(now);
    
    // 使用 ComeJsonCodec 编码为 JSON-RPC 2.0 格式
    std::string json_str = ComeJsonCodec::encodeJsonRpcRequest(edge_online, COME_METHOD_EDGE_ONLINE, req_id);
    if (json_str.empty()) {
        return false;
    }

    // 发送注册消息
    int ret = g_DevWsRegisterCli.SendText(json_str.c_str(), json_str.length());
    if (ret != 0) {
        return false;
    }

    return true;
}

void CFunRegisterCli::StartHeartbeat()
{
    if (m_heartbeat_timer.IsStarted()) {
        return;  // 心跳已启动
    }

    // 启动周期性定时器
    m_heartbeat_timer.Start(
        this,
        (EZ_TIMER_PROC)&CFunRegisterCli::on_heartbeat_timeout,
        HEARTBEAT_INTERVAL_SEC * 1000,  // 持续时间（毫秒）
        HEARTBEAT_INTERVAL_SEC * 1000,  // 间隔时间（毫秒，周期性）
        0,                               // 参数
        0                                // 超时
    );

    ez_printf_info("Heartbeat started, interval: %d seconds\n", HEARTBEAT_INTERVAL_SEC);
}

void CFunRegisterCli::StopHeartbeat()
{
    if (m_heartbeat_timer.IsStarted()) {
        m_heartbeat_timer.Stop(EZTHREAD_BOOL_FALSE);
        ez_printf_info("Heartbeat stopped\n");
    }
}

void CFunRegisterCli::on_heartbeat_timeout(EZTHREAD_PARAM wParam)
{
    (void)wParam;
    if (m_registered && g_DevWsRegisterCli.IsConnected()) {
        send_heartbeat();
    }
}

void CFunRegisterCli::send_heartbeat()
{
    // 获取当前时间戳
    time_t now = time(NULL);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    // 阶段 1：使用 EdgeHeartbeat 数据结构构造 JSON-RPC 2.0 通知
    EdgeHeartbeat heartbeat;
    heartbeat.id = m_edge_id;
    heartbeat.access_token = m_token;  // 使用注册时获取的 token
    heartbeat.timestamp = timestamp;

    // 使用 ComeJsonCodec 编码为 JSON-RPC 2.0 通知格式（无 id）
    std::string json_str = ComeJsonCodec::encodeJsonRpcNotification(heartbeat, COME_METHOD_EDGE_HEARTBEAT);

    if (json_str.empty()) {
        ez_printf_error("Failed to encode heartbeat message\n");
        return;
    }

    // 发送心跳消息（通知，无响应）
    int ret = g_DevWsRegisterCli.SendText(json_str.c_str(), json_str.length());
    if (ret != 0) {
        ez_printf_error("Failed to send heartbeat message\n");
        return;
    }

    ez_printf_info("Heartbeat sent at %s\n", timestamp);
}

void CFunRegisterCli::OnWebsocketNotify(CDevWsRegisterCli::SignalType sig_type, const void *data, size_t len, int param1, int param2)
{
    switch (sig_type) {
        case CDevWsRegisterCli::SIGNAL_RECEIVE:
            // param1=is_binary, param2=status
            if (param1 == 0) {  // 只处理文本消息
                handle_receive(data, len);
            }
            break;

        case CDevWsRegisterCli::SIGNAL_CONNECTED:
            // param1=status, param2=0
            // 连接建立后自动触发上电注册
            RegisterEdge(true);
            break;

        case CDevWsRegisterCli::SIGNAL_DISCONNECTED:
            // param1=error_code, param2=0
            // 连接断开，ezsocket 库会自动尝试重连（当前配置为无限重试）
            // 注意：重连是异步的，由 DevWsRegisterCli/ez_wsclient-native 管理重连策略
            ez_printf_error("WebSocket disconnected, ezsocket will auto-reconnect...\n");
            // 连接断开时停止心跳
            StopHeartbeat();
            m_registered = false;
            break;

        default:
            break;
    }
}

void CFunRegisterCli::handle_receive(const void *data, size_t len)
{
    std::string jsonStr = std::string((const char*)data, len);

    // 首先检查是否为 JSON-RPC 2.0 响应（注册响应、心跳响应等）
    if (ComeJsonCodec::isResponse(jsonStr)) {
        // 尝试解码为 AckEdgeOnline（成功响应）
        AckEdgeOnline ack;
        if (ComeJsonCodec::decodeJsonRpcResponse(jsonStr, ack)) {
            // 成功响应：edge.online 的响应
            handle_register_ack_jsonrpc2(ack);
            return;
        }

        // 尝试解码为错误响应
        int32_t error_code = 0;
        std::string error_msg;
        if (ComeJsonCodec::decodeJsonRpcError(jsonStr, error_code, error_msg)) {
            // 错误响应：注册失败
            handle_register_ack_jsonrpc2_error(error_code, error_msg);
            return;
        }
        return;
    }

    // 检查是否为 JSON-RPC 2.
    if (ComeJsonCodec::isRequest(jsonStr)) {
        // 提取请求 ID
        int64_t req_id = ComeJsonCodec::extractJsonRpcId(jsonStr);

        // 尝试解析 JSON 获取 method
        JsonValue json = JsonValue::parse(jsonStr);
        if (json.isNull()) {
            ez_printf_warning("Failed to parse JSON request\n");
            return;
        }

        std::string method = json.getString("method");
        
        // 处理 edge.config.query 查询请求
        if (method == "edge.config.query") {
            ez_printf_info("Received edge.config.query request [req_id=%ld]\n", (long)req_id);
            
            // 从 params 中获取 edge_id
            std::string edge_id;
            if (json.contains("params")) {
                JsonValue params = json.getObject("params");
                if (params.contains("edge_id")) {
                    edge_id = params.getString("edge_id");
                }
            }
            
            // 验证 edge_id 是否匹配
            if (!edge_id.empty() && edge_id != m_edge_id) {
                ez_printf_warning("edge.config.query: edge_id mismatch (expected=%s, got=%s)\n", m_edge_id.c_str(), edge_id.c_str());
            }
            
            // 读取当前 frpc 配置文件
            std::string config_file = get_frpc_config_path();
            std::ifstream ifs(config_file);
            if (!ifs.is_open()) {
                ez_printf_error("Failed to open config file: %s\n", config_file.c_str());
                send_config_query_error(req_id, -1, "Failed to read config file");
                return;
            }
            
            std::string config_json((std::istreambuf_iterator<char>(ifs)),
                                     std::istreambuf_iterator<char>());
            ifs.close();
            
            ez_printf_info("Read config from %s: %s\n", config_file.c_str(), config_json.c_str());
            
            // 构造查询响应：直接返回 frpc 配置 JSON
            JsonValue result_obj = JsonValue::createObject();
            result_obj.setString("edge_id", m_edge_id);
            result_obj.setString("config_type", "tunnelService");
            
            // 解析 frpc JSON 并添加到 result 中
            JsonValue frpc_json = JsonValue::parse(config_json);
            if (!frpc_json.isNull()) {
                result_obj.setObject("config", frpc_json);
            } else {
                ez_printf_warning("Failed to parse frpc config JSON for response\n");
                result_obj.setString("config_error", "Failed to parse config file");
            }
            
            // 构造 JSON-RPC 2.0 成功响应
            JsonValue id_val = JsonValue::createInt64(req_id);
            
            std::string resp_json = ComeJsonCodec::buildJsonRpcSuccess(result_obj, id_val);
            if (resp_json.empty()) {
                ez_printf_error("Failed to build query response JSON\n");
                return;
            }
            
            // 发送响应
            send_message(resp_json.c_str(), resp_json.length());
            ez_printf_info("Sent config query response for edge %s\n", m_edge_id.c_str());
            return;
        }

        // 尝试 decode 为 ConfigUpdate_tunnelService（配置更新）
        ConfigUpdate_tunnelService configMsg;
        if (ComeJsonCodec::decode(jsonStr, configMsg)) {
            // 成功解析为配置消息
            ez_printf_info("=== [CONFIG.UPDATE.RECV] Received ConfigUpdate_tunnelService for edge %s ===\n", configMsg.edge_id.c_str());
            ez_printf_info("  - config_type: %s\n", configMsg.config_type.c_str());
            ez_printf_info("  - version: %ld\n", (long)configMsg.version);
            ez_printf_info("  - access_token: %s\n", configMsg.access_token.c_str());
            ez_printf_info("  - configContent.services.size(): %zu\n", configMsg.configContent.services.size());
            if (!configMsg.configContent.services.empty()) {
                auto& svc = configMsg.configContent.services[0];
                ez_printf_info("  - service[0].name: %s\n", svc.serviceName.c_str());
                auto& ts = svc.tunnelService;
                ez_printf_info("  - tunnelService.endpoint: %s:%d\n", ts.endpoint.host.c_str(), ts.endpoint.port);
                ez_printf_info("  - tunnelService.localManagement: %s:%d\n", ts.localManagement.bindAddress.c_str(), ts.localManagement.bindPort);
                ez_printf_info("  - tunnelService.security.authMethod: %s, enableTls: %d\n", ts.security.authMethod.c_str(), ts.security.enableTls);
                ez_printf_info("  - service.accessPolicies.size(): %zu\n", svc.accessPolicies.size());
                for (size_t i = 0; i < svc.accessPolicies.size(); i++) {
                    auto& ap = svc.accessPolicies[i];
                    ez_printf_info("    - accessPolicy[%zu]: %s, protocol=%s, targetService=%s:%d, exposedPort=%d\n",
                                   i, ap.policyId.c_str(), ap.protocol.c_str(),
                                   ap.targetService.ip.c_str(), ap.targetService.port, ap.exposedPort);
                }
            }

            // 验证 edge_id 是否匹配
            if (configMsg.edge_id != m_edge_id) {
                ez_printf_error("Edge ID mismatch: expected %s, got %s\n", m_edge_id.c_str(), configMsg.edge_id.c_str());
                send_config_ack_error(req_id, -1, "Edge ID mismatch");
                return;
            }

            // 1. 转换为 FrpcConfig
            FrpcConfig frpcCfg;
            if (!ComeJsonCodec::toFrpcConfig(configMsg, frpcCfg)) {
                ez_printf_error("Failed to convert ConfigUpdate to FrpcConfig\n");
                send_config_ack_error(req_id, -2, "Failed to convert to FrpcConfig");
                return;
            }
            
            // 打印转换后的 frpc 配置
            ez_printf_info("=== [CONFIG.UPDATE.CONVERT] Converted to FrpcConfig ===\n");
            ez_printf_info("  - serverAddr: %s, serverPort: %d\n", frpcCfg.serverAddr.c_str(), frpcCfg.serverPort);
            ez_printf_info("  - authMethod: %s, token: %s\n", frpcCfg.authMethod.c_str(), frpcCfg.token.c_str());
            ez_printf_info("  - tlsEnable: %d\n", frpcCfg.tlsEnable);
            ez_printf_info("  - webServerAddr: %s, webServerPort: %d\n", frpcCfg.webServerAddr.c_str(), frpcCfg.webServerPort);
            ez_printf_info("  - proxies.size(): %zu\n", frpcCfg.proxies.size());
            for (size_t i = 0; i < frpcCfg.proxies.size(); i++) {
                auto& p = frpcCfg.proxies[i];
                ez_printf_info("    - proxy[%zu]: name=%s, type=%s, localIP=%s, localPort=%d, remotePort=%d\n",
                               i, p.name.c_str(), p.type.c_str(), p.localIP.c_str(), p.localPort, p.remotePort);
            }
            
            // 2. 编码为 JSON 字符串
            std::string frpcJson = ComeJsonCodec::encode(frpcCfg);
            if (frpcJson.empty()) {
                ez_printf_error("Failed to encode FrpcConfig to JSON\n");
                send_config_ack_error(req_id, -3, "Failed to encode FrpcConfig");
                return;
            }

            // 3. 写入 frpc 配置文件
            if (!write_frpc_config_file(frpcJson)) {
                ez_printf_error("Failed to write frpc config file\n");
                send_config_ack_error(req_id, -4, "Failed to write config file");
                return;
            }

            ez_printf_info("=== [CONFIG.UPDATE.WRITE] Successfully wrote frpc config to %s ===\n", get_frpc_config_path().c_str());
            ez_printf_info("  - Written config JSON: %s\n", frpcJson.c_str());

            // 4. 回复 AckConfigUpdate（成功）
            send_config_ack_success(req_id);
            return;
        }

        // 无法解析为已知消息类型，忽略
        ez_printf_info("Unknown request message type, ignoring\n");
        return;
    }

    // 非 JSON-RPC 2.0 消息，忽略
    ez_printf_info("Not a valid JSON-RPC 2.0 message, ignoring\n");
}

// 处理 JSON-RPC 2.0 格式的注册响应（成功）
void CFunRegisterCli::handle_register_ack_jsonrpc2(const AckEdgeOnline& ack)
{
    // 使用 AckEdgeOnline 结构中的字段
    bool success = ack.success;
    std::string msg = ack.msg;
    std::string access_token = ack.access_token;

    // 保存 access_token 供后续使用
    m_token = access_token;

    // 调用用户回调
    if (m_on_register_result) {
        m_on_register_result(success, msg, m_user_data);
    }

    if (!success) {
        ez_printf_error("touch_edge: Edge registration failed [edge_id=%s, msg=%s]\n",
                        m_edge_id.c_str(), msg.c_str());
        return;
    }

    // 注册成功，启动心跳
    m_registered = true;
    StartHeartbeat();
    ez_printf_info("touch_edge: Edge registered successfully [edge_id=%s, heartbeat started]\n",
                   m_edge_id.c_str());
}

// 处理 JSON-RPC 2.0 格式的注册响应（错误）
void CFunRegisterCli::handle_register_ack_jsonrpc2_error(int32_t error_code, const std::string& error_msg)
{
    // 调用用户回调
    if (m_on_register_result) {
        m_on_register_result(false, error_msg, m_user_data);
    }

    ez_printf_error("touch_edge: Edge registration failed [edge_id=%s, code=%d, msg=%s]\n",
           m_edge_id.c_str(), error_code, error_msg.c_str());
}

// ==============================
// frpc 远程配置处理辅助函数
// ==============================

void CFunRegisterCli::send_config_ack_success(int64_t req_id)
{
    // 构建 JSON-RPC 2.0 成功响应
    // {"jsonrpc":"2.0","result":{"code":0,"message":"success","applied":true,"edge_id":"edge001"},"id":<req_id>}
    JsonValue resultObj = JsonValue::createObject();
    resultObj.setInt("code", 0);
    resultObj.setString("message", "success");  // 使用 message 字段名，与 manager 期望的一致
    resultObj.setBool("applied", true);
    resultObj.setString("edge_id", m_edge_id);  // 添加 edge_id 字段，方便 manager 更新数据库

    std::string ackJson = ComeJsonCodec::buildJsonRpcSuccess(resultObj, JsonValue::createInt64(req_id));
    send_message(ackJson.c_str(), ackJson.length());
}

void CFunRegisterCli::send_config_ack_error(int64_t req_id, int32_t code, const std::string& msg)
{
    // 构建错误响应
    std::string errJson = ComeJsonCodec::buildJsonRpcError(code, msg, JsonValue::createInt64(req_id));
    send_message(errJson.c_str(), errJson.length());
}

void CFunRegisterCli::send_config_query_error(int64_t req_id, int32_t code, const std::string& msg)
{
    // 构建错误响应
    std::string errJson = ComeJsonCodec::buildJsonRpcError(code, msg, JsonValue::createInt64(req_id));
    send_message(errJson.c_str(), errJson.length());
}

void CFunRegisterCli::send_message(const char *data, size_t len)
{
    if (!g_DevWsRegisterCli.IsConnected()) {
        ez_printf_error("WebSocket not connected, cannot send message\n");
        return;
    }

    int ret = g_DevWsRegisterCli.SendText(data, len);
    if (ret != 0) {
        ez_printf_error("Failed to send message: %d\n", ret);
    }
}

bool CFunRegisterCli::write_frpc_config_file(const std::string& jsonStr)
{
    // 确保目录存在
    system(("mkdir -p " + FRPC_CONFIG_DIR).c_str());

    // 写入文件
    const std::string config_file = get_frpc_config_path();
    FILE* fp = fopen(config_file.c_str(), "w");
    if (!fp) {
        ez_printf_error("Failed to open %s for writing\n", config_file.c_str());
        return false;
    }

    size_t written = fwrite(jsonStr.c_str(), 1, jsonStr.length(), fp);
    fclose(fp);

    if (written != jsonStr.length()) {
        ez_printf_error("Failed to write complete JSON to file\n");
        return false;
    }

    // 设置文件权限为 0644
    chmod(config_file.c_str(), 0644);

    return true;
}

// 心跳是通知，无响应，不需要处理响应
