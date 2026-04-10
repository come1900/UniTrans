/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNvpRegisterServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNvpRegisterServer.h 5884 2017-04-18 06:13:55Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-18 06:13:55  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigNvpRegisterServer_H__
#define __ConfigNvpRegisterServer_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigNvpRegisterServer
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
ConfigNvpRegisterServer;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigNvpRegisterServer, 1, 4> CConfigNvpRegisterServer;

#endif //__ConfigNvpRegisterServer_H__
