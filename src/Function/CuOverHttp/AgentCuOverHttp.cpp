/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentCuOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentCuOverHttp.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefCuOverHttp.h"

#include "../../Logs.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigCuOverHttp.h"
#endif //_USE_MODULES_EZCONFIG

#include "AgentCuOverHttp.h"

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


PATTERN_SINGLETON_IMPLEMENT(CAgentCuOverHttp);
void CAgentCuOverHttp::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerCuOverHttp->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerCuOverHttp->GetCount())
    {
    	// 配合login失败计数， 实现重连接机制。
    	// 在连接某些单片机系统或者localhost的时候防止异常的处理
#if 1
        __fline;
        printf("ConnectAll ...\n");

        ConnectAll();
#else
        __fline;
        printf("idle.\n");
#endif
        sleep(1);
        return;
    }

    m_pHandlerCuOverHttp->Select(1,0);
    m_pHandlerCuOverHttp->Update();
}

CAgentCuOverHttp::CAgentCuOverHttp():CEZThread("CAgentCuOverHttp", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfCuOverHttp.HttpServicePort = 60088; // 如果不启用，则设置为0

    m_pHandlerCuOverHttp = new CHandlerCuOverHttp(&m_Log);
    assert(m_pHandlerCuOverHttp);

    ///////////////////////////////////////////////
}
CAgentCuOverHttp::~CAgentCuOverHttp()
{
    m_ConfigOfCuOverHttp.HttpServicePort = 0;
    // handler里面自管理
    m_pCCuOverHttpKeepConnector = NULL;

    if (m_pHandlerCuOverHttp)
    {
        delete m_pHandlerCuOverHttp;
    }
}

CHandlerCuOverHttp * CAgentCuOverHttp::GetHandler()
{
    return m_pHandlerCuOverHttp;
}

void CAgentCuOverHttp::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfCuOverHttp.HttpServicePort = port;
}

int CAgentCuOverHttp::SendBySocketName(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerCuOverHttp->GetSocketByName(strServerName);

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


void CAgentCuOverHttp::ConnectAll()
{
    {
        CCuOverHttpKeepConnector *_pConnector
        = new CCuOverHttpKeepConnector(*m_pHandlerCuOverHttp, m_ConfigOfCuOverHttp.strServerName);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfCuOverHttp.strHost.c_str(), m_ConfigOfCuOverHttp.Port);
            if (bRet)
            {
                printf("CuOverHttpConnector(%s):%s:%d Opened\n"
                	   , m_ConfigOfCuOverHttp.strServerName.c_str()
                       , m_ConfigOfCuOverHttp.strHost.c_str()
                       , m_ConfigOfCuOverHttp.Port);

                m_pHandlerCuOverHttp->Add(_pConnector);
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
        CCuOverHttpKeepConnector *_pConnector
        = new CCuOverHttpKeepConnector(*m_pHandlerCuOverHttp, m_ConfigOfCuOverHttp.strServerName+"Internet");
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfCuOverHttp.strHost.c_str(), m_ConfigOfCuOverHttp.Port);
            if (bRet)
            {
                printf("CuOverHttpConnector:%s:%d Open\n"
                       , m_ConfigOfCuOverHttp.strHost.c_str()
                       , m_ConfigOfCuOverHttp.Port);

                m_pHandlerCuOverHttp->Add(_pConnector);
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
        CCuOverHttpKeepConnector *_pConnector
        = new CCuOverHttpKeepConnector(*m_pHandlerCuOverHttp, strName/*m_ConfigOfCuOverHttp.strServerName+"Internet"*/);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfCuOverHttp.strHost.c_str(), m_ConfigOfCuOverHttp.Port);
            if (bRet)
            {
                printf("CuOverHttpConnector:%s:%d Open\n"
                       , m_ConfigOfCuOverHttp.strHost.c_str()
                       , m_ConfigOfCuOverHttp.Port);

                m_pHandlerCuOverHttp->Add(_pConnector);
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

void CAgentCuOverHttp::DisconnectAll()
{}

EZTHREAD_BOOL CAgentCuOverHttp::Start()
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

EZTHREAD_BOOL CAgentCuOverHttp::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentCuOverHttp::SetDefaultConfig()
{
    m_ConfigOfCuOverHttp.IsUse = 1;
    m_ConfigOfCuOverHttp.Port = DEFAULT_SMS_HOST_PORT;
    m_ConfigOfCuOverHttp.strHost = DEFAULT_SMS_HOST;

    // will find by name
    m_ConfigOfCuOverHttp.strServerName = CCuOverHttpKeepConnector_SOCKET_NAME;

    m_ConfigOfCuOverHttp.iConnTimeOut = 5;//超时时间
    m_ConfigOfCuOverHttp.iConnTimes = 5; //连接尝试次数（保留）

    m_ConfigOfCuOverHttp.iInterval = 5; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    m_ConfigOfCuOverHttp.bEnableRecon  = 1;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    m_ConfigOfCuOverHttp.strUName = "root";
    m_ConfigOfCuOverHttp.strUPwd = "admin";

    return;

#ifdef _USE_MODULES_EZCONFIG

    CConfigCuOverHttp __cfg;
    __cfg.update();

    SetGeneralHttpAgent((unsigned short)(__cfg.getConfig().iCuOverHttpPort));
#else

    //SetGeneralHttpAgent((unsigned short)(SMARTHOMETELCOMCTRLSERVER_PORT));

#endif
}

void CAgentCuOverHttp::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}

int CAgentCuOverHttp::SendSmsMsg(const std::string &strUri, const std::string &strSession, const std::string &strMsg, const std::string &matchRequest)
{
    CCuOverHttpKeepConnector *p = m_pHandlerCuOverHttp->GetSocketBySessionID(strSession);

    if (p)
    {
        return p->SendSmsMsg(strUri, strMsg, matchRequest);
    }
    else
    {
        __trip;
        printf("Not found session:%s\n", strSession.c_str());
    }

    return -1;
}
