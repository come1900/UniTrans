/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTcpSocketConnector.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTcpSocketConnector.cpp 5884 2013-06-07 10:19:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/



#include "ConfigTcpSocketConnector.h"

template<> void exchangeTable<ConfigTcpSocketConnector>(CConfigExchange& xchg, CConfigTable& table, ConfigTcpSocketConnector& config, int index, int app)
{
    xchg.exchange(table, "Host"    , config.strSMSHost    , "sms.faceaip.com");
    xchg.exchange(table, "Port"    , config.iSMSPort    , 0, 65535 ,10000);

    xchg.exchange(table, "Name"    , config.strSMSName    , "sms");

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,1);
}
