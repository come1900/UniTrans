/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveRecordServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveRecordServer.cpp 5884 2017-02-19 07:07:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-19 07:07:22  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigLiveRecordServer.h"

#include "../UniTransDef.h"
#include "../Function/Pandora/PandoraDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */

template<>
void exchangeTable<ConfigLiveRecordServer>(CConfigExchange& xchg, CConfigTable& table, ConfigLiveRecordServer& config, int index, int app)
{
    xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_Pandora_LiveRecordServer_Port);
    xchg.exchange(table, "ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

    xchg.exchange(table, "Accounts"    , config.iAccounts, 0, 1, 0); //不支持账号功能
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}

