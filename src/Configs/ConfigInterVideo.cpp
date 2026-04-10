/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigInterVideo.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigInterVideo.cpp 5884 2017-04-18 06:13:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-18 06:13:47  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "../UniTransDef.h"
#include "../Function/UniService/DefUniService.h"

#include "ConfigInterVideo.h"

template<>
void exchangeTable<ConfigInterVideo>(CConfigExchange& xchg, CConfigTable& table, ConfigInterVideo& config, int index, int app)
{
    xchg.exchange(table, "ServerPort"    , config.iPort, HTTPD_MIN_PORT, 65535 ,def_UniService_InterVideo_Port);
    xchg.exchange(table, "ConnTimeout"    , config.iTimeout, 0, 65535 , HTTPD_CONN_TIMEOUT);
    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}

