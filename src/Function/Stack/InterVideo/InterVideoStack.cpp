/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * InterVideoStack.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: InterVideoStack.cpp 5884 2018-03-09 03:22:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-09 03:22:15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "InterVideoStack.h"

#include <assert.h>

CInterVideoStack::CInterVideoStack()
{}

CInterVideoStack::~CInterVideoStack()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

void CInterVideoStack::Encode(const CAckMsgSimple &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CInterVideoStack::Decode(CAckMsgSimple &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CInterVideoStack::Encode(const CInterVideoMsg_addDevice &Msg)
{
    Json::Value var;

    var[InterVideoMsg_Key_DevID        ] = Msg.m_strDevID        ;
    var[InterVideoMsg_Key_Account      ] = Msg.m_strAccount      ;
    var[InterVideoMsg_Key_Passwd       ] = Msg.m_strPasswd       ;
    var[InterVideoMsg_Key_Manufacture  ] = Msg.m_strManufacture  ;
    var[InterVideoMsg_Key_Model        ] = Msg.m_strModel        ;
    var[InterVideoMsg_Key_HostIp       ] = Msg.m_strHostIp       ;
    var[InterVideoMsg_Key_HttpPort     ] = Msg.m_strHttpPort     ;
    var[InterVideoMsg_Key_RtspPort     ] = Msg.m_strRtspPort     ;
    var[InterVideoMsg_Key_TcpPort      ] = Msg.m_strTcpPort      ;

    m_JsonValue = var;
    setValid();
}

bool CInterVideoStack::Decode(CInterVideoMsg_addDevice &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strDevID         = m_JsonValue[InterVideoMsg_Key_DevID        ].asString();
    Msg.m_strAccount       = m_JsonValue[InterVideoMsg_Key_Account      ].asString();
    Msg.m_strPasswd        = m_JsonValue[InterVideoMsg_Key_Passwd       ].asString();
    Msg.m_strManufacture   = m_JsonValue[InterVideoMsg_Key_Manufacture  ].asString();
    Msg.m_strModel         = m_JsonValue[InterVideoMsg_Key_Model        ].asString();
    Msg.m_strHostIp        = m_JsonValue[InterVideoMsg_Key_HostIp       ].asString();
    Msg.m_strHttpPort      = m_JsonValue[InterVideoMsg_Key_HttpPort     ].asString();
    Msg.m_strRtspPort      = m_JsonValue[InterVideoMsg_Key_RtspPort     ].asString();
    Msg.m_strTcpPort       = m_JsonValue[InterVideoMsg_Key_TcpPort      ].asString();

    return true;
}

void CInterVideoStack::Encode(const CInterVideoMsg_addDeviceAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result      ] = Msg.Result;
    var[PssMsg_Key_ResultMsg   ] = Msg.Reason;

    var[InterVideoMsg_Key_DevID  ] = Msg.m_strDevID  ;

    m_JsonValue = var;
    setValid();
}

bool CInterVideoStack::Decode(CInterVideoMsg_addDeviceAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result       = m_JsonValue[PssMsg_Key_Result     ].asString();
    Msg.Reason       = m_JsonValue[PssMsg_Key_ResultMsg  ].asString();

    Msg.m_strDevID   = m_JsonValue[InterVideoMsg_Key_DevID  ].asString();

    return true;
}