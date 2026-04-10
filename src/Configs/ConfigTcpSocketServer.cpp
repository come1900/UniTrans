/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTcpSocketServer.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTcpSocketServer.cpp 5884 2013-06-05 05:09:12Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-05 05:09:12  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigTcpSocketServer.h"

template<> void exchangeTable<ConfigTcpSocketServer>(CConfigExchange& xchg, CConfigTable& table, ConfigTcpSocketServer& config, int index, int app)
{
    xchg.exchange(table, "SPort"    , config.SPort,       0, 65535 ,60000);

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"PeerTimeout", config.PeerTimeout,  0, 65535 ,300);

    xchg.exchange(table, "Name"    , config.strSName    , "sms");
}
