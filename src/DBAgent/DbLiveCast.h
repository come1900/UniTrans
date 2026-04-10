/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbLiveCast.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbLiveCast.h 5884 2017-04-25 02:24:41Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-25 02:24:41  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <SQLiteCpp.h>
#include <VariadicBind.h>

//#include "DbDriver.h"
#if !defined(_DbLiveCast_H)
#define _DbLiveCast_H

class CDbLiveCast//:public CDbDriver
{
public:
    // Constructor
    CDbLiveCast();
    virtual ~CDbLiveCast();

    virtual int Open(const std::string strConn);

    int getAppSecret(const std::string &AppKey, std::string &AppSecret);

    //添加Cast记录
    int addCast(const std::string &StreamID
                , const std::string &PushUrlRtmp
                , const std::string &PushUrlRtsp
                , const std::string &PlayUrlRtsp
                , const std::string &PlayUrlRtmp
                , const std::string &PlayUrlHls
               );

    //删除rtsp记录
    int delCast(const std::string &StreamID);
    //修改rtsp记录
    int modCast(const std::string &StreamID, const int reqDt, const int reqCount=-1);
    //获取rtsp记录
    int getCast(const std::string &StreamID, int &FilePeriod, int &ValidPeriod);

private:
    SQLite::Database    *m_pDb;    ///< Database connection
    SQLite::Statement   *m_pQuery; ///< Database prepared SQL query
};

#endif // !defined(_DbLiveCast_H)
