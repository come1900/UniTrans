/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiKeyStack.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiKeyStack.h 5884 2013-08-30 09:33:05Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-08-30 09:33:05 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GeminiKeyStack_H
#define _GeminiKeyStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

#include "GeminiKey.h"

class CGeminiKeyStack
{
public :
    CGeminiKeyStack();
    virtual ~CGeminiKeyStack();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // to json
    void Encode(const GeminiKeys                        &Msg);
    // to class
    int Decode(GeminiKeys                        &Msg);
    virtual bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int GeminiKeyCreate(GeminiKey &Lock
                        , GeminiKey &Unlock
                        , const std::string &strPid = "13359211641994461155"
#ifdef USE_TIME_STAMP_IN_KEY    	
                        , const std::string &strTimeStamp = "" /*will be current time*/
                        , const std::string &strMemo = "gminitech"
#endif //USE_TIME_STAMP_IN_KEY
                                                      , const int iBits = GEMINI_KEY_BITS);

    bool GeminiKeyExport(std::string &document);
    int GeminiKeyImport(const std::string &document);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 上锁
    // PlainText --返回明文
    // LockedData -- 返回加密后数据， base64
    // ret 0-ok, -1 -data error -2:rsa enc error failed.
    int GeminiLock(const GeminiKeys &LockKey
                   , std::string &PlainText
                   , std::string &LockedData);
    int GeminiUnlock(const GeminiKeys &UnlockKey
                     , const std::string &PlainText
                     , const std::string &LockedData);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 上锁
    // PlainText --返回明文
    // LockedData -- 返回加密后数据， base64
    // ret 0-ok, -1 -data error -2:rsa enc error failed.
    int GeminiLock(const std::string &strKey
                   , std::string &PlainText
                   , std::string &LockedData);
    int GeminiUnlock(const std::string &strKey
                     , const std::string &PlainText
                     , const std::string &LockedData);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    bool isValid();
    void setValid(bool bValid=true);
    void dump();
    const Json::Value &GetValue() const;


    virtual const int getError() const;
    virtual const std::string &getErrorMessages() const;

private:
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //工具
    /* To get the C-string PEM form: */
    int dump_PrivateKeyAsPem(RSA *r, char *pBuf);
    int dump_PublicKeyAsPem(RSA *r, char *pBuf);
    int geminiRandString(const unsigned char *pSeed, const int SeedLen, unsigned char *pOut, const int iOutNum);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


    bool m_bDataValid;
    Json::Value m_JsonValue;
    std::string m_strDocument;
    int m_iError;
    std::string m_strError;
};
#endif /* _GeminiKeyStack_H */
