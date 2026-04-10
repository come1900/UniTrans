/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SmsStack.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SmsStack.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     SmsMsg ---- Intelligent system communication protocol
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *     2013-12-02 10:21:25 WuJunjie 549 all decode interface ready
 * 
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "SmsStack.h"

#include <assert.h>

CSmsStack::CSmsStack()
{
}

CSmsStack::~CSmsStack()
{
}

void CSmsStack::Encode(const CAckMsgSimple &Msg)
{
	CUniTransRestStack::Encode(Msg);
}

bool CSmsStack::Decode(CAckMsgSimple &Msg)
{
		return CUniTransRestStack::Decode(Msg);
}

void CSmsStack::Encode(const CAckMsgSession &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var["SessionId"] = Msg.SessionId;
    m_JsonValue = var;

    setValid();
}

bool CSmsStack::Decode(CAckMsgSession &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result    = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason    = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.SessionId = m_JsonValue["SessionId"].asString();

    return true;
}

void CSmsStack::Encode(const CSmsMsg_RegisterReq                                &Msg)
{
    Json::Value var;

    var["ProductID"]    = Msg.ProductID;
    var["AuthCode"]  = Msg.AuthCode;
    var["AuthName"]     = Msg.AuthName;
    var["Password"] = Msg.Password;

    var["LocalIp"] = Msg.m_LocalStatus.LocalIp;
    var["LocalDateTime"] = Msg.m_LocalStatus.LocalDateTime;

    m_JsonValue = var;

    setValid();
}

bool CSmsStack::Decode(CSmsMsg_RegisterReq &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.ProductID    = m_JsonValue["ProductID"]   .asString();
    Msg.AuthCode  = m_JsonValue["AuthCode"] .asString();
    Msg.AuthName     = m_JsonValue["AuthName"]    .asString();
    Msg.Password = m_JsonValue["Password"].asString();

    Msg.m_LocalStatus.LocalIp = m_JsonValue["LocalIp"].asString();
    Msg.m_LocalStatus.LocalDateTime = m_JsonValue["LocalDateTime"].asString();
    return true;
}

void CSmsStack::Encode(const CSmsMsg_RegisterAck                        &Msg)
{
    Json::Value var;

    var["session"] = Msg.m_Session;
    var[PssMsg_Key_Result] = Msg.m_ackInfo.Result;
    var[PssMsg_Key_ResultMsg] = Msg.m_ackInfo.Reason;

    Json::Value operate_all;//store all pieces
    for (unsigned int ii=0; ii<Msg.m_Service.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate["ServiceName"]     = Msg.m_Service[ii].ServiceName       ;
        operate["ServiceLocation"]  = Msg.m_Service[ii].ServiceLocation    ;

        operate_all.append(operate);
    }
    var["Service"] = operate_all;

    m_JsonValue = var;
    setValid();
}

bool CSmsStack::Decode(CSmsMsg_RegisterAck &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_ackInfo.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.m_ackInfo.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.m_Session = m_JsonValue["session"].asString();

    const Json::Value arrayOperate = m_JsonValue["Service"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CSmsMsg_ServiceAgent _operate;
        _operate.ServiceName       = arrayOperate[i]["ServiceName"]     .asString();
        _operate.ServiceLocation    = arrayOperate[i]["ServiceLocation"]  .asString();

        Msg.m_Service.push_back(_operate);
    }

    return true;
}

void CSmsStack::Encode(const CSmsMsg_requestAuth &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CSmsStack::Decode(CSmsMsg_requestAuth &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CSmsStack::Encode(const CSmsMsg_requestAuthACK &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CSmsStack::Decode(CSmsMsg_requestAuthACK &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

// 转发
void 	CSmsStack::Encode(const CSmsMsg_requestSend                        &Msg)
{
    Json::Value var;
    //unsigned int ii;

    var["SessionID"] =        Msg.m_strSessionID           ;// 授权用户名

    Json::Value SystemPieces;//store all pieces

    SystemPieces["HostName"]    = Msg.m_LocalSystemInfo.m_strHostID   ;
    SystemPieces["Serial"]      = Msg.m_LocalSystemInfo.m_strSerial     ;
    SystemPieces["Description"] = Msg.m_LocalSystemInfo.m_strDescription;
    SystemPieces["Type"]        = Msg.m_LocalSystemInfo.m_strType       ;
    SystemPieces["LocalDate"]   = Msg.m_LocalSystemInfo.m_strLocalDate  ;
    var["System"]               = SystemPieces;

    Json::Value piecesFrom;//store all pieces
    piecesFrom["ServiceID"]    = Msg.m_From.m_strServiceID     ;
    piecesFrom["Description"]  = Msg.m_From.m_strDescription   ;
    piecesFrom["URI"]          = Msg.m_From.m_strURI           ;
    //piecesFrom["Port"]         = Msg.m_From.m_strPort          ;
    piecesFrom["Protocol"]     = Msg.m_From.m_strProtocol      ;
    var["From"]                = piecesFrom;

    Json::Value piecesDest;//store all pieces
    piecesDest["ServiceID"]    = Msg.m_Dest.m_strServiceID     ;
    piecesDest["Description"]  = Msg.m_Dest.m_strDescription   ;
    piecesDest["URI"]          = Msg.m_Dest.m_strURI           ;
    //piecesDest["Port"]         = Msg.m_Dest.m_strPort          ;
    piecesDest["Protocol"]     = Msg.m_Dest.m_strProtocol      ;
    var["Dest"]                = piecesDest;

    Json::Value piecesData;//store all pieces
    piecesData["Type"]    = Msg.m_Data.m_strType    ;
    piecesData["Encode"]  = Msg.m_Data.m_strEncode  ;
    piecesData["Len"]     = Msg.m_Data.m_strLen     ;
    piecesData["Value"]   = Msg.m_Data.m_strValue   ;
    var["Msg"]            = piecesData;

    m_JsonValue = var;

    setValid();
}

bool CSmsStack::Decode(CSmsMsg_requestSend                        &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strSessionID      = m_JsonValue["SessionID"]      .asString(); // 授权用户名

    const Json::Value SystemPieces = m_JsonValue["System"];

    Msg.m_LocalSystemInfo.m_strHostID    = SystemPieces["HostName"]   .asString();
    Msg.m_LocalSystemInfo.m_strSerial      = SystemPieces["Serial"]     .asString();
    Msg.m_LocalSystemInfo.m_strDescription = SystemPieces["Description"].asString();
    Msg.m_LocalSystemInfo.m_strType        = SystemPieces["Type"]       .asString();
    Msg.m_LocalSystemInfo.m_strLocalDate   = SystemPieces["LocalDate"]  .asString();

    const Json::Value FromPieces = m_JsonValue["From"];
    Msg.m_From.m_strServiceID    = FromPieces["ServiceID"]   .asString();
    Msg.m_From.m_strDescription  = FromPieces["Description"] .asString();
    Msg.m_From.m_strURI          = FromPieces["URI"]         .asString();
    //Msg.m_From.m_strPort         = FromPieces["Port"]        .asString();
    Msg.m_From.m_strProtocol     = FromPieces["Protocol"]    .asString();

    const Json::Value DestPieces = m_JsonValue["Dest"];
    Msg.m_Dest.m_strServiceID    = DestPieces["ServiceID"]   .asString();
    Msg.m_Dest.m_strDescription  = DestPieces["Description"] .asString();
    Msg.m_Dest.m_strURI          = DestPieces["URI"]         .asString();
    //Msg.m_Dest.m_strPort         = DestPieces["Port"]        .asString();
    Msg.m_Dest.m_strProtocol     = DestPieces["Protocol"]    .asString();

    const Json::Value DataPieces = m_JsonValue["Msg"];
    Msg.m_Data.m_strType         = DataPieces["Type"]        .asString();
    Msg.m_Data.m_strEncode       = DataPieces["Encode"]      .asString();
    Msg.m_Data.m_strLen          = DataPieces["Len"]         .asString();
    Msg.m_Data.m_strValue        = DataPieces["Value"]       .asString();

    return true;
}

// 转发 ACK
void 	CSmsStack::Encode(const CSmsMsg_requestSendACK                        &Msg)
{
    Json::Value var;
    //unsigned int ii;

    var["SessionID"]            = Msg.m_strSessionID           ;// 授权用户名
    var[PssMsg_Key_Result]               = Msg.Result          ;// 结果
    var[PssMsg_Key_ResultMsg]            = Msg.Reason           ;// Result的参考文字描述

    Json::Value SystemPieces;//store all pieces

    SystemPieces["HostName"]    = Msg.m_LocalSystemInfo.m_strHostID   ;
    SystemPieces["Serial"]      = Msg.m_LocalSystemInfo.m_strSerial     ;
    SystemPieces["Description"] = Msg.m_LocalSystemInfo.m_strDescription;
    SystemPieces["Type"]        = Msg.m_LocalSystemInfo.m_strType       ;
    SystemPieces["LocalDate"]   = Msg.m_LocalSystemInfo.m_strLocalDate  ;
    var["System"]               = SystemPieces;


    Json::Value piecesFrom;//store all pieces
    piecesFrom["ServiceID"]     = Msg.m_From.m_strServiceID     ;
    piecesFrom["Description"]   = Msg.m_From.m_strDescription   ;
    piecesFrom["URI"]           = Msg.m_From.m_strURI           ;
    //piecesFrom["Port"]          = Msg.m_From.m_strPort          ;
    piecesFrom["Protocol"]      = Msg.m_From.m_strProtocol      ;
    var["From"]                 = piecesFrom;

    Json::Value piecesDest;//store all pieces
    piecesDest["ServiceID"]     = Msg.m_Dest.m_strServiceID     ;
    piecesDest["Description"]   = Msg.m_Dest.m_strDescription   ;
    piecesDest["URI"]           = Msg.m_Dest.m_strURI           ;
    //piecesDest["Port"]          = Msg.m_Dest.m_strPort          ;
    piecesDest["Protocol"]      = Msg.m_Dest.m_strProtocol      ;
    var["Dest"]                 = piecesDest;

    m_JsonValue = var;

    setValid();
}

bool CSmsStack::Decode(CSmsMsg_requestSendACK                        &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strSessionID             = m_JsonValue["SessionID"]   .asString(); // 授权用户名
    //Msg.m_strResult                = m_JsonValue["Result"]      .asString(); // 结果
    //Msg.m_strResultMsg             = m_JsonValue["ResultMsg"]   .asString(); // Result的参考文字描述
    Msg.Result         = m_JsonValue[PssMsg_Key_Result]         .asString(); // 结果
    Msg.Reason      = m_JsonValue[PssMsg_Key_ResultMsg]      .asString(); // Result的参考文字描述

    const Json::Value SystemPieces = m_JsonValue["System"];

    Msg.m_LocalSystemInfo.m_strHostID    = SystemPieces["HostName"]   .asString();
    Msg.m_LocalSystemInfo.m_strSerial      = SystemPieces["Serial"]     .asString();
    Msg.m_LocalSystemInfo.m_strDescription = SystemPieces["Description"].asString();
    Msg.m_LocalSystemInfo.m_strType        = SystemPieces["Type"]       .asString();
    Msg.m_LocalSystemInfo.m_strLocalDate   = SystemPieces["LocalDate"]  .asString();


    const Json::Value FromPieces = m_JsonValue["From"];
    Msg.m_From.m_strServiceID    = FromPieces["ServiceID"]   .asString();
    Msg.m_From.m_strDescription  = FromPieces["Description"] .asString();
    Msg.m_From.m_strURI          = FromPieces["URI"]         .asString();
    //Msg.m_From.m_strPort         = FromPieces["Port"]        .asString();
    Msg.m_From.m_strProtocol     = FromPieces["Protocol"]    .asString();

    const Json::Value DestPieces = m_JsonValue["Dest"];
    Msg.m_Dest.m_strServiceID    = DestPieces["ServiceID"]   .asString();
    Msg.m_Dest.m_strDescription  = DestPieces["Description"] .asString();
    Msg.m_Dest.m_strURI          = DestPieces["URI"]         .asString();
    //Msg.m_Dest.m_strPort         = DestPieces["Port"]        .asString();
    Msg.m_Dest.m_strProtocol     = DestPieces["Protocol"]    .asString();

    return true;
}
