/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigInterVideo.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigInterVideo.h 5884 2017-04-18 06:13:55Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-18 06:13:55  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigInterVideo_H__
#define __ConfigInterVideo_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigInterVideo
{
    int iPort;// 服务器端口

    // 超时时间 
    /** Enable timeout control. 0=disable timeout check. */
    int iTimeout;

    //按位控制
    // bit0 appKey
    // bit1 ip
    int iAuth;
}
ConfigInterVideo;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigInterVideo, 1, 4> CConfigInterVideo;

#endif //__ConfigInterVideo_H__
