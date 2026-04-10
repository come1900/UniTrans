/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UniTransRestStack.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: UniTransRestStack.cpp 5884 2016-09-25 10:26:11Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-09-25 10:26:11  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "UniTransRestStack.h"
#include <assert.h>

CUniTransRestStack::CUniTransRestStack()
{    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";
}

CUniTransRestStack::~CUniTransRestStack()
{}

void CUniTransRestStack::Encode(const CAckMsgSimple &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CUniTransRestStack::Decode(CAckMsgSimple &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}

void CUniTransRestStack::Encode(const CSmsMsg_requestAuth                        &Msg)
{
    Json::Value var;

    var[UniMsg_Key_Account]   =    Msg.m_strAccount        ;// 设备编号
    var[UniMsg_Key_Password]  =   Msg.m_strPassword       ;// 访问授权码 通过编号+授权码直接访问
    var[UniMsg_Key_SessionID] =  Msg.m_strSessionID      ;// 授权用户名

    Json::Value SystemPieces;//store all pieces

    SystemPieces[UniMsg_Key_HostID]      = Msg.m_LocalSystemInfo.m_strHostID      ;
    SystemPieces[UniMsg_Key_HostAlias]   = Msg.m_LocalSystemInfo.m_strHostAlias   ;
    SystemPieces[UniMsg_Key_Serial]      = Msg.m_LocalSystemInfo.m_strSerial      ;
    SystemPieces[UniMsg_Key_Description] = Msg.m_LocalSystemInfo.m_strDescription ;
    SystemPieces[UniMsg_Key_Type]        = Msg.m_LocalSystemInfo.m_strType        ;
    SystemPieces[UniMsg_Key_Version]     = Msg.m_LocalSystemInfo.m_strVersion        ;
    SystemPieces[UniMsg_Key_Location]    = Msg.m_LocalSystemInfo.m_strLocation;
    SystemPieces[UniMsg_Key_Vendor]      = Msg.m_LocalSystemInfo.m_strVendor;
    SystemPieces[UniMsg_Key_Mac]         = Msg.m_LocalSystemInfo.m_strMac;
    SystemPieces[UniMsg_Key_LocalDate]   = Msg.m_LocalSystemInfo.m_strLocalDate   ;

    var[UniMsg_Key_System]               = SystemPieces;

    Json::Value pieces_all;//store all pieces
    for (unsigned int ii=0; ii<Msg.m_vLocalService.size(); ii++)
    {
        Json::Value pieces;//store all pieces

        pieces[UniMsg_Key_ServiceID]    = Msg.m_vLocalService[ii].m_strServiceID     ;
        pieces[UniMsg_Key_Description]  = Msg.m_vLocalService[ii].m_strDescription   ;
        pieces[UniMsg_Key_URI]          = Msg.m_vLocalService[ii].m_strURI           ;
        pieces[UniMsg_Key_Protocol]     = Msg.m_vLocalService[ii].m_strProtocol      ;

        pieces_all.append(pieces);
    }
    var[UniMsg_Key_Service] = pieces_all;

    m_JsonValue = var;

    setValid();
}

bool CUniTransRestStack::Decode(CSmsMsg_requestAuth                        &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strAccount    = m_JsonValue[UniMsg_Key_Account].asString()      ;// 
    Msg.m_strPassword   = m_JsonValue[UniMsg_Key_Password].asString()     ;// 访问授权码 通过编号+授权码直接访问
    Msg.m_strSessionID  = m_JsonValue[UniMsg_Key_SessionID].asString()    ;// 授权用户名

    const Json::Value SystemPieces = m_JsonValue[UniMsg_Key_System];

    Msg.m_LocalSystemInfo.m_strHostID      = SystemPieces[UniMsg_Key_HostID]      .asString();
    Msg.m_LocalSystemInfo.m_strHostAlias   = SystemPieces[UniMsg_Key_HostAlias]    .asString();
    Msg.m_LocalSystemInfo.m_strSerial      = SystemPieces[UniMsg_Key_Serial]      .asString();
    Msg.m_LocalSystemInfo.m_strDescription = SystemPieces[UniMsg_Key_Description] .asString();
    Msg.m_LocalSystemInfo.m_strType        = SystemPieces[UniMsg_Key_Type]        .asString();
    Msg.m_LocalSystemInfo.m_strVersion     = SystemPieces[UniMsg_Key_Version]     .asString();
    Msg.m_LocalSystemInfo.m_strLocation    = SystemPieces[UniMsg_Key_Location]    .asString();
    Msg.m_LocalSystemInfo.m_strVendor      = SystemPieces[UniMsg_Key_Vendor]      .asString();
    Msg.m_LocalSystemInfo.m_strMac         = SystemPieces[UniMsg_Key_Mac]         .asString();
    Msg.m_LocalSystemInfo.m_strLocalDate   = SystemPieces[UniMsg_Key_LocalDate]   .asString();

    const Json::Value arrayOperate = m_JsonValue[UniMsg_Key_Service];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CMsgService _operate;

        _operate.m_strServiceID   = arrayOperate[i][UniMsg_Key_ServiceID  ] .asString();
        _operate.m_strDescription = arrayOperate[i][UniMsg_Key_Description] .asString();
        _operate.m_strURI         = arrayOperate[i][UniMsg_Key_URI        ] .asString();
        _operate.m_strProtocol    = arrayOperate[i][UniMsg_Key_Protocol   ] .asString();

        Msg.m_vLocalService.push_back(_operate);
    }

    return true;
}

void CUniTransRestStack::Encode(const CSmsMsg_requestAuthACK                        &Msg)
{
    Json::Value var;

    var["SessionID"] =        Msg.m_strSessionID           ;// 授权用户名
    var["SessionTimeout"] =   Msg.m_strSessionTimeout    ;// 设备编号
    var[PssMsg_Key_Result]           = Msg.Result          ;// 结果
    var[PssMsg_Key_ResultMsg]        = Msg.Reason;// Result的参考文字描述

    Json::Value SystemPieces;//store all pieces

    SystemPieces["HostName"]    = Msg.m_LocalSystemInfo.m_strHostID   ;
    SystemPieces["Serial"]      = Msg.m_LocalSystemInfo.m_strSerial     ;
    SystemPieces["Description"] = Msg.m_LocalSystemInfo.m_strDescription;
    SystemPieces[UniMsg_Key_Type]        = Msg.m_LocalSystemInfo.m_strType       ;
    SystemPieces[UniMsg_Key_Version]        = Msg.m_LocalSystemInfo.m_strVersion       ;
    SystemPieces["LocalDate"]   = Msg.m_LocalSystemInfo.m_strLocalDate  ;
    var["System"]           = SystemPieces;

    Json::Value pieces_all;//store all pieces
    for (unsigned int ii=0; ii<Msg.m_vLocalService.size(); ii++)
    {
        Json::Value pieces;//store all pieces

        pieces["ServiceID"]    = Msg.m_vLocalService[ii].m_strServiceID     ;
        pieces["Description"]  = Msg.m_vLocalService[ii].m_strDescription   ;
        pieces["URI"]          = Msg.m_vLocalService[ii].m_strURI           ;
        //pieces["Port"]         = Msg.m_vLocalService[ii].m_strPort          ;
        pieces["Protocol"]     = Msg.m_vLocalService[ii].m_strProtocol      ;

        pieces_all.append(pieces);
    }
    var["Service"] = pieces_all;

    m_JsonValue = var;

    setValid();
}

bool CUniTransRestStack::Decode(CSmsMsg_requestAuthACK                        &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strSessionID      = m_JsonValue["SessionID"]      .asString(); // 授权用户名
    Msg.m_strSessionTimeout = m_JsonValue["SessionTimeout"] .asString(); // 设备编号
    Msg.Result         = m_JsonValue[PssMsg_Key_Result]         .asString(); // 结果
    Msg.Reason      = m_JsonValue[PssMsg_Key_ResultMsg]      .asString(); // Result的参考文字描述

    const Json::Value SystemPieces = m_JsonValue["System"];

    Msg.m_LocalSystemInfo.m_strHostID    = SystemPieces["HostName"]   .asString();
    Msg.m_LocalSystemInfo.m_strSerial      = SystemPieces["Serial"]     .asString();
    Msg.m_LocalSystemInfo.m_strDescription = SystemPieces["Description"].asString();
    Msg.m_LocalSystemInfo.m_strType        = SystemPieces[UniMsg_Key_Type]       .asString();
    Msg.m_LocalSystemInfo.m_strVersion        = SystemPieces[UniMsg_Key_Version]       .asString();
    Msg.m_LocalSystemInfo.m_strLocalDate   = SystemPieces["LocalDate"]  .asString();


    const Json::Value arrayOperate = m_JsonValue["Service"];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        CMsgService _operate;

        _operate.m_strServiceID   = arrayOperate[i]["ServiceID"]   .asString();
        _operate.m_strDescription = arrayOperate[i]["Description"] .asString();
        _operate.m_strURI         = arrayOperate[i]["URI"]         .asString();
        //_operate.m_strPort        = arrayOperate[i]["Port"]        .asString();
        _operate.m_strProtocol    = arrayOperate[i]["Protocol"]    .asString();

        Msg.m_vLocalService.push_back(_operate);
    }

    return true;
}

void CUniTransRestStack::Encode(const CUniMsg_reqCreateAppKey &Msg)
{
    Json::Value var;

    var[UniMsg_Key_Account]  = Msg.Account;
    var[UniMsg_Key_Password] = Msg.Password;
    var[UniMsg_Key_Service]  = Msg.Service;

    m_JsonValue = var;
    setValid();
}
bool CUniTransRestStack::Decode(CUniMsg_reqCreateAppKey &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Account  = m_JsonValue[UniMsg_Key_Account] .asString();
    Msg.Password = m_JsonValue[UniMsg_Key_Password].asString();
    Msg.Service  = m_JsonValue[UniMsg_Key_Service] .asString();

    return true;
}

void CUniTransRestStack::Encode(const CUniMsg_reqCreateAppKeyAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result]           = Msg.Result ; // 结果
    var[PssMsg_Key_ResultMsg]        = Msg.Reason ; // Result的参考文字描述

    var[UniMsg_Key_AppKey]    = Msg.AppKey   ;
    var[UniMsg_Key_AppSecret] = Msg.AppSecret;
    var[UniMsg_Key_Validity]  = Msg.Validity ;

    m_JsonValue = var;
    setValid();
}
bool CUniTransRestStack::Decode(CUniMsg_reqCreateAppKeyAck &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }
    Msg.Result    = m_JsonValue[PssMsg_Key_Result]    .asString() ;
    Msg.Reason    = m_JsonValue[PssMsg_Key_ResultMsg] .asString() ;

    Msg.AppKey    = m_JsonValue[UniMsg_Key_AppKey]    .asString() ;
    Msg.AppSecret = m_JsonValue[UniMsg_Key_AppSecret] .asString();
    Msg.Validity  = m_JsonValue[UniMsg_Key_Validity]  .asString();

    return true;
}

void CUniTransRestStack::Encode(const CUniMsg_reqGetSetGmLic &Msg)
{
    Json::Value var;

    var[UniMsg_Key_GLic]  = Msg.strGLic;

    m_JsonValue = var;
    setValid();
}
bool CUniTransRestStack::Decode(CUniMsg_reqGetSetGmLic &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.strGLic  = m_JsonValue[UniMsg_Key_GLic] .asString();

    return true;
}

void CUniTransRestStack::Encode(const CUniMsg_reqGetSetGmLicAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result]           = Msg.Result ; // 结果
    var[PssMsg_Key_ResultMsg]        = Msg.Reason ; // Result的参考文字描述

    var[UniMsg_Key_GLicVer]    = Msg.m_iGLicVer   ;

    if (!Msg.m_strllValidSeconds.empty())
        var[UniMsg_Key_ValidSeconds] = Msg.m_strllValidSeconds;

    if (!Msg.m_strttValidDate.empty())
        var[UniMsg_Key_ValidDate]  = Msg.m_strttValidDate ;

    if (!Msg.strGLic.empty())
        var[UniMsg_Key_GLic]  = Msg.strGLic;

    m_JsonValue = var;
    setValid();
}
bool CUniTransRestStack::Decode(CUniMsg_reqGetSetGmLicAck &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }
    Msg.Result    = m_JsonValue[PssMsg_Key_Result]    .asString() ;
    Msg.Reason    = m_JsonValue[PssMsg_Key_ResultMsg] .asString() ;

    Msg.m_iGLicVer    = m_JsonValue[UniMsg_Key_GLicVer]    .asUInt() ;
    Msg.m_strllValidSeconds    = m_JsonValue[UniMsg_Key_ValidSeconds]    .asString() ;
    Msg.m_strttValidDate = m_JsonValue[UniMsg_Key_ValidDate] .asString();
    Msg.strGLic  = m_JsonValue[UniMsg_Key_GLic]  .asString();

    return true;
}

void CUniTransRestStack::Encode(const CUniMsg_reqConfigModify &Msg)
{
    Json::Value var;

    var[UniMsg_Key_User]    = Msg.User   ;
    var[UniMsg_Key_Section] = Msg.Section;
    var[UniMsg_Key_Name]    = Msg.Name   ;
    var[UniMsg_Key_Value]   = Msg.Value  ;

    m_JsonValue = var;
    setValid();
}
bool CUniTransRestStack::Decode(CUniMsg_reqConfigModify &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.User    = m_JsonValue[UniMsg_Key_User]    .asString();
    Msg.Section = m_JsonValue[UniMsg_Key_Section] .asString();
    Msg.Name    = m_JsonValue[UniMsg_Key_Name]    .asString();
    Msg.Value   = m_JsonValue[UniMsg_Key_Value]   .asString();

    return true;
}

bool CUniTransRestStack::Parse( const std::string &document)
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

bool CUniTransRestStack::ToString( std::string &document)
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

const std::string &CUniTransRestStack::ToString()
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

bool CUniTransRestStack::isValid()
{
    return m_bDataValid;
}

void CUniTransRestStack::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CUniTransRestStack::getError() const
{
    return m_iError;
}

const std::string &CUniTransRestStack::getErrorMessages() const
{
    return m_strError;
}

const Json::Value &CUniTransRestStack::GetValue() const
{
    return m_JsonValue;
}


