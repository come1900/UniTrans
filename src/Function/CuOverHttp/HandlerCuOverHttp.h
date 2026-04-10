/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerCuOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerCuOverHttp.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerCuOverHttp_H
#define _HandlerCuOverHttp_H

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../UniTransDef.h"

#include "CuOverHttpKeepConnector.h"

#include "../../Maintain/GeneralAgentLog.h"

class CHandlerCuOverHttp : public SocketHandler
{
public:
    CHandlerCuOverHttp(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Update();

    Socket *GetSocketByName(std::string strServerName);

    CCuOverHttpKeepConnector *GetSocketBySessionID(std::string strSessionID);

    // ¼øÈ¨
    int AuthSession(std::string strSession, int iAccessId=0);

private:
    // strSessionId
    //CSessionManager m_requestSession;
};

#endif // _HandlerCuOverHttp_H
