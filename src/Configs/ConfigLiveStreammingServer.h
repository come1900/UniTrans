/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveStreammingServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveStreammingServer.h 5884 2016-05-17 04:30:01Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-05-17 04:30:01  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigLiveStreammingServer_H__
#define __ConfigLiveStreammingServer_H__

#include "ConfigBase.h"

//!
typedef enum __em_auth_type_t {
    em_auth_type_appKey =  0,	// 第0位为1表示使用appkey鉴权
    em_auth_type_appKeyStrict,	// 第1位为1表示使用appkey鉴权严格方式-- 校验时间
    em_auth_type_allowedIP,	    // 第2位为1表示使用白名单
}EM_AUTH_TYPE_T;

typedef struct tagConfigLiveStreammingServer
{
    int iPort;// 服务器端口

    // 超时时间 
    /** Enable timeout control. 0=disable timeout check. */
    int iTimeout;

    //按位控制__em_auth_type_t
    int iAuth;
    int iAccounts;// 用账号的方式验证
}
ConfigLiveStreammingServer;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigLiveStreammingServer, 1, 4> CConfigLiveStreammingServer;

#endif //__ConfigLiveStreammingServer_H__
