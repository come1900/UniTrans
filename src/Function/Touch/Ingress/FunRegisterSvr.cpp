/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunRegisterSvr.cpp - device registration function layer implementation (server)
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: FunRegisterSvr.cpp $
 *
 *  Explain:
 *     Implementation of device registration function layer for server side.
 *
 *  Update:
 *     2013-10-14 21:30:55 Create
 *     2026-02-25 Add heartbeat support
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include <ez_def_devel_debug.h>
#include <ez_socket.h>

#include "../../../Logs.h"

#include "FunRegisterSvr.h"

CFunRegisterSvr::CFunRegisterSvr()
    : m_started(false)
    , m_on_device_register(NULL)
    , m_user_data(NULL)
    , m_manager_client_id(EZ_WS_SERVER_INVALID_CLIENT_ID)
    , m_EdgeReportMode(1)  // 默认：上报所有已连接的 Edge
{
}

CFunRegisterSvr::~CFunRegisterSvr()
{
    Stop();
}

void CFunRegisterSvr::Start(unsigned short port, const char *protocol, const char *path_prefix)
{
    if (m_started) {
        ez_printf_warning("touch_ingress: Already started, ignoring Start() call\n");
        return;
    }

    ez_printf_info("touch_ingress: Starting on port %d, protocol=%s, path_prefix=%s\n",
           port, protocol ? protocol : "come.1", path_prefix ? path_prefix : "/come");

    // 启动 WebSocket 服务端
    if (!g_DevWsRegisterSvr.Start(port, protocol ? protocol : "come.1", path_prefix ? path_prefix : "/come")) {
        ez_printf_error("touch_ingress: Failed to start WebSocket server on port %d\n", port);
        return;
    }

    // 注册信号处理（启动 WebSocket 服务端线程并注册回调）
    if (!g_DevWsRegisterSvr.Start(this, (CDevWsRegisterSvr::DevWsRegisterSvrSignalProc_t)&CFunRegisterSvr::OnWebsocketNotify)) {
        ez_printf_error("touch_ingress: Failed to register WebSocket signal handler\n");
        g_DevWsRegisterSvr.Stop();
        return;
    }

    m_started = true;
    ez_printf_info("touch_ingress: Started successfully on port %d\n", port);
}

void CFunRegisterSvr::Stop()
{
    if (!m_started) {
        ez_printf_warning("touch_ingress: Already stopped, ignoring Stop() call\n");
        return;
    }

    ez_printf_info("touch_ingress: Stopping... (online_edges=%zu, offline_cache=%zu)\n",
           m_online_edges.size(), m_offline_edges_cache.size());

    // 注销信号处理（停止线程、清理 WebSocket 服务端）
    g_DevWsRegisterSvr.Stop(this, (CDevWsRegisterSvr::DevWsRegisterSvrSignalProc_t)&CFunRegisterSvr::OnWebsocketNotify);
    m_started = false;

    // 清理所有设备信息
    m_online_edges.clear();
    m_offline_edges_cache.clear();
    m_manager_client_id = EZ_WS_SERVER_INVALID_CLIENT_ID;

    ez_printf_info("touch_ingress: Stopped successfully\n");
}

void CFunRegisterSvr::SetRegisterCallback(OnDeviceRegisterProc_t proc, void *user_data)
{
    m_on_device_register = proc;
    m_user_data = user_data;
}

ClientType CFunRegisterSvr::GetClientType(int client_id) const
{
    auto it = m_online_edges.find(client_id);
    if (it != m_online_edges.end()) {
        return CLIENT_TYPE_EDGE;
    }
    if (client_id == m_manager_client_id) {
        return CLIENT_TYPE_MANAGER;
    }
    return CLIENT_TYPE_UNKNOWN;
}

void CFunRegisterSvr::OnWebsocketNotify(CDevWsRegisterSvr::SignalType sig_type, int client_id, const char *str_param, int int_param1, int int_param2, int int_param3)
{
    switch (sig_type) {
        case CDevWsRegisterSvr::SIGNAL_RECEIVE:
            // str_param=data(转为 char*), int_param1=len, int_param2=is_binary, int_param3=status
            if (int_param2 == 0) {  // 只处理文本消息
                handle_receive(client_id, str_param, (size_t)int_param1);
            }
            break;

        case CDevWsRegisterSvr::SIGNAL_CONNECTED: {
            // str_param=ip, int_param1=port, int_param2=status, int_param3=0
            // 连接建立时记录客户端的 IP 和端口
            ez_printf_info("touch_ingress: [CONNECT] client_id=%d, ip=%s, port=%d, online_edges=%zu\n",
                   client_id,
                   str_param ? str_param : "",
                   int_param1,
                   m_online_edges.size());

            // 创建 Edge 设备信息并记录公网 IP
            // 注意：这里先创建一个临时条目，等 edge.online 请求到达后再填充 edge_id
            EdgeDeviceInfo edge_info;
            edge_info.public_ip = str_param ? str_param : "";
            edge_info.edge_id = "";  // 初始为空，等待 edge.online 填充
            m_online_edges[client_id] = edge_info;
            break;
        }

        case CDevWsRegisterSvr::SIGNAL_DISCONNECTED: {
            // str_param=NULL, int_param1=error_code, int_param2=0, int_param3=0
            // 设备断开连接，打印信息并通知 manager（如果设备已注册）

            // 检查是否是 Manager 断开
            if (client_id == m_manager_client_id) {
                ez_printf_info("touch_ingress: [DISCONNECT] Manager disconnected [client_id=%d, error_code=%d], online_edges=%zu\n",
                       client_id, int_param1, m_online_edges.size());
                m_manager_client_id = EZ_WS_SERVER_INVALID_CLIENT_ID;
                break;
            }

            // 检查是否是 Edge 断开
            auto it = m_online_edges.find(client_id);
            if (it != m_online_edges.end()) {
                EdgeDeviceInfo& edge = it->second;

                if (edge.edge_id.empty()) {
                    // 连接建立但尚未完成 Edge 注册
                    ez_printf_debug("touch_ingress: [DISCONNECT] Edge disconnected before registration [client_id=%d, ip=%s, error_code=%d]\n",
                           client_id, edge.public_ip.c_str(), int_param1);
                } else {
                    // 已完成 Edge 注册后断开
                    ez_printf_info("touch_ingress: [DISCONNECT] Edge disconnected [client_id=%d, edge_id=%s, error_code=%d]\n",
                           client_id, edge.edge_id.c_str(), int_param1);

                    // 缓存离线事件
                    CacheEdgeOfflineEvent(edge.edge_id, edge);

                    // 如果 Manager 在线，立即上报离线事件
                    if (IsManagerConnected()) {
                        notify_device_offline(edge.edge_id);
                        m_offline_edges_cache[edge.edge_id].pending_manager_confirm = true;
                        ez_printf_debug("touch_ingress: [OFFLINE] Sent offline notification for edge %s to manager\n", edge.edge_id.c_str());
                    } else {
                        ez_printf_debug("touch_ingress: [OFFLINE] Cached offline event for edge %s (manager offline)\n", edge.edge_id.c_str());
                    }
                }

                // 清理在线设备信息
                m_online_edges.erase(it);
            } else {
                // 未知连接断开（可能是被踢掉的旧 Manager 或其他连接）
                ez_printf_debug("touch_ingress: [DISCONNECT] Unknown client disconnected [client_id=%d, error_code=%d]\n", client_id, int_param1);
            }
            break;
        }

        default:
            break;
    }
}

void CFunRegisterSvr::handle_receive(int client_id, const char *data, size_t len)
{
    std::string json_str(data, len);

    // 优先判断是否为 JSON-RPC 2.0 格式
    if (!ComeJsonCodec::isJsonRpc2(json_str)) {
        ez_printf_warning("touch_ingress: Non-JSON-RPC message from client_id=%d, closing connection: %.100s\n", client_id, data);
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 如果是响应（包含 result 或 error），先处理
    if (ComeJsonCodec::isJsonRpcResponse(json_str)) {
        JsonValue root = JsonValue::parse(json_str);
        JsonValue result_obj = root.getObject("result");
        if (!result_obj.isNull() && result_obj.contains("edge_id")) {
            std::string edge_id = result_obj.getString("edge_id", "");
            if (!edge_id.empty()) {
                HandleManagerOnlineConfirmResponse(edge_id);
                HandleManagerOfflineConfirmResponse(edge_id);
            }
        }
        return;
    }

    // 提取 method 和 id，根据 method 路由
    std::string method = ComeJsonCodec::extractJsonRpcMethod(json_str);
    int64_t id = ComeJsonCodec::extractJsonRpcId(json_str);

    if (method.empty()) {
        ez_printf_warning("touch_ingress: JSON-RPC message without method from client_id=%d, closing connection\n", client_id);
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 根据 method 精确路由
    if (method == COME_METHOD_EDGE_ONLINE) {
        EdgeOnline msg;
        if (ComeJsonCodec::decodeJsonRpcRequest(json_str, msg, method, id)) {
            handle_device_online_jsonrpc2(client_id, msg, id);
        }
    } else if (method == COME_METHOD_MANAGER_CONNECT) {
        ManagerConnect msg;
        if (ComeJsonCodec::decodeJsonRpcRequest(json_str, msg, method, id)) {
            handle_manager_connect_jsonrpc2(client_id, msg, id);
        }
    } else if (method == COME_METHOD_MANAGER_EDGE_LIST) {
        ManagerEdgeList msg;
        if (ComeJsonCodec::decodeJsonRpcRequest(json_str, msg, method, id)) {
            handle_device_list_query_jsonrpc2(client_id, msg, id);
        }
    } else if (method == COME_METHOD_MANAGER_EDGE_OFFLINE_LIST) {
        ManagerEdgeOfflineList msg;
        if (ComeJsonCodec::decodeJsonRpcRequest(json_str, msg, method, id)) {
            handle_offline_device_list_query_jsonrpc2(client_id, msg, id);
        }
    } else if (method == COME_METHOD_MANAGER_EDGE_KICK) {
        ManagerEdgeKick msg;
        if (ComeJsonCodec::decodeJsonRpcRequest(json_str, msg, method, id)) {
            handle_kick_device_jsonrpc2(client_id, msg, id);
        }
    } else if (method == COME_METHOD_EDGE_HEARTBEAT) {
        EdgeHeartbeat msg;
        // 心跳是 JSON-RPC 2.0 通知格式（无 id），需要从 params 中解码
        std::string dummy_method;
        int64_t dummy_id;
        if (ComeJsonCodec::decodeJsonRpcNotification(json_str, msg, dummy_method, dummy_id)) {
            ez_printf_info("touch_ingress: [HEARTBEAT] Decoded successfully [edge_id=%s, access_token=%s]\n", 
                   msg.id.c_str(), msg.access_token.substr(0, 20).c_str());
            handle_device_heartbeat_jsonrpc2(client_id, msg);
        } else {
            ez_printf_warning("touch_ingress: [HEARTBEAT] Failed to decode heartbeat from client_id=%d, json=%.200s\n", client_id, json_str.c_str());
            g_DevWsRegisterSvr.CloseClient(client_id);
        }
    } else {
        ez_printf_warning("touch_ingress: Unknown method=%s from client_id=%d, closing connection\n", method.c_str(), client_id);
        g_DevWsRegisterSvr.CloseClient(client_id);
    }
}

// ==============================
// JSON-RPC 2.0 格式处理函数
// ==============================

// 阶段2：处理 Edge → Ingress 的 edge.online 请求
void CFunRegisterSvr::handle_device_online_jsonrpc2(int client_id, const EdgeOnline& msg, int64_t id)
{
    std::string edge_id = msg.id;
    std::string device_key = msg.key;
    std::string edge_type = msg.type;

    ez_printf_debug("touch_ingress: [EDGE.ONLINE] Received edge.online request [client_id=%d, edge_id=%s, type=%s, id=%ld]\n",
           client_id, edge_id.c_str(), edge_type.c_str(), (long)id);

    if (edge_id.empty() || device_key.empty()) {
        // 参数错误
        ez_printf_warning("touch_ingress: [EDGE.ONLINE] Invalid params [client_id=%d, edge_id=%s]\n", client_id, edge_id.c_str());
        std::string err_json = ComeJsonCodec::buildJsonRpcError(-32602, "Invalid params: id and key are required", JsonValue::createInt64(id));
        if (!err_json.empty()) {
            g_DevWsRegisterSvr.SendText(client_id, err_json.c_str(), err_json.length());
        }
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 检查 Manager 是否连接
    if (!IsManagerConnected()) {
        ez_printf_warning("touch_ingress: [EDGE.ONLINE] No manager connected, rejecting edge %s\n", edge_id.c_str());
        std::string err_json = ComeJsonCodec::buildJsonRpcError(40002, "No manager connected", JsonValue::createInt64(id));
        if (!err_json.empty()) {
            g_DevWsRegisterSvr.SendText(client_id, err_json.c_str(), err_json.length());
        }
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 检查是否已有相同 edge_id 的设备连接
    for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
        if (it->second.edge_id == edge_id) {
            // 业务错误码使用正整数
            ez_printf_warning("touch_ingress: [EDGE.ONLINE] Device already connected [edge_id=%s, old_client_id=%d, new_client_id=%d]\n",
                   edge_id.c_str(), it->first, client_id);
            std::string err_json = ComeJsonCodec::buildJsonRpcError(40003, "Device already connected", JsonValue::createInt64(id));
            if (!err_json.empty()) {
                g_DevWsRegisterSvr.SendText(client_id, err_json.c_str(), err_json.length());
            }
            g_DevWsRegisterSvr.CloseClient(client_id);
            return;
        }
    }

    // 验证设备
    bool verified = verify_device(edge_id, device_key);
    if (!verified) {
        ez_printf_warning("touch_ingress: [EDGE.ONLINE] Device verification failed [edge_id=%s, client_id=%d]\n", edge_id.c_str(), client_id);
        std::string err_json = ComeJsonCodec::buildJsonRpcError(40001, "Device verification failed", JsonValue::createInt64(id));
        if (!err_json.empty()) {
            g_DevWsRegisterSvr.SendText(client_id, err_json.c_str(), err_json.length());
        }
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 通知 manager 并等待确认
    int32_t req_id = NotifyManagerEdgeOnline(client_id, edge_id, edge_type, "", "");
    if (req_id == 0) {
        // 没有 manager 连接
        ez_printf_warning("touch_ingress: [EDGE.ONLINE] NotifyManagerEdgeOnline failed, rejecting edge %s\n", edge_id.c_str());
        std::string err_json = ComeJsonCodec::buildJsonRpcError(40004, "Device rejected", JsonValue::createInt64(id));
        if (!err_json.empty()) {
            g_DevWsRegisterSvr.SendText(client_id, err_json.c_str(), err_json.length());
        }
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 标记为等待 manager 确认
    m_online_edges[client_id].pending_manager_confirm = true;

    // Manager 确认，接受设备上线
    // 生成 access_token
    char token_buf[65];
    time_t now = time(NULL);
    snprintf(token_buf, sizeof(token_buf), "%s_%ld", edge_id.c_str(), now);
    std::string access_token(token_buf);

    // 构造成功响应（使用 come.1 库的直接编码接口）
    AckEdgeOnline ack_msg;
    ack_msg.code = 0;
    ack_msg.success = true;
    ack_msg.msg = "Register success";
    ack_msg.access_token = access_token;
    ack_msg.token_type = "Bearer";
    ack_msg.expires_in = 3600;

    std::string resp_json = ComeJsonCodec::encodeJsonRpcResponse(ack_msg, id);
    if (!resp_json.empty()) {
        g_DevWsRegisterSvr.SendText(client_id, resp_json.c_str(), resp_json.length());
    }

    // 保存设备信息和上线时间（保留已有的 public_ip）
    auto edge_it = m_online_edges.find(client_id);
    if (edge_it != m_online_edges.end()) {
        edge_it->second.edge_id = edge_id;
        edge_it->second.edge_type = edge_type;
        edge_it->second.online_time = now;
        edge_it->second.last_heartbeat = now;
        edge_it->second.pending_manager_confirm = false;
        ez_printf_info("touch_ingress: [EDGE.ONLINE] Edge registered successfully [client_id=%d, edge_id=%s, online_edges=%zu]\n",
               client_id, edge_id.c_str(), m_online_edges.size());
    } else {
        EdgeDeviceInfo device_info(edge_id, now);
        device_info.edge_type = edge_type;
        m_online_edges[client_id] = device_info;
    }

    // Edge 重新上线时，清除其离线缓存
    ClearEdgeOfflineCache(edge_id);

    // 调用用户回调
    if (m_on_device_register) {
        m_on_device_register(client_id, edge_id, true, m_user_data);
    }
}

// 阶段1：处理 Edge → Ingress 的 device.heartbeat 通知
void CFunRegisterSvr::handle_device_heartbeat_jsonrpc2(int client_id, const EdgeHeartbeat& msg)
{
    // 心跳是通知，无响应
    // 注意：EdgeHeartbeat 的 JSON 字段名是 id（不是 edge_id）
    std::string edge_id = msg.id;

    // 查找设备是否存在
    auto edge_it = m_online_edges.find(client_id);
    if (edge_it == m_online_edges.end()) {
        // 设备未注册，关闭连接
        ez_printf_warning("touch_ingress: Heartbeat received from unregistered client_id=%d, closing connection\n", client_id);
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 验证设备 ID 是否匹配
    if (edge_it->second.edge_id != edge_id) {
        // 设备 ID 不匹配，关闭连接
        ez_printf_warning("touch_ingress: Heartbeat edge_id mismatch [client_id=%d, expected=%s, got=%s], closing connection\n",
               client_id, edge_it->second.edge_id.c_str(), edge_id.c_str());
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 更新最后心跳时间
    time_t now = time(NULL);
    edge_it->second.last_heartbeat = now;

    // 检查是否等待 Manager 确认上线
    if (edge_it->second.pending_manager_confirm) {
        // Manager 还没确认上线，再次通知
        ez_printf_debug("touch_ingress: Edge %s heartbeat received, but manager hasn't confirmed online yet (pending=%d), re-notifying manager\n",
               edge_id.c_str(), edge_it->second.pending_manager_confirm);
        notify_device_online(edge_id);
    } else {
        ez_printf_debug("touch_ingress: Edge %s heartbeat received (confirmed)\n", edge_id.c_str());
    }

    // 心跳不更新 online_since，保持连续在线时长
}

// 阶段2：处理 Manager → Ingress 的 manager.connect 通知
void CFunRegisterSvr::handle_manager_connect_jsonrpc2(int client_id, const ManagerConnect& msg, int64_t id)
{
    // Manager 连接通知，无响应
    (void)id;  // 通知不需要使用 id

    ez_printf_debug("touch_ingress: [MANAGER.CONNECT] Received manager.connect [client_id=%d, manager_id=%s, ingress_id=%s]\n",
           client_id, msg.manager_id.c_str(), msg.ingress_id.c_str());

    // 如果已有 Manager 连接，拒绝新 Manager
    if (m_manager_client_id != EZ_WS_SERVER_INVALID_CLIENT_ID) {
        ez_printf_warning("touch_ingress: [MANAGER.CONNECT] Manager already connected [existing_client_id=%d], rejecting new manager [client_id=%d, manager_id=%s]\n",
               m_manager_client_id, client_id, msg.manager_id.c_str());
        g_DevWsRegisterSvr.CloseClient(client_id);
        return;
    }

    // 保存 Manager client_id
    m_manager_client_id = client_id;

    ez_printf_info("touch_ingress: [MANAGER.CONNECT] Manager connected successfully [client_id=%d, manager_id=%s, ingress_id=%s]\n",
           client_id, msg.manager_id.c_str(), msg.ingress_id.c_str());

    // 调用用户回调
    if (m_on_device_register) {
        m_on_device_register(client_id, "manager", true, m_user_data);
    }

    // Manager 连接后，根据 EdgeReportMode 上报 Edge 状态
    int report_mode = GetEdgeReportMode();
    ez_printf_debug("touch_ingress: [MANAGER.CONNECT] EdgeReportMode=%d, online_edges=%zu, offline_cache=%zu\n",
           report_mode, m_online_edges.size(), m_offline_edges_cache.size());

    if (report_mode == 1) {
        // 模式 1：上报所有已连接的 Edge
        ez_printf_debug("touch_ingress: [MANAGER.CONNECT] Reporting all %zu existing edges\n", m_online_edges.size());
        for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
            if (!it->second.edge_id.empty()) {
                notify_device_online(it->second.edge_id);
            }
        }
    } else if (report_mode == 2) {
        // 模式 2：仅上报有变化的 Edge（当前实现：上报所有在线 Edge + 缓存的离线事件）
        ez_printf_debug("touch_ingress: [MANAGER.CONNECT] Reporting changed edges (%zu online, %zu cached offline)\n",
               m_online_edges.size(), m_offline_edges_cache.size());
        // 上报在线 Edge
        for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
            if (!it->second.edge_id.empty()) {
                notify_device_online(it->second.edge_id);
            }
        }
        // 上报缓存的离线事件
        ReportCachedOfflineEvents();
    } else {
        // 模式 0：不上报
        ez_printf_debug("touch_ingress: [MANAGER.CONNECT] Edge report mode 0, not reporting any edges\n");
    }
}

// 阶段2：处理 Manager → Ingress 的 device.list 请求
void CFunRegisterSvr::handle_device_list_query_jsonrpc2(int client_id, const ManagerEdgeList& msg, int64_t id)
{
    int offset = msg.offset;
    int limit = msg.limit;

    // 限制 limit 的最大值
    if (limit > 1000) {
        limit = 1000;
    }

    // 构建边缘 ID 列表
    std::vector<std::string> edge_ids;
    int count = 0;
    int skipped = 0;

    for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
        if (it->second.edge_id.empty()) {
            continue;
        }
        if (skipped < offset) {
            skipped++;
            continue;
        }
        if (count >= limit) {
            break;
        }

        edge_ids.push_back(it->second.edge_id);
        count++;
    }

    // 构造响应（使用 come.1 库的直接编码接口）
    AckManagerEdgeList resp_msg;
    resp_msg.edge_ids = edge_ids;
    resp_msg.total = m_online_edges.size();
    resp_msg.offset = offset;
    resp_msg.limit = limit;
    resp_msg.count = count;

    std::string resp_json = ComeJsonCodec::encodeJsonRpcResponse(resp_msg, id);
    if (!resp_json.empty()) {
        g_DevWsRegisterSvr.SendText(client_id, resp_json.c_str(), resp_json.length());
    }
}

// 阶段2：处理 Manager → Ingress 的 offline.edge.list 请求（查询离线边缘列表）
void CFunRegisterSvr::handle_offline_device_list_query_jsonrpc2(int client_id, const ManagerEdgeOfflineList& msg, int64_t id)
{
    int offset = msg.offset;
    int limit = msg.limit;

    // 限制 limit 的最大值
    if (limit > 1000) {
        limit = 1000;
    }

    // 构建离线设备列表
    std::vector<OfflineEdgeInfo> offline_devices;
    int count = 0;
    int skipped = 0;

    for (auto it = m_offline_edges_cache.begin(); it != m_offline_edges_cache.end(); ++it) {
        if (skipped < offset) {
            skipped++;
            continue;
        }
        if (count >= limit) {
            break;
        }

        OfflineEdgeInfo info;
        info.edge_id = it->second.edge_id;
        info.offline_time = it->second.online_time;  // 使用 online_time 作为离线时间
        info.pending_confirm = it->second.pending_manager_confirm;
        info.edge_type = it->second.edge_type;
        offline_devices.push_back(info);
        count++;
    }

    // 构造响应（使用 come.1 库的直接编码接口）
    AckManagerEdgeOfflineList resp_msg;
    resp_msg.offline_devices = offline_devices;
    resp_msg.total = m_offline_edges_cache.size();
    resp_msg.offset = offset;
    resp_msg.limit = limit;
    resp_msg.count = count;

    std::string resp_json = ComeJsonCodec::encodeJsonRpcResponse(resp_msg, id);
    if (!resp_json.empty()) {
        g_DevWsRegisterSvr.SendText(client_id, resp_json.c_str(), resp_json.length());
    }
}

// 阶段2：处理 Manager → Ingress 的 device.kick 请求
void CFunRegisterSvr::handle_kick_device_jsonrpc2(int client_id, const ManagerEdgeKick& msg, int64_t id)
{
    std::string edge_id = msg.edge_id;
    std::string reason = msg.reason.empty() ? "Kicked by manager" : msg.reason;

    if (edge_id.empty()) {
        std::string err_json = ComeJsonCodec::buildJsonRpcError(-32602, "Invalid params", JsonValue::createInt64(id));
        if (!err_json.empty()) {
            g_DevWsRegisterSvr.SendText(client_id, err_json.c_str(), err_json.length());
        }
        return;
    }

    // 先发送确认响应给 manager（使用 come.1 库的直接编码接口）
    AckManagerEdgeKick resp_msg;
    resp_msg.edge_id = edge_id;
    resp_msg.success = true;
    resp_msg.code = 0;
    resp_msg.msg = "Device kicked successfully";

    std::string resp_json = ComeJsonCodec::encodeJsonRpcResponse(resp_msg, id);
    if (!resp_json.empty()) {
        g_DevWsRegisterSvr.SendText(client_id, resp_json.c_str(), resp_json.length());
    }

    // 然后踢掉设备
    kick_device(edge_id);
}

// ==============================
// Edge 上报模式和离线缓存特性实现
// ==============================

void CFunRegisterSvr::CacheEdgeOfflineEvent(const std::string& edge_id, const EdgeDeviceInfo& edge_info)
{
    // 缓存 Edge 离线事件
    m_offline_edges_cache[edge_id] = edge_info;
    ez_printf_debug("touch_ingress: Cached offline event for edge %s at %ld\n", edge_id.c_str(), edge_info.online_time);
}

void CFunRegisterSvr::ClearEdgeOfflineCache(const std::string& edge_id)
{
    // Edge 重新上线时，清除其离线缓存
    auto it = m_offline_edges_cache.find(edge_id);
    if (it != m_offline_edges_cache.end()) {
        m_offline_edges_cache.erase(it);
        ez_printf_debug("touch_ingress: Cleared offline cache for edge %s\n", edge_id.c_str());
    }
}

void CFunRegisterSvr::ReportCachedOfflineEvents()
{
    // 上报缓存的离线事件（等待 Manager 确认）
    if (m_offline_edges_cache.empty()) {
        ez_printf_debug("touch_ingress: No cached offline events to report\n");
        return;
    }

    ez_printf_debug("touch_ingress: Reporting %zu cached offline events (waiting for manager confirm)\n", m_offline_edges_cache.size());
    for (auto it = m_offline_edges_cache.begin(); it != m_offline_edges_cache.end(); ++it) {
        if (!it->second.pending_manager_confirm) {
            notify_device_offline(it->second.edge_id);
            it->second.pending_manager_confirm = true;  // 标记为等待确认
            ez_printf_debug("touch_ingress: Sent offline notification for edge %s, waiting for confirm\n", it->second.edge_id.c_str());
        }
    }
}

void CFunRegisterSvr::HandleManagerOnlineConfirmResponse(const std::string& edge_id)
{
    // 查找对应的在线设备并更新确认状态
    for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
        if (it->second.edge_id == edge_id) {
            it->second.pending_manager_confirm = false;
            ez_printf_debug("touch_ingress: Manager confirmed online for edge %s (client_id=%d)\n", edge_id.c_str(), it->first);
            return;
        }
    }
}

void CFunRegisterSvr::HandleManagerOfflineConfirmResponse(const std::string& edge_id)
{
    // 查找对应的离线缓存并删除
    auto it = m_offline_edges_cache.find(edge_id);
    if (it != m_offline_edges_cache.end()) {
        it->second.pending_manager_confirm = false;
        m_offline_edges_cache.erase(it);
        ez_printf_debug("touch_ingress: Manager confirmed offline for edge %s, removed from cache\n", edge_id.c_str());
    }
}

// ==============================
// 通知和验证函数实现
// ==============================

int32_t CFunRegisterSvr::NotifyManagerEdgeOnline(int client_id, const std::string& edge_id,
                                               const std::string& edge_type,
                                               const std::string& local_ip,
                                               const std::string& public_ip)
{
    // 检查 Manager 是否连接
    if (!IsManagerConnected()) {
        ez_printf_warning("touch_ingress: No manager connected, cannot notify for edge %s\n", edge_id.c_str());
        return 0;
    }

    // 获取 Edge 的公网 IP（从连接时记录的信息）
    std::string edge_public_ip;
    auto edge_it = m_online_edges.find(client_id);
    if (edge_it != m_online_edges.end()) {
        edge_public_ip = edge_it->second.public_ip;
    }

    // 发送 edge.online 请求（需要等待响应）
    time_t online_since = time(NULL);

    // 使用 come.1 库的消息类型
    IngressEdgeOnline msg;
    msg.edge_id = edge_id;
    msg.edge_type = edge_type;
    msg.local_ip = local_ip;
    msg.public_ip = edge_public_ip.empty() ? public_ip : edge_public_ip;  // 优先使用连接时获取的公网 IP
    msg.ingress_id = "";  // TODO: 获取实际的 ingress_id
    msg.online_since = online_since;

    // 生成请求 ID
    int32_t req_id = static_cast<int32_t>(time(NULL) & 0x7FFFFFFF);

    // 构建 JSON-RPC 2.0 请求（使用 come.1 库的直接编码接口）
    std::string req_json = ComeJsonCodec::encodeJsonRpcRequest(msg, COME_METHOD_INGRESS_EDGE_ONLINE, req_id);
    if (!req_json.empty()) {
        g_DevWsRegisterSvr.SendText(m_manager_client_id, req_json.c_str(), req_json.length());
        ez_printf_debug("touch_ingress: Sent edge.online request to manager for edge %s (req_id=%d, client_id=%d, public_ip=%s)\n",
               edge_id.c_str(), req_id, client_id, msg.public_ip.c_str());

        // 记录等待状态
        if (client_id > 0) {
            auto device_it = m_online_edges.find(client_id);
            if (device_it != m_online_edges.end()) {
                device_it->second.pending_manager_confirm = true;
                device_it->second.manager_confirmed = 99;  // 初始为 pending
            }
        }

        return req_id;
    }

    return 0;
}

void CFunRegisterSvr::notify_device_online(const std::string& edge_id)
{
    // 检查 Manager 是否连接
    if (!IsManagerConnected()) {
        ez_printf_warning("touch_ingress: [NOTIFY.ONLINE] No manager connected, cannot notify for edge %s\n", edge_id.c_str());
        return;
    }

    // 获取 Edge 的设备信息和公网 IP
    std::string edge_public_ip;
    std::string edge_type;
    for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
        if (it->second.edge_id == edge_id) {
            edge_public_ip = it->second.public_ip;
            edge_type = it->second.edge_type;
            break;
        }
    }

    // 使用 come.1 库的消息类型
    IngressEdgeOnline msg;
    msg.edge_id = edge_id;
    msg.edge_type = edge_type;
    msg.ingress_id = "";  // TODO: 获取实际的 ingress_id
    msg.public_ip = edge_public_ip;
    msg.online_since = time(NULL);

    // 生成请求 ID
    int32_t req_id = static_cast<int32_t>(time(NULL) & 0x7FFFFFFF);

    // 编码为 JSON-RPC 2.0 请求（有 id，需要等待响应）
    std::string req_json = ComeJsonCodec::encodeJsonRpcRequest(msg, COME_METHOD_INGRESS_EDGE_ONLINE, req_id);
    if (!req_json.empty()) {
        g_DevWsRegisterSvr.SendText(m_manager_client_id, req_json.c_str(), req_json.length());
        ez_printf_debug("touch_ingress: [NOTIFY.ONLINE] Sent to manager [edge_id=%s, edge_type=%s, req_id=%d, manager_client_id=%d]\n",
               edge_id.c_str(), edge_type.c_str(), req_id, m_manager_client_id);
    }
}

void CFunRegisterSvr::notify_device_offline(const std::string& edge_id)
{
    // 检查 Manager 是否连接
    if (!IsManagerConnected()) {
        ez_printf_warning("touch_ingress: [NOTIFY.OFFLINE] No manager connected, cannot notify for edge %s\n", edge_id.c_str());
        return;
    }

    // 使用 come.1 库的消息类型
    IngressEdgeOffline msg;
    msg.edge_id = edge_id;
    msg.ingress_id = "";  // TODO: 获取实际的 ingress_id

    // 生成请求 ID
    int32_t req_id = static_cast<int32_t>(time(NULL) & 0x7FFFFFFF);

    // 编码为 JSON-RPC 2.0 请求（有 id，需要等待响应）
    std::string req_json = ComeJsonCodec::encodeJsonRpcRequest(msg, COME_METHOD_INGRESS_EDGE_OFFLINE, req_id);
    if (!req_json.empty()) {
        // 防止重复发送：检查 Manager 连接是否仍然有效
        if (IsManagerConnected()) {
            g_DevWsRegisterSvr.SendText(m_manager_client_id, req_json.c_str(), req_json.length());
            ez_printf_debug("touch_ingress: [NOTIFY.OFFLINE] Sent to manager [edge_id=%s, req_id=%d, manager_client_id=%d]\n",
                   edge_id.c_str(), req_id, m_manager_client_id);
        }
    }
}

bool CFunRegisterSvr::verify_device(const std::string& edge_id, const std::string& device_key)
{
    // 简单验证：检查 edge_id 和 device_key 是否为空
    if (edge_id.empty() || device_key.empty()) {
        return false;
    }
    // 当前实现：接受所有非空验证
    return true;
}

void CFunRegisterSvr::kick_device(const std::string& edge_id)
{
    // 查找并断开指定设备的连接
    for (auto it = m_online_edges.begin(); it != m_online_edges.end(); ++it) {
        if (it->second.edge_id == edge_id) {
            int client_id = it->first;

            // 发送拒绝消息给设备（如果连接仍然有效）
            AckEdgeOnline ack(40001, false, "Edge rejected by admin");
            std::string ack_json = ComeJsonCodec::encode(ack);
            if (!ack_json.empty()) {
                g_DevWsRegisterSvr.SendText(client_id, ack_json.c_str(), ack_json.length());
            }

            // 关闭连接
            // 注意：CloseClient 会触发 SIGNAL_DISCONNECTED 回调
            // SIGNAL_DISCONNECTED 处理中会：
            //   1. 调用 notify_device_offline（通知 Manager）
            //   2. 从 m_online_edges 中删除该条目
            // 所以这里不需要主动删除，避免 double erase 导致崩溃
            g_DevWsRegisterSvr.CloseClient(client_id);
            ez_printf_info("touch_ingress: [KICK] Kicked edge %s (client_id=%d)\n", edge_id.c_str(), client_id);

            // 不再主动 erase，让 SIGNAL_DISCONNECTED 回调处理
            return;
        }
    }

    // Edge 不在在线列表中，可能已经自然断开
    // 此时 SIGNAL_DISCONNECTED 回调已经处理了离线通知，无需额外操作
    ez_printf_debug("touch_ingress: [KICK] Edge %s not found in online list (may have already disconnected)\n", edge_id.c_str());
}
