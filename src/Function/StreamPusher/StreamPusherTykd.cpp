/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StreamPusherTykd.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: StreamPusherTykd.cpp 5884 2016-11-03 09:16:11Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-03 09:16:11  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h> // sleep()
#include <ez_fs.h>
#include <str_opr.h>
#include "sha1.h"
#include <ez_socket.h> /*ezDef_StreamType_Str_megaeye*/
#include <StringOpr.h>
#include <ez_url_parser.h>
#include <ez_util_platform_Exec.h>

#include "../../Logs.h"
#include "../../Solar.h"

#include "../../Configs/ConfigGeneral.h"
#if defined(_FUNC_StreamPusher)
#include "../../Configs/ConfigStreamPusher.h"
#endif // _FUNC_StreamPusher

//#if defined(_FUNC_PullTykdMobileService)
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_PusherTykdServer_SingleServer)
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_TykdMobileService)
#include "../../Configs/ConfigTykdMobileService.h"
#endif
#if defined(_FUNC_LiveRecord)
#include "../../Configs/ConfigLiveRecord.h"
#endif //_FUNC_LiveRecord

#include "../Pandora/PandoraDef.h"

#include "dtPst.h"

//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */
#include "uniFace.h"

#include "StreamPusherTykd.h"
#ifdef _FUNC_LiveStatusServer
#include "../LiveStatusServer/AgentLiveStatusServer.h"
#endif// _FUNC_LiveStatusServer 

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

#define MY_PUSER_MAN_CMD "pssixman"
//#define g_StreamPusherTykd (*CStreamPusherTykd::instance())

#define def_DEFAULT_REC_SNAP_FILE "/default_record_snap.jpg"
#define PULLER_LIST_SPLITER "-------------------------------------------------------------------"

// 开启后多久容许关闭
#define def_MinSecOfRecordStarted 45
#define def_MinSecOfRtmpStreammingStarted 45
#define def_MinSecOfRtspStreammingStarted 45

#if 0
#define def_THE_M3U8_CONTENTS_PreHlsIndex \
"#EXTM3U\n"\
"#EXT-X-VERSION:3\n"\
"#EXT-X-MEDIA-SEQUENCE:14775\n"\
"#EXT-X-TARGETDURATION:8\n"\
"#EXTINF:6.000,\n"\
"http://61.160.149.164:18080/c836/8128CD2002D_0_1-14775.ts\n"\
"#EXTINF:6.000,\n"\
"http://61.160.149.164:18080/c836/8128CD2002D_0_1-14776.ts\n"\
"#EXTINF:7.999,\n"\
"http://61.160.149.164:18080/c836/8128CD2002D_0_1-14777.ts\n"\
"#EXTINF:6.000,\n"\
"http://61.160.149.164:18080/c836/8128CD2002D_0_1-14778.ts\n"\
"#EXTINF:6.000,\n"\
"http://61.160.149.164:18080/c836/8128CD2002D_0_1-14779.ts\n"

#define def_THE_M3U8_CONTENTS_PreHlsIndex \
"#EXTM3U\n"\
"#EXT-X-VERSION:3\n"\
"#EXT-X-MEDIA-SEQUENCE:0\n"\
"#EXT-X-TARGETDURATION:8\n"\
"#EXTINF:6.006400,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-2.ts\n"\
"#EXTINF:7.341156,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-3.ts\n"\
"#EXTINF:4.880200,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-4.ts\n"\
"#EXTINF:5.756133,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-5.ts\n"\
"#EXTINF:7.841689,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-6.ts\n"\
"#EXTINF:5.088756,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-7.ts\n"\
"#EXTINF:6.632067,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-8.ts\n"\
"#EXTINF:5.422444,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-9.ts\n"\
"#EXTINF:6.006400,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-10.ts\n"\
"#EXTINF:6.048111,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-11.ts\n"

#define def_THE_M3U8_CONTENTS_PreHlsIndex \
"#EXTM3U\n"\
"#EXT-X-VERSION:3\n"\
"#EXT-X-MEDIA-SEQUENCE:0\n"\
"#EXT-X-TARGETDURATION:8\n"\
"#EXT-X-PLACEMENT-OPPORTUNITY\n"\
"#EXTINF:7.508000,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-0.ts\n"\
"#EXT-X-PLACEMENT-OPPORTUNITY\n"\
"#EXTINF:4.629933,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-1.ts\n"\
"#EXT-X-PLACEMENT-OPPORTUNITY\n"\
"#EXTINF:6.006400,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-2.ts\n"\
"#EXT-X-PLACEMENT-OPPORTUNITY\n"\
"#EXTINF:7.341156,\n"\
"http://61.160.149.164:18080/pssix_ad/1/pssix_ad_a-3.ts\n"

#endif
#define def_THE_M3U8_CONTENTS_PreHlsIndex \
"#EXTM3U\n"\
"#EXT-X-VERSION:3\n"\
"#EXT-X-TARGETDURATION:6\n"\
"#EXT-X-MEDIA-SEQUENCE:0\n"\
"#EXTINF:5.505867,\n"\
"pssix_ad-0.ts\n"\
"#EXTINF:6.006400,\n"\
"pssix_ad-1.ts\n"\
"#EXTINF:4.629933,\n"\
"pssix_ad-2.ts\n"\
"#EXTINF:5.088756,\n"\
"pssix_ad-3.ts\n"\
"#EXTINF:4.254533,\n"\
"pssix_ad-4.ts\n"\
"#EXTINF:4.546511,\n"\
"pssix_ad-5.ts\n"\


PATTERN_SINGLETON_IMPLEMENT(CStreamPusherTykd);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CStreamPusherTykd::CStreamPusherTykd() :CEZThread("CStreamPusherTykd", THREAD_PRIORITY_DEFAULT)
{
    //ez_printf_debug("CStreamPusherTykd Enter--------\n");

    m_iDynamicIssueID_LiveStream = 0;
    m_iAutoStopAfterSec = ValidPeriod_DEF_Auto_LiveSecond;
    m_iAutoStopAfterSecRtmp = ValidPeriod_SECOND_MIN;
    m_iStartPullAuto = 0;

    m_strHlsPreAd = "";

    m_strEndPoint4VideoRs = "";
#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    m_pCacheWithDb_PullerTykd2Rtsp = NULL;
#endif// _FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveStreammingStream2Hls)
    m_pCacheWithDb_PullerStream2Rtmp = NULL;
#endif// _FUNC_LiveStreammingStream2Hls

    // 录像
    m_iDynamicIssueID_Record = 0;//动态ID

#if defined(_FUNC_LiveRecord)

    m_strIndexPath = "";//索引路径
    m_strMediaPath = "";//录像文件路径

    m_pIdxRecFile = new(CRecFileIdx);
    assert(m_pIdxRecFile);

#if defined(_Func_Use_FileIdxer)
    m_pFileIdxer = new(CFileIdxer);
    assert(m_pFileIdxer);
#endif

    m_iFuncMask = 0;

    m_iSegmentFileTime = 0; // 文件片段时长
    m_iFileValidPeriod = 0;// 文件保存时长
    m_iStartRecordTykdMediaUrlWithCatchDB = 0;

    m_pCacheWithDb_RecordTykdMediaUrl = NULL;
#endif //#if defined(_FUNC_LiveRecord)

    m_iStreamPusherMaint = 10;
    m_iFileHelperPeriod = 0;
#if defined(_USE_MODULES_LocalDataBase)

    m_dbLiveStreamming = NULL;
#endif // _USE_MODULES_LocalDataBase

    m_iPushInterval = 0;
    m_strHostRelative = "";

    m_iDefaultVideoCodec = 0;
    m_iDefaultAudioCodec = 0;

    m_iPushedByTheFront = 0;
#if defined(_Func_CCourier_StreamPusherTykd)
    m_pCourier_StreamPusherTykd = NULL;
#endif // _Func_CCourier_StreamPusherTykd
}

CStreamPusherTykd::~CStreamPusherTykd()
{
    ez_printf_debug("CStreamPusherTykd Leave--------\n");
#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    if (m_pCacheWithDb_PullerTykd2Rtsp)
    {
        delete m_pCacheWithDb_PullerTykd2Rtsp;
    }
#endif// _FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveStreammingStream2Hls)

    if (m_pCacheWithDb_PullerStream2Rtmp)
    {
        delete m_pCacheWithDb_PullerStream2Rtmp;
    }
#endif// _FUNC_LiveStreammingStream2Hls

#if defined(_USE_MODULES_LocalDataBase)

    if (NULL != m_dbLiveStreamming)
    {
        delete m_dbLiveStreamming;
    }
#endif // _USE_MODULES_LocalDataBase

#if defined(_FUNC_LiveRecord)
    if (NULL != m_pIdxRecFile)
    {
        delete m_pIdxRecFile;
    }
#if defined(_Func_Use_FileIdxer)
    if (NULL != m_pFileIdxer)
    {
        delete m_pFileIdxer;
    }
#endif //_Func_Use_FileIdxer

    if (NULL != m_pCacheWithDb_RecordTykdMediaUrl)
    {
        delete m_pCacheWithDb_RecordTykdMediaUrl;
    }
#endif // _FUNC_LiveRecord

#if defined(_Func_CCourier_StreamPusherTykd)
    if (NULL != m_pCourier_StreamPusherTykd)
    {
        delete m_pCourier_StreamPusherTykd;
    }
#endif // _Func_CCourier_StreamPusherTykd
}

EZTHREAD_BOOL CStreamPusherTykd::isReady()
{
    return m_bLoop;
}

EZTHREAD_BOOL CStreamPusherTykd::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    std::string __PssixmanPathname = getenv("HOME") + ("/bin/" + std::string(MY_PUSER_MAN_CMD) );
#if defined(_FUNC_StreamPusher)
        CConfigStreamPusher __cfg_CConfigStreamPusher;
        __cfg_CConfigStreamPusher.update();

        m_iPushInterval = __cfg_CConfigStreamPusher.getConfig().iPushInterval;
#endif // _FUNC_StreamPusher
    //used by LiveStreamming
#if defined(_FUNC_PullTykdMobileService)// || defined(_FUNC_PusherTykdServer_SingleServer)

    {
        CConfigPullTykdMobileService __cfg;
        __cfg.update();

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
        std::string __IssueRTSP = __cfg.getConfig().strDefaultUrlPrefixRTSPNat.length()>5 ? __cfg.getConfig().strDefaultUrlPrefixRTSPNat : __cfg.getConfig().strDefaultUrlPrefixRTSP;
#else
        std::string __IssueRTSP = "rtsp://127.0.0.1:18554";
#endif //_FUNC_LiveStreammingTykd2Rtsp

#if defined(_SUPPORT_CFG_TYKDWSURL)
        std::string __TykdWsUrl = __cfg.getConfig().strTykdWsUrl;
#else
        std::string __TykdWsUrl = "http://202.102.108.45:9000/cxf/MobileService";
#endif //_SUPPORT_CFG_TYKDWSURL
        m_strTykdWsUrl = __TykdWsUrl;
        //ez_printf_debug("start set wsurl:%s\n", m_strTykdWsUrl.c_str());

        std::string m_strEndPoint4VideoRs = __cfg.getConfig().strEndPoint4VideoRs;

#if defined(_SUPPORT_CFG_PSSIXMAN)
        std::string __PssixmanPathname = __cfg.getConfig().strPushmanFullName;
#endif //_SUPPORT_CFG_PSSIXMAN
#if defined(_FUNC_LiveStreammingTykd2Hls) || defined(_FUNC_LiveStreammingTykd2Rtsp)
        //onCfgPssServer(__TykdWsUrl
        //               , MY_PUSER_MAN_CMD
        //               , __PssixmanPathname
        //               , __cfg.getConfig().strDefaultUrlPrefixRTMPNat.length()>5 ? __cfg.getConfig().strDefaultUrlPrefixRTMPNat : __cfg.getConfig().strDefaultUrlPrefixRTMP
        //               , __IssueRTSP
        //              );

        //onCfgEndpointTykdws(__TykdWsUrl);
#endif

#if defined(_FUNC_LiveStreammingTykd2Hls) || defined(_FUNC_LiveStreammingTykd2Rtsp) || defined(_FUNC_LiveStreammingStream2Hls)
        int iret_setParameter_Pss = \
        setParameter_Pss( MY_PUSER_MAN_CMD
                       , __PssixmanPathname
                       , __cfg.getConfig().strDefaultUrlPrefixRTMPNat.length()>5 ? __cfg.getConfig().strDefaultUrlPrefixRTMPNat : __cfg.getConfig().strDefaultUrlPrefixRTMP
                       , __IssueRTSP
                      );

        if (iret_setParameter_Pss != 0)
        {
            ez_printf_error("Failed setParameter_Pss:%d\n", iret_setParameter_Pss);
            return -1;
        }
#endif //defined(_FUNC_LiveStreammingTykd2Hls) || defined(_FUNC_LiveStreammingTykd2Rtsp)
        m_iDynamicIssueID_LiveStream = (__cfg.getConfig().iDynamicIssueID==EncryptID_Encrypt)
                                       ? EncryptID_Encrypt : EncryptID_Do_Not_Encrypt;

        m_iAutoStopAfterSec = __cfg.getConfig().iAutoStopAfterSec;
        m_iAutoStopAfterSecRtmp = __cfg.getConfig().iAutoStopAfterSecRtmp;

        m_iStartPullAuto = __cfg.getConfig().iStartPullAuto;
        m_strHlsPreAd = __cfg.getConfig().strHlsPreAd;

        // 防止多处配置
        if (m_strHostRelative.length() > 0)
        {
            ez_printf_error("m_strHostRelative have been configed.\n");
        }
        m_strHostRelative = __cfg.getConfig().strHostRelative ;

        m_iDefaultVideoCodec = __cfg.getConfig().iDefaultVideoCodec;
        m_iDefaultAudioCodec = __cfg.getConfig().iDefaultAudioCodec;

        m_iPushedByTheFront = __cfg.getConfig().iPushedByTheFront;
    }

#if defined(_USE_MODULES_LocalDataBase)
    m_dbLiveStreamming = new CDbLiveStreamming;
    assert(m_dbLiveStreamming);

    //std::string __dbFN = __cfgGeneral.getConfig().strDataFilePath + std::string("/") + _CONFIG_LocalDataBase;
    //m_dbLiveStreamming->Open(_CONFIG_LocalDataBase);
    m_dbLiveStreamming->Open(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + _CONFIG_LocalDataBase);
#endif // _USE_MODULES_LocalDataBase

    // 推流持久化信息
#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    if (m_pCacheWithDb_PullerTykd2Rtsp)
    {
        delete m_pCacheWithDb_PullerTykd2Rtsp;
        m_pCacheWithDb_PullerTykd2Rtsp = NULL;
    }
    if (NULL == m_pCacheWithDb_PullerTykd2Rtsp)
    {
        m_pCacheWithDb_PullerTykd2Rtsp = new CCacheKeyValue;
        assert(m_pCacheWithDb_PullerTykd2Rtsp);

        //std::string strDbFileName = g_Solar.GetConfigFilePath();
        //strDbFileName+="/";
        //strDbFileName+=def_dbFileKeyValue_PullerStream2Rtmp;

        //m_pCacheWithDb_PullerTykd2Rtsp->setDbfilename(strDbFileName);
        m_pCacheWithDb_PullerTykd2Rtsp->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_dbFileKeyValue_PullerStream2Rtsp);
    }
#endif// _FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveStreammingStream2Hls)

    if (NULL == m_pCacheWithDb_PullerStream2Rtmp)
    {
        m_pCacheWithDb_PullerStream2Rtmp = new CCacheKeyValue;
        assert(m_pCacheWithDb_PullerStream2Rtmp);

        //std::string strDbFileName = g_Solar.GetConfigFilePath();
        //strDbFileName+="/";
        //strDbFileName+=def_dbFileKeyValue_PullerStream2Rtmp;

        //m_pCacheWithDb_PullerStream2Rtmp->setDbfilename(strDbFileName);
        m_pCacheWithDb_PullerStream2Rtmp->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_dbFileKeyValue_PullerStream2Rtmp);
    }
#endif// _FUNC_LiveStreammingStream2Hls

#endif // _FUNC_PullTykdMobileService

    //used by LiveRecord
#if defined(_FUNC_TykdMobileService)

    {
        CConfigTykdMobileService __cfgTykdMobileService;
        __cfgTykdMobileService.update();

#if defined(_SUPPORT_CFG_TYKDWSURL)
        std::string __TykdWsUrl = __cfgTykdMobileService.getConfig().strTykdWsUrl;
#else
        std::string __TykdWsUrl = "http://202.102.108.45:9000/cxf/MobileService";
#endif //_SUPPORT_CFG_TYKDWSURL
        m_strTykdWsUrl = __TykdWsUrl;
        //ez_printf_debug("start set wsurl:%s\n", m_strTykdWsUrl.c_str());

        m_strEndPoint4VideoRs = __cfgTykdMobileService.getConfig().strEndPoint4VideoRs;
        //ez_printf_debug("EndPoint4VideoRs:%s\n", m_strEndPoint4VideoRs.c_str());

#if defined(_SUPPORT_CFG_PSSIXMAN)
        std::string __PssixmanPathname = __cfgTykdMobileService.getConfig().strPushmanFullName;
#endif //_SUPPORT_CFG_PSSIXMAN

#if defined(_FUNC_LiveRecord)
        //onCfgPssServer(__TykdWsUrl
        //               , MY_PUSER_MAN_CMD
        //               , __PssixmanPathname
        //               , ""
        //               , ""
        //             );
#if defined(_FUNC_liveRecordTykd)
        //onCfgEndpointTykdws(__TykdWsUrl);
#endif //#if defined(_FUNC_liveRecordTykd)

        int iret_setParameter_Pss = \
        setParameter_Pss( MY_PUSER_MAN_CMD
                       , __PssixmanPathname
                       , ""
                       , ""
                      );

        if (iret_setParameter_Pss != 0)
        {
            ez_printf_error("Failed setParameter_Pss:%d\n", iret_setParameter_Pss);
            return -1;
        }

#endif //_FUNC_liveRecordTykd
        // 防止多处配置
        if (m_strHostRelative.length() > 0)
        {
            ez_printf_error("m_strHostRelative have been configed.\n");
        }

        if (__cfgTykdMobileService.getConfig().strHostRelative.length() > 1)
        {
            m_strHostRelative = __cfgTykdMobileService.getConfig().strHostRelative;
            //ez_printf_error("m_strHostRelative m_strHostRelative:[%s]\n", m_strHostRelative.c_str());
        }

        m_iDefaultVideoCodec = __cfgTykdMobileService.getConfig().iDefaultVideoCodec;
        m_iDefaultAudioCodec = __cfgTykdMobileService.getConfig().iDefaultAudioCodec;
    }
#endif //_FUNC_TykdMobileService

#if defined(_FUNC_LiveRecord)

    {
        CConfigLiveRecord __cfg;
        __cfg.update();

        m_strIndexPath = __cfg.getConfig().strIndexPath;//索引路径
        m_strMediaPath = __cfg.getConfig().strMediaPath;//录像文件路径
        m_strIsueUrlPrefix = __cfg.getConfig().strDefaultUrlPrefix;
        m_iStreamPusherMaint = __cfg.getConfig().iStreamPusherMaint ;
        m_iFileHelperPeriod = __cfg.getConfig().iFileHelperPeriod ;

        m_iFuncMask = __cfg.getConfig().iFuncMask ;
        //
        m_iSegmentFileTime = __cfg.getConfig().iSegmentFileTime;//单个录像文件时长， s
        m_iFileValidPeriod = __cfg.getConfig().iFileValidPeriod;

        m_iStartRecordTykdMediaUrlWithCatchDB = __cfg.getConfig().iStartWithCacheDB;

//#if defined(_FUNC_liveRecordTykd)
        //onCfgRecordServer("", "", ""
        //                  , __cfg.getConfig().strIndexPath
        //                  , __cfg.getConfig().strMediaPath);
        setParameter_Rec("", ""
                          , __cfg.getConfig().strIndexPath
                          , __cfg.getConfig().strMediaPath);

//#endif //#if defined(_FUNC_liveRecordTykd)

        m_pIdxRecFile->setFilePath(__cfg.getConfig().strMediaPath);
        m_pIdxRecFile->setPrefix(__cfg.getConfig().strDefaultUrlPrefix);
		
        m_pIdxRecFile->setStoragePath(__cfg.getConfig().strStoragePath);
        m_pIdxRecFile->setStorageUrlPrefix(__cfg.getConfig().strStorageUrlPrefix);

        m_pIdxRecFile->setCheapStoragePath(__cfg.getConfig().strCheapStoragePath);
        m_pIdxRecFile->setCheapStorageUrlPrefix(__cfg.getConfig().strCheapStorageUrlPrefix);

        m_iDynamicIssueID_Record = __cfg.getConfig().iDynamicIssueID;

        // 持久化录像数据
        if (NULL == m_pCacheWithDb_RecordTykdMediaUrl)
        {
            m_pCacheWithDb_RecordTykdMediaUrl = new CCacheKeyValue;
            assert(m_pCacheWithDb_RecordTykdMediaUrl);

            m_pCacheWithDb_RecordTykdMediaUrl->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_dbFileKeyValue_RecordMediaUrl);
        }

#if defined(_Func_Use_FileIdxer)

        m_pFileIdxer->AddPath(m_strMediaPath);
        m_pFileIdxer->Start();
#endif//_Func_Use_FileIdxer

    }

    //m_strIndexPath = "";//索引路径
    //m_strMediaPath = "";//录像文件路径
    //m_strIsueUrlPrefix = "http://notdefined.com";
    //m_iIntervalSecFreshRecFile = 15;
#endif //liveRecord
    //__PssixmanPathname = getenv("HOME") + ("/bin/" + std::string(MY_PUSER_MAN_CMD) );

#if defined(_Func_CCourier_StreamPusherTykd)
    m_pCourier_StreamPusherTykd = new CCourier_StreamPusherTykd(&m_vPullerStream2Hls);
    assert(m_pCourier_StreamPusherTykd);
    m_pCourier_StreamPusherTykd->Start();
#endif // _Func_CCourier_StreamPusherTykd

    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CStreamPusherTykd::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}
void CStreamPusherTykd::ThreadProc()
{
    while (m_bLoop)
    {
        /*
        #hls and rtmp
        CFLAGS += -D_FUNC_LiveStreammingTykd2Hls
        #rtsp
        CFLAGS += -D_FUNC_LiveStreammingTykd2Rtsp
        #Stream2Hls
        CFLAGS += -D_FUNC_LiveStreammingStream2Hls
        */
#if defined(_FUNC_LiveRecord)
        // 重启后，开启既有录像任务
        maintainRecordTykdMediaUrl();
#if 0
#if defined(_Func_useRecFileHelper)
        // 分散在每个录像单元中
#else //_Func_useRecFileHelper
        // 录像文件索引
        m_pIdxRecFile->renew(m_iFuncMask);

#if defined(_Func_queryRecord)
        m_pIdxRecFile->fileMaintain(m_iFuncMask);
#else
        m_pIdxRecFile->cutFiles(m_iFuncMask);
#endif//_Func_queryRecord
#endif //_Func_useRecFileHelper
        //ez_printf_debug("numFile:[%d]\n", m_pIdxRecFile->numFile());
        //m_pIdxRecFile->dump(1);
#endif
#endif //#if defined(_FUNC_LiveRecord)

        // 自动停止等功能
#if defined(_FUNC_LiveStreammingTykd2Rtsp)

        maintainPullTykd2Rtsp();
#endif// _FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveStreammingTykd2Hls)

        //maintainPullTykd2Hls();
#endif// _FUNC_LiveStreammingTykd2Hls

#if defined(_FUNC_LiveStreammingStream2Hls)

        maintainPullStream2Hls();
#endif// _FUNC_LiveStreammingStream2Hls

        sleep(m_iStreamPusherMaint);
    }
}

void CStreamPusherTykd::ListPuller(std::string &strDump, int iLevel)
{
    unsigned int ii;
    char sendto[1024];

    ARG_USED(ii);
    ARG_USED(sendto);

    snprintf(sendto, sizeof (sendto)-1, "Puller List, TimePoint(%llu)\n", process_counter_sec());
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "%s\n", PULLER_LIST_SPLITER);
    strDump += sendto;

    snprintf(sendto, sizeof (sendto)-1, "%-3s %-18s %-18s %-5s %-5s %-8s\n"
             , "No"
             , "DevID"
             , "StreamID"
             , "Pid"
             , "Runed"
             , "TimePoint"
            );
    strDump += sendto;

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    snprintf(sendto, sizeof (sendto)-1, "%s Rtsp :%lu \n", PULLER_LIST_SPLITER, m_vPullerTykd2Rtsp.size());
    strDump += sendto;
    for (ii=0; ii<m_vPullerTykd2Rtsp.size(); ii++)
    {
        snprintf(sendto, sizeof (sendto)-1, "%-3d %-5d %-5u %-8lu %-18s %-18s\n"
                 , ii+1
                 , m_vPullerTykd2Rtsp[ii]->getChildPid()
                 , m_vPullerTykd2Rtsp[ii]->getRound()
                 , m_vPullerTykd2Rtsp[ii]->getPushLast()
                 , m_vPullerTykd2Rtsp[ii]->getTykdDevID().c_str()
                 , m_vPullerTykd2Rtsp[ii]->getIdDst().c_str()
                );

        strDump += sendto;
    }
#endif // _FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveRecord)
    snprintf(sendto, sizeof (sendto)-1, "%s Record :%lu \n", PULLER_LIST_SPLITER, m_vRecordTykdMediaUrl.size());
    strDump += sendto;
    for (ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        snprintf(sendto, sizeof (sendto)-1, "%-3d %-5d %-5u %-8lu %-18s %-18s\n"
                 , ii+1
                 , m_vRecordTykdMediaUrl[ii]->getChildPid()
                 , m_vRecordTykdMediaUrl[ii]->getRound()
                 , m_vRecordTykdMediaUrl[ii]->getPushLast()
                 , m_vRecordTykdMediaUrl[ii]->getTykdDevID().c_str()
                 , m_vRecordTykdMediaUrl[ii]->getIdDst().c_str()
                );

        strDump += sendto;
    }
#endif //#if defined(_FUNC_LiveRecord)

}

void CStreamPusherTykd::ListPullerContents(std::string &strDump, int iLevel)
{
    //unsigned int ii;
    char sendto[1024];
    ARG_USED(sendto);

#if defined(_FUNC_LiveStreammingTykd2Rtsp)

    snprintf(sendto, sizeof (sendto)-1, "%s Rtsp :%lu \n", PULLER_LIST_SPLITER, m_vPullerTykd2Rtsp.size());
    strDump += sendto;
#endif //#if defined(_FUNC_LiveStreammingTykd2Rtsp)

#if defined(_FUNC_LiveRecord)

    snprintf(sendto, sizeof (sendto)-1, "%s Record :%lu \n", PULLER_LIST_SPLITER, getNumRecordTykdMediaUrl());
    strDump += sendto;
#endif //#if defined(_FUNC_LiveRecord)

}

void CStreamPusherTykd::ListPullerTables(std::string &strDump, int iLevel)
{
    //unsigned int ii;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "Puller List, TimePoint(%llu)\n", process_counter_sec());
    strDump += sendto;
    snprintf(sendto, sizeof (sendto)-1, "%s\n", PULLER_LIST_SPLITER);
    strDump += sendto;

    snprintf(sendto, sizeof (sendto)-1, "%-3s %-5s %-5s %-8s %-18s %-18s\n"
             , "No"
             , "Pid"
             , "Runed"
             , "Last"
             , "DevID"
             , "StreamID"
            );
    strDump += sendto;
}

void CStreamPusherTykd::dumpPuller(const std::string &strDevID, std::string &strDump, int iLevel)
{
    unsigned int ii;
    char sendto[1024];

    ARG_USED(ii);
    ARG_USED(sendto);
    //snprintf(sendto, sizeof (sendto)-1, "dumpPuller, DevID:%s\n");
    strDump = "dumpPuller, DevID(";
    strDump+=strDevID+"):\n";

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    snprintf(sendto, sizeof (sendto)-1, "%s Rtsp :%lu \n", PULLER_LIST_SPLITER, m_vPullerTykd2Rtsp.size());
    strDump += sendto;
    for (ii=0; ii<m_vPullerTykd2Rtsp.size(); ii++)
    {
        if (m_vPullerTykd2Rtsp[ii]->getTykdDevID() == strDevID)
        {
            strDump += m_vPullerTykd2Rtsp[ii]->getUrlDstFix();
            strDump += "\n";
        }
    }
#endif //#if defined(_FUNC_LiveStreammingTykd2Hls)

#if defined(_FUNC_LiveRecord)
    snprintf(sendto, sizeof (sendto)-1, "%s Rec :%lu \n", PULLER_LIST_SPLITER, m_vRecordTykdMediaUrl.size());
    strDump += sendto;
    for (ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]->getTykdDevID() == strDevID)
        {
            strDump += "FileValidPeriod:" + ezConvertToString(m_vRecordTykdMediaUrl[ii]->getFileValidPeriod());
            strDump += "\n";
        }
    }
#endif //_FUNC_LiveRecord
}

void CStreamPusherTykd::OnPullMsg(int iMsg, void *pPuller)
{
    switch ( iMsg)
    {
#if defined(_FUNC_LiveStreammingStream2Hls)
        case MSG_Pusher_Stream2Hls_OK:
        {
            //ez_printf_info("OnPullMsg MSG_Pusher_Stream2Hls_OK.\n");

            break;
        }
        case MSG_Pusher_Stream2Hls_STOPED:
        {
            ez_printf_info("OnPullMsg MSG_Pusher_Stream2Hls_STOPED.\n");

            CPusherStream2Hls *pOnPuller = (CPusherStream2Hls *)pPuller;
            //CPusherStream2Hls *pOnPuller = dynamic_cast<CPusherStream2Hls *>(pPuller);
            // 不会发生， 但是防止后期维护后出问题
            assert(pOnPuller);

            delPullerStream2Hls(pOnPuller);
            delete pOnPuller;

            break;
        }
#endif//#if defined(_FUNC_LiveStreammingStream2Hls)
#if defined(_FUNC_LiveStreammingTykd2Rtsp)
        case MSG_Pusher_Tykd2Rtsp_OK:
        {
            //ez_printf_info("OnPullMsg MSG_Pusher_Tykd2Rtsp_OK.\n");

            break;
        }
        case MSG_Pusher_Tykd2Rtsp_STOPED:
        {
            ez_printf_info("OnPullMsg MSG_Pusher_Tykd2Rtsp_STOPED.\n");

            CPusherTykd2Rtsp *pOnPuller = (CPusherTykd2Rtsp *)pPuller;

            delPullerTykd2Rtsp(pOnPuller);

            break;
        }
#endif //#if defined(_FUNC_LiveStreammingTykd2Rtsp)
#if defined(_FUNC_LiveRecord)
        case MSG_Pusher_Tykd2Record_OK:
        {
            //ez_printf_info("OnPullMsg MSG_Pusher_Tykd2Rtsp_OK.\n");

            break;
        }
        case MSG_Pusher_Tykd2Record_STOPED:
        {
            ez_printf_info("OnPullMsg MSG_Pusher_Tykd2Record_STOPED.\n");

            CRecordTykdMediaUrl *pOnPuller = (CRecordTykdMediaUrl *)pPuller;

            delRecordTykdMediaUrl(pOnPuller);

            break;
        }
#endif //#if defined(_FUNC_LiveRecord)

        default:
        {
            // Code
            break;
        }
    }
}

int CStreamPusherTykd::getCfg_PushInterval()
{
    return m_iPushInterval;
}

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
void CStreamPusherTykd::ListPullerRtsp(std::string &strDump, unsigned int iFrom, unsigned int iTo, int iLevel)
{
    unsigned int ii;
    char sendto[1024];

    unsigned int __from = iFrom<m_vPullerTykd2Rtsp.size()?iFrom:m_vPullerTykd2Rtsp.size()-1;
    unsigned int __to = iTo<=m_vPullerTykd2Rtsp.size()?iTo:m_vPullerTykd2Rtsp.size();

    snprintf(sendto, sizeof (sendto)-1, "%s Rtsp :%lu \n", PULLER_LIST_SPLITER, m_vPullerTykd2Rtsp.size());
    strDump += sendto;
    for (ii=__from; ii<__to; ii++)
    {
        snprintf(sendto, sizeof (sendto)-1, "%-3d %-5d %-5u %-8lu %-18s %-18s\n"
                 , ii+1
                 , m_vPullerTykd2Rtsp[ii]->getChildPid()
                 , m_vPullerTykd2Rtsp[ii]->getRound()
                 , m_vPullerTykd2Rtsp[ii]->getPushLast()
                 , m_vPullerTykd2Rtsp[ii]->getTykdDevID().c_str()
                 , m_vPullerTykd2Rtsp[ii]->getIdDst().c_str()
                );

        strDump += sendto;
    }
}
int CStreamPusherTykd::addPullerTykd2Rtsp(CPusherTykd2Rtsp *pPuller)
{
    if (NULL == pPuller)
    {
        return -1;
    }
    CEZLock __lock(m_MutexPullerTykd2Rtsp);
    m_vPullerTykd2Rtsp.push_back(pPuller);

    return -1;
}

int CStreamPusherTykd::delPullerTykd2Rtsp(CPusherTykd2Rtsp *pPuller)
{
    if (NULL == pPuller)
    {
        return -1;
    }
    //CEZLock __lock(m_MutexPullerTykd2Rtsp);
    unsigned int ii;
    for (ii=0; ii<m_vPullerTykd2Rtsp.size(); ii++)
    {
        if (m_vPullerTykd2Rtsp[ii] == pPuller)
        {
            delPullerTykd2Rtsp(ii);
            return 0;
        }
    }

    return -1;
}

int CStreamPusherTykd::delPullerTykd2Rtsp(int idx)
{
    CEZLock __lock(m_MutexPullerTykd2Rtsp);

    //delete m_vPullerTykd2Rtsp[idx];
    m_vPullerTykd2Rtsp.erase(m_vPullerTykd2Rtsp.begin() + idx);

    return 0;
}

#define def_CacheWithDb_PullerStream2Rtsp_Key             "_1c7709b58ab0331d5b275c544e813ac9737ed51c" // 此标识为主key// sha14str LiveStreammingStream2Rtsp

int CStreamPusherTykd::startPullRtsp( \
                                      const std::string &strTykdDevID
                                      , const std::string &strChannel
                                      , const std::string &strStream
                                      , const std::string &strValidPeriod
                                      , std::string &strUrlRtsp
                                      , int &iCountReq)
{	
    if (/*strStreamID.length()<2 || */strTykdDevID.length()<2)
    {
        ez_printf_error("strTykdDevID not valid!\n");

        return -1;
    }

    if (EZTHREAD_BOOL_TRUE != isReady())
    {
        ez_printf_error("Not ready!\n");

        return -1;
    }

    for (unsigned int ii=0; ii<m_vPullerTykd2Rtsp.size(); ii++)
    {
        if (m_vPullerTykd2Rtsp[ii]!=NULL &&
            //(strStreamID == m_vPullerTykd2Rtsp[ii]->getIdDst() || strTykdDevID == m_vPullerTykd2Rtsp[ii]->getTykdDevID()))
            (strTykdDevID==m_vPullerTykd2Rtsp[ii]->getTykdDevID() && strChannel==m_vPullerTykd2Rtsp[ii]->getChanel() && strStream==m_vPullerTykd2Rtsp[ii]->getStream())
           )
        {
            //ez_printf_info("Repet for:%s_%s_%s\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());
            ez_printf_info("Repet for:%s\n", m_vPullerTykd2Rtsp[ii]->getDevChnStreamID().c_str());

            strUrlRtsp = m_vPullerTykd2Rtsp[ii]->getUrlDstFix();

            // 原始数据++
            //iCountReq=(++(m_vPullerTykd2Rtsp[ii]->m_iCountReq));
            iCountReq = m_vPullerTykd2Rtsp[ii]->onReq();

#if defined(_USE_MODULES_LocalDataBase)

            m_dbLiveStreamming->startStreamming(\
                                                m_vPullerTykd2Rtsp[ii]->getDevChnStreamID() \
                                                , ""/*strUrlHls*/ \
                                                , ""/*strUrlRtmp*/ \
                                                , strUrlRtsp \
                                                , iCountReq \
                                               );
#endif //_USE_MODULES_LocalDataBase

            return -2;
        }
    }

    CPusherTykd2Rtsp *pPuller = new CPusherTykd2Rtsp;

    if (NULL == pPuller)
    {
        ez_printf_error("Not enough memory.\n");

        return -3;
    }
    else
    {
        // 设置配置参数
        pPuller->setAutoLiveSecond(m_iAutoStopAfterSec);
        pPuller->setValidPeriod(atoi(strValidPeriod.c_str()));
        pPuller->setHostRelative(m_strHostRelative);
        pPuller->setTykdWsUrl(m_strTykdWsUrl);

        pPuller->m_iVideoEncode = m_iDefaultVideoCodec;
        pPuller->m_iAudioEncode = m_iDefaultAudioCodec;
#if 0

        pPuller->setIdDst(strTykdDevID+"_"+strChannel+"_"+strStream);
        pPuller->setTykdDevID(strTykdDevID);
        pPuller->setChanel(strChannel);
        pPuller->setStream(strStream);
#else

        pPuller->setDevChnStreamID(strTykdDevID, strChannel, strStream);
        pPuller->setIdDst(pPuller->getDevChnStreamID()/*strTykdDevID+"_"+strChannel+"_"+strStream*/, m_iDynamicIssueID_LiveStream);
#endif

        std::string strThrName = PREFIX_CPusherTykd2Rtsp_THREAD_NAME + pPuller->getDevChnStreamID()/*strTykdDevID*/;

        pPuller->SetThreadName(strThrName);

        // 原始数据++
        //iCountReq=(++(pPuller->m_iCountReq));
        iCountReq = pPuller->onReq();

        EZTHREAD_BOOL bRet = pPuller->Start(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

        if (EZTHREAD_BOOL_TRUE == bRet)
        {
            addPullerTykd2Rtsp(pPuller);

            //strUrlRtsp = "http://202.102.101.133:18080/hls/"+strTykdDevID+".m3u8";
            strUrlRtsp = pPuller->getUrlDstFix();
#if defined(_USE_MODULES_LocalDataBase)

            m_dbLiveStreamming->startStreamming(\
                                                pPuller->getDevChnStreamID() \
                                                , ""/*strUrlHls*/ \
                                                , ""/*strUrlRtmp*/ \
                                                , strUrlRtsp \
                                                , iCountReq \
                                               );
#endif //_USE_MODULES_LocalDataBase
            // 第一次start的时候有效
            if (NULL != m_pCacheWithDb_PullerTykd2Rtsp)
            {
                m_pCacheWithDb_PullerTykd2Rtsp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtsp_Key, pPuller->getDevChnStreamID(), modPair_If_Not_Exist_Then_Add);

                // 用于反查
                m_pCacheWithDb_PullerTykd2Rtsp->modPair(pPuller->getIdDst()+def_CacheWithDb_PullerStream2Rtsp_Key, pPuller->getDevChnStreamID(), modPair_If_Not_Exist_Then_Add);
            }

        }
        else
        {
            delete pPuller;
            return -4;
        }
    }

    return 0;
}

// -1, 不存在 -2 刚刚开启，暂时不容许stop

int CStreamPusherTykd::stopPullRtsp( \
                                     const std::string &strTykdDevID
                                     , const std::string &strChannel
                                     , const std::string &strStream
                                   )
{
    ez_printf_info("stopPull...  Url(%s).\n", strTykdDevID.c_str());

    for (unsigned int ii=0; ii<m_vPullerTykd2Rtsp.size(); ii++)
    {
        if (m_vPullerTykd2Rtsp[ii]!=NULL &&
            //(strHlsStreamID == m_vPullerTykd2Rtsp[ii]->getIdDst() || strTykdDevID == m_vPullerTykd2Rtsp[ii]->getTykdDevID()))
            (strTykdDevID==m_vPullerTykd2Rtsp[ii]->getTykdDevID() && strChannel==m_vPullerTykd2Rtsp[ii]->getChanel() && strStream==m_vPullerTykd2Rtsp[ii]->getStream())
           )
        {
            // 刚启动的暂时不容许关闭
            if (m_vPullerTykd2Rtsp[ii]->secOfStarted() < def_MinSecOfRtspStreammingStarted)
            {
                ez_printf_error("Have been start in %ld sec, stop not permited:%s\n", m_vPullerTykd2Rtsp[ii]->secOfStarted(), m_vPullerTykd2Rtsp[ii]->getDevChnStreamID().c_str());

                return -2;
            }

#if defined(_USE_MODULES_LocalDataBase)
            m_dbLiveStreamming->stopStreamming(\
                                               m_vPullerTykd2Rtsp[ii]->getDevChnStreamID()
                                              );
#endif //_USE_MODULES_LocalDataBase

            m_vPullerTykd2Rtsp[ii]->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

            delPullerTykd2Rtsp(ii);

            ez_printf_info("done stopPull of ID(%s)_%s_%s.\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

            return 0;
        }
    }

    ez_printf_error("stopPull failed, Not found puller for ID(%s)_%s_%s.\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

    return -1;
}

unsigned int CStreamPusherTykd::getNumPullerRtsp()
{
    return m_vPullerTykd2Rtsp.size();
}

int CStreamPusherTykd::maintainPullTykd2Rtsp()
{
    //ez_printf_info("%s\n", __FUNCTION__);

    for (unsigned int ii=0; ii<m_vPullerTykd2Rtsp.size(); ii++)
    {
        int iLife = m_vPullerTykd2Rtsp[ii]->chkLifeCycle();

        if (Pusher_LifeCycle_T_TimeOut == iLife) //
        {
            ez_printf_info("%s ID(%s) -- Stop .\n", __FUNCTION__, m_vPullerTykd2Rtsp[ii]->getTykdDevID().c_str());

            m_vPullerTykd2Rtsp[ii]->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

            delPullerTykd2Rtsp(ii);
        }
    }//

    return 0;
}
#endif //_FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveRecord)

#define def_CacheWithDb_RecordTykdMediaUrl_Key                   "_b5d547cd6c90dfc8dbd0a72a3efc13db91121a8f" // 此标识为主key// sha14str RecordTykdMediaUrl
#define def_CacheWithDb_RecordTykdMediaUrl_Key_DevID             "_DevID"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_Channel           "_Channel"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_Stream            "_Stream"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_FileSegTime       "_FileSegTime"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_FileValidPeriod   "_FileValidPeriod"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_RecValidPeriod   "_RecValidPeriod"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_StreamType        "_StreamType"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_Url               "_Url"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_Account           "_Account"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_Passwd            "_Passwd"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_VideoEncode       "_VideoEncode"
#define def_CacheWithDb_RecordTykdMediaUrl_Key_AudioEncode       "_AudioEncode"

/*
Type
    #define ezDef_StreamType_Str_Streamming "1"
    #define ezDef_StreamType_Str_IPC_hikvision "2"
    #define ezDef_StreamType_Str_IPC_dahua "3"
    #define ezDef_StreamType_Str_megaeye "4"
    #define ezDef_StreamType_Str_onvif "5"
 
    #define ezDef_StreamType_Streamming 1
    #define ezDef_StreamType_IPC_hikvision 2
    #define ezDef_StreamType_IPC_dahua 3
    #define ezDef_StreamType_megaeye 4
    #define ezDef_StreamType_onvif 5
*/
int CStreamPusherTykd::startRecordTykdMediaUrl( \
        const std::string &strTykdDevID
        , const std::string &strChannel
        , const std::string &strStream
        , const std::string &strFileSegTime
        , const std::string &strFileValidPeriod
        , const std::string &strRecValidPeriod // 录制时长

        , const int iStreamType          // Atatch
        , const std::string &Url           // Atatch
        , const std::string &Account       // Atatch
        , const std::string &Passwd        // Atatch
        , const std::string &VideoEncode   // Atatch
        , const std::string &AudioEncode   //

        , EM_TERMINAL_STATUS_T &iStatus
        , EM_TERMINAL_STATUS_T &iUpStatus
        , unsigned int &UsedSpace
        , unsigned int &UsedUpSpace
                                              )
{
    iStatus = em_terminal_status_unknown;
    iUpStatus = em_terminal_status_unknown;
    UsedSpace = 0;
    UsedUpSpace = 0;		  

    if (strTykdDevID.length()<2
        || strChannel.length()<1
        || strStream.length()<1
        || strFileSegTime.length()<1
        || strFileValidPeriod.length()<1
       )
    {
        ez_printf_error("Invalid para:ID(%s)_%s_%s.\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

        return -1;
    }
    if (EZTHREAD_BOOL_TRUE != isReady())
    {
        ez_printf_error("Not ready!\n");

        return -1;
    }

    // 全球眼参数检查
    if (ezDef_StreamType_megaeye == iStreamType)
    {
        if (Url.length()<5
            || Account.length()<1
            || Passwd.length()<1
           )
        {
            ez_printf_error("Invalid para:Url(%s), Account:%s, Pwd:%s\n", Url.c_str(), Account.c_str(), Passwd.c_str());

            return -1;
        }
    }

    //ez_printf_debug("VideoEncode:%s\n", VideoEncode.c_str());
    //ez_printf_debug("AudioEncode:%s\n", AudioEncode.c_str());

    CPusher *__pPullerTmp = new CPusher;
    assert(__pPullerTmp);
    __pPullerTmp->setDevChnStreamID(strTykdDevID, strChannel, strStream);

    for (unsigned int ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]!=NULL &&
            //(strTykdDevID==m_vRecordTykdMediaUrl[ii]->getTykdDevID() && strChannel==m_vRecordTykdMediaUrl[ii]->getChanel() && strStream==m_vRecordTykdMediaUrl[ii]->getStream())
            (__pPullerTmp->getDevChnStreamID() == /*m_vPullerStream2Hls[ii]*/m_vRecordTykdMediaUrl[ii]->getDevChnStreamID())
           )
        {
            //ez_printf_info("repet pull:ID(%s)_%s_%s\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());
            ez_printf_info("Restart rec:%s\n", __pPullerTmp->getDevChnStreamID().c_str());

            //iCountReq = pPuller->onReq();
            iStatus = m_vRecordTykdMediaUrl[ii]->getDevStatus();
            iUpStatus = iStatus;

            // joy_todo_001
#if 1
            UsedSpace = 11.36*30;// 1Mbps
            UsedUpSpace = 1136*30;
#else
//#if defined(_FUNC_GetUsedSpace)
            std::string __strCmd = "du -ms " + m_strMediaPath + "/" +m_vRecordTykdMediaUrl[ii]->getIdDst();
            std::vector<std::string> resvec;

            ez_printf_debug("ez_Exec:%s\n", __strCmd.c_str());
            int iret = ez_Exec(__strCmd.c_str(), resvec);
            ARG_USED(iret);

            if (resvec.size() > 0)
            {
                UsedSpace = atoi(resvec[0].c_str());
                UsedUpSpace = UsedSpace;

                ez_printf_debug("UsedSpace:%d\n", UsedSpace);
            }
#endif//_FUNC_GetUsedSpace
            return -2;
        }
    }

    CRecordTykdMediaUrl *pRecorder = new CRecordTykdMediaUrl;

    if (NULL == pRecorder)
    {
        ez_printf_error("Not enough memory.\n");

        return -3;
    }
    else
    {
        //pRecorder->setIdDst(strStreamID);
        //pRecorder->setTykdDevID(strTykdDevID);
        //pRecorder->setChanel(strChannel);
        //pRecorder->setStream(strStream);

        pRecorder->setDevChnStreamID(strTykdDevID, strChannel, strStream);
        pRecorder->setIdDst(pRecorder->getDevChnStreamID()/*strTykdDevID+"_"+strChannel+"_"+strStream*/, EncryptID_Do_Not_Encrypt/*, m_iDynamicIssueID_Record*/);
        //pRecorder->setHostRelative(m_strHostRelative);
        pRecorder->m_strEndPoint4VideoRs = m_strEndPoint4VideoRs;
        pRecorder->setTykdWsUrl(m_strTykdWsUrl);

        pRecorder->m_iVideoEncode = 0;
        pRecorder->m_iAudioEncode = 0;

        ezConvertFromString(pRecorder->m_iVideoEncode, VideoEncode);
        ezConvertFromString(pRecorder->m_iAudioEncode, AudioEncode);

        // 协议中传0表示自动， 自动则用配置
        if (0 == pRecorder->m_iVideoEncode)
        {
            ez_printf_debug("use m_iDefaultVideoCodec:%d\n", m_iDefaultVideoCodec);
            pRecorder->m_iVideoEncode = m_iDefaultVideoCodec;
        }

        if (0 == pRecorder->m_iAudioEncode)
        {
            ez_printf_debug("use m_iDefaultAudioCodec:%d\n", m_iDefaultAudioCodec);
            pRecorder->m_iAudioEncode = m_iDefaultAudioCodec;
        }

        pRecorder->m_iFileHelperPeriod = m_iFileHelperPeriod;

        int __iSeg = 0;
        ezConvertFromString(__iSeg, strFileSegTime);
        if (0 == __iSeg)
        {
            __iSeg = m_iSegmentFileTime;
        }
        int __iPeriod = 0;
        ezConvertFromString(__iPeriod, strFileValidPeriod);
        if (0 == __iPeriod)
        {
            __iPeriod = m_iFileValidPeriod;
        }


        int iret = 0;
        iret = pRecorder->setFileSegTime(__iSeg);
        if (iret < 0)
        {
            ez_printf_error("Failed setFileSegTime(%d):%d\n", __iSeg, iret)
        }

        ez_printf_debug("setFileValidPeriod:%d\n", __iPeriod);
        iret = pRecorder->setFileValidPeriod (__iPeriod);
        if (iret < 0)
        {
            ez_printf_error("Failed setFileValidPeriod(%d):%d\n", __iPeriod, iret)
        }

        ez_printf_debug("setRecValidPeriod:%s\n", strRecValidPeriod.c_str())
        pRecorder->setRecValidPeriod(strRecValidPeriod);

        pRecorder->setHostRelative(m_strHostRelative);

        // 全球眼等
        if (iStreamType >= ezDef_StreamType_Streamming)
        {
            pRecorder->m_iType = iStreamType;
            pRecorder->m_strUrl         = Url         ;  // Atatch
            pRecorder->m_strAccount     = Account     ;  // Atatch
            pRecorder->m_strPasswd      = Passwd      ;  // Atatch
            //pRecorder->m_strVideoEncode = VideoEncode ;  // Atatch
            //ezConvertFromString(pPuller->m_iVideoEncode, strVideoEncode);
        }
#if 1
        std::string __strID = pRecorder->getDevChnStreamID();

        if (__strID.length() > def_max_ThreadNameLen)
        {
            __strID = __strID.substr(__strID.length()-def_max_ThreadNameLen);
        }

        std::string strThrName = PREFIX_CRecordTykdMediaUrl_THREAD_NAME + __strID;
#else
        std::string strThrName = PREFIX_CRecordTykdMediaUrl_THREAD_NAME + pRecorder->getDevChnStreamID();
#endif
        pRecorder->SetThreadName(strThrName);

        EZTHREAD_BOOL bRet = pRecorder->Start(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

        if (EZTHREAD_BOOL_TRUE == bRet)
        {
            addRecordTykdMediaUrl(pRecorder);

            // 刚开始， 直接用初始值
            //iCountReq = pPuller->onReq();
            //iStatus = pRecorder->PushStatus();

            // 第一次，记录入参
            if (NULL != m_pCacheWithDb_RecordTykdMediaUrl)
            {
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key                , pRecorder->getDevChnStreamID(), modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_DevID          , strTykdDevID          , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_Channel        , strChannel        , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_Stream         , strStream         , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_FileSegTime    , strFileSegTime    , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_FileValidPeriod, strFileValidPeriod, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_RecValidPeriod, strRecValidPeriod, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_StreamType     , iStreamType       , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_Url            , Url               , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_Account        , Account           , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_Passwd         , Passwd            , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_VideoEncode    , VideoEncode       , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_RecordTykdMediaUrl->modPair(pRecorder->getDevChnStreamID()+def_CacheWithDb_RecordTykdMediaUrl_Key_AudioEncode    , AudioEncode       , modPair_If_Not_Exist_Then_Add);
            }

            // joy_todo_001
#if 1
                UsedSpace = 11.36*30;// 1Mbps
                UsedUpSpace = 1136*30;
#else
            std::string __strCmd = "du -ms " + m_strMediaPath + "/" +pRecorder->getIdDst();
            std::vector<std::string> resvec;

            ez_printf_debug("ez_Exec:%s\n", __strCmd.c_str());
            int iret = ez_Exec(__strCmd.c_str(), resvec);
            ARG_USED(iret);

            if (resvec.size() > 0)
            {
                UsedSpace = atoi(resvec[0].c_str());
                UsedUpSpace = UsedSpace;

                ez_printf_debug("UsedSpace:%d\n", UsedSpace);
            }
#endif
        }
        else
        {
            delete pRecorder;
            return -4;
        }
    }

    return 0;
}

bool CStreamPusherTykd::isStartedRecordTykdMediaUrl( \
        const std::string &strTykdDevID
        , const std::string &strChannel
        , const std::string &strStream
                                                   )
{
    if (strTykdDevID.length()<2
        || strChannel.length()<1
        || strStream.length()<1
       )
    {
        ez_printf_error("Invalid para:ID(%s)_%s_%s.\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());
        return false;
    }

    for (unsigned int ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]!=NULL &&
            //(strStreamID == m_vRecordTykdMediaUrl[ii]->getIdDst() || strTykdDevID == m_vRecordTykdMediaUrl[ii]->getTykdDevID()))
            (strTykdDevID==m_vRecordTykdMediaUrl[ii]->getTykdDevID() && strChannel==m_vRecordTykdMediaUrl[ii]->getChanel() && strStream==m_vRecordTykdMediaUrl[ii]->getStream())
           )
        {
            //ez_printf_error("repet pull:ID(%s_%s_%s).\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

            //strUrlRtsp = m_vRecordTykdMediaUrl[ii]->getUrlDstFix();

            return true;
        }
    }

    return false;
}

// -1, 不存在 -2 刚刚开启，暂时不容许stop
int CStreamPusherTykd::stopRecordTykdMediaUrl( \
        const std::string &strTykdDevID
        , const std::string &strChannel
        , const std::string &strStream
                                             )
{
    ez_printf_info("stopPull...  ID(%s_%s_%s).\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

    for (unsigned int ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]!=NULL &&
            //(strHlsStreamID == m_vRecordTykdMediaUrl[ii]->getIdDst() || strTykdDevID == m_vRecordTykdMediaUrl[ii]->getTykdDevID()))
            (strTykdDevID==m_vRecordTykdMediaUrl[ii]->getTykdDevID() && strChannel==m_vRecordTykdMediaUrl[ii]->getChanel() && strStream==m_vRecordTykdMediaUrl[ii]->getStream())
           )
        {
            // 刚启动的暂时不容许关闭
            if (m_vRecordTykdMediaUrl[ii]->secOfStarted() < def_MinSecOfRecordStarted)
            {
                ez_printf_error("Have been start in %ld sec, stop not permited:%s_%s_%s\n", m_vRecordTykdMediaUrl[ii]->secOfStarted(), strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

                return -2;
            }

            m_vRecordTykdMediaUrl[ii]->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

            delRecordTykdMediaUrl(ii);

            ez_printf_info("Stop rec:%s_%s_%s, it has been started:%ld sec\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str(), m_vRecordTykdMediaUrl[ii]->secOfStarted());

            return 0;
        }
    }

    ez_printf_error("stopPull failed, Not found puller for ID:%s_%s_%s.\n", strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

    return -1; // PssMsg_Value_Result_NoNeedThisOpr
}

int CStreamPusherTykd::getRecorderAttr( \
                                        const std::string &strDevChnStreamID
                                        , int &iFileSegTime
                                        , int &iFileValidPeriod
                                      )
{
    //ez_printf_info("%s...  ID(%s).\n", __FUNCTION__, strDevChnStreamID.c_str());

    for (unsigned int ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]!=NULL &&
            strDevChnStreamID==m_vRecordTykdMediaUrl[ii]->getDevChnStreamID()
           )
        {
            iFileSegTime = m_vRecordTykdMediaUrl[ii]->getFileSegTime();

            iFileValidPeriod = m_vRecordTykdMediaUrl[ii]->getFileValidPeriod();

            return 0;
        }
    }

    //ez_printf_debug("%s failed, Not found puller of ID(%s).\n", __FUNCTION__, strDevChnStreamID.c_str());

    return -1;
}

size_t CStreamPusherTykd::getNumRecordTykdMediaUrl()
{
    return m_vRecordTykdMediaUrl.size();
}

void CStreamPusherTykd::ListRecordTykdMediaUrl(std::string &strDump, unsigned int iFrom, unsigned int iTo, int iLevel)
{
    unsigned int ii;
    char sendto[1024];

    unsigned int __from = iFrom<m_vRecordTykdMediaUrl.size()?iFrom:m_vRecordTykdMediaUrl.size()-1;
    unsigned int __to = iTo<=m_vRecordTykdMediaUrl.size()?iTo:m_vRecordTykdMediaUrl.size();

    snprintf(sendto, sizeof (sendto)-1, "%s RecordTykd :%lu \n", PULLER_LIST_SPLITER, m_vRecordTykdMediaUrl.size());
    strDump += sendto;
    for (ii=__from; ii<__to; ii++)
    {
        snprintf(sendto, sizeof (sendto)-1, "%-3d %-5d %-5u %-8lu %-18s %-18s\n"
                 , ii+1
                 , m_vRecordTykdMediaUrl[ii]->getChildPid()
                 , m_vRecordTykdMediaUrl[ii]->getRound()
                 , m_vRecordTykdMediaUrl[ii]->getPushLast()
                 , m_vRecordTykdMediaUrl[ii]->getTykdDevID().c_str()
                 , m_vRecordTykdMediaUrl[ii]->getIdDst().c_str()
                );

        strDump += sendto;
    }
}
#if 0
//查询文件
int CStreamPusherTykd::queryRecordTykdMediaUrl( \
        const std::string &strTykdDevID
        , const std::string &strChannel
        , const std::string &strStream
        , time_t ttStart
        , time_t ttEnd
        , int iItems
        , int iPages
        , tdef_RecordList &listRecordFile
                                              )
{
    int iret = m_pIdxRecFile->getFile( \
                                       strTykdDevID+"_"+strChannel+"_"+strStream
                                       , ttStart
                                       , ttEnd
                                       , listRecordFile);

    return iret;
}
#endif
//查询文件
int CStreamPusherTykd::queryRecord( \
        const std::string &strTykdDevID
        , const std::string &iChannel
        , const std::string &iStream
        , time_t ttStart
        , time_t ttEnd
        , int iItems
        , int iPages
        , tdefList_CFileStruct &listRecordFile \
                                              )
{
    int iret = m_pIdxRecFile->findFile( \
                                       strTykdDevID
                                       , iChannel
                                       , iStream
                                       , ttStart
                                       , ttEnd
                                       , listRecordFile);

    return iret;
}
int CStreamPusherTykd::makeFileUrl(const CRecFileNode &node, std::string &strUrl, std::string &strSnapUrl)
{
    if (node.strPath.length()<=m_strMediaPath.length())
    {
        ez_printf_error("Filename:%s\n", node.strPath.c_str());

        return -1;
    }

    //strUrl = m_strIsueUrlPrefix + "/"+ node.strPath.substr(m_strMediaPath.length())+ "/"+ node.strName;
    if (node.strNameQtFile.length()>3)
    {
        strUrl = m_strIsueUrlPrefix + node.strPath.substr(m_strMediaPath.length())+ "/"+ node.strNameQtFile;
    }
    else
    {
        strUrl = m_strIsueUrlPrefix + node.strPath.substr(m_strMediaPath.length())+ "/"+ node.strName;
    }
    if (node.strNameSnap.length()>3)
    {
        strSnapUrl = m_strIsueUrlPrefix + node.strPath.substr(m_strMediaPath.length())+ "/"+ node.strNameSnap;
    }
    else
    {
        strSnapUrl = m_strIsueUrlPrefix + def_DEFAULT_REC_SNAP_FILE;
    }
    return 0;
}

int CStreamPusherTykd::maintainRecordTykdMediaUrl()
{
    //ez_printf_debug("%s\n", __FUNCTION__);

    /////
    static int __maintainRecordTykdMediaUrl_first_run = 1;
    if (1 == __maintainRecordTykdMediaUrl_first_run)
    {
        __maintainRecordTykdMediaUrl_first_run=0;

        if (1==m_iStartRecordTykdMediaUrlWithCatchDB // 配置了自动启动
            && NULL != m_pCacheWithDb_RecordTykdMediaUrl)
        {
            ez_printf_info("Use cache db.\n");

            def_uniKeyVector __val;

            // 获取开启的转码id
            m_pCacheWithDb_RecordTykdMediaUrl->getValueLike(__val, def_CacheWithDb_RecordTykdMediaUrl_Key);
            def_uniKeyVector::const_iterator it = __val.begin();
            for ( ; it != __val.end() ; it++)
            {
                DBG(
                    std::cout << "::::::::" << *it << std::endl;
                );

                std::string         strDevID        ;
                std::string         strChannel          ;
                std::string         strStream           ;
                std::string         strFileSegTime      ;
                std::string         strFileValidPeriod  ;
                std::string         strRecValidPeriod  ;
                int                 iStreamType         ;
                std::string         Url                 ;
                std::string         Account             ;
                std::string         Passwd              ;
                std::string         VideoEncode         ;
                std::string         AudioEncode         ;

                EM_TERMINAL_STATUS_T iStatus             ;
                EM_TERMINAL_STATUS_T iUpStatus           ;
                unsigned int        UsedSpace           ;
                unsigned int        UsedUpSpace         ;

                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_DevID          , strDevID           );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_Channel        , strChannel         );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_Stream         , strStream          );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_FileSegTime    , strFileSegTime     );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_FileValidPeriod, strFileValidPeriod );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_RecValidPeriod, strRecValidPeriod );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_StreamType     , iStreamType        );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_Url            , Url                );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_Account        , Account            );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_Passwd         , Passwd             );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_VideoEncode    , VideoEncode        );
                m_pCacheWithDb_RecordTykdMediaUrl->getPair(*it+def_CacheWithDb_RecordTykdMediaUrl_Key_AudioEncode    , AudioEncode        );

                // 合法定义的略过- 它们是一次性录像功能
                if (atoi(strRecValidPeriod.c_str()) >= def_MinRecValidPeriod)
                {
                    ez_printf_debug("Skip:%s,%s\n", strDevID.c_str(), strRecValidPeriod.c_str());

                    continue;
                }

                int iret = startRecordTykdMediaUrl( \
                            strDevID
                           ,strChannel
                           ,strStream
                           ,strFileSegTime
                           ,strFileValidPeriod
                           ,strRecValidPeriod
                           ,iStreamType
                           ,Url
                           ,Account
                           ,Passwd
                           ,VideoEncode
                           ,AudioEncode

                           ,iStatus
                           ,iUpStatus
                           ,UsedSpace
                           ,UsedUpSpace);

                if (0 == iret)
                {
                    // success
                    ez_printf_debug("%s::(%s).\n", def_CacheWithDb_RecordTykdMediaUrl_Key_DevID, strDevID.c_str());
                }
                else
                {
                    ez_printf_error("Failed(%d) startRecordTykdMediaUrl:%s\n", iret, strDevID.c_str());
                }
                //ARG_USED(iret);
            }//
        } // if (NULL != m_pCacheWithDb_PullerStream2Rtmp)
    }
    //录像持续时长功能-- 输入参数
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    for (unsigned int ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]!=NULL && m_vRecordTykdMediaUrl[ii]->getRecValidPeriod()>=def_MinRecValidPeriod/*此时认为需要维护*/
            &&((unsigned long)(m_vRecordTykdMediaUrl[ii]->getRecValidPeriod()) < m_vRecordTykdMediaUrl[ii]->secOfStarted()) /**/
            //&&(time(NULL)-m_vRecordTykdMediaUrl[ii]->m_ttRecStartAt)>(time_t)(m_vRecordTykdMediaUrl[ii]->getRecValidPeriod())
           )
        {
            //(strHlsStreamID == m_vRecordTykdMediaUrl[ii]->getIdDst() || strTykdDevID == m_vRecordTykdMediaUrl[ii]->getTykdDevID()))
            ez_printf_debug("Stop:%s %d of %d\n"
                    , m_vRecordTykdMediaUrl[ii]->getIdDst().c_str()
                    , m_vRecordTykdMediaUrl[ii]->getRecValidPeriod()
                    //, (time(NULL)-m_vRecordTykdMediaUrl[ii]->m_ttRecStartAt)
                    //, m_vRecordTykdMediaUrl[ii]->m_ttRecStartAt
                    , m_vRecordTykdMediaUrl[ii]->secOfStarted());

            m_vRecordTykdMediaUrl[ii]->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

            delRecordTykdMediaUrl(ii);
        }
        else if (0)
        {
            ez_printf_debug("Continue:%s %d of %d\n"
                    , m_vRecordTykdMediaUrl[ii]->getIdDst().c_str()
                    , m_vRecordTykdMediaUrl[ii]->getRecValidPeriod()
                    //, (time(NULL)-m_vRecordTykdMediaUrl[ii]->m_ttRecStartAt)
                    //, m_vRecordTykdMediaUrl[ii]->m_ttRecStartAt
                    , m_vRecordTykdMediaUrl[ii]->secOfStarted());
        }
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    //录像持续时长功能-- 配置参数
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //todo

    return 0;
}


int CStreamPusherTykd::addRecordTykdMediaUrl(CRecordTykdMediaUrl *pPuller)
{
    if (NULL == pPuller)
    {
        return -1;
    }
    CEZLock __lock(m_MutexRecordTykdMediaUrl);
    m_vRecordTykdMediaUrl.push_back(pPuller);

    return -1;
}

int CStreamPusherTykd::delRecordTykdMediaUrl(CRecordTykdMediaUrl *pPuller)
{
    if (NULL == pPuller)
    {
        return -1;
    }

    //CEZLock __lock(m_MutexRecordTykdMediaUrl);
    unsigned int ii;
    for (ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii] == pPuller)
        {
            delRecordTykdMediaUrl(ii);
            return 0;
        }
    }

    return -1;
}

int CStreamPusherTykd::delRecordTykdMediaUrl(int idx)
{
    CEZLock __lock(m_MutexRecordTykdMediaUrl);

    m_vRecordTykdMediaUrl.erase(m_vRecordTykdMediaUrl.begin() + idx);
    return 0;
}
#if 0
// 删除文件
int CStreamPusherTykd::delRecordTykdMediaUrl( \
        const std::string &strTykdDevID
        , const std::string &strChannel
        , const std::string &strStream
        , time_t dtStart
        , time_t dtEnd
                                            )
{
    ez_printf_info("%s...  DevID(%s).\n", __FUNCTION__, strTykdDevID.c_str());

    for (unsigned int ii=0; ii<m_vRecordTykdMediaUrl.size(); ii++)
    {
        if (m_vRecordTykdMediaUrl[ii]!=NULL &&
            (strTykdDevID==m_vRecordTykdMediaUrl[ii]->getTykdDevID() && strChannel==m_vRecordTykdMediaUrl[ii]->getChanel() && strStream==m_vRecordTykdMediaUrl[ii]->getStream())
           )
        {
            // do ...
            //m_vRecordTykdMediaUrl[ii]->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

            ez_printf_info("done %s of ID(%s)_%s_%s.\n", __FUNCTION__, strTykdDevID.c_str(), strChannel.c_str(), strStream.c_str());

            return 0;
        }
    }

    return 0;
}
#endif // if 0
#endif //#if defined(_FUNC_LiveRecord)

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//Begin Stream pull ... 2017-05-19 8:33:54
#if defined(_FUNC_LiveStreammingStream2Hls)
#define def_CacheWithDb_PullerStream2Rtmp_Key             "_6c35a878e5b80a37a553c434e725e5e054801310" // 此标识为主key// sha14str LiveStreammingStream2Hls
#define def_CacheWithDb_PullerStream2Rtmp_Key_ID          "_ID"
#define def_CacheWithDb_PullerStream2Rtmp_Key_Alias          "_Alias" // 名称
#define def_CacheWithDb_PullerStream2Rtmp_Key_Type        "_Type"
#define def_CacheWithDb_PullerStream2Rtmp_Key_ValidPeriod "_ValidPeriod"
#define def_CacheWithDb_PullerStream2Rtmp_Key_Url         "_Url"
#define def_CacheWithDb_PullerStream2Rtmp_Key_Account     "_Account"
#define def_CacheWithDb_PullerStream2Rtmp_Key_Passwd      "_Passwd"
#define def_CacheWithDb_PullerStream2Rtmp_Key_ChnnelIn    "_ChnnelIn"
#define def_CacheWithDb_PullerStream2Rtmp_Key_SubtypeIn   "_SubtypeIn"
#define def_CacheWithDb_PullerStream2Rtmp_Key_AudioEncode "_AudioEncode"
#define def_CacheWithDb_PullerStream2Rtmp_Key_VideoEncode "_VideoEncode"
#define def_CacheWithDb_PullerStream2Rtmp_Key_VideoResize "_VideoResize"
#define def_CacheWithDb_PullerStream2Rtmp_Key_VideoRefps  "_VideoRefps"
#define def_CacheWithDb_PullerStream2Rtmp_Key_UrlHls      "_UrlHls"
#define def_CacheWithDb_PullerStream2Rtmp_Key_UrlHlss      "_UrlHlss"
#define def_CacheWithDb_PullerStream2Rtmp_Key_UrlRtmp     "_UrlRtmp"
#define def_CacheWithDb_PullerStream2Rtmp_Key_UrlFlv      "_UrlFlv"
#define def_CacheWithDb_PullerStream2Rtmp_Key_UrlDash      "_UrlDash"
#define def_CacheWithDb_PullerStream2Rtmp_Key_Status     "_Status"// 0-NotReady, 1-Ready, 999-Unknow

int CStreamPusherTykd::startPullStream2Hls( \
        const std::string &strIDIn
        , const std::string &strAlias
        , const std::string &strType
        , const std::string &strValidPeriod
        , const std::string &strUrl
        , const std::string &strAccount
        , const std::string &strPasswd
        , const std::string &strChnnelIn
        , const std::string &strSubtypeIn
        , const std::string &strAudioEncode
        , const std::string &strVideoEncode
        , const std::string &strVideoResize
        , const std::string &strVideoRefps

        , std::string &strUrlHls
        , std::string &strUrlHlss
        , std::string &strUrlRtmp
        , std::string &strUrlFlv
        , std::string &strUrlDash
        , std::string &strMediaSource
        , std::string &strDstStreamID //流ID
        , EM_TERMINAL_STATUS_T &iStatus
        , int &iCountReq
                                          )
{
    // 参数修正
    std::string strID = strIDIn;
    std::string strChnnel;
    std::string strSubtype;

    // support生成id
    if (ezDef_StreamType_Str_Streamming == strType)
    {
        if (strUrl.length()<5)
        {
            return -1;
        }

        if (strIDIn.length()<10)
        {
            strChnnel  = "1" ;
            strSubtype = "1" ;

            char dst[SHA1HashSizeDestBuffer] = {0};
            int iret = ez_sha1_str(dst, sizeof(dst), strUrl.c_str() );

            if (iret<0)
            {
                return -1;
            }

            strID = dst;
    	}
    }

    //
    if (strID.length()<2
        || strType.length()<1
        /*|| strUrl.length()<2*//*并不是所有情况都需要url*/)
    {
        return -1;
    }

    if (EZTHREAD_BOOL_TRUE != isReady())
    {
        ez_printf_error("Not ready!\n");

        return -1;
    }
    // 防错处理
    // 全球眼等这些字段可以不传
    if (ezDef_StreamType_Str_megaeye == strType
        || ezDef_StreamType_Str_hk8200 == strType)
    {
        strChnnel  = "1" ;
        strSubtype = "1" ;
    }
    // 天翼看店通道号从0开始
#if defined(_FUNC_TykdChnFromZero)
    else if (ezDef_StreamType_Str_tykd== strType
        ||ezDef_StreamType_Str_JSvCloud == strType)
    {
        int __iChannel;
        ezConvertFromString(__iChannel, strChnnelIn);

        __iChannel--;

        strChnnel = ezConvertToString(__iChannel);
        strSubtype = strSubtypeIn ;
    }
#endif //_FUNC_TykdChnFromZero
    else
    {
        strChnnel  = strChnnelIn  ;
        strSubtype = strSubtypeIn ;
    }

    CPusher *__pPullerTmp = new CPusher;
    assert(__pPullerTmp);
    __pPullerTmp->setDevChnStreamID(strID, strChnnel, strSubtype);

    for (def_PusherContainer::iterator it = m_vPullerStream2Hls.begin(); it != m_vPullerStream2Hls.end(); it++)
    {
        CPusher *pPusher = (*it);
        if (NULL==pPusher)
        {
            continue;
        }
    //for (unsigned int ii=0; ii<m_vPullerStream2Hls.size(); ii++)
    //{
        if (//m_vPullerStream2Hls[ii]!=NULL &&
            //(strID == m_vPullerStream2Hls[ii]->getIdDst() ) /**/
            //(strID==m_vPullerStream2Hls[ii]->getTykdDevID()/* && strChnnel==m_vPullerStream2Hls[ii]->getChanel() && strSubtype==m_vPullerStream2Hls[ii]->getStream()*/)
            (__pPullerTmp->getDevChnStreamID() == /*m_vPullerStream2Hls[ii]*/pPusher->getDevChnStreamID())
           )
        {
            ez_printf_info("Re for:%s\n", __pPullerTmp->getDevChnStreamID().c_str());

            //CPusherStream2Hls *__pThePuser = (CPusherStream2Hls *)m_vPullerStream2Hls[ii];
            CPusherStream2Hls *__pThePuser = dynamic_cast<CPusherStream2Hls *>(pPusher);
            // 不会发生， 但是防止后期维护后出问题
            assert(__pThePuser);

            strUrlHls = __pThePuser->getUrlHlsFix();
            strUrlHlss = __pThePuser->getUrlHlssFix();
            strUrlRtmp = __pThePuser->getUrlRtmpFix();
            strUrlFlv = __pThePuser->getUrlFlvFix();
            strUrlDash = __pThePuser->getUrlDashFix();
            strMediaSource = __pThePuser->m_strPlayUrl_Src;
            strDstStreamID = __pThePuser->getIdDst();
            //ez_printf_debug("strMediaSource:%s\n", strMediaSource.c_str());
            // 原始数据++
            //iCountReq=(++(__pThePuser->m_iCountReq));
            iCountReq = __pThePuser->onReq();
            iStatus = __pThePuser->getDevStatus();
#if defined(_VEND_LiveStreammingNewMedia_ZSDC)
            iStatus = Pusher_PushStatus_T_Pushing;
#endif //_VEND_LiveStreammingNewMedia_ZSDC

#if defined(_USE_MODULES_LocalDataBase)

            m_dbLiveStreamming->startStreamming(\
                                                __pThePuser->getDevChnStreamID() \
                                                , strUrlHls \
                                                , strUrlRtmp \
                                                , ""/*, strUrlRtsp \ */
                                                , iCountReq \
                                               );
#endif //_USE_MODULES_LocalDataBase

            if (NULL != m_pCacheWithDb_PullerStream2Rtmp)
            {
                m_pCacheWithDb_PullerStream2Rtmp->modPair(__pPullerTmp->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlHls,   strUrlHls , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(__pPullerTmp->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlHlss,   strUrlHlss , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(__pPullerTmp->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlRtmp,   strUrlRtmp, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(__pPullerTmp->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlFlv,   strUrlFlv , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(__pPullerTmp->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlDash,   strUrlDash , modPair_If_Not_Exist_Then_Add);
            }

            if (__pPullerTmp)
            {
                delete __pPullerTmp;
                __pPullerTmp = NULL;
            }

            return -2;
        }
    }
    if (__pPullerTmp)
    {
        delete __pPullerTmp;
        __pPullerTmp = NULL;
    }

    CPusherStream2Hls *pPuller = new CPusherStream2Hls;

    if (NULL == pPuller)
    {
        ez_printf_error("Not enough memory.\n");

        return -3;
    }
    else
    {
        // 设置配置参数
        //pPuller->setAutoLiveSecond(m_iAutoStopAfterSec);
        pPuller->setAutoLiveSecond(m_iAutoStopAfterSecRtmp);

        int iretSetPeriod = pPuller->setValidPeriod(strValidPeriod);
        ARG_USED(iretSetPeriod);
        ez_printf_debug("iretSetPeriod(%s):%d--%d\n", strValidPeriod.c_str(), iretSetPeriod, pPuller->getValidPeriod());

        pPuller->setHostRelative(m_strHostRelative);
        pPuller->m_strEndPoint4VideoRs = m_strEndPoint4VideoRs;
        pPuller->setTykdWsUrl(m_strTykdWsUrl);
        //ez_printf_debug("setTykdWsUrl:%s\n", m_strTykdWsUrl.c_str());

        pPuller->m_strID      = strID      ;
        pPuller->m_strType    = strType    ;
        pPuller->m_strUrl     = strUrl     ;
        pPuller->m_strAccount = strAccount ;
        pPuller->m_strPasswd  = strPasswd  ;
        pPuller->m_strChnnel  = strChnnel  ;
        pPuller->m_strSubtype = strSubtype ;
        //pPuller->m_strVideoEncode = strVideoEncode ;

        pPuller->m_iAudioEncode = 0;
        pPuller->m_iVideoEncode = 0;
        pPuller->m_iVideoWidth = 0;
        pPuller->m_iVideoFps = 0;

        ezConvertFromString(pPuller->m_iAudioEncode, strAudioEncode);
        ezConvertFromString(pPuller->m_iVideoEncode, strVideoEncode);
        ezConvertFromString(pPuller->m_iVideoWidth, strVideoResize);
        ezConvertFromString(pPuller->m_iVideoFps, strVideoRefps);

        // 协议中传0表示自动， 自动则用配置
        if (0 == pPuller->m_iVideoEncode)
        {
            pPuller->m_iVideoEncode = m_iDefaultVideoCodec;
        }

        if (0 == pPuller->m_iAudioEncode)
        {
            pPuller->m_iAudioEncode = m_iDefaultAudioCodec;
        }

        //视频上云不转码
#if defined(_VENDOR_newMedia__)
        pPuller->m_iVideoEncode = 0;
        pPuller->m_iVideoWidth = 0;
        pPuller->m_iVideoFps = 0;
#endif

        //ez_printf_debug("VE:%s, VR:%s, VFps:%s\n", strVideoEncode.c_str(), strVideoResize.c_str(), strVideoRefps.c_str());
        ez_printf_debug("AE:%d, VE:%d, VR:%d, VFps:%d\n", pPuller->m_iAudioEncode, pPuller->m_iVideoEncode, pPuller->m_iVideoWidth, pPuller->m_iVideoFps);

        // 强制转码
        //pPuller->m_iVideoEncode = 1;
        //pPuller->m_iVideoWidth = 3;
        //pPuller->m_iVideoFps = 15;

        pPuller->setDevChnStreamID(strID, strChnnel, strSubtype);
        pPuller->setIdDst(pPuller->getDevChnStreamID()/*strTykdDevID+"_"+strChannel+"_"+strStream*/, m_iDynamicIssueID_LiveStream);

        //ez_printf_info("pPuller->getDevChnStreamID():%s\n", pPuller->getDevChnStreamID().c_str());
        pPuller->m_iPushedByTheFront = m_iPushedByTheFront;
        if (1 == pPuller->m_iPushedByTheFront)
            ez_printf_debug("use m_iPushedByTheFront\n");


        // 原始数据++
        //iCountReq=(++(pPuller->m_iCountReq));
        iCountReq = pPuller->onReq();
        iStatus = pPuller->getDevStatus();

        std::string strThrName = PREFIX_CPusherStream2Hls_THREAD_NAME + pPuller->getIdDst()/*strTykdDevID*/;
        pPuller->SetThreadName(strThrName);

        int iret_chkValid = pPuller->checkValid();
        if (iret_chkValid < 0)
        {
            return -5;
        }

        EZTHREAD_BOOL bRet = pPuller->Start(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);
        //ez_printf_info("pPuller->getDevChnStreamID():%s\n", pPuller->getDevChnStreamID().c_str());

        if (EZTHREAD_BOOL_TRUE == bRet)
        {
            addPullerStream2Hls(pPuller);

            strUrlHls = pPuller->getUrlHlsFix();
            strUrlHlss = pPuller->getUrlHlssFix();
            strUrlRtmp = pPuller->getUrlRtmpFix();
            strUrlFlv = pPuller->getUrlFlvFix();
            strUrlDash = pPuller->getUrlDashFix();
            // 此时还不能确定
            strMediaSource = pPuller->m_strPlayUrl_Src;
            strDstStreamID = pPuller->getIdDst();

#if defined(_USE_MODULES_LocalDataBase)

            m_dbLiveStreamming->startStreamming(\
                                                pPuller->getDevChnStreamID() \
                                                , strUrlHls \
                                                , strUrlRtmp \
                                                , ""/*, strUrlRtsp \*/
                                                , iCountReq \
                                               );
#endif //_USE_MODULES_LocalDataBase

            // 第一次start的时候有效
            if (NULL != m_pCacheWithDb_PullerStream2Rtmp)
            {
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key, pPuller->getDevChnStreamID(), modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_ID,           strID         , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Alias,           strAlias         , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Type,         strType       , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_ValidPeriod,  strValidPeriod, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Url,          strUrl        , modPair_If_Not_Exist_Then_Add);

                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Account,      strAccount    , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Passwd,       strPasswd     , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_ChnnelIn,     strChnnelIn   , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_SubtypeIn,    strSubtypeIn  , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_AudioEncode,  strAudioEncode, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_VideoEncode,  strVideoEncode, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_VideoResize,  strVideoResize, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_VideoRefps,   strVideoRefps , modPair_If_Not_Exist_Then_Add);
            //}

            //if (NULL != m_pCacheWithDb_PullerStream2Rtmp)
            //{
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlHls,   strUrlHls , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlHlss,   strUrlHlss , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlRtmp,   strUrlRtmp, modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlFlv,   strUrlFlv , modPair_If_Not_Exist_Then_Add);
                m_pCacheWithDb_PullerStream2Rtmp->modPair(pPuller->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_UrlDash,   strUrlDash , modPair_If_Not_Exist_Then_Add);

                // 用于反查
                m_pCacheWithDb_PullerStream2Rtmp->modPair(strDstStreamID+def_CacheWithDb_PullerStream2Rtmp_Key, pPuller->getDevChnStreamID(), modPair_If_Not_Exist_Then_Add);
            }

            // Hls快速播放
            //#if _FUNC_HlsCONTENTS_PreHlsIndex
            if (m_strHlsPreAd.length() > 0)
            {
                std::string strStreamId = pPuller->getIdDst();
#if 0
                // 命令行方式
                std::string strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-.m3u8 /tmp/hls/" + strStreamId + ".m3u8; ";

                strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-0.ts /tmp/hls/" + strStreamId + "-0.ts; ";
                strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-1.ts /tmp/hls/" + strStreamId + "-1.ts; ";
                strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-2.ts /tmp/hls/" + strStreamId + "-2.ts; ";
                strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-3.ts /tmp/hls/" + strStreamId + "-3.ts; ";
                strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-4.ts /tmp/hls/" + strStreamId + "-4.ts; ";
                strCmd = "cp /home/pss/html/pssix_ad/1/pssix_ad-5.ts /tmp/hls/" + strStreamId + "-4.ts; ";

                system(strCmd.c_str());
#else

                std::string strCmd;
                strCmd += "rm -f /tmp/hls/" + strStreamId + "*; ";
                strCmd += "cp /home/pss/html/pssix_ad/1/pssix_ad-0.ts /tmp/hls/" + strStreamId + "-0.ts; ";
                strCmd += "cp /home/pss/html/pssix_ad/1/pssix_ad-1.ts /tmp/hls/" + strStreamId + "-1.ts; ";
                strCmd += "cp /home/pss/html/pssix_ad/1/pssix_ad-2.ts /tmp/hls/" + strStreamId + "-2.ts; ";
                strCmd += "cp /home/pss/html/pssix_ad/1/pssix_ad-3.ts /tmp/hls/" + strStreamId + "-3.ts; ";
                strCmd += "cp /home/pss/html/pssix_ad/1/pssix_ad-4.ts /tmp/hls/" + strStreamId + "-4.ts; ";
                strCmd += "cp /home/pss/html/pssix_ad/1/pssix_ad-5.ts /tmp/hls/" + strStreamId + "-5.ts; ";

                system(strCmd.c_str());

                char buff[2048] = {'\0'};
                strcpy(buff, def_THE_M3U8_CONTENTS_PreHlsIndex);
                char *pBuf = replace_chars_with_chars(buff, "pssix_ad", strStreamId.c_str());

                // 自行保存文件
                std::string strFileName = "/tmp/hls/" + strStreamId + ".m3u8";
                int iret_sf = ez_save_file_from_memory(strFileName.c_str()
                                                       , pBuf
                                                       , strlen(pBuf));

                ez_printf_debug("mem2file(%d):%s\n", iret_sf, strFileName.c_str());
#endif

            }
            //#endif

        }
        else
        {
            delete pPuller;
            return -4;
        }
    }

    return 0;
}

// -1, 不存在 -2 刚刚开启，暂时不容许stop
int CStreamPusherTykd::stopPullStream2Hls( \
        const std::string &strIDIn
        , const std::string &strType
        , const std::string &strUrl
        , const std::string &strAccount
        , const std::string &strPasswd
        , const std::string &strChnnelIn
        , const std::string &strSubtypeIn
                                         )
{
    // 参数修正
    std::string strID = strIDIn;
    std::string strChnnel;
    std::string strSubtype;

    // support生成id
    if (ezDef_StreamType_Str_Streamming == strType)
    {
        if (strUrl.length()<5)
        {
            return -101;
        }

        if (strIDIn.length()<10)
        {
            strChnnel  = "1" ;
            strSubtype = "1" ;

            char dst[SHA1HashSizeDestBuffer] = {0};
            int iret = ez_sha1_str(dst, sizeof(dst), strUrl.c_str() );

            if (iret<0)
            {
                return -102;
            }

            strID = dst;
    	}
    }

    //
    if (strID.length()<2
        || strType.length()<1
        /*|| strUrl.length()<2*//*并不是所有情况都需要url*/)
    {
        return -103;
    }

    // 防错处理
    // 全球眼等这些字段可以不传
    if (ezDef_StreamType_Str_megaeye == strType
        ||ezDef_StreamType_Str_hk8200 == strType)
    {
        strChnnel  = "1" ;
        strSubtype = "1" ;
    }
    else
    {
        strChnnel  = strChnnelIn  ;
        strSubtype = strSubtypeIn ;
    }

    CPusher *__pPullerTmp = new CPusher;
    assert(__pPullerTmp);
    __pPullerTmp->setDevChnStreamID(strID, strChnnel, strSubtype);
    // 默认没有发现
    int iret = -1;

    for (def_PusherContainer::iterator it = m_vPullerStream2Hls.begin(); it != m_vPullerStream2Hls.end(); it++)
    {
        CPusher *pPusher = (*it);
        if (NULL==pPusher)
        {
            continue;
        }

    //for (unsigned int ii=0; ii<m_vPullerStream2Hls.size(); ii++)
    //{
        if (//m_vPullerStream2Hls[ii] != NULL &&
            //(strID == m_vPullerStream2Hls[ii]->getIdDst() ) /**/
            //(strID==m_vPullerStream2Hls[ii]->getTykdDevID()/* && strChnnel==m_vPullerStream2Hls[ii]->getChanel() && strSubtype==m_vPullerStream2Hls[ii]->getStream()*/)
            (__pPullerTmp->getDevChnStreamID() == /*m_vPullerStream2Hls[ii]*/pPusher->getDevChnStreamID())
           )
        {
            // 刚启动的暂时不容许关闭
            if (pPusher->secOfStarted() < def_MinSecOfRtmpStreammingStarted)
            {
                ez_printf_error("Have been start in %ld sec, stop not permited:%s\n", pPusher->secOfStarted(), pPusher->getDevChnStreamID().c_str());
                iret = -2;
                break;
                //return -2;
            }

#if defined(_USE_MODULES_LocalDataBase)
            m_dbLiveStreamming->stopStreamming(\
                                               pPusher->getDevChnStreamID()
                                              );
#endif //_USE_MODULES_LocalDataBase
            CPusherStream2Hls *__pThePuser = (CPusherStream2Hls *)pPusher;
            __pThePuser->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);

            delPullerStream2Hls(__pThePuser);
            if (NULL != m_pCacheWithDb_PullerStream2Rtmp)
            {
                //只删除一个
                m_pCacheWithDb_PullerStream2Rtmp->delPair(__pPullerTmp->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key);
            }
            ez_printf_info("done stopPull of ID(%s)\n", strID.c_str());

            iret = 0;
            break;
            //return 0;
        }
    }

    if (__pPullerTmp)
    {
        delete __pPullerTmp;
        __pPullerTmp = NULL;
    }

    ez_printf_error("stopPull (%s):%d\n", strID.c_str(), iret);

    return iret;
}

int CStreamPusherTykd::addPullerStream2Hls(CPusherStream2Hls *pPuller)
{
    if (NULL == pPuller)
    {
        return -1;
    }
    CEZLock __lock(m_MutexPullerStream2Hls);
    m_vPullerStream2Hls.push_back(pPuller);

    return -1;
}

int CStreamPusherTykd::delPullerStream2Hls(CPusherStream2Hls *pPuller)
{
    if (NULL == pPuller)
    {
        return -1;
    }
    //CEZLock __lock(m_MutexPullerStream2Hls);
    for (def_PusherContainer::iterator it = m_vPullerStream2Hls.begin(); it != m_vPullerStream2Hls.end(); it++)
    {
        CPusher *pPusher = (*it);
        if (NULL==pPusher)
        {
            continue;
        }
        CPusherStream2Hls *__pThePuser = dynamic_cast<CPusherStream2Hls *>(pPusher);
        // 不会发生， 但是防止后期维护后出问题
        assert(__pThePuser);
    //unsigned int ii;
    //for (ii=0; ii<m_vPullerStream2Hls.size(); ii++)
    //{
        if (__pThePuser == pPuller)
        {
            //delPullerStream2Hls(ii);
            CEZLock __lock(m_MutexPullerStream2Hls);

            m_vPullerStream2Hls.erase(it);

            return 0;
        }
    }

    return -1;
}
unsigned int CStreamPusherTykd::getNumPullerStream2Hls()
{
    return m_vPullerStream2Hls.size();
}
//
int CStreamPusherTykd::listPullerStream2Hls(CPssMsg_requestMsgDeviceListAck &msgDevices, unsigned int iPageSize, unsigned int iPages)
{
    msgDevices.m_iTotalCount = m_vPullerStream2Hls.size();

    //std::vector<CPusherStream2Hls *>::const_iterator it = m_vPullerStream2Hls.begin();
    def_PusherContainer::const_iterator it = m_vPullerStream2Hls.begin();

    for ( ; it != m_vPullerStream2Hls.end() ; it++)
    {
        CDeviceMsg __device;
        //__device.m_strStatus = "1";

        if (m_pCacheWithDb_PullerStream2Rtmp)
        {
            std::string __strUrl;
            m_pCacheWithDb_PullerStream2Rtmp->getPair((*it)->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Url,           __strUrl         );

            ez_parsed_url_t *parsed_url = ez_parse_url(__strUrl.c_str());
            if (parsed_url)
            {
                __device.m_strHostIp = parsed_url->host;

                // use the port
                if (!strcasecmp(parsed_url->scheme, "rtsp"))
                {
                    __device.m_strRtspPort = parsed_url->port;
                }
                else if (!strcasecmp(parsed_url->scheme, "http"))
                {
                    //__port_http = atoi(parsed_url->port);
                }
                else if (!strcasecmp(parsed_url->scheme, "pss"))
                {
                    // 防止：pss://192.168.5.66:18088?http=80&rtsp=554
                    if (NULL == parsed_url->path
                        || NULL == parsed_url->query)
                    {
                        //break;
                    }
                    else
                    {

                        char *pFindPort = NULL;

                        pFindPort = strstr(parsed_url->query, "http=");
                        if (pFindPort)
                        {
                            //__port_http = atoi(pFindPort+strlen("http="));
                        }

                        pFindPort = strstr(parsed_url->query, "rtsp=");
                        if (pFindPort)
                        {
                            int __port_rtsp = atoi(pFindPort+strlen("rtsp="));
                            __device.m_strRtspPort = ezConvertToString(__port_rtsp);
                        }

                    }
                }
                else // unknow
                {
                    __device.m_strRtspPort = "0";
                }

                ez_parsed_url_free(parsed_url);
            }

            m_pCacheWithDb_PullerStream2Rtmp->getPair((*it)->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Type,           __device.m_strCode);
            std::string __strAlias;
            m_pCacheWithDb_PullerStream2Rtmp->getPair((*it)->getDevChnStreamID()+def_CacheWithDb_PullerStream2Rtmp_Key_Alias,           __strAlias);
            if (__strAlias.length() > 0)
            {
                __device.m_strName = __strAlias;
            }
            else
            {}

            // 状态
#if 0
            std::string strFPN = "/tmp/hls/" + (*it)->getDevChnStreamID() + ".m3u8";

            int iret_fexists = ez_check_file_exist(strFPN.c_str());
#else
            int iret_fexists = ((*it)->getDevStatus()==em_terminal_status_online) ? 1 : 0;
#endif
            if (1 == iret_fexists)
            {
                __device.m_strStatus = "1";
            }
            else
            {
                __device.m_strStatus = "0";
            }
        }
        __device.m_strDevID = (*it)->getTykdDevID();
        __device.m_strChannelNum = "1";

        if (__device.m_strName.empty())
            __device.m_strName = (*it)->getDevChnStreamID();

        msgDevices.Devices.push_back(__device);
    }

    return m_vPullerStream2Hls.size();
}

int CStreamPusherTykd::maintainPullStream2Hls()
{
    //ez_printf_debug("%s\n", __FUNCTION__);

    for (def_PusherContainer::iterator it = m_vPullerStream2Hls.begin(); it != m_vPullerStream2Hls.end(); it++)
    {
        CPusher *pPusher = (*it);
        if (NULL==pPusher)
        {
            continue;
        }
        CPusherStream2Hls *__pThePuser = dynamic_cast<CPusherStream2Hls *>(pPusher);
        // 不会发生， 但是防止后期维护后出问题
        assert(__pThePuser);

    //for (unsigned int ii=0; ii<m_vPullerStream2Hls.size(); ii++)
    //{
        int iLife = __pThePuser->chkLifeCycle();

        if (Pusher_LifeCycle_T_TimeOut == iLife) //
        {
            ez_printf_debug("Pusher_LifeCycle_T_TimeOut\n");

            // 判断是否还在被播放
#ifdef _FUNC_LiveStatusServer
            // rtmp 播放判断
            int __iPlayerNums = g_AgentLiveStatusServer.GetHandler()->getPlayerNums(__pThePuser->getIdDst());
            //ARG_USED(__iPlayerNums);
DBG(
            ez_printf_debug("%s:%d\n", "iPlayerNums", __iPlayerNums);
);
            if (__iPlayerNums > 0)
            {
                __pThePuser->onReq();

                // 还有播放 不停止
                continue;
            }

            //hls播放判断
            int __iValidPeriod = __pThePuser->getAutoLiveSecond();//ValidPeriod_SECOND_MIN+1;//30;//
            time_t __ttLastPlayTime = g_AgentLiveStatusServer.GetHandler()->getLastPlayTime(__pThePuser->getIdDst());
            time_t __ttNow = time(NULL);

            ez_printf_debug("%ld, %ld, %ld, %ld\n",  __ttLastPlayTime, __iValidPeriod, __ttNow, __ttLastPlayTime+__iValidPeriod);

            if (__ttLastPlayTime+__iValidPeriod > __ttNow)
            {
                //__pThePuser->onReq();
                ez_printf_debug("%s is playing, do not stop now.\n",  __pThePuser->getIdDst().c_str());

                // 还有播放没有超时，不停止
                continue;
            }
#endif // _FUNC_LiveStatusServer

            ez_printf_info("%s ID(%s) -- Stop .\n", __FUNCTION__, __pThePuser->getTykdDevID().c_str());

            __pThePuser->Stop(this, (CPusher::PusherSignalProc_t)&CStreamPusherTykd::OnPullMsg);
            // 收到消息的时候删除， 此处不必
            //delPullerStream2Hls(ii);
        }
    }//

    /////
    static int __maintainPullStream2Hls_first_run = 1;
    if (1 == __maintainPullStream2Hls_first_run)
    {
        __maintainPullStream2Hls_first_run=0;

        if (1==m_iStartPullAuto // 配置了自动启动
            && NULL != m_pCacheWithDb_PullerStream2Rtmp)
        {
            def_uniKeyVector __val;

            // 获取开启的转码id
            m_pCacheWithDb_PullerStream2Rtmp->getValueLike(__val, def_CacheWithDb_PullerStream2Rtmp_Key);
            def_uniKeyVector::const_iterator it = __val.begin();
            for ( ; it != __val.end() ; it++)
            {
                DBG(
                    std::cout << "::::::::" << *it << std::endl;
                );
                std::string strID           ;
                std::string strAlias           ;
                std::string strType         ;
                std::string strValidPeriod  ;
                std::string strUrl          ;
                std::string strAccount      ;
                std::string strPasswd       ;
                std::string strChnnelIn     ;
                std::string strSubtypeIn    ;
                std::string strAudioEncode  ;
                std::string strVideoEncode  ;
                std::string strVideoResize  ;
                std::string strVideoRefps   ;
                std::string strUrlHls       ;
                std::string strUrlHlss       ;
                std::string strUrlRtmp      ;
                std::string strUrlFlv      ;
                std::string strUrlDash      ;
                std::string strMediaSource;
                std::string strDstStreamID;
                EM_TERMINAL_STATUS_T iPushStatus = em_terminal_status_unknown;
                int         iCountReq       ;

                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_ID,           strID         );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_Alias,           strAlias);
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_Type,         strType       );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_ValidPeriod,  strValidPeriod);
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_Url,          strUrl        );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_Account,      strAccount    );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_Passwd,       strPasswd     );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_ChnnelIn,     strChnnelIn   );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_SubtypeIn,    strSubtypeIn  );
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_AudioEncode,  strAudioEncode);
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_VideoEncode,  strVideoEncode);
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_VideoResize,  strVideoResize);
                m_pCacheWithDb_PullerStream2Rtmp->getPair(*it+def_CacheWithDb_PullerStream2Rtmp_Key_VideoRefps,   strVideoRefps );


                int iret = g_StreamPusherTykd.startPullStream2Hls( \
                           strID
                           , strAlias
                           , strType
                           , strValidPeriod
                           , strUrl
                           , strAccount
                           , strPasswd
                           , strChnnelIn
                           , strSubtypeIn
                           , strAudioEncode
                           , strVideoEncode
                           , strVideoResize
                           , strVideoRefps

                           , strUrlHls
                           , strUrlHlss
                           , strUrlRtmp
                           , strUrlFlv
                           , strUrlDash
                           , strMediaSource
                           , strDstStreamID
                           , iPushStatus
                           , iCountReq);

                if (0 == iret)
                {
                    // success
                    //ez_printf_debug("%s::(%s).\n", def_CacheWithDb_PullerStream2Rtmp_Key_ID, strID.c_str());
                }
                else
                {
                    ez_printf_error("Failed(%d) startPullStream2Hls:%s\n", iret, strID.c_str());
                }

                //ARG_USED(iret);
            }//
        } // if (NULL != m_pCacheWithDb_PullerStream2Rtmp)
    }
    /////

    return 0;
}
void CStreamPusherTykd::ListPullerStream2Hls(std::string &strDump, unsigned int iFrom, unsigned int iTo, int iLevel)
{
    unsigned int ii = 0;
    char sendto[1024];

    unsigned int __from = iFrom<m_vPullerStream2Hls.size()?iFrom:m_vPullerStream2Hls.size()-1;
    unsigned int __to = iTo<=m_vPullerStream2Hls.size()?iTo:m_vPullerStream2Hls.size();

    snprintf(sendto, sizeof (sendto)-1, "%s PullerStream2Hls :%lu \n", PULLER_LIST_SPLITER, m_vPullerStream2Hls.size());
    strDump += sendto;
    for (def_PusherContainer::iterator it = m_vPullerStream2Hls.begin(); it != m_vPullerStream2Hls.end(); it++)
    {
    //for (ii=__from; ii<__to; ii++)
    //{
        if (ii<__from)
            continue;
        else if (ii>__to)
            break;
	
        snprintf(sendto, sizeof (sendto)-1, "%-3d %-5d %-5u %-8lu %-18s %-18s\n"
                 , ii+1
                 , (*it)->getChildPid()
                 , (*it)->getRound()
                 , (*it)->getPushLast()
                 , (*it)->getTykdDevID().c_str()
                 , (*it)->getIdDst().c_str()
                );

        strDump += sendto;
    }
}

#endif //#if defined(_FUNC_LiveStreammingStream2Hls)
//End stream pull ... 2017-05-19 8:33:54
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


