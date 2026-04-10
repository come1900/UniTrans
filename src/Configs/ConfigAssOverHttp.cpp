/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigAssOverHttp.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigAssOverHttp.cpp 5884 2013-12-20 02:19:19Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-20 02:19:19 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigAssOverHttp.h"

template<>
void exchangeTable<ConfigAssOverHttp>(CConfigExchange& xchg, CConfigTable& table, ConfigAssOverHttp& config, int index, int app)
{
    xchg.exchange(table, "AssPort"    , config.iAssPort, 0, 65535 ,60600);

    // this module ÊÕµ½µÄ
    xchg.exchange(table, "registerURL", config.strURL_registerURL, "/Ass/Register");

    xchg.exchange(table, "requestAuth",      config.strURIrequestAuth     , "/Ass/requestAuth");
    xchg.exchange(table, "requestRegister",  config.strURIrequestRegister , "/Ass/requestRegister");
    xchg.exchange(table, "requestHeartbeat", config.strURIrequestHeartbeat, "/Ass/requestHeartbeat");
    xchg.exchange(table, "requestCancel",    config.strURIrequestCancel   , "/Ass/requestCancel");
    xchg.exchange(table, "requestTransit",   config.strURIrequestTransit  , "/Ass/requestTransit");

    xchg.exchange(table, "requestAuthByHost", config.strURL_requestAuthByHost, "/Ass/requestAuthByHost");
    xchg.exchange(table, "heartbeat", config.strURL_heartbeat, "/Ass/heartbeat");
    xchg.exchange(table, "stopHostNetwork", config.strURL_stopHostNetwork, "/Ass/stopHostNetwork");
    xchg.exchange(table, "notifyUpdate", config.strURL_notifyUpdate, "/Ass/notifyUpdate");
    xchg.exchange(table, "getDeviceStateList", config.strURL_getDeviceStateList, "/Ass/getDeviceStateList");
    xchg.exchange(table, "controlDevice", config.strURL_controlDevice, "/Ass/controlDevice");

    xchg.exchange(table, "getPubIP4", config.strURL_getPubIP4, "/Ass/getPubIP4");
}
