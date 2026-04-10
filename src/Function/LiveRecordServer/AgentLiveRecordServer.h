/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveRecordServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveRecordServer.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentLiveRecordServer_H
#define _AgentLiveRecordServer_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerLiveRecordServer.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerLiveRecordServer.h"

typedef struct __AgentLiveRecordServerCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentLiveRecordServerCfg_T;

#define g_AgentLiveRecordServer (*CAgentLiveRecordServer::instance())

class CAgentLiveRecordServer : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentLiveRecordServer);
    CAgentLiveRecordServer();
    virtual ~CAgentLiveRecordServer();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
    void SetDefaultConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void Connect();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void Disconnect();

    void Run();

    //int SendBySocketName(std::string strSocketName, const char *pData, size_t Len);

    //int SendBySession(const std::string &strServerSID, const std::string &stack, const std::string &matchRequest="");
    	
    CHandlerLiveRecordServer *GetHandler();

private:
    AgentLiveRecordServerCfg_T m_ConfigOfLiveRecordServer;
    CHandlerLiveRecordServer *m_pHandlerLiveRecordServer;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerLiveRecordServer> *m_pCHttpServerLiveRecordServer;

}
; //CAgentLiveRecordServer
#endif //_AgentLiveRecordServer_H
