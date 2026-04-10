/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigCuOverHttp.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigCuOverHttp.h 5884 2013-06-07 10:19:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigCuOverHttp_H__
#define __ConfigCuOverHttp_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigCuOverHttp
{
    std::string strAccount;
    std::string strPasswd;

    std::string strSmsHost;
    int iSmsPort;// 端口

    std::string strAssHost;
    int iAssPort;// 端口
 
    // 心跳 / 检查周期(sec)
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
}
ConfigCuOverHttp;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigCuOverHttp, 1, 4> CConfigCuOverHttp;

#endif //__ConfigCuOverHttp_H__
