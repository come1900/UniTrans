/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigMinerMtService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigMinerMtService.h 5884 2021-04-16 03:01:23Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-16 03:01:23  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigMinerMtService_H__
#define __ConfigMinerMtService_H__

#include "ConfigBase.h"

typedef struct tagConfigMinerMtService
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
ConfigMinerMtService;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigMinerMtService, 1, 4> CConfigMinerMtService;

#endif //__ConfigMinerMtService_H__
