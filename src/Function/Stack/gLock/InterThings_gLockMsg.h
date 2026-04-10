/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * InterThings_gLockMsg.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: InterThings_gLockMsg.h 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     InterThings_gLockMsg -- 联网glock
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _InterThings_gLockMsg_H
#define _InterThings_gLockMsg_H

#include <string>
#include <vector>
#include <iostream>

//
// 秘钥消息
//
class CMsgGeminiKeyString
{
public:
	// 版本号 parse、 use
    std::string m_strKeyVersion;

    //秘钥数据
    std::string m_strKeyData;

    CMsgGeminiKeyString()
    {
        m_strKeyVersion = "";
        m_strKeyData = "";
    }

    void dump()
    {
        std::cout << "m_strKeyVersion:" << m_strKeyVersion << std::endl;
        std::cout << "m_strKeyData:" << m_strKeyData << std::endl;
    }
};

const std::string GminiKey_Msg_Ver= "Ver";
const std::string GminiKey_Msg_Data= "Key";

#endif /* _InterThings_gLockMsg_H */
