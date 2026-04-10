/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * InterThings_gLockStack.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: InterThings_gLockStack.h 5884 2013-11-02 11:10:13Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain -- Intelligent system communication protocol
 *     -
 *
 *  Update:
 *     2013-11-02 11:10:13 WuJunjie Create
 *
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _InterThings_gLockStack_H
#define _InterThings_gLockStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "InterThings_gLockMsg.h"

class CInterThings_gLockStack
{
public :
    CInterThings_gLockStack();
    virtual ~CInterThings_gLockStack();

    virtual bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    virtual const int getError() const;
    virtual const std::string &getErrorMessages() const;

    virtual void Encode(); //example only

    virtual void Encode(const CMsgGeminiKeyString             &Msg);
    // 下列Msg， 调用者自行保证初始化， 接口内部不初始化（vector）
    bool Decode(CMsgGeminiKeyString                           &Msg);

    const Json::Value &GetValue() const;
    bool isValid();
    void setValid(bool bValid=true);
    void dump();

protected:
    bool m_bDataValid;
    Json::Value m_JsonValue;
    std::string m_strDocument;
    int m_iError;
    std::string m_strError;
};
#endif /* _InterThings_gLockStack_H */
