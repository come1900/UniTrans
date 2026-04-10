/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigKeyManagementService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigKeyManagementService.h 5884 2019-03-19 05:13:19Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-03-19 05:13:19  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigKeyManagementService_H__
#define __ConfigKeyManagementService_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigKeyManagementService
{
    int iPort;// 服务器端口

    // 超时时间 
    /** Enable timeout control. 0=disable timeout check. */
    int iTimeout;

    //按位控制
    // bit0 appKey
    // bit1 ip
    int iAuth;
}
ConfigKeyManagementService;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigKeyManagementService, 1, 4> CConfigKeyManagementService;

#endif //__ConfigKeyManagementService_H__
