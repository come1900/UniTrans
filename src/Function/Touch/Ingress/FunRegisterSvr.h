/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunRegisterSvr.h - device registration function layer (server)
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: FunRegisterSvr.h $
 *
 *  Explain:
 *     Device registration function layer for server side. Based on devWsRegister
 *     layer, implements business logic, maintains state, and calls device function
 *     interfaces. Handles device registration requests and validates device credentials.
 *
 *  Update:
 *     2013-09-08 18:45:12 Create
 *     2026-02-25 Add heartbeat support
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef FUN_REGISTER_SVR_H
#define FUN_REGISTER_SVR_H

#include <string>
#include <map>
#include <ctime>
#include "EZObject.h"
#include "DevWsRegisterSvr.h"
#include "come_1.h"
#include "come.1.json.h"

// 客户端类型枚举（放在 DeviceInfo 之前，避免编译错误）
enum ClientType {
    CLIENT_TYPE_UNKNOWN = 0,
    CLIENT_TYPE_EDGE = 1,      // Edge 设备
    CLIENT_TYPE_MANAGER = 2    // Manager 管理端
};

// Edge 设备信息结构（仅用于 Edge 设备）
struct EdgeDeviceInfo {
    std::string edge_id;
    time_t last_heartbeat;      // 最后心跳时间
    time_t online_time;         // 上线时间（UTC 秒）
    std::string public_ip;      // 边缘公网 IP（从 WebSocket 连接获取）
    std::string edge_type;      // 边缘类型

    // Manager 确认状态
    bool pending_manager_confirm;   // 是否等待 Manager 确认上线（true=等待中，false=已确认/无需确认）
    int manager_confirmed;          // Manager 返回的 confirmed 状态（99=pending, 1=confirmed, 0=rejected）

    EdgeDeviceInfo() : last_heartbeat(0), online_time(0),
                       pending_manager_confirm(false), manager_confirmed(99) {}
    EdgeDeviceInfo(const std::string& id, time_t t)
        : edge_id(id), last_heartbeat(t), online_time(t),
          pending_manager_confirm(false), manager_confirmed(99) {}
};

// Manager 连接的 client_id（Ingress 只能有一个 Manager）
// 当新 Manager 连接时，旧连接会被断开

class CFunRegisterSvr : public CEZObject
{
public:
    CFunRegisterSvr();
    virtual ~CFunRegisterSvr();

    // 启动/停止
    void Start(unsigned short port, const char *protocol = "come.1", const char *path_prefix = "/come");
    void Stop();

    // 注册回调：设备注册成功/失败（保留兼容性）
    typedef void (*OnDeviceRegisterProc_t)(int client_id, const std::string& edge_id,
                                            bool success, void *user_data);
    void SetRegisterCallback(OnDeviceRegisterProc_t proc, void *user_data);

    // 获取客户端类型
    ClientType GetClientType(int client_id) const;

    // 检查 Manager 是否连接
    bool IsManagerConnected() const { return m_manager_client_id != EZ_WS_SERVER_INVALID_CLIENT_ID; }

    // 获取 Manager client_id
    int GetManagerClientId() const { return m_manager_client_id; }

    // 获取 Edge 上报模式
    int GetEdgeReportMode() const { return m_EdgeReportMode; }

    // 设置 Edge 上报模式（支持动态配置）
    void SetEdgeReportMode(int mode) { m_EdgeReportMode = mode; }

private:
    bool m_started;
    OnDeviceRegisterProc_t m_on_device_register;
    void *m_user_data;

    // 在线 Edge 设备信息（client_id -> EdgeDeviceInfo）
    // 保存当前有 WebSocket 连接的所有 Edge 设备
    std::map<int, EdgeDeviceInfo> m_online_edges;

    // 离线 Edge 设备信息缓存（edge_id -> EdgeDeviceInfo）
    // 用于记录没有 Manager 连接时下线的设备
    // Manager 上线后根据 EdgeReportMode 上报这些离线事件
    std::map<std::string, EdgeDeviceInfo> m_offline_edges_cache;

    // Manager 连接的 client_id（Ingress 只能有一个 Manager）
    int m_manager_client_id;

    // Edge 上报模式（Manager 连接后）
    // 0 - 不上报
    // 1 - 上报所有已连接的 Edge（默认）
    // 2 - 仅上报有变化的 Edge
    int m_EdgeReportMode;

    // 缓存 Edge 离线事件
    void CacheEdgeOfflineEvent(const std::string& edge_id, const EdgeDeviceInfo& edge_info);

    // 清除 Edge 离线缓存（Edge 重新上线时）
    void ClearEdgeOfflineCache(const std::string& edge_id);

    // 上报缓存的离线事件（Manager 连接后调用）
    void ReportCachedOfflineEvents();

    // 通知 manager 边缘上线（内部实现，直接使用 m_manager_client_id）
    int32_t NotifyManagerEdgeOnline(int client_id, const std::string& edge_id,
                                     const std::string& edge_type,
                                     const std::string& local_ip,
                                     const std::string& public_ip);

    // WebSocket 统一信号槽处理函数
    void OnWebsocketNotify(CDevWsRegisterSvr::SignalType sig_type, int client_id, const char *str_param, int int_param1, int int_param2, int int_param3);

    void handle_receive(int client_id, const char *data, size_t len);

    // JSON-RPC 2.0 请求处理（直接使用消息结构体）
    void handle_device_online_jsonrpc2(int client_id, const EdgeOnline& msg, int64_t id);
    void handle_device_list_query_jsonrpc2(int client_id, const ManagerEdgeList& msg, int64_t id);
    void handle_offline_device_list_query_jsonrpc2(int client_id, const ManagerEdgeOfflineList& msg, int64_t id);
    void handle_kick_device_jsonrpc2(int client_id, const ManagerEdgeKick& msg, int64_t id);
    void handle_manager_connect_jsonrpc2(int client_id, const ManagerConnect& msg, int64_t id);

    // JSON-RPC 2.0 通知处理（直接使用消息结构体）
    void handle_device_heartbeat_jsonrpc2(int client_id, const EdgeHeartbeat& msg);

    // 通知 manager 设备上线/下线（ingress 是业务纽带，负责通知 manager）
    void notify_device_online(const std::string& edge_id);
    void notify_device_offline(const std::string& edge_id);

    // 处理 manager 返回的 online/offline 确认响应
    void HandleManagerOnlineConfirmResponse(const std::string& edge_id);
    void HandleManagerOfflineConfirmResponse(const std::string& edge_id);

    // 验证设备（简单实现：检查 key 是否匹配）
    bool verify_device(const std::string& edge_id, const std::string& device_key);

    // 踢掉指定设备（被 manager 调用）
    void kick_device(const std::string& edge_id);
};

#endif // FUN_REGISTER_SVR_H
