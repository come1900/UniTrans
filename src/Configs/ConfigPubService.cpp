/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPubService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPubService.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigPubService.h"

template<>
void exchangeTable<ConfigPubService>(CConfigExchange& xchg, CConfigTable& table, ConfigPubService& config, int index, int app)
{
    xchg.exchange(table, "PubServicePort"    , config.iPubServicePort, 0, 65535 ,60089);

    // this module ÊÕµ½µÄ
    xchg.exchange(table, "requestAuth", config.strURL_requestAuth, "/PubService/requestAuth");
    xchg.exchange(table, "requestAuthByHost", config.strURL_requestAuthByHost, "/PubService/requestAuthByHost");
    xchg.exchange(table, "heartbeat", config.strURL_heartbeat, "/PubService/heartbeat");
    xchg.exchange(table, "registerURL", config.strURL_registerURL, "/PubService/registerURL");
    xchg.exchange(table, "stopHostNetwork", config.strURL_stopHostNetwork, "/PubService/stopHostNetwork");
    xchg.exchange(table, "notifyUpdate", config.strURL_notifyUpdate, "/PubService/notifyUpdate");
    xchg.exchange(table, "getDeviceStateList", config.strURL_getDeviceStateList, "/PubService/getDeviceStateList");
    xchg.exchange(table, "controlDevice", config.strURL_controlDevice, "/PubService/controlDevice");

    xchg.exchange(table, "getPubIP4", config.strURL_getPubIP4, "/PubService/getPubIP4");
}
