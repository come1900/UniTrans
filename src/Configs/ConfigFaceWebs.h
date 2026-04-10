/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigFaceWebs.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigFaceWebs.h 5884 2013-06-05 05:11:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-05 05:11:14  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigFaceWebs_H__
#define __ConfigFaceWebs_H__

#include "ConfigBase.h"
//!
typedef struct tagConfigFaceWebs
{
    int SPort;// 端口
    int iRetries;// 端口试探上限

    // 终端状态维护/ 检查周期
    // 0- 不检查
    // >0 秒
    int CheckPeriod;
    // 超时时间
    int PeerTimeout;

    // 默认页面
    std::string strDefaultPages;

    // 根目录
    std::string strRootPath;
}
ConfigFaceWebs;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigFaceWebs, 1, 4> CConfigFaceWebs;

#endif //__ConfigFaceWebs_H__
