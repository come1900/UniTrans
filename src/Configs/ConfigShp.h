/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigShp.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigShp.h 5884 2013-07-29 10:10:35Z WuJunjie $
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

#ifndef __ConfigShp_H__
#define __ConfigShp_H__

#include "ConfigShp.h"
#include "ConfigBase.h"

// 编号从?开始
#define Shp_NO_START_AT 1
// 数量
#define MAX_Shp_INTERFACE_NUM 6

//!
typedef struct tagConfigShp_t
{
    int bEnable;// true or false

    //std::string strShpDevice;// 名称
    //def_Service_Protocol_Shp, def_PqMsg_ValueKey_LiveUrlSmoothRtmp, def_PqMsg_ValueKey_LiveUrlClearRtmp
    std::string strFunction;// 功能

    std::string local_ip;// 本端ip
    int local_port;
    int remote_port; // 远端端口
    std::string strType;// 类型 tcp, udp
    std::string strDescription;// 描述

    // 返回后存放
    std::string server_addr;// 描述
    int server_port;
    std::string privilege_token;// 描述
}
CONFIG_ShpShare_T;

typedef TConfig<CONFIG_ShpShare_T, MAX_Shp_INTERFACE_NUM/*结构数目, 即最多?个口*/, 4/*观察者最大数目*/> CConfigShp;

#endif //__ConfigShp_H__
