/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CacheKeyValue.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CacheKeyValue.h 5884 2017-08-11 10:08:23Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-08-11 10:08:23  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <map>

#include <EZMutex.h>

#include "defKeyValue.h"

#if !defined(_CacheKeyValue_H)
#define _CacheKeyValue_H

#if defined(_USE_DB_FILE)
#include "DbKeyValue.h"
#endif //#if defined(_USE_DB_FILE)

#define modPair_Must_Exist 0
#define modPair_If_Not_Exist_Then_Add 1
//class CDbKeyValue;

class CCacheKeyValue// : public CEZObject
{
public:

    CCacheKeyValue();
    //CCacheKeyValue(CDbKeyValue *DbStore);

    virtual ~CCacheKeyValue();

    // 设置了则使用持久化机制
    int setDbfilename(const std::string &strFilename);
    // should , not must ... 
   int setKeyLen(int keyLen);
   int setValueLen(int valueLen);

    // 0- ok, -1- dup
    int addPair(const std::string &strKey, const std::string &strValue);
    int delPair(const std::string &strKey);
    //关键字前部符合即清除
    int clearPair(const std::string &strFront);

    // ret 0 - succ, -1  not exist
    // iMode: 0-normal, 1-create if not exitst
    int modPair(const std::string &strKey, const std::string &strValue, int iMode = modPair_Must_Exist);
    int modPair(const std::string &strKey, const int iValue, int iMode = modPair_Must_Exist);
    int modPair(const std::string &strKey, const time_t ttValue, int iMode = modPair_Must_Exist);
    int getPair(const std::string &strKey, std::string &strValue);
    int getPair(const std::string &strKey, int &iValue);
    int getPair(const std::string &strKey, time_t &ttValue);

    size_t numPair();
    //#ifdef _FUNC_STANDALONE_UTIL
    int dump(int iMode);
    //#endif //#ifdef _FUNC_STANDALONE_UTIL
    // return, num of key
    int getKey(def_uniKeyVector &Key, int iPos=-1);

    // 获取key中包含strKeyLike字串的所有value
    // return: 个数
    int getValueLike(def_uniKeyVector &Value, const std::string &strKeyLike);

    // 数值使用
    static int bldCacheKey(std::string &strDstIKey, const std::string &strDevID, const std::string &strKey, const int iChn);
    static int bldCacheKey(std::string &strDstIKey, const std::string &strDevID, const std::string &strKey, const std::string &strChn);
    static int bldCacheKey(std::string &strDstIKey, const std::string &strDevID, const std::string &strKey);
    // strDstIKey is sha1 ...
    static int bldCacheKey2(std::string &strDstIKey, const std::string &strSrcKey);

private:
    CEZMutex m_Mutex_mapPair;
    def_uniKeyValue m_mapPair;

    // 持久化存储用
#if defined(_USE_DB_FILE)
    std::string m_strFilename;
    CDbKeyValue *m_pDbStore;

    int m_iKeyLen;
    int m_iValueLen;
#endif //_USE_DB_FILE
};

#endif // !defined(_CacheKeyValue_H)
