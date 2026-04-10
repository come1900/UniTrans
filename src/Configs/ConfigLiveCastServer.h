/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveCastServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveCastServer.h 5884 2017-02-19 07:07:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-19 07:07:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigLiveCastServer_H__
#define __ConfigLiveCastServer_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigLiveCastServer
{
    int iPort;// 服务器端口

    // 超时时间 
    /** Enable timeout control. 0=disable timeout check. */
    int iTimeout;

    //按位控制
    // bit0 appKey
    // bit1 ip
    int iAuth;

    // 1-rand
    // 0-static
    int iRandChannalID;// 随机可变地址
}
ConfigLiveCastServer;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigLiveCastServer, 1, 4> CConfigLiveCastServer;

#endif //__ConfigLiveCastServer_H__
