/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPandoraNode.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPandoraNode.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentPandoraNode_H
#define _AgentPandoraNode_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>
// 将使用sms stack
#include "../../Stack/SmsStack.h"

// 反应堆
#include "HandlerPandoraNode.h"
#include "../../../Maintain/GeneralAgentLog.h"

//
#include "PandoraNodeKeepConnector.h"

typedef struct __AgentPandoraNodeCfg
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

    __AgentPandoraNodeCfg()
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
AgentPandoraNodeCfg_T;

typedef enum __msg_PandoraNode_t {
    MSG_PANDORANODE_T_CONNECT_SUCCEEDED = 800,		    	// 链接成功
    MSG_PANDORANODE_T_CONNECT_TRYING,		    	        // 链接尝试中
    MSG_PANDORANODE_T_CONNECT_TRY_FAILED,		    	        // 链接尝试失败
    MSG_PANDORANODE_T_CONNECT_FAILED,		    	        // 链接失败
}MSG_PandoraNode_T;
typedef struct __MSG_PandoraNodeData_t
{
    int iConnTimes; //连接尝试次数（保留）
    time_t ttFirstConnTime;//超时时间
    time_t ttLastConnTime;//超时时间
    __MSG_PandoraNodeData_t()
    {
        iConnTimes = 0; //连接尝试次数（保留）
        ttFirstConnTime = 0;//超时时间
        ttLastConnTime = 0;//超时时间
    }
}
MSG_PandoraNodeData_T;

#define g_AgentPandoraNode (*CAgentPandoraNode::instance())

class CAgentPandoraNode : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentPandoraNode);
    CAgentPandoraNode();
    virtual ~CAgentPandoraNode();
    ////////
    // cmd, partition, len, time
    typedef TSignal2<int, void *> PandoraNodeSignal_t;
    typedef PandoraNodeSignal_t::SigProc PandoraNodeSignalProc_t;

    //EZTHREAD_BOOL Start();
    //EZTHREAD_BOOL Stop();
    EZTHREAD_BOOL Start(CEZObject * pObj, PandoraNodeSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, PandoraNodeSignalProc_t pProc);

    void ThreadProc();
    void SetConfig();

    // 设置服务，
    void SetGeneralHttpAgent(unsigned short  port);
    void ConnectAll();

    void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    CHandlerPandoraNode *GetHandler();

private:
    AgentPandoraNodeCfg_T m_ConfigOfPandoraNode;
    CHandlerPandoraNode *m_pHandlerPandoraNode;
    CMaintainAgentLog m_Log;

    ListenSocket<CPandoraNodeKeepConnector> *m_pCPandoraNodeKeepConnector;

    PandoraNodeSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;
    //计数
    int				m_iUser;
}
; //CAgentPandoraNode
#endif //_AgentPandoraNode_H
