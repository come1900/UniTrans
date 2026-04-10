/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveCastServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveCastServer.cpp 5884 2017-02-19 07:07:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-19 07:07:22  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigLiveCastServer.h"

#include "../UniTransDef.h"
#include "../Function/Pandora/PandoraDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */

template<>
void exchangeTable<ConfigLiveCastServer>(CConfigExchange& xchg, CConfigTable& table, ConfigLiveCastServer& config, int index, int app)
{
#ifdef _PROJECT_NAME
    // dialog项目默认端口18091
    if (strstr(_PROJECT_NAME, "liveDialog"))
    {
        xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_Pandora_LiveDialogServer_Port);
    }
    else
    {
        xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_Pandora_LiveCastServer_Port);
    }
#else
    xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_Pandora_LiveCastServer_Port);
#endif

    xchg.exchange(table, "ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

    xchg.exchange(table, "RandChannalID"    , config.iRandChannalID, 0, 1, 1); //
}

