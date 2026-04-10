/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbPandoraMatrix.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbPandoraMatrix.h 5884 2018-04-22 08:47:10Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-04-22 08:47:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <SQLiteCpp.h>
#include <VariadicBind.h>

#if !defined(_DbPandoraMatrix_H)
#define _DbPandoraMatrix_H

class CDbPandoraMatrix//:public CDbDriver
{
public:
    // Constructor
    CDbPandoraMatrix();
    virtual ~CDbPandoraMatrix();
    
    virtual int Open(const std::string strConn);
   // ≤È’“app sec
   int getAppSecret(const std::string &AppKey, std::string &AppSecret);

private:
    SQLite::Database    *m_pDb;    ///< Database connection
    SQLite::Statement   *m_pQuery; ///< Database prepared SQL query
};

#endif // !defined(_DbPandoraMatrix_H)