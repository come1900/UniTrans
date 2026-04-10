/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPullTykdMobileService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPullTykdMobileService.cpp 5884 2016-11-02 09:21:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-02 09:21:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigPullTykdMobileService.h"

#include "../UniTransDef.h"
#include "../Solar.h"
#include "../Function/Pandora/PandoraDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
// 根据客户选择默认值
#if defined(_Vendor_TaizhouVC)
#define _SOAP_SVR_VIDEO_CLOUD_TaiZhou
#else
// 默认情况是tykd
#endif //_Vendor_TaizhouVC
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
#ifndef TYKD_URL_Endpoint
//#define TYKD_URL_Endpoint     "http://61.160.52.40:9000/cxf/MobileService"
#define TYKD_URL_Endpoint     "http://www.189eyes.com:9000/cxf/MobileService"
#endif //TYKD_URL_Endpoint

#define StatusUploadWS_QQH "http://42.101.40.163:17071/cloudVideoMgr/cxf/StorageGatewayServices?wsdl"
#define StatusUploadWS_ALPHA "http://192.168.5.81:18086/storageGateway/cxf/StorageGatewayServices?wsdl"
#ifndef DEFAULT_RTMP_URL
#define DEFAULT_RTMP_URL "rtmp://" "61.160.149.160" ":19090/live"
#endif// DEFAULT_RTMP_URL

#ifndef DEFAULT_HLS_URL_HEADER
#define DEFAULT_HLS_URL_HEADER "http://" "61.160.149.160" ":18080/hls"
#endif// DEFAULT_HLS_URL_HEADER

#ifndef DEFAULT_RTSP_URL
#define DEFAULT_RTSP_URL "rtsp://" "61.160.149.160" ":18554"
#endif// DEFAULT_RTSP_URL

#define LOCALHOST_LO "127.0.0.1"

#define LOCALHOST_RTMP_URL "rtmp://" LOCALHOST_LO ":19090/live"
//#define LOCALHOST_HLS_URL "http://"LOCALHOST_LO":18080/hls"
#define LOCALHOST_RTSP_URL "rtsp://" LOCALHOST_LO ":18554"

#define SERVER_URL_RTMP "61.160.149.160"
#define SERVER_URL_RTSP "61.160.149.160"
#define DEFAULT_PubPushRTMP_URL "rtmp://" SERVER_URL_RTMP ":19090/man"
#define DEFAULT_PubPushRTSP_URL "rtsp://" SERVER_URL_RTSP ":18554"

template<>
void exchangeTable<ConfigPullTykdMobileService>(CConfigExchange& xchg, CConfigTable& table, ConfigPullTykdMobileService& config, int index, int app)
{

#if defined(_SUPPORT_CFG_PSSIXMAN)

#if defined(LINUX)
    std::string strDefaultPushmanPathname = getenv("HOME") + ("/bin/" + std::string(MY_PUSER_MAN_CMD) );
    xchg.exchange(table, "PushmanFullName", config.strPushmanFullName, strDefaultPushmanPathname);
#else //LINUX

    xchg.exchange(table, "PushmanFullName", config.strPushmanFullName, MY_PUSER_MAN);
#endif //LINUX

#endif//_SUPPORT_CFG_PSSIXMAN

#if defined(_SUPPORT_CFG_TYKDWSURL)

    xchg.exchange(table, "TykdWsUrl", config.strTykdWsUrl, TYKD_URL_Endpoint);
#endif//_SUPPORT_CFG_TYKDWSURL

#if defined(_SUPPORT_CFG_StatusUploadWS)
    xchg.exchange(table, "StatusUploadWS", config.strStatusUploadWS, std::string(StatusUploadWS_QQH));
    xchg.exchange(table, "StatusUploadPeriod", config.iStatusUploaderiod,  0, 3600, 10);
#endif//_SUPPORT_CFG_StatusUploadWS

    std::string __defaultUrlPrefixRTMP = DEFAULT_RTMP_URL;
    std::string __defaultUrlPrefixHls = DEFAULT_RTMP_URL;
    std::string __defaultUrlPrefixFlv = DEFAULT_RTMP_URL;
    std::string __defaultUrlPrefixDash = DEFAULT_RTMP_URL;
    std::string __defaultUrlPrefixRTSP = DEFAULT_RTMP_URL;

    if (g_Solar.GetHostIP().length() >= sizeof("0.0.0.0") )
    {
        __defaultUrlPrefixRTMP = "rtmp://" + g_Solar.GetHostIP() + ":19090/live";
        __defaultUrlPrefixHls = "http://" + g_Solar.GetHostIP() +  ":18080/hls";
        //http://61.160.149.159:18080/lflv?port=19090&app=live&stream=0ba1b856c476c14ec9e0dff51b9ebe4c90f8cd25
        __defaultUrlPrefixFlv = "http://" + g_Solar.GetHostIP() +  ":18080/lflv?port=19090&app=live&stream=";
        //http://61.160.149.159:18080/dash/0ba1b856c476c14ec9e0dff51b9ebe4c90f8cd25.mpd
        __defaultUrlPrefixDash = "http://" + g_Solar.GetHostIP() +  ":18080/dash";
        __defaultUrlPrefixRTSP = "rtsp://" + g_Solar.GetHostIP() +  ":18554";
    }

    // Dash 暂时不用
    __defaultUrlPrefixDash = "";

    xchg.exchange(table, "IssueRTMP", config.strDefaultUrlPrefixRTMP, __defaultUrlPrefixRTMP);
    xchg.exchange(table, "IssueHLS", config.strDefaultUrlPrefixHLS, __defaultUrlPrefixHls);
    xchg.exchange(table, "IssueHLSs", config.strDefaultUrlPrefixHLSs, "");//默认不支持

    xchg.exchange(table, "IssueFlv", config.strDefaultUrlPrefixFlv, __defaultUrlPrefixFlv);
    xchg.exchange(table, "IssueDash", config.strDefaultUrlPrefixDash, __defaultUrlPrefixDash);

#if defined(_FUNC_LiveStreammingTykd2Rtsp)  || defined(_FUNC_LiveStreammingStream2Rtsp)

    xchg.exchange(table, "IssueRTSP", config.strDefaultUrlPrefixRTSP, __defaultUrlPrefixRTSP);
#endif//_FUNC_LiveStreammingTykd2Rtsp

    xchg.exchange(table, "LocPushRTMP", config.strDefaultUrlPrefixRTMPNat, LOCALHOST_RTMP_URL);

#if defined(_FUNC_LiveStreammingTykd2Rtsp) || defined(_FUNC_LiveStreammingStream2Rtsp)
    xchg.exchange(table, "LocPushRTSP", config.strDefaultUrlPrefixRTSPNat, LOCALHOST_RTSP_URL);
#endif//_FUNC_LiveStreammingTykd2Rtsp

    //xchg.exchange(table, "PubPushRTMP", config.strPubPushRTMP, DEFAULT_PubPushRTMP_URL);
    //xchg.exchange(table, "PubPushRTSP", config.strPubPushRTSP, DEFAULT_PubPushRTSP_URL);

    // 空为不用
    xchg.exchange(table, "HostRelative", config.strHostRelative, "");

    // EM_GetPlayUrlmethod_T, 1-getPlayUrlHX, 2-getPssServerPlayUrlHXReq, 3-getPssStoragePlayUrlHXReq
    xchg.exchange(table,"GetPlayUrlmethod", config.iGetPlayUrlmethod,  0, 100, 2); // em_GetPlayUrlmethod_default_value
    //xchg.exchange(table, "ShareFix", config.strShareFix, "");

#if defined(_PROD_LiveStreammingNewMedia)
    xchg.exchange(table,"DynamicIssueID", config.iDynamicIssueID,  0, 1, EncryptID_Do_Not_Encrypt);
    xchg.exchange(table,"AutoStopAfterSec", config.iAutoStopAfterSec,  0, 3600*24*365, 3600*24*365);
    // rtmp pusher ...
    xchg.exchange(table,"AutoStopAfterSecRtmp", config.iAutoStopAfterSecRtmp,  0, 3600*24*365, ValidPeriod_DEF_Auto_LiveSecond);// 为了上报状态， 暂不快速停止

    xchg.exchange(table, "z05_StartPullAuto", config.iStartPullAuto,  0, 1, 1);
#else // _PROD_LiveStreammingNewMedia

    xchg.exchange(table,"DynamicIssueID", config.iDynamicIssueID,  0, 1, EncryptID_Encrypt);

    // 小于ValidPeriod_SECOND_MIN(30s) 不自动停止 即禁用此功能
    // rtsp push ...
    xchg.exchange(table,"AutoStopAfterSec", config.iAutoStopAfterSec,  0, 3600*24*365, ValidPeriod_DEF_Auto_LiveSecond);
    // rtmp pusher ...
    // 默认ValidPeriod_SECOND_MIN 使之尽快停止
  #ifdef _FUNC_LiveStatusServer
    //xchg.exchange(table,"AutoStopAfterSecRtmp", config.iAutoStopAfterSecRtmp,  0, 3600*24*365, ValidPeriod_SECOND_MIN);
    xchg.exchange(table,"AutoStopAfterSecRtmp", config.iAutoStopAfterSecRtmp,  0, 3600*24*365, ValidPeriod_SECOND_MIN+100);
  #else
    xchg.exchange(table,"AutoStopAfterSecRtmp", config.iAutoStopAfterSecRtmp,  0, 3600*24*365, ValidPeriod_DEF_Auto_LiveSecond);
  #endif// _FUNC_LiveStatusServer

    xchg.exchange(table, "z05_StartPullAuto", config.iStartPullAuto,  0, 1, 0);
#endif //_PROD_LiveStreammingNewMedia

    // 空不用此功能
    //xchg.exchange(table, "z02_HlsPreAd", config.strHlsPreAd, "/home/pss/html/pssix_ad/1");
    xchg.exchange(table, "z02_HlsPreAd", config.strHlsPreAd, "");// 默认不用此功能

    //最大流数量
    // 0 - 不限
    xchg.exchange(table, "z01_MaxStream", config.iMaxStream,  0, 4096, 0);
    //0-copy, 1-libx264
    xchg.exchange(table, "z03_DefaultVideoCodec", config.iDefaultVideoCodec,  0, 1, 0);
    // 0-audo(copy), 1-aac, 2-noaudio, 3-libfdk_aac
    xchg.exchange(table, "z04_DefaultAudioCodec", config.iDefaultAudioCodec,  0, 3, 0);

    // 播放地址使用选择
#ifdef _SOAP_SVR_SUZHOUSIFA_23
    //司法项目中全部置空不用
    xchg.exchange(table, "z06_UsePlayUrl_Local", config.iUsePlayUrl_Local,  0, 1, 0);
#else

    xchg.exchange(table, "z06_UsePlayUrl_Local", config.iUsePlayUrl_Local,  0, 1, 1);
#endif

    xchg.exchange(table, "z07_UsePlayUrl_Direct", config.iUsePlayUrl_Direct,  0, 1, 0);
    xchg.exchange(table, "z08_UsePlayUrl_Relay", config.iUsePlayUrl_Relay,  0, 1, 1);
    xchg.exchange(table, "z09_EndP4VideoRs", config.strEndPoint4VideoRs, "http://122.227.101.27:60000/gbManager/cxf/rest/client/getPssServerPlayUrlHXReq");
    xchg.exchange(table, "z10_PushedByTheFront", config.iPushedByTheFront,  0, 1, 0);
}

