/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WsTransport.h - 传输服务抽象（明文 ws 与 TLS wss 双实例）
 *
 * Copyright (C) 2013 - ezlibs.com -, All Rights Reserved.
 *
 *  Explain:
 *     将 ingress 的 WebSocket 服务从"单实例明文"扩展为"明文 + TLS 并存"。
 *     CWsTransportSvc 为抽象接口，CWsPlainTransport 封装明文 ez_ws_server_*，
 *     CWssTransport 封装 TLS ez_wss_server_*。每个实例独立一个线程跑 service_exec。
 *
 *     client_id 合并到单一 id 空间：每个实例持 id_base，上抛/接收时用
 *     gid = id_base + 原生 id，保证跨实例全局唯一，业务层（CFunRegisterSvr）
 *     无需感知传输来自明文还是 TLS。
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef WS_TRANSPORT_H
#define WS_TRANSPORT_H

#include <string>
#include <functional>
#include <pthread.h>

/* 传输配置 */
struct TransportConfig {
    unsigned short port = 54321;
    std::string protocol = "come.1";
    std::string path_prefix = "/come";

    /* TLS（明文实例忽略这些字段） */
    int tls_enable = 0;               /* 0/1，监听级 */
    std::string tls_cert_path;        /* 证书链，空 → 内置 CA 互认 */
    std::string tls_key_path;         /* 私钥 */
    std::string tls_ca_path;          /* 可选：双向认证 client CA */

    /* client_id 合并到单一 id 空间的基数 */
    int id_base = 0;

    /* 保活（与现有 CDevWsRegisterSvr 一致） */
    uint32_t ping_interval_ms = 30000;
    uint32_t ping_timeout_ms = 10000;
    uint32_t idle_timeout_ms = 180000;
    uint32_t timer_interval_ms = 3000;
    uint32_t ping_jitter_percent = 10;
};

/* 传输服务抽象基类 */
class CWsTransportSvc {
public:
    virtual ~CWsTransportSvc() {}

    /* 生命周期 */
    virtual bool Start() = 0;         /* 创建句柄并启动服务线程 */
    virtual void Stop() = 0;          /* 停止线程并清理句柄 */

    /* 状态 */
    virtual bool IsReady() const = 0;
    virtual int GetClientCount() const = 0;

    /* 发送/关闭（gid = 全局 id，已含 id_base） */
    virtual int SendText(int gid, const char *data, size_t len) = 0;
    virtual int SendBinary(int gid, const void *data, size_t len) = 0;
    virtual int CloseClient(int gid) = 0;

    /* 回调（client_id 均为全局 id = id_base + 原生 id）
     * on_receive: gid, data, len, is_binary
     * on_connected: gid, ip, port
     * on_disconnected: gid, error_code */
    std::function<void(int, const char*, size_t, int)> on_receive;
    std::function<void(int, const char*, int)> on_connected;
    std::function<void(int, int)> on_disconnected;

    int GetIdBase() const { return m_cfg.id_base; }
    int GetPort() const { return m_cfg.port; }

    /* 配置注入（在 Start() 之前调用） */
    void AssignConfig(const TransportConfig &cfg) { m_cfg = cfg; }

    /* 服务线程主循环（由 Start 启动的线程调用，子类实现各自的 service_exec） */
    virtual void RunLoop() = 0;

protected:
    TransportConfig m_cfg;

    /* 内部：将外部全局 id 校验/转换为本实例原生 id；非本实例范围返回 -1 */
    int ToLocalId(int gid) const {
        int local = gid - m_cfg.id_base;
        if (local < 0)
            return -1;
        return local;
    }
    int ToGlobalId(int local) const { return m_cfg.id_base + local; }
};

/* 工厂：按 cfg.tls_enable 创建明文或 TLS 传输实例 */
CWsTransportSvc *WsTransportCreate(const TransportConfig &cfg);
void WsTransportDestroy(CWsTransportSvc *svc);

#endif /* WS_TRANSPORT_H */
