/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentTouchEdge.h - Touch Edge Agent
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentTouchEdge.h 1 2026-03-15 Create $
 *
 *  Explain:
 *     Touch Edge Agent - integrates touch_edge as a component
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentTouchEdge_H
#define _AgentTouchEdge_H

#include "EZThread.h"
#include "EZSignals.h"

#include <time.h>
// ʹsms stack
// #include "../../Stack/SmsStack.h"

#include "../../../Maintain/GeneralAgentLog.h"

#include "../../../Configs/ConfigTouchEdge.h"

#include "FunRegisterCli.h"

typedef struct __AgentTouchEdgeCfg
{
    std::string IngressHost;
    unsigned short IngressPort;
    std::string EdgeId;
    std::string EdgeKey;
    std::string EdgeType;
    int iConnTimeOut;

    __AgentTouchEdgeCfg()
    {
        IngressHost = "127.0.0.1";
        IngressPort = 54321;
        EdgeId = "edge001";
        EdgeKey = "key001";
        EdgeType = "touch";
        iConnTimeOut = 5;
    }
}
AgentTouchEdgeCfg_T;

typedef enum __msg_TouchEdge_t {
    MSG_TOUCHEDGE_T_CONNECT_SUCCEEDED = 800,
    MSG_TOUCHEDGE_T_CONNECT_TRYING,
    MSG_TOUCHEDGE_T_CONNECT_TRY_FAILED,
    MSG_TOUCHEDGE_T_CONNECT_FAILED,
} MSG_TouchEdge_T;

#define g_AgentTouchEdge (*CAgentTouchEdge::instance())

class CAgentTouchEdge : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentTouchEdge);
    CAgentTouchEdge();
    virtual ~CAgentTouchEdge();

    // cmd, partition, len, time
    typedef TSignal2<int, void *> TouchEdgeSignal_t;
    typedef TouchEdgeSignal_t::SigProc TouchEdgeSignalProc_t;

    EZTHREAD_BOOL Start(CEZObject * pObj, TouchEdgeSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, TouchEdgeSignalProc_t pProc);

    void ThreadProc();
    void SetConfig();

    // 设置配置
    void SetIngressHost(const std::string& host);
    void SetIngressPort(unsigned short port);
    void SetEdgeId(const std::string& edgeId);
    void SetEdgeKey(const std::string& edgeKey);
    void SetEdgeType(const std::string& edgeType);
    
    // 获取 Handler
    CFunRegisterCli *GetFunRegisterCli();

private:
    AgentTouchEdgeCfg_T m_Config;
    CFunRegisterCli *m_pFunRegisterCli;
    CMaintainAgentLog m_Log;

    TouchEdgeSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;
    
    int m_iUser;

    CConfigTouchEdge *m_pCfgTouchEdge;
}
; //CAgentTouchEdge
#endif //_AgentTouchEdge_H
