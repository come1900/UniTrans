/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTykdMobileService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTykdMobileService.h 5884 2016-11-02 09:21:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-02 09:21:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigTykdMobileService_H__
#define __ConfigTykdMobileService_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigTykdMobileService
{
#if defined(_SUPPORT_CFG_PSSIXMAN)
    std::string strPushmanFullName;//Pushman全路径
#endif//_SUPPORT_CFG_PSSIXMAN

#if defined(_SUPPORT_CFG_TYKDWSURL)
    std::string strTykdWsUrl;//天翼看店 webservice 地址
#endif//_SUPPORT_CFG_TYKDWSURL

#if defined(_SUPPORT_CFG_StatusUploadWS)
    std::string strStatusUploadWS; //状态上报接口地址 视频上云用
#endif//_SUPPORT_CFG_StatusUploadWS

    // 取播放地址方法
    // 定义: EM_GetPlayUrlmethod_T
    int iGetPlayUrlmethod;
    // HostRelative of relay host
    // 替换RelayPlayUrl(turn)的ip
    // 当turn和本服务 部署于同一台服务器时，替换后优化组网
    // 默认为空时不替换
    // 配置格式 1 : 219.150.56.141 所有relay ip替换为此ip
    // 配置格式 2 : 219.150.56.141||219.150.56.140&&219.150.56.141||219.150.56.140 , 对应替换
    std::string strHostRelative;//Turn ip地址替换

    std::string strEndPoint4VideoRs;//视频源接口地址

    /*0-copy, 1-libx264*/
    int iDefaultVideoCodec;
    /*0-copy, 1-aac*/
    int iDefaultAudioCodec;
}
ConfigTykdMobileService;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigTykdMobileService, 1, 4> CConfigTykdMobileService;

#endif //__ConfigTykdMobileService_H__
