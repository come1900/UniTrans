/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPuOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPuOverHttp.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefPuOverHttp.h"

#include "AgentPuOverHttp.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigPuOverHttp.h"
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


PATTERN_SINGLETON_IMPLEMENT(CAgentPuOverHttp);
void CAgentPuOverHttp::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerPuOverHttp->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerPuOverHttp->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    m_pHandlerPuOverHttp->Select(1,0);
    m_pHandlerPuOverHttp->Update();
}

CAgentPuOverHttp::CAgentPuOverHttp():CEZThread("CAgentPuOverHttp", THREAD_PRIORITY_DEFAULT, 10)
{
    m_ConfigOfPuOverHttp.HttpServicePort = 60088; // 如果不启用，则设置为0

    m_pHandlerPuOverHttp = new CHandlerPuOverHttp(&m_Log);
    assert(m_pHandlerPuOverHttp);

    ///////////////////////////////////////////////
}
CAgentPuOverHttp::~CAgentPuOverHttp()
{
    m_ConfigOfPuOverHttp.HttpServicePort = 0;
    // handler里面自管理
    m_pCPuOverHttpKeepConnector = NULL;

    if (m_pHandlerPuOverHttp)
    {
        delete m_pHandlerPuOverHttp;
    }
}

CHandlerPuOverHttp * CAgentPuOverHttp::GetHandler()
{
    return m_pHandlerPuOverHttp;
}

void CAgentPuOverHttp::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfPuOverHttp.HttpServicePort = port;
}

int CAgentPuOverHttp::SendBySocketName(std::string strServerName, const char *pData, size_t Len)
{
    Socket *p = m_pHandlerPuOverHttp->GetSocketByName(strServerName);

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

int CAgentPuOverHttp::SendSmsMsg(const std::string &strUri, const std::string &strSession, const std::string &strMsg, const std::string &matchRequest)
{
    CPuOverHttpKeepConnector *p = m_pHandlerPuOverHttp->GetSocketBySessionID(strSession);

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


void CAgentPuOverHttp::ConnectAll()
{
    CPuOverHttpKeepConnector *_pConnector
    = new CPuOverHttpKeepConnector(*m_pHandlerPuOverHttp, m_ConfigOfPuOverHttp.strServerName);
    if (_pConnector)
    {
        printf("PuOverHttpConnector(%s:%d) Open "
               , m_ConfigOfPuOverHttp.strHost.c_str()
               , m_ConfigOfPuOverHttp.Port);

        bool bRet = _pConnector->Open(m_ConfigOfPuOverHttp.strHost.c_str(), m_ConfigOfPuOverHttp.Port);
        if (bRet)
        {
            printf("success\n");

            m_pHandlerPuOverHttp->Add(_pConnector);
        }
        else
        {
            //__trip;
            // 失败处理
            printf("failed.\n");
        }
    }
    else
    {
        __trip;
        //失败处理
    }

}

void CAgentPuOverHttp::DisconnectAll()
{}

EZTHREAD_BOOL CAgentPuOverHttp::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    // 设置连接参数
    SetConfig();

    ConnectAll();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentPuOverHttp::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentPuOverHttp::SetConfig()
{
    m_ConfigOfPuOverHttp.IsUse = 1;

    // 在下面设置
    //m_ConfigOfPuOverHttp.Port = DEFAULT_SMS_HOST_PORT;
    //m_ConfigOfPuOverHttp.strHost = DEFAULT_SMS_HOST;

    // will find by name
    m_ConfigOfPuOverHttp.strServerName = CPuOverHttpKeepConnector_SOCKET_NAME;

    m_ConfigOfPuOverHttp.iConnTimeOut = 5;//超时时间
    m_ConfigOfPuOverHttp.iConnTimes = 5; //连接尝试次数（保留）

    m_ConfigOfPuOverHttp.iInterval = 5; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    m_ConfigOfPuOverHttp.bEnableRecon  = 1;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    //m_ConfigOfPuOverHttp.strUName = "root";
    //m_ConfigOfPuOverHttp.strUPwd = "admin";

    //return;

#ifdef _USE_MODULES_EZCONFIG

    CConfigPuOverHttp __cfg;
    __cfg.update();

    m_ConfigOfPuOverHttp.Port = __cfg.getConfig().iSmsPort;
    m_ConfigOfPuOverHttp.strHost = __cfg.getConfig().strSmsHost;
    m_ConfigOfPuOverHttp.strUName = __cfg.getConfig().strAccount;
    m_ConfigOfPuOverHttp.strUPwd = __cfg.getConfig().strPasswd;
#else
    // 默认情况
    m_ConfigOfPuOverHttp.Port = DEFAULT_SMS_HOST_PORT;
    m_ConfigOfPuOverHttp.strHost = DEFAULT_SMS_HOST;
    m_ConfigOfPuOverHttp.strUName = "root";
    m_ConfigOfPuOverHttp.strUPwd = "admin";
#endif
}

void CAgentPuOverHttp::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
