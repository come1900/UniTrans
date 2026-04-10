/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CacheKeyValue.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CacheKeyValue.cpp 5884 2017-08-11 10:08:25Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-08-11 10:08:25  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>

#include "str_opr.h"
#include "md5.h"
#include "sha1.h"

#include "StringOpr.h"
#include "EZLock.h"

#if defined(_USE_DB_FILE)
#include "DbKeyValue.h" //
#endif //_USE_DB_FILE
#include "CacheKeyValue.h"

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

CCacheKeyValue::CCacheKeyValue()
{
#if defined(_USE_DB_FILE)
    m_pDbStore = NULL;

    m_iKeyLen = def_defaultKeyLen;
    m_iValueLen = def_defaultValueLen;
#endif //_USE_DB_FILE
}
CCacheKeyValue::~CCacheKeyValue()
{
#if defined(_USE_DB_FILE)
    if (m_pDbStore)
        delete m_pDbStore;
#endif //_USE_DB_FILE
}

// return: 1 已经设置, -2, -3
#if defined(_USE_DB_FILE)
int CCacheKeyValue::setDbfilename(const std::string &strFilename)
{
    if (strFilename.length()<1)
    {
        return -1;
    }

    //std::cout << "strFilename:" << strFilename << std::endl;

    //已经设置了
    if (m_pDbStore != NULL)
        return 1;

    m_pDbStore = new CDbKeyValue;

    if (NULL == m_pDbStore)
        return -2;

    m_pDbStore->setKeyLen(m_iKeyLen);
    m_pDbStore->setValueLen(m_iValueLen);

    //
    int iretOpen = m_pDbStore->Open(strFilename, true);
    if (iretOpen < 0)
    {
        //
        return -3;
    }

    CEZLock __lock(m_Mutex_mapPair);
    m_pDbStore->getValue(m_mapPair);

    return 0;
}

// should , not must ...
int CCacheKeyValue::setKeyLen(int keyLen)
{
    if (keyLen > def_maxKeyLen
        || keyLen < 1)
    {
        return -1;
    }

    m_iKeyLen = keyLen;

    return 0;
}

int CCacheKeyValue::setValueLen(int valueLen)
{
    if (valueLen > def_maxValueLen
        || valueLen < 1)
    {
        return -1;
    }

    m_iValueLen = valueLen;

    return 0;
}

#endif //_USE_DB_FILE

int CCacheKeyValue::addPair(const std::string &strKey, const std::string &strValue)
{
    CEZLock __lock(m_Mutex_mapPair);

    std::pair< def_uniKeyValue::const_iterator, bool > ret;
    ret = m_mapPair.insert ( std::pair<std::string, std::string>(strKey, strValue) );

    // ok
    if( ret.second )
    {
#if defined(_USE_DB_FILE)
        if (m_pDbStore)
            m_pDbStore->addValue(strKey, strValue);
#endif

        return 0;
    }

    return -1;
}

int CCacheKeyValue::modPair(const std::string &strKey, const std::string &strValue, int iMode)
{
    if (strKey.length()<1 ||strValue.length()<1)
    {
        return -1;
    }

    def_uniKeyValue::iterator it = m_mapPair.find(strKey);

    // not found
    if (it == m_mapPair.end())
    {
        // 添加
        if (modPair_If_Not_Exist_Then_Add==iMode)
        {
            addPair(strKey, strValue);
        }
        else
        {
            return -2;
        }
    }
    // found
    else
    {
        CEZLock __lock(m_Mutex_mapPair);

#if defined(_USE_DB_FILE)

        if (m_pDbStore
            && it->second != strValue)// performance
        {
            m_pDbStore->modValue(strKey, strValue);
        }
#endif

        it->second = strValue;
    }

    return 0;
}

int CCacheKeyValue::modPair(const std::string &strKey, const int iValue, int iMode)
{
    return modPair(strKey, ezConvertToString(iValue), iMode);
}

int CCacheKeyValue::modPair(const std::string &strKey, const time_t ttValue, int iMode)
{
    return modPair(strKey, ezConvertToString(ttValue), iMode);
}

int CCacheKeyValue::getPair(const std::string &strKey, std::string &strValue)
{
    //CEZLock __lock(m_Mutex_mapPair);

    def_uniKeyValue::const_iterator it = m_mapPair.find(strKey);

    // not found
    if (it == m_mapPair.end())
    {
        return -1;
    }

    strValue = it->second;

    return 0;
}
int CCacheKeyValue::getPair(const std::string &strKey, int &iValue)
{
    //CEZLock __lock(m_Mutex_mapPair);

    def_uniKeyValue::const_iterator it = m_mapPair.find(strKey);

    // not found
    if (it == m_mapPair.end())
    {
        return -1;
    }

    //strValue = it->second;
    ezConvertFromString(iValue, it->second);

    return 0;
}
int CCacheKeyValue::getPair(const std::string &strKey, time_t &ttValue)
{
    def_uniKeyValue::const_iterator it = m_mapPair.find(strKey);

    // not found
    if (it == m_mapPair.end())
    {
        return -1;
    }

    ezConvertFromString(ttValue, it->second);

    return 0;
}

// -1 not found
int CCacheKeyValue::delPair(const std::string &strKey)
{
    //def_uniKeyValue::const_iterator it = m_mapPair.find(strKey);

    // not found
    //if (it == m_mapPair.end())
    if (0) // use erase
    {
        return -1;
    }

    CEZLock __lock(m_Mutex_mapPair);

    unsigned int numErase = m_mapPair.erase(strKey);                  // erasing by key

    // not found
    if (numErase < 1)
    {
        return -1;
    }

    DBG(
        std::cout << "numErase:" << numErase << std::endl;
    );

#if defined(_USE_DB_FILE)

    if (m_pDbStore)
        m_pDbStore->delValue(strKey);
#endif

    return 0;
}

int CCacheKeyValue::clearPair(const std::string &strFront)
{
    if (strFront.length()<1)
    {
        return -1;
    }

    CEZLock __lock(m_Mutex_mapPair);

    def_uniKeyValue::iterator it = m_mapPair.begin();
    for ( ; it != m_mapPair.end() ; /*it++*/)
    {

        std::size_t foundt = it->first.find(strFront);

        //在首部
        if (0==foundt)
        {
#if defined(_USE_DB_FILE)
            if (m_pDbStore)
                m_pDbStore->delValue(it->first);
#endif

            m_mapPair.erase(it++);
        }
        else
        {
            it++;
        }
    }

    return 0;
}

size_t CCacheKeyValue::numPair()
{
    return m_mapPair.size();
}

int CCacheKeyValue::dump(int iMode)
{
    if (m_mapPair.size() <= 0)
    {
        printf("%s:: no values\n", __FUNCTION__);
        return 0;
    }

    printf("%16s | %s\n", "Key", "Value");
    printf("--------------------------------------\n");

    def_uniKeyValue::const_iterator it = m_mapPair.begin();
    for ( ; it != m_mapPair.end() ; it++)
    {
        printf("%16s | %s\n", it->first.c_str(), it->second.c_str());
    }

    return 0;
}
int CCacheKeyValue::getKey(def_uniKeyVector &Key, int iPos)
{
    if (m_mapPair.size() <= 0)
    {
        DBG(
            printf("%s:: no values\n", __FUNCTION__);
        );
        return 0;
    }

    DBG(
        printf("%16s | %s\n", "Key", "Value");
        printf("--------------------------------------\n");
    );

    int __curPos=0;
    def_uniKeyValue::const_iterator it = m_mapPair.begin();
    for ( ; it != m_mapPair.end() ; it++)
    {
        DBG(
            printf("%16s | %s\n", it->first.c_str(), it->second.c_str());
        );

        // get all
        if (iPos == -1)
        {
            Key.push_back(it->first);
        }
        else if (iPos == __curPos)
        {
            Key.push_back(it->first);
            return 1;
        }

        __curPos++;
    }

    return Key.size();
}

int CCacheKeyValue::getValueLike(def_uniKeyVector &Value, const std::string &strKeyLike)
{
    if (strKeyLike.length() < 1)
    {
        return -1;
    }

    if (m_mapPair.size() <= 0)
    {
        DBG(
            printf("%s:: no values\n", __FUNCTION__);
        );
        return 0;
    }

    DBG(
        printf("%16s | %s\n", "Key", "Value");
        printf("--------------------------------------\n");
    );

    //int __curPos=0;
    def_uniKeyValue::const_iterator it = m_mapPair.begin();
    for ( ; it != m_mapPair.end() ; it++)
    {
        DBG(
            printf("%16s | %s\n", it->first.c_str(), it->second.c_str());
        );

        if (strstr(it->first.c_str(), strKeyLike.c_str()))
        {
            Value.push_back(it->second);
        }

        //__curPos++;
    }

    return Value.size();
}

int CCacheKeyValue::bldCacheKey(std::string &strDstIKey, const std::string &strDevID, const std::string &strKey, const int iChn)
{
    if (strDevID.empty() || strKey.empty() /*|| iChn.empty()*/)
    {
        //ez_printf_error("Error:%s\n", __FUNCTION__);

        return -1;
    }

    //strDstIKey = (strDevID+strKey+iChn);
    strDstIKey = strDevID;
    strDstIKey += strKey;
    strDstIKey += ezConvertToString(iChn);

    return 0;
}

int CCacheKeyValue::bldCacheKey(std::string &strDstIKey, const std::string &strDevID, const std::string &strKey, const std::string &strChn)
{
    if (strDevID.empty() || strKey.empty() || strChn.empty())
    {
        //ez_printf_error("Error:%s\n", __FUNCTION__);

        return -1;
    }

    //strDstIKey = (strDevID+strKey+strChn);
    strDstIKey = strDevID;
    strDstIKey += strKey;
    strDstIKey += strChn;

    return 0;
}
int CCacheKeyValue::bldCacheKey(std::string &strDstIKey, const std::string &strDevID, const std::string &strKey)
{
    if (strDevID.empty() || strKey.empty())
    {
        //ez_printf_error("Error:%s\n", __FUNCTION__);

        return -1;
    }

    //strDstIKey = (strDevID+strKey+strChn);
    strDstIKey = strDevID;
    strDstIKey += strKey;
    //strDstIKey += strChn;

    return 0;
}

// strDstIKey is sha1 ...
int CCacheKeyValue::bldCacheKey2(std::string &strDstIKey, const std::string &strSrcKey)
{
    if (strSrcKey.empty())
    {
        return -1;
    }

    char bufAppSecret[SHA1HashSizeDestBuffer] = {'\0'};
    int iret = ez_sha1_str(bufAppSecret, sizeof(bufAppSecret), strSrcKey.c_str());

    if (iret < 0)
    {
        return -2;
    }

    strDstIKey = bufAppSecret;

    return 0;
}

#if defined (_FUNC_STANDALONE_UTIL) || defined (_FUNC_STANDALONE_UTIL_MF)

int main(int argc, char *argv[])
{
    CCacheKeyValue __kv;

#if defined(_USE_DB_FILE)

    __kv.setDbfilename("kv.s3");
#endif //#if defined(_USE_DB_FILE)

    assert(__kv.numPair()==0);

    {
        std::string strKey = "hello";
        std::string strValue = "world";
        int iret;

        iret = __kv.addPair(strKey, strValue);
        std::cout << "Tst:" << (iret==0?"OK":"Failed") << "\t" << "addPair:" << iret << std::endl;

        // 重复
        iret = __kv.addPair(strKey, strValue);
        std::cout << "Tst:" << (iret==-1?"OK":"Failed") << "\t" << "addPair:" << iret << std::endl;

        //__kv.dump(0);
        assert(__kv.numPair()==1);
    }

    {
        std::string strKey = "hello";
        std::string strValue;


        int iret = __kv.getPair(strKey, strValue);
        std::cout << "Tst:" << (iret==0?"OK":"Failed") << "\t";
        std::cout << "getPair(" << iret << ")::" << strKey << "=" << strValue << std::endl;
    }
    // 不存在
    {
        std::string strKey = "hello__";
        std::string strValue;


        int iret = __kv.getPair(strKey, strValue);
        std::cout << "Tst:" << (iret==-1?"OK":"Failed") << "\t";
        std::cout << "getPair(" << iret << ")::" << strKey << "=" << strValue << std::endl;
    }

    {
        std::string strKey = "hello";
        std::string strValue = "Unitrans";
        int iret;

        iret = __kv.modPair(strKey, strValue);
        std::cout << "Tst:" << (iret==0?"OK":"Failed") << "\t";
        std::cout << "modPair:" << iret << std::endl;

        //__kv.dump(0);
        assert(__kv.numPair()==1);
    }
    // 不存在
    {
        std::string strKey = "love";
        std::string strValue = "Unitrans";
        int iret;

        iret = __kv.modPair(strKey, strValue);
        std::cout << "Tst:" << (iret==-1?"OK":"Failed") << "\t";
        std::cout << "modPair:" << iret << std::endl;

        //__kv.dump(0);
        assert(__kv.numPair()==1);
    }
    // 不存在添加模式
    {
        std::string strKey = "love";
        std::string strValue = "Unitrans";
        int iret;

        iret = __kv.modPair(strKey, strValue, 1);
        std::cout << "Tst:" << (iret==0?"OK":"Failed") << "\t";
        std::cout << "modPair:" << iret << std::endl;

        //__kv.dump(0);
        assert(__kv.numPair()==2);
    }

    // 再添加一个
    {
        std::string strKey = "hello_k";
        std::string strValue = "world";
        int iret;

        iret = __kv.modPair(strKey, strValue, 1);
        std::cout << "Tst:" << (iret==0?"OK":"Failed") << "\t";
        std::cout << "modPair:" << iret << std::endl;

        //__kv.dump(0);
        assert(__kv.numPair()==3);
    }

    //
    {
        def_uniKeyVector __val;
        int iret = __kv.getValueLike(__val, "hello");
        std::cout << "Tst:" << (iret==2?"OK":"Failed") << "\t";
        std::cout << "getValueLike:" << iret << std::endl;
        //std::cout << "Value0:" << __val[0] << std::endl;
        //std::cout << "Value1:" << __val[1] << std::endl;

        //__kv.dump(0);
        assert(__kv.numPair()==3);
    }

    {
        std::string strKey = "hello";
        int iret;

        iret = __kv.delPair(strKey);
        std::cout << "Tst:" << (iret==0?"OK":"Failed") << "\t";
        std::cout << "delPair:" << iret << std::endl;
        iret = __kv.delPair(strKey);
        std::cout << "Tst:" << (iret==-1?"OK":"Failed") << "\t";
        std::cout << "delPair:" << iret << std::endl;
        //__kv.dump(0);
        assert(__kv.numPair()==2);
    }

    //return 0;

    // clearPair
    {
        std::string strKey = "DevID_001_hehe";
        std::string strValue = "abc";
        int iret;

        iret = __kv.modPair(strKey, strValue, 1);
        strKey = strKey+"b";
        iret = __kv.modPair(strKey, strValue, 1);

        int beforeClear = __kv.numPair();
        iret = __kv.clearPair("DevID");
        int afterClear = __kv.numPair();
        std::cout << "Tst:" << ((beforeClear-afterClear)==2?"OK":"Failed") << "\t";
        std::cout << "clearPair:" << iret << std::endl;

        //__kv.dump(0);
        assert((beforeClear-afterClear)==2);
    }

    printf("\t\t -- Fini\n");

    return 0;
}

#endif // _FUNC_STANDALONE_UTIL
