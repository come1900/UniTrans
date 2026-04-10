/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentInterVideo.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentInterVideo.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"

#include "TypeDefInterVideo.h"

#include "AgentInterVideo.h"
#include "../../Logs.h"

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

PATTERN_SINGLETON_IMPLEMENT(CAgentInterVideo);
void CAgentInterVideo::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerInterVideo->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerInterVideo->GetCount())
    {
        sleep(1);

        ez_printf_info_content("%25s:%d\n", "InterVideo-idle", m_sSerPort);

        return;
    }

    m_pHandlerInterVideo->Select(1, 0);
    m_pHandlerInterVideo->Update();
}

CAgentInterVideo::CAgentInterVideo():CEZThread("CAgentInterVideo", THREAD_PRIORITY_DEFAULT, 10)
{
    m_sSerPort = def_SER_PORT_InterVideo; // 如果不启用，则设置为0

    m_pHandlerInterVideo = new CHandlerInterVideo(&m_Log);
    assert(m_pHandlerInterVideo);

    ///////////////////////////////////////////////
}
CAgentInterVideo::~CAgentInterVideo()
{
    //m_sSerPort = 0;

    // handler里面自管理
    m_pCHttpServerInterVideo = NULL;

    if (m_pHandlerInterVideo)
    {
        delete m_pHandlerInterVideo;
    }
}

CHandlerInterVideo * CAgentInterVideo::GetHandler()
{
    return m_pHandlerInterVideo;
}

void CAgentInterVideo::SetSerPort(unsigned short  port)
{
    m_sSerPort = port;
}

void CAgentInterVideo::Connect()
{
    if (m_sSerPort > 0)
    {
        m_pCHttpServerInterVideo = new ListenSocket<CHttpServerInterVideo>(*m_pHandlerInterVideo);

        if (m_pCHttpServerInterVideo)
        {
            if (m_pCHttpServerInterVideo->Bind(m_sSerPort) < 0)
            {
                ez_printf_info_content("%25s:%d failed!\n", "InterVideo", m_sSerPort);
            }
            else
            {
                m_pHandlerInterVideo->SetTimeout();
                m_pHandlerInterVideo->Add(m_pCHttpServerInterVideo);
                ez_printf_info_content("%25s:%d\n", "InterVideo", m_sSerPort);
            }
        }
    }
    else
    {
        ez_printf_error("Error, m_pCHttpServerInterVideo: %d\n", m_sSerPort);
    }
}

void CAgentInterVideo::Disconnect()
{}

EZTHREAD_BOOL CAgentInterVideo::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    // 调试的时候使用默认参数
    UseConfig();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentInterVideo::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentInterVideo::UseConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigInterVideo __cfg;
    __cfg.update();

    SetSerPort((unsigned short)(__cfg.getConfig().iPort));
#else

    SetSerPort((unsigned short)(def_SER_PORT_InterVideo));
#endif
}

void CAgentInterVideo::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
