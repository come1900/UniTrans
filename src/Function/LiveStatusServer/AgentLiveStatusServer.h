/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveStatusServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveStatusServer.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentLiveStatusServer_H
#define _AgentLiveStatusServer_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

#include "FileTailer.h"


// 反应堆
#include "HandlerLiveStatusServer.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerLiveStatusServer.h"

typedef struct __AgentLiveStatusServerCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentLiveStatusServerCfg_T;

#define g_AgentLiveStatusServer (*CAgentLiveStatusServer::instance())

class CAgentLiveStatusServer : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentLiveStatusServer);
    CAgentLiveStatusServer();
    virtual ~CAgentLiveStatusServer();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
    //void SetDefaultConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void Connect();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void Disconnect();

    void Run();

    //int SendBySocketName(std::string strSocketName, const char *pData, size_t Len);

    //int SendBySession(const std::string &strServerSID, const std::string &stack, const std::string &matchRequest="");
    	
    CHandlerLiveStatusServer *GetHandler();

    void OnFileTailer(int iCMD, char *pLine);
private:
    AgentLiveStatusServerCfg_T m_ConfigOfLiveStatusServer;
    CHandlerLiveStatusServer *m_pHandlerLiveStatusServer;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerLiveStatusServer> *m_pCHttpServerLiveStatusServer;

    CConfigLiveStatusServer *m_pCfgLiveStatusServer;

    CFileTailer *m_pFileTailer_NgxAccessLog;

}
; //CAgentLiveStatusServer
#endif //_AgentLiveStatusServer_H
