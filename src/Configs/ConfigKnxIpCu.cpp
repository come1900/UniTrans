/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigKnxIpCu.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigKnxIpCu.cpp 5884 2014-01-03 05:15:57Z WuJunjie $
 *
 *  Notes:
 *     -
 *      向knxip 网关连接的参数
 *     -
 *
 *  Update:
 *     2014-01-03 05:15:57 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigKnxIpCu.h"

template<> void exchangeTable<ConfigKnxIpCu>(CConfigExchange& xchg, CConfigTable& table, ConfigKnxIpCu& config, int index, int app)
{
    xchg.exchange(table, "strSearchHost"    , config.strSearchHost    , "224.0.23.12");
    xchg.exchange(table, "iSearchPort"    , config.iSearchPort    , 0, 65535 , 3671);

    xchg.exchange(table, "strKnxIpHost"    , config.strKnxIpHost    , "192.168.6.200");
    xchg.exchange(table, "iKnxIpPort"    , config.iKnxIpPort    , 0, 65535 ,3671);

    xchg.exchange(table,"iCheckPeriod", config.iCheckPeriod,  0, 65535 ,60);
}
