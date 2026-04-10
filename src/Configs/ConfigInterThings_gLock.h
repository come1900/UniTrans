/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigInterThings_gLock.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigInterThings_gLock.h 5884 2012-09-07 10:46:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigInterThings_gLock_H__
#define __ConfigInterThings_gLock_H__

#include "ConfigBase.h"
//!
typedef struct tagConfigInterThings_gLock
{

    int iEnableServer;// 0-不用， 1 - 使用
    int iInterThings_gLockPort;// 服务器端口

    // 超时时间 
    int ConnTimeout;

	// 检查key
    std::string strURI_chkKey;
	// 使用key
    std::string strURI_useKey;

	// 查询锁状态
    std::string strURI_queStatus;
}
ConfigInterThings_gLock;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigInterThings_gLock, 1, 4> CConfigInterThings_gLock;

#endif //__ConfigInterThings_gLock_H__
