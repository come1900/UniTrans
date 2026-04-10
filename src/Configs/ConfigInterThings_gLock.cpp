/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigInterThings_gLock.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigInterThings_gLock.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigInterThings_gLock.h"

template<>
void exchangeTable<ConfigInterThings_gLock>(CConfigExchange& xchg, CConfigTable& table, ConfigInterThings_gLock& config, int index, int app)
{
    xchg.exchange(table, "iEnableServer"    , config.iEnableServer, 0, 1 ,1);
    xchg.exchange(table, "InterThings_gLockPort"    , config.iInterThings_gLockPort, 0, 65535 ,60088);

    xchg.exchange(table, "ConnTimeout"    , config.ConnTimeout, 0, 65535 ,5);

    // this module 收到的
    xchg.exchange(table, "chkKey", config.strURI_chkKey, "/gLock/chkKey");

    // this module 收到的
    xchg.exchange(table, "useKey", config.strURI_useKey, "/gLock/useKey");
    // 
    xchg.exchange(table, "queStatus", config.strURI_queStatus, "/gLock/queStatus");
}

