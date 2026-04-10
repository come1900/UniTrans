/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigKnxIpGw.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigKnxIpGw.h 5884 2014-01-03 05:14:30Z WuJunjie $
 *
 *  Notes:
 *     -
 *      knxip 网关的参数 -- 宏伟目标
 *     -
 *
 *  Update:
 *     2014-01-03 05:14:30 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigKnxIpGw_H__
#define __ConfigKnxIpGw_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigKnxIpGw
{
    std::string strSearchHost;
    int iSearchPort;// 端口

    // knxip服务端口
    int iKnxIpPortRecv;// 接收端口
 
    // 心跳 / 检查周期(sec)
    // 0- 不发送， 不可能
    // >0 秒
    int iCheckPeriod;
}
ConfigKnxIpGw;

// 先配置一个
//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigKnxIpGw, 1, 4> CConfigKnxIpGw;

#endif //__ConfigKnxIpGw_H__
