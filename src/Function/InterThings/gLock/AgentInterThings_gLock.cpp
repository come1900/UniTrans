/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentInterThings_gLock.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentInterThings_gLock.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefInterThings_gLock.h"

#include "AgentInterThings_gLock.h"
#include "../../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigInterThings_gLock.h"
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


PATTERN_SINGLETON_IMPLEMENT(CAgentInterThings_gLock);
void CAgentInterThings_gLock::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerInterThings_gLock->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerInterThings_gLock->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerInterThings_gLock->Select(1,0);
    m_pHandlerInterThings_gLock->Update();
}

CAgentInterThings_gLock::CAgentInterThings_gLock():CEZThread("CAgentInterThings_gLock", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfInterThings_gLock.HttpServicePort = 60089; // 如果不启用，则设置为0

    m_pHandlerInterThings_gLock = new CHandlerInterThings_gLock(&m_Log);
    assert(m_pHandlerInterThings_gLock);

    ///////////////////////////////////////////////
}
CAgentInterThings_gLock::~CAgentInterThings_gLock()
{
    m_ConfigOfInterThings_gLock.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerInterThings_gLock = NULL;

    if (m_pHandlerInterThings_gLock)
    {
        delete m_pHandlerInterThings_gLock;
    }
}

CHandlerInterThings_gLock * CAgentInterThings_gLock::GetHandler()
{
    return m_pHandlerInterThings_gLock;
}

void CAgentInterThings_gLock::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfInterThings_gLock.HttpServicePort = port;
}

int CAgentInterThings_gLock::SendBySocketName(std::string strSocketName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerInterThings_gLock->GetSocketBySocketName(strSocketName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("SendBySocketName %s, Len:%d failed\n", strSocketName.c_str(), (int)Len);
    }

    return 0;
}

// 反向发送
int CAgentInterThings_gLock::SendBySession(const std::string &strServerSID, const std::string &stack, const std::string &matchRequest)
{
    CHttpServerInterThings_gLock *p = m_pHandlerInterThings_gLock->GetSocketBySessionID( strServerSID);

    if (p)
    {
        p->SendSmsMsg(stack, matchRequest);
    }
    else
    {
        __trip;
        //printf("Sendto %s, msg Len:%d failed\n", strServerSID.c_str(), (int)stack.size());

        std::cout << "strServerSID:              [[[" << strServerSID << "]]] failed." << std::endl;
    }

    return 0;
}

void CAgentInterThings_gLock::ConnectAll()
{
    if (m_ConfigOfInterThings_gLock.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerInterThings_gLock: %d\n", m_ConfigOfInterThings_gLock.HttpServicePort);
        );

        m_pCHttpServerInterThings_gLock = new ListenSocket<CHttpServerInterThings_gLock>(*m_pHandlerInterThings_gLock);

        if (m_pCHttpServerInterThings_gLock)
        {
            if (m_pCHttpServerInterThings_gLock->Bind(m_ConfigOfInterThings_gLock.HttpServicePort) < 0)
            {
                __trip;
                printf("InterThings_gLock->Bind :%d error\n", m_ConfigOfInterThings_gLock.HttpServicePort);
            }
            else
            {
                m_pHandlerInterThings_gLock->SetTimeout();
                m_pHandlerInterThings_gLock->Add(m_pCHttpServerInterThings_gLock);
                // DBG_CODE(
                //__fline;
                printf("InterThings_gLock\t\tReady:%d\n", m_ConfigOfInterThings_gLock.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "InterThings_gLock     Ready:"<<m_ConfigOfInterThings_gLock.HttpServicePort);
            }
        }
    }
}

void CAgentInterThings_gLock::DisconnectAll()
{}

EZTHREAD_BOOL CAgentInterThings_gLock::Start()
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

EZTHREAD_BOOL CAgentInterThings_gLock::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentInterThings_gLock::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigInterThings_gLock __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iInterThings_gLockPort));
#else

    SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));

#endif
}

void CAgentInterThings_gLock::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
