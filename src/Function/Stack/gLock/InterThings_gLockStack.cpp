/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * InterThings_gLockStack.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: InterThings_gLockStack.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     InterThings_gLockMsg ---- Intelligent system communication protocol
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *     2013-12-02 10:21:25 WuJunjie 549 all decode interface ready
 * 
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "InterThings_gLockStack.h"

#include <assert.h>

CInterThings_gLockStack::CInterThings_gLockStack()
{
    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";

    //assert(m_pJsonValue);
}

CInterThings_gLockStack::~CInterThings_gLockStack()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

bool CInterThings_gLockStack::Parse( const std::string &document)
{
#if 0
    // 启用严格模式，让非法的json解析时直接返回false，不自动容错。
    // 编译失败， 库？
    Json::Features f = Json::Features::strictMode();
    Json::Reader reader(f);
#else

    Json::Reader reader;
#endif

    bool parsingSuccessful = reader.parse( document, m_JsonValue );

    if ( !parsingSuccessful )
    {
        printf( "Failed to parse %s file: \n%s\n",
                document.c_str(),
                reader.getFormatedErrorMessages().c_str() );

        m_strError = reader.getFormatedErrorMessages();
        m_iError = -1;
    }
    else
    {
        m_strError = "";
        m_iError = 0;
    }

    setValid(parsingSuccessful);
    return parsingSuccessful;
}

bool CInterThings_gLockStack::ToString( std::string &document)
{
    if (!m_bDataValid)
    {
        return false;
    }

    //Json::StyledWriter writer(document);
    Json::FastWriter writer(document);
    writer.write( m_JsonValue );

    return true;
}

const std::string &CInterThings_gLockStack::ToString()
{
    if (!m_bDataValid)
    {
        m_strDocument = "";
    }
    else
    {
        ToString(m_strDocument);
    }

    return m_strDocument;//generate json string:),here all is done
}

bool CInterThings_gLockStack::isValid()
{
    return m_bDataValid;
}

void CInterThings_gLockStack::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CInterThings_gLockStack::getError() const
{
    return m_iError;
}

const std::string &CInterThings_gLockStack::getErrorMessages() const
{
    return m_strError;
}

const Json::Value &CInterThings_gLockStack::GetValue() const
{
    return m_JsonValue;
}

// just example
void CInterThings_gLockStack::Encode()
{

    Json::Value var;

    //apply name and “game_id” value to json struct
    var["name"] = "json";
    var["age"] = 123;

    Json::Value pieces;//store all pieces
    for (int i=1351254;i < 1351254+3;i++)
    {
        Json::Value piece_ex;//here it store just one piece
        piece_ex["mobile"] = i;
        piece_ex["email"] = "ezlibs.com";
        piece_ex["fax"] = i+9;
        pieces.append(piece_ex);//ok,yes we just have apply One piece ,then push back to the array
    }
    var["joy"] = pieces;//yes,store pieces in var [Value]
    m_JsonValue = var;

    setValid();
}

void CInterThings_gLockStack::Encode(const CMsgGeminiKeyString &Msg)
{
    Json::Value var;

    var[GminiKey_Msg_Ver] = Msg.m_strKeyVersion;
    var[GminiKey_Msg_Data] = Msg.m_strKeyData;

    m_JsonValue = var;
    setValid();
}

bool CInterThings_gLockStack::Decode(CMsgGeminiKeyString &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strKeyVersion = m_JsonValue[GminiKey_Msg_Ver].asString();
    Msg.m_strKeyData = m_JsonValue[GminiKey_Msg_Data].asString();

    return true;
}

