/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SocketStatusAgent.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: SocketStatusAgent.h 5884 2012-07-19 04:09:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-19 04:09:02  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _SOCKETSTATUSAGENT_H
#define _SOCKETSTATUSAGENT_H

#include <TcpSocket.h>
#include <ISocketHandler.h>

class CSocketStatusAgent : public TcpSocket
{
public:
	CSocketStatusAgent(ISocketHandler& );

	void OnAccept();
};

#endif // _SOCKETSTATUSAGENT_H
