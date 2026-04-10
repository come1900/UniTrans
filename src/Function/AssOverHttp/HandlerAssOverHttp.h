/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerAssOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerAssOverHttp.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerAssOverHttp_H
#define _HandlerAssOverHttp_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"

#include "HttpServerAssOverHttp.h"

#include "../../Maintain/GeneralAgentLog.h"

class CHandlerAssOverHttp : public SocketHandler
{
public:
    CHandlerAssOverHttp(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();

    Socket *GetSocketByName(std::string strServerName);

    Socket *GetSocketBySessionID(std::string strSessionID);

    // 新验证
    //int newSession(const CSession &newss);

    // 鉴权
    int AuthSession(std::string strSession, int iAccessId=0);

private:
    // strSessionId
    //CSessionManager m_requestSession;
};

#endif // _HandlerAssOverHttp_H
