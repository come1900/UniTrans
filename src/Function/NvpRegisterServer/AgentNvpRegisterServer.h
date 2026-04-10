/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentNvpRegisterServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentNvpRegisterServer.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentNvpRegisterServer_H
#define _AgentNvpRegisterServer_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

#ifdef _USE_MODULES_EZCONFIG
//#include "../../Configs/ConfigLiveCast.h"
#include "../../Configs/ConfigNvpRegisterServer.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

// 反应堆
#include "HandlerNvpRegisterServer.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerNvpRegisterServer.h"

typedef struct __AgentNvpRegisterServerCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentNvpRegisterServerCfg_T;

#define g_AgentNvpRegisterServer (*CAgentNvpRegisterServer::instance())

class CAgentNvpRegisterServer : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentNvpRegisterServer);
    CAgentNvpRegisterServer();
    virtual ~CAgentNvpRegisterServer();
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
    	
    CHandlerNvpRegisterServer *GetHandler();
	
//    ConfigLiveCast *getConfigLiveCast();

private:
    unsigned int m_iCurrLiveCaster;
//    std::vector<ConfigLiveCast> m_vLiveCaster;

private:
    AgentNvpRegisterServerCfg_T m_ConfigOfNvpRegisterServer;
    CHandlerNvpRegisterServer *m_pHandlerNvpRegisterServer;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerNvpRegisterServer> *m_pCHttpServerNvpRegisterServer;

}
; //CAgentNvpRegisterServer
#endif //_AgentNvpRegisterServer_H
