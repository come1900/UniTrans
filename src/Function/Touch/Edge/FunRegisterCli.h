/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunRegisterCli.h - device registration function layer (client)
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: FunRegisterCli.h $
 *
 *  Explain:
 *     Device registration function layer for client side. Based on devWsRegister
 *     layer, implements business logic, maintains state, and calls device function
 *     interfaces. Supports automatic device registration on power-on, restart, and
 *     reconnection scenarios.
 *
 *  Update:
 *     2013-07-15 19:23:45 Create
 *     2026-02-25 Add heartbeat support
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef FUN_REGISTER_CLI_H
#define FUN_REGISTER_CLI_H

#include <string>
#include "EZObject.h"
#include "DevWsRegisterCli.h"
#include "EZTimer.h"
#include "come_1.h"
#include "come.1.json.h"

class CFunRegisterCli : public CEZObject
{
public:
    CFunRegisterCli(const std::string& edge_id, const std::string& edge_key,
                    const std::string& edge_type = "");
    virtual ~CFunRegisterCli();

    // 启动/停止
    void Start(const char *server_addr, unsigned short port, int reconnect_max_retries = 0);
    void Stop();

    // 更新边缘参数（允许在配置加载后重新设置）
    void UpdateEdgeParams(const std::string& edge_id, const std::string& edge_key,
                            const std::string& edge_type = "");

    // 注册回调：边缘注册成功/失败（保留兼容性）
    typedef void (*OnRegisterResultProc_t)(bool success, const std::string& msg, void *user_data);
    void SetRegisterCallback(OnRegisterResultProc_t proc, void *user_data);

    // 重连失败回调：WebSocket 重连失败后需要重新向 manager 注册
    typedef void (*OnReconnectFailedProc_t)(void *user_data);
    void SetReconnectFailedCallback(OnReconnectFailedProc_t proc, void *user_data);

    // 手动触发注册（上电注册或重启注册）
    bool RegisterEdge(bool is_power_on = true);  // true=上电注册，false=重启注册

    // 检查是否需要重新注册（重连失败后）
    bool NeedReregister() const;

    // 启动/停止心跳
    void StartHeartbeat();
    void StopHeartbeat();

private:
    std::string m_edge_id;
    std::string m_edge_key;
    std::string m_edge_type;
    std::string m_token;  // 从注册响应中获取的 access_token

    bool m_started;
    OnRegisterResultProc_t m_on_register_result;
    OnReconnectFailedProc_t m_on_reconnect_failed;
    void *m_user_data;
    bool m_need_reregister;  // 标记是否需要重新向 manager 注册
    bool m_registered;       // 标记是否已注册成功
    CEZTimer m_heartbeat_timer;  // 心跳定时器
    static const int HEARTBEAT_INTERVAL_SEC = 30;  // 心跳间隔（秒）
    static const std::string FRPC_CONFIG_DIR;  // frpc 配置目录
    std::string get_frpc_config_path() const;  // 该 edge 的 frpc 配置文件路径(按 edge_id 区分)

    // WebSocket 统一信号槽处理函数
    void OnWebsocketNotify(CDevWsRegisterCli::SignalType sig_type, const void *data, size_t len, int param1, int param2);

    void handle_receive(const void *data, size_t len);  // frpc 远程配置处理（第一阶段实现）
    bool write_frpc_config_file(const std::string& jsonStr);
    void send_config_ack_success(int64_t req_id);
    void send_config_ack_error(int64_t req_id, int32_t code, const std::string& msg);
    void send_config_query_error(int64_t req_id, int32_t code, const std::string& msg);
    void send_message(const char *data, size_t len);

    void handle_register_ack_jsonrpc2(const AckEdgeOnline& ack);  // JSON-RPC 2.0 格式
    void handle_register_ack_jsonrpc2_error(int32_t error_code, const std::string& error_msg);  // JSON-RPC 2.0 错误

    // 心跳超时回调
    void on_heartbeat_timeout(EZTHREAD_PARAM wParam);
    void send_heartbeat();
};

#endif // FUN_REGISTER_CLI_H
