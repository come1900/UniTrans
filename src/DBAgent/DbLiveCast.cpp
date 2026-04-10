/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbLiveCast.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbLiveCast.cpp 5884 2017-04-25 02:24:33Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-25 02:24:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>

#include "DbLiveCast.h"

#ifndef _DEBUG_THIS
// #define _DEBUG_THIS
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

//
static const char *psql_uniAppKey = \
                                    /*1.*/ "CREATE TABLE IF NOT EXISTS uniAppKey("  \
                                    /*  */ "AppKey         varchar(16)     NOT NULL UNIQUE," \
                                    /*  */ "AppSecret      varchar(64)     NOT NULL," \
                                    /*  */ "HUID           varchar(16)             ,"
                                    /*  */ "dtCreate       datetime        DEFAULT 1490090786" \
                                    /*  */ "); " \
                                    /*2.*/ "CREATE TABLE IF NOT EXISTS liveCast("   \
                                    /*  */ "StreamID       varchar(256)     NOT NULL UNIQUE   ," \
                                    /*  */ "PushUrlRtmp    varchar(1024)                      ," \
                                    /*  */ "PushUrlRtsp    varchar(1024)                      ," \
                                    /*  */ "PlayUrlRtsp    varchar(1024)                      ," \
                                    /*  */ "PlayUrlRtmp    varchar(1024)                      ," \
                                    /*  */ "PlayUrlHls     varchar(1024)                      ," \
                                    /*  */ "countReq       INT                      ," 			\
                                    /*  */ "dtCreate       datetime        DEFAULT 1490090786 ," 	\
                                    /*  */ "dtReq          datetime        DEFAULT 1490090786  " 	\
                                    /*  */ "); " \
                                    ;

static const char *psql_uniAppKey_defaultValue = \
        /*  */ "INSERT INTO uniAppKey VALUES (\"701818788\", \"c48faffaedf5d4cae01c6a21c11476fc118e32b0\", \"974620270\", 1490090786); " \
        /*  */ "INSERT INTO uniAppKey VALUES (\"701918788\", \"9cba8e3fe970ddbca35424db80197a0167d68813\", \"043979497\", 1490090786); " \
        ;


CDbLiveCast::CDbLiveCast()
{
    m_pDb = NULL;
    m_pQuery = NULL;
}

CDbLiveCast::~CDbLiveCast()
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

int CDbLiveCast::Open(const std::string strConn)
{
    try
    {
        if (NULL != m_pDb)
        {
            delete m_pDb;
        }

        m_pDb = new SQLite::Database(strConn, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);

        //        iret = m_pDb->exec(psql_uniAppKey);
        //        // std::cout << "exec psql_uniAppKey: " << iret << std::endl;
        //
        //        iret = m_pDb->exec(psql_uniAppKey_defaultValue);
        //        // std::cout << "exec psql_uniAppKey_defaultValue: " << iret << std::endl;

        //ARG_USED(iret);
    }
    //catch (std::exception& e)
    catch (SQLite::Exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        //std::cout << "SQLite exception: " << e.getErrorCode()  << std::endl;

        return -1; // unexpected error : exit the example program
    }

    // 初始化数据
    try
    {
        int iret;

        iret = m_pDb->exec(psql_uniAppKey);
        // std::cout << "exec psql_uniAppKey: " << iret << std::endl;

        iret = m_pDb->exec(psql_uniAppKey_defaultValue);
        // std::cout << "exec psql_uniAppKey_defaultValue: " << iret << std::endl;

        ARG_USED(iret);
    }
    //catch (std::exception& e)
    catch (SQLite::Exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        //std::cout << "SQLite exception: " << e.getErrorCode()  << std::endl;

        return 2; // unexpected error : exit the example program
    }

    return 0;
}

int CDbLiveCast::getAppSecret(const std::string &AppKey, std::string &AppSecret)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        //iret = m_pDb->exec(psql_uniAppKey);
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

        return -2; // unexpected error : exit the example program
    }

    return 0;
}

//添加录像记录
int CDbLiveCast::addCast(const std::string &StreamID
                         , const std::string &PushUrlRtmp
                         , const std::string &PushUrlRtsp
                         , const std::string &PlayUrlRtsp
                         , const std::string &PlayUrlRtmp
                         , const std::string &PlayUrlHls
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
        << "INSERT INTO liveCast VALUES ("
        /*StreamID*/ << "'" << StreamID.c_str() << "', "
        /*        */ << "'" << PushUrlRtmp << "', "
        /*        */ << "'" << PushUrlRtsp << "', "
        /*        */ << "'" << PlayUrlRtsp << "', "
        /*        */ << "'" << PlayUrlRtmp << "', "
        /*        */ << "'" << PlayUrlHls  << "', "
        /*        */ << 1 << ", " /* countReq */
        /*        */ << time(NULL) << ", " /* dtReq */
        /*        */ << time(NULL) << "" /* dtCreate */
        << "); "
        ;

        std::cout << __FUNCTION__ << " Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );
        return -2; // unexpected error : exit the example program
    }

    return 0;
}
//删除录像记录
int CDbLiveCast::delCast(const std::string &StreamID)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM liveCast WHERE StreamID="
        << "'" << StreamID.c_str() << "'"
        << "; "
        ;

        std::cout << __FUNCTION__ << " Sql:\n" << __ssSql.str() << std::endl;

        /*int iret = */
        m_pDb->exec(__ssSql.str());
    }
    catch (std::exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );

        return -2; // unexpected error : exit the example program
    }

    return 0;

}
//修改录像记录
int CDbLiveCast::modCast(const std::string &StreamID, const int reqDt, const int reqCount)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __sssSql;

        __sssSql \
        << "UPDATE liveCast SET " \
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
        return -2; // unexpected error : exit the example program
    }

    return 0;
}
//获取记录
int CDbLiveCast::getCast(const std::string &StreamID, int &FilePeriod, int &ValidPeriod)
{
    return 0;
}

// 工具 和 测试
#ifdef _FUNC_STANDALONE_UTIL
static const char *pDbFileName_liveCast = \
        "liveCast.s3";

int main(int argc, char *argv[])
{
    std::cout << "SQlite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")" << std::endl;
    std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

    int iret;

    ////////////////////////////////////////////////////////////////////////////
    CDbLiveCast dbLiveCast;
    iret = dbLiveCast.Open(pDbFileName_liveCast);

    std::cout << "initialize:" << iret << std::endl;
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
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    char buf[64];
    snprintf(buf, sizeof(buf), "%ld_T", time(NULL));
    // add
    iret = dbLiveCast.addCast(/*"81105210485_0_1"*/buf, "a", "b", "c", "d", "e");
    std::cout << "addCast:" << iret << std::endl;
    iret = dbLiveCast.modCast(buf, 1493276888);
    std::cout << "modCast:" << iret << std::endl;

    iret = dbLiveCast.delCast(/*"81105210485_0_1"*/buf);
    std::cout << "delCast:" << iret << std::endl;
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    std::cout << "everything ok, quitting\n";

    return 0;
}
#endif //_FUNC_STANDALONE_UTIL
