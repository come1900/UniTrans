/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevWsRegisterCli.cpp - WebSocket client communication layer implementation
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 * $Id: DevWsRegisterCli.cpp $
 *
 *  Explain:
 *     Implementation of WebSocket client communication layer.
 *
 *  Update:
 *     2013-08-05 20:38:27 Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "DevWsRegisterCli.h"
#include <stdio.h>
#include <string.h>

#include "../../../Logs.h"

CDevWsRegisterCli* CDevWsRegisterCli::instance()
{
    // Meyers 单例模式：使用局部静态变量，C++11 及以后线程安全
    static CDevWsRegisterCli s_instance;
    return &s_instance;
}

CDevWsRegisterCli::CDevWsRegisterCli()
    : CEZThread("CDevWsRegisterCli", THREAD_PRIORITY_DEFAULT)
    , m_ws_handle(NULL)
#ifdef _FUNC_TouchEdge_EnableTls
    , m_wss_handle(NULL)
#endif
    , m_tls_enable(true)
    , m_tls_verify_peer(true)
    , m_SigNotify(2/*SIGNAL_NODE_NEW*/)
    , m_iUser(0)
{
}

CDevWsRegisterCli::~CDevWsRegisterCli()
{
    Stop();
}

bool CDevWsRegisterCli::Start(const char *server_addr, unsigned short port,
                              const char *url_path, const char *protocol,
                              int reconnect_max_retries,
                              int tls_enable, int tls_verify_peer,
                              const char *tls_ca_path)
{
    // 参数检查
    if (!server_addr || server_addr[0] == '\0') {
        return false;  // server_addr 不能为空
    }

    if (port == 0 || port > 65535) {
        return false;  // port 必须在有效范围内 (1-65535)
    }

    if (m_bLoop) {
        return true;  // 已经启动
    }

    m_tls_enable = tls_enable != 0;
    m_tls_verify_peer = tls_verify_peer != 0;
    m_tls_ca_path = tls_ca_path ? tls_ca_path : "";

#ifndef _FUNC_TouchEdge_EnableTls
    // wss 未编译（未定义 _FUNC_TouchEdge_EnableTls），强制使用明文 ws
    m_tls_enable = false;
    m_tls_verify_peer = false;
#endif

    const char *urlp = url_path ? url_path : "/come";
    const char *protop = protocol ? protocol : "come.1";

    // 设置回调（明文与 wss 共用同类型回调结构）
    struct ez_ws_callbacks callbacks = {0};
    callbacks.on_receive = s_on_receive;
    callbacks.on_connected = s_on_connected;
    callbacks.on_disconnected = s_on_disconnected;
    callbacks.user_data = this;

    if (m_tls_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
        // ---------- TLS wss 客户端 ----------
        struct ez_wss_client_config config = {0};
        config.server_addr = server_addr;
        config.port = port;
        config.url_path = urlp;
        config.protocol = protop;
        config.connect_timeout_ms = 5000;
        config.reconnect_interval_ms = 3000;
        config.reconnect_max_retries = reconnect_max_retries;
        config.reconnect_backoff_enable = 1;
        config.reconnect_backoff_min_retries = 2;
        config.reconnect_backoff_high_threshold = 5;
        config.tls_enable = 1;
        config.tls_verify_peer = m_tls_verify_peer ? 1 : 0;
        config.tls_ca_path = m_tls_ca_path.empty() ? NULL : m_tls_ca_path.c_str();

        m_wss_handle = ez_wss_client_handle_create(&config, &callbacks);
        if (!m_wss_handle) {
            return false;
        }

        if (CreateThread() != EZTHREAD_BOOL_TRUE) {
            ez_wss_client_cleanup(m_wss_handle);
            m_wss_handle = NULL;
            return false;
        }
#else
        // wss 未编译，不应进入此分支（m_tls_enable 已被强制为 false）
        return false;
#endif
    } else {
        // ---------- 明文 ws 客户端 ----------
        struct ez_ws_client_config config = {0};
        config.server_addr = server_addr;
        config.port = port;
        config.url_path = urlp;
        config.protocol = protop;
        config.connect_timeout_ms = 5000;
        config.reconnect_interval_ms = 3000;
        config.reconnect_max_retries = reconnect_max_retries;
        config.reconnect_backoff_enable = 1;
        config.reconnect_backoff_min_retries = 2;
        config.reconnect_backoff_high_threshold = 5;

        m_ws_handle = ez_ws_client_handle_create(&config, &callbacks);
        if (!m_ws_handle) {
            return false;
        }

        if (CreateThread() != EZTHREAD_BOOL_TRUE) {
            ez_ws_client_cleanup(m_ws_handle);
            m_ws_handle = NULL;
            return false;
        }
    }

    return true;
}

bool CDevWsRegisterCli::Stop()
{
    if (!m_bLoop) {
        return true;
    }

    m_bLoop = EZTHREAD_BOOL_FALSE;
    DestroyThread(EZTHREAD_BOOL_TRUE);

    // 清理 WebSocket 客户端（按 tls_enable 选择明文或 wss）
    if (m_tls_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
        if (m_wss_handle) {
            ez_wss_client_cleanup(m_wss_handle);
            m_wss_handle = NULL;
        }
#endif
    } else {
        if (m_ws_handle) {
            ez_ws_client_cleanup(m_ws_handle);
            m_ws_handle = NULL;
        }
    }

    return true;
}

bool CDevWsRegisterCli::Start(CEZObject *pObj, DevWsRegisterCliSignalProc_t pProc)
{
    CEZLock lock(m_MutexSigBuffer);

    int ret = m_SigNotify.Attach(pObj, pProc);
    if (ret >= 0 && m_iUser == 0) {
        // 第一次注册信号，确保 WebSocket 已启动
        // 注意：这里不自动启动，需要先调用 Start(server_addr, port)
        m_iUser++;
    }

    return ret >= 0;
}

bool CDevWsRegisterCli::Stop(CEZObject *pObj, DevWsRegisterCliSignalProc_t pProc)
{
    CEZLock lock(m_MutexSigBuffer);

    if (m_iUser > 0) {
        m_iUser--;
    }

    int ret = m_SigNotify.Detach(pObj, pProc);
    return ret == 0;
}

int CDevWsRegisterCli::SendText(const char *data, size_t len)
{
    if (m_tls_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
        if (!m_wss_handle) {
            return -1;
        }
        return ez_wss_send_text(m_wss_handle, data, len);
#else
        return -1;
#endif
    }
    if (!m_ws_handle) {
        return -1;
    }
    return ez_ws_send_text(m_ws_handle, data, len);
}

int CDevWsRegisterCli::SendBinary(const void *data, size_t len)
{
    if (m_tls_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
        if (!m_wss_handle) {
            return -1;
        }
        return ez_wss_send_binary(m_wss_handle, data, len);
#else
        return -1;
#endif
    }
    if (!m_ws_handle) {
        return -1;
    }
    return ez_ws_send_binary(m_ws_handle, data, len);
}

bool CDevWsRegisterCli::IsConnected() const
{
    if (m_tls_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
        if (!m_wss_handle) {
            return false;
        }
        return ez_wss_is_connected(m_wss_handle) != 0;
#else
        return false;
#endif
    }
    if (!m_ws_handle) {
        return false;
    }
    return ez_ws_is_connected(m_ws_handle) != 0;
}

void CDevWsRegisterCli::ThreadProc()
{
    // ezThread 自驱动循环
    while (m_bLoop) {
        if (m_tls_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
            if (m_wss_handle) {
                int ret = ez_wss_service_exec(m_wss_handle, 100);
                if (ret < 0) {
                    break;
                }
            } else {
                SystemSleep(100);  // 等待 100ms
            }
#else
            SystemSleep(100);  // wss 未编译，不应进入此分支
#endif
        } else {
            if (m_ws_handle) {
                int ret = ez_ws_service_exec(m_ws_handle, 100);
                if (ret < 0) {
                    break;
                }
            } else {
                SystemSleep(100);  // 等待 100ms
            }
        }
    }
}

void CDevWsRegisterCli::s_on_receive(const void *data, size_t len, int is_binary, void *user_data)
{
    CDevWsRegisterCli *self = (CDevWsRegisterCli *)user_data;
    if (self) {
        CEZLock lock(self->m_MutexSigBuffer);
        // 触发统一信号：SIGNAL_RECEIVE, data, len, is_binary, status(0=正常)
        self->m_SigNotify(SIGNAL_RECEIVE, data, len, is_binary, 0);
    }
}

void CDevWsRegisterCli::s_on_connected(void *user_data)
{
    CDevWsRegisterCli *self = (CDevWsRegisterCli *)user_data;
    if (self) {
        ez_printf_info("WebSocket connected to ingress\n");
        CEZLock lock(self->m_MutexSigBuffer);
        // 触发统一信号：SIGNAL_CONNECTED, NULL, 0, status(0=成功), 0
        self->m_SigNotify(SIGNAL_CONNECTED, NULL, 0, 0, 0);
    }
}

void CDevWsRegisterCli::s_on_disconnected(void *user_data)
{
    CDevWsRegisterCli *self = (CDevWsRegisterCli *)user_data;
    if (self) {
        ez_printf_warning("WebSocket disconnected from ingress\n");
        CEZLock lock(self->m_MutexSigBuffer);
        // 触发统一信号：SIGNAL_DISCONNECTED, NULL, 0, error_code(0=正常断开), 0
        self->m_SigNotify(SIGNAL_DISCONNECTED, NULL, 0, 0, 0);
    }
}
