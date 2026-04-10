/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerInterThings_gLock.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerInterThings_gLock.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerInterThings_gLock_H
#define _HandlerInterThings_gLock_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../../UniTransDef.h"

#include "HttpServerInterThings_gLock.h"

#include "../../../Maintain/GeneralAgentLog.h"

class CHandlerInterThings_gLock : public SocketHandler
{
public:
    CHandlerInterThings_gLock(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();

    Socket *GetSocketBySocketName(std::string strServerName);

    CHttpServerInterThings_gLock *GetSocketBySessionID(const std::string &strSessionID);

    // ¼øÈ¨
    int AuthSession(std::string strSession, int iAccessId=0);

private:
    // strSessionId
    //CSessionManager m_requestSession;
};

#endif // _HandlerInterThings_gLock_H
