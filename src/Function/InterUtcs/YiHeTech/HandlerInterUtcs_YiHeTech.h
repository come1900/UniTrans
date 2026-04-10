/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerInterUtcs_YiHeTech.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerInterUtcs_YiHeTech.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 16:36:02 Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerInterUtcs_YiHeTech_H
#define _HandlerInterUtcs_YiHeTech_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../../UniTransDef.h"

#include "HttpServerInterUtcs_YiHeTech.h"

#include "../../../Maintain/GeneralAgentLog.h"

class CHandlerInterUtcs_YiHeTech : public SocketHandler
{
public:
    CHandlerInterUtcs_YiHeTech(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();

    Socket *GetSocketBySocketName(std::string strServerName);

    CHttpServerInterUtcs_YiHeTech *GetSocketBySessionID(const std::string &strSessionID);

    // ¼øÈ¨
    int AuthSession(std::string strSession, int iAccessId=0);

private:
    // strSessionId
    //CSessionManager m_requestSession;
};

#endif // _HandlerInterUtcs_YiHeTech_H
