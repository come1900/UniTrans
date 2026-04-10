/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * InterVideoMsg.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: InterVideoMsg.h 5884 2018-10-15 08:49:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-10-15 08:49:47  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _InterVideoMsg_H
#define _InterVideoMsg_H

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "../UniTransRestMsg.h"

//#define _DEV_THIS_WITH_COUT 1

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// 通用请求协议， 用于从Agent 到 Node的通讯
const std::string InterVideoMsg_Key_DevID       = "DevID"            ;

const std::string InterVideoMsg_Key_Account     = "Account"          ; // 账号
const std::string InterVideoMsg_Key_Passwd      = "Passwd"           ; //
const std::string InterVideoMsg_Key_Manufacture = "Manufacture"      ; //
const std::string InterVideoMsg_Key_Model       = "Model"            ; //型号
const std::string InterVideoMsg_Key_ChannelNum  = "ChannelNum"       ; //通道数量

const std::string InterVideoMsg_Key_HostIp      = "HostIp"           ; // ip
const std::string InterVideoMsg_Key_HttpPort    = "HttpPort"         ; // Http端口
const std::string InterVideoMsg_Key_RtspPort    = "RtspPort"         ; //
const std::string InterVideoMsg_Key_TcpPort     = "TcpPort"          ; //

class CInterVideoMsg_addDevice
{
public:
    std::string m_strDevID        ;
    std::string m_strAccount      ;
    std::string m_strPasswd       ;
    std::string m_strManufacture  ;
    std::string m_strModel        ;
    std::string m_strChannelNum   ;
    std::string m_strHostIp       ;
    std::string m_strHttpPort     ;
    std::string m_strRtspPort     ;
    std::string m_strTcpPort      ;
    std::string m_strName;    // 名称

    CInterVideoMsg_addDevice()
    {
        m_strDevID        = "";
        m_strAccount      = "";
        m_strPasswd       = "";
        m_strManufacture  = "";
        m_strModel        = "";
        m_strChannelNum   = "";
        m_strHostIp       = "";
        m_strHttpPort     = "";
        m_strRtspPort     = "";
        m_strTcpPort      = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << InterVideoMsg_Key_DevID          << ":" << m_strDevID         << std::endl;
        std::cout << InterVideoMsg_Key_Account        << ":" << m_strAccount       << std::endl;
        std::cout << InterVideoMsg_Key_Passwd         << ":" << m_strPasswd        << std::endl;
        std::cout << InterVideoMsg_Key_Manufacture    << ":" << m_strManufacture   << std::endl;
        std::cout << InterVideoMsg_Key_Model          << ":" << m_strModel         << std::endl;
        std::cout << InterVideoMsg_Key_ChannelNum     << ":" << m_strChannelNum    << std::endl;
        std::cout << InterVideoMsg_Key_HostIp         << ":" << m_strHostIp        << std::endl;
        std::cout << InterVideoMsg_Key_HttpPort       << ":" << m_strHttpPort      << std::endl;
        std::cout << InterVideoMsg_Key_RtspPort       << ":" << m_strRtspPort      << std::endl;
        std::cout << InterVideoMsg_Key_TcpPort        << ":" << m_strTcpPort       << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

#define def_InterVideo_Value_Manufacture_DAHUA     "dahuatech.com" // 大华
#define def_InterVideo_Value_Manufacture_HIKVISION "hikvision.com" //海康
#define def_InterVideo_Value_Manufacture_RYHT      "ryht.net" //
#define def_InterVideo_Value_Manufacture_TIANDY    "tiandy.com" //

#define def_InterVideo_Value_Status_Online "Online" //
#define def_InterVideo_Value_Status_Offline "Offline" //
#define def_InterVideo_Value_Status_Error "Error" //
#define def_InterVideo_Value_Status_Unknown "Unknown" //

class CInterVideoMsg_addDeviceAck : public CAckMsgSimple
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID

    CInterVideoMsg_addDeviceAck()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << InterVideoMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif /* _InterVideoMsg_H */
