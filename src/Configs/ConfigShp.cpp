/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigShp.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigShp.cpp 5884 2013-06-18 05:03:33Z WuJunjie $
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

#include "ConfigShp.h"

template<>
void exchangeTable<CONFIG_ShpShare_T>(CConfigExchange& xchg, CConfigTable& table, CONFIG_ShpShare_T& config, int index, int app)
{
    //默认改为0 配置后生效
    xchg.exchange(table, "0_Enable"    , config.bEnable    , 0, 1, 0);
    xchg.exchange(table, "1_Function"  , config.strFunction, "");

    xchg.exchange(table, "2_LocalIP"   , config.local_ip   , "");
    xchg.exchange(table, "3_LocalPort" , config.local_port , 0, 65535, 0);
    xchg.exchange(table, "4_RemotePort", config.remote_port, 0, 65535, 0);
    xchg.exchange(table, "5_Type"  , config.strType, "");
    xchg.exchange(table, "6_Description"  , config.strDescription, "");

    xchg.exchange(table, "7_ServerAddr"  , config.server_addr, "");
    xchg.exchange(table, "8_ServerPort" , config.server_port , 1, 65535, 1);
    xchg.exchange(table, "9_PrivilegeToken"  , config.privilege_token, "");
}

