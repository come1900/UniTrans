/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigSerial.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigSerial.h 5884 2013-07-29 10:10:35Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-29 10:10:35 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigSerial_H__
#define __ConfigSerial_H__

#include "ConfigSerial.h"
#include "ConfigBase.h"

// 串口编号从?开始
#define SERIAL_NO_START_AT 1
// 串口数量
#define MAX_SERIAL_INTERFACE_NUM 4
//#define COM_NAME_LENGTH         64

//!
typedef struct tagConfigSerial_t
{
    int bEnable;// true or false

    int iChannel;// 编号1-n
    std::string strSerialDevice;// 名称
    //PKTH100B,往后罗列即可
    std::string strFunction;// 所接设备

    int baud;
    int data_bits; //7- 8-
    int parity; // checkparity 0-noparity 1-odd(奇) 2-even(偶)
    int stop_bits; // 1, 15, 2
}
CONFIG_SerialShare_T;

typedef TConfig<CONFIG_SerialShare_T, MAX_SERIAL_INTERFACE_NUM/*结构数目, 即最多?个口*/, 4/*观察者最大数目*/> CConfigSerial;

#endif //__ConfigSerial_H__
