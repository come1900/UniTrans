/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPandoraMatrix.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPandoraMatrix.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentPandoraMatrix_H
#define _AgentPandoraMatrix_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigPandoraMatrix.h"
#endif //_USE_MODULES_EZCONFIG

// 反应堆
#include "HandlerPandoraMatrix.h"
#include "../../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerPandoraMatrix.h"
#include "UdpServerPandoraMatrix.h"

//typedef struct __AgentPandoraMatrixCfg
//{
//    unsigned short HttpServicePort; // 如果不启用，则设置为0
//}
//AgentPandoraMatrixCfg_T;

#define g_AgentPandoraMatrix (*CAgentPandoraMatrix::instance())

class CAgentPandoraMatrix : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentPandoraMatrix);
    CAgentPandoraMatrix();
    virtual ~CAgentPandoraMatrix();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
    //void SetDefaultConfig();

    // 设置服务，
    //void SetGeneralHttpAgent(unsigned short  port);
    void ConnectAll();

    //void ConnectAllSync();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void DisconnectAll();

    void Run();

    //int MatchReq(const std::string &strUrl, const CPandoraMsg_TykdPullStart &msgReq);
    //int MatchReq(const std::string &strUrl, const std::string &strReq, const std::string &strStreamID, const std::string &strMsgSession);

    CHandlerPandoraMatrix *GetHandler();

private:
    //AgentPandoraMatrixCfg_T m_ConfigOfPandoraMatrix;
    CHandlerPandoraMatrix *m_pHandlerPandoraMatrix;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerPandoraMatrix> *m_pCHttpServerPandoraMatrix;
    CUdpServerPandoraMatrix *m_pUdpServerPandoraMatrixServer;

}
; //CAgentPandoraMatrix
#endif //_AgentPandoraMatrix_H
