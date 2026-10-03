/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevWsRegisterSvr.h - WebSocket server communication layer (ezThread self-driven)
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: DevWsRegisterSvr.h $
 *
 *  Explain:
 *     WebSocket server communication layer implementation. Provides reliable data
 *     communication link and notifies upper layer about data and connection status.
 *     Based on ezThread self-driven entity for automatic connection management.
 *
 *     2026-09-30 双实例接入：本层持有明文 + TLS wss 两个传输实例（CWsTransportSvc），
 *     client_id 合并到单一 id 空间（明文 id_base=0，wss id_base=1000000），
 *     上抛给业务层（CFunRegisterSvr）的 client_id 均为全局唯一 id。
 *
 *  Update:
 *     2013-11-12 18:20:40 Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef DEV_WS_REGISTER_SVR_H
#define DEV_WS_REGISTER_SVR_H

#include <string>
#include "EZThread.h"
#include "EZSignals.h"
#include <api_ezthread.h>

#include "ez_wsserver-native.h"
#include "WsTransport.h"

#define g_DevWsRegisterSvr (*CDevWsRegisterSvr::instance())

class CDevWsRegisterSvr : public CEZThread
{
public:
    // 单例模式
    PATTERN_SINGLETON_DECLARE(CDevWsRegisterSvr);

    // 信号类型枚举
    enum SignalType {
        SIGNAL_RECEIVE = 0,      // 数据接收信号: sig_type, client_id, data, len, is_binary, status
        SIGNAL_CONNECTED = 1,    // 连接建立信号: sig_type, client_id, ip_str, port, status, 0
        SIGNAL_DISCONNECTED = 2  // 连接断开信号: sig_type, client_id, NULL, error_code, 0, 0
    };

    // 统一信号类型定义
    // 参数: SignalType sig_type, int client_id, const char *str_param, int int_param1, int int_param2, int int_param3
    // RECEIVE: sig_type=SIGNAL_RECEIVE, client_id=客户端ID(全局id), str_param=数据指针(转为char*), int_param1=len, int_param2=is_binary, int_param3=status
    // CONNECTED: sig_type=SIGNAL_CONNECTED, client_id=客户端ID(全局id), str_param=ip, int_param1=port, int_param2=status, int_param3=0
    // DISCONNECTED: sig_type=SIGNAL_DISCONNECTED, client_id=客户端ID(全局id), str_param=NULL, int_param1=error_code, int_param2=0, int_param3=0
    typedef TSignal6<SignalType, int, const char *, int, int, int> DevWsRegisterSvrSignal_t;
    typedef DevWsRegisterSvrSignal_t::SigProc DevWsRegisterSvrSignalProc_t;

    // 启动/停止（无参版本，保留兼容）
    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    // 启动明文 WebSocket 服务（端口、协议、路径前缀）
    bool Start(unsigned short port, const char *protocol = "come.1", const char *path_prefix = "/come");

    // 配置 TLS wss 实例（在 Start 之前调用；不调用或 enable=0 则不启动 wss）
    void SetWssConfig(bool enable, unsigned short port,
                      const std::string &cert_path = "",
                      const std::string &key_path = "",
                      const std::string &ca_path = "");

    // 注册信号槽（统一接口）
    EZTHREAD_BOOL Start(CEZObject *pObj, DevWsRegisterSvrSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject *pObj, DevWsRegisterSvrSignalProc_t pProc);

    // 发送消息（client_id 为全局 id：明文 [0,1000000)，wss [1000000,∞)）
    int SendText(int client_id, const char *data, size_t len = 0);  // client_id=-1表示广播
    int SendBinary(int client_id, const void *data, size_t len);

    // 获取状态
    bool IsReady() const;
    int GetClientCount() const;

    // 主动关闭指定客户端连接（client_id 为全局 id）
    int CloseClient(int client_id);

    // ezThread 自驱动循环（双实例各自拥有独立事件循环线程，本方法不再承担事件循环）
    void ThreadProc();

private:
    CDevWsRegisterSvr();
    virtual ~CDevWsRegisterSvr();

    // 禁止拷贝
    CDevWsRegisterSvr(const CDevWsRegisterSvr&);
    CDevWsRegisterSvr& operator=(const CDevWsRegisterSvr&);

    // 按全局 id 路由到对应传输实例；超出所有实例范围返回 NULL
    CWsTransportSvc *RouteByGlobalId(int gid) const;
    // 判断全局 id 是否属于指定实例（id_base 段内）
    static bool BelongsTo(const CWsTransportSvc *svc, int gid);

    // 两个传输实例：明文 + TLS wss
    CWsTransportSvc *m_plain;
    CWsTransportSvc *m_wss;

    // wss 配置（Start 前由 SetWssConfig 注入）
    bool m_wss_enable;
    unsigned short m_wss_port;
    std::string m_wss_cert_path;
    std::string m_wss_key_path;
    std::string m_wss_ca_path;

    // 统一信号槽
    DevWsRegisterSvrSignal_t m_SigNotify;
    // 注意：必须使用递归锁（MUTEX_RECURSIVE）
    // 原因：回调链中可能二次进入（如回调内调用 CloseClient 触发断开回调），
    //       非递归锁同线程二次加锁会产生死锁，导致服务线程永久阻塞。
    CEZMutex m_MutexSigBuffer{MUTEX_RECURSIVE};

    // 计数
    int m_iUser;

    // 传输实例回调（转发到统一信号槽，client_id 已由传输层合并为全局 id）
    void OnPlainReceive(int gid, const char *data, size_t len, int is_binary);
    void OnPlainConnected(int gid, const char *ip, int port);
    void OnPlainDisconnected(int gid, int code);
    void OnWssReceive(int gid, const char *data, size_t len, int is_binary);
    void OnWssConnected(int gid, const char *ip, int port);
    void OnWssDisconnected(int gid, int code);
};

#endif // DEV_WS_REGISTER_SVR_H
