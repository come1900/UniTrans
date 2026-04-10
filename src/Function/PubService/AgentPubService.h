/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPubService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPubService.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentPubService_H
#define _AgentPubService_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerPubService.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerPubService.h"

typedef struct __AgentPubServiceCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentPubServiceCfg_T;

#define g_AgentPubService (*CAgentPubService::instance())

class CAgentPubService : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentPubService);
    CAgentPubService();
    virtual ~CAgentPubService();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    void SetConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void ConnectAll();

    void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    int Send2Socket(std::string strSocketName, const char *pData, size_t Len);
    	
    CHandlerPubService *GetHandler();

private:
    AgentPubServiceCfg_T m_ConfigOfPubService;
    CHandlerPubService *m_pHandlerPubService;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerPubService> *m_pCHttpServerPubService;

}
; //CAgentPubService
#endif //_AgentPubService_H
