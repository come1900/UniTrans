/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentNvpRegisterServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentNvpRegisterServer.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

//#include "TypeDefNvpRegisterServer.h"

#include "AgentNvpRegisterServer.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigNvpRegisterServer.h"
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
//#define AgentNvpRegisterServer_CHK_SOCKET_TIMEOUT_US 1000*1000


PATTERN_SINGLETON_IMPLEMENT(CAgentNvpRegisterServer);
void CAgentNvpRegisterServer::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerNvpRegisterServer->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerNvpRegisterServer->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    //    m_pHandlerNvpRegisterServer->Select(0, AgentNvpRegisterServer_CHK_SOCKET_TIMEOUT_US);
    m_pHandlerNvpRegisterServer->Select(1, 0);
    m_pHandlerNvpRegisterServer->Update();
}

CAgentNvpRegisterServer::CAgentNvpRegisterServer():CEZThread("CAgentNvpRegisterServer", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfNvpRegisterServer.HttpServicePort = 60090; // 如果不启用，则设置为0

    m_pHandlerNvpRegisterServer = new CHandlerNvpRegisterServer(&m_Log);
    assert(m_pHandlerNvpRegisterServer);

    ///////////////////////////////////////////////
}
CAgentNvpRegisterServer::~CAgentNvpRegisterServer()
{
    m_ConfigOfNvpRegisterServer.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerNvpRegisterServer = NULL;

    if (m_pHandlerNvpRegisterServer)
    {
        delete m_pHandlerNvpRegisterServer;
    }
}

CHandlerNvpRegisterServer * CAgentNvpRegisterServer::GetHandler()
{
    return m_pHandlerNvpRegisterServer;
}

void CAgentNvpRegisterServer::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfNvpRegisterServer.HttpServicePort = port;
}

void CAgentNvpRegisterServer::Connect()
{
    if (m_ConfigOfNvpRegisterServer.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerNvpRegisterServer: %d\n", m_ConfigOfNvpRegisterServer.HttpServicePort);
        );

        m_pCHttpServerNvpRegisterServer = new ListenSocket<CHttpServerNvpRegisterServer>(*m_pHandlerNvpRegisterServer);

        if (m_pCHttpServerNvpRegisterServer)
        {
            if (m_pCHttpServerNvpRegisterServer->Bind(m_ConfigOfNvpRegisterServer.HttpServicePort) < 0)
            {
                __trip;
                printf("NvpRegisterServer->Bind :%d error\n", m_ConfigOfNvpRegisterServer.HttpServicePort);
            }
            else
            {
                m_pHandlerNvpRegisterServer->SetTimeout();
                m_pHandlerNvpRegisterServer->Add(m_pCHttpServerNvpRegisterServer);
                // DBG_CODE(
                //__fline;
                printf("NvpRegisterServer\t\tReady:%d\n", m_ConfigOfNvpRegisterServer.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "NvpRegisterServer     Ready:"<<m_ConfigOfNvpRegisterServer.HttpServicePort);
            }
        }
    }
    else
    {
        __fline;
        printf("Error, m_pCHttpServerNvpRegisterServer: %d\n", m_ConfigOfNvpRegisterServer.HttpServicePort);
    }
}

void CAgentNvpRegisterServer::Disconnect()
{}

//ConfigLiveCast *CAgentNvpRegisterServer::getConfigLiveCast()
//{
//    if (m_vLiveCaster.size()==0)
//    {
//        return NULL;
//    }
//
//    m_iCurrLiveCaster++;
//
//    return &(m_vLiveCaster[m_iCurrLiveCaster%m_vLiveCaster.size()]);
//}

EZTHREAD_BOOL CAgentNvpRegisterServer::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    m_iCurrLiveCaster = -1;

//    for (int ii=0; ii<def_MaxLiveCaster; ii++)
//    {
//        CConfigLiveCast __cfgCast;
//        __cfgCast.update();
//
//        if (__cfgCast.getConfig(ii).strDefaultUrlPrefixRTMP.length()<1
//            && __cfgCast.getConfig(ii).strDefaultUrlPrefixRTSP.length()<1
//            && __cfgCast.getConfig(ii).strDefaultUrlPrefixHLS.length()<1
//            && __cfgCast.getConfig(ii).strPubPushRTMP.length()<1
//            && __cfgCast.getConfig(ii).strPubPushRTSP.length()<1
//           )
//        {
//            // 全空， 跳过
//            //m_vLiveCaster.push_back(__cfgCast.getConfig(ii));
//        }
//        else
//        {
//            m_vLiveCaster.push_back(__cfgCast.getConfig(ii));
//        }
//
//    }


    // 调试的时候使用默认参数
    SetDefaultConfig();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentNvpRegisterServer::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentNvpRegisterServer::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigNvpRegisterServer __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iPort));
#else

    SetGeneralHttpAgent((unsigned short)(def_Pandora_NvpRegisterServer_Port));
#endif
}

void CAgentNvpRegisterServer::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
