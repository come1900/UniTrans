/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentCuOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentCuOverHttp.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentCuOverHttp_H
#define _AgentCuOverHttp_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>
// 将使用sms stack
#include "../Stack/SmsStack.h"

// 反应堆
#include "HandlerCuOverHttp.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "CuOverHttpKeepConnector.h"

typedef struct __AgentCuOverHttpCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0


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

    __AgentCuOverHttpCfg()
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
AgentCuOverHttpCfg_T;

#define g_AgentCuOverHttp (*CAgentCuOverHttp::instance())

class CAgentCuOverHttp : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentCuOverHttp);
    CAgentCuOverHttp();
    virtual ~CAgentCuOverHttp();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
    void SetDefaultConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void ConnectAll();

    void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    int SendBySocketName(std::string strSocketName, const char *pData, size_t Len);
    int SendSmsMsg(const std::string &strUri, const std::string &strSession, const std::string &strMsg, const std::string &matchRequest="");
    CHandlerCuOverHttp *GetHandler();

private:
    AgentCuOverHttpCfg_T m_ConfigOfCuOverHttp;
    CHandlerCuOverHttp *m_pHandlerCuOverHttp;
    CMaintainAgentLog m_Log;

    ListenSocket<CCuOverHttpKeepConnector> *m_pCCuOverHttpKeepConnector;

}
; //CAgentCuOverHttp
#endif //_AgentCuOverHttp_H
