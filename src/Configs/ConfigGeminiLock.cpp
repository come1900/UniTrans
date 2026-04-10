/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigGeminiLock.cpp - brief
 *
 * Copyright (C) 2015 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigGeminiLock.cpp 5884 2015-04-29 10:10:35Z WuJunjie $
 *
 *  Notes:
 *     -
 *      À´◊”À¯≈‰÷√
 *     -
 *
 *  Update:
 *     2015-04-29 10:10:35  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigGeminiLock.h"

template<>
void exchangeTable<CONFIG_GeminiLock_T>(CConfigExchange& xchg, CConfigTable& table, CONFIG_GeminiLock_T& config, int index, int app)
{
    xchg.exchange(table, "iLockDevType",config.iLockDevType,0,2550,1);
    xchg.exchange(table, "iLockChannel",config.iLockChannel,0,1024,1);
    xchg.exchange(table, "iLockTimeOut",config.iLockTimeOut,0,65535,2);
    
    xchg.exchange(table, "strLockInterface", config.strLockInterface, "/dev/ttyUSB0");
    xchg.exchange(table, "strLockSerialHost"    , config.strLockSerialHost, "");
    xchg.exchange(table, "iLockSerialPort"    , config.iLockSerialPort    , 0, 65535 ,0);

    xchg.exchange(table, "strKeyInterface", config.strKeyInterface, "");
    xchg.exchange(table, "strKeySerialHost"    , config.strKeySerialHost, "");
    xchg.exchange(table, "iKeySerialPort"    , config.iKeySerialPort    , 0, 65535 ,0);
}
