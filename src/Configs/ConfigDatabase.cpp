/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigDatabase.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigDatabase.cpp 5884 2014-10-20 08:59:01Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-20 08:59:01  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigDatabase.h"

template<> void exchangeTable<ConfigDatabase>(CConfigExchange& xchg, CConfigTable& table, ConfigDatabase& config, int index, int app)
{
    xchg.exchange(table, "Enable"    , config.iEnable    , 0, 1 , 1);

    xchg.exchange(table, "Name"    , config.strName    , "utcs1");
    xchg.exchange(table, "Passwd"    , config.strPasswd    , "ltgsadmin");
    xchg.exchange(table, "DBString"    , config.strDBString    , "192.168.0.115:1521/ORCL");
}

