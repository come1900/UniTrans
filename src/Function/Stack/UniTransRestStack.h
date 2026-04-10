/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UniTransRestStack.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: UniTransRestStack.h 5884 2016-09-25 10:26:06Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-09-25 10:26:06  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"

#include "UniTransRestMsg.h"

#if !defined(_UniTransRestStack_H)
#define _UniTransRestStack_H

class CUniTransRestStack// : public CEZObject
{
public:

    CUniTransRestStack();
    virtual ~CUniTransRestStack();

    bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    const int getError() const;
    const std::string &getErrorMessages() const;

    virtual void Encode(const CAckMsgSimple             &Msg);
    virtual bool Decode(CAckMsgSimple                   &Msg);
    // ¼øÈ¨ | ×¢²á | ÐÄÌø | ×¢Ïú
    virtual void    Encode(const CSmsMsg_requestAuth    &Msg);
    virtual bool Decode(CSmsMsg_requestAuth             &Msg);
    // ¼øÈ¨ | ×¢²á | ÐÄÌø | ×¢Ïú ACK
    virtual void    Encode(const CSmsMsg_requestAuthACK &Msg);
    virtual bool Decode(CSmsMsg_requestAuthACK          &Msg);

    virtual void Encode(const CUniMsg_reqCreateAppKey     &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKey           &Msg);
    virtual void Encode(const CUniMsg_reqCreateAppKeyAck  &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKeyAck        &Msg);

    virtual void Encode(const CUniMsg_reqGetSetGmLic      &Msg);
    virtual bool Decode(CUniMsg_reqGetSetGmLic            &Msg);
    virtual void Encode(const CUniMsg_reqGetSetGmLicAck   &Msg);
    virtual bool Decode(CUniMsg_reqGetSetGmLicAck         &Msg);

    virtual void Encode(const CUniMsg_reqConfigModify     &Msg);
    virtual bool Decode(CUniMsg_reqConfigModify           &Msg);

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

private:

};

#endif // !defined(_UniTransRestStack_H)
