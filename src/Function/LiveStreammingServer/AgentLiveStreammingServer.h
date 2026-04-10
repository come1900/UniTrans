/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveStreammingServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveStreammingServer.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentLiveStreammingServer_H
#define _AgentLiveStreammingServer_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerLiveStreammingServer.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerLiveStreammingServer.h"

typedef struct __AgentLiveStreammingServerCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentLiveStreammingServerCfg_T;

#define g_AgentLiveStreammingServer (*CAgentLiveStreammingServer::instance())

class CAgentLiveStreammingServer : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentLiveStreammingServer);
    CAgentLiveStreammingServer();
    virtual ~CAgentLiveStreammingServer();
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
    	
    CHandlerLiveStreammingServer *GetHandler();

private:
    AgentLiveStreammingServerCfg_T m_ConfigOfLiveStreammingServer;
    CHandlerLiveStreammingServer *m_pHandlerLiveStreammingServer;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerLiveStreammingServer> *m_pCHttpServerLiveStreammingServer;

}
; //CAgentLiveStreammingServer
#endif //_AgentLiveStreammingServer_H
