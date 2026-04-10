/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigKnxIpCu.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigKnxIpCu.h 5884 2014-01-03 05:14:30Z WuJunjie $
 *
 *  Notes:
 *     -
 *      向knxip 网关连接的参数
 *     -
 *
 *  Update:
 *     2014-01-03 05:14:30 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigKnxIpCu_H__
#define __ConfigKnxIpCu_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigKnxIpCu
{
    std::string strSearchHost;
    int iSearchPort;// 端口

// knxip网关的参数
    std::string strKnxIpHost;
    int iKnxIpPort;// 端口
 
    // 心跳 / 检查周期(sec)
    // 0- 不发送， 不可能
    // >0 秒
    int iCheckPeriod;
}
ConfigKnxIpCu;

// 先配置一个
//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigKnxIpCu, 1, 4> CConfigKnxIpCu;

#endif //__ConfigKnxIpCu_H__
