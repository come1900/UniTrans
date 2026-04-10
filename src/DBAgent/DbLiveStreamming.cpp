/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbLiveStreamming.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbLiveStreamming.cpp 5884 2017-04-27 04:14:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-27 04:14:43  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h>

#include <unistd.h>

#include "DbLiveStreamming.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
bool CLiveStreamDBNode::isValid()
{
    if (StreamID.length()<1)
    {
        return false;
    }

    return true;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//
static const char *psql_uniAppKey[] =
    {
        \
        /*1.*/ "CREATE TABLE IF NOT EXISTS uniAppKey(" 				\
        /*  */ "AppKey         varchar(16)     NOT NULL UNIQUE," 		\
        /*  */ "AppSecret      varchar(64)     NOT NULL," 				\
        /*  */ "HUID           varchar(16)             ," 				\
        /*  */ "dtCreate       datetime        DEFAULT 1490090786" 	\
        /*  */ "); ",
#if 0
        /*2.*/ "CREATE TABLE IF NOT EXISTS liveRtsp("   				\
        /*  */ "StreamID       varchar(256)     NOT NULL UNIQUE   ," 	\
        /*  */ "PlayUrl        varchar(1024)                      ," 	\
        /*  */ "countReq       INT                      ," 			\
        /*  */ "dtCreate       datetime        DEFAULT 1490090786 ," 	\
        /*  */ "dtReq          datetime        DEFAULT 1490090786  " 	\
        /*  */ "); ",
        /*3.*/ "CREATE TABLE IF NOT EXISTS liveHls("   				\
        /*  */ "StreamID       varchar(256)     NOT NULL UNIQUE   ," 	\
        /*  */ "PlayUrl        varchar(1024)                      ," 	\
        /*  */ "countReq       INT                      ," 			\
        /*  */ "dtCreate       datetime        DEFAULT 1490090786 ," 	\
        /*  */ "dtReq          datetime        DEFAULT 1490090786  " 	\
        /*  */ "); ",
        /*4.*/ "CREATE TABLE IF NOT EXISTS liveRtmp("   				\
        /*  */ "StreamID       varchar(256)     NOT NULL UNIQUE   ," 	\
        /*  */ "PlayUrl        varchar(1024)                      ," 	\
        /*  */ "countReq       INT                      ," 			\
        /*  */ "dtCreate       datetime        DEFAULT 1490090786 ," 	\
        /*  */ "dtReq          datetime        DEFAULT 1490090786  " 	\
        /*  */ "); ",
#endif
        /*5.*/ "CREATE TABLE IF NOT EXISTS liveStream("   				\
        /*  */ "StreamID       varchar(256)     NOT NULL UNIQUE   ," 	\
        /*  */ "UrlHls         varchar(1024)                      ," 	\
        /*  */ "UrlRtmp        varchar(1024)                      ," 	\
        /*  */ "UrlRtsp        varchar(1024)                      ," 	\
        /*  */ "countReq       INT                                ,"    \
        /*  */ "ValidPeriod    INT             DEFAULT 0          ,"    \
        /*  */ "dtStart        datetime        DEFAULT 0          ," 	\
        /*  */ "dtStop         datetime        DEFAULT 0          ," 	\
        /*  */ "dtCreate       datetime        DEFAULT 0 ," 	\
        /*  */ "dtReq          datetime        DEFAULT 0  " 	\
        /*  */ "); ",
        /*  */ NULL
    };

static const char *psql_uniAppKey_defaultValue[] = \
        {
            /*  */ "INSERT INTO uniAppKey VALUES (\"701818788\", \"c48faffaedf5d4cae01c6a21c11476fc118e32b0\", \"974620270\", 1490090786); ",
            /*  */ "INSERT INTO uniAppKey VALUES (\"701918788\", \"9cba8e3fe970ddbca35424db80197a0167d68813\", \"043979497\", 1490090786); ",
            /*  */ NULL
        };

CDbLiveStreamming::CDbLiveStreamming()
{
    m_pDb = NULL;
    m_pQuery = NULL;
}
CDbLiveStreamming::~CDbLiveStreamming()
{
    if (m_pDb)
    {
        delete m_pDb;
    }

    if (m_pQuery)
    {
        delete m_pQuery;
    }
}

int CDbLiveStreamming::Open(const std::string strConn)
{
    int iret = 0;

    try
    {
        if (NULL != m_pDb)
        {
            delete m_pDb;
        }

        m_pDb = new SQLite::Database(strConn, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    }
    //catch (std::exception& e)
    catch (SQLite::Exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );

        return -1; // unexpected error : exit the example program
    }

    // 初始化表
    for (int ii=0; psql_uniAppKey[ii]!=NULL; ii++)
    {
        try
        {
            int __ret;

            __ret = m_pDb->exec(psql_uniAppKey[ii]);

            DBG(
                std::cout << "EXEC SQL:(" << __ret << ")" << psql_uniAppKey[ii] << std::endl;
            );

            ARG_USED(__ret);
        }
        catch (SQLite::Exception& e)
        {
            DBG(
                std::cout << "error ini tables:" << ii << std::endl;
                std::cout << "SQLite exception: " << e.what() << std::endl;
            );

            iret = ii+1; //
        }
    }// for

    // 初始化数据
    for (int ii=0; psql_uniAppKey_defaultValue[ii]!=NULL; ii++)
    {
        try
        {
            int __ret;

            //iret = m_pDb->exec(psql_uniAppKey);

            __ret = m_pDb->exec(psql_uniAppKey_defaultValue[ii]);
            DBG(
                std::cout << "EXEC SQL:(" << __ret << ")" << psql_uniAppKey[ii] << std::endl;
            );

            ARG_USED(__ret);
        }
        //catch (std::exception& e)
        catch (SQLite::Exception& e)
        {
            DBG(
                std::cout << "error ini data:" << ii << std::endl;
                std::cout << "SQLite exception: " << e.what() << std::endl;
            );

            iret += 1000*(ii+1);
        }
    }// for

    return iret;
}

int CDbLiveStreamming::getAppSecret(const std::string &AppKey, std::string &AppSecret)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        SQLite::Statement   query(*m_pDb, "SELECT AppSecret FROM uniAppKey where AppKey="+AppKey);
        query.executeStep() ;

        // std::cout << "getColumnCount:" << query.getColumnCount () << std::endl;

        const char* aa = query.getColumn(0);
        AppSecret   = aa;

        // std::cout << "AppSecret:" << aa << std::endl;
    }
    catch (std::exception& e)
    {
        // std::cout << "SQLite exception: " << e.what() << std::endl;

        return -102; // unexpected error : exit the example program
    }

    return 0;
}
#if 0
//添加录像记录
int CDbLiveStreamming::addRtsp(const std::string &StreamID, const std::string &PlayUrl)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "INSERT INTO liveRtsp VALUES ("
        << "'" << StreamID.c_str() << "', "
        << "'" << PlayUrl.c_str() << "', "
        << 1 << ", " /* countReq */
        << time(NULL) << ", " /* dtReq */
        << time(NULL) << "" /* dtCreate */
        << "); "
        ;
        DBG(
            std::cout << "addRecRecord Sql:\n" << __ssSql.str() << std::endl;
        );
        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}
//删除录像记录
int CDbLiveStreamming::delRtsp(const std::string &StreamID)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM liveRtsp WHERE StreamID="
        << "'" << StreamID.c_str() << "'"
        << "; "
        ;

        std::cout << "delRecRecord Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );

        return -102; // unexpected error : exit the example program
    }

    return 0;

}
//修改录像记录
int CDbLiveStreamming::modRtsp(const std::string &StreamID, const int reqDt, const int reqCount)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __sssSql;

        __sssSql \
        << "UPDATE liveRtsp SET " \
        << "dtReq=" << reqDt << " ";

        if (reqCount>=0)
        {
            __sssSql \
            << ",countReq=" << reqCount << " ";
        }

        __sssSql \
        << "WHERE StreamID=" \
        << "'" << StreamID.c_str() << "'" \
        << "; " \
        ;

        DBG(
            __fline;
            std::cout << "\nStreamID:[" << StreamID << "]" << std::endl;
            __fline;
            std::cout << "\nmodRtsp Sql:[" << __sssSql.str() << "]" << std::endl;
        );

        /*int iret = */
        m_pDb->exec(__sssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}
//获取记录
int CDbLiveStreamming::getRtsp(const std::string &StreamID, int &FilePeriod, int &ValidPeriod)
{
    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//添加录像记录
int CDbLiveStreamming::addStream(const std::string &StreamType, const std::string &StreamID, const std::string &PlayUrl)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "INSERT INTO "<< StreamType <<" VALUES ("
        << "'" << StreamID.c_str() << "', "
        << "'" << PlayUrl.c_str() << "', "
        << 1 << ", " /* countReq */
        << time(NULL) << ", " /* dtReq */
        << time(NULL) << "" /* dtCreate */
        << "); "
        ;

        std::cout << "addRecRecord Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}
//删除录像记录
int CDbLiveStreamming::delStream(const std::string &StreamType, const std::string &StreamID)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM "<< StreamType <<" WHERE StreamID="
        << "'" << StreamID.c_str() << "'"
        << "; "
        ;

        std::cout << "delRecRecord Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );

        return -102; // unexpected error : exit the example program
    }

    return 0;

}

//修改录像记录
int CDbLiveStreamming::modStream(const std::string &StreamType, const std::string &StreamID, const int reqDt, const int reqCount)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __sssSql;

        __sssSql \
        << "UPDATE "<< StreamType <<" SET " \
        << "dtReq=" << reqDt << " ";

        if (reqCount>=0)
        {
            __sssSql \
            << ",countReq=" << reqCount << " ";
        }

        __sssSql \
        << "WHERE StreamID=" \
        << "'" << StreamID.c_str() << "'" \
        << "; " \
        ;

        DBG(
            __fline;
            std::cout << "\nStreamID:[" << StreamID << "]" << std::endl;
            __fline;
            std::cout << "\nmodRtsp Sql:[" << __sssSql.str() << "]" << std::endl;
        );

        /*int iret = */
        m_pDb->exec(__sssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}
//获取记录
int CDbLiveStreamming::getStream(const std::string &StreamType, const std::string &StreamID, int &FilePeriod, int &ValidPeriod)
{
    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//添加录像记录
int CDbLiveStreamming::addHls(const std::string &StreamID, const std::string &PlayUrl)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "INSERT INTO liveHls VALUES ("
        << "'" << StreamID.c_str() << "', "
        << "'" << PlayUrl.c_str() << "', "
        << 1 << ", " /* countReq */
        << time(NULL) << ", " /* dtReq */
        << time(NULL) << "" /* dtCreate */
        << "); "
        ;

        std::cout << "addRecRecord Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}
//删除录像记录
int CDbLiveStreamming::delHls(const std::string &StreamID)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM liveHls WHERE StreamID="
        << "'" << StreamID.c_str() << "'"
        << "; "
        ;

        std::cout << "delRecRecord Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );

        return -102; // unexpected error : exit the example program
    }

    return 0;

}
//修改录像记录
int CDbLiveStreamming::modHls(const std::string &StreamID, const int reqDt, const int reqCount)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __sssSql;

        __sssSql \
        << "UPDATE liveHls SET " \
        << "dtReq=" << reqDt << " " ;

        if (reqCount>=0)
        {
            __sssSql \
            << ",countReq=" << reqCount << " ";
        }

        __sssSql \
        << "WHERE StreamID=" \
        << "'" << StreamID.c_str() << "'" \
        << "; " \
        ;

        DBG(
            __fline;
            std::cout << "\nStreamID:[" << StreamID << "]" << std::endl;
            __fline;
            std::cout << "\nmodHls Sql:[" << __sssSql.str() << "]" << std::endl;
        );

        /*int iret = */
        m_pDb->exec(__sssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}
//获取记录
int CDbLiveStreamming::getHls(const std::string &StreamID, int &FilePeriod, int &ValidPeriod)
{
    return 0;
}
#endif //
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// ???ˇ·???
bool CDbLiveStreamming::isStreammingExist(const std::string &StreamID)
{
    if (!m_pDb)
    {
        return false;
    }

    try
    {
        SQLite::Statement   query(*m_pDb, "SELECT StreamID FROM liveStream where StreamID='"+StreamID+"'");
        query.executeStep() ;

        //std::cout << "getColumnCount:" << query.getColumnCount () << std::endl;

        const char* aa = query.getColumn(0);
        ARG_USED(aa);
        //AppSecret   = aa;

        //std::cout << "Result:" << aa << std::endl;

        // 不需要， 找不到会异常
        if (0)
            //if (StreamID == aa)
        {
            return true;
        }
    }
    catch (std::exception& e)
    {
        //std::cout << "SQLite exception: " << e.what() << std::endl;

        return false; // unexpected error : exit the example program
    }

    return true;
}

int CDbLiveStreamming::insertStreamming( \
        const std::string &StreamID
        , const std::string &strUrlHls
        , const std::string &strUrlRtmp
        , const std::string &strUrlRtsp
        , const int iCountReq
                                       )
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "INSERT INTO liveStream VALUES ("
        << "'" << StreamID.c_str() << "', "     /* StreamID */
        << "'" << strUrlHls.c_str() << "', "    /* UrlHls */
        << "'" << strUrlRtmp.c_str() << "', "   /* UrlRtmp */
        << "'" << strUrlRtsp.c_str() << "', "   /* UrlRtsp */
        << 1 << ", "                            /* countReq */
        << 0 << ", "                            /* ValidPeriod */
        << time(NULL) << ", "                     /* dtStart */
        << 0 << ", "                                   /* dtStop */
        << time(NULL) << ", "                     /* dtCreate */
        << time(NULL) << ""                     /* dtReq */
        << "); "
        ;
        DBG(
            std::cout << "Sql:\n" << __ssSql.str() << std::endl;
        );
        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}

// update
int CDbLiveStreamming::updateStreamming( \
        const std::string &StreamID
        , const std::string &strUrlHls
        , const std::string &strUrlRtmp
        , const std::string &strUrlRtsp
        , const int iCountReq
        , const time_t dtStart
        , const time_t dtStop
                                       )
{
    if (StreamID.length()<1)
    {
        return -1;
    }

    if (!m_pDb)
    {
        return -102;
    }

    try
    {
        std::stringstream __sssSql;

        __sssSql \
        << "UPDATE liveStream SET " \
        << "dtReq=" << time(NULL) << " " ;

        if (strUrlHls.length()>0)
        {
            __sssSql \
            << ", UrlHls='" << strUrlHls << "' ";
        }

        if (strUrlRtmp.length()>0)
        {
            __sssSql \
            << ", UrlRtmp='" << strUrlRtmp << "' ";
        }

        if (strUrlRtsp.length()>0)
        {
            __sssSql \
            << ", UrlRtsp='" << strUrlRtsp << "' ";
        }

        if (iCountReq>=0)
        {
            __sssSql \
            << ",countReq=" << iCountReq << " ";
        }
        //dtStart
        if (dtStart >= 0)
        {
            __sssSql \
            << ",dtStart=" << dtStart << " ";
        }

        //dtStop
        if (dtStop >= 0)
        {
            __sssSql \
            << ",dtStop=" << dtStop << " ";
        }

        __sssSql \
        << "WHERE StreamID=" \
        << "'" << StreamID.c_str() << "'" \
        << "; " \
        ;

        DBG(
            __fline;
            std::cout << "\nStreamID:[" << StreamID << "]" << std::endl;
            __fline;
            std::cout << "\n__sssSql:[" << __sssSql.str() << "]" << std::endl;
        );

        /*int iret = */
        m_pDb->exec(__sssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -102; // unexpected error : exit the example program
    }

    return 0;
}

// query
int CDbLiveStreamming::queryStreamming( \
                                        const std::string &StreamID
                                        , std::string &strUrlHls
                                        , std::string &strUrlRtmp
                                        , std::string &strUrlRtsp
                                        , int &iCountReq
                                        , time_t &dtStart
                                        , time_t &dtStop
                                      )
{
    return 0;
}

int CDbLiveStreamming::startStreamming( \
                                        const std::string &StreamID
                                        , const std::string &strUrlHls
                                        , const std::string &strUrlRtmp
                                        , const std::string &strUrlRtsp
                                        , const int iCountReq
                                      )
{
    int iret;
    bool bret;

    bret = isStreammingExist(StreamID);

    //
    if (!bret)
    {
        iret = insertStreamming(StreamID, strUrlHls, strUrlRtmp, strUrlRtsp, iCountReq);
    }
    else
    {
        //dtStop 设置为0
        iret = updateStreamming(StreamID, strUrlHls, strUrlRtmp, strUrlRtsp, iCountReq, time(NULL), 0);
    }

    return iret;
}

//
int CDbLiveStreamming::stopStreamming(const std::string &StreamID)
{
    int iret;

    //dtStart 设置为0
    iret = updateStreamming(StreamID, ""/*strUrlHls*/, ""/*strUrlRtmp*/, ""/*strUrlRtsp*/, 0/*iCountReq*/, 0/*dtStart*/, time(NULL));

    return iret;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/



// 工具 和 测试
#ifdef _FUNC_STANDALONE_UTIL
const char *pDbFileName_liveCast = \
                                   "liveStreamming.s3";

int main(int argc, char *argv[])
{
    std::cout << "SQlite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")" << std::endl;
    std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

    int iret;
    bool bret;

    ////////////////////////////////////////////////////////////////////////////
    CDbLiveStreamming dbLiveCast;
    iret = dbLiveCast.Open(pDbFileName_liveCast);

    std::cout << "initialize:" << iret << std::endl;
    if (iret<0)
    {
        std::cout << "Open failed:" << iret << std::endl;
        return -1;
    }

    std::string strSec;
    std::string strKey;

    strKey = "701818788";
    strSec = "";
    iret = dbLiveCast.getAppSecret(strKey, strSec);
    std::cout << "getAppSecret:" << iret << std::endl;
    std::cout << "strSec:" << strSec << std::endl;

    strKey = "701918788";
    strSec = "";
    iret = dbLiveCast.getAppSecret(strKey, strSec);
    std::cout << "getAppSecret, Key:"<< strKey <<" Ret:" << iret << std::endl;
    std::cout << "strSec:" << strSec << std::endl;

    strSec = "";
    iret = dbLiveCast.getAppSecret("701818788-", strSec);
    std::cout << "getAppSecret:" << iret << std::endl;
    assert(iret<0);
    //std::cout << "strSec:" << strSec << std::endl;

    char buf[64];
    char buf_mod[64];
    snprintf(buf, sizeof(buf), "%ld_T", time(NULL));
    strcpy(buf_mod, buf);

    //    iret = dbLiveCast.delRecRecord(/*"81105210485_0_1"*/buf);
    //    std::cout << "delRecRecord:" << iret << std::endl;
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // add
    std::string StreamID = "81155350053_0_1";
    std::string strUrlHls    ="http://abc.com";
    std::string strUrlRtmp   ="rtmp://abc.com";
    std::string strUrlRtsp   ="rtsp://abc.com";
    int iCountReq            = 1;
    //    time_t dtStart = time(NULL);
    //    time_t dtStop = time(NULL);

    // 准备数据
    // 先检查是否有记录
    bret = dbLiveCast.isStreammingExist(StreamID);
    if (bret==false)
    {
        iret = dbLiveCast.startStreamming(StreamID, strUrlHls, strUrlRtmp, strUrlRtsp, iCountReq);
        //std::cout << "newStreamming:" << iret << std::endl;
    }

    bret = dbLiveCast.isStreammingExist(StreamID);
    if(false==bret)
    {
        std::cout << "ERROR isStreammingExist("<< StreamID << "):" << bret << std::endl;
    }

    iret = dbLiveCast.startStreamming(StreamID, strUrlHls, strUrlRtmp, strUrlRtsp, iCountReq);
    if (0 != iret)
    {
        std::cout << "ERROR startStreamming:"<< iret << std::endl;
    }

    sleep(1);

    iret = dbLiveCast.stopStreamming(StreamID);
    if (0 != iret)
    {
        std::cout << "ERROR stopStreamming:"<< iret << std::endl;
    }

#if 0
    StreamID = "81155350053_0_1";
    strUrlHls    ="http://mod.com";
    strUrlRtmp   ="rtmp://mod.com";
    strUrlRtsp   ="rtsp://mod.com";
    iCountReq            = 10;
    dtStart = 88888;
    dtStop = 99999;

    iret = dbLiveCast.updateStreamming(StreamID, strUrlHls, strUrlRtmp, strUrlRtsp, iCountReq, dtStart, dtStop);
    if (0 != iret)
    {
        std::cout << "ERROR updateStreamming" << std::endl;
    }
#endif

    std::cout << "everything ok, quitting." << std::endl;

    return 0;
}
#endif // _FUNC_STANDALONE_UTIL
