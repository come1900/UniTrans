/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentAssOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentAssOverHttp.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefAssOverHttp.h"

#include "AgentAssOverHttp.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigAssOverHttp.h"
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


PATTERN_SINGLETON_IMPLEMENT(CAgentAssOverHttp);
void CAgentAssOverHttp::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerAssOverHttp->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerAssOverHttp->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerAssOverHttp->Select(1,0);
    m_pHandlerAssOverHttp->Update();
}

CAgentAssOverHttp::CAgentAssOverHttp():CEZThread("CAgentAssOverHttp", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfAssOverHttp.HttpServicePort = 60089; // 如果不启用，则设置为0

    m_pHandlerAssOverHttp = new CHandlerAssOverHttp(&m_Log);
    assert(m_pHandlerAssOverHttp);

    ///////////////////////////////////////////////
}
CAgentAssOverHttp::~CAgentAssOverHttp()
{
    m_ConfigOfAssOverHttp.HttpServicePort = 0;
    // handler里面自管理
    m_pCHttpServerAssOverHttp = NULL;
    
    if (m_pHandlerAssOverHttp)
    {
        delete m_pHandlerAssOverHttp;
    }
}

CHandlerAssOverHttp * CAgentAssOverHttp::GetHandler()
{
	return m_pHandlerAssOverHttp;
}

void CAgentAssOverHttp::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfAssOverHttp.HttpServicePort = port;
}

int CAgentAssOverHttp::Send2Socket(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerAssOverHttp->GetSocketByName(strServerName);

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


void CAgentAssOverHttp::ConnectAll()
{
    if (m_ConfigOfAssOverHttp.HttpServicePort > 0)
    {
        DBG(
        __fline;
        printf("new m_pCHttpServerAssOverHttp: %d\n", m_ConfigOfAssOverHttp.HttpServicePort);
         );

        m_pCHttpServerAssOverHttp = new ListenSocket<CHttpServerAssOverHttp>(*m_pHandlerAssOverHttp);

        if (m_pCHttpServerAssOverHttp)
        {
            if (m_pCHttpServerAssOverHttp->Bind(m_ConfigOfAssOverHttp.HttpServicePort) < 0)
            {
                __trip;
                printf("AssOverHttp->Bind :%d error\n", m_ConfigOfAssOverHttp.HttpServicePort);
            }
            else
            {
                m_pHandlerAssOverHttp->Add(m_pCHttpServerAssOverHttp);
                // DBG_CODE(
                //__fline;
                printf("AssOverHttp\t\tReady:%d\n", m_ConfigOfAssOverHttp.HttpServicePort);
                // );
                LOG4CPLUS_INFO(LOG_WEBSERVICES, "AssOverHttp     Ready:"<<m_ConfigOfAssOverHttp.HttpServicePort);
            }
        }
    }
}

void CAgentAssOverHttp::DisconnectAll()
{

}

EZTHREAD_BOOL CAgentAssOverHttp::Start()
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

EZTHREAD_BOOL CAgentAssOverHttp::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentAssOverHttp::SetDefaultConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigAssOverHttp __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iAssPort));
#else
    SetGeneralHttpAgent((unsigned short)(AssOverHttp_SERVER_PORT));

#endif
}

void CAgentAssOverHttp::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
