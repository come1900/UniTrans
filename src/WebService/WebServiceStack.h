/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WebServiceStack.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: WebServiceStack.h 5884 2013-01-29 03:20:41Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:41  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _WebServiceStack_H
#define _WebServiceStack_H

#include <json.h>
#include <algorithm>
#include <stdio.h>

#include "WebServiceMsg.h"

class CWebServiceMsg
{
public :
    CWebServiceMsg();
    virtual ~CWebServiceMsg();

    virtual bool Parse( const std::string &document);
    bool ToString( std::string &document);
    const std::string &ToString();

    virtual const int getError() const;
    virtual const std::string &getErrorMessages() const;

    virtual void Encode(const CCommonSimpleMsg                                &Msg);
    virtual void Encode(const CIPC_registerURL                               &Msg);
    virtual void Encode(const CIPC_registerURL_ACK                        &Msg);
    virtual void Encode(const CIPC_heartbeat                               &Msg);
    virtual void Encode(const CIPC_heartbeat_ACK                        &Msg);
    virtual void Encode(const CIPC_getDeviceStateList            &Msg);
    virtual void Encode(const CIPC_controlDevice              &Msg);
    virtual void Encode(const CIPC_controlDevice_ACK              &Msg);
    virtual void Encode(const CAuthResponse              &Msg);
    virtual void Encode(const CIPC_requestAuth              &Msg);
    virtual void Encode(const CAuthHostResponse              &Msg);
    virtual void Encode(const CIPC_requestAuthByHost 			&Msg);
    virtual void Encode(const CIPC_getDeviceStateList_ACK                        &Msg);
    virtual void Encode(const CIPC_stopHostNetwork            &Msg);
    virtual void Encode(const CIPC_stopHostNetwork_ACK              &Msg);
    virtual void Encode(const CIPC_checkUpdate              &Msg);
    virtual void Encode(const CIPC_checkUpdate_ACK              &Msg);
    virtual void Encode(const CIPC_notifyUpdate              &Msg);
    virtual void Encode(const CIPC_notifyUpdate_ACK                        &Msg);

    // 下列Msg， 调用者自行保证初始化， 接口内部不初始化（vector）
    bool Decode(CCommonSimpleMsg                                &Msg);
    bool Decode(CIPC_registerURL                               &Msg);
    bool Decode(CIPC_registerURL_ACK                        &Msg);
    bool Decode(CIPC_heartbeat                               &Msg);
    bool Decode(CIPC_heartbeat_ACK                        &Msg);
    bool Decode(CIPC_getDeviceStateList            &Msg);
    bool Decode(CIPC_controlDevice              &Msg);
	bool Decode(CIPC_controlDevice_ACK               &Msg);
    bool Decode(CAuthResponse              &Msg);
    bool Decode(CIPC_requestAuth              &Msg);
    bool Decode(CAuthHostResponse              &Msg);
    bool Decode(CIPC_requestAuthByHost			&Msg);
    bool Decode(CIPC_getDeviceStateList_ACK                        &Msg);
    bool Decode(CIPC_stopHostNetwork            &Msg);
    bool Decode(CIPC_stopHostNetwork_ACK              &Msg);
	bool Decode(CIPC_checkUpdate               &Msg);
    bool Decode(CIPC_checkUpdate_ACK              &Msg);
    bool Decode(CIPC_notifyUpdate              &Msg);
    bool Decode(CIPC_notifyUpdate_ACK                        &Msg);
	
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
#endif /* _WebServiceStack_H */
