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
 *     2026-09-30 双实例接入：明文 + TLS wss 两个传输实例，client_id 全局合并。
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

// 明文与 wss 的 id_base（与 spec-design-tls-websocket.md 一致）
static const int kPlainIdBase = 0;
#ifdef _FUNC_TouchEdge_EnableTls
static const int kWssIdBase   = 1000000;
#endif

CDevWsRegisterSvr::CDevWsRegisterSvr()
    : CEZThread("CDevWsRegisterSvr", THREAD_PRIORITY_DEFAULT)
    , m_plain(NULL)
    , m_wss(NULL)
    , m_wss_enable(false)
    , m_wss_port(0)
    , m_SigNotify(2/*SIGNAL_NODE_NEW*/)
    , m_iUser(0)
{
}

CDevWsRegisterSvr::~CDevWsRegisterSvr()
{
    Stop();
}

void CDevWsRegisterSvr::SetWssConfig(bool enable, unsigned short port,
                                     const std::string &cert_path,
                                     const std::string &key_path,
                                     const std::string &ca_path)
{
    m_wss_enable = enable;
    m_wss_port = port;
    m_wss_cert_path = cert_path;
    m_wss_key_path = key_path;
    m_wss_ca_path = ca_path;

#ifndef _FUNC_TouchEdge_EnableTls
    // wss 未编译（未定义 _FUNC_TouchEdge_EnableTls），强制关闭 wss
    m_wss_enable = false;
#endif
}

bool CDevWsRegisterSvr::Start(unsigned short port, const char *protocol, const char *path_prefix)
{
    if (m_plain) {
        return true;  // 已经启动
    }

    const char *proto = protocol ? protocol : "come.1";
    const char *prefix = path_prefix ? path_prefix : "/come";

    // ---------- 明文实例 ----------
    TransportConfig plain_cfg;
    plain_cfg.port = port;
    plain_cfg.protocol = proto;
    plain_cfg.path_prefix = prefix;
    plain_cfg.tls_enable = 0;
    plain_cfg.id_base = kPlainIdBase;

    m_plain = WsTransportCreate(plain_cfg);
    if (!m_plain) {
        ez_printf_error("[WS_SERVER] Failed to create plain transport\n");
        return false;
    }
    m_plain->on_receive = [this](int gid, const char *d, size_t l, int b) { OnPlainReceive(gid, d, l, b); };
    m_plain->on_connected = [this](int gid, const char *ip, int pt) { OnPlainConnected(gid, ip, pt); };
    m_plain->on_disconnected = [this](int gid, int c) { OnPlainDisconnected(gid, c); };

    if (!m_plain->Start()) {
        ez_printf_error("[WS_SERVER] Failed to start plain transport on port %d\n", port);
        WsTransportDestroy(m_plain);
        m_plain = NULL;
        return false;
    }
    ez_printf_info("[WS_SERVER] Plain server started on port %d, path=%s, protocol=%s\n",
           port, prefix, proto);

    // ---------- TLS wss 实例（可选，默认关闭） ----------
    if (m_wss_enable) {
#ifdef _FUNC_TouchEdge_EnableTls
        if (m_wss_port == 0 || m_wss_port > 65535) {
            ez_printf_error("[WSS_SERVER] Invalid wss port %d, wss disabled\n", m_wss_port);
            m_wss_enable = false;
        } else {
            TransportConfig wss_cfg;
            wss_cfg.port = m_wss_port;
            wss_cfg.protocol = proto;
            wss_cfg.path_prefix = prefix;
            wss_cfg.tls_enable = 1;
            wss_cfg.tls_cert_path = m_wss_cert_path;
            wss_cfg.tls_key_path = m_wss_key_path;
            wss_cfg.tls_ca_path = m_wss_ca_path;
            wss_cfg.id_base = kWssIdBase;

            m_wss = WsTransportCreate(wss_cfg);
            if (!m_wss) {
                ez_printf_error("[WSS_SERVER] Failed to create wss transport\n");
            } else {
                m_wss->on_receive = [this](int gid, const char *d, size_t l, int b) { OnWssReceive(gid, d, l, b); };
                m_wss->on_connected = [this](int gid, const char *ip, int pt) { OnWssConnected(gid, ip, pt); };
                m_wss->on_disconnected = [this](int gid, int c) { OnWssDisconnected(gid, c); };

                if (!m_wss->Start()) {
                    ez_printf_error("[WSS_SERVER] Failed to start wss transport on port %d\n", m_wss_port);
                    WsTransportDestroy(m_wss);
                    m_wss = NULL;
                    m_wss_enable = false;
                } else {
                    ez_printf_info("[WSS_SERVER] WSS server started on port %d, path=%s, protocol=%s (id_base=%d)\n",
                           m_wss_port, prefix, proto, kWssIdBase);
                }
            }
        }
#else
        // wss 未编译，不应进入此分支（SetWssConfig 已强制 m_wss_enable=false）
        m_wss_enable = false;
#endif
    }

    return true;
}

EZTHREAD_BOOL CDevWsRegisterSvr::Start()
{
    // 双实例各自拥有事件循环线程，本层不再自建线程；保留兼容返回
    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CDevWsRegisterSvr::Stop()
{
    if (m_plain) {
        WsTransportDestroy(m_plain);
        m_plain = NULL;
    }
    if (m_wss) {
        WsTransportDestroy(m_wss);
        m_wss = NULL;
    }
    m_wss_enable = false;
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
        // 检查传输是否已初始化
        if (!m_plain) {
            m_SigNotify.Detach(pObj, pProc);
            return bRet;
        }
        // 线程由各传输实例自行管理，这里不 CreateThread
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

    // 为了避免detach失败而不停止，使用者自行注意start和stop成对调用
    if (m_iUser > 0) {
        m_iUser--;
    }

    if (m_SigNotify.Detach(pObj, pProc) == 0) {
        bRet = EZTHREAD_BOOL_TRUE;
    }

    if (m_iUser == 0) {
        // 停止并清理两个传输实例（会 join 各自线程）
        if (m_plain) {
            WsTransportDestroy(m_plain);
            m_plain = NULL;
        }
        if (m_wss) {
            WsTransportDestroy(m_wss);
            m_wss = NULL;
        }
        m_wss_enable = false;
    }

    return bRet;
}

bool CDevWsRegisterSvr::BelongsTo(const CWsTransportSvc *svc, int gid)
{
    if (!svc)
        return false;
    int local = gid - svc->GetIdBase();
    return local >= 0;
}

CWsTransportSvc *CDevWsRegisterSvr::RouteByGlobalId(int gid) const
{
    // 广播走明文（业务层未用 -1 广播，双实例下广播语义归明文）
    if (gid == EZ_WS_SERVER_BROADCAST_ALL)
        return m_plain;
    if (BelongsTo(m_wss, gid))
        return m_wss;
    if (BelongsTo(m_plain, gid))
        return m_plain;
    return NULL;
}

int CDevWsRegisterSvr::SendText(int client_id, const char *data, size_t len)
{
    CWsTransportSvc *svc = RouteByGlobalId(client_id);
    if (!svc)
        return -1;
    return svc->SendText(client_id, data, len);
}

int CDevWsRegisterSvr::SendBinary(int client_id, const void *data, size_t len)
{
    CWsTransportSvc *svc = RouteByGlobalId(client_id);
    if (!svc)
        return -1;
    return svc->SendBinary(client_id, data, len);
}

bool CDevWsRegisterSvr::IsReady() const
{
    if (m_plain && m_plain->IsReady())
        return true;
    if (m_wss && m_wss->IsReady())
        return true;
    return false;
}

int CDevWsRegisterSvr::GetClientCount() const
{
    int count = 0;
    if (m_plain)
        count += m_plain->GetClientCount();
    if (m_wss)
        count += m_wss->GetClientCount();
    return count;
}

int CDevWsRegisterSvr::CloseClient(int client_id)
{
    CWsTransportSvc *svc = RouteByGlobalId(client_id);
    if (!svc)
        return -1;
    return svc->CloseClient(client_id);
}

void CDevWsRegisterSvr::ThreadProc()
{
    // 双实例各自拥有独立事件循环线程（见 CWsTransportSvc::Start 内部 pthread_create），
    // 本方法不再承担事件循环，仅占位。若被误用（CreateThread），退化为睡眠。
    ez_printf_info("CDevWsRegisterSvr::ThreadProc called but transport threads are self-managed\n");
    while (m_bLoop) {
        SystemSleep(100);
    }
}

void CDevWsRegisterSvr::OnPlainReceive(int gid, const char *data, size_t len, int is_binary)
{
    CEZLock lock(m_MutexSigBuffer);
    m_SigNotify(SIGNAL_RECEIVE, gid, data, (int)len, is_binary, 0);
}

void CDevWsRegisterSvr::OnPlainConnected(int gid, const char *ip, int port)
{
    ez_printf_info("WebSocket client connected [client_id=%d, ip=%s, port=%d, transport=plain]\n",
                   gid, ip ? ip : "unknown", port);
    CEZLock lock(m_MutexSigBuffer);
    m_SigNotify(SIGNAL_CONNECTED, gid, ip ? ip : "", port, 0, 0);
}

void CDevWsRegisterSvr::OnPlainDisconnected(int gid, int code)
{
    ez_printf_info("WebSocket client disconnected [client_id=%d, transport=plain]\n", gid);
    CEZLock lock(m_MutexSigBuffer);
    m_SigNotify(SIGNAL_DISCONNECTED, gid, NULL, code, 0, 0);
}

void CDevWsRegisterSvr::OnWssReceive(int gid, const char *data, size_t len, int is_binary)
{
    CEZLock lock(m_MutexSigBuffer);
    m_SigNotify(SIGNAL_RECEIVE, gid, data, (int)len, is_binary, 0);
}

void CDevWsRegisterSvr::OnWssConnected(int gid, const char *ip, int port)
{
    ez_printf_info("WebSocket client connected [client_id=%d, ip=%s, port=%d, transport=wss]\n",
                   gid, ip ? ip : "unknown", port);
    CEZLock lock(m_MutexSigBuffer);
    m_SigNotify(SIGNAL_CONNECTED, gid, ip ? ip : "", port, 0, 0);
}

void CDevWsRegisterSvr::OnWssDisconnected(int gid, int code)
{
    ez_printf_info("WebSocket client disconnected [client_id=%d, transport=wss]\n", gid);
    CEZLock lock(m_MutexSigBuffer);
    m_SigNotify(SIGNAL_DISCONNECTED, gid, NULL, code, 0, 0);
}
