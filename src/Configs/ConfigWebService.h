/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigWebService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigWebService.h 5884 2012-09-07 10:46:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __CONFIGWEBSERVICE_H__
#define __CONFIGWEBSERVICE_H__

#include "ConfigBase.h"
//!
typedef struct tagConfigWebService
{
    int iWebServicePort;// 服务器端口

    // URL of webservice
    std::string strURL_requestAuth							;
    std::string strURL_requestAuthByHost							;
    std::string strURL_heartbeat   ;
    std::string strURL_registerURL               ;
    std::string strURL_stopHostNetwork               ;
    std::string strURL_notifyUpdate                    ;
    std::string strURL_getDeviceStateList               ;
    std::string strURL_controlDevice                    ;

}
ConfigWebService;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigWebService, 1, 4> CConfigWebService;

#endif //__CONFIGSMARTHOMETELCOM_H__
