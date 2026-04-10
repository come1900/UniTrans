/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveRecordServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveRecordServer.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerLiveRecordServer_H
#define _HandlerLiveRecordServer_H

#include <SocketHandler.h>
#include <SocketHandlerEp.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"
#include "../../DBAgent/CacheKeyValue.h"

#include "HttpServerLiveRecordServer.h"

#include "../../Maintain/GeneralAgentLog.h"

#if defined(_USE_MODULES_LocalDataBase)
#include "../../DBAgent/DbLiveRecord.h"
#endif // _USE_MODULES_LocalDataBase
#include "../Interface/HandlerGRestHttpdSocket.h"

class CHandlerLiveRecordServer : public CHandlerGRestHttpdSocket//,  public CEZObject
{
public:
    CHandlerLiveRecordServer(CMaintainAgentLog *pLog=NULL);
    ~CHandlerLiveRecordServer();

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();
    void Start();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
public:
    // 节点信息收集, 使用数据库持久化
    //CCacheKeyValue *m_pCacheWithDb;
    // 数据采集用, 不使用数据库持久化
    //CCacheKeyValue *m_pCacheKeyValue;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 保存本地数据
#if defined(_USE_MODULES_LocalDataBase)
    CDbLiveRecord *m_dbaLiveRecord;
#endif // _USE_MODULES_LocalDataBase

    CConfigLiveRecordServer *m_pCfgLiveRecordServer;
};

#endif // _HandlerLiveRecordServer_H

