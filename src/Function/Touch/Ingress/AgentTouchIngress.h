/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentTouchIngress.h - Touch Ingress Agent
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentTouchIngress.h 1 2026-03-15 Create $
 *
 *  Explain:
 *     Touch Ingress Agent - integrates touch_ingress as a component
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentTouchIngress_H
#define _AgentTouchIngress_H

#include "EZThread.h"
#include "EZSignals.h"

#include <time.h>

#include "../../../Maintain/GeneralAgentLog.h"
#include "FunRegisterSvr.h"

typedef struct __AgentTouchIngressCfg
{
    std::string strHost;          // 监听地址
    unsigned short Port;          // 监听端口

    // WebSocket 配置
    std::string strProtocol;      // WebSocket 协议
    std::string strPathPrefix;    // WebSocket 路径前缀

    int iConnTimeOut;             // 连接超时

    // Edge 上报模式（Manager 连接后）
    // 0 - 不上报
    // 1 - 上报所有已连接的 Edge（默认）
    // 2 - 仅上报有变化的 Edge
    int EdgeReportMode;

    __AgentTouchIngressCfg()
    {
        strHost = "0.0.0.0";
        Port = 54321;
        strProtocol = "come.1";
        strPathPrefix = "/come";
        iConnTimeOut = 5;
        EdgeReportMode = 1;  // 默认：上报所有已连接的 Edge
    }
}
AgentTouchIngressCfg_T;

typedef enum __msg_TouchIngress_t {
    MSG_TOUCHINGRESS_T_CONNECT_SUCCEEDED = 800,
    MSG_TOUCHINGRESS_T_CONNECT_TRYING,
    MSG_TOUCHINGRESS_T_CONNECT_TRY_FAILED,
    MSG_TOUCHINGRESS_T_CONNECT_FAILED,
} MSG_TouchIngress_T;

#define g_AgentTouchIngress (*CAgentTouchIngress::instance())

class CAgentTouchIngress : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentTouchIngress);
    CAgentTouchIngress();
    virtual ~CAgentTouchIngress();

    // cmd, partition, len, time
    typedef TSignal2<int, void *> TouchIngressSignal_t;
    typedef TouchIngressSignal_t::SigProc TouchIngressSignalProc_t;

    EZTHREAD_BOOL Start(CEZObject * pObj, TouchIngressSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, TouchIngressSignalProc_t pProc);

    void ThreadProc();
    void SetConfig();

    // 设置端口
    void SetPort(unsigned short port);
    
    // 设置 WebSocket 协议和路径
    void SetProtocol(const std::string& protocol);
    void SetPathPrefix(const std::string& pathPrefix);

    // 获取 Handler
    CFunRegisterSvr *GetFunRegisterSvr();

private:
    AgentTouchIngressCfg_T m_Config;
    CFunRegisterSvr *m_pFunRegisterSvr;
    CMaintainAgentLog m_Log;

    TouchIngressSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;
    
    int m_iUser;
}
; //CAgentTouchIngress
#endif //_AgentTouchIngress_H
