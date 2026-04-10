/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ltUtcsDbAgent.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ltUtcsDbAgent.h 5884 2014-10-16 01:22:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-16 01:22:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_ltUtcsDbAgent_H)
#define _ltUtcsDbAgent_H

#include <iostream>

#define LINUXOCCI
#include <occi.h>

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

using namespace std;
using namespace oracle::occi;

#define g_ltUtcsDbAgent  (*CltUtcsDbAgent::instance())

class CltUtcsDbAgent:public CEZThread//CEZThread("CltUtcsDbAgent", TP_DEFAULT)// : public CEZObject
{
public:
    PATTERN_SINGLETON_DECLARE(CltUtcsDbAgent);

    CltUtcsDbAgent();
    virtual ~CltUtcsDbAgent();

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

    bool execSQL(string m_strSQL); /* execute one SQL string */
    Statement* getStatement();
    ResultSet* getResultSet();

    bool getReady(); /* 是否可用 */

private:
    bool open();
    bool close();
    bool open(std::string name, std::string pass, std::string dbstr);
    void setSQL(string oneSQL);
    bool execSQL(string sqlStr, string op);
    bool execQuerySQL();
    bool execUpdateSQL();
    bool execBatchedUpdate();
    bool commit();
    bool closeResultSet();

    Environment *m_pOcciEnv; /* in $ORACLE_HOME/rdbms/public/occiControl.h */
    Connection *m_pOcciConn;
    Statement *m_pOcciStmt;
    ResultSet *m_pOcciResultSet;

    string m_strSQL;

    string m_strUser;
    string m_strPassword;
    string m_strConStr;

    // 是否处于待用状态
    bool m_bReadyUse;

    CEZMutex m_MutexOcciOpr;
};

#endif // !defined(_ltUtcsDbAgent_H)
