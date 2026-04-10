/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigKnxIpGw.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigKnxIpGw.cpp 5884 2014-01-03 05:15:57Z WuJunjie $
 *
 *  Notes:
 *     -
 *      knxip 网关的参数 -- 宏伟目标
 *     -
 *
 *  Update:
 *     2014-01-03 05:15:57 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigKnxIpGw.h"

template<> void exchangeTable<ConfigKnxIpGw>(CConfigExchange& xchg, CConfigTable& table, ConfigKnxIpGw& config, int index, int app)
{
    xchg.exchange(table, "strSearchHost"    , config.strSearchHost    , "224.0.23.12");
    xchg.exchange(table, "iSearchPort"    , config.iSearchPort    , 0, 65535 , 3671);

    xchg.exchange(table, "iKnxIpPort"    , config.iKnxIpPortRecv    , 0, 65535 ,3671);

    xchg.exchange(table,"iCheckPeriod", config.iCheckPeriod,  0, 65535 ,60);
}
