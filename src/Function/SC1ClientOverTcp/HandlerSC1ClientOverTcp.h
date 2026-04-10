/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerSC1ClientOverTcp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerSC1ClientOverTcp.h 5884 2015-01-05 14:04:57Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerSC1ClientOverTcp_H
#define _HandlerSC1ClientOverTcp_H

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"

#include "SC1ClientOverTcpKeepConnector.h"

#include "../../Maintain/GeneralAgentLog.h"

class CHandlerSC1ClientOverTcp : public SocketHandler
{
public:
    CHandlerSC1ClientOverTcp(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();

    Socket *GetSocketByName(std::string strServerName);

    CSC1ClientOverTcpKeepConnector *GetSocketBySessionID(std::string strSessionID);

    // ¼øÈ¨
    int AuthSession(std::string strSession, int iAccessId=0);
    	int SendCmd(const int cmdNo);

private:
    // strSessionId
    //CSessionManager m_requestSession;
};

#endif // _HandlerSC1ClientOverTcp_H
