/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigCuOverHttp.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigCuOverHttp.cpp 5884 2013-06-07 10:19:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigCuOverHttp.h"

template<> void exchangeTable<ConfigCuOverHttp>(CConfigExchange& xchg, CConfigTable& table, ConfigCuOverHttp& config, int index, int app)
{
    xchg.exchange(table, "Account"    , config.strAccount    , "Account");
    xchg.exchange(table, "Passwd"    , config.strPasswd    , "Passwd");

    //xchg.exchange(table, "SmsHost"    , config.strSmsHost    , "sms.faceaip.com");
    xchg.exchange(table, "SmsHost"    , config.strSmsHost    , "192.168.10.246");
    xchg.exchange(table, "SmsPort"    , config.iSmsPort    , 0, 65535 , 60088);

    //xchg.exchange(table, "AssHost"    , config.strAssHost    , "sms.faceaip.com");
    xchg.exchange(table, "AssHost"    , config.strAssHost    , "192.168.10.246");
    xchg.exchange(table, "AssPort"    , config.iAssPort    , 0, 65535 ,60600);

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,1);
}
