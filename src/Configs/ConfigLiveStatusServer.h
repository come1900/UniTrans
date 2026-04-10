/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveStatusServer.h - _explain_
 *
 * Copyright (C) 2019 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveStatusServer.h 5884 2019-08-13 10:17:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-08-13 10:17:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigLiveStatusServer_H__
#define __ConfigLiveStatusServer_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigLiveStatusServer
{
    int iPort;// 服务器端口

    // 超时时间 
    /** Enable timeout control. 0=disable timeout check. */
    int iTimeout;

    //按位控制
    // bit0 appKey
    // bit1 ip
    int iAuth;

    // ngx访问日志文件全名
    std::string strNgxAccessLog;// 
}
ConfigLiveStatusServer;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigLiveStatusServer, 1, 4> CConfigLiveStatusServer;

#endif //__ConfigLiveStatusServer_H__
