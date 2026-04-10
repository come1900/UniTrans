/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentMinerMtService.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: AgentMinerMtService.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

#include "../UniService/DefUniService.h"

#include "AgentMinerMtService.h"

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

PATTERN_SINGLETON_IMPLEMENT(CAgentMinerMtService);
void CAgentMinerMtService::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerMinerMtService->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerMinerMtService->GetCount())
    {
        sleep(1);

        ez_printf_info_content("%25s:%d\n", "MinerMtService-idle", m_sSerPort);

        return;
    }

    m_pHandlerMinerMtService->Select(1, 0);
    m_pHandlerMinerMtService->Update();
}

CAgentMinerMtService::CAgentMinerMtService():CEZThread("CAgentMinerMtService", THREAD_PRIORITY_DEFAULT, 10)
{
    m_sSerPort = def_UniService_MinerMtService_Port; // 如果不启用，则设置为0
    m_pHandlerMinerMtService = NULL;
    ///////////////////////////////////////////////
}
CAgentMinerMtService::~CAgentMinerMtService()
{
    // handler里面自管理
    m_pCHttpServerMinerMtService = NULL;

    if (m_pHandlerMinerMtService)
    {
        delete m_pHandlerMinerMtService;
        m_pHandlerMinerMtService = NULL;
    }
}

CHandlerMinerMtService * CAgentMinerMtService::GetHandler()
{
    return m_pHandlerMinerMtService;
}

void CAgentMinerMtService::Connect()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigMinerMtService __cfg;
    __cfg.update();

    m_sSerPort = ((unsigned short)(__cfg.getConfig().iPort));
#else

    m_sSerPort = ((unsigned short)(def_UniService_MinerMtService_Port));
#endif

    if (m_sSerPort > 0)
    {
        m_pCHttpServerMinerMtService = new ListenSocket<CHttpServerMinerMtService>(*m_pHandlerMinerMtService);

        if (m_pCHttpServerMinerMtService)
        {
            if (m_pCHttpServerMinerMtService->Bind(m_sSerPort) < 0)
            {
                ez_printf_info_content("%25s:%d failed!\n", "MinerMtService", m_sSerPort);
            }
            else
            {
                m_pHandlerMinerMtService->SetTimeout();
                m_pHandlerMinerMtService->Add(m_pCHttpServerMinerMtService);
                ez_printf_info_content("%25s:%d\n", "MinerMtService", m_sSerPort);
            }
        }
    }
    else
    {
        ez_printf_error("Error, m_pCHttpServerMinerMtService: %d\n", m_sSerPort);
    }
}

void CAgentMinerMtService::Disconnect()
{}

EZTHREAD_BOOL CAgentMinerMtService::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    if (NULL == m_pHandlerMinerMtService)
    {
        m_pHandlerMinerMtService = new CHandlerMinerMtService(&m_Log);
        assert(m_pHandlerMinerMtService);
    }

    // 初始化一些变量
    m_pHandlerMinerMtService->Start();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentMinerMtService::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentMinerMtService::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
