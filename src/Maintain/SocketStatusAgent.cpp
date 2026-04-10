/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SocketStatusAgent.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: SocketStatusAgent.cpp 5884 2012-07-19 04:09:10Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-19 04:09:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <Utility.h>
#include "SocketStatusAgent.h"

CSocketStatusAgent::CSocketStatusAgent(ISocketHandler& h)
:TcpSocket(h)
{
}

void CSocketStatusAgent::OnAccept()
{
	Send("Local hostname : " + Utility::GetLocalHostname() + "\n");
	Send("Local address : " + Utility::GetLocalAddress() + "\n");
	Send("Number of sockets in list : " + Utility::l2string(Handler().GetCount()) + "\n");
	Send("\n");
}
