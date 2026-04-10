/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTcpSocketServer.h 5884 2013-06-05 05:11:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-05 05:11:14  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigTcpSocketServer_H__
#define __ConfigTcpSocketServer_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigTcpSocketServer
{
    int SPort;// 端口
 
    // 终端状态维护/ 检查周期
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
    // 超时时间 
    int PeerTimeout;

// 服务名称
    std::string strSName;
}
ConfigTcpSocketServer;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigTcpSocketServer, 1, 4> CConfigTcpSocketServer;

#endif //__ConfigTcpSocketServer_H__
