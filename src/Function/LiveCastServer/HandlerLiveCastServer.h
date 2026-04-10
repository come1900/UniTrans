/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveCastServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveCastServer.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerLiveCastServer_H
#define _HandlerLiveCastServer_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../UniService/DefUniService.h"
#include "../../UniTransDef.h"

#include "HttpServerLiveCastServer.h"

#include "../../Maintain/GeneralAgentLog.h"
#if defined(_USE_MODULES_LocalDataBase)
#include "../../DBAgent/DbLiveCast.h"
#endif // _USE_MODULES_LocalDataBase

class CHandlerLiveCastServer : public SocketHandler,  public CEZObject
{
public:
    CHandlerLiveCastServer(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CStreamContainer *GetPandoraContainer();

CStreamContainer *m_pChannelContainer;
    
     // 保存本地数据
#if defined(_USE_MODULES_LocalDataBase)
    CDbLiveCast *m_dbaLiveRecord;
#endif // _USE_MODULES_LocalDataBase

//#if defined(_FUNC_LiveCastServer_SingleServer)
//    CStreamContainer *GetPandoraContainer4Channel();
//private:
//    //直播 通道-- 单机版使用
//    CStreamContainer *m_pChannelContainer;
//#endif //_FUNC_LiveCastServer_SingleServer

};

#endif // _HandlerLiveCastServer_H

