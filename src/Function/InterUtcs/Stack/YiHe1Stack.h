/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * YiHe1Stack.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: YiHe1Stack.h 5884 2014-10-12 04:33:39Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 04:33:39  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _YiHe1Stack_H
#define _YiHe1Stack_H

//#include <json.h>
#include "tinyxml.h"
#include "tinystr.h"

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "YiHe1Msg.h"

class CYiHe1Stack
{
public :
    CYiHe1Stack();
    virtual ~CYiHe1Stack();

    virtual bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    virtual const int getError() const;
    virtual const std::string &getErrorMessages() const;

    virtual void Encode(); //example only

    // 下列Msg， 调用者自行保证初始化， 接口内部不初始化（vector）
    virtual void  Encode(const CYH1MsgHeader &Msg);
    bool 		  Decode(CYH1MsgHeader &Msg);

    virtual void  Encode(const CMsgGetSignalControlerParaRequest &Msg);
    bool 		  Decode(CMsgGetSignalControlerParaRequest &Msg);
    virtual void  Encode(const CMsgGetSignalControlerParaResponse &Msg);
    bool 		  Decode(CMsgGetSignalControlerParaResponse &Msg);

    virtual void  Encode(const CMsgGetCrossParaRequest &Msg);
    bool 		  Decode(CMsgGetCrossParaRequest &Msg);
    virtual void  Encode(const CMsgGetCrossParaResponse &Msg);
    bool 		  Decode(CMsgGetCrossParaResponse &Msg);

    virtual void  Encode(const CMsgGetCrossStatusRequest &Msg);
    bool 		  Decode(CMsgGetCrossStatusRequest &Msg);
    virtual void  Encode(const CMsgGetCrossStatusResponse &Msg);
    bool 		  Decode(CMsgGetCrossStatusResponse &Msg);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    bool isValid();
    void setValid(bool bValid=true);
    void dump();

protected:
    bool m_bDataValid;

    //    Json::Value m_JsonValue;

    std::string m_strDocument;

    TiXmlDocument m_xmlDocument;

    int m_iError;
    std::string m_strError;

    void AddIntElement(TiXmlElement &root, const std::string &subTitle, int value);
    void AddStrElement(TiXmlElement& root, const std::string& subTitle, const std::string& value);
};
#endif /* _YiHe1Stack_H */
