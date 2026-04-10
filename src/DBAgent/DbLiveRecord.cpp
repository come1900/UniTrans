/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbLiveRecord.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbLiveRecord.cpp 5884 2017-04-26 05:22:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-26 05:22:22  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "DbLiveRecord.h"

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

//
static const char *psql_defalutTables = \
                             /*1.*/ "CREATE TABLE IF NOT EXISTS uniAppKey(" 				\
                             /*  */ "AppKey         varchar(16)     NOT NULL UNIQUE," 		\
                             /*  */ "AppSecret      varchar(64)     NOT NULL," 				\
                             /*  */ "HUID           varchar(16)             ," 				\
                             /*  */ "dtCreate       datetime        DEFAULT 1490090786" 	\
                             /*  */ "); " 													\
                             /*2.*/ "CREATE TABLE IF NOT EXISTS liveRecord("   				\
                             /*  */ "StreamID       varchar(256)     NOT NULL UNIQUE   ," 	\
                             /*  */ "FilePeriod     INT                      ," 			\
                             /*  */ "ValidPeriod    INT                      ," 			\
                             /*  */ "countReq       INT                      ," 			\
                             /*  */ "dtCreate       datetime        DEFAULT 1490090786 ," 	\
                             /*  */ "dtReq          datetime        DEFAULT 1490090786  " 	\
                             /*  */ "); " 													\
                             ;

static const char *psql_defaultValues = \
        /*  */ "INSERT INTO uniAppKey VALUES (\"701818788\", \"c48faffaedf5d4cae01c6a21c11476fc118e32b0\", \"974620270\", 1490090786); " \
        /*  */ "INSERT INTO uniAppKey VALUES (\"701918788\", \"9cba8e3fe970ddbca35424db80197a0167d68813\", \"043979497\", 1490090786); " \
        ;

CDbLiveRecord::CDbLiveRecord()
{
    m_pDb = NULL;
    m_pQuery = NULL;
}
CDbLiveRecord::~CDbLiveRecord()
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

int CDbLiveRecord::Open(const std::string strConn)
{
    try
    {
        if (NULL != m_pDb)
        {
            delete m_pDb;
        }

        m_pDb = new SQLite::Database(strConn, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);

    }
    catch (SQLite::Exception& e)
    {
        DBG(
            std::cout << "SQLite exception: " << e.what() << std::endl;
        );

        return -1; // unexpected error : exit the example program
    }

    // Default tables and values
    try
    {
        int iret;

        iret = m_pDb->exec(psql_defalutTables);
        // std::cout << "exec psql_defalutTables: " << iret << std::endl;

        iret = m_pDb->exec(psql_defaultValues);
        // std::cout << "exec psql_defaultValues: " << iret << std::endl;

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

int CDbLiveRecord::getAppSecret(const std::string &AppKey, std::string &AppSecret)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        //iret = m_pDb->exec(psql_defalutTables);
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

//
int CDbLiveRecord::addRecRecord(const std::string &StreamID, const int &FilePeriod, const int &ValidPeriod)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "INSERT INTO liveRecord VALUES ("
        << "'" << StreamID.c_str() << "', "
        << FilePeriod << ", "
        << ValidPeriod << ", "
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
        return -2; // unexpected error : exit the example program
    }

    return 0;
}
//
int CDbLiveRecord::delRecRecord(const std::string &StreamID)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM liveRecord WHERE StreamID="
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

        return -2; // unexpected error : exit the example program
    }

    return 0;

}
//
int CDbLiveRecord::modRecRecord(const std::string &StreamID, const int reqDt)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "UPDATE liveRecord SET " \
        << "dtReq=" << reqDt << " " \
        << "WHERE StreamID=" \
        << "'" << StreamID.c_str() << "'" \
        << "; " \
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
        return -2; // unexpected error : exit the example program
    }

    return 0;
}
//
int CDbLiveRecord::getRecRecord(const std::string &StreamID, int &FilePeriod, int &ValidPeriod)
{
    return 0;
}

// for test
#ifdef _FUNC_STANDALONE_UTIL
const char *pDbFileName_liveCast = \
                                   "liveRecord.s3";

int main(int argc, char *argv[])
{
    std::cout << "SQlite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")" << std::endl;
    std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

    int iret;

    ////////////////////////////////////////////////////////////////////////////
    CDbLiveRecord dbLiveCast;
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
    snprintf(buf, sizeof(buf), "%ld_T", time(NULL));
    // add
    iret = dbLiveCast.addRecRecord(/*"81105210485_0_1"*/buf, 300, 3600);
    std::cout << "addRecRecord:" << iret << std::endl;
    iret = dbLiveCast.modRecRecord(buf, 1493276888);
    std::cout << "modRecRecord:" << iret << std::endl;

    //    iret = dbLiveCast.delRecRecord(/*"81105210485_0_1"*/buf);
    //    std::cout << "delRecRecord:" << iret << std::endl;

    std::cout << "everything ok, quitting\n";

    return 0;
}
#endif //_FUNC_STANDALONE_UTIL

