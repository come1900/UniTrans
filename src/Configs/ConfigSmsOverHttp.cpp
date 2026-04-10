/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigSmsOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigSmsOverHttp.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigSmsOverHttp.h"

template<>
void exchangeTable<ConfigSmsOverHttp>(CConfigExchange& xchg, CConfigTable& table, ConfigSmsOverHttp& config, int index, int app)
{
    xchg.exchange(table, "SmsOverHttpPort"    , config.iSmsOverHttpPort, 0, 65535 ,60088);

    xchg.exchange(table, "ConnTimeout"    , config.ConnTimeout, 0, 65535 ,5);

    // this module ÊÕµ½µÄ
    xchg.exchange(table, "registerURL", config.strURL_registerURL, "/sms/register");

    xchg.exchange(table, "requestAuth",      config.strURIrequestAuth     , "/sms/requestAuth");
    xchg.exchange(table, "requestRegister",  config.strURIrequestRegister , "/sms/requestRegister");
    xchg.exchange(table, "requestHeartbeat", config.strURIrequestHeartbeat, "/sms/requestHeartbeat");
    xchg.exchange(table, "requestCancel",    config.strURIrequestCancel   , "/sms/requestCancel");
    xchg.exchange(table, "requestTransit",   config.strURIrequestTransit  , "/sms/requestTransit");


    xchg.exchange(table, "requestAuthByHost", config.strURL_requestAuthByHost, "/sms/requestAuthByHost");
    xchg.exchange(table, "heartbeat", config.strURL_heartbeat, "/sms/heartbeat");
    xchg.exchange(table, "stopHostNetwork", config.strURL_stopHostNetwork, "/sms/stopHostNetwork");
    xchg.exchange(table, "notifyUpdate", config.strURL_notifyUpdate, "/sms/notifyUpdate");
    xchg.exchange(table, "getDeviceStateList", config.strURL_getDeviceStateList, "/sms/getDeviceStateList");
    xchg.exchange(table, "controlDevice", config.strURL_controlDevice, "/sms/controlDevice");

    xchg.exchange(table, "getPubIP4", config.strURL_getPubIP4, "/sms/getPubIP4");
}
