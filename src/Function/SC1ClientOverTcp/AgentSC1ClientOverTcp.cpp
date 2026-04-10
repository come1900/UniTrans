/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentSC1ClientOverTcp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentSC1ClientOverTcp.cpp 5884 2015-01-05 14:04:41Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefSC1ClientOverTcp.h"

#include "../../Logs.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigSC1ClientOverTcp.h"
#endif //_USE_MODULES_EZCONFIG

#include "AgentSC1ClientOverTcp.h"

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

PATTERN_SINGLETON_IMPLEMENT(CAgentSC1ClientOverTcp);
void CAgentSC1ClientOverTcp::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerSC1ClientOverTcp->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerSC1ClientOverTcp->GetCount())
    {
        __fline;
        printf("ConnectAll ...\n");

        ConnectAll();

        sleep(1);
        return;
    }

    m_pHandlerSC1ClientOverTcp->Select(1,0);
    m_pHandlerSC1ClientOverTcp->Update();
}

CAgentSC1ClientOverTcp::CAgentSC1ClientOverTcp():CEZThread("CAgentSC1ClientOverTcp", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfSC1ClientOverTcp.HttpServicePort = 60088; // 如果不启用，则设置为0

    m_pHandlerSC1ClientOverTcp = new CHandlerSC1ClientOverTcp(&m_Log);
    assert(m_pHandlerSC1ClientOverTcp);

    ///////////////////////////////////////////////
}
CAgentSC1ClientOverTcp::~CAgentSC1ClientOverTcp()
{
    m_ConfigOfSC1ClientOverTcp.HttpServicePort = 0;
    // handler里面自管理
    m_pCSC1ClientOverTcpKeepConnector = NULL;

    if (m_pHandlerSC1ClientOverTcp)
    {
        delete m_pHandlerSC1ClientOverTcp;
    }
}

CHandlerSC1ClientOverTcp * CAgentSC1ClientOverTcp::GetHandler()
{
    return m_pHandlerSC1ClientOverTcp;
}

void CAgentSC1ClientOverTcp::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfSC1ClientOverTcp.HttpServicePort = port;
}

int CAgentSC1ClientOverTcp::SendBySocketName(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerSC1ClientOverTcp->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf(pData, Len);
    }
    else
    {
        __trip;
        printf("SendBySocketName %s, Len:%d failed\n", strServerName.c_str(), (int)Len);
    }

    return 0;
}


void CAgentSC1ClientOverTcp::ConnectAll()
{
    {
        CSC1ClientOverTcpKeepConnector *_pConnector
        = new CSC1ClientOverTcpKeepConnector(*m_pHandlerSC1ClientOverTcp, m_ConfigOfSC1ClientOverTcp.strServerName);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfSC1ClientOverTcp.strHost.c_str(), m_ConfigOfSC1ClientOverTcp.Port);
            if (bRet)
            {
                printf("SC1ClientOverTcpConnector(%s):%s:%d Opened\n"
                	   , m_ConfigOfSC1ClientOverTcp.strServerName.c_str()
                       , m_ConfigOfSC1ClientOverTcp.strHost.c_str()
                       , m_ConfigOfSC1ClientOverTcp.Port);

                m_pHandlerSC1ClientOverTcp->Add(_pConnector);
            }
            else
            {
                __trip;
                // 失败处理
            }
        }
        else
        {
            __trip;
            //失败处理
        }
    }
    
    // 多客户端的情况
    if (0)
    {
        CSC1ClientOverTcpKeepConnector *_pConnector
        = new CSC1ClientOverTcpKeepConnector(*m_pHandlerSC1ClientOverTcp, m_ConfigOfSC1ClientOverTcp.strServerName+"Internet");
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfSC1ClientOverTcp.strHost.c_str(), m_ConfigOfSC1ClientOverTcp.Port);
            if (bRet)
            {
                printf("SC1ClientOverTcpConnector:%s:%d Open\n"
                       , m_ConfigOfSC1ClientOverTcp.strHost.c_str()
                       , m_ConfigOfSC1ClientOverTcp.Port);

                m_pHandlerSC1ClientOverTcp->Add(_pConnector);
            }
            else
            {
                __trip;
                // 失败处理
            }
        }
        else
        {
            __trip;
            //失败处理
        }
    }

    // 多客户端的情况
    std::string strName = "Ci";
    //for (int ii=0; ii<50; ii++)
    for (int ii=0; ii<0; ii++)
    {
        CSC1ClientOverTcpKeepConnector *_pConnector
        = new CSC1ClientOverTcpKeepConnector(*m_pHandlerSC1ClientOverTcp, strName/*m_ConfigOfSC1ClientOverTcp.strServerName+"Internet"*/);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfSC1ClientOverTcp.strHost.c_str(), m_ConfigOfSC1ClientOverTcp.Port);
            if (bRet)
            {
                printf("SC1ClientOverTcpConnector:%s:%d Open\n"
                       , m_ConfigOfSC1ClientOverTcp.strHost.c_str()
                       , m_ConfigOfSC1ClientOverTcp.Port);

                m_pHandlerSC1ClientOverTcp->Add(_pConnector);
            }
            else
            {
                __trip;
                // 失败处理
            }
        }
        else
        {
            __trip;
            //失败处理
        }

        strName += "Ci";

    }
}

void CAgentSC1ClientOverTcp::DisconnectAll()
{}

EZTHREAD_BOOL CAgentSC1ClientOverTcp::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    // 调试的时候使用默认参数
    SetDefaultConfig();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentSC1ClientOverTcp::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentSC1ClientOverTcp::SetDefaultConfig()
{
    m_ConfigOfSC1ClientOverTcp.IsUse = 1;
    m_ConfigOfSC1ClientOverTcp.Port = DEFAULT_SMS_HOST_PORT;
    m_ConfigOfSC1ClientOverTcp.strHost = DEFAULT_SMS_HOST;

    // will find by name
    m_ConfigOfSC1ClientOverTcp.strServerName = CSC1ClientOverTcpKeepConnector_SOCKET_NAME;

    m_ConfigOfSC1ClientOverTcp.iConnTimeOut = 5;//超时时间
    m_ConfigOfSC1ClientOverTcp.iConnTimes = 5; //连接尝试次数（保留）

    m_ConfigOfSC1ClientOverTcp.iInterval = 5; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    m_ConfigOfSC1ClientOverTcp.bEnableRecon  = 1;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    m_ConfigOfSC1ClientOverTcp.strUName = "root";
    m_ConfigOfSC1ClientOverTcp.strUPwd = "admin";

    return;

#ifdef _USE_MODULES_EZCONFIG

    CConfigSC1ClientOverTcp __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iSC1ClientOverTcpPort));
#else

    //SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));

#endif
}

void CAgentSC1ClientOverTcp::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}

int CAgentSC1ClientOverTcp::SendSmsMsg(const std::string &strUri, const std::string &strSession, const std::string &strMsg, const std::string &matchRequest)
{
    CSC1ClientOverTcpKeepConnector *p = m_pHandlerSC1ClientOverTcp->GetSocketBySessionID(strSession);

    if (p)
    {
        return 0;//p->SendSmsMsg(strUri, strMsg, matchRequest);
    }
    else
    {
        __trip;
        printf("Not found session:%s\n", strSession.c_str());
    }

    return -1;
}

int CAgentSC1ClientOverTcp::SendCmd(const int cmdNo)
{
    m_pHandlerSC1ClientOverTcp->SendCmd(cmdNo);

    return 0;
}

