/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigSerial.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigSerial.cpp 5884 2013-06-18 05:03:33Z WuJunjie $
 *
 *  Notes:
 *     -
 *      串口配置
 *     -
 *
 *  Update:
 *     2013-06-18 05:03:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigSerial.h"

#define def_Default_Serial_Device "/dev/ttyUSB0"

template<>
void exchangeTable<CONFIG_SerialShare_T>(CConfigExchange& xchg, CConfigTable& table, CONFIG_SerialShare_T& config, int index, int app)
{
    //默认改为0 配置后生效
    xchg.exchange(table,"0_Enable",config.bEnable,0,1,0);

    xchg.exchange(table, "1_Channel",config.iChannel, SERIAL_NO_START_AT, 2550, index+SERIAL_NO_START_AT);
    //char serialDev[COM_NAME_LENGTH+1] = {0};
    //sprintf(serialDev, "/dev/ttyUSB%d", index);
    xchg.exchange(table, "2_Device", config.strSerialDevice, def_Default_Serial_Device/*serialDev*/);
    xchg.exchange(table, "3_Function", config.strFunction, "");
    xchg.exchange(table, "4_Baud", config.baud, 75, 115200, 9600);
    xchg.exchange(table, "5_DataBits", config.data_bits, 7, 8, 8);
    xchg.exchange(table, "6_Parity", config.parity, 0, 2, 0);
    xchg.exchange(table, "7_StopBits", config.stop_bits, 1, 15, 1);

    //    if (index == 0)
    //    {
    //    }
    //    else if (index == 1)
    //    {
    //    }
    //    else if (index == 2)
    //    {
    //    }
    //    else
    //    {
    //    }

}

