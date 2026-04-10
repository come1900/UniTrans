/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNvpRegisterServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNvpRegisterServer.cpp 5884 2017-04-18 06:13:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-18 06:13:47  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigNvpRegisterServer.h"

#include "../UniTransDef.h"
#include "../Function/Pandora/PandoraDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"

template<>
void exchangeTable<ConfigNvpRegisterServer>(CConfigExchange& xchg, CConfigTable& table, ConfigNvpRegisterServer& config, int index, int app)
{
    xchg.exchange(table, "ServerPort"    , config.iPort, 0, 65535 ,def_Pandora_NvpRegisterServer_Port);
    xchg.exchange(table, "ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}

