/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * NetSerial.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: NetSerial.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <iostream>

#include "NetSerial.h"

//#include <ez_fs.h>
#ifdef _UNIT_TEST_VER
#include "Logs.h"
#else
#include "../../../Logs.h"
#endif

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigNetSerialTcpCli.h"
#endif //_USE_MODULES_EZCONFIG

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
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


PATTERN_SINGLETON_IMPLEMENT(CNetSerial);
void CNetSerial::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerNetSerial->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerNetSerial->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    // 读写和维护socket
    m_pHandlerNetSerial->Select(1,0);
    // 其他操作，自定义
    m_pHandlerNetSerial->Update();
}

CNetSerial::CNetSerial():CEZThread("NetSerial", THREAD_PRIORITY_DEFAULT, 10)
        , m_sigData(1)
{
    m_pHandlerNetSerial = new CNetSerialHandler(&m_Log);
    assert(m_pHandlerNetSerial);
}
CNetSerial::~CNetSerial()
{
    if (m_pHandlerNetSerial)
    {
        delete m_pHandlerNetSerial;
    }
}

void CNetSerial::AddTcpHost(std::string strHost, int nPort, std::string strServerName)
{
    // todo 检查不要重复加
    NetSerialSERVERCFG_T __peer;
    __peer.Port = nPort;

    __peer.strHost = strHost;
    __peer.strServerName = strServerName;

    m_ConfigNetSerial.TcpHost.push_back(__peer);
}

int CNetSerial::Send2Socket(std::string strServerName, const unsigned char *pData, size_t Len)
{
    if (m_pHandlerNetSerial==NULL)
    {
        return -1;
    }

    CEZLock _lock(m_MutexSendData);

    Socket *p = m_pHandlerNetSerial->GetSocketByName(strServerName);

    if (p)
    {
        p->SendBuf((const char*)pData, Len);
    }
    else
    {
        __trip;
        printf("Send2Socket %s, Len:%ul failed\n", strServerName.c_str(), Len);
    }

    return 0;
}

Socket * CNetSerial::GetSocketByName(std::string strServerName)
{
    if (m_pHandlerNetSerial==NULL)
    {
        __trip;
        return NULL;
    }

    return m_pHandlerNetSerial->GetSocketByName(strServerName);
}

int CNetSerial::ConnectorCreate()
{
    for (unsigned int ii=0; ii<m_ConfigNetSerial.TcpHost.size(); ii++)
    {
        CNetSerialTcpSocketConnector *_pConnector = new CNetSerialTcpSocketConnector(*m_pHandlerNetSerial
                , m_ConfigNetSerial.TcpHost[ii].strServerName
                , m_ConfigNetSerial.TcpHost[ii].iConnTimeOut
                , m_ConfigNetSerial.TcpHost[ii].bEnableRecon);
        if (_pConnector)
        {
            //                            DEB_CODE(
            printf("Connector[%s]:%d Host--%s:%d Open "
                   , m_ConfigNetSerial.TcpHost[ii].strServerName.c_str()
                   , ii
                   , m_ConfigNetSerial.TcpHost[ii].strHost.c_str()
                   , m_ConfigNetSerial.TcpHost[ii].Port);
            //			);
            bool bRet = _pConnector->Open(m_ConfigNetSerial.TcpHost[ii].strHost.c_str(), m_ConfigNetSerial.TcpHost[ii].Port);
            if (bRet)
            {
                //                            DEB_CODE(
                printf("Succeeded.\n");
                //			);

                m_pHandlerNetSerial->Add(_pConnector);
            }
            else
            {
                //                 DEB_CODE(
                printf("Failed.\n");
                //		);
                __trip;
                // 失败处理
                delete _pConnector;
            }
        }
        else
        {
            __trip;
            //失败处理
        }
    }

    return 0;
}

int CNetSerial::ConnectorDestroy()
{
    assert(m_pHandlerNetSerial);

    return m_pHandlerNetSerial->CloseAndDeleteAll();

    // 关闭客户端
    for (size_t ii=0; ii<m_ConfigNetSerial.TcpHost.size(); ii++)
    {
        printf("Disconnect %d -- %s\n", ii, m_ConfigNetSerial.TcpHost[ii].strHost.c_str());
        // todo
    }

    return 0;
}

EZTHREAD_BOOL CNetSerial::Start(CEZObject * pObj, SIG_CNetSerialHandler_DATA::SigProc pProc)
{
    if (pObj && pProc)
    {
        CEZLock _lock(m_MutexSig);

        if(m_sigData.Attach(pObj, pProc) < 0)
        {
            ez_trace("attach error\n");
            return EZTHREAD_BOOL_FALSE;
        }
    }

    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    ConnectorCreate();

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

#ifdef EXAMPLE_SIGNAL_ONDATA
EZTHREAD_BOOL CNetSerial::Stop(CEZObject * pObj, SIG_CNetSerialHandler_DATA::SigProc pProc)
#else
EZTHREAD_BOOL CNetSerial::Stop()
#endif //EXAMPLE_SIGNAL_ONDATA
{
#ifdef EXAMPLE_SIGNAL_ONDATA
    if (pObj && pProc)
    {
        CEZLock _lock(m_MutexSig);

        if(m_sigData.Detach(pObj, pProc) < 0)
        {
            ez_trace("detach error\n");
            return EZTHREAD_BOOL_FALSE;
        }
    }
#endif //EXAMPLE_SIGNAL_ONDATA

    m_bLoop = EZTHREAD_BOOL_FALSE;

    return EZTHREAD_BOOL_TRUE;
}

void CNetSerial::SetConfig(NetSerialCFG_T *pConfig)
{
    if (pConfig)
    {
        m_ConfigNetSerial = *pConfig;

        return;
    }

    // 调试的时候使用默认参数
    __fline;
    printf("SetConfig\n");

#ifdef _USE_MODULES_EZCONFIG

    CConfigNetSerialTcpCli __cfg;
    __cfg.update();

    std::string strHost = __cfg.getConfig().strHost;
    int iPort = __cfg.getConfig().iPort;
    std::string strServiceName = __cfg.getConfig().strSName;
#else

    std::string strHost = DEFAULT_NET_SERIAL_HOST;
    int iPort = DEFAULT_NET_SERIAL_HOST_PORT;
    std::string strServiceName = DEFAULT_NET_SERIAL_SERVICE_NAME;
#endif //   _USE_MODULES_EZCONFIG

    //AddTcpHost(DEFAULT_NET_SERIAL_HOST, DEFAULT_NET_SERIAL_HOST_PORT, DEFAULT_NET_SERIAL_SERVICE_NAME);
    AddTcpHost(strHost, iPort, strServiceName);
}

void CNetSerial::ThreadProc()
{
    std::cout << ">>>>>>>>>>>>>> CNetSerial::ThreadProc" << std::endl;
#ifdef EXAMPLE_SIGNAL_ONDATA

    m_pHandlerNetSerial->AddSignal(this, (SIG_CNetSerialHandler_DATA::SigProc)&CNetSerial::OnData);
#endif //EXAMPLE_SIGNAL_ONDATA

    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)

#ifdef EXAMPLE_SIGNAL_ONDATA
    m_pHandlerNetSerial->DelSignal(this, (SIG_CNetSerialHandler_DATA::SigProc)&CNetSerial::OnData);
#endif //EXAMPLE_SIGNAL_ONDATA

    std::cout << "CNetSerial::ThreadProc >>>>>>>>>>>" << std::endl;
}

void CNetSerial::Initialize(int argc, char * argv[])
{
    SetConfig();

    LOG4CPLUS_INFO(LOG_SOLAR, "Starting(" << "CNetSerial::Initialize" << ") ...");
}

void YourFunction(const char *pSocketName, const char *pdat, size_t len)
{
    // your code
    __fline;
    printf("%s From:%s Len:%d Char[0]:%c\n", __FUNCTION__, pSocketName, len, pdat[0]);

}

EZTHREAD_BOOL CNetSerial::OnData(Socket *pSocket, const char *pdat, size_t len)
{
    if((pdat == NULL) || (len <= 0))
    {
        return false;
    }
    // on sock
    m_sigData(pSocket, pdat, len);

    YourFunction(pSocket->GetSockName().c_str(), pdat, len);

    // your code
    __fline;
    printf("OnData From:%s Len:%d Char[0]:%c\n", pSocket->GetSockName().c_str(), len, pdat[0]);

    return EZTHREAD_BOOL_TRUE;
}

