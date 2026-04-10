/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbKeyValue.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbKeyValue.cpp 5884 2018-05-20 03:39:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-05-20 03:39:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

#include "DbKeyValue.h"

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
                                        /*1.*/ "CREATE TABLE IF NOT EXISTS KeyValue("                  \
                                        /*  */ "Key          varchar(%d)     NOT NULL UNIQUE,"      \
                                        /*  */ "Value        varchar(%d)     NOT NULL,"             \
                                        /*  */ "dtCreate       datetime        DEFAULT 1490090786,"    \
                                        /*  */ "dtModify       datetime        DEFAULT 1490090786"     \
                                        /*  */ "); "                                                   \
                                        ;
static const char *psql_defalutTablesNotUniqueKey = \
        /*1.*/ "CREATE TABLE IF NOT EXISTS KeyValue("                  \
        /*  */ "Key          varchar(%d)     NOT NULL,"      \
        /*  */ "Value        varchar(%d)     NOT NULL,"             \
        /*  */ "dtCreate       datetime        DEFAULT 1490090787,"    \
        /*  */ "dtModify       datetime        DEFAULT 1490090787"     \
        /*  */ "); "                                                   \
        ;

static const char *psql_defaultValues = \
                                        /*  */ "INSERT INTO KeyValue VALUES (\"701818788\", \"c48faffaedf5d4cae01c6a21c11476fc118e32b0\", 1490090786, 1490090786); " \
                                        /*  */ "INSERT INTO KeyValue VALUES (\"701918788\", \"9cba8e3fe970ddbca35424db80197a0167d68813\", 1490090786, 1490090786); " \
                                        ;

CDbKeyValue::CDbKeyValue()
{
    m_pDb = NULL;
    m_pQuery = NULL;

    m_iKeyLen = def_defaultKeyLen;
    m_iValueLen = def_defaultValueLen;
}
CDbKeyValue::~CDbKeyValue()
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

// should , not must ...
int CDbKeyValue::setKeyLen(int keyLen)
{
    if (keyLen > def_maxKeyLen
        || keyLen < 1)
    {
        return -1;
    }

    m_iKeyLen = keyLen;

    return 0;
}

int CDbKeyValue::setValueLen(int valueLen)
{
    if (valueLen > def_maxValueLen
        || valueLen < 1)
    {
        return -1;
    }

    m_iValueLen = valueLen;

    return 0;
}

int CDbKeyValue::Open(const std::string strConn, const bool bKeyUnique)
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

    //char tablesSql[sizeof(psql_defalutTables)+32] = {'\0'};
    char tablesSql[1024] = {'\0'};

    if (bKeyUnique)
    {
        snprintf(tablesSql, sizeof (tablesSql)-1, psql_defalutTables, m_iKeyLen, m_iValueLen);
    }
    else
    {
        snprintf(tablesSql, sizeof (tablesSql)-1, psql_defalutTablesNotUniqueKey, m_iKeyLen, m_iValueLen);
    }

    // Default tables and values
    try
    {
        int iret;
        iret = m_pDb->exec(tablesSql);

        DBG(
            std::cout << "exec psql_defalutTables: " << iret << std::endl;
        );
        // 不插默认值
#if 0

        iret = m_pDb->exec(psql_defaultValues);
        std::cout << "exec psql_defaultValues: " << iret << std::endl;
#endif

        ARG_USED(iret);
        ARG_USED(psql_defaultValues);
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
//
int CDbKeyValue::addValue(const std::string &Key, const std::string &Value)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "INSERT INTO KeyValue VALUES ("
        << "'" << Key << "', "
        << "'" << Value << "', "
        << time(NULL) << ", " /* dtCreate */
        << time(NULL) << "" /* dtModify  */
        << "); "
        ;

        DBG(
            std::cout << "addValue Sql:\n" << __ssSql.str() << std::endl;
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
int CDbKeyValue::delValue(const std::string &Key, int iMode)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM KeyValue WHERE Key="
        << "'" << Key << "'"
        << "; "
        ;

        DBG(
            std::cout << "delValue Sql:\n" << __ssSql.str() << std::endl;
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
int CDbKeyValue::modValue(const std::string &Key, const std::string &Value)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "UPDATE KeyValue SET " \
        << "Value='" << Value<< "', " \
        << "dtModify=" << time(NULL) << " " /* dtModify  */
        << "WHERE Key=" \
        << "'" << Key << "'" \
        << "; " \
        ;

        DBG(
            std::cout << "modValue Sql:\n" << __ssSql.str() << std::endl;
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

int CDbKeyValue::getValue(const std::string &Key, std::string &Value)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::string strSql = "SELECT Value FROM KeyValue where Key='" + Key + "'";

        DBG(
            std::cout << "getValue Sql:\n" << strSql << std::endl;
        );
        //iret = m_pDb->exec(psql_defalutTables);
        SQLite::Statement   query(*m_pDb, strSql.c_str());
        if (query.executeStep() )
        {
            DBG(
                std::cout << "getColumnCount:" << query.getColumnCount () << std::endl;
            );
            const char* aa = query.getColumn(0);
            Value   = aa;
        }
        else
        {
            return -2;
        }
        // std::cout << "Value:" << aa << std::endl;
    }
    catch (std::exception& e)
    {
        // std::cout << "SQLite exception: " << e.what() << std::endl;

        return -3; // unexpected error : exit the example program
    }

    return 0;
}


int CDbKeyValue::getValue(def_uniKeyValue &valBuff)
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::string strSql = "SELECT Key,Value FROM KeyValue";

        DBG(
            std::cout << "getValue Sql:\n" << strSql << std::endl;
        );
        //iret = m_pDb->exec(psql_defalutTables);
        SQLite::Statement   query(*m_pDb, strSql.c_str());

        while (query.executeStep())
        {

            DBG(
                std::cout << "getColumnCount:" << query.getColumnCount () << std::endl;
            );

            std::pair< def_uniKeyValue::const_iterator, bool > ret;
            ret = valBuff.insert ( std::pair<std::string, std::string>(query.getColumn(0), query.getColumn(1)) );
            ARG_USED(ret);

        }
        // std::cout << "Value:" << aa << std::endl;
    }
    catch (std::exception& e)
    {
        // std::cout << "SQLite exception: " << e.what() << std::endl;

        return -2; // unexpected error : exit the example program
    }

    return 0;
}

//
int CDbKeyValue::clearAll()
{
    if (!m_pDb)
    {
        return -1;
    }

    try
    {
        std::stringstream __ssSql;

        __ssSql \
        << "DELETE FROM KeyValue"
        ;
        DBG(
            std::cout << "delValue Sql:\n" << __ssSql.str() << std::endl;
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

// for test
#ifdef _FUNC_STANDALONE_UTIL
const char *pDbFileName_liveCast = \
                                   "keyValues.s3";

int main(int argc, char *argv[])
{
    std::cout << "SQlite3 version " << SQLite::VERSION << " (" << SQLite::getLibVersion() << ")" << std::endl;
    std::cout << "SQliteC++ version " << SQLITECPP_VERSION << std::endl;

    int iret;

    ////////////////////////////////////////////////////////////////////////////
    CDbKeyValue __dbKeyValue;
    iret = __dbKeyValue.Open(pDbFileName_liveCast);

    std::cout << "initialize:" << iret << std::endl;
    if (iret<0)
    {
        std::cout << "Open failed:" << iret << std::endl;
        return -1;
    }

    std::string strVal;
    std::string strKey;

    // 初始化数据
    {
        iret = __dbKeyValue.clearAll();
        assert(iret==0);

        strKey = "701818788";
        strVal = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
        iret = __dbKeyValue.addValue(strKey, strVal);
        assert(iret==0);

        strKey = "701918788";
        strVal = "9cba8e3fe970ddbca35424db80197a0167d68813";
        iret = __dbKeyValue.addValue(strKey, strVal);
        assert(iret==0);
    }

    strKey = "701818788";
    strVal = "";
    iret = __dbKeyValue.getValue(strKey, strVal);
    std::cout << "getValue:" << iret << std::endl;
    std::cout << "strVal:" << strVal << std::endl;
    assert(iret==0);

    strKey = "701918788";
    strVal = "";
    iret = __dbKeyValue.getValue(strKey, strVal);
    std::cout << "getValue, Key:"<< strKey <<" Ret:" << iret << std::endl;
    std::cout << "strVal:" << strVal << std::endl;
    assert(iret==0);

    strVal = "";
    iret = __dbKeyValue.getValue("701818788-", strVal);
    std::cout << "getValue:" << iret << std::endl;
    assert(iret<0);
    //std::cout << "strVal:" << strVal << std::endl;

    def_uniKeyValue valBuff;
    iret = __dbKeyValue.getValue(valBuff);
    std::cout << "getValue:" << iret << std::endl;
    //assert(iret<0);
    std::cout << "valBuff.size:" << valBuff.size() << std::endl;

    {
        //add
        strKey = "good";
        strVal = "baby";

        iret = __dbKeyValue.addValue(strKey, strVal);
        assert(iret==0);

        std::string strValGetted="";
        iret = __dbKeyValue.getValue(strKey, strValGetted);
        assert(iret==0);
        assert(strValGetted==strVal);
        //mod
        //strKey = "good";
        strVal = "boy";
        iret = __dbKeyValue.modValue(strKey, strVal);
        assert(iret==0);

        iret = __dbKeyValue.getValue(strKey, strValGetted);
        assert(iret==0);
        assert(strValGetted==strVal);

        //del
        //strKey = "good";
        strVal = "boy";
        iret = __dbKeyValue.delValue(strKey, 0);
        assert(iret==0);

        iret = __dbKeyValue.getValue(strKey, strValGetted);
        assert(iret==-2);
    }

    std::cout << "everything ok, quitting\n";

    return 0;
}
#endif //_FUNC_STANDALONE_UTIL

