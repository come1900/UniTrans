/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbLiveRecord.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbLiveRecord.h 5884 2017-04-26 05:22:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-04-26 05:22:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <SQLiteCpp.h>
#include <VariadicBind.h>

//#include "DbDriver.h"
#if !defined(_DbLiveRecord_H)
#define _DbLiveRecord_H

class CDbLiveRecord//:public CDbDriver
{
public:
    // Constructor
    CDbLiveRecord();
    virtual ~CDbLiveRecord();
    
    virtual int Open(const std::string strConn);
   // ²éÕÒapp sec
   int getAppSecret(const std::string &AppKey, std::string &AppSecret);

   //Ìí¼ÓÂ¼Ïñ¼ÇÂ¼
   int addRecRecord(const std::string &StreamID, const int &FilePeriod, const int &ValidPeriod);
   //É¾³ýÂ¼Ïñ¼ÇÂ¼
   int delRecRecord(const std::string &StreamID);
   //ÐÞ¸ÄÂ¼Ïñ¼ÇÂ¼
   int modRecRecord(const std::string &StreamID, const int reqDt);
   //»ñÈ¡Â¼Ïñ¼ÇÂ¼
   int getRecRecord(const std::string &StreamID, int &FilePeriod, int &ValidPeriod);

private:
    SQLite::Database    *m_pDb;    ///< Database connection
    SQLite::Statement   *m_pQuery; ///< Database prepared SQL query
};

#endif // !defined(_DbLiveRecord_H)