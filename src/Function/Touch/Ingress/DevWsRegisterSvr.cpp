/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevWsRegisterSvr.cpp - WebSocket server communication layer implementation
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: DevWsRegisterSvr.cpp $
 *
 *  Explain:
 *     Implementation of WebSocket server communication layer.
 *
 *  Update:
 *     2013-12-03 22:10:15 Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "DevWsRegisterSvr.h"
#include <stdio.h>
#include <string.h>

#include "../../../Logs.h"

PATTERN_SINGLETON_IMPLEMENT(CDevWsRegisterSvr);

CDevWsRegisterSvr::CDevWsRegisterSvr()
    : CEZThread("CDevWsRegisterSvr", THREAD_PRIORITY_DEFAULT)
    , m_ws_handle(NULL)
    , m_SigNotify(2/*SIGNAL_NODE_NEW*/)
    , m_iUser(0)
{
}

CDevWsRegisterSvr::~CDevWsRegisterSvr()
{
    Stop();
}

bool CDevWsRegisterSvr::Start(unsigned short port, const char *protocol, const char *path_prefix)
{
    // 参数检查：端口范围 1-65535
    if (port == 0 || port > 65535) {
        return false;  // port 必须在有效范围内 (1-65535)
    }
    
    if (m_bLoop) {
        return true;  // 已经启动
    }
    
    // 配置WebSocket服务端（按照 ezsocket 库规范）
    struct ez_ws_server_config config = {0};
    config.port = (int)port;  // 转换为底层库需要的 int 类型
    config.protocol = protocol ? protocol : "come.1";
    config.path_prefix = path_prefix ? path_prefix : "/come";
    config.ip = NULL;  // 监听所有接口（NULL 或空串表示 INADDR_ANY）
    config.options = 0;
    
    // 保活配置（按照 ezsocket 库规范和示例代码）
    // 参考：touch-svr-native.c 中的配置
    config.ping_interval_ms = 30000;      // 30秒 ping 间隔
    config.ping_timeout_ms = 10000;       // 10秒 pong 超时（ping_interval/3）
    config.idle_timeout_ms = 180000;      // 180秒空闲超时（6倍 ping_interval）
    config.timer_interval_ms = 3000;      // 3秒定时器检测周期（ping_timeout/3）
    config.ping_jitter_percent = 10;      // 10% ping 间隔抖动
    
    // 设置回调
    struct ez_ws_server_callbacks callbacks = {0};
    callbacks.on_receive = s_on_receive;
    callbacks.on_connected = s_on_connected;
    callbacks.on_disconnected = s_on_disconnected;
    callbacks.user_data = this;
    
    // 创建WebSocket服务端句柄
    m_ws_handle = ez_ws_server_handle_create(&config, &callbacks);
    if (!m_ws_handle) {
        ez_printf_error("touch_ingress: [WS_SERVER] Failed to create WebSocket server handle on port %d\n", port);
        return false;
    }

    ez_printf_info("touch_ingress: [WS_SERVER] WebSocket server handle created on port %d, path=%s, protocol=%s\n",
           port, config.path_prefix, config.protocol);

    // 注意：这里不自动创建线程，线程由 Start(CEZObject*, SignalProc_t) 管理
    // 如果已经有用户注册了信号槽，线程应该已经启动
    return true;
}

EZTHREAD_BOOL CDevWsRegisterSvr::Start()
{
    if (m_bLoop) {
        return EZTHREAD_BOOL_TRUE;
    }
    int ret = CreateThread();
    return ret;
}

EZTHREAD_BOOL CDevWsRegisterSvr::Stop()
{
    if (m_bLoop) {
        // 先停止线程循环
        m_bLoop = EZTHREAD_BOOL_FALSE;
        
        // 等待线程结束（确保 ThreadProc 中的循环退出）
        DestroyThread();
        
        // 清理WebSocket服务端（这会关闭所有连接）
        // 注意：ez_ws_server_cleanup 会自动停止服务并关闭所有连接
        if (m_ws_handle) {
            ez_ws_server_cleanup(m_ws_handle);
            m_ws_handle = NULL;
        }
    }
    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CDevWsRegisterSvr::Start(CEZObject *pObj, DevWsRegisterSvrSignalProc_t pProc)
{
    CEZLock lock(m_MutexSigBuffer);
    
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;
    
    if (m_SigNotify.Attach(pObj, pProc) < 0) {
        return bRet;
    }
    
    if (m_iUser == 0) {
        // 检查WebSocket服务器是否已初始化
        if (!m_ws_handle) {
            // WebSocket服务器未初始化，需要先调用 Start(port, ...)
            m_SigNotify.Detach(pObj, pProc);
            return bRet;
        }
        CreateThread();
        bRet = EZTHREAD_BOOL_TRUE;
    } else {
        bRet = EZTHREAD_BOOL_TRUE;
    }
    m_iUser++;
    
    return bRet;
}

EZTHREAD_BOOL CDevWsRegisterSvr::Stop(CEZObject *pObj, DevWsRegisterSvrSignalProc_t pProc)
{
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;
    
    CEZLock lock(m_MutexSigBuffer);
    
    // 为了避免detach失败而不停止线程，使用者自行注意start和stop成对调用
    if (m_iUser > 0) {
        m_iUser--;
    }
    
    if (m_SigNotify.Detach(pObj, pProc) == 0) {
        bRet = EZTHREAD_BOOL_TRUE;
    }
    
    if (m_iUser == 0) {
        // 先停止线程循环
        if (m_bLoop) {
            m_bLoop = EZTHREAD_BOOL_FALSE;
        }
        
        // 等待线程结束
        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
        
        // 清理WebSocket服务端（这会关闭所有连接）
        // 注意：ez_ws_server_cleanup 会自动停止服务并关闭所有连接
        if (m_ws_handle) {
            ez_ws_server_cleanup(m_ws_handle);
            m_ws_handle = NULL;
        }
    }
    
    return bRet;
}

int CDevWsRegisterSvr::SendText(int client_id, const char *data, size_t len)
{
    if (!m_ws_handle) {
        return -1;
    }
    return ez_ws_server_send_text(m_ws_handle, client_id, data, len);
}

int CDevWsRegisterSvr::SendBinary(int client_id, const void *data, size_t len)
{
    if (!m_ws_handle) {
        return -1;
    }
    return ez_ws_server_send_binary(m_ws_handle, client_id, data, len);
}

bool CDevWsRegisterSvr::IsReady() const
{
    if (!m_ws_handle) {
        return false;
    }
    return ez_ws_server_is_ready(m_ws_handle) != 0;
}

int CDevWsRegisterSvr::GetClientCount() const
{
    if (!m_ws_handle) {
        return 0;
    }
    return ez_ws_server_get_client_count(m_ws_handle);
}

int CDevWsRegisterSvr::CloseClient(int client_id)
{
    if (!m_ws_handle) {
        return -1;
    }
    return ez_ws_server_close_client(m_ws_handle, client_id);
}

void CDevWsRegisterSvr::ThreadProc()
{
    // ezThread 自驱动循环
    // 按照 ezsocket 库规范：循环调用 ez_ws_server_service_exec 处理事件
    // 参考：touch-svr-native.c 中的 ws_server_thread_func
    ez_printf_info("touch_ingress: WebSocket server thread started\n");
    while (m_bLoop) {
        if (m_ws_handle) {
            // 执行一次事件循环迭代
            // timeout_ms 参数被忽略（内部使用智能调度）
            // 返回 0 表示继续运行，-1 表示应该停止
            // 注意：为了快速响应停止信号，使用较小的超时时间（50ms）
            int ret = ez_ws_server_service_exec(m_ws_handle, 50);
            if (ret < 0) {
                // 服务出错，打印日志并退出循环
                ez_printf_error("touch_ingress: ez_ws_server_service_exec returned %d, stopping server loop\n", ret);
                m_bLoop = EZTHREAD_BOOL_FALSE;
                break;
            }
        } else {
            // WebSocket 句柄未初始化，等待
            SystemSleep(50);  // 减少等待时间，提高响应速度
        }
    }

    // 线程退出前，确保资源清理
    // 注意：ez_ws_server_cleanup 会在 Stop() 中调用，这里不需要重复清理
    ez_printf_info("touch_ingress: WebSocket server thread exiting\n");
}

void CDevWsRegisterSvr::s_on_receive(int client_id, const void *data, size_t len, int is_binary, void *user_data)
{
    CDevWsRegisterSvr *self = (CDevWsRegisterSvr *)user_data;
    if (self) {
        CEZLock lock(self->m_MutexSigBuffer);
        // 触发统一信号：SIGNAL_RECEIVE, client_id, data(转为char*), len, is_binary, status(0=正常)
        self->m_SigNotify(SIGNAL_RECEIVE, client_id, (const char*)data, (int)len, is_binary, 0);
    }
}

void CDevWsRegisterSvr::s_on_connected(int client_id, const char *ip, int port, void *user_data)
{
    CDevWsRegisterSvr *self = (CDevWsRegisterSvr *)user_data;
    if (self) {
        ez_printf_info("touch_ingress: WebSocket client connected [client_id=%d, ip=%s, port=%d]\n",
                       client_id, ip ? ip : "unknown", port);
        CEZLock lock(self->m_MutexSigBuffer);
        // 触发统一信号：SIGNAL_CONNECTED, client_id, ip, port, status(0=成功), 0
        self->m_SigNotify(SIGNAL_CONNECTED, client_id, ip ? ip : "", port, 0, 0);
    }
}

void CDevWsRegisterSvr::s_on_disconnected(int client_id, void *user_data)
{
    CDevWsRegisterSvr *self = (CDevWsRegisterSvr *)user_data;
    if (self) {
        ez_printf_info("touch_ingress: WebSocket client disconnected [client_id=%d]\n", client_id);
        CEZLock lock(self->m_MutexSigBuffer);
        // 触发统一信号：SIGNAL_DISCONNECTED, client_id, NULL, error_code(0=正常断开), 0, 0
        self->m_SigNotify(SIGNAL_DISCONNECTED, client_id, NULL, 0, 0, 0);
    }
}

