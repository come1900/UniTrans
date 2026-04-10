/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveStatusServer.cpp - _explain_
 *
 * Copyright (C) 2019 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveStatusServer.cpp 5884 2019-08-13 10:16:57Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-08-13 10:16:57  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigLiveStatusServer.h"

#include "../UniTransDef.h"
#include "../Function/UniService/DefUniService.h"

#define def_Default_strNgxAccessLog "/home/pss/logs/pssix_access.log"

template<>
void exchangeTable<ConfigLiveStatusServer>(CConfigExchange& xchg, CConfigTable& table, ConfigLiveStatusServer& config, int index, int app)
{
    xchg.exchange(table, "01_ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_UniService_LiveStatus_Port);
    xchg.exchange(table, "02_ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "03_Auth"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

    xchg.exchange(table, "04_NgxAccessLog", config.strNgxAccessLog, def_Default_strNgxAccessLog/*serialDev*/);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}

