/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentInterThings_gLock.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentInterThings_gLock.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentInterThings_gLock_H
#define _AgentInterThings_gLock_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

// 反应堆
#include "HandlerInterThings_gLock.h"
#include "../../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerInterThings_gLock.h"

typedef struct __AgentInterThings_gLockCfg
{
    unsigned short HttpServicePort; // 如果不启用，则设置为0
}
AgentInterThings_gLockCfg_T;

#define g_AgentInterThings_gLock (*CAgentInterThings_gLock::instance())

class CAgentInterThings_gLock : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentInterThings_gLock);
    CAgentInterThings_gLock();
    virtual ~CAgentInterThings_gLock();
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

    int SendBySession(const std::string &strServerSID, const std::string &stack, const std::string &matchRequest="");
    	
    CHandlerInterThings_gLock *GetHandler();

private:
    AgentInterThings_gLockCfg_T m_ConfigOfInterThings_gLock;
    CHandlerInterThings_gLock *m_pHandlerInterThings_gLock;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerInterThings_gLock> *m_pCHttpServerInterThings_gLock;

}
; //CAgentInterThings_gLock
#endif //_AgentInterThings_gLock_H
