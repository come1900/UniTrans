/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNetSerialTcpCli.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNetSerialTcpCli.cpp 5884 2014-07-19 08:48:17Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-07-19 08:48:17  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigNetSerialTcpCli.h"

template<> void exchangeTable<ConfigNetSerialTcpCli>(CConfigExchange& xchg, CConfigTable& table, ConfigNetSerialTcpCli& config, int index, int app)
{
    xchg.exchange(table, "Enable"    , config.iEnable    , 0, 1 , 1);
    
    xchg.exchange(table, "SName"    , config.strSName    , "NetSerialTcpCli");

    xchg.exchange(table, "Host"    , config.strHost    , "192.168.5.107");
    xchg.exchange(table, "Port"    , config.iPort    , 0, 65535 , 60000); // 50000

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,10);
}
