/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveStreammingServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveStreammingServer.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerLiveStreammingServer_H
#define _HandlerLiveStreammingServer_H

#include <SocketHandler.h>

//#include <SocketHandler.h>
//#include <TcpSocket.h>
#include <map>

#include "../UniService/DefUniService.h"
#include "../../UniTransDef.h"
#include "../../DBAgent/CacheKeyValue.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveStreammingServer.h"
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif //_USE_MODULES_EZCONFIG

#include "HttpServerLiveStreammingServer.h"

#include "../../Maintain/GeneralAgentLog.h"
#include "../Interface/HandlerGRestHttpdSocket.h"

class CHandlerLiveStreammingServer : public CHandlerGRestHttpdSocket//SocketHandler,  public CEZObject
{
public:
    CHandlerLiveStreammingServer(CMaintainAgentLog *pLog=NULL);
    ~CHandlerLiveStreammingServer();

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();
    void Start();
    void Disconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CConfigLiveStreammingServer *m_pPssCfg;
    CConfigPullTykdMobileService *m_pPullCfg;
};

#endif // _HandlerLiveStreammingServer_H

