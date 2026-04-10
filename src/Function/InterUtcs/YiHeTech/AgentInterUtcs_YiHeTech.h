/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentInterUtcs_YiHeTech.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentInterUtcs_YiHeTech.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 16:36:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentInterUtcs_YiHeTech_H
#define _AgentInterUtcs_YiHeTech_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerInterUtcs_YiHeTech.h"
#include "../../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerInterUtcs_YiHeTech.h"

typedef struct __AgentInterUtcs_YiHeTechCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentInterUtcs_YiHeTechCfg_T;

#define g_AgentInterUtcs_YiHeTech (*CAgentInterUtcs_YiHeTech::instance())

class CAgentInterUtcs_YiHeTech : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentInterUtcs_YiHeTech);
    CAgentInterUtcs_YiHeTech();
    virtual ~CAgentInterUtcs_YiHeTech();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
    void SetDefaultConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void ConnectAll();

    void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    int SendBySocketName(std::string strSocketName, const char *pData, size_t Len);

    int SendBySession(const std::string &strServerSID, const std::string &stack, const std::string &matchRequest="");
    	
    CHandlerInterUtcs_YiHeTech *GetHandler();

private:
    AgentInterUtcs_YiHeTechCfg_T m_ConfigOfInterUtcs_YiHeTech;
    CHandlerInterUtcs_YiHeTech *m_pHandlerInterUtcs_YiHeTech;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerInterUtcs_YiHeTech> *m_pCHttpServerInterUtcs_YiHeTech;

}
; //CAgentInterUtcs_YiHeTech
#endif //_AgentInterUtcs_YiHeTech_H
