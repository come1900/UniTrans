/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WebServiceStack.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: WebServiceStack.cpp 5884 2013-01-29 03:20:48Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:48  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include "WebServiceStack.h"

CWebServiceMsg::CWebServiceMsg()
{
    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";
}

CWebServiceMsg::~CWebServiceMsg()
{
}

bool CWebServiceMsg::Parse( const std::string &document)
{
    Json::Reader reader;
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

bool CWebServiceMsg::ToString( std::string &document)
{
    if (!m_bDataValid)
    {
        return false;
    }

    //Json::StyledWriter writer(document);
    Json::FastWriter writer(document);

    //writer.write( m_JsonValue );
    writer.writeCompact( m_JsonValue );

    return true;
}

const std::string &CWebServiceMsg::ToString()
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

bool CWebServiceMsg::isValid()
{
    return m_bDataValid;
}

void CWebServiceMsg::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CWebServiceMsg::getError() const
{
    return m_iError;
}

const std::string &CWebServiceMsg::getErrorMessages() const
{
    return m_strError;
}

const Json::Value &CWebServiceMsg::GetValue() const
{
    return m_JsonValue;
}

void CWebServiceMsg::Encode(const CCommonSimpleMsg &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;

    m_JsonValue = var;
    setValid();
}

bool CWebServiceMsg::Decode(CCommonSimpleMsg &Msg)
{
    Msg.result = m_JsonValue["result"].asString();
    Msg.reason = m_JsonValue["reason"].asString();
    return true;
}

void CWebServiceMsg::Encode(const CIPC_registerURL &Msg)
{
    Json::Value var;

    var["sessionId"] = Msg.sessionId;
    var["baseURL"] = Msg.baseURL;
    m_JsonValue = var;

    setValid();
}

bool CWebServiceMsg::Decode(CIPC_registerURL &Msg)
{
    Msg.baseURL= m_JsonValue["baseURL"].asString();
    Msg.sessionId= m_JsonValue["sessionId"].asString();

    return true;
}

void CWebServiceMsg::Encode(const CIPC_registerURL_ACK &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;

    m_JsonValue = var;

    setValid();
}
bool CWebServiceMsg::Decode(CIPC_registerURL_ACK &Msg)
{
    Msg.result = m_JsonValue["result"].asString();
    Msg.reason = m_JsonValue["reason"].asString();

    return true;
}


void CWebServiceMsg::Encode(const CIPC_heartbeat &Msg)
{
    Json::Value var;

    var["sessionId"] = Msg.sessionId;
    m_JsonValue = var;

    setValid();
}

bool CWebServiceMsg::Decode(CIPC_heartbeat &Msg)
{
    Msg.sessionId= m_JsonValue["sessionId"].asString();

    return true;
}

void CWebServiceMsg::Encode(const CIPC_heartbeat_ACK &Msg)
{
    Json::Value var;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;

    m_JsonValue = var;

    setValid();
}
bool CWebServiceMsg::Decode(CIPC_heartbeat_ACK &Msg)
{
    Msg.result = m_JsonValue["result"].asString();
    Msg.reason = m_JsonValue["reason"].asString();

    return true;
}

void CWebServiceMsg::Encode(const CIPC_getDeviceStateList &Msg)
{
    Json::Value var;
    unsigned int ii;

    var["sessionId"]    = Msg.sessionId;
	
	Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_operate.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["dataPoint"]     = Msg.m_operate[ii].dataPoint;
        operate_all.append(operate);
    }
    var["operate"] = operate_all;
    m_JsonValue = var;

    setValid();
}

bool CWebServiceMsg::Decode(CIPC_getDeviceStateList &Msg)
{
    Msg.sessionId= m_JsonValue["sessionId"] .asString();
	
    const Json::Value arrayOperate = m_JsonValue["operate"];
    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CIPC_getDeviceStateList_operate _operate;
        _operate.dataPoint = arrayOperate[i]["dataPoint"].asString();

        Msg.m_operate.push_back(_operate);
    }

    return true;
}


void CWebServiceMsg::Encode(const CIPC_controlDevice &Msg)
{
    setValid(false);

    Json::Value var;
    unsigned int ii;

    var["sessionId"] = Msg.sessionId;
	
	Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_operate.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["dataPoint"] = Msg.m_operate[ii].dataPoint;
        operate["dataLen"] = Msg.m_operate[ii].dataLen;
        operate["data"] = Msg.m_operate[ii].data;
		
        operate_all.append(operate);
    }
    var["operate"] = operate_all;
    m_JsonValue = var;

    setValid();
}

bool CWebServiceMsg::Decode(CIPC_controlDevice &Msg)
{
    Msg.sessionId= m_JsonValue["sessionId"].asString();
    Msg.homeIdStr= m_JsonValue["homeIdStr"].asString();
	
    const Json::Value arrayOperate = m_JsonValue["operate"];
    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CIPC_controlDevice_operate _operate;
        _operate.dataPoint = arrayOperate[i]["dataPoint"].asString();
        _operate.dataLen = arrayOperate[i]["dataLen"].asString();
        _operate.data = arrayOperate[i]["data"].asString();

        Msg.m_operate.push_back(_operate);
    }

    return true;

}

void CWebServiceMsg::Encode(const CIPC_controlDevice_ACK &Msg)
{
    setValid(false);

    Json::Value var;
    unsigned int ii;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;
	
	Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_operate.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["dataPoint"] = Msg.m_operate[ii].dataPoint;
        operate["dataLen"] = Msg.m_operate[ii].dataLen;
        operate["data"] = Msg.m_operate[ii].data;
		
        operate_all.append(operate);
    }
    var["operate"] = operate_all;
    m_JsonValue = var;

    setValid();

}

bool CWebServiceMsg::Decode(CIPC_controlDevice_ACK &Msg)
{
    Msg.result= m_JsonValue["result"] .asString();
    Msg.reason= m_JsonValue["reason"] .asString();
	
    const Json::Value arrayOperate = m_JsonValue["operate"];
    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CIPC_controlDevice_ACK_Operate _operate;
        _operate.dataPoint = arrayOperate[i]["dataPoint"].asString();
        _operate.dataLen = arrayOperate[i]["dataLen"].asString();
        _operate.data = arrayOperate[i]["data"].asString();

        Msg.m_operate.push_back(_operate);
    }

    return true;

}


void CWebServiceMsg::Encode(const CAuthResponse &Msg)
{
    Json::Value var;

    var["sessionId"]     = Msg.sessionId ;
    var["result"]     = Msg.result ;
    var["reason"]      = Msg.reason  ;

    m_JsonValue = var;

    setValid();
}

bool CWebServiceMsg::Decode(CAuthResponse &Msg)
{
    Msg.sessionId = m_JsonValue["sessionId"].asString() ;
    Msg.result = m_JsonValue["result"].asString() ;
    Msg.reason  = m_JsonValue["reason"] .asString() ;

    return true;
}

void CWebServiceMsg::Encode(const CIPC_requestAuth &Msg)
{
    Json::Value var;

    var["authUser"] = Msg.authUser;
    var["authPassword"] = Msg.authPassword;

    m_JsonValue = var;
    setValid();
}

bool CWebServiceMsg::Decode(CIPC_requestAuth &Msg)
{
    Msg.authUser = m_JsonValue["authUser"].asString();
    Msg.authPassword = m_JsonValue["authPassword"].asString();

    return true;
}

bool CWebServiceMsg::Decode(CAuthHostResponse &Msg)
{
    Msg.sessionId2 = m_JsonValue["sessionId2"].asString() ;
    Msg.result = m_JsonValue["result"].asString() ;
    Msg.reason  = m_JsonValue["reason"] .asString() ;

    return true;
}

void CWebServiceMsg::Encode(const CAuthHostResponse 			 &Msg)
{
    Json::Value var;

    var["sessionId2"]     = Msg.sessionId2;
    var["result"]     = Msg.result ;
    var["reason"]      = Msg.reason  ;

    m_JsonValue = var;

    setValid();

}

bool CWebServiceMsg::Decode(CIPC_requestAuthByHost &Msg)
{
    Msg.sessionId = m_JsonValue["sessionId"].asString();
    Msg.homeIdStr = m_JsonValue["homeIdStr"].asString();
    Msg.userName = m_JsonValue["userName"].asString();
    Msg.password = m_JsonValue["password"].asString();

    return true;
}

void CWebServiceMsg::Encode(const CIPC_requestAuthByHost			&Msg)
{
	Json::Value var;
	
	var["sessionId"] = Msg.sessionId;
	var["homeIdStr"] = Msg.homeIdStr;
	var["userName"] = Msg.userName;
	var["password"] = Msg.password;

	m_JsonValue = var;
	setValid();

}

void CWebServiceMsg::Encode(const CIPC_getDeviceStateList_ACK &Msg)
{
    Json::Value var;
    unsigned int ii;

    var["result"] = Msg.result;
    var["reason"] = Msg.reason;
	
	Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_operate.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["dataPoint"] = Msg.m_operate[ii].dataPoint;
        operate["dataLen"] = Msg.m_operate[ii].dataLen;
        operate["data"] = Msg.m_operate[ii].data;
		
        operate_all.append(operate);
    }
    var["operate"] = operate_all;
    m_JsonValue = var;

    setValid();

}

bool CWebServiceMsg::Decode(CIPC_getDeviceStateList_ACK &Msg)
{
    Msg.result= m_JsonValue["result"] .asString();
    Msg.reason= m_JsonValue["reason"] .asString();
	
    const Json::Value arrayOperate = m_JsonValue["operate"];
    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CIPC_getDeviceStateList_ACK_operate _operate;
        _operate.dataPoint = arrayOperate[i]["dataPoint"].asString();
        _operate.dataLen = arrayOperate[i]["dataLen"].asString();
        _operate.data = arrayOperate[i]["data"].asString();

        Msg.m_operate.push_back(_operate);
    }

    return true;

}

void CWebServiceMsg::Encode(const CIPC_stopHostNetwork &Msg)
{
    Json::Value var;

    var["sessionId"] = Msg.sessionId ;
    var["homeIdStr"] = Msg.homeIdStr ;
    var["username"]  = Msg.username  ;

    m_JsonValue = var;

    setValid();
}
bool CWebServiceMsg::Decode(CIPC_stopHostNetwork &Msg)
{
    Msg.sessionId = m_JsonValue["sessionId"].asString();
    Msg.homeIdStr = m_JsonValue["homeIdStr"].asString();
    Msg.username  = m_JsonValue["username"] .asString();

    return true;
}

void CWebServiceMsg::Encode(const CIPC_stopHostNetwork_ACK                         &Msg)
{
    Json::Value var;

    var["result"] 	 = Msg.result;
    var["reason"] 	 = Msg.reason;

    m_JsonValue = var;

    setValid();
}
bool CWebServiceMsg::Decode(CIPC_stopHostNetwork_ACK &Msg)
{
    Msg.result  = m_JsonValue["result"].asString();
    Msg.reason  = m_JsonValue["reason"].asString();

    return true;
}
void CWebServiceMsg::Encode(const CIPC_checkUpdate                       &Msg)
{
    Json::Value var;

    var["homeIdStr"] = Msg.homeIdStr;
    var["sessionId"] = Msg.sessionId;
    var["lastTime"] = Msg.lastTime;
    var["aesKey"] = Msg.aesKey;
    var["queryFrom"] = Msg.queryFrom;
    var["product"] = Msg.product;
    m_JsonValue = var;

    setValid();
}
bool CWebServiceMsg::Decode(CIPC_checkUpdate &Msg)
{
    Msg.homeIdStr  = m_JsonValue["homeIdStr"].asString();
    Msg.sessionId  = m_JsonValue["sessionId"].asString();
    Msg.lastTime  = m_JsonValue["lastTime"].asString();
    Msg.aesKey  = m_JsonValue["aesKey"].asString();
    Msg.queryFrom  = m_JsonValue["queryFrom"].asString();
    Msg.product  = m_JsonValue["product"].asString();

    return true;
}
void CWebServiceMsg::Encode(const CIPC_checkUpdate_ACK            &Msg)
{
    Json::Value var;

    var["result"]   = Msg.result  ;
    var["reason"] = Msg.reason;
    var["homeIdStr"]   = Msg.homeIdStr  ;
    var["sessionId"] = Msg.sessionId;
    var["queryFrom"]   = Msg.queryFrom  ;
    var["cfgForce"] = Msg.cfgForce;
    var["cfgUrl"]   = Msg.cfgUrl  ;
    var["softForce"] = Msg.softForce;
    var["softUrl"]   = Msg.softUrl  ;
    var["md5"] = Msg.md5;
    m_JsonValue = var;

    setValid();
}
bool CWebServiceMsg::Decode(CIPC_checkUpdate_ACK &Msg)
{
    Msg.result    = m_JsonValue["result"].asString();
    Msg.reason  = m_JsonValue["reason"].asString();
    Msg.homeIdStr    = m_JsonValue["homeIdStr"].asString();
    Msg.sessionId  = m_JsonValue["sessionId"].asString();
    Msg.queryFrom    = m_JsonValue["queryFrom"].asString();
    Msg.cfgForce  = m_JsonValue["cfgForce"].asString();
    Msg.cfgUrl    = m_JsonValue["cfgUrl"].asString();
    Msg.softForce  = m_JsonValue["softForce"].asString();
    Msg.softUrl    = m_JsonValue["softUrl"].asString();
    Msg.md5  = m_JsonValue["md5"].asString();

    return true;
}

void CWebServiceMsg::Encode(const CIPC_notifyUpdate                   &Msg)
{
    Json::Value var;

    var["sessionId"]   = Msg.sessionId  ;
    var["homeIdStr"]   = Msg.homeIdStr  ;

    m_JsonValue = var;
    setValid();
}
bool CWebServiceMsg::Decode(CIPC_notifyUpdate &Msg)
{
    Msg.sessionId = m_JsonValue["sessionId"].asString();
    Msg.homeIdStr = m_JsonValue["homeIdStr"].asString();
    return true;
}
void CWebServiceMsg::Encode(const CIPC_notifyUpdate_ACK        &Msg)
{
    Json::Value var;

    var["result"]    = Msg.result;
    var["reason"]    = Msg.reason;

    m_JsonValue = var;

    setValid();
}

bool CWebServiceMsg::Decode(CIPC_notifyUpdate_ACK &Msg)
{
    Msg.result    = m_JsonValue["result"].asString();
    Msg.reason    = m_JsonValue["reason"].asString();

    return true;
}


