/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTouchIngress.h - Touch Ingress Configuration
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTouchIngress.h 1 2026-03-15 Create $
 *
 *  Explain:
 *     Configuration for Touch Ingress server
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigTouchIngress_H__
#define __ConfigTouchIngress_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigTouchIngress
{
    std::string strHost;          // 监听地址 (0.0.0.0 表示所有接口)
    int iPort;                    // 监听端口

    // WebSocket 配置
    std::string strProtocol;      // WebSocket 协议 (默认 come.1)
    std::string strPathPrefix;    // WebSocket 路径前缀 (默认/come)

    // 连接状态维护/ 心跳检测 (sec)
    // 0- 不检测
    // >0 检测周期
    int CheckPeriod;

    // 超时时间 (sec)
    int PeerTimeout;

    // 最大连接数
    int iMaxConnections;

    // Edge 上报模式（Manager 连接后）
    // 0 - 不上报
    // 1 - 上报所有已连接的 Edge（默认）
    // 2 - 仅上报有变化的 Edge
    int EdgeReportMode;
}
ConfigTouchIngress;

//1 -结构配置项
//4 -观测用配置项
typedef TConfig<ConfigTouchIngress, 1, 4> CConfigTouchIngress;

#endif //__ConfigTouchIngress_H__
