/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentWebService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentWebService.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefWebService.h"

#include "AgentWebService.h"
#include "../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../Configs/ConfigWebService.h"
#endif //_USE_MODULES_EZCONFIG
#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
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

PATTERN_SINGLETON_IMPLEMENT(CAgentWebService);
void CAgentWebService::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerWebService->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerWebService->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerWebService->Select(1,0);
    m_pHandlerWebService->Update();
}

CAgentWebService::CAgentWebService():CEZThread("CAgentWebService", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigGeneralAgent.HttpServicePort = 9990; // 如果不启用，则设置为0

    m_pHandlerWebService = new CHandlerWebService(&m_Log);
    assert(m_pHandlerWebService);

    ///////////////////////////////////////////////
}
CAgentWebService::~CAgentWebService()
{
    m_ConfigGeneralAgent.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerWebService = NULL;
    
    if (m_pHandlerWebService)
    {
        delete m_pHandlerWebService;
    }
}

CHandlerWebService * CAgentWebService::GetHandler()
{
	return m_pHandlerWebService;
}

void CAgentWebService::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigGeneralAgent.HttpServicePort = port;
}

int CAgentWebService::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerWebService->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket %s, Len:%ul failed\n", strServerName.c_str(), Len);
    }

    return 0;
}


void CAgentWebService::ConnectAll()
{
    if (m_ConfigGeneralAgent.HttpServicePort > 0)
    {
        DBG(
        __fline;
        printf("new m_pCHttpServerWebService: %d\n", m_ConfigGeneralAgent.HttpServicePort);
         );

        m_pCHttpServerWebService = new ListenSocket<CHttpServerWebService>(*m_pHandlerWebService);

        if (m_pCHttpServerWebService)
        {
            if (m_pCHttpServerWebService->Bind(m_ConfigGeneralAgent.HttpServicePort) < 0)
            {
                __trip;
                printf("WebService->Bind :%d error\n", m_ConfigGeneralAgent.HttpServicePort);
            }
            else
            {
                m_pHandlerWebService->Add(m_pCHttpServerWebService);
                // DBG(
                //__fline;
                printf("WebService\t\tReady:%d\n", m_ConfigGeneralAgent.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "WebService     Ready:"<<m_ConfigGeneralAgent.HttpServicePort);
            }
        }
    }
}

void CAgentWebService::DisconnectAll()
{

}

EZTHREAD_BOOL CAgentWebService::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    // 调试的时候使用默认参数
    SetDefaultConfig();

    ConnectAll();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentWebService::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentWebService::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigWebService __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iWebServicePort));
#else
    SetGeneralHttpAgent((unsigned short)(MY_WEBSERVICE_SERVER_PORT));

#endif
}

void CAgentWebService::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
