/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveRecordServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveRecordServer.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

#include "TypeDefLiveRecordServer.h"

#include "AgentLiveRecordServer.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveRecordServer.h"
#endif //_USE_MODULES_EZCONFIG

//#include "gPushMan/tykdMobileServiceHX_Cli.h"
//#include "../Interface/tykdWebService/tykdMobileServiceClient.h"
//#include "gPushMan/TypedefPssOfTykd.h"
//#include "../Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */

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
//#define AgentLiveRecordServer_CHK_SOCKET_TIMEOUT_US 1000*1000


PATTERN_SINGLETON_IMPLEMENT(CAgentLiveRecordServer);
void CAgentLiveRecordServer::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerLiveRecordServer->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerLiveRecordServer->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

//    m_pHandlerLiveRecordServer->Select(0, AgentLiveRecordServer_CHK_SOCKET_TIMEOUT_US);
    m_pHandlerLiveRecordServer->Select(1, 0);
    m_pHandlerLiveRecordServer->Update();
}

CAgentLiveRecordServer::CAgentLiveRecordServer():CEZThread("CAgentLiveRecordServer", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfLiveRecordServer.HttpServicePort = 60090; // 如果不启用，则设置为0

    ///////////////////////////////////////////////
}
CAgentLiveRecordServer::~CAgentLiveRecordServer()
{
    m_ConfigOfLiveRecordServer.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerLiveRecordServer = NULL;

    if (m_pHandlerLiveRecordServer)
    {
        delete m_pHandlerLiveRecordServer;
    }
}

CHandlerLiveRecordServer * CAgentLiveRecordServer::GetHandler()
{
    return m_pHandlerLiveRecordServer;
}

void CAgentLiveRecordServer::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfLiveRecordServer.HttpServicePort = port;
}

void CAgentLiveRecordServer::Connect()
{
    if (m_ConfigOfLiveRecordServer.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerLiveRecordServer: %d\n", m_ConfigOfLiveRecordServer.HttpServicePort);
        );

        m_pCHttpServerLiveRecordServer = new ListenSocket<CHttpServerLiveRecordServer>(*m_pHandlerLiveRecordServer);

        if (m_pCHttpServerLiveRecordServer)
        {
            if (m_pCHttpServerLiveRecordServer->Bind(m_ConfigOfLiveRecordServer.HttpServicePort) < 0)
            {
                ez_printf_error("LiveRecordServer->Bind :%d error\n", m_ConfigOfLiveRecordServer.HttpServicePort);
            }
            else
            {
                m_pHandlerLiveRecordServer->SetTimeout();
                m_pHandlerLiveRecordServer->Add(m_pCHttpServerLiveRecordServer);
                // DBG_CODE(
                //__fline;
                //printf("LiveRecordServer\t\tReady:%d\n", m_ConfigOfLiveRecordServer.HttpServicePort);
                ez_printf_info_content("%25s:%d\n", "LiveRecordServer", m_ConfigOfLiveRecordServer.HttpServicePort);
                // );
                //LOG4CPLUS_INFO(LOG_WEBSERVICES, "LiveRecordServer     Ready:"<<m_ConfigOfLiveRecordServer.HttpServicePort);
            }
        }
    }
    else
    {
	ez_printf_error("Error, m_pCHttpServerLiveRecordServer: %d\n", m_ConfigOfLiveRecordServer.HttpServicePort);
    }
}

void CAgentLiveRecordServer::Disconnect()
{
}

EZTHREAD_BOOL CAgentLiveRecordServer::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    m_pHandlerLiveRecordServer = new CHandlerLiveRecordServer(&m_Log);
    assert(m_pHandlerLiveRecordServer);
    // 初始化一些变量
    m_pHandlerLiveRecordServer->Start();

    // 调试的时候使用默认参数
    SetDefaultConfig();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentLiveRecordServer::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentLiveRecordServer::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigLiveRecordServer __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iPort));
#else
    SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));
#endif
}

void CAgentLiveRecordServer::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
