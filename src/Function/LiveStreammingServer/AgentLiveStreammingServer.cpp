/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveStreammingServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveStreammingServer.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

#include "TypeDefLiveStreammingServer.h"

#include "AgentLiveStreammingServer.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveStreammingServer.h"
#endif //_USE_MODULES_EZCONFIG

//#include "gPushMan/tykdMobileServiceHX_Cli.h"
//#include "../Interface/tykdWebService/tykdMobileServiceClient.h"
//#include "gPushMan/TypedefPssOfTykd.h"
//#include "../Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */

#ifdef _FUNC_LiveStatusServer
#include "../LiveStatusServer/AgentLiveStatusServer.h"
#endif// _FUNC_LiveStatusServer 

#ifdef _DEBUG
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

// 有任务队列
// 100ms， 利于尽快返回-- 用的是异步机制
#define AgentLiveStreammingServer_CHK_SOCKET_TIMEOUT_US 100*1000


PATTERN_SINGLETON_IMPLEMENT(CAgentLiveStreammingServer);
void CAgentLiveStreammingServer::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerLiveStreammingServer->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerLiveStreammingServer->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerLiveStreammingServer->Select(0, AgentLiveStreammingServer_CHK_SOCKET_TIMEOUT_US);
    m_pHandlerLiveStreammingServer->Update();
}

CAgentLiveStreammingServer::CAgentLiveStreammingServer():CEZThread("CAgentLiveStreammingServer", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfLiveStreammingServer.HttpServicePort = 60089; // 如果不启用，则设置为0
    m_pCHttpServerLiveStreammingServer = NULL;
    m_pHandlerLiveStreammingServer = NULL;
    ///////////////////////////////////////////////
}
CAgentLiveStreammingServer::~CAgentLiveStreammingServer()
{
    m_ConfigOfLiveStreammingServer.HttpServicePort = 0;

    // handler里面自管理
    //if (m_pCHttpServerLiveStreammingServer)
        m_pCHttpServerLiveStreammingServer = NULL;

    if (m_pHandlerLiveStreammingServer)
    {
        delete m_pHandlerLiveStreammingServer;
    }
}

CHandlerLiveStreammingServer * CAgentLiveStreammingServer::GetHandler()
{
    return m_pHandlerLiveStreammingServer;
}

void CAgentLiveStreammingServer::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfLiveStreammingServer.HttpServicePort = port;
}

void CAgentLiveStreammingServer::Connect()
{
    if (m_ConfigOfLiveStreammingServer.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerLiveStreammingServer: %d\n", m_ConfigOfLiveStreammingServer.HttpServicePort);
        );

        m_pCHttpServerLiveStreammingServer = new ListenSocket<CHttpServerLiveStreammingServer>(*m_pHandlerLiveStreammingServer);

        if (m_pCHttpServerLiveStreammingServer)
        {
            if (m_pCHttpServerLiveStreammingServer->Bind(m_ConfigOfLiveStreammingServer.HttpServicePort) < 0)
            {
                ez_printf_error("LiveStreammingServer->Bind :%d error\n", m_ConfigOfLiveStreammingServer.HttpServicePort);
            }
            else
            {
                m_pHandlerLiveStreammingServer->SetTimeout();
                m_pHandlerLiveStreammingServer->Add(m_pCHttpServerLiveStreammingServer);
                // DBG_CODE(
                //__fline;
                //printf("LiveStreammingServer\t\tReady:%d\n", m_ConfigOfLiveStreammingServer.HttpServicePort);
                ez_printf_info_content("%25s:%d\n", "LiveStreammingServer", m_ConfigOfLiveStreammingServer.HttpServicePort);
                // );
                //LOG4CPLUS_INFO(LOG_WEBSERVICES, "LiveStreammingServer     Ready:"<<m_ConfigOfLiveStreammingServer.HttpServicePort);
            }
        }
    }
    else
    {
	ez_printf_error("Error, m_pCHttpServerLiveStreammingServer: %d\n", m_ConfigOfLiveStreammingServer.HttpServicePort);
    }
}

void CAgentLiveStreammingServer::Disconnect()
{
    m_pHandlerLiveStreammingServer->Disconnect();
    m_pCHttpServerLiveStreammingServer->Close();
}

EZTHREAD_BOOL CAgentLiveStreammingServer::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    // 调试的时候使用默认参数
    SetDefaultConfig();
	
    m_pHandlerLiveStreammingServer = new CHandlerLiveStreammingServer(&m_Log);
    assert(m_pHandlerLiveStreammingServer);
    // 初始化一些变量
    m_pHandlerLiveStreammingServer->Start();

    Connect();

    int iret = CreateThread();

    ARG_USED(iret);

#ifdef _FUNC_LiveStatusServer
    iret = g_AgentLiveStatusServer.Start();
#endif // _FUNC_LiveStatusServer

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentLiveStreammingServer::Stop()
{
    Disconnect();

    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentLiveStreammingServer::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigLiveStreammingServer __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iPort));
#else
    SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));
#endif
}

void CAgentLiveStreammingServer::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
