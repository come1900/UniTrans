/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveStatusServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveStatusServer.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerLiveStatusServer_H
#define _HandlerLiveStatusServer_H

#include <SocketHandler.h>
#include <SocketHandlerEp.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"
#include "../../DBAgent/CacheKeyValue.h"

#include "HttpServerLiveStatusServer.h"

#include "../../Maintain/GeneralAgentLog.h"

#if defined(_USE_MODULES_LocalDataBase)
#include "../../DBAgent/DbLiveRecord.h"
#endif // _USE_MODULES_LocalDataBase

class CHandlerLiveStatusServer : public SocketHandlerEp,  public CEZObject
{
public:
    CHandlerLiveStatusServer(CMaintainAgentLog *pLog=NULL);
    ~CHandlerLiveStatusServer();

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();
    void Start();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 获取播放数量
    int getPlayerNums(const std::string &strStreamID);

    // 播放时间
    time_t getLastPlayTime(const std::string &strStreamID);
    // 发布时间
    // >0 ok
    // 0 not publish, offline ...
    time_t getLastUpdate(const std::string &strStreamID);
    // 返回正在发布的列表
    int listPublished(std::vector<std::string> &vPublishedID);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
public:

#if defined(def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer)
    // 节点信息收集, 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb;
#endif //def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer
    // 数据采集用, 不使用数据库持久化
    CCacheKeyValue *m_pCacheKeyValue;
};

#endif // _HandlerLiveStatusServer_H

