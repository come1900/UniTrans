/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WsTransport.cpp - 传输服务抽象实现（明文 ws 与 TLS wss）
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 *  Explain:
 *     CWsPlainTransport：封装明文 ez_ws_server_*（端口 54321）。
 *     CWssTransport：封装 TLS ez_wss_server_*（端口 54443，默认双开）。
 *     每个实例独立一个线程跑 service_exec，互不拖累。
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "WsTransport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "ez_wsserver-native.h"
#ifdef _FUNC_TouchEdge_EnableTls
#include "ez_wss-server-native.h"
#endif

/* ======================================================================
 * 通用：服务线程入口（调用虚方法 RunLoop，由子类实现）
 * ====================================================================== */

namespace {
struct RunCtx {
    CWsTransportSvc *svc;
};
void *transport_thread_entry(void *arg)
{
    RunCtx *ctx = (RunCtx *)arg;
    CWsTransportSvc *svc = ctx->svc;
    delete ctx;
    svc->RunLoop();
    return NULL;
}
} // namespace

/* ======================================================================
 * 明文传输 CWsPlainTransport
 * ====================================================================== */

class CWsPlainTransport : public CWsTransportSvc {
public:
    CWsPlainTransport() : m_handle(NULL), m_thread(0), m_running(false) {}

    virtual bool Start() override {
        if (m_running)
            return true;

        struct ez_ws_server_config config;
        memset(&config, 0, sizeof(config));
        config.port = m_cfg.port;
        config.protocol = m_cfg.protocol.c_str();
        config.path_prefix = m_cfg.path_prefix.c_str();
        config.ip = NULL;
        config.options = 0;
        config.ping_interval_ms = m_cfg.ping_interval_ms;
        config.ping_timeout_ms = m_cfg.ping_timeout_ms;
        config.idle_timeout_ms = m_cfg.idle_timeout_ms;
        config.timer_interval_ms = m_cfg.timer_interval_ms;
        config.ping_jitter_percent = m_cfg.ping_jitter_percent;

        struct ez_ws_server_callbacks cb;
        memset(&cb, 0, sizeof(cb));
        cb.on_receive = s_on_receive;
        cb.on_connected = s_on_connected;
        cb.on_disconnected = s_on_disconnected;
        cb.user_data = this;

        m_handle = ez_ws_server_handle_create(&config, &cb);
        if (!m_handle)
            return false;

        m_running = true;
        RunCtx *ctx = new RunCtx;
        ctx->svc = this;
        if (pthread_create(&m_thread, NULL, transport_thread_entry, ctx) != 0) {
            m_running = false;
            ez_ws_server_cleanup(m_handle);
            m_handle = NULL;
            return false;
        }
        return true;
    }

    virtual void Stop() override {
        if (!m_running)
            return;
        m_running = false;
        if (m_thread) {
            pthread_join(m_thread, NULL);
            m_thread = 0;
        }
        if (m_handle) {
            ez_ws_server_cleanup(m_handle);
            m_handle = NULL;
        }
    }

    virtual bool IsReady() const override {
        return m_handle && ez_ws_server_is_ready(m_handle) != 0;
    }

    virtual int GetClientCount() const override {
        return m_handle ? ez_ws_server_get_client_count(m_handle) : 0;
    }

    virtual int SendText(int gid, const char *data, size_t len) override {
        if (!m_handle)
            return -1;
        if (gid == -1)
            return ez_ws_server_send_text(m_handle, EZ_WS_SERVER_BROADCAST_ALL, data, len);
        int local = ToLocalId(gid);
        if (local < 0)
            return -1;
        return ez_ws_server_send_text(m_handle, local, data, len);
    }

    virtual int SendBinary(int gid, const void *data, size_t len) override {
        if (!m_handle)
            return -1;
        if (gid == -1)
            return ez_ws_server_send_binary(m_handle, EZ_WS_SERVER_BROADCAST_ALL, data, len);
        int local = ToLocalId(gid);
        if (local < 0)
            return -1;
        return ez_ws_server_send_binary(m_handle, local, data, len);
    }

    virtual int CloseClient(int gid) override {
        if (!m_handle)
            return -1;
        int local = ToLocalId(gid);
        if (local < 0)
            return -1;
        return ez_ws_server_close_client(m_handle, local);
    }

    /* 服务线程主循环 */
    void RunLoop() {
        while (m_running) {
            if (m_handle) {
                if (ez_ws_server_service_exec(m_handle, 50) < 0) {
                    m_running = false;
                    break;
                }
            } else {
                usleep(50000);
            }
        }
    }

private:
    struct ez_ws_server_handle *m_handle;
    pthread_t m_thread;
    volatile bool m_running;

    static void s_on_receive(int client_id, const void *data, size_t len, int is_binary, void *ud) {
        CWsPlainTransport *self = (CWsPlainTransport *)ud;
        if (self && self->on_receive)
            self->on_receive(self->ToGlobalId(client_id), (const char *)data, len, is_binary);
    }
    static void s_on_connected(int client_id, const char *ip, int port, void *ud) {
        CWsPlainTransport *self = (CWsPlainTransport *)ud;
        if (self && self->on_connected)
            self->on_connected(self->ToGlobalId(client_id), ip ? ip : "", port);
    }
    static void s_on_disconnected(int client_id, void *ud) {
        CWsPlainTransport *self = (CWsPlainTransport *)ud;
        if (self && self->on_disconnected)
            self->on_disconnected(self->ToGlobalId(client_id), 0);
    }
};

/* ======================================================================
 * TLS 传输 CWssTransport
 * ====================================================================== */

#ifdef _FUNC_TouchEdge_EnableTls
class CWssTransport : public CWsTransportSvc {
public:
    CWssTransport() : m_handle(NULL), m_thread(0), m_running(false) {}

    virtual bool Start() override {
        if (m_running)
            return true;

        struct ez_wss_server_config config;
        memset(&config, 0, sizeof(config));
        config.port = m_cfg.port;
        config.protocol = m_cfg.protocol.c_str();
        config.path_prefix = m_cfg.path_prefix.c_str();
        config.ip = NULL;
        config.options = 0;
        config.ping_interval_ms = m_cfg.ping_interval_ms;
        config.ping_timeout_ms = m_cfg.ping_timeout_ms;
        config.idle_timeout_ms = m_cfg.idle_timeout_ms;
        config.timer_interval_ms = m_cfg.timer_interval_ms;
        config.ping_jitter_percent = m_cfg.ping_jitter_percent;

        config.tls_enable = m_cfg.tls_enable;
        config.tls_cert_path = m_cfg.tls_cert_path.empty() ? NULL : m_cfg.tls_cert_path.c_str();
        config.tls_key_path = m_cfg.tls_key_path.empty() ? NULL : m_cfg.tls_key_path.c_str();
        config.tls_ca_path = m_cfg.tls_ca_path.empty() ? NULL : m_cfg.tls_ca_path.c_str();

        struct ez_ws_server_callbacks cb;
        memset(&cb, 0, sizeof(cb));
        cb.on_receive = s_on_receive;
        cb.on_connected = s_on_connected;
        cb.on_disconnected = s_on_disconnected;
        cb.user_data = this;

        m_handle = ez_wss_server_handle_create(&config, &cb);
        if (!m_handle)
            return false;

        m_running = true;
        RunCtx *ctx = new RunCtx;
        ctx->svc = this;
        if (pthread_create(&m_thread, NULL, transport_thread_entry, ctx) != 0) {
            m_running = false;
            ez_wss_server_cleanup(m_handle);
            m_handle = NULL;
            return false;
        }
        return true;
    }

    virtual void Stop() override {
        if (!m_running)
            return;
        m_running = false;
        if (m_thread) {
            pthread_join(m_thread, NULL);
            m_thread = 0;
        }
        if (m_handle) {
            ez_wss_server_cleanup(m_handle);
            m_handle = NULL;
        }
    }

    virtual bool IsReady() const override {
        return m_handle && ez_wss_server_is_ready(m_handle) != 0;
    }

    virtual int GetClientCount() const override {
        return m_handle ? ez_wss_server_get_client_count(m_handle) : 0;
    }

    virtual int SendText(int gid, const char *data, size_t len) override {
        if (!m_handle)
            return -1;
        if (gid == -1)
            return ez_wss_server_send_text(m_handle, EZ_WS_SERVER_BROADCAST_ALL, data, len);
        int local = ToLocalId(gid);
        if (local < 0)
            return -1;
        return ez_wss_server_send_text(m_handle, local, data, len);
    }

    virtual int SendBinary(int gid, const void *data, size_t len) override {
        if (!m_handle)
            return -1;
        if (gid == -1)
            return ez_wss_server_send_binary(m_handle, EZ_WS_SERVER_BROADCAST_ALL, data, len);
        int local = ToLocalId(gid);
        if (local < 0)
            return -1;
        return ez_wss_server_send_binary(m_handle, local, data, len);
    }

    virtual int CloseClient(int gid) override {
        if (!m_handle)
            return -1;
        int local = ToLocalId(gid);
        if (local < 0)
            return -1;
        return ez_wss_server_close_client(m_handle, local);
    }

    /* 服务线程主循环 */
    void RunLoop() {
        while (m_running) {
            if (m_handle) {
                if (ez_wss_server_service_exec(m_handle, 50) < 0) {
                    m_running = false;
                    break;
                }
            } else {
                usleep(50000);
            }
        }
    }

private:
    struct ez_wss_server_handle *m_handle;
    pthread_t m_thread;
    volatile bool m_running;

    static void s_on_receive(int client_id, const void *data, size_t len, int is_binary, void *ud) {
        CWssTransport *self = (CWssTransport *)ud;
        if (self && self->on_receive)
            self->on_receive(self->ToGlobalId(client_id), (const char *)data, len, is_binary);
    }
    static void s_on_connected(int client_id, const char *ip, int port, void *ud) {
        CWssTransport *self = (CWssTransport *)ud;
        if (self && self->on_connected)
            self->on_connected(self->ToGlobalId(client_id), ip ? ip : "", port);
    }
    static void s_on_disconnected(int client_id, void *ud) {
        CWssTransport *self = (CWssTransport *)ud;
        if (self && self->on_disconnected)
            self->on_disconnected(self->ToGlobalId(client_id), 0);
    }
};
#endif /* _FUNC_TouchEdge_EnableTls */

/* ======================================================================
 * 工厂：创建/销毁传输实例（按 tls_enable 选择明文或 TLS）
 * ====================================================================== */

CWsTransportSvc *WsTransportCreate(const TransportConfig &cfg)
{
#ifdef _FUNC_TouchEdge_EnableTls
    CWsTransportSvc *svc = cfg.tls_enable ? (CWsTransportSvc *)(new CWssTransport)
                                          : (CWsTransportSvc *)(new CWsPlainTransport);
#else
    // wss 未编译（未定义 _FUNC_TouchEdge_EnableTls），始终回落明文传输
    CWsTransportSvc *svc = (CWsTransportSvc *)(new CWsPlainTransport);
#endif
    if (svc)
        svc->AssignConfig(cfg);
    return svc;
}

void WsTransportDestroy(CWsTransportSvc *svc)
{
    if (svc) {
        svc->Stop();
        delete svc;
    }
}
