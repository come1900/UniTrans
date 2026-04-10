/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigAssOverHttp.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigAssOverHttp.h 5884 2013-12-20 02:19:51Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-20 02:19:51 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigAssOverHttp_H__
#define __ConfigAssOverHttp_H__

#include "ConfigBase.h"
//!
typedef struct tagConfigAssOverHttp
{
    int iAssPort;// 服务器端口

    std::string strURIrequestAuth;
    std::string strURIrequestRegister;
    std::string strURIrequestHeartbeat;
    std::string strURIrequestCancel;
    std::string strURIrequestTransit;

    // URL of webservice
    std::string strURL_registerURL               ;

    // flowing reserved
    std::string strURL_requestAuthByHost							;
    std::string strURL_heartbeat   ;
    std::string strURL_stopHostNetwork               ;
    std::string strURL_notifyUpdate                    ;
    std::string strURL_getDeviceStateList               ;
    std::string strURL_controlDevice                    ;
    std::string strURL_getPubIP4                    ;
}
ConfigAssOverHttp;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigAssOverHttp, 1, 4> CConfigAssOverHttp;

#endif //__ConfigAssOverHttp_H__
