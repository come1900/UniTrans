/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CNetSerialHandler.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CNetSerialHandler.h 0001 2012-04-06 09:54:05Z WuJunjie $
 *
 *  Explain:
 *     -·´Ó¦¶Ñ-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _NetSerialHANDLER_H
#define _NetSerialHANDLER_H

#include <SocketHandler.h>

#include <SocketHandler.h>
#include <TcpSocket.h>
#include "NetSerialLog.h"

#ifdef EXAMPLE_SIGNAL_ONDATA
#include "EZObject.h"
#include "EZSignals.h"
typedef TSignal3<Socket *, const char *, size_t> SIG_CNetSerialHandler_DATA;
#endif //EXAMPLE_SIGNAL_ONDATA
class CNetSerialHandler : public SocketHandler
{
public:
    CNetSerialHandler(CNetSerialLog *pLog=NULL);

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void Update();

    Socket *GetSocketByName(std::string strServerName);
    int CloseAndDeleteAll();

    void OnDataCall(Socket *, const char *buf,size_t len);
    bool AddSignal(CEZObject * pObj, SIG_CNetSerialHandler_DATA::SigProc pProc);
    bool DelSignal(CEZObject * pObj, SIG_CNetSerialHandler_DATA::SigProc pProc);
private:

    CEZMutex m_MutexSig;
    SIG_CNetSerialHandler_DATA m_sigData;

private:
};

#endif // _NetSerialHANDLER_H
