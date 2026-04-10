/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentWebService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentWebService.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentWebService_H
#define _AgentWebService_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerWebService.h"
#include "../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerWebService.h"

typedef struct __AgentWebServiceCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentWebServiceCfg_T;

#define g_AgentWebService (*CAgentWebService::instance())

class CAgentWebService : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentWebService);
    CAgentWebService();
    virtual ~CAgentWebService();
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

    int Send2Socket(std::string strSocketName, const char *pData, size_t Len);
    	
    CHandlerWebService *GetHandler();

private:
    AgentWebServiceCfg_T m_ConfigGeneralAgent;
    CHandlerWebService *m_pHandlerWebService;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerWebService> *m_pCHttpServerWebService;

}
; //CAgentWebService
#endif //_AgentWebService_H
