/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgent.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgent.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALAGENT_H
#define _GENERALAGENT_H

#include <time.h>
#include <vector>
#include "../UniTransDef.h" /* default def */

#include "ListenSocket.h"

#include "EZThread.h"
#include "EZTimer.h"

// 反应堆
#include "GeneralAgentHandler.h"
// tcp server
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#include "GeneralAgentTcpSocketServer.h"
#endif
#ifdef EXAMPLE_GENERALAGENTHTTPSERVER
#include "GeneralAgentHttpServer.h"
#endif
#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER
#include "GeneralAgentUdpSocket.h"
#endif
// tcp client, auto conn
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#include "GeneralAgentTcpSocketConnector.h"
#endif
// HTTP client, auto conn
#ifdef EXAMPLE_GENERALAGENTHTTPSOCKETCONNECTOR
#include "GeneralAgentHttpSocketConnector.h"
#endif
// http client, auto conn when brocken
#ifdef EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR
#include "GeneralAgentHttpKeepConnector.h"
#endif

#ifdef EXAMPLE_SOCKET_STATUS_AGENT
#include "SocketStatusAgent.h"
#endif
#ifdef _FUNC_CONSOLE_OVERTCP
#include "GeneralConsoleOverTcp.h"
#endif

typedef struct __generalservercfg_t
{
    unsigned short IsUse;
    unsigned short Port;
    std::string strHost;
    // will find by name
    std::string strServerName;

    int iConnTimeOut;//超时时间
    int iConnTimes; //连接尝试次数（保留）

    int iInterval; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    int bEnableRecon ;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    std::string strUName;
    std::string strUPwd;

    __generalservercfg_t()
    {
        IsUse = 0;
        Port = 0;
        strHost = "";
        // will find by name
        strServerName="";

        iConnTimeOut = 0;//超时时间
        iConnTimes = 0; //连接尝试次数（保留）

        iInterval = 0; //[in] 重连间隔，单位毫秒，参数默认值为30秒
        bEnableRecon  = 0;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

        strUName = "";
        strUPwd = "";
    }
}
GENERALSERVERCFG_T;


typedef struct __GeneralAgentCfg
{
    // TCP服务端口
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETSERVER
    unsigned short PortTcpServer; // 如果不启用，则设置为0
#endif
#ifdef EXAMPLE_GENERALAGENTHTTPSERVER

    unsigned short PortHttpServer; // 如果不启用，则设置为0
#endif
    // UDP服务端口
#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER

    unsigned short UdpServerPort; // 如果不启用，则设置为0
#endif
    // 状态服务端口
#ifdef EXAMPLE_SOCKET_STATUS_AGENT

    unsigned short StatusServicePort; // 如果不启用，则设置为0
#endif
    //调试命令行接口， 0无效
#ifdef _FUNC_CONSOLE_OVERTCP

    unsigned short ConsoleOverTcpPort; // 调试命令行接口， 0无效
#endif
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
    // tcp 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<GENERALSERVERCFG_T> TcpHost;
#endif

#if defined(EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR) || defined(EXAMPLE_GENERALAGENTHTTPSOCKETCONNECTOR)
    // http 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<GENERALSERVERCFG_T> HttpHost;
#endif

}
GENERALAGENTCFG_T;

#define g_MaintainAgent (*CMaintainAgent::instance())

class CMaintainAgent : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CMaintainAgent);
    CMaintainAgent();
    virtual ~CMaintainAgent();
    ////////
    void Initialize(int argc, char * argv[]);

#ifdef EXAMPLE_SIGNAL_ONDATA

    EZTHREAD_BOOL Start(CEZObject * pObj=NULL, SIG_GeneralAgentHandler_DATA::SigProc pProc=NULL);
    EZTHREAD_BOOL Stop(CEZObject * pObj=NULL, SIG_GeneralAgentHandler_DATA::SigProc pProc=NULL);
#else

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
#endif //EXAMPLE_SIGNAL_ONDATA

    void ThreadProc();

    void Run();

private:
    // 设置配置参数
    void SetConfig(GENERALAGENTCFG_T *pConfig=NULL);

    // 设置服务，
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETSERVER
    // port =0 disable, others use it
    void SetTcpServer(unsigned short  port);
#endif
#ifdef EXAMPLE_GENERALAGENTHTTPSERVER

    void SetHttpServer(unsigned short  port);
#endif
#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER

    void SetUdpServer(unsigned short  port);
#endif
public:

#ifdef EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR

    void AddTcpHost(std::string strHost, int nPort, std::string strName="");
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#if defined(EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR) || defined(EXAMPLE_GENERALAGENTHTTPSOCKETCONNECTOR)

    void AddHttpHost(std::string strHost, int nPort, std::string strServerName=CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME);
#endif

    CMaintainHandler *GetHandler();

public:
    int ConnectorCreate();

    // 内部需要一定的时间自行处理
    // 为确保内部销毁完成，一般调用之后， 10s之后再调用Connect，
    int ConnectorDestroy();


    int SendBySocket(std::string strSocketName, const char *pData, size_t Len);
    Socket * GetSocketByName(std::string strServerName);


    void TimerProcMaintain();
    const std::string &GetVersion();
    unsigned int GetRunPeriod();
    time_t GetStartTime();

#ifdef EXAMPLE_SIGNAL_ONDATA

    EZTHREAD_BOOL OnData(Socket *pSocket, const char *pdat, size_t len);
#endif //EXAMPLE_SIGNAL_ONDATA

private:
    GENERALAGENTCFG_T m_ConfigGeneralAgent;
    CMaintainAgentLog *m_Log;
    CMaintainHandler *m_pHandlerMaintainAgent;

#ifdef EXAMPLE_SOCKET_STATUS_AGENT

    ListenSocket<CSocketStatusAgent> *m_pSocketStatusAgent;
#endif // EXAMPLE_SOCKET_STATUS_AGENT

#ifdef _FUNC_CONSOLE_OVERTCP

    ListenSocket<CMaintainConsoleOverTcp> *m_pGeneralConsoleOverTcp;
#endif // _FUNC_CONSOLE_OVERTCP

#ifdef EXAMPLE_GENERALAGENTTCPSOCKETSERVER

    ListenSocket<CGeneralAgentTcpSocketServer> *m_pGeneralAgentTcpSocketServer;
#endif // EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#ifdef EXAMPLE_GENERALAGENTHTTPSERVER

    ListenSocket<CGeneralAgentHttpServer> *m_pGeneralAgentHttpServer;
#endif // EXAMPLE_GENERALAGENTHTTPSERVER


#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER

    CGeneralAgentUdpSocket *m_pGeneralAgentUdpSocketServer;
#endif

    int InitializeLogs();

#ifdef EXAMPLE_SIGNAL_ONDATA

    CEZMutex m_MutexSig;
    SIG_GeneralAgentHandler_DATA m_sigData;
#endif //EXAMPLE_SIGNAL_ONDATA
}
; //CMaintainAgent

#endif //_GENERALAGENT_H
