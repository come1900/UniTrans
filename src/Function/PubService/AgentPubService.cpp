/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPubService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPubService.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefPubService.h"

#include "AgentPubService.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigPubService.h"
#endif //_USE_MODULES_EZCONFIG
#ifdef _DEBUG
	#define DEB(x)
	#define DBG(x)
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


PATTERN_SINGLETON_IMPLEMENT(CAgentPubService);
void CAgentPubService::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerPubService->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerPubService->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerPubService->Select(1,0);
    m_pHandlerPubService->Update();
}

CAgentPubService::CAgentPubService():CEZThread("CAgentPubService", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfPubService.HttpServicePort = 60089; // 如果不启用，则设置为0

    m_pHandlerPubService = new CHandlerPubService(&m_Log);
    assert(m_pHandlerPubService);

    ///////////////////////////////////////////////
}
CAgentPubService::~CAgentPubService()
{
    m_ConfigOfPubService.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerPubService = NULL;
    
    if (m_pHandlerPubService)
    {
        delete m_pHandlerPubService;
    }
}

CHandlerPubService * CAgentPubService::GetHandler()
{
	return m_pHandlerPubService;
}

void CAgentPubService::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfPubService.HttpServicePort = port;
}

int CAgentPubService::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerPubService->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket %s, Len:%d failed\n", strServerName.c_str(), (int)Len);
    }

    return 0;
}


void CAgentPubService::ConnectAll()
{
    if (m_ConfigOfPubService.HttpServicePort > 0)
    {
        DBG(
        __fline;
        printf("new m_pCHttpServerPubService: %d\n", m_ConfigOfPubService.HttpServicePort);
         );

        m_pCHttpServerPubService = new ListenSocket<CHttpServerPubService>(*m_pHandlerPubService);

        if (m_pCHttpServerPubService)
        {
            if (m_pCHttpServerPubService->Bind(m_ConfigOfPubService.HttpServicePort) < 0)
            {
                __trip;
                printf("PubService->Bind :%d error\n", m_ConfigOfPubService.HttpServicePort);
            }
            else
            {
                m_pHandlerPubService->Add(m_pCHttpServerPubService);
                // DBG_CODE(
                //__fline;
                printf("PubService\t\tReady:%d\n", m_ConfigOfPubService.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "PubService      Ready:"<<m_ConfigOfPubService.HttpServicePort);
            }
        }
    }
}

void CAgentPubService::DisconnectAll()
{

}

EZTHREAD_BOOL CAgentPubService::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    // 调试的时候使用默认参数
    SetConfig();

    ConnectAll();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentPubService::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentPubService::SetConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigPubService __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iPubServicePort));
#else
    SetGeneralHttpAgent((unsigned short)(PubService_SERVER_PORT));

#endif
}

void CAgentPubService::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
