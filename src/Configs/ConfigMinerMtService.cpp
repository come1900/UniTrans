/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigMinerMtService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigMinerMtService.cpp 5884 2021-04-16 03:01:29Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-16 03:01:29  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "../UniTransDef.h"
#include "../Function/UniService/DefUniService.h"

#include "ConfigMinerMtService.h"

template<>
void exchangeTable<ConfigMinerMtService>(CConfigExchange& xchg, CConfigTable& table, ConfigMinerMtService& config, int index, int app)
{
    xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_UniService_KeyManagementService_Port);
    xchg.exchange(table, "ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey
    xchg.exchange(table, "Accounts"    , config.iAccounts, 0, 1, 1); //支持账号功能
}
