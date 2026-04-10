/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbLiveStreamming.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbLiveStreamming.h 5884 2017-04-27 04:14:38Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-27 04:14:38  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <SQLiteCpp.h>
#include <VariadicBind.h>

//#include "DbApp.h"

#if !defined(_DbLiveStreamming_H)
#define _DbLiveStreamming_H

#define StreamType_liveRtsp "liveRtsp"
#define StreamType_liveRtmp "liveRtmp"
#define StreamType_liveHls "liveHls"


class CLiveStreamDBNode
{
public:
    std::string StreamID     ;
    std::string UrlHls       ;
    std::string UrlRtmp      ;
    std::string UrlRtsp      ;
    int         countReq     ;
    int         ValidPeriod  ;
    time_t      dtStart      ;
    time_t      dtStop       ;
    time_t      dtCreate     ;
    time_t      dtReq        ;

    bool isValid();
};

class CDbLiveStreamming//:public CDbApp
{
public:
    // Constructor
    CDbLiveStreamming();
    virtual ~CDbLiveStreamming();

    virtual int Open(const std::string strConn);
    // 查找app sec
    int getAppSecret(const std::string &AppKey, std::string &AppSecret);
#if 0
    //添加rtsp记录
    int addRtsp(const std::string &StreamID, const std::string &PlayUrl);
    //删除rtsp记录
    int delRtsp(const std::string &StreamID);
    //修改rtsp记录
    int modRtsp(const std::string &StreamID, const int reqDt, const int reqCount=-1);
    //获取rtsp记录
    int getRtsp(const std::string &StreamID, int &FilePeriod, int &ValidPeriod);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // StreamType must: StreamType_XXXX
    //添加Stream记录
    int addStream(const std::string &StreamType, const std::string &StreamID, const std::string &PlayUrl);
    //删除Stream记录
    int delStream(const std::string &StreamType, const std::string &StreamID);
    //修改Stream记录
    int modStream(const std::string &StreamType, const std::string &StreamID, const int reqDt, const int reqCount=-1);
    //获取Stream记录
    int getStream(const std::string &StreamType, const std::string &StreamID, int &FilePeriod, int &ValidPeriod);

    //添加hls记录
    int addHls(const std::string &StreamID, const std::string &PlayUrl);
    //删除hls记录
    int delHls(const std::string &StreamID);
    //修改hls记录
    int modHls(const std::string &StreamID, const int reqDt, const int reqCount=-1);
    //获取hls记录
    int getHls(const std::string &StreamID, int &FilePeriod, int &ValidPeriod);
#endif
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 记录是否存在
    bool isStreammingExist(const std::string &StreamID);

    // 下列接口供组合调用
private:
    // insert
    int insertStreamming( \
                          const std::string &StreamID
                          , const std::string &strUrlHls
                          , const std::string &strUrlRtmp
                          , const std::string &strUrlRtsp
                          , const int iCountReq
                        );
    // update
    int updateStreamming( \
                          const std::string &StreamID
                          , const std::string &strUrlHls
                          , const std::string &strUrlRtmp
                          , const std::string &strUrlRtsp
                          , const int iCountReq
                          , const time_t dtStart
                          , const time_t dtStop
                        );
    // query
    int queryStreamming( \
                         const std::string &StreamID
                         , std::string &strUrlHls
                         , std::string &strUrlRtmp
                         , std::string &strUrlRtsp
                         , int &iCountReq
                         , time_t &dtStart
                         , time_t &dtStop
                       );
    // 功能接口
public:

    int startStreamming( \
                         const std::string &StreamID
                         , const std::string &strUrlHls
                         , const std::string &strUrlRtmp
                         , const std::string &strUrlRtsp
                         , const int iCountReq
                       );

    //
    int stopStreamming(const std::string &StreamID);

private:
    SQLite::Database    *m_pDb;    ///< Database connection
    SQLite::Statement   *m_pQuery; ///< Database prepared SQL query
};

#endif // !defined(_DbLiveStreamming_H)
