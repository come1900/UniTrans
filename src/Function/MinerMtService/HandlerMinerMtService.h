/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerMinerMtService.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: HandlerMinerMtService.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerMinerMtService_H
#define _HandlerMinerMtService_H

#include <SocketHandlerEp.h>
#include <TcpSocket.h>

#include "../../UniTransDef.h"
#include "../../DBAgent/CacheKeyValue.h"

#include "../../Maintain/GeneralAgentLog.h"

#include "HttpServerMinerMtService.h"

class CHandlerMinerMtService : public SocketHandlerEp,  public CEZObject
{
public:
    CHandlerMinerMtService(CMaintainAgentLog *pLog=NULL);
    ~CHandlerMinerMtService();

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();
    void Start();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
public:
    // 节点信息收集, 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb;
    // 数据采集用, 不使用数据库持久化
    CCacheKeyValue *m_pCacheKeyValue;
};

#endif // _HandlerMinerMtService_H

