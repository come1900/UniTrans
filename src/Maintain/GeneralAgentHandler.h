/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CMaintainHandler.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CMaintainHandler.h 0001 2012-04-06 09:54:05Z WuJunjie $
 *
 *  Explain:
 *     -·´Ó¦¶Ñ-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GENERALAGENTHANDLER_H
#define _GENERALAGENTHANDLER_H
#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include "./GeneralAgentLog.h"

#ifdef EXAMPLE_SIGNAL_ONDATA
#include "EZObject.h"
#include "EZSignals.h"
typedef TSignal3<Socket *, const char *, size_t> SIG_GeneralAgentHandler_DATA;
#endif //EXAMPLE_SIGNAL_ONDATA
class CMaintainHandler : public SocketHandler
{
public:
    CMaintainHandler(CMaintainAgentLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump);
    void Update();

    Socket *GetSocketByName(std::string strServerName);
    int CloseAndDeleteAll();

#ifdef EXAMPLE_SIGNAL_ONDATA

    void OnDataCall(Socket *, const char *buf,size_t len);
    bool AddSignal(CEZObject * pObj, SIG_GeneralAgentHandler_DATA::SigProc pProc);
    bool DelSignal(CEZObject * pObj, SIG_GeneralAgentHandler_DATA::SigProc pProc);
private:

    CEZMutex m_MutexSig;
    SIG_GeneralAgentHandler_DATA m_sigData;
#endif //EXAMPLE_SIGNAL_ONDATA

private:
};

#endif // _GENERALAGENTHANDLER_H
