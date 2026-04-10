/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentInterUtcs_YiHeTech.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentInterUtcs_YiHeTech.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 16:36:13  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefInterUtcs_YiHeTech.h"

#include "AgentInterUtcs_YiHeTech.h"
#include "../../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigInterUtcs_YiHeTech.h"
#endif //_USE_MODULES_EZCONFIG

#ifdef _USE_DBAGENT_OCCI
#include "../../../DBAgent/ltUtcsDbAgent.h"
#endif //_USE_DBAGENT_OCCI

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


PATTERN_SINGLETON_IMPLEMENT(CAgentInterUtcs_YiHeTech);
void CAgentInterUtcs_YiHeTech::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerInterUtcs_YiHeTech->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerInterUtcs_YiHeTech->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerInterUtcs_YiHeTech->Select(1,0);
    m_pHandlerInterUtcs_YiHeTech->Update();
}

CAgentInterUtcs_YiHeTech::CAgentInterUtcs_YiHeTech():CEZThread("CAgentInterUtcs_YiHeTech", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfInterUtcs_YiHeTech.HttpServicePort = 60089; // 如果不启用，则设置为0

    m_pHandlerInterUtcs_YiHeTech = new CHandlerInterUtcs_YiHeTech(&m_Log);
    assert(m_pHandlerInterUtcs_YiHeTech);

    ///////////////////////////////////////////////
}
CAgentInterUtcs_YiHeTech::~CAgentInterUtcs_YiHeTech()
{
    m_ConfigOfInterUtcs_YiHeTech.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerInterUtcs_YiHeTech = NULL;

    if (m_pHandlerInterUtcs_YiHeTech)
    {
        delete m_pHandlerInterUtcs_YiHeTech;
    }
}

CHandlerInterUtcs_YiHeTech * CAgentInterUtcs_YiHeTech::GetHandler()
{
    return m_pHandlerInterUtcs_YiHeTech;
}

void CAgentInterUtcs_YiHeTech::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfInterUtcs_YiHeTech.HttpServicePort = port;
}

int CAgentInterUtcs_YiHeTech::SendBySocketName(std::string strSocketName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerInterUtcs_YiHeTech->GetSocketBySocketName(strSocketName);

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
int CAgentInterUtcs_YiHeTech::SendBySession(const std::string &strServerSID, const std::string &stack, const std::string &matchRequest)
{
    CHttpServerInterUtcs_YiHeTech *p = m_pHandlerInterUtcs_YiHeTech->GetSocketBySessionID( strServerSID);

    if (p)
    {
        p->SendYiHe1Msg(stack, matchRequest);
    }
    else
    {
        __trip;
        //printf("Sendto %s, msg Len:%d failed\n", strServerSID.c_str(), (int)stack.size());
             cout << "strServerSID:              [[[" << strServerSID << "]]] failed." << endl;
    }

    return 0;
}

void CAgentInterUtcs_YiHeTech::ConnectAll()
{
    if (m_ConfigOfInterUtcs_YiHeTech.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerInterUtcs_YiHeTech: %d\n", m_ConfigOfInterUtcs_YiHeTech.HttpServicePort);
        );

        m_pCHttpServerInterUtcs_YiHeTech = new ListenSocket<CHttpServerInterUtcs_YiHeTech>(*m_pHandlerInterUtcs_YiHeTech);

        if (m_pCHttpServerInterUtcs_YiHeTech)
        {
            if (m_pCHttpServerInterUtcs_YiHeTech->Bind(m_ConfigOfInterUtcs_YiHeTech.HttpServicePort) < 0)
            {
                __trip;
                printf("InterUtcs_YiHeTech->Bind :%d error\n", m_ConfigOfInterUtcs_YiHeTech.HttpServicePort);
            }
            else
            {
                m_pHandlerInterUtcs_YiHeTech->SetTimeout();
                m_pHandlerInterUtcs_YiHeTech->Add(m_pCHttpServerInterUtcs_YiHeTech);
                // DBG_CODE(
                //__fline;
                printf("InterUtcs_YiHeTech\tReady:%d\n", m_ConfigOfInterUtcs_YiHeTech.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "InterUtcs_YiHeTech     Ready:"<<m_ConfigOfInterUtcs_YiHeTech.HttpServicePort);
            }
        }
    }
}

void CAgentInterUtcs_YiHeTech::DisconnectAll()
{
}

EZTHREAD_BOOL CAgentInterUtcs_YiHeTech::Start()
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

EZTHREAD_BOOL CAgentInterUtcs_YiHeTech::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentInterUtcs_YiHeTech::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigInterUtcs_YiHeTech __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iInterUtcs_YiHeTechPort));
#else

    SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));

#endif
}

void CAgentInterUtcs_YiHeTech::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
