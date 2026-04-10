/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPandoraNode.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPandoraNode.h 5884 2013-06-07 10:19:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigPandoraNode_H__
#define __ConfigPandoraNode_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigPandoraNode
{
    std::string strNodeName;

    std::string AppKey;
    std::string AppSecret;

    std::string strMatrixHost;
    int iMatrixPort;// 端口

 
    // 心跳 / 检查周期(sec)
    // 0- 不检查
    // >0 秒
    int CheckPeriod;

    // 支持的流通道数量
    int iIpcChn;
    // 重连时间, 0-n, 0 不限 > ?秒数重启等
    int iMaxConnSecs;
}

ConfigPandoraNode;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigPandoraNode, 1, 4> CConfigPandoraNode;

#endif //__ConfigPandoraNode_H__
