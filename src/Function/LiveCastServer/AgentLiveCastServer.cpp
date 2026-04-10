/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveCastServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveCastServer.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

#include "TypeDefLiveCastServer.h"

#include "AgentLiveCastServer.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveCastServer.h"
#endif //_USE_MODULES_EZCONFIG

//#include "gPushMan/tykdMobileServiceHX_Cli.h"
//#include "../Interface/tykdWebService/tykdMobileServiceClient.h"
//#include "gPushMan/TypedefPssOfTykd.h"
//#include "../Interface/tykdWebService/TypedefTykdMobileService.h"

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
//#define AgentLiveCastServer_CHK_SOCKET_TIMEOUT_US 1000*1000


PATTERN_SINGLETON_IMPLEMENT(CAgentLiveCastServer);
void CAgentLiveCastServer::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerLiveCastServer->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerLiveCastServer->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    //    m_pHandlerLiveCastServer->Select(0, AgentLiveCastServer_CHK_SOCKET_TIMEOUT_US);
    m_pHandlerLiveCastServer->Select(1, 0);
    m_pHandlerLiveCastServer->Update();
}

CAgentLiveCastServer::CAgentLiveCastServer():CEZThread("CAgentLiveCastServer", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfLiveCastServer.HttpServicePort = 60090; // 如果不启用，则设置为0

    m_pHandlerLiveCastServer = new CHandlerLiveCastServer(&m_Log);
    assert(m_pHandlerLiveCastServer);

    ///////////////////////////////////////////////
}
CAgentLiveCastServer::~CAgentLiveCastServer()
{
    m_ConfigOfLiveCastServer.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerLiveCastServer = NULL;

    if (m_pHandlerLiveCastServer)
    {
        delete m_pHandlerLiveCastServer;
    }
}

CHandlerLiveCastServer * CAgentLiveCastServer::GetHandler()
{
    return m_pHandlerLiveCastServer;
}

void CAgentLiveCastServer::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfLiveCastServer.HttpServicePort = port;
}

void CAgentLiveCastServer::Connect()
{
    if (m_ConfigOfLiveCastServer.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerLiveCastServer: %d\n", m_ConfigOfLiveCastServer.HttpServicePort);
        );

        m_pCHttpServerLiveCastServer = new ListenSocket<CHttpServerLiveCastServer>(*m_pHandlerLiveCastServer);

        if (m_pCHttpServerLiveCastServer)
        {
            if (m_pCHttpServerLiveCastServer->Bind(m_ConfigOfLiveCastServer.HttpServicePort) < 0)
            {
                ez_printf_error("LiveCastServer->Bind :%d error\n", m_ConfigOfLiveCastServer.HttpServicePort);				
            }
            else
            {
                m_pHandlerLiveCastServer->SetTimeout();
                m_pHandlerLiveCastServer->Add(m_pCHttpServerLiveCastServer);
                // DBG_CODE(
                //__fline;
                printf("LiveCastServer\t\tReady:%d\n", m_ConfigOfLiveCastServer.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "LiveCastServer     Ready:"<<m_ConfigOfLiveCastServer.HttpServicePort);
            }
        }
    }
    else
    {
        __fline;
        printf("Error, m_pCHttpServerLiveCastServer: %d\n", m_ConfigOfLiveCastServer.HttpServicePort);
    }
}

void CAgentLiveCastServer::Disconnect()
{}

ConfigLiveCast *CAgentLiveCastServer::getConfigLiveCast()
{
    if (m_vLiveCaster.size()==0)
    {
        return NULL;
    }

    m_iCurrLiveCaster++;

    return &(m_vLiveCaster[m_iCurrLiveCaster%m_vLiveCaster.size()]);
}

EZTHREAD_BOOL CAgentLiveCastServer::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    m_iCurrLiveCaster = -1;

    for (int ii=0; ii<def_MaxLiveCaster; ii++)
    {
        CConfigLiveCast __cfgCast;
        __cfgCast.update();

        if (__cfgCast.getConfig(ii).strDefaultUrlPrefixRTMP.length()<1
            && __cfgCast.getConfig(ii).strDefaultUrlPrefixRTSP.length()<1
            && __cfgCast.getConfig(ii).strDefaultUrlPrefixHLS.length()<1
            && __cfgCast.getConfig(ii).strPubPushRTMP.length()<1
            && __cfgCast.getConfig(ii).strPubPushRTSP.length()<1
           )
        {
            // 全空， 跳过
            //m_vLiveCaster.push_back(__cfgCast.getConfig(ii));
        }
        else
        {
            m_vLiveCaster.push_back(__cfgCast.getConfig(ii));
        }

    }


    // 调试的时候使用默认参数
    SetDefaultConfig();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentLiveCastServer::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentLiveCastServer::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigLiveCastServer __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iPort));
#else

    SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));
#endif
}

void CAgentLiveCastServer::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
