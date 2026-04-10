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
