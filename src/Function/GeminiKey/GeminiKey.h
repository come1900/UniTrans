/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiKey.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiKey.h 5884 2013-08-30 09:31:53Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-08-30 09:31:53 WuJunjie Create
 *     2014-02-11 11:39:08 WuJunjie add dump string inf
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GeminiKey_H
#define _GeminiKey_H

#include <string.h>

#include <string>
#include <vector>
#include <iostream>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <base64.h>
#include <md5.h>
#include <ez_system_api.h>

//#define GEMINI_KEY_BITS 1024//228, 512, 1024 2048 4096
#define GEMINI_KEY_BITS 2048//228, 512, 1024 2048 4096
#define MAX_GEMINI_KEY_DATA_LEN 2*1024 //??ó|GEMINI_KEY_BITS μ?3?

#define PLAN_TEXT_BYTES 20
// Lock
#define GeminiKeyTypeLockKey       "L"
// Unlock
#define GeminiKeyTypeUnlockKey       "U"

const int VERIFY_CODE_LEN = 6;

#define     GeminiKeyIdentity "gminiTech"
#define     GeminiKeyVersion "1.0.0"

// 钥匙数据中不要掺杂业务数据， 移到keys中的一个字段中处理
#undef USE_TIME_STAMP_IN_KEY

class GeminiKey
{
public:
    std::string strVerifyCode;//校验码, DO NOT MODIFY
    std::string strPeerID;//卡号
    std::string strKeyType;//L-Lock/U-Unlock/
    std::string strKeyString;//pem
    	
#ifdef USE_TIME_STAMP_IN_KEY    	
    std::string strTimeStamp;//20141027185509
    std::string strMemo;//备注信息
#endif //USE_TIME_STAMP_IN_KEY

    GeminiKey();

    void dump() const; //discards qualifiers 声明不会改变
    void dump(std::string &strDest) const; //discards qualifiers 声明不会改变
    // 数据校验
    bool IsValid() const;
    bool checkVerifyCode();
    // 刷新校验码
    int freshVerifyCode();

    GeminiKey& operator = (const GeminiKey& obj);
    /* == */
    bool operator==(const GeminiKey& obj) const;

private:
    bool m_bValid;
    // 生成校验码
    int GenVerifyCode(std::string &strVerifyCode);
};

class GeminiKeys
{
public:
    std::string Identity;//自身说明
    std::string Version;//版本
    std::string strFunString;//自定义功能字符串，如 E20141027185509，？？到期； B20141027185509  ？？开始使用等

    std::vector<GeminiKey> m_vecGeminiKey;

    GeminiKeys();

    void dump() const; //discards qualifiers 声明不会改变
    void dump(std::string &strDest) const; //discards qualifiers 声明不会改变

    // 密钥数据必须有，其他暂不做检查
    bool isValid() const;
    bool checkVerifyCode();

    GeminiKeys& operator = (const GeminiKeys& obj);
    /* == */
    bool operator==(const GeminiKeys& obj) const;

};


#endif /* _GeminiKey_H */
