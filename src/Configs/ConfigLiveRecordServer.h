/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveRecordServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveRecordServer.h 5884 2017-02-19 07:07:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-19 07:07:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigLiveRecordServer_H__
#define __ConfigLiveRecordServer_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigLiveRecordServer
{
    int iPort;// 服务器端口

    // 超时时间 
    /** Enable timeout control. 0=disable timeout check. */
    int iTimeout;

    //按位控制
    // bit0 appKey
    // bit1 ip
    int iAuth;
    int iAccounts;// 用账号的方式验证
}
ConfigLiveRecordServer;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigLiveRecordServer, 1, 4> CConfigLiveRecordServer;

#endif //__ConfigLiveRecordServer_H__
