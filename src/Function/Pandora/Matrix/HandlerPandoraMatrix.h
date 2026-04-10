/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerPandoraMatrix.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerPandoraMatrix.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerPandoraMatrix_H
#define _HandlerPandoraMatrix_H

#include <SocketHandlerEp.h>
#include <TcpSocket.h>

//#include "../../UniService/DefUniService.h"
#include "../../../UniTransDef.h"

#include "../../../DBAgent/CacheKeyValue.h"

//#include "HttpServerPandoraMatrix.h"
#include "../../Stack/pq/PqStack.h"

#include "../../../Maintain/GeneralAgentLog.h"
#include "../../Interface/HandlerGRestHttpdSocket.h"

class CHttpServerPandoraMatrix;

class CHandlerPandoraMatrix : public CHandlerGRestHttpdSocket//SocketHandlerEp
{
public:
    CHandlerPandoraMatrix(CMaintainAgentLog *pLog=NULL);
    ~CHandlerPandoraMatrix();

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel);
    void List(std::string &strDump, int From, int To);
    void List(CPqMsg_requestMsgDeviceListAck &DeviceLis
		, const std::string strGroupID=""
		, const int strPageSize=0
		, const int strPages=0);

    // 获取设备信息
    // ret 0-Found, -1-not found
    int getNode(CDeviceMsg *pTheDevice, const std::string strDeviceID);

    void Start();
    void Update();

    //使用名字取
    CHttpServerPandoraMatrix *GetPandoraNode(const std::string &strNodeName);
    int Do_MatchReq(const std::string &strUrl, const CPqMsg_requestMsg &msgReq);
    int On_MatchReqAck(const std::string &strUrl, const CPqMsg_requestMsgAck &msgAck);


    // strReqSession 用来找寻调用者， 异步
    int Do_MatchReq(const std::string &strUrl, const std::string &strReq, const std::string &strNodeID, const std::string &strReqSession);
    int On_MatchReqAck(const std::string &strUrl, const std::string &strReqSession, const void *pMsg);

private:
    // of tykd
    CEZMutex m_MutexMatchReq;//for call Do_MatchReq

public:

    // 节点信息收集, 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb;
    // 数据采集用, 不使用数据库持久化
    CCacheKeyValue *m_pCacheKeyValue;

    // 目前用来收集终端配置
    CCacheKeyValue *m_pCacheWithDb_NodeConfig;
};

#endif // _HandlerPandoraMatrix_H
