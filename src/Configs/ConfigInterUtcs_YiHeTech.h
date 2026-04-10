/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigInterUtcs_YiHeTech.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigInterUtcs_YiHeTech.h 5884 2012-09-07 10:46:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigInterUtcs_YiHeTech_H__
#define __ConfigInterUtcs_YiHeTech_H__

#include "ConfigBase.h"
//!
typedef struct tagConfigInterUtcs_YiHeTech
{
    int iInterUtcs_YiHeTechPort;// 服务器端口
    // 超时时间 
    int ConnTimeout;

    std::string strURIgetSignalControlerPara;
    std::string strURIgetCrossPara;
    std::string strURIgetCrossStatus;
}
ConfigInterUtcs_YiHeTech;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigInterUtcs_YiHeTech, 1, 4> CConfigInterUtcs_YiHeTech;

#endif //__ConfigInterUtcs_YiHeTech_H__
