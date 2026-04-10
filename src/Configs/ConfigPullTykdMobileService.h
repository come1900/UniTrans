/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPullTykdMobileService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPullTykdMobileService.h 5884 2016-11-02 09:21:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-02 09:21:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigPullTykdMobileService_H__
#define __ConfigPullTykdMobileService_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigPullTykdMobileService
{
#if defined(_SUPPORT_CFG_PSSIXMAN)
    std::string strPushmanFullName;//Pushman全路径
#endif//_SUPPORT_CFG_PSSIXMAN

    // 逐步废弃
#if defined(_SUPPORT_CFG_TYKDWSURL)
    std::string strTykdWsUrl;//天翼看店 webservice 地址
#endif//_SUPPORT_CFG_TYKDWSURL
    std::string strEndPoint4VideoRs;//视频源接口地址

#if defined(_SUPPORT_CFG_StatusUploadWS)
    //地址长度为0不用
    std::string strStatusUploadWS; //状态上报接口地址 视频上云用
    //0-不用 > 0 用
    int iStatusUploaderiod; //上报周期
#endif//_SUPPORT_CFG_StatusUploadWS

    // issue addr
    // 短于10则表示不返回此url
    std::string strDefaultUrlPrefixRTMP;//分发url
    std::string strDefaultUrlPrefixHLS;// 
    std::string strDefaultUrlPrefixHLSs;// 
    std::string strDefaultUrlPrefixFlv;//
    std::string strDefaultUrlPrefixDash;// 
#if defined(_FUNC_LiveStreammingTykd2Rtsp) || defined(_FUNC_LiveStreammingStream2Rtsp)
    std::string strDefaultUrlPrefixRTSP;//
#endif//_FUNC_LiveStreammingTykd2Rtsp

    // push addr
    std::string strDefaultUrlPrefixRTMPNat;//推流地址
#if defined(_FUNC_LiveStreammingTykd2Rtsp) || defined(_FUNC_LiveStreammingStream2Rtsp)
    std::string strDefaultUrlPrefixRTSPNat;//
#endif//_FUNC_LiveStreammingTykd2Rtsp

    // push addr of internet
    // 视频通话用
    //std::string strPubPushRTMP;//推流地址
    //std::string strPubPushRTSP;//
    //std::string strDefaultUrlPrefixHLSNat;//

    // HostRelative of relay host
    // 替换RelayPlayUrl(turn)的ip
    // 当turn和本服务 部署于同一台服务器时，替换后优化组网
    // 默认为空时不替换
    // 配置格式 1 : 219.150.56.141 所有relay ip替换为此ip
    // 配置格式 2 : 219.150.56.141||219.150.56.140&&219.150.56.141||219.150.56.140 , 对应替换
    // 天津示例: 
    // 219.150.56.138||10.10.101.6&&219.150.56.139||10.10.101.7&&219.150.56.140||10.10.101.8&&219.150.56.141||10.10.101.9
    std::string strHostRelative;//Turn ip地址替换

    // 播放id动态生成
    //1-动态ID
    int iDynamicIssueID;

    // 自动停止的时间-- 没有请求后? 秒停止
    // 0 - ValidPeriod_SECOND_MIN 10 的数值不用， 目前表示不用此功能
    // 配置为1-10的数值，同时传入的为自动 表示不自动停止
    // 有效秒数 60-3600*24*365(one year)
    int iAutoStopAfterSec;
    // 定义同上， 当大于 ValidPeriod_SECOND_MIN 时 在配置的数据之后，没有播放的才会被停止
    int iAutoStopAfterSecRtmp;

    //最大拉流数量
    // 0 - 不限
    int iMaxStream;

    // 取播放地址方法
    // 定义: EM_GetPlayUrlmethod_T
    int iGetPlayUrlmethod;
    // 分享url修正
    //std::string strShareFix;
    // hls的起始广告
    // 目前是随便配置启用
    std::string strHlsPreAd;

    /*0-copy, 1-libx264*/
    int iDefaultVideoCodec;
    /*0-copy, 1-aac*/
    int iDefaultAudioCodec;
    // 自动启动拉流， 上次的拉流流媒体开启后自动开启
    // 当成nvr的时候使用
    // 0 - 不启动
    // 1 - 启动
    int iStartPullAuto;
    // 外部推流模式
    // 0 - 非，拉流模式
    // 1 - 是，此处不拉流
    int iPushedByTheFront;

    // 使用播放地址: 
    int iUsePlayUrl_Local;
    int iUsePlayUrl_Direct;
    int iUsePlayUrl_Relay;
}
ConfigPullTykdMobileService;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigPullTykdMobileService, 1, 4> CConfigPullTykdMobileService;

#endif //__ConfigPullTykdMobileService_H__
