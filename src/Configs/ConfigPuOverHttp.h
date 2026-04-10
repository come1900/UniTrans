/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPuOverHttp.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPuOverHttp.h 5884 2013-06-07 10:19:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigPuOverHttp_H__
#define __ConfigPuOverHttp_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigPuOverHttp
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
ConfigPuOverHttp;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigPuOverHttp, 1, 4> CConfigPuOverHttp;

#endif //__ConfigPuOverHttp_H__
