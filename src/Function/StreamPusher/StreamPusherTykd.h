/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StreamPusherTykd.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: StreamPusherTykd.h 5884 2016-11-03 09:17:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-03 09:17:15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#if !defined(_StreamPusherTykd_H)
#define _StreamPusherTykd_H

#include <string>
#include <ez_socket.h> /*ezDef_StreamType_Str_megaeye*/
#include "EZThread.h"

#include "../../UniTransDef.h"

//
#include "../Stack/pss/PssMsg.h"

//数据持久化
#include "../../DBAgent/CacheKeyValue.h"

#if defined(_FUNC_LiveStreammingTykd2Hls)
#include "PusherTykd2Rtmp.h"
#endif //_FUNC_LiveStreammingTykd2Hls

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
#include "PusherTykd2Rtsp.h"
#endif // _FUNC_LiveStreammingTykd2Rtsp

#if defined(_FUNC_LiveStreammingStream2Hls)
#include "PusherStream2Hls.h"
#endif

#if defined(_USE_MODULES_LocalDataBase)
#include "../../DBAgent/DbLiveStreamming.h"
#endif // _USE_MODULES_LocalDataBase

// 录像
#include "RecordTykdMediaUrl.h"
#include "RecFileIdx.h"
#if defined(_Func_Use_FileIdxer)
#include "../FileIdxer/FileIdxer.h"
#endif
//上报状态
#if defined(_Func_CCourier_StreamPusherTykd)
#include "Courier_StreamPusherTykd.h"
#endif // _Func_CCourier_StreamPusherTykd

#define g_StreamPusherTykd (*CStreamPusherTykd::instance())

class CStreamPusherTykd : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CStreamPusherTykd);

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

private:
    CStreamPusherTykd();
    virtual ~CStreamPusherTykd();

    EZTHREAD_BOOL isReady();
#if defined(_Func_CCourier_StreamPusherTykd)
    CCourier_StreamPusherTykd *m_pCourier_StreamPusherTykd;
#endif // _Func_CCourier_StreamPusherTykd

public:
    void OnPullMsg(int iMsg, void *pPuller);
    int getCfg_PushInterval();

    // 下述为功能公用变量
private:
    //动态ID
    // 实时
    int m_iDynamicIssueID_LiveStream;
    // 自动停止时间 sec
    int m_iAutoStopAfterSec;
    // 自动停止时间 sec rtmp
    int m_iAutoStopAfterSecRtmp;
    int m_iStartPullAuto;
	
    // 录像
    int m_iDynamicIssueID_Record;//动态ID

    int m_iStreamPusherMaint;

    int m_iFileHelperPeriod;
    // hls的起始广告
    std::string m_strHlsPreAd;
    // 数据库公用
#if defined(_USE_MODULES_LocalDataBase)

    CDbLiveStreamming *m_dbLiveStreamming;
#endif //_USE_MODULES_LocalDataBase

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
public:
    // rtsp
    int startPullRtsp( \
                       const std::string &strTykdDevID
                       , const std::string &strChannel
                       , const std::string &strStream
                       , const std::string &strValidPeriod
                       , std::string &strUrlRtsp
                       , int &iCountReq);

    int stopPullRtsp( \
                      const std::string &strTykdDevID
                      , const std::string &strChannel
                      , const std::string &strStream);
    unsigned int getNumPullerRtsp();

    void ListPullerRtsp(std::string &strDump, unsigned int iFrom, unsigned int iTo, int iLevel=enLogLevelShow_contes);

private:
    //rtsp
    CEZMutex m_MutexPullerTykd2Rtsp;
    std::vector<CPusherTykd2Rtsp *> m_vPullerTykd2Rtsp;
    int addPullerTykd2Rtsp(CPusherTykd2Rtsp *pPullerTykd2Rtsp);
    int delPullerTykd2Rtsp(CPusherTykd2Rtsp *pPullerTykd2Rtsp);
    int delPullerTykd2Rtsp(int idx);

    int maintainPullTykd2Rtsp();

    // 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb_PullerTykd2Rtsp;
#endif // #if defined(_FUNC_LiveStreammingTykd2Rtsp)
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //Begin Stream pull ... 2017-05-19 8:33:54
    // 实际是 LiveStreammingStream2Rtmp, 命名将错就错
#if defined(_FUNC_LiveStreammingStream2Hls)
public:
    int startPullStream2Hls( \
                             const std::string &strID
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
                             , int &iCountReq);

    int stopPullStream2Hls( \
                            const std::string &strID
                            , const std::string &strType
                            , const std::string &strUrl
                            , const std::string &strAccount
                            , const std::string &strPasswd
                            , const std::string &strChnnelIn
                            , const std::string &strSubtypeIn
                          );

    unsigned int getNumPullerStream2Hls();
    int listPullerStream2Hls(CPssMsg_requestMsgDeviceListAck &msgDevices, unsigned int iPageSize, unsigned int iPages);
    void ListPullerStream2Hls(std::string &strDump, unsigned int iFrom, unsigned int iTo, int iLevel);

private:
    CEZMutex m_MutexPullerStream2Hls;
    //std::vector<CPusherStream2Hls *> m_vPullerStream2Hls;
    def_PusherContainer m_vPullerStream2Hls;
    int addPullerStream2Hls(CPusherStream2Hls *pPullerStream2Hls);
    int delPullerStream2Hls(CPusherStream2Hls *pPullerStream2Hls);
    //int delPullerStream2Hls(int idx);
    // 维护
    int maintainPullStream2Hls();

    // 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb_PullerStream2Rtmp;
#endif// _FUNC_LiveStreammingStream2Hls
    //End stream pull ... 2017-05-19 8:33:54
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if defined(_FUNC_LiveRecord)
    /* my rec */
private:

    std::string m_strIndexPath;
    std::string m_strMediaPath;
    std::string m_strIsueUrlPrefix;
    CRecFileIdx *m_pIdxRecFile;
#if defined(_Func_Use_FileIdxer)
    CFileIdxer *m_pFileIdxer;
#endif//_Func_Use_FileIdxer
    int m_iFuncMask;

    int m_iSegmentFileTime; // 文件片段时长
    int m_iFileValidPeriod;// 文件保存时长

    int m_iStartRecordTykdMediaUrlWithCatchDB;

    //rec
    CEZMutex m_MutexRecordTykdMediaUrl;
    std::vector<CRecordTykdMediaUrl *> m_vRecordTykdMediaUrl;

    int addRecordTykdMediaUrl(CRecordTykdMediaUrl *pRecordTykdMediaUrl);
    int delRecordTykdMediaUrl(CRecordTykdMediaUrl *pRecordTykdMediaUrl);
    int delRecordTykdMediaUrl(int idx);

public:
    void ListRecordTykdMediaUrl(std::string &strDump, unsigned int iFrom, unsigned int iTo, int iLevel);
    size_t getNumRecordTykdMediaUrl();
    int getRecorderAttr( \
                         const std::string &strDevChnStreamID
                         , int &iFileSegTime
                         , int &iFileValidPeriod);

    int startRecordTykdMediaUrl( \
                                 const std::string &strTykdDevID
                                 , const std::string &strChannel
                                 , const std::string &strStream
                                 , const std::string &strFileSegTime
                                 , const std::string &strFileValidPeriod
                                 , const std::string &strRecValidPeriod

                                 , const int iStreamType          // Atatch
                                 , const std::string &Url           // Atatch
                                 , const std::string &Account       // Atatch
                                 , const std::string &Passwd        // Atatch
                                 , const std::string &VideoEncode   // Atatch
                                 , const std::string &AudioEncode   // Atatch

                                 , EM_TERMINAL_STATUS_T &iStatus
                                 , EM_TERMINAL_STATUS_T &iUpStatus
                                 , unsigned int &UsedSpace
                                 , unsigned int &UsedUpSpace
                               );

    bool isStartedRecordTykdMediaUrl( \
                                      const std::string &strTykdDevID
                                      , const std::string &strChannel
                                      , const std::string &strStream
                                    );

    int stopRecordTykdMediaUrl( \
                                const std::string &strTykdDevID
                                , const std::string &strChannel
                                , const std::string &strStream);

    // 删除文件
#if 0
    int delRecordTykdMediaUrl( \
                               const std::string &strTykdDevID
                               , const std::string &strChannel
                               , const std::string &strStream
                               , time_t dtStart
                               , time_t dtEnd
                             );
    //查询文件
    int queryRecordTykdMediaUrl( \
                                 const std::string &strTykdDevID
                                 , const std::string &strChannel
                                 , const std::string &strStream
                                 , time_t ttStart
                                 , time_t ttEnd
                                 , int iItems
                                 , int iPages
                                 , tdef_RecordList &listRecordFile
                               );
#endif // if 0
    //查询文件
    int queryRecord( \
                                 const std::string &strTykdDevID
                                 , const std::string &iChannel
                                 , const std::string &iStream
                                 , time_t ttStart
                                 , time_t ttEnd
                                 , int iItems
                                 , int iPages
                                 , tdefList_CFileStruct &listRecordFile
                               );

    int makeFileUrl(const CRecFileNode &node, std::string &strUrl, std::string &strSnapUrl);
    // 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb_RecordTykdMediaUrl;

    int maintainRecordTykdMediaUrl();
#endif //#if defined(_FUNC_LiveRecord)
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
private:
    // 对应ConfigStreamPusher.iPushInterval
    int m_iPushInterval;// 重试间隔， 0 默认， >60 用这个参数， 1-59 备用
    // 替换RelayPlayUrl(turn)的ip
    // 从配置而来
    std::string m_strHostRelative;
    //tykd 视频源平台接口地址
    std::string m_strTykdWsUrl;

    // 视频源平台接口地址
    std::string m_strEndPoint4VideoRs;

    // 从配置而来, 默认的音视频编码配置
    int m_iDefaultVideoCodec;
    int m_iDefaultAudioCodec;

    int m_iPushedByTheFront;
public:
    void ListPuller(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void ListPullerContents(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void ListPullerTables(std::string &strDump, int iLevel=enLogLevelShow_contes);
    void dumpPuller(const std::string &strDevID, std::string &strDump, int iLevel=enLogLevelShow_contes);

};

#endif // !defined(_StreamPusherTykd_H)

