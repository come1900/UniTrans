/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentMinerMtService.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: AgentMinerMtService.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentMinerMtService_H
#define _AgentMinerMtService_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigMinerMtService.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

// 反应堆
#include "HandlerMinerMtService.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerMinerMtService.h"

#define g_AgentMinerMtService (*CAgentMinerMtService::instance())

class CAgentMinerMtService : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentMinerMtService);
    CAgentMinerMtService();
    virtual ~CAgentMinerMtService();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

    void Connect();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void Disconnect();

    void Run();
  	
    CHandlerMinerMtService *GetHandler();

private:
    //AgentMinerMtServiceCfg_T m_ConfigOfMinerMtService;
    unsigned short m_sSerPort;
    CHandlerMinerMtService *m_pHandlerMinerMtService;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerMinerMtService> *m_pCHttpServerMinerMtService;

}
; //CAgentMinerMtService
#endif //_AgentMinerMtService_H
