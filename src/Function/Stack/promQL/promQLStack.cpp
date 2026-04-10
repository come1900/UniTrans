/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * promQLStack.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: promQLStack.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     promQLMsg ---- Intelligent system communication protocol
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *     2013-12-02 10:21:25 WuJunjie 549 all decode interface ready
 * 
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <assert.h>

//#define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

#include "promQLStack.h"

void CpromQLStack::Encode(const CMmtMsgPlayUrlList          &Msg)
{
    Json::Value var;

    var[PromQlMsg_Key_result]  = Msg.Result;

    Json::Value __valueUrlList;

    for (std::list<CMmtMsgPlayUrl>::const_iterator ciUrl = Msg.UrlList.begin(); ciUrl != Msg.UrlList.end(); ++ciUrl)
    {
        Json::Value __valueUrl;

        __valueUrl[PromQlMsg_CMmtMsgPlayUrl_Key_Url]  = ciUrl->Url;
        __valueUrl[PromQlMsg_CMmtMsgPlayUrl_Key_Type]  = ciUrl->Type;

        __valueUrlList.append(__valueUrl);
    }

    var[PromQlMsg_Key_UrlList]  = __valueUrlList;
    m_JsonValue = var;
    setValid();
}

bool CpromQLStack::Decode(CMmtMsgPlayUrlList                &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PromQlMsg_Key_result].asInt();

    const Json::Value __valueUrlList = m_JsonValue[PromQlMsg_Key_UrlList];

    for (size_t i = 0; i < __valueUrlList.size(); i++)
    {
        CMmtMsgPlayUrl playUrlMsg;
    
        playUrlMsg.Url = __valueUrlList[i][PromQlMsg_CMmtMsgPlayUrl_Key_Url].asString();
        playUrlMsg.Type = __valueUrlList[i][PromQlMsg_CMmtMsgPlayUrl_Key_Type].asString();

        Msg.UrlList.push_back(playUrlMsg);
    }

    return true;
}

void CpromQLStack::Encode(const CMmtMsgValuesIpcamDialing          &Msg)
{
    Json::Value var;

    var[MmtMsg_Key_MmtSid]  = Msg.m_strMmtSid;
    var[MmtMsg_Key_Items]  = Msg.Items;

    m_JsonValue = var;
    setValid();
}
bool CpromQLStack::Decode(CMmtMsgValuesIpcamDialing                &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMmtSid = m_JsonValue[MmtMsg_Key_MmtSid].asString();
    Msg.Items = m_JsonValue[MmtMsg_Key_Items].asInt();
    
    return true;
}

void CpromQLStack::Encode(const CMmtMsgValuesIpcamDialingAck          &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var[MmtMsg_Key_Items] = Msg.Items;
    var[PromQlMsg_Key_RemainingItems] = Msg.RemainingItems;

    Json::Value __piecesValuesIpcamDialing; //store all pieces

    for (std::list<CMmtMsgValueCamDialing>::const_iterator ciValues = Msg.ValuesIpcamDialing.begin(); ciValues != Msg.ValuesIpcamDialing.end(); ++ciValues)
    {
        Json::Value varValuesIpcamDialing;

        varValuesIpcamDialing[PromQlMsg_Key_DevId] = ciValues->DevId;

        Json::Value __pieces; //store all pieces

        for (std::list<CDialingResult>::const_iterator ci = ciValues->DialingResults.begin(); ci != ciValues->DialingResults.end(); ++ci)
        {
            Json::Value __piece;
            __piece[PromQlMsg_Key_Result] = ci->m_strResult;
            __piece[PromQlMsg_Key_ResultMsg] = ci->m_strResultMsg;
            __piece[PromQlMsg_Key_Time] = ci->m_strTime;
            __pieces.append(__piece);
        }

        varValuesIpcamDialing[PromQlMsg_Key_DialingResults] = __pieces;

        __piecesValuesIpcamDialing.append(varValuesIpcamDialing);
    }

    var[PromQlMsg_Key_ValuesIpcamDialing] = __piecesValuesIpcamDialing;

    m_JsonValue = var;
    setValid();
}

bool CpromQLStack::Decode(CMmtMsgValuesIpcamDialingAck                &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.Items = m_JsonValue[MmtMsg_Key_Items].asInt();
    Msg.RemainingItems = m_JsonValue[PromQlMsg_Key_RemainingItems].asInt();
    const Json::Value __valueDevDialing = m_JsonValue[PromQlMsg_Key_ValuesIpcamDialing];

    for (size_t i = 0; i < __valueDevDialing.size(); i++)
    {
        CMmtMsgValueCamDialing dialingMsg;
        dialingMsg.DevId = __valueDevDialing[i][PromQlMsg_Key_DevId].asString();

        if (false == __valueDevDialing[i].isMember(PromQlMsg_Key_DialingResults))
            return false;
        const Json::Value __result = __valueDevDialing[i][PromQlMsg_Key_DialingResults];

        for (size_t i = 0; i < __result.size(); i++)
        {
            CDialingResult __dr;

            __dr.m_strResult = __result[i][PromQlMsg_Key_Result].asString();
            __dr.m_strResultMsg = __result[i][PromQlMsg_Key_ResultMsg].asString();
            __dr.m_strTime = __result[i][PromQlMsg_Key_Time].asDouble();

            dialingMsg.DialingResults.push_back(__dr);
        }

        Msg.ValuesIpcamDialing.push_back(dialingMsg);
    }

    return true;
}

void CpromQLStack::Encode(const CMsgMmtReqValueCamDialingAck          &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PromQlMsg_Key_DevId] = Msg.DevId;

    Json::Value __pieces;//store all pieces
    //for (size_t ii=0; ii <Msg.m_vHost.size(); ii++)
    for (std::list<CDialingResult>::const_iterator ci = Msg.DialingResults.begin() ; ci != Msg.DialingResults.end() ; ++ci)
    {
        Json::Value __piece;
        __piece[PromQlMsg_Key_Result] = ci->m_strResult;
        __piece[PromQlMsg_Key_ResultMsg] = ci->m_strResultMsg;
        __piece[PromQlMsg_Key_Time] = ci->m_strTime;
        __pieces.append(__piece);
    }

    var[PromQlMsg_Key_DialingResults] = __pieces;

    m_JsonValue = var;
    setValid();
}
bool CpromQLStack::Decode(CMsgMmtReqValueCamDialingAck                &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.DevId = m_JsonValue[PromQlMsg_Key_DevId].asString();

    if (false == m_JsonValue.isMember(PromQlMsg_Key_DialingResults))
        return false;
    const Json::Value __result = m_JsonValue[PromQlMsg_Key_DialingResults];

    for (size_t i = 0; i < __result.size(); i++)
    {
        CDialingResult __dr;

        __dr.m_strResult = __result[i][PromQlMsg_Key_Result].asString();
        __dr.m_strResultMsg = __result[i][PromQlMsg_Key_ResultMsg].asString();
        __dr.m_strTime = __result[i][PromQlMsg_Key_Time].asDouble();

        Msg.DialingResults.push_back(__dr);
    }

    return true;
}

void CpromQLStack::Encode(const CMsgMmtReqValueCamDialing          &Msg)
{
    Json::Value var;

    var[PromQlMsg_Key_DevId]  = Msg.DevId;

    m_JsonValue = var;
    setValid();
}
bool CpromQLStack::Decode(CMsgMmtReqValueCamDialing                &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.DevId = m_JsonValue[PromQlMsg_Key_DevId].asString();
    
    return true;
}

void CpromQLStack::Encode(const CMmtMsgCamDialing &Msg)
{
    Json::Value var;
    var[PromQlMsg_Key_DevId] = Msg.DevId;

    Json::Value __pieceMediaSource;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_Type       ] = Msg.MediaSource.iType      ;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_Account    ] = Msg.MediaSource.Account    ;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_Passwd     ] = Msg.MediaSource.Passwd     ;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_Channel    ] = Msg.MediaSource.Channel    ;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_Subtype    ] = Msg.MediaSource.Subtype    ;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_PlayUrl    ] = Msg.MediaSource.PlayUrl    ;
    __pieceMediaSource[PromQlMsg_Key_MediaSource_Description] = Msg.MediaSource.Description;
    var[PromQlMsg_Key_MediaSource] = __pieceMediaSource;

    Json::Value __pieceTask;
    __pieceTask[PromQlMsg_Key_Task_Priority] = Msg.Task.Priority;
    __pieceTask[PromQlMsg_Key_Task_Period  ] = Msg.Task.Period  ;
    __pieceTask[PromQlMsg_Key_Task_Times   ] = Msg.Task.Times   ;
    var[PromQlMsg_Key_Task] = __pieceTask;

    m_JsonValue = var;
    setValid();
}

bool CpromQLStack::Decode(CMmtMsgCamDialing &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if (!(m_JsonValue.type() == Json::objectValue))
    {
        return false;
    }

    Msg.DevId = m_JsonValue[PromQlMsg_Key_DevId].asString();

    const Json::Value valueMediaSource = m_JsonValue[PromQlMsg_Key_MediaSource];
    Msg.MediaSource.iType          = valueMediaSource[PromQlMsg_Key_MediaSource_Type       ]   .asInt();
    Msg.MediaSource.Account        = valueMediaSource[PromQlMsg_Key_MediaSource_Account    ]   .asString();
    Msg.MediaSource.Passwd         = valueMediaSource[PromQlMsg_Key_MediaSource_Passwd     ]   .asString();
    Msg.MediaSource.Channel        = valueMediaSource[PromQlMsg_Key_MediaSource_Channel    ]   .asString();
    Msg.MediaSource.Subtype        = valueMediaSource[PromQlMsg_Key_MediaSource_Subtype    ]   .asString();
    Msg.MediaSource.PlayUrl        = valueMediaSource[PromQlMsg_Key_MediaSource_PlayUrl    ]   .asString();
    Msg.MediaSource.Description    = valueMediaSource[PromQlMsg_Key_MediaSource_Description]   .asString();

    const Json::Value valueTask = m_JsonValue[PromQlMsg_Key_Task];
    Msg.Task.Priority           = valueTask[PromQlMsg_Key_Task_Priority     ]   .asInt();
    Msg.Task.Period             = valueTask[PromQlMsg_Key_Task_Period       ]   .asInt();
    Msg.Task.Times              = valueTask[PromQlMsg_Key_Task_Times        ]   .asInt();

    return true;
}

void CpromQLStack::Encode(const CAckMsgSimple &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CpromQLStack::Decode(CAckMsgSimple &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CpromQLStack::Encode(const CMsgPromQlValue &Msg, const std::string strValueType)
{
    Json::Value var;
    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PromQlMsg_Key_TotalItems]  = (unsigned int)(Msg.m_vHost.size());//= Msg.TotalItems
    Json::Value __pieces;//store all pieces

    for (size_t ii=0; ii <Msg.m_vHost.size(); ii++)
    {
        Json::Value __piece;
        __piece[PromQlMsg_Key_Host] = Msg.m_vHost[ii].m_strHost;
        __piece[PromQlMsg_Key_Value] = Msg.m_vHost[ii].m_strValue;
        __piece[PromQlMsg_Key_Time] = Msg.m_vHost[ii].m_strTime;
        __pieces.append(__piece);
    }
    
    var[strValueType] = __pieces;

    m_JsonValue = var;
    setValid();
}
bool CpromQLStack::Decode(CMsgPromQlValue &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if (!(m_JsonValue.type() == Json::objectValue))
    {
        return false;
    }

    if (false == m_JsonValue.isMember("status"))
        return false;
    if (m_JsonValue["status"].asString() != "success")
        return false;

    if (false == m_JsonValue.isMember("data"))
        return false;
    const Json::Value __data = m_JsonValue["data"];

    if (false == __data.isMember("result"))
        return false;
    const Json::Value __result = __data["result"];

    for (size_t i = 0; i < __result.size(); i++)
    {

        if (false == __result[i].isMember("metric"))
            continue;
        const Json::Value __metric = __result[i]["metric"];

        CHostValues __hv;
        __hv.m_strHost = __metric["instance"].asString();

        const Json::Value __value = __result[i]["value"];

        if (__value.type() == Json::arrayValue && __value.size() == 2)
        {
            size_t iii = 0;
            __hv.m_strTime = __value[iii++].asDouble();

            __hv.m_strValue = __value[iii++].asString();
        }

        if (__hv.bValid())
            Msg.m_vHost.push_back(__hv);
    }

    Msg.iTotalItems = Msg.m_vHost.size();

    return true;
}

void CpromQLStack::Encode(const CMsgMmt_requestMsg          &Msg)
{
    Json::Value var;

    var[MmtMsg_Key_MmtSid]  = Msg.m_strMmtSid;

    m_JsonValue = var;
    setValid();
}
bool CpromQLStack::Decode(CMsgMmt_requestMsg                &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMmtSid = m_JsonValue[MmtMsg_Key_MmtSid].asString();
    
    return true;
}

CpromQLStack::CpromQLStack()
{
}

CpromQLStack::~CpromQLStack()
{
}
