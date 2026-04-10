/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentAssOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentAssOverHttp.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentAssOverHttp_H
#define _AgentAssOverHttp_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerAssOverHttp.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerAssOverHttp.h"

typedef struct __AgentAssOverHttpCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentAssOverHttpCfg_T;

#define g_AgentAssOverHttp (*CAgentAssOverHttp::instance())

class CAgentAssOverHttp : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentAssOverHttp);
    CAgentAssOverHttp();
    virtual ~CAgentAssOverHttp();
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
    	
    CHandlerAssOverHttp *GetHandler();

private:
    AgentAssOverHttpCfg_T m_ConfigOfAssOverHttp;
    CHandlerAssOverHttp *m_pHandlerAssOverHttp;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerAssOverHttp> *m_pCHttpServerAssOverHttp;

}
; //CAgentAssOverHttp
#endif //_AgentAssOverHttp_H
