/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigWebService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigWebService.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigWebService.h"

template<>
void exchangeTable<ConfigWebService>(CConfigExchange& xchg, CConfigTable& table, ConfigWebService& config, int index, int app)
{
    xchg.exchange(table, "WebServicePort"    , config.iWebServicePort, 0, 65535 ,60090);

    // this module ÊÕµ½µÄ
    xchg.exchange(table, "requestAuth", config.strURL_requestAuth, "/WebService/requestAuth");
    xchg.exchange(table, "requestAuthByHost", config.strURL_requestAuthByHost, "/WebService/requestAuthByHost");
    xchg.exchange(table, "heartbeat", config.strURL_heartbeat, "/WebService/heartbeat");
    xchg.exchange(table, "registerURL", config.strURL_registerURL, "/WebService/registerURL");
    xchg.exchange(table, "stopHostNetwork", config.strURL_stopHostNetwork, "/WebService/stopHostNetwork");
    xchg.exchange(table, "notifyUpdate", config.strURL_notifyUpdate, "/WebService/notifyUpdate");
    xchg.exchange(table, "getDeviceStateList", config.strURL_getDeviceStateList, "/WebService/getDeviceStateList");
    xchg.exchange(table, "controlDevice", config.strURL_controlDevice, "/WebService/controlDevice");
}
