/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHttpServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHttpServer.h 5884 2012-09-06 01:31:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 01:31:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALAGENTHTTPSERVER_H
#define _GENERALAGENTHTTPSERVER_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>

class CGeneralAgentHttpServer : public HttpdSocket
{
public:
    CGeneralAgentHttpServer(ISocketHandler& h);

    void Init();

    void Exec();

    void CreateHeader();

    void GenerateDocument();

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
private:
    	std::string m_strBody;
};

#endif // _GENERALAGENTHTTPSERVER_H
