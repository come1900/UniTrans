/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTcpSocketConnector.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTcpSocketConnector.h 5884 2013-06-07 10:19:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigTcpSocketConnector_H__
#define __ConfigTcpSocketConnector_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigTcpSocketConnector
{
    std::string strSMSHost;
    int iSMSPort;// 端口

    std::string strSMSName;
 
    // 心跳 / 检查周期(sec)
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
}
ConfigTcpSocketConnector;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigTcpSocketConnector, 1, 4> CConfigTcpSocketConnector;

#endif //__ConfigTcpSocketConnector_H__
