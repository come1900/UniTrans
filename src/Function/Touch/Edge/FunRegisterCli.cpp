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

#include "../../../Logs.h"

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

    // 阶段1：使用 EdgeHeartbeat 数据结构构造 JSON-RPC 2.0 通知
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
    std::string json_str((const char *)data, len);

    // JSON-RPC 2.0 格式处理
    if (ComeJsonCodec::isResponse(json_str)) {
        // 尝试解码为 AckEdgeOnline（成功响应）
        AckEdgeOnline ack;
        if (ComeJsonCodec::decodeJsonRpcResponse(json_str, ack)) {
            // 成功响应：edge.online 的响应
            handle_register_ack_jsonrpc2(ack);
            return;
        }
        
        // 尝试解码为错误响应
        int32_t error_code = 0;
        std::string error_msg;
        if (ComeJsonCodec::decodeJsonRpcError(json_str, error_code, error_msg)) {
            // 错误响应：注册失败
            handle_register_ack_jsonrpc2_error(error_code, error_msg);
            return;
        }
    }
    
    // 心跳是通知，不会有响应，这里不需要处理
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


// 心跳是通知，无响应，不需要处理响应
