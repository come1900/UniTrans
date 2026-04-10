/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigConsoleOverTcp.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigConsoleOverTcp.h 5884 2013-06-07 11:28:25Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 11:28:25  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigConsoleOverTcp_H__
#define __ConfigConsoleOverTcp_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigConsoleOverTcp
{
    int SPort;// 端口
 
    // 终端状态维护/ 检查周期
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
    // 超时时间 
    int PeerTimeout;
}
ConfigConsoleOverTcp;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigConsoleOverTcp, 1, 4> CConfigConsoleOverTcp;

#endif //__ConfigConsoleOverTcp_H__
