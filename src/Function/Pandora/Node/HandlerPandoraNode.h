/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerPandoraNode.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerPandoraNode.h 5884 2013-01-29 03:21:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HandlerPandoraNode_H
#define _HandlerPandoraNode_H

#include <SocketHandler.h>
#include <TcpSocket.h>
#include <map>

#include "../../../UniTransDef.h"
#include "../../../DBAgent/CacheKeyValue.h"

#include "../../../Configs/ConfigPandoraNode.h"
#include "PandoraNodeKeepConnector.h"

#include "../../../Maintain/GeneralAgentLog.h"

class CHandlerPandoraNode : public SocketHandler
{
public:
    CHandlerPandoraNode(CMaintainAgentLog *pLog=NULL);
    ~CHandlerPandoraNode();

    void tprintf(TcpSocket *,const char *format, ...);
    void List(TcpSocket *);
    void List(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void Start();
    void Update();

    Socket *GetSocketByName(std::string strServerName);

    CPandoraNodeKeepConnector *GetSocketBySessionID(std::string strSessionID);

    CConfigPandoraNode *getCfgPandoraNode();
    unsigned int getTryConnTimes();

private:

    unsigned int m_iTryConnTimes;
    CConfigPandoraNode *m_pCfgPandoraNode;
};

#endif // _HandlerPandoraNode_H
