/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPandoraMatrix.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPandoraMatrix.h 5884 2012-09-07 10:46:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigPandoraMatrix_H__
#define __ConfigPandoraMatrix_H__

#include "ConfigBase.h"
#define def_CleanValuesWhenDisconnect 1

//!
typedef enum __em_auth_type_t {
    em_auth_type_appKey =  0,	// 第0位为1表示使用appkey鉴权
    em_auth_type_appKeyStrict,	// 第1位为1表示使用appkey鉴权严格方式-- 校验时间
    em_auth_type_allowedIP,	    // 第2位为1表示使用白名单
}EM_AUTH_TYPE_T;

//!
typedef struct tagConfigPandoraMatrix
{
    int iPandoraMatrixPort;// 服务器端口

    // 超时时间 
    int ConnTimeout;

    //按位控制__em_auth_type_t
    int iAuth;

    //断开时清理旧数据
    int iCleanValuesWhenDisconnect;

    int iPandoraMatrixPortUdp;// 服务器端口
}
ConfigPandoraMatrix;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigPandoraMatrix, 1, 4> CConfigPandoraMatrix;

#endif //__ConfigPandoraMatrix_H__
