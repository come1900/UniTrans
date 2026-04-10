/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveStreammingServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveStreammingServer.cpp 5884 2016-05-17 04:30:06Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-05-17 04:30:06  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigLiveStreammingServer.h"

#include "../UniTransDef.h"
#include "../Function/Pandora/PandoraDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */

template<>
void exchangeTable<ConfigLiveStreammingServer>(CConfigExchange& xchg, CConfigTable& table, ConfigLiveStreammingServer& config, int index, int app)
{
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_Pandora_StreammingServer_Port);
    xchg.exchange(table, "ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

#if defined(_PROD_LiveStreammingNewMedia)
    xchg.exchange(table, "Accounts"    , config.iAccounts, 0, 1, 1); //支持账号功能
#else
    xchg.exchange(table, "Accounts"    , config.iAccounts, 0, 1, 0); //不支持账号功能
#endif
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}

