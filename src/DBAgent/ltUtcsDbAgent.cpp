/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ltUtcsDbAgent.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ltUtcsDbAgent.cpp 5884 2014-10-16 01:22:07Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-16 01:22:07  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <strings.h>
#include "../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../Configs/ConfigDatabase.h"
#endif //_USE_MODULES_EZCONFIG

#include "ltUtcsDbAgent.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
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

#define ARG_USED(x) (void)&x;

PATTERN_SINGLETON_IMPLEMENT(CltUtcsDbAgent);

CltUtcsDbAgent::CltUtcsDbAgent():CEZThread("CltUtcsDbAgent", THREAD_PRIORITY_DEFAULT)
{
    m_bReadyUse = false;

    m_pOcciEnv       = NULL;
    m_pOcciConn      = NULL;
    m_pOcciStmt      = NULL;
    m_pOcciResultSet = NULL;
}

CltUtcsDbAgent::~CltUtcsDbAgent()
{
    close();
}

bool CltUtcsDbAgent::open()
{
    if (true == m_bReadyUse)
    {
        return true;
    }

    EZ_LOGS(
        ez_log("%s(%d) -- CltUtcsDbAgent::open \n", __FILE__, __LINE__);
    );


    EZ_LOGS(
        ez_log("%s(%d) -- db para:[%s],[%s],[%s]\n", __FILE__, __LINE__
               , m_strUser.c_str()
               , m_strPassword.c_str()
               , m_strConStr.c_str() );
    );
    // 默认参数
    //return open("utcs1", "ltgsadmin", "www.faceaip.com/ORCL");
    return open(
               m_strUser     ,
               m_strPassword ,
               m_strConStr   );
}

//
// Desc:
//   Open connection to Oracle database.
// Return:
//   true  -- Successful.
//   false -- Failed.
//
bool CltUtcsDbAgent::open(std::string name, std::string pass, std::string dbstr)
{
    // 内部维护， 无需外锁
    //    CEZLock __lock(m_MutexOcciOpr);
#if 0
    if (true == m_bReadyUse)
    {
        return true;
    }
#endif
    //m_strUser = name;
    //m_strPassword = pass;
    //m_strConStr = dbstr;

    try
    {
        close();

        //m_pOcciEnv = Environment::createEnvironment(Environment::OBJECT);
        //m_pOcciEnv = Environment::createEnvironment("ZHS16GBK","AL32UTF8");
        m_pOcciEnv = Environment::createEnvironment("AL32UTF8","AL32UTF8"); // 解决了中文问题, 2014-10-24 16:11:56
        m_pOcciConn = m_pOcciEnv->createConnection(m_strUser, m_strPassword, m_strConStr);
        m_pOcciStmt = m_pOcciConn->createStatement();

        m_bReadyUse = true;
        EZ_LOGS(
            ez_log("%s(%d) -- createConnection succeeded.\n", __FILE__, __LINE__);
        );
    }
    catch (SQLException ex)
    {
        cout<<"Exception: Code - "<<ex.getErrorCode()<<", Message - "<<ex.getMessage();
        EZ_LOGS(
            ez_log("%s(%d) -- Exception: Code：%d， Message:%s.\n", __FILE__, __LINE__
                   , ex.getErrorCode() \
                   , ex.getMessage().c_str());
        );
        return false;
    }
    return true;
}

//
// Desc:
//   Close connection to Oracle database.
// Return:
//   true  -- Successful.
//   false -- Failed.
//
bool CltUtcsDbAgent::close()
{
    //CEZLock __lock(m_MutexOcciOpr);
    //if (false == m_bReadyUse)
    //{
    //    return true;
    //}

    try
    {
        if (m_pOcciResultSet)
        {
            closeResultSet();
        }

        if(m_pOcciConn && m_pOcciStmt)
        {
            m_pOcciConn->terminateStatement (m_pOcciStmt);
        }

        if(m_pOcciEnv && m_pOcciConn)
        {
            m_pOcciEnv->terminateConnection(m_pOcciConn);
        }

        if(m_pOcciEnv)
        {
            Environment::terminateEnvironment(m_pOcciEnv);
        }

        m_bReadyUse = false;

        m_pOcciEnv       = NULL;
        m_pOcciConn      = NULL;
        m_pOcciStmt      = NULL;
        m_pOcciResultSet = NULL;
    }
    catch (SQLException ex)
    {
        cout<<"Exception: Code - "<<ex.getErrorCode()<<", Message - "<<ex.getMessage();
        return false;
    }

    return true;
}

//
// Desc:
//   commit the operation.
// Return:
//   true  -- Successful.
//   false -- Failed.
//
bool CltUtcsDbAgent::commit()
{
    //CEZLock __lock(m_MutexOcciOpr);
    try
    {
        m_pOcciConn->commit();
    }
    catch (SQLException ex)
    {
        cout<<"Exception: Code - "<<ex.getErrorCode()<<", Message - "<<ex.getMessage();
        return false;
    }
    return true;
}

void CltUtcsDbAgent::setSQL(string oneSQL)
{
    m_strSQL = oneSQL;
}

bool CltUtcsDbAgent::execBatchedUpdate()
{
    try
    {
        m_pOcciStmt->executeUpdate();
    }
    catch (SQLException ex)
    {
        cout<<"Exception: Code - "<<ex.getErrorCode()<<", Message - "<<ex.getMessage();
        return false;
    }
    return true;
}

bool CltUtcsDbAgent::execUpdateSQL()
{
    try
    {
        m_pOcciStmt->setSQL(m_strSQL);
        m_pOcciStmt->executeUpdate();
    }
    catch (SQLException ex)
    {
        cout<<"Exception: Code - "<<ex.getErrorCode()<<", Message - "<<ex.getMessage();
        return false;
    }
    return true;
}

bool CltUtcsDbAgent::execQuerySQL()
{
    try
    {
        m_pOcciStmt->setSQL(m_strSQL);
        m_pOcciResultSet = m_pOcciStmt->executeQuery();
    }
    catch (SQLException ex)
    {
        cout<<"Exception: Code - "<<ex.getErrorCode()<<", Message - "<<ex.getMessage();
        return false;
    }
    return true;
}

bool CltUtcsDbAgent::getReady()
{
    return m_bReadyUse;
}

ResultSet* CltUtcsDbAgent::getResultSet()
{
    return m_pOcciResultSet;
}

bool CltUtcsDbAgent::closeResultSet()
{
    m_pOcciStmt->closeResultSet (m_pOcciResultSet);
    m_pOcciResultSet = NULL;

    return true;
}

Statement* CltUtcsDbAgent::getStatement()
{
    return m_pOcciStmt;
}

//
// Desc:
//   execute SQL string.
// Parameters:
//   string sqlStr -- SQL string
//   string op -- it has only four value.
//                insert, update, delete, select
//
bool CltUtcsDbAgent::execSQL(string sqlStr)
{
if (!getReady())
{return false;}

    CEZLock __lock(m_MutexOcciOpr);

    bool bSelectOp = false;

    if (sqlStr.size() <= 6/*strlen("insert")*/)
    {
        cout<<"SQL sqlStr not valid " << endl;
        return false;
    }
    else
    {
        if (strncasecmp("select", sqlStr.c_str(), 6) == 0 )
        {
            bSelectOp = true;
        }
    }

    if (m_pOcciResultSet)
    {
        closeResultSet();
    }

    m_strSQL = sqlStr;
    if(bSelectOp)
    {
        return execQuerySQL();
    }
    else
    {
        return execUpdateSQL();
    }
}

EZTHREAD_BOOL CltUtcsDbAgent::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CltUtcsDbAgent::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

void CltUtcsDbAgent::ThreadProc()
{
    bool __bUseMe = true;

#ifdef _USE_MODULES_EZCONFIG

    CConfigDatabase __cfg;
    __cfg.update();

    m_strUser     = __cfg.getConfig().strName;
    m_strPassword = __cfg.getConfig().strPasswd;
    m_strConStr   = __cfg.getConfig().strDBString;

    __bUseMe = (__cfg.getConfig().iEnable==1);

    EZ_LOGS(
        ez_log("%s(%d) -- Use CConfigDatabase, __bUseMe:%d\n", __FILE__, __LINE__, __bUseMe);
    );

#else

    m_strUser = "utcs1";
    m_strPassword = "ltgsadmin";
    m_strConStr = "192.168.5.146:1521/ORCL";
    EZ_LOGS(
        ez_log("%s(%d) -- Use Default para\n", __FILE__, __LINE__);
    );

#endif

    while (__bUseMe
           && m_bLoop)
    {
        // __trip;
        open();

        sleep(1);
    }

    // 自行关闭
    close();
}
