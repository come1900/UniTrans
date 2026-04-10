/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerNvpRegisterServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerNvpRegisterServer.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerNvpRegisterServer_H
#define _HandlerNvpRegisterServer_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"

#include "HttpServerNvpRegisterServer.h"

#include "../../Maintain/GeneralAgentLog.h"


class CHandlerNvpRegisterServer : public SocketHandler,  public CEZObject
{
public:
    CHandlerNvpRegisterServer(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CStreamContainer *GetPandoraContainer();

CStreamContainer *m_pChannelContainer;
    
#if defined(_FUNC_NvpRegisterServer_SingleServer)
    CStreamContainer *GetPandoraContainer4Channel();
private:
    //直播 通道-- 单机版使用
    CStreamContainer *m_pChannelContainer;
#endif //_FUNC_NvpRegisterServer_SingleServer

};

#endif // _HandlerNvpRegisterServer_H

