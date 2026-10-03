/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTouchEdge.h - Touch Edge Configuration
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTouchEdge.h 1 2026-03-15 Create $
 *
 *  Explain:
 *     Configuration for Touch Edge client
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigTouchEdge_H__
#define __ConfigTouchEdge_H__

#include "ConfigBase.h"

#define def_Default_TouchEdge_Name  "TE"//// strlen("[{}]")
#define def_Min_TouchEdge_Name_Length strlen(def_Default_TouchEdge_Name)+1 // 让默认的失效

//!
typedef struct tagConfigTouchEdge
{
    std::string strIngressHost;  // Ingress 服务器地址
    int iIngressPort;            // Ingress 服务器端口

    // wss/TLS 配置（wss 翻切：默认启用，连 ingress wss 端口）
    int iWssEnable;              // 0=明文 ws，1=TLS wss（默认 1）
    int iWssPort;                // wss 端口（默认 54443，iWssEnable=1 时使用）
    int iTlsVerifyPeer;          // wss 时是否校验服务端证书（1=默认，内置 CA 互认；0=仅加密）
    std::string strTlsCaPath;    // wss 校验服务端所用 CA（空=内置/系统 CA）

    std::string strEdgeId;     // 设备 ID
    std::string strEdgeKey;    // 设备密钥
    std::string strEdgeType;   // 设备类型

    // 重连配置
    int iReconnectInterval;      // 重连间隔 (sec)
    int iReconnectMaxRetries;    // 最大重连次数 (-1 表示无限重连)

    // 心跳配置 (sec)
    int iHeartbeatInterval;

    // 连接超时 (sec)
    int iConnectTimeout;
}ConfigTouchEdge;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigTouchEdge, 1, 4> CConfigTouchEdge;

#endif //__ConfigTouchEdge_H__
