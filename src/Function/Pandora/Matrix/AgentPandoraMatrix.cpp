/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPandoraMatrix.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPandoraMatrix.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//#include "TypeDefPandoraMatrix.h"

#include "AgentPandoraMatrix.h"
#include "../../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigPandoraMatrix.h"
#endif //_USE_MODULES_EZCONFIG

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

// 有任务队列
//#define CHK_SOCKET_TIMEOUT_US 1*1000
//无任务队列
#define CAgentPandoraMatrix_CHK_SOCKET_TIMEOUT_US 900*1000

PATTERN_SINGLETON_IMPLEMENT(CAgentPandoraMatrix);
void CAgentPandoraMatrix::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerPandoraMatrix->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerPandoraMatrix->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    //long sec,long usec
    m_pHandlerPandoraMatrix->Select(0, CAgentPandoraMatrix_CHK_SOCKET_TIMEOUT_US);
    m_pHandlerPandoraMatrix->Update();

    //ez_printf_info("%s\n", __FUNCTION__);
}

//const char*pName, int nPriority, int nMsgQueSize /* = 0 */, unsigned int dwStackSize
CAgentPandoraMatrix::CAgentPandoraMatrix():CEZThread("CAgentPandoraMatrix", THREAD_PRIORITY_DEFAULT, 100)
{
    //m_ConfigOfPandoraMatrix.HttpServicePort = 60089; // 如果不启用，则设置为0
    m_pHandlerPandoraMatrix = NULL;
    ///////////////////////////////////////////////
}

CAgentPandoraMatrix::~CAgentPandoraMatrix()
{
    //m_ConfigOfPandoraMatrix.HttpServicePort = 0;

    //m_pCHttpServerPandoraMatrix = NULL;

    if (m_pHandlerPandoraMatrix)
    {
        delete m_pHandlerPandoraMatrix;
    }
}

CHandlerPandoraMatrix * CAgentPandoraMatrix::GetHandler()
{
    return m_pHandlerPandoraMatrix;
}

void CAgentPandoraMatrix::ConnectAll()
{
    port_t HttpServicePort = 60089;
    port_t UdpServicePort = 60089;

#ifdef _USE_MODULES_EZCONFIG
    CConfigPandoraMatrix __cfg;
    __cfg.update();

    HttpServicePort = (port_t)(__cfg.getConfig().iPandoraMatrixPort);
    UdpServicePort = (port_t)(__cfg.getConfig().iPandoraMatrixPortUdp);
#endif//_USE_MODULES_EZCONFIG

    if (HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerPandoraMatrix: %d\n", HttpServicePort);
        );

        m_pCHttpServerPandoraMatrix = new ListenSocket<CHttpServerPandoraMatrix>(*m_pHandlerPandoraMatrix);
        //m_pHandlerPandoraMatrix->SetNumberOfThreads(10);

        if (m_pCHttpServerPandoraMatrix)
        {
            if (m_pCHttpServerPandoraMatrix->Bind(HttpServicePort) < 0)
            {
                //__trip;
                //printf("PandoraMatrix->Bind :%d error\n", HttpServicePort);
                ez_printf_info_content("%25s:%d failed!\n", "PandoraMatrix", HttpServicePort);
            }
            else
            {
                m_pHandlerPandoraMatrix->SetTimeout();
                m_pHandlerPandoraMatrix->Add(m_pCHttpServerPandoraMatrix);
                // DBG_CODE(
                //__fline;
                //printf("PandoraMatrix\t\tReady:%d\n", HttpServicePort);
                ez_printf_info_content("%25s:%d\n", "PandoraMatrix", HttpServicePort);
                // );
                //LOG4CPLUS_INFO(LOG_WEBSERVICES, "PandoraMatrix     Ready:"<<HttpServicePort);
            }
        }
    }
    else
    {
        ez_printf_warning("invalid port:%d\n", HttpServicePort);
    }

    if (UdpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pUdpServerPandoraMatrixServer:%d\n", UdpServicePort);
        );

        m_pUdpServerPandoraMatrixServer = new CUdpServerPandoraMatrix(*m_pHandlerPandoraMatrix);

        if (m_pUdpServerPandoraMatrixServer)
        {
            if (m_pUdpServerPandoraMatrixServer->Bind(UdpServicePort, 10) < 0)
            {
                ez_printf_info_content("%25s:%d failed!\n", "PandoraMatrixUdp", HttpServicePort);
            }
            else
            {
                m_pHandlerPandoraMatrix->Add(m_pUdpServerPandoraMatrixServer);
                ez_printf_info_content("%25s:%d\n", "PandoraMatrixUdp", HttpServicePort);
            }
        }
    }
}

void CAgentPandoraMatrix::DisconnectAll()
{
}

EZTHREAD_BOOL CAgentPandoraMatrix::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    m_pHandlerPandoraMatrix = new CHandlerPandoraMatrix(&m_Log);
    assert(m_pHandlerPandoraMatrix);
    // 初始化一些变量
    m_pHandlerPandoraMatrix->Start();

    ConnectAll();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentPandoraMatrix::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}


void CAgentPandoraMatrix::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
