/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentKeyManagementService.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: AgentKeyManagementService.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentKeyManagementService_H
#define _AgentKeyManagementService_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigKeyManagementService.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

// 反应堆
#include "HandlerKeyManagementService.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerKeyManagementService.h"

#define g_AgentKeyManagementService (*CAgentKeyManagementService::instance())

class CAgentKeyManagementService : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentKeyManagementService);
    CAgentKeyManagementService();
    virtual ~CAgentKeyManagementService();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

    void Connect();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void Disconnect();

    void Run();
  	
    CHandlerKeyManagementService *GetHandler();

private:
    //AgentKeyManagementServiceCfg_T m_ConfigOfKeyManagementService;
    unsigned short m_sSerPort;
    CHandlerKeyManagementService *m_pHandlerKeyManagementService;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerKeyManagementService> *m_pCHttpServerKeyManagementService;

}
; //CAgentKeyManagementService
#endif //_AgentKeyManagementService_H
