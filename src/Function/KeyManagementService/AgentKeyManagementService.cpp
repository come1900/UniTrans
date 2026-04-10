/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentKeyManagementService.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: AgentKeyManagementService.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

#include "TypeDefKeyManagementService.h"

#include "AgentKeyManagementService.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
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

PATTERN_SINGLETON_IMPLEMENT(CAgentKeyManagementService);
void CAgentKeyManagementService::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerKeyManagementService->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerKeyManagementService->GetCount())
    {
        sleep(1);

        ez_printf_info_content("%25s:%d\n", "KeyManagementService-idle", m_sSerPort);

        return;
    }

    m_pHandlerKeyManagementService->Select(1, 0);
    m_pHandlerKeyManagementService->Update();
}

CAgentKeyManagementService::CAgentKeyManagementService():CEZThread("CAgentKeyManagementService", THREAD_PRIORITY_DEFAULT, 10)
{
    m_sSerPort = def_SER_PORT_KeyManagementService; // 如果不启用，则设置为0
    m_pHandlerKeyManagementService = NULL;
    ///////////////////////////////////////////////
}
CAgentKeyManagementService::~CAgentKeyManagementService()
{
    // handler里面自管理
    m_pCHttpServerKeyManagementService = NULL;

    if (m_pHandlerKeyManagementService)
    {
        delete m_pHandlerKeyManagementService;
        m_pHandlerKeyManagementService = NULL;
    }
}

CHandlerKeyManagementService * CAgentKeyManagementService::GetHandler()
{
    return m_pHandlerKeyManagementService;
}

void CAgentKeyManagementService::Connect()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigKeyManagementService __cfg;
    __cfg.update();

    m_sSerPort = ((unsigned short)(__cfg.getConfig().iPort));
#else

    m_sSerPort = ((unsigned short)(def_SER_PORT_KeyManagementService));
#endif

    if (m_sSerPort > 0)
    {
        m_pCHttpServerKeyManagementService = new ListenSocket<CHttpServerKeyManagementService>(*m_pHandlerKeyManagementService);

        if (m_pCHttpServerKeyManagementService)
        {
            if (m_pCHttpServerKeyManagementService->Bind(m_sSerPort) < 0)
            {
                ez_printf_info_content("%25s:%d failed!\n", "KeyManagementService", m_sSerPort);
            }
            else
            {
                m_pHandlerKeyManagementService->SetTimeout();
                m_pHandlerKeyManagementService->Add(m_pCHttpServerKeyManagementService);
                ez_printf_info_content("%25s:%d\n", "KeyManagementService", m_sSerPort);
            }
        }
    }
    else
    {
        ez_printf_error("Error, m_pCHttpServerKeyManagementService: %d\n", m_sSerPort);
    }
}

void CAgentKeyManagementService::Disconnect()
{}

EZTHREAD_BOOL CAgentKeyManagementService::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    if (NULL == m_pHandlerKeyManagementService)
    {
        m_pHandlerKeyManagementService = new CHandlerKeyManagementService(&m_Log);
        assert(m_pHandlerKeyManagementService);
    }

    // 初始化一些变量
    m_pHandlerKeyManagementService->Start();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentKeyManagementService::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentKeyManagementService::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
