/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PqStack.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PqStack.cpp 5884 2018-03-09 03:22:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-09 03:22:15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "PqStack.h"

#include <assert.h>

CPqStack::CPqStack()
{}

CPqStack::~CPqStack()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

void CPqStack::Encode(const CAckMsgSimple &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPqStack::Decode(CAckMsgSimple &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CPqStack::Encode(const CSmsMsg_requestAuth &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPqStack::Decode(CSmsMsg_requestAuth &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CPqStack::Encode(const CSmsMsg_requestAuthACK &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPqStack::Decode(CSmsMsg_requestAuthACK &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CPqStack::Encode(const CUniMsg_reqCreateAppKey &Msg)
{
    CUniTransRestStack::Encode(Msg);
}

bool CPqStack::Decode(CUniMsg_reqCreateAppKey &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPqStack::Encode(const CUniMsg_reqCreateAppKeyAck &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPqStack::Decode(CUniMsg_reqCreateAppKeyAck &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPqStack::Encode(const CStreamCapabilityMsg &Msg)
{
    Json::Value var;

    //var[StreamCapabilityMsg_Key_BandwidthIn       ] = Msg.BandwidthIn  ;
    //var[StreamCapabilityMsg_Key_BandwidthOut      ] = Msg.BandwidthOut ;
    var[StreamCapabilityMsg_Key_CpuMark           ] = Msg.CpuMark      ;
    //var[StreamCapabilityMsg_Key_Memory            ] = Msg.Memory       ;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CStreamCapabilityMsg &Msg)
{
    if(!(m_JsonValue.type() == Json::objectValue))
    {
        return false;
    }

    //Msg.BandwidthIn        = m_JsonValue[StreamCapabilityMsg_Key_BandwidthIn       ].asInt();
    //Msg.BandwidthOut       = m_JsonValue[StreamCapabilityMsg_Key_BandwidthOut      ].asInt();
    Msg.CpuMark            = m_JsonValue[StreamCapabilityMsg_Key_CpuMark           ].asInt();
    //Msg.Memory             = m_JsonValue[StreamCapabilityMsg_Key_Memory            ].asInt();

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsg &Msg)
{
    Json::Value var;

    var[PqMsg_Key_Session] = Msg.m_strSession;
    var[PqMsg_Key_DevID  ] = Msg.m_strDevID  ;
    var[PqMsg_Key_MsgBody] = Msg.m_strMsgBody;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsg &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strSession = m_JsonValue[PqMsg_Key_Session].asString();
    Msg.m_strDevID   = m_JsonValue[PqMsg_Key_DevID  ].asString();
    Msg.m_strMsgBody = m_JsonValue[PqMsg_Key_MsgBody].asString();

    return true;
}

//{ "DevID" : "812A930E4BC", "MsgBody" : "MsgBody", "Session" : "6f61dfe3" }
void CPqStack::Encode(const CPqMsg_requestMsgAck &Msg)
{
    Json::Value var;

    var[PqMsg_Key_Session] = Msg.m_strSession;
    var[PqMsg_Key_DevID  ] = Msg.m_strDevID  ;
    var[PqMsg_Key_MsgBody] = Msg.m_strMsgBody;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strSession = m_JsonValue[PqMsg_Key_Session].asString();
    Msg.m_strDevID   = m_JsonValue[PqMsg_Key_DevID  ].asString();
    Msg.m_strMsgBody = m_JsonValue[PqMsg_Key_MsgBody].asString();

    return true;
}

//{ "Channel" : "1", "DevID" : "812A930E4BC", "Type" : "1", "Value" : "1" }
void CPqStack::Encode(const CPqMsg_requestMsgPub_IO_Out &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID  ] = Msg.m_strDevID  ;
    var[PqMsg_Key_Channel] = Msg.m_strChannel;
    var[PqMsg_Key_Method ] = Msg.m_strMethod;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgPub_IO_Out &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID   = m_JsonValue[PqMsg_Key_DevID  ].asString();
    Msg.m_strChannel = m_JsonValue[PqMsg_Key_Channel].asString();
    Msg.m_strMethod  = m_JsonValue[PqMsg_Key_Method ].asString();

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgShareHID &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;
    var[PqMsg_Key_ShareHID_IFNo] = Msg.m_strIFNo;
    var[PqMsg_Key_ShareHID_CmdType] = Msg.m_strCmdType;
    var[PqMsg_Key_ShareHID_UsualKey] = Msg.m_strUsualKey;
    var[PqMsg_Key_ShareHID_ControlKey] = Msg.m_strControlKey;

    var[PqMsg_Key_ShareHID_MouseKey		    ] = Msg.m_strMouseKey		;
    var[PqMsg_Key_ShareHID_MouseXMove		] = Msg.m_strMouseXMove		;
    var[PqMsg_Key_ShareHID_MouseYMove		] = Msg.m_strMouseYMove		;
    var[PqMsg_Key_ShareHID_MouseRollerMove  ] = Msg.m_strMouseRollerMove;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgShareHID &Msg)
{
    if (!(m_JsonValue.type() == Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();
    Msg.m_strIFNo = m_JsonValue[PqMsg_Key_ShareHID_IFNo].asString();
    Msg.m_strCmdType = m_JsonValue[PqMsg_Key_ShareHID_CmdType].asString();
    Msg.m_strUsualKey = m_JsonValue[PqMsg_Key_ShareHID_UsualKey].asString();
    Msg.m_strControlKey = m_JsonValue[PqMsg_Key_ShareHID_ControlKey].asString();

    Msg.m_strMouseKey		 = m_JsonValue[PqMsg_Key_ShareHID_MouseKey		    ].asString();
    Msg.m_strMouseXMove		 = m_JsonValue[PqMsg_Key_ShareHID_MouseXMove		].asString();
    Msg.m_strMouseYMove		 = m_JsonValue[PqMsg_Key_ShareHID_MouseYMove		].asString();
    Msg.m_strMouseRollerMove = m_JsonValue[PqMsg_Key_ShareHID_MouseRollerMove   ].asString();

    return true;
}

// { "DevID" : "812A930E4BC", "Values" : [ { "Channel" : "1", "Type" : "1", "Value" : "1" }, { "Channel" : "2", "Type" : "2", "Value" : "100" } ] }
void CPqStack::Encode(const CPqMsg_requestMsgPubValue &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    Json::Value pieces_all;//store all pieces
    std::list<CValueMsg>::const_iterator it = Msg.Values.begin();
    for ( ; it != Msg.Values.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_Channel] = it->m_strChannel;
        pieces [PqMsg_Key_Type       ] = it->m_strType        ;
        pieces [PqMsg_Key_Value       ] = it->m_strValue        ;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Values] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgPubValue &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Values];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CValueMsg __event;

        __event.m_strChannel        = arrayOperate[i] [PqMsg_Key_Channel       ].asString();
        __event.m_strType        = arrayOperate[i] [PqMsg_Key_Type       ].asString();
        __event.m_strValue        = arrayOperate[i] [PqMsg_Key_Value       ].asString();

        Msg.Values.push_back(__event);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgPubValueAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    Json::Value pieces_all;//store all pieces
    std::list<CValueMsg>::const_iterator it = Msg.Values.begin();
    for ( ; it != Msg.Values.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_Channel] = it->m_strChannel;
        pieces [PqMsg_Key_Type       ] = it->m_strType        ;
        pieces [PqMsg_Key_Value       ] = it->m_strValue        ;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Values] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgPubValueAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Values];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CValueMsg __event;

        __event.m_strChannel        = arrayOperate[i] [PqMsg_Key_Channel       ].asString();
        __event.m_strType        = arrayOperate[i] [PqMsg_Key_Type       ].asString();
        __event.m_strValue        = arrayOperate[i] [PqMsg_Key_Value       ].asString();

        Msg.Values.push_back(__event);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgPubValueList &Msg)
{
    Json::Value var;

    var[PqMsg_Key_GroupID] = Msg.m_strGroupID;

    Json::Value pieces_all;//store all pieces
    std::list<CPqMsg_requestMsgPubValue>::const_iterator it = Msg.DeviceValues.begin();
    for ( ; it != Msg.DeviceValues.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_DevID] = it->m_strDevID   ;

        {
            Json::Value pieces_value;//store all pieces
            std::list<CValueMsg>::const_iterator it_v = it->Values.begin();
            for ( ; it_v != it->Values.end() ; it_v++)
            {
                Json::Value pieces_v;//store all pieces

                pieces_v [PqMsg_Key_Channel] = it_v->m_strChannel;
                pieces_v [PqMsg_Key_Type       ] = it_v->m_strType        ;
                pieces_v [PqMsg_Key_Value       ] = it_v->m_strValue        ;

                pieces_value.append(pieces_v);
            }
            pieces[PqMsg_Key_Values] = pieces_value;

        }

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_DeviceValues] = pieces_all;

    m_JsonValue = var;
    setValid();
}


bool CPqStack::Decode(CPqMsg_requestMsgPubValueList &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strGroupID = m_JsonValue[PqMsg_Key_GroupID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_DeviceValues];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CPqMsg_requestMsgPubValue __event;

        __event.m_strDevID           = arrayOperate[i] [PqMsg_Key_DevID].asString();

        {
            const Json::Value pieces_value = m_JsonValue[PqMsg_Key_Values];
            for (size_t iv = 0; iv < pieces_value.size(); iv++)
            {
                CValueMsg __val;
                __val.m_strChannel = pieces_value[iv] [PqMsg_Key_Channel].asString();
                __val.m_strType    = pieces_value[iv] [PqMsg_Key_Type   ].asString();
                __val.m_strValue   = pieces_value[iv] [PqMsg_Key_Value  ].asString();
                __event.Values.push_back(__val);
            }
        }

        Msg.DeviceValues.push_back(__event);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgPubValueListAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var[PqMsg_Key_GroupID] = Msg.m_strGroupID;

    Json::Value pieces_all;//store all pieces
    std::list<CPqMsg_requestMsgPubValue>::const_iterator it = Msg.DeviceValues.begin();
    for ( ; it != Msg.DeviceValues.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_DevID] = it->m_strDevID   ;

        {
            Json::Value pieces_value;//store all pieces
            std::list<CValueMsg>::const_iterator it_v = it->Values.begin();
            for ( ; it_v != it->Values.end() ; it_v++)
            {
                Json::Value pieces_v;//store all pieces

                pieces_v [PqMsg_Key_Channel] = it_v->m_strChannel;
                pieces_v [PqMsg_Key_Type       ] = it_v->m_strType        ;
                pieces_v [PqMsg_Key_Value       ] = it_v->m_strValue        ;

                pieces_value.append(pieces_v);
            }
            pieces[PqMsg_Key_Values] = pieces_value;

        }

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_DeviceValues] = pieces_all;

    m_JsonValue = var;
    setValid();
}


bool CPqStack::Decode(CPqMsg_requestMsgPubValueListAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.m_strGroupID = m_JsonValue[PqMsg_Key_GroupID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_DeviceValues];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CPqMsg_requestMsgPubValue __event;

        __event.m_strDevID           = arrayOperate[i] [PqMsg_Key_DevID].asString();

        {
            const Json::Value pieces_value = m_JsonValue[PqMsg_Key_Values];
            for (size_t iv = 0; iv < pieces_value.size(); iv++)
            {
                CValueMsg __val;
                __val.m_strChannel = pieces_value[iv] [PqMsg_Key_Channel].asString();
                __val.m_strType    = pieces_value[iv] [PqMsg_Key_Type   ].asString();
                __val.m_strValue   = pieces_value[iv] [PqMsg_Key_Value  ].asString();
                __event.Values.push_back(__val);
            }
        }

        Msg.DeviceValues.push_back(__event);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgSetSchedule &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    Json::Value pieces_all;//store all pieces
    std::list<CScheduleMsg>::const_iterator it = Msg.Schedules.begin();
    for ( ; it != Msg.Schedules.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_ScheduleID ] = it->m_strScheduleID;
        pieces [PqMsg_Key_Minute] = it->m_strMinute;
        pieces [PqMsg_Key_Hour  ] = it->m_strHour  ;
        pieces [PqMsg_Key_Day   ] = it->m_strDay   ;
        pieces [PqMsg_Key_Month ] = it->m_strMonth ;
        pieces [PqMsg_Key_Date  ] = it->m_strDate  ;
        pieces [PqMsg_Key_TaskID  ] = it->m_strTaskID  ;
        pieces [PqMsg_Key_TaskType] = it->m_strTaskType;
        pieces [PqMsg_Key_TaskData] = it->m_strTaskData;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Schedules] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgSetSchedule &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Schedules];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CScheduleMsg __Schedule;

        __Schedule.m_strScheduleID = arrayOperate[i] [PqMsg_Key_ScheduleID].asString();
        __Schedule.m_strMinute        = arrayOperate[i] [PqMsg_Key_Minute       ].asString();
        __Schedule.m_strHour          = arrayOperate[i] [PqMsg_Key_Hour         ].asString();
        __Schedule.m_strDay           = arrayOperate[i] [PqMsg_Key_Day          ].asString();
        __Schedule.m_strMonth         = arrayOperate[i] [PqMsg_Key_Month        ].asString();
        __Schedule.m_strDate          = arrayOperate[i] [PqMsg_Key_Date         ].asString();
        __Schedule.m_strTaskID          = arrayOperate[i] [PqMsg_Key_TaskID         ].asString();
        __Schedule.m_strTaskType          = arrayOperate[i] [PqMsg_Key_TaskType].asString();
        __Schedule.m_strTaskData          = arrayOperate[i] [PqMsg_Key_TaskData].asString();

        Msg.Schedules.push_back(__Schedule);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgGetSchedule &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgGetSchedule &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgGetScheduleAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    Json::Value pieces_all;//store all pieces
    std::list<CScheduleMsg>::const_iterator it = Msg.Schedules.begin();
    for ( ; it != Msg.Schedules.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_ScheduleID ] = it->m_strScheduleID;
        pieces [PqMsg_Key_Minute] = it->m_strMinute;
        pieces [PqMsg_Key_Hour  ] = it->m_strHour  ;
        pieces [PqMsg_Key_Day   ] = it->m_strDay   ;
        pieces [PqMsg_Key_Month ] = it->m_strMonth ;
        pieces [PqMsg_Key_Date  ] = it->m_strDate  ;
        pieces [PqMsg_Key_TaskID  ] = it->m_strTaskID  ;
        pieces [PqMsg_Key_TaskType] = it->m_strTaskType;
        pieces [PqMsg_Key_TaskData] = it->m_strTaskData;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Schedules] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgGetScheduleAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Schedules];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CScheduleMsg __Schedule;

        __Schedule.m_strScheduleID = arrayOperate[i] [PqMsg_Key_ScheduleID].asString();
        __Schedule.m_strMinute        = arrayOperate[i] [PqMsg_Key_Minute       ].asString();
        __Schedule.m_strHour          = arrayOperate[i] [PqMsg_Key_Hour         ].asString();
        __Schedule.m_strDay           = arrayOperate[i] [PqMsg_Key_Day          ].asString();
        __Schedule.m_strMonth         = arrayOperate[i] [PqMsg_Key_Month        ].asString();
        __Schedule.m_strDate          = arrayOperate[i] [PqMsg_Key_Date         ].asString();
        __Schedule.m_strTaskID          = arrayOperate[i] [PqMsg_Key_TaskID         ].asString();
        __Schedule.m_strTaskType          = arrayOperate[i] [PqMsg_Key_TaskType ].asString();
        __Schedule.m_strTaskData          = arrayOperate[i] [PqMsg_Key_TaskData ].asString();

        Msg.Schedules.push_back(__Schedule);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgTaskPub &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;
    var[PqMsg_Key_TaskID] = Msg.m_strTaskID;

    Json::Value pieces_all;//store all pieces
    std::list<CValueMsg>::const_iterator it = Msg.Values.begin();
    for ( ; it != Msg.Values.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_Channel] = it->m_strChannel;
        pieces [PqMsg_Key_Type       ] = it->m_strType        ;
        pieces [PqMsg_Key_Value       ] = it->m_strValue        ;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_TaskData] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgTaskPub &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();
    Msg.m_strTaskID = m_JsonValue[PqMsg_Key_TaskID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_TaskData];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CValueMsg __event;

        __event.m_strChannel        = arrayOperate[i] [PqMsg_Key_Channel       ].asString();
        __event.m_strType        = arrayOperate[i] [PqMsg_Key_Type       ].asString();
        __event.m_strValue        = arrayOperate[i] [PqMsg_Key_Value       ].asString();

        Msg.Values.push_back(__event);
    }
    return true;
}
void CPqStack::Encode(const CPqMsg_requestMsgTaskList &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgTaskList &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgTaskListAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;

    Json::Value pieces_all;//store all pieces
    std::list<CTaskMsg>::const_iterator it = Msg.Tasks.begin();
    for ( ; it != Msg.Tasks.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_TaskID  ] = it->m_strTaskID;
        pieces [PqMsg_Key_TaskType] = it->m_strTaskType;
        pieces [PqMsg_Key_Option  ] = it->m_strOption;
        pieces [PqMsg_Key_Memo    ] = it->m_strMemo;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Tasks] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgTaskListAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }
    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Tasks];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CTaskMsg __Task;

        __Task.m_strTaskID        = arrayOperate[i] [PqMsg_Key_TaskID       ].asString();
        __Task.m_strTaskType        = arrayOperate[i] [PqMsg_Key_TaskType].asString();
        __Task.m_strOption        = arrayOperate[i] [PqMsg_Key_Option       ].asString();
        __Task.m_strMemo          = arrayOperate[i] [PqMsg_Key_Memo         ].asString();

        Msg.Tasks.push_back(__Task);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgTaskDetails &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;
    var[PqMsg_Key_TaskID] = Msg.m_strTaskID;
    var[PqMsg_Key_TaskType] = Msg.m_strTaskType;
    var[PqMsg_Key_StartDt] = Msg.m_strStartDt;
    var[PqMsg_Key_EndDt] = Msg.m_strEndDt;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgTaskDetails &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();
    Msg.m_strTaskID = m_JsonValue[PqMsg_Key_TaskID].asString();
    Msg.m_strTaskType = m_JsonValue[PqMsg_Key_TaskType].asString();
    Msg.m_strStartDt = m_JsonValue[PqMsg_Key_StartDt].asString();
    Msg.m_strEndDt = m_JsonValue[PqMsg_Key_EndDt].asString();

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgTaskDetailsAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PqMsg_Key_DevID] = Msg.m_strDevID;
    //var[PqMsg_Key_TaskID] = Msg.m_strTaskID;

    Json::Value pieces_all;//store all pieces
    std::list<CTaskValue>::const_iterator it = Msg.TaskValues.begin();
    for ( ; it != Msg.TaskValues.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_DataID   ] = it->m_strDataID   ;
        pieces [PqMsg_Key_TriggerID] = it->m_strTriggerID;
        pieces [PqMsg_Key_TaskID   ] = it->m_strTaskID   ;
        pieces [PqMsg_Key_TaskType] = it->m_strTaskType   ;
        pieces [PqMsg_Key_StartDt  ] = it->m_strStartDt  ;
        pieces [PqMsg_Key_EndDt    ] = it->m_strEndDt    ;

        pieces [PqMsg_Key_LastTaskID    ] = it->m_strLastTaskID     ;
        pieces [PqMsg_Key_LastTaskType  ] = it->m_strLastTaskType   ;
        pieces [PqMsg_Key_LastTaskValue ] = it->m_strLastTaskValue  ;

        {
            Json::Value pieces_value;//store all pieces
            std::list<CValueMsg>::const_iterator it_v = it->Values.begin();
            for ( ; it_v != it->Values.end() ; it_v++)
            {
                Json::Value pieces_v;//store all pieces

                pieces_v [PqMsg_Key_Channel] = it_v->m_strChannel;
                pieces_v [PqMsg_Key_Type       ] = it_v->m_strType        ;
                pieces_v [PqMsg_Key_Value       ] = it_v->m_strValue        ;

                pieces_value.append(pieces_v);
            }
            pieces[PqMsg_Key_Values] = pieces_value;

        }

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_TaskValues] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgTaskDetailsAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }
    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strDevID = m_JsonValue[PqMsg_Key_DevID].asString();
    //Msg.m_strTaskID = m_JsonValue[PqMsg_Key_TaskID].asString();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_TaskValues];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CTaskValue __event;

        __event.m_strDataID           = arrayOperate[i] [PqMsg_Key_DataID     ].asString();
        __event.m_strTriggerID        = arrayOperate[i] [PqMsg_Key_TriggerID  ].asString();
        __event.m_strTaskID           = arrayOperate[i] [PqMsg_Key_TaskID     ].asString();
        __event.m_strTaskType           = arrayOperate[i] [PqMsg_Key_TaskType].asString();
        __event.m_strStartDt          = arrayOperate[i] [PqMsg_Key_StartDt    ].asString();
        __event.m_strEndDt            = arrayOperate[i] [PqMsg_Key_EndDt      ].asString();

        __event.m_strLastTaskID     = arrayOperate[i] [PqMsg_Key_LastTaskID     ].asString();
        __event.m_strLastTaskType   = arrayOperate[i] [PqMsg_Key_LastTaskType   ].asString();
        __event.m_strLastTaskValue  = arrayOperate[i] [PqMsg_Key_LastTaskValue  ].asString();

        {
            const Json::Value pieces_value = m_JsonValue[PqMsg_Key_Values];
            for (size_t iv = 0; iv < pieces_value.size(); iv++)
            {
                CValueMsg __val;
                __val.m_strChannel = pieces_value[iv] [PqMsg_Key_Channel].asString();
                __val.m_strType    = pieces_value[iv] [PqMsg_Key_Type   ].asString();
                __val.m_strValue   = pieces_value[iv] [PqMsg_Key_Value  ].asString();
                __event.Values.push_back(__val);
            }
        }

        Msg.TaskValues.push_back(__event);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgDeviceList &Msg)
{
    Json::Value var;

    var[PqMsg_Key_GroupID ] = Msg.m_strGroupID ;
    var[PqMsg_Key_PageSize] = Msg.m_iPageSize;
    var[PqMsg_Key_Pages   ] = Msg.m_iPages   ;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgDeviceList &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strGroupID  = m_JsonValue[PqMsg_Key_GroupID ].asString();
    Msg.m_iPageSize = m_JsonValue[PqMsg_Key_PageSize].asInt();
    Msg.m_iPages    = m_JsonValue[PqMsg_Key_Pages   ].asInt();

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgDeviceListAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var[PqMsg_Key_GroupID   ] = Msg.m_strGroupID   ;
    var[PqMsg_Key_TotalCount] = Msg.m_iTotalCount;
    var[PqMsg_Key_Pages     ] = Msg.m_iPages     ;

    Json::Value pieces_all;//store all pieces
    std::list<CDeviceMsg>::const_iterator it = Msg.Devices.begin();
    for ( ; it != Msg.Devices.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_DevID   ] = it->m_strDevID ;
        pieces [PqMsg_Key_Type    ] = it->m_strCode  ;
        pieces [UniMsg_Key_Version ] = it->m_strVersion;
        pieces [PqMsg_Key_Status  ] = it->m_strStatus;
        pieces [PqMsg_Key_LastOnTime] = it->m_strLastOnTime;
        pieces [PqMsg_Key_Name] = it->m_strName;
        pieces [PqMsg_Key_Vendor] = it->m_strVendor;
        pieces [PqMsg_Key_Mac] = it->m_strMac;
        pieces [PqMsg_Key_Location] = it->m_strLocation;
        pieces [UniMsg_Key_Description] = it->m_strDescription;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Devices] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgDeviceListAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result        = m_JsonValue[PssMsg_Key_Result   ].asString();
    Msg.Reason        = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.m_strGroupID  = m_JsonValue[PqMsg_Key_GroupID   ].asString();
    Msg.m_iTotalCount = m_JsonValue[PqMsg_Key_TotalCount].asInt();
    Msg.m_iPages      = m_JsonValue[PqMsg_Key_Pages     ].asInt();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Devices];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CDeviceMsg __Devices;

        __Devices.m_strDevID        = arrayOperate[i] [PqMsg_Key_DevID       ].asString();
        __Devices.m_strCode         = arrayOperate[i] [PqMsg_Key_Type        ].asString();
        __Devices.m_strVersion = arrayOperate[i] [UniMsg_Key_Version        ].asString();
        __Devices.m_strStatus       = arrayOperate[i] [PqMsg_Key_Status      ].asString();
        __Devices.m_strLastOnTime   = arrayOperate[i] [PqMsg_Key_LastOnTime  ].asString();
        __Devices.m_strName         = arrayOperate[i] [PqMsg_Key_Name        ].asString();
        __Devices.m_strVendor       = arrayOperate[i] [PqMsg_Key_Vendor      ].asString();
        __Devices.m_strMac          = arrayOperate[i] [PqMsg_Key_Mac         ].asString();
        __Devices.m_strLocation     = arrayOperate[i] [PqMsg_Key_Location    ].asString();
        __Devices.m_strDescription  = arrayOperate[i] [UniMsg_Key_Description].asString();

        Msg.Devices.push_back(__Devices);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgSetShpc &Msg)
{
    Json::Value var;

    var[PqMsg_Key_DevID        ] = Msg.m_strDevID   ;
    var[PqMsg_Key_Shpc_Func    ] = Msg.m_strFunc    ;

    var[PqMsg_Key_Shpc_sHost   ] = Msg.m_strsHost   ;
    var[PqMsg_Key_Shpc_sPort   ] = Msg.m_strsPort   ;
    var[PqMsg_Key_Shpc_sTocken ] = Msg.m_strsTocken ;

    var[PqMsg_Key_Shpc_sTls ] = Msg.m_i_sTls;

    Json::Value pieces_all;//store all pieces
    std::list<CShpc>::const_iterator it = Msg.Shpcs.begin();
    for ( ; it != Msg.Shpcs.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_Shpc_cIteam      ] = it->m_strcIteam       ;
        pieces [PqMsg_Key_Shpc_cType       ] = it->m_strcType        ;
        pieces [PqMsg_Key_Shpc_cHost       ] = it->m_strcHost        ;
        pieces [PqMsg_Key_Shpc_cPort       ] = it->m_strcPort        ;
        pieces [PqMsg_Key_Shpc_cRemontePort] = it->m_strcRemontePort ;

        pieces [PqMsg_Key_Shpc_cEncryption] = it->m_iEncryption        ;
        pieces [PqMsg_Key_Shpc_cCompression] = it->m_iCompression;

        pieces_all.append(pieces);
    }
    var[PqMsg_Key_Shpcs] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgSetShpc &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID   = m_JsonValue[PqMsg_Key_DevID       ].asString();
    Msg.m_strFunc    = m_JsonValue[PqMsg_Key_Shpc_Func   ].asString();

    Msg.m_strsHost   = m_JsonValue[PqMsg_Key_Shpc_sHost  ].asString();
    Msg.m_strsPort   = m_JsonValue[PqMsg_Key_Shpc_sPort  ].asInt();
    Msg.m_strsTocken = m_JsonValue[PqMsg_Key_Shpc_sTocken].asString();

    Msg.m_i_sTls   = m_JsonValue[PqMsg_Key_Shpc_sTls  ].asInt();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Shpcs];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CShpc __Devices;

        __Devices.m_strcIteam       = arrayOperate[i] [PqMsg_Key_Shpc_cIteam       ].asString();
        __Devices.m_strcType        = arrayOperate[i] [PqMsg_Key_Shpc_cType        ].asString();
        __Devices.m_strcHost        = arrayOperate[i] [PqMsg_Key_Shpc_cHost        ].asString();
        __Devices.m_strcPort        = arrayOperate[i] [PqMsg_Key_Shpc_cPort        ].asInt();
        __Devices.m_strcRemontePort = arrayOperate[i] [PqMsg_Key_Shpc_cRemontePort ].asInt();

        __Devices.m_iEncryption        = arrayOperate[i] [PqMsg_Key_Shpc_cEncryption].asInt();
        __Devices.m_iCompression = arrayOperate[i] [PqMsg_Key_Shpc_cCompression].asInt();

        Msg.Shpcs.push_back(__Devices);
    }

    return true;
}

void CPqStack::Encode(const CPqMsg_requestMsgSetShpcAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result      ] = Msg.Result;
    var[PssMsg_Key_ResultMsg   ] = Msg.Reason;

    var[PqMsg_Key_DevID        ] = Msg.m_strDevID   ;
    var[PqMsg_Key_Shpc_Func    ] = Msg.m_strFunc   ;

    var[PqMsg_Key_Shpc_sHost   ] = Msg.m_strsHost   ;
    var[PqMsg_Key_Shpc_sPort   ] = Msg.m_strsPort   ;
    var[PqMsg_Key_Shpc_sTocken ] = Msg.m_strsTocken ;
    var[PqMsg_Key_Shpc_sTls] = Msg.m_i_sTls;

    Json::Value pieces_all;//store all pieces
    std::list<CShpc>::const_iterator it = Msg.Shpcs.begin();
    for ( ; it != Msg.Shpcs.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PqMsg_Key_Shpc_cIteam      ] = it->m_strcIteam       ;
        pieces [PqMsg_Key_Shpc_cType       ] = it->m_strcType        ;
        pieces [PqMsg_Key_Shpc_cHost       ] = it->m_strcHost        ;
        pieces [PqMsg_Key_Shpc_cPort       ] = it->m_strcPort        ;
        pieces [PqMsg_Key_Shpc_cRemontePort] = it->m_strcRemontePort ;

        pieces [PqMsg_Key_Shpc_cEncryption] = it->m_iEncryption        ;
        pieces [PqMsg_Key_Shpc_cCompression] = it->m_iCompression;

        pieces_all.append(pieces);
    }

    var[PqMsg_Key_Shpcs] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPqStack::Decode(CPqMsg_requestMsgSetShpcAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result       = m_JsonValue[PssMsg_Key_Result     ].asString();
    Msg.Reason       = m_JsonValue[PssMsg_Key_ResultMsg  ].asString();

    Msg.m_strDevID   = m_JsonValue[PqMsg_Key_DevID       ].asString();
    Msg.m_strFunc    = m_JsonValue[PqMsg_Key_Shpc_Func   ].asString();

    Msg.m_strsHost   = m_JsonValue[PqMsg_Key_Shpc_sHost  ].asString();
    Msg.m_strsPort   = m_JsonValue[PqMsg_Key_Shpc_sPort  ].asInt();
    Msg.m_strsTocken = m_JsonValue[PqMsg_Key_Shpc_sTocken].asString();

    Msg.m_i_sTls   = m_JsonValue[PqMsg_Key_Shpc_sTls  ].asInt();

    const Json::Value arrayOperate = m_JsonValue[PqMsg_Key_Shpcs];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CShpc __Devices;

        __Devices.m_strcIteam       = arrayOperate[i] [PqMsg_Key_Shpc_cIteam       ].asString();
        __Devices.m_strcType        = arrayOperate[i] [PqMsg_Key_Shpc_cType        ].asString();
        __Devices.m_strcHost        = arrayOperate[i] [PqMsg_Key_Shpc_cHost        ].asString();
        __Devices.m_strcPort        = arrayOperate[i] [PqMsg_Key_Shpc_cPort        ].asInt();
        __Devices.m_strcRemontePort = arrayOperate[i] [PqMsg_Key_Shpc_cRemontePort ].asInt();

        __Devices.m_iEncryption        = arrayOperate[i] [PqMsg_Key_Shpc_cEncryption].asInt();
        __Devices.m_iCompression = arrayOperate[i] [PqMsg_Key_Shpc_cCompression].asInt();

        Msg.Shpcs.push_back(__Devices);
    }

    return true;
}


