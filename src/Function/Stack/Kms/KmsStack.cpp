/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * KmsStack.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: KmsStack.cpp 5884 2019-02-27 05:26:20Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-02-27 05:26:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "KmsStack.h"

#include <assert.h>

CKmsStack::CKmsStack()
{}

CKmsStack::~CKmsStack()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

void CKmsStack::Encode(const CAckMsgSimple &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CKmsStack::Decode(CAckMsgSimple &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CKmsStack::Encode(const CSmsMsg_requestAuth &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CKmsStack::Decode(CSmsMsg_requestAuth &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CKmsStack::Encode(const CSmsMsg_requestAuthACK &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CKmsStack::Decode(CSmsMsg_requestAuthACK &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CKmsStack::Encode(const CUniMsg_reqCreateAppKey &Msg)
{
    CUniTransRestStack::Encode(Msg);
}

bool CKmsStack::Decode(CUniMsg_reqCreateAppKey &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CKmsStack::Encode(const CUniMsg_reqCreateAppKeyAck &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CKmsStack::Decode(CUniMsg_reqCreateAppKeyAck &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CKmsStack::Encode(const CKmsMsg_requestMsgGenerateKeypair &Msg)
{
    Json::Value var;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgGenerateKeypair &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();

    return true;
}
void CKmsStack::Encode(const CKmsMsg_requestMsgGenerateKeypairAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;
    var[KmsMsg_Key_PublicKey  ] = Msg.m_strPublicKey  ;
    var[KmsMsg_Key_PrivateKey  ] = Msg.m_strPrivateKey  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgGenerateKeypairAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();
    Msg.m_strPublicKey   = m_JsonValue[KmsMsg_Key_PublicKey  ].asString();
    Msg.m_strPrivateKey   = m_JsonValue[KmsMsg_Key_PrivateKey  ].asString();

    return true;
}

void CKmsStack::Encode(const CKmsMsg_requestMsgGenerateKey &Msg)
{
    Json::Value var;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgGenerateKey &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();

    return true;
}
void CKmsStack::Encode(const CKmsMsg_requestMsgGenerateKeyAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;
    var[KmsMsg_Key_Key  ] = Msg.m_strKey  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgGenerateKeyAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();
    Msg.m_strKey   = m_JsonValue[KmsMsg_Key_Key  ].asString();

    return true;
}

void CKmsStack::Encode(const CKmsMsg_requestMsgEncryptAsymmetric &Msg)
{
    Json::Value var;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;
    var[KmsMsg_Key_PublicKey  ] = Msg.m_strPublicKey  ;
    var[KmsMsg_Key_PlainText  ] = Msg.m_strPlainText  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgEncryptAsymmetric &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();
    Msg.m_strPublicKey   = m_JsonValue[KmsMsg_Key_PublicKey  ].asString();
    Msg.m_strPlainText   = m_JsonValue[KmsMsg_Key_PlainText  ].asString();

    return true;
}

void CKmsStack::Encode(const CKmsMsg_requestMsgEncryptAsymmetricAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;
    var[KmsMsg_Key_CipherText  ] = Msg.m_strCipherText  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgEncryptAsymmetricAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();
    Msg.m_strCipherText   = m_JsonValue[KmsMsg_Key_CipherText  ].asString();

    return true;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


void CKmsStack::Encode(const CKmsMsg_requestMsgDecryptAsymmetric &Msg)
{
    Json::Value var;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;
    var[KmsMsg_Key_PrivateKey  ] = Msg.m_strPrivateKey  ;
    var[KmsMsg_Key_CipherText  ] = Msg.m_strCipherText  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgDecryptAsymmetric &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();
    Msg.m_strPrivateKey   = m_JsonValue[KmsMsg_Key_PrivateKey  ].asString();
    Msg.m_strCipherText   = m_JsonValue[KmsMsg_Key_CipherText  ].asString();

    return true;
}

void CKmsStack::Encode(const CKmsMsg_requestMsgDecryptAsymmetricAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[KmsMsg_Key_KmsID  ] = Msg.m_strKmsID  ;
    var[KmsMsg_Key_PlainText  ] = Msg.m_strPlainText  ;

    m_JsonValue = var;
    setValid();
}

bool CKmsStack::Decode(CKmsMsg_requestMsgDecryptAsymmetricAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strKmsID   = m_JsonValue[KmsMsg_Key_KmsID  ].asString();
    Msg.m_strPlainText   = m_JsonValue[KmsMsg_Key_PlainText  ].asString();

    return true;
}



