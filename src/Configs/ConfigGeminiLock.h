/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigGeminiLock.h - brief
 *
 * Copyright (C) 2015 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigGeminiLock.h 5884 2015-04-29 10:10:35Z WuJunjie $
 *
 *  Notes:
 *     -
 *      双子锁配置
 *     -
 *
 *  Update:
 *     2015-04-29 10:10:35 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigGeminiLock_H__
#define __ConfigGeminiLock_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigGeminiLock_t
{
    // 锁类型
    int iLockDevType; // >=0 0-on board， 1-carry 8i8o
    int iLockChannel; // 锁数量，
    int iLockTimeOut; // 控制器维持时长（s）， 0 -- 一直维持,  不自动关闭

    // 控制锁的串口设备名， 如： /dev/ttyUSB0
    //空字符串表示无
    std::string strLockInterface;
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 网络串口
    // Host为空， 或者Port为0 表示不用
    std::string strLockSerialHost;
    int iLockSerialPort;// 端口

    // 读卡器的串口设备名， 如： /dev/ttyUSB0
    //空字符串表示无
    std::string strKeyInterface;
    // 网络串口
    // Host为空， 或者Port为0 表示不用
    std::string strKeySerialHost;
    int iKeySerialPort;// 端口
}
CONFIG_GeminiLock_T;

typedef TConfig<CONFIG_GeminiLock_T, 1/*结构数目, 即最多?个口*/, 4/*观察者最大数目*/> CConfigGeminiLock;

#endif //__ConfigGeminiLock_H__
