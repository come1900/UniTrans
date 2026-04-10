/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DbKeyValue.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DbKeyValue.h 5884 2018-05-20 03:39:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-05-20 03:39:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <SQLiteCpp.h>
#include <VariadicBind.h>

#include "defKeyValue.h"

// 持久化的数据库文件名, 保持唯一， 统一在此定义
#define def_dbFileKeyValue_PullerStream2Rtsp "keyValue_PullerStream2Rtsp.s3"
#define def_dbFileKeyValue_PullerStream2Rtmp "keyValue_PullerStream2Rtmp.s3"
#define def_dbFileKeyValue_RecordMediaUrl "keyValue_Record.s3"
#define def_dbFileKeyValue_CrawlerMmtPlayUrl "keyValue_CrawlerMmtPlayUrl.s3"

#if !defined(_DbKeyValue_H)
#define _DbKeyValue_H

const int def_defaultKeyLen = 256;
const int def_defaultValueLen = 256;
const int def_maxKeyLen = 10*1024;
const int def_maxValueLen = 10*1024;

class CDbKeyValue
{
public:
    // Constructor
    CDbKeyValue();
    virtual ~CDbKeyValue();
	
    // should , not must ... 
   int setKeyLen(int keyLen);
   int setValueLen(int valueLen);

    //
    //strConn -- 文件
    //bKeyUnique -- key 唯一?
    virtual int Open(const std::string strConn, const bool bKeyUnique=true);

   int addValue(const std::string &Key, const std::string &Value);
   int delValue(const std::string &Key, int iMode=0);
   int modValue(const std::string &Key, const std::string &Value);
   // get the values of the key
   int getValue(const std::string &Key, std::string &Value);
   // get all the value
   int getValue(def_uniKeyValue &valBuff);

   int clearAll();

private:
    SQLite::Database    *m_pDb;    ///< Database connection
    SQLite::Statement   *m_pQuery; ///< Database prepared SQL query

    int m_iKeyLen;
    int m_iValueLen;
};

#endif // !defined(_DbKeyValue_H)
