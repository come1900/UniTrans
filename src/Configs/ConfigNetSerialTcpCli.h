/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigNetSerialTcpCli.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigNetSerialTcpCli.h 5884 2014-07-19 08:48:30Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-07-19 08:48:30  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigNetSerialTcpCli_H__
#define __ConfigNetSerialTcpCli_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigNetSerialTcpCli
{
    int iEnable;// 0-不用， 1 - 使用

    std::string strSName;

    std::string strHost;
    int iPort;// 端口

    // 心跳 / 检查周期(sec)
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
}
ConfigNetSerialTcpCli;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigNetSerialTcpCli, 1, 4> CConfigNetSerialTcpCli;

#endif //__ConfigNetSerialTcpCli_H__
