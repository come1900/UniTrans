/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerSmsOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerSmsOverHttp.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerSmsOverHttp_H
#define _HandlerSmsOverHttp_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"

#include "HttpServerSmsOverHttp.h"

#include "../../Maintain/GeneralAgentLog.h"

class CHandlerSmsOverHttp : public SocketHandler
{
public:
    CHandlerSmsOverHttp(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();

    Socket *GetSocketBySocketName(std::string strServerName);

    CHttpServerSmsOverHttp *GetSocketBySessionID(const std::string &strSessionID);

    void BroadCast();
    // 新验证
    //int newSession(const CSession &newss);

    // 鉴权
    int AuthSession(std::string strSession, int iAccessId=0);

private:
    // strSessionId
    //CSessionManager m_requestSession;
};

#endif // _HandlerSmsOverHttp_H
