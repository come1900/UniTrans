/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PqMsg.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PqMsg.h 5884 2018-03-09 03:22:10Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-09 03:22:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _PqMsg_H
#define _PqMsg_H

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "../UniTransRestMsg.h"

//#define _DEV_THIS_WITH_COUT 1

//const std::string StreamCapabilityMsg_Key_Piece          = "StreamCapability";
const std::string StreamCapabilityMsg_Key_BandwidthIn    = "BandwidthIn";
const std::string StreamCapabilityMsg_Key_BandwidthOut   = "BandwidthOut";
const std::string StreamCapabilityMsg_Key_CpuMark        = "CpuMark";
const std::string StreamCapabilityMsg_Key_Memory         = "Memory";
//const std::string StreamCapabilityMsg_Key_StreamChannelTotal  = "StreamChannelTotal";
//const std::string StreamCapabilityMsg_Key_StreamChannelUsed  = "StreamChannelUsed";
//const std::string StreamCapabilityMsg_Key_StreamChannelUsing  = "StreamChannelUsing";
class CStreamCapabilityMsg
{
public:
    int BandwidthIn        ; // 入口带宽 bps
    int BandwidthOut       ; // 出口带宽 bps
    int CpuMark            ; // cpu评分，
    int Memory             ; // 内存数量 MByte
    //int StreamChannelTotal      ; // 分发能力
    //int StreamChannelUsed      ; // 已经分配的
    //int StreamChannelUsing      ; // 正在使用的

    CStreamCapabilityMsg()
    {
        BandwidthIn        = 0; // 入口带宽 bps
        BandwidthOut       = 0 ; // 出口带宽 bps
        CpuMark            = 0; // cpu评分，
        Memory             = 0; // 内存数量 MByte
        //StreamChannelTotal = 0; // 分发能力
        //StreamChannelUsed  = 0; // 已经分配的
        //StreamChannelUsing = 0; // 正在使用的
    }

    CStreamCapabilityMsg & operator=(const CStreamCapabilityMsg &t1)
    {
        BandwidthIn        = t1.BandwidthIn   ;
        BandwidthOut       = t1.BandwidthOut  ;
        CpuMark            = t1.CpuMark     ;
        Memory             = t1.Memory      ;
        //StreamChannelTotal = t1.StreamChannelTotal  ;
        //StreamChannelUsed  = t1.StreamChannelUsed  ;
        //StreamChannelUsing = t1.StreamChannelUsing  ;

        return *this;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << StreamCapabilityMsg_Key_BandwidthIn        << ":" << BandwidthIn        << std::endl;
        std::cout << StreamCapabilityMsg_Key_BandwidthOut       << ":" << BandwidthOut       << std::endl;
        std::cout << StreamCapabilityMsg_Key_CpuMark            << ":" << CpuMark            << std::endl;
        std::cout << StreamCapabilityMsg_Key_Memory             << ":" << Memory             << std::endl;
        //std::cout << StreamCapabilityMsg_Key_StreamChannelTotal << ":" << StreamChannelTotal << std::endl;
        //std::cout << StreamCapabilityMsg_Key_StreamChannelUsed  << ":" << StreamChannelUsed  << std::endl;
        //std::cout << StreamCapabilityMsg_Key_StreamChannelUsing << ":" << StreamChannelUsing << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};


// 通用请求协议， 用于从Agent 到 Node的通讯
const std::string PqMsg_Key_Session = "Session";
const std::string PqMsg_Key_DevID   = "DevID";
const std::string PqMsg_Key_MsgBody = "MsgBody";

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
class CPqMsg_requestMsg
{
public:
    std::string m_strSession;
    std::string m_strDevID;   // 通常为主控设备的ID
    std::string m_strMsgBody; // 消息体

    CPqMsg_requestMsg()
    {
        m_strSession = "";
        m_strDevID   = "";
        m_strMsgBody = "";
    }

    void dump()
    {
        std::cout << PqMsg_Key_Session  << ":" << m_strSession << std::endl;
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        std::cout << PqMsg_Key_MsgBody  << ":" << m_strMsgBody << std::endl;
    }
};
class CPqMsg_requestMsgAck
{
public:
    std::string m_strSession;
    std::string m_strDevID;   // 通常为主控设备的ID
    std::string m_strMsgBody; // 消息体

    CPqMsg_requestMsgAck()
    {
        m_strSession = "";
        m_strDevID   = "";
        m_strMsgBody = "";
    }

    void dump()
    {
        std::cout << PqMsg_Key_Session  << ":" << m_strSession << std::endl;
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        std::cout << PqMsg_Key_MsgBody  << ":" << m_strMsgBody << std::endl;
    }
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
const std::string PqMsg_Key_Channel = "Channel";
const std::string PqMsg_Key_Method  = "Method";

class CPqMsg_requestMsgPub_IO_Out
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::string m_strChannel; // 通道  1-n
    std::string m_strMethod;  // 操作  0-关 1-开

    CPqMsg_requestMsgPub_IO_Out()
    {
        m_strDevID   = "";
        m_strChannel = "";
        m_strMethod  = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        std::cout << PqMsg_Key_Channel  << ":" << m_strChannel << std::endl;
        std::cout << PqMsg_Key_Method  << ":" << m_strMethod << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// { "CmdType" : "NormalKB", "ControlKey" : "", "DevID" : "1", "UsualKey" : "abc" }
const std::string PqMsg_Key_ShareHID_IFNo = "IFNo";
const std::string PqMsg_Key_ShareHID_CmdType = "CmdType"; // 控制类别，比如普通键盘， 多媒体， 鼠标相对移动 等等
const std::string PqMsg_Key_ShareHID_UsualKey = "UsualKey";
const std::string PqMsg_Key_ShareHID_ControlKey = "ControlKey";

const std::string PqMsg_Key_ShareHID_MouseKey		 ="MouseKey";
const std::string PqMsg_Key_ShareHID_MouseXMove		 ="MouseXMove";
const std::string PqMsg_Key_ShareHID_MouseYMove		 ="MouseYMove";
const std::string PqMsg_Key_ShareHID_MouseRollerMove  ="MouseRollerMove";

const std::string PqMsg_Key_ShareHID_CmdType_Keyboard  ="k";
const std::string PqMsg_Key_ShareHID_CmdType_Mouse  ="m";

class CPqMsg_requestMsgShareHID
{
public:
    std::string m_strDevID;      // 通常为主控设备的ID G1189185272
    std::string m_strIFNo;       // 控制接口序号 1 ... n
    std::string m_strCmdType;    // 发送类别, 缺省以及k 表示键盘输入； m-鼠标输入
    std::string m_strUsualKey;   // 普通字符 , 键盘输入用
    std::string m_strControlKey; // 控制字符 , 键盘输入用

    // 鼠标按键， m_strCmdType 为m时用
    std::string m_strMouseKey;        // 鼠标按键按下, 1-左键, 2-右键, 4-中键 - 点击一次  （其他行为以后再定； 01 表示按下左键， 21表示双击左键）
    std::string m_strMouseXMove;      // 鼠标水平移动, +右移像素, -左移像素
    std::string m_strMouseYMove;      // 鼠标垂直移动, +下移像素, -上移像素
    std::string m_strMouseRollerMove; // 鼠标滚轮移动, +下滚动数, -上滚动数-- (win习惯-下滚滚轮屏幕向上滚动)

    CPqMsg_requestMsgShareHID()
    {
        m_strDevID = "";
        m_strIFNo = "";
        m_strCmdType = "";
        m_strUsualKey = "";
        m_strControlKey = "";

        m_strMouseKey = "";        // 鼠标按键
        m_strMouseXMove = "";      // 鼠标按键
        m_strMouseYMove = "";      // 鼠标按键
        m_strMouseRollerMove = ""; // 鼠标按键
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID << ":" << m_strDevID << std::endl;
        std::cout << PqMsg_Key_ShareHID_IFNo << ":" << m_strIFNo << std::endl;
        std::cout << PqMsg_Key_ShareHID_CmdType << ":" << m_strCmdType << std::endl;
        std::cout << PqMsg_Key_ShareHID_UsualKey << ":" << m_strUsualKey << std::endl;
        std::cout << PqMsg_Key_ShareHID_ControlKey << ":" << m_strControlKey << std::endl;

std::cout << PqMsg_Key_ShareHID_MouseKey		 << ":" << m_strMouseKey		     << std::endl;
std::cout << PqMsg_Key_ShareHID_MouseXMove		 << ":" << m_strMouseXMove		 << std::endl;
std::cout << PqMsg_Key_ShareHID_MouseYMove		 << ":" << m_strMouseYMove		 << std::endl;
std::cout << PqMsg_Key_ShareHID_MouseRollerMove << ":" << m_strMouseRollerMove    << std::endl;

#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PqMsg_Key_Type = "Type";
const std::string PqMsg_Key_Value = "Value";
class CValueMsg
{
public:
    std::string m_strChannel; // 通道  1-n
    std::string m_strType;  // 类型英文，不区分大小写 Switch, Temperature, Humidity
    std::string m_strValue;  // 数值

    CValueMsg()
    {
        m_strChannel = "";
        m_strType = "";
        m_strValue = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_Channel  << ":" << m_strChannel << std::endl;
        std::cout << PqMsg_Key_Type << ":" << m_strType << std::endl;
        std::cout << PqMsg_Key_Value << ":" << m_strValue << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};
#define def_PqMsg_ValueKey_Temperature      "Temperature"      //温度
#define def_PqMsg_ValueKey_Humidity         "Humidity"         //湿度
#define def_PqMsg_ValueKey_LiquidLevel      "LiquidLevel"      //液位
#define def_PqMsg_ValueKey_PH               "PH"               //PH
#define def_PqMsg_ValueKey_DissolvedOxygen  "DissolvedOxygen"  //溶氧
#define def_PqMsg_ValueKey_Conductivity     "Conductivity"     //电导率
#define def_PqMsg_ValueKey_Turbidity        "Turbidity"        //浊度
#define def_PqMsg_ValueKey_Phosphorus       "Phosphorus"       //总磷
#define def_PqMsg_ValueKey_Nitrogen         "Nitrogen"         //总氮
#define def_PqMsg_ValueKey_Permanganate     "Permanganate"     //高锰酸盐
#define def_PqMsg_ValueKey_AmmoniaNitrogen  "AmmoniaNitrogen"  //氨氮

#define def_PqMsg_ValueKey_LiveUrlClearRtmp   "LiveUrlClearRtmp"  // 实时视频- 清晰 rtmp
#define def_PqMsg_ValueKey_LiveUrlSmoothRtmp  "LiveUrlSmoothRtmp"  // 实时视频-流畅-rtmp

#define def_PqMsg_ValueKey_DigitalInput       "DI"      //数字量输入  开关量输入
#define def_PqMsg_ValueKey_DigitalOutput      "DO"      //数字量输入  开关量输入
#define def_PqMsg_ValueKey_AnalogInput        "AI"      //模拟量输入
#define def_PqMsg_ValueKey_AnalogOutput       "AO"      //模拟量输入

#define def_PqMsg_ValueKey_WindSpeed "WindSpeed" //#           WindSpeed       风速
#define def_PqMsg_ValueKey_WindDirection "WindDirection" //#           WindDirection   风向
#define def_PqMsg_ValueKey_AirPressure "AirPressure" //#           AirPressure     气压
#define def_PqMsg_ValueKey_Rainfall "Rainfall" //雨量

#define def_PqMsg_ValueKey_Algae "Algae" // 藻类
#define def_PqMsg_ValueKey_Cyanobacteria "Cyanobacteria" //蓝藻

#define def_PqMsg_ValueKey_AlgaeConBlue   "AlgaeConBlue"   // 蓝藻
#define def_PqMsg_ValueKey_AlgaeConGreen  "AlgaeConGreen"  // 绿藻
#define def_PqMsg_ValueKey_AlgaeConDino   "AlgaeConDino"   // 硅甲藻
#define def_PqMsg_ValueKey_AlgaeConHide   "AlgaeConHide"   // 隐藻
#define def_PqMsg_ValueKey_AlgaeConYellow "AlgaeConYellow" // 黄色物质
#define def_PqMsg_ValueKey_Transmittance "Transmittance" // 透光率
#define def_PqMsg_ValueKey_Depth "Depth" // 深度

#define def_PqMsg_ValueKey_MotherLiquidConcentration "MotherLiquidConcentration"//母液浓度
#define def_PqMsg_ValueKey_DosingConcentration "DosingConcentration"//配液浓度

//RTO 2018-12-06 16:01:36
#define def_DevIdUdpServerPandoraMatrix              "USPMDID" // 设备ID前缀

#define def_PqMsg_ValueKey_AvgTemperatureOfFirepot   "AvgTemperatureOfFirepot"   // 炉膛平均温度
#define def_PqMsg_ValueKey_TemperatureOfFirepot      "TemperatureOfFirepot"      // 炉内温度设定值
#define def_PqMsg_ValueKey_Voc1                      "Voc1"                      // VOC浓度1
#define def_PqMsg_ValueKey_Voc2                      "Voc2"                      // VOC浓度2
#define def_PqMsg_ValueKey_Voc3                      "Voc3"                      // VOC浓度3
#define def_PqMsg_ValueKey_AirOfRto                  "AirOfRto"                  // RTO当前系统风量
#define def_PqMsg_ValueKey_InTemperatureOfRto        "InTemperatureOfRto"        // RTO进口温度
#define def_PqMsg_ValueKey_OutTemperatureOfRto       "OutTemperatureOfRto"       // RTO出口温度
#define def_PqMsg_ValueKey_TemperatureOfCoolingTower "TemperatureOfCoolingTower" // 急冷塔温度

#define def_PqMsg_ValueKey_CommDetect                  "CommDetect"                  // RTO通讯检测
#define def_PqMsg_ValueKey_Fault_1                  "Fault_1"                  // RTO远程故障1
#define def_PqMsg_ValueKey_Fault_2                  "Fault_2"                  // RTO远程故障1

//鱼毒性仪
#define def_PqMsg_ValueKey_FishTaximeter1_Activated          "FT_Activated"          // 活性
#define def_PqMsg_ValueKey_FishTaximeter1_activated_sp       "FT_activated_sp"       // 特殊活性
#define def_PqMsg_ValueKey_FishTaximeter1_TopCoverage        "FT_TopCoverage"        // 顶部覆盖率
#define def_PqMsg_ValueKey_FishTaximeter1_Row_1              "FT_Row_1"              //  Row #1 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_2              "FT_Row_2"              //  Row #2 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_3              "FT_Row_3"              //  Row #3 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_4              "FT_Row_4"              //  Row #4 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_5              "FT_Row_5"              //  Row #5 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_6              "FT_Row_6"              //  Row #6 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_7              "FT_Row_7"              //  Row #7 activity
#define def_PqMsg_ValueKey_FishTaximeter1_Row_8              "FT_Row_8"              //  Row #8 activity
#define def_PqMsg_ValueKey_FishTaximeter1_temperature        "FT_temperature"        // 温度
#define def_PqMsg_ValueKey_FishTaximeter1_flow               "FT_flow"               // 流量
#define def_PqMsg_ValueKey_FishTaximeter1_Heat               "FT_Heat"               // 加热
#define def_PqMsg_ValueKey_FishTaximeter1_HeatWalt           "FT_HeatWalt"           // 加热功率
#define def_PqMsg_ValueKey_FishTaximeter1_Alarm_toxicity     "FT_Alarm_toxicity"     // 毒性警报
#define def_PqMsg_ValueKey_FishTaximeter1_Alarm_Hardware     "FT_Alarm_Hardware"     // 硬件警报
#define def_PqMsg_ValueKey_FishTaximeter1_Alarm_flow         "FT_Alarm_flow"         // 流量警报
#define def_PqMsg_ValueKey_FishTaximeter1_Alarm_Odoor        "FT_Alarm_Odoor"        // 开门警报
#define def_PqMsg_ValueKey_FishTaximeter1_Alarm_Level        "FT_Alarm_Level"        // 警报级别
#define def_PqMsg_ValueKey_FishTaximeter1_Feeding            "FT_Feeding"            // 喂食

//噪音
#define def_PqMsg_ValueKey_Noise1          "Noise1"          // 

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_Psso_ValueKey_Account "Account" // 账号
#define def_Psso_ValueKey_Passwd "Passwd" //
#define def_Psso_ValueKey_Manufacture "Manufacture" //
#define def_Psso_ValueKey_Model "Model" //型号
#define def_Psso_ValueKey_ChannelNum "ChannelNum" //

#define def_Psso_ValueKey_HostIp "HostIp" // ip
#define def_Psso_ValueKey_HttpPort "HttpPort" // Http端口
#define def_Psso_ValueKey_RtspPort "RtspPort" //
#define def_Psso_ValueKey_TcpPort "TcpPort" //

#define def_Psso_ValueKey_UrlRtsp "UrlRtsp" //
#define def_Psso_ValueKey_UrlRtspLocal "UrlRtspLocal" //
#define def_Psso_ValueKey_UrlRtspMapping "UrlRtspMapping" //

#define def_Psso_ValueKey_DevStatus "Status" //

#define def_Psso_Value_Manufacture_DAHUA     "dahuatech.com" // 大华
#define def_Psso_Value_Manufacture_HIKVISION "hikvision.com" //海康
#define def_Psso_Value_Manufacture_RYHT      "ryht.net" //
#define def_Psso_Value_Manufacture_TIANDY    "tiandy.com" //

#define def_Psso_Value_Status_Online "Online" //
#define def_Psso_Value_Status_Offline "Offline" //
#define def_Psso_Value_Status_Error "Error" //
#define def_Psso_Value_Status_Unknown "Unknown" //

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

const std::string PqMsg_Key_Values   = "Values";
// pub和get相同
class CPqMsg_requestMsgPubValue
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::list<CValueMsg> Values;

    CPqMsg_requestMsgPubValue()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        for (std::list<CValueMsg>::iterator ci = Values.begin() ; ci != Values.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CPqMsg_requestMsgPubValueAck : public CAckMsgSimple
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::list<CValueMsg> Values;

    CPqMsg_requestMsgPubValueAck()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        for (std::list<CValueMsg>::iterator ci = Values.begin() ; ci != Values.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

// set and get
// 批量设置/获取 设备数值——用来批量操作
const std::string PqMsg_Key_GroupID      = "GroupID";
const std::string PqMsg_Key_DeviceValues   = "DeviceValues";

class CPqMsg_requestMsgPubValueList
{
public:
    std::string m_strGroupID;     // appkey

    std::list<CPqMsg_requestMsgPubValue> DeviceValues;

    CPqMsg_requestMsgPubValueList()
    {
        m_strGroupID      = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_GroupID       << ":" << m_strGroupID      << std::endl;

        for (std::list<CPqMsg_requestMsgPubValue>::iterator ci = DeviceValues.begin() ; ci != DeviceValues.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CPqMsg_requestMsgPubValueListAck : public CAckMsgSimple
{
public:
    std::string m_strGroupID;     // appkey

    std::list<CPqMsg_requestMsgPubValue> DeviceValues;

    CPqMsg_requestMsgPubValueListAck()
    {
        m_strGroupID      = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << PqMsg_Key_GroupID       << ":" << m_strGroupID      << std::endl;

        for (std::list<CPqMsg_requestMsgPubValue>::iterator ci = DeviceValues.begin() ; ci != DeviceValues.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_ScheduleID = "ScheduleID";
const std::string PqMsg_Key_Minute = "Minute";
const std::string PqMsg_Key_Hour   = "Hour";
const std::string PqMsg_Key_Day    = "Day";
const std::string PqMsg_Key_Month  = "Month";
const std::string PqMsg_Key_Date   = "Date";
const std::string PqMsg_Key_TaskID   = "TaskID";
const std::string PqMsg_Key_TaskType   = "TaskType";
const std::string PqMsg_Key_TaskData   = "TaskData";
class CScheduleMsg
{
public:
    std::string m_strScheduleID;
    std::string m_strMinute;
    std::string m_strHour  ;
    std::string m_strDay   ;
    std::string m_strMonth ;
    std::string m_strDate  ;
    std::string m_strTaskID  ;
    std::string m_strTaskType  ;
    std::string m_strTaskData ;

    CScheduleMsg()
    {
        m_strScheduleID = "";
        m_strMinute = "";
        m_strHour   = "";
        m_strDay    = "";
        m_strMonth  = "";
        m_strDate   = "";
        m_strTaskID   = "";
        m_strTaskType   = "";
        m_strTaskData   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_ScheduleID  << ":" << m_strScheduleID << std::endl;
        std::cout << PqMsg_Key_Minute  << ":" << m_strMinute << std::endl;
        std::cout << PqMsg_Key_Hour    << ":" << m_strHour   << std::endl;
        std::cout << PqMsg_Key_Day     << ":" << m_strDay    << std::endl;
        std::cout << PqMsg_Key_Month   << ":" << m_strMonth  << std::endl;
        std::cout << PqMsg_Key_Date    << ":" << m_strDate   << std::endl;
        std::cout << PqMsg_Key_TaskID    << ":" << m_strTaskID   << std::endl;
        std::cout << PqMsg_Key_TaskType    << ":" << m_strTaskType << std::endl;
        std::cout << PqMsg_Key_TaskData    << ":" << m_strTaskData << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_Schedules   = "Schedules";

class CPqMsg_requestMsgSetSchedule
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::list<CScheduleMsg> Schedules;

    CPqMsg_requestMsgSetSchedule()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        for (std::list<CScheduleMsg>::iterator ci = Schedules.begin() ; ci != Schedules.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CPqMsg_requestMsgGetSchedule
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID

    CPqMsg_requestMsgGetSchedule()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CPqMsg_requestMsgGetScheduleAck  : public CAckMsgSimple
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::list<CScheduleMsg> Schedules;

    CPqMsg_requestMsgGetScheduleAck()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        for (std::list<CScheduleMsg>::iterator ci = Schedules.begin() ; ci != Schedules.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

//const std::string PqMsg_Key_TaskID = "TaskID";
const std::string PqMsg_Key_Option = "Option";
const std::string PqMsg_Key_Memo   = "Memo";

class CTaskMsg
{
public:
    std::string m_strTaskID;
    std::string m_strTaskType;
    std::string m_strOption;
    std::string m_strMemo  ;

    CTaskMsg()
    {
        m_strTaskID = "";
        m_strTaskType   = "";
        m_strOption = "";
        m_strMemo   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_TaskID  << ":" << m_strTaskID << std::endl;
        std::cout << PqMsg_Key_TaskType << ":" << m_strTaskType << std::endl;
        std::cout << PqMsg_Key_Option  << ":" << m_strOption << std::endl;
        std::cout << PqMsg_Key_Memo    << ":" << m_strMemo   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

//taskID
//Same as 机器功能
const std::string Pq_TaskID_QZ_startSampling = "QZ_startSampling";//  开始取证
const std::string Pq_TaskID_QZ_stopSampling  = "QZ_stopSampling"; //  停止取证
const std::string Pq_TaskID_QZ_trigLock      = "QZ_trigLock";     //  触发门锁-（开锁）

const std::string Pq_TaskID_QC_PureWaterStart      = "QC_PureWaterStart";     //  质控纯水
const std::string Pq_TaskID_QC_CustomSolutionStart      = "QC_CustomSolutionStart";     //  质控配液
const std::string Pq_TaskID_QC_StandardizeStart      = "QC_StandardizeStart";     //  加标

const std::string Pq_TaskID_trigReboot      = "reboot";     //  重启-本机
const std::string Pq_TaskID_sysupgrade      = "sysupgrade";     //  重启-本机

// PubTaskStart
// PubTaskStop
class CPqMsg_requestMsgTaskPub
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::string m_strTaskID;
    std::list<CValueMsg> Values;  // 任务数据

    CPqMsg_requestMsgTaskPub()
    {
        m_strDevID   = "";
        m_strTaskID = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        std::cout << PqMsg_Key_TaskID  << ":" << m_strTaskID << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CPqMsg_requestMsgTaskList
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    //std::list<CTaskMsg> Tasks;

    CPqMsg_requestMsgTaskList()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_Tasks   = "Tasks";

class CPqMsg_requestMsgTaskListAck : public CAckMsgSimple
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::list<CTaskMsg> Tasks;

    CPqMsg_requestMsgTaskListAck()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        for (std::list<CTaskMsg>::iterator ci = Tasks.begin() ; ci != Tasks.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_StartDt     = "StartDt";
const std::string PqMsg_Key_EndDt       = "EndDt";

class CPqMsg_requestMsgTaskDetails
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    std::string m_strTaskID;  // 任务ID
    std::string m_strTaskType  ;
    std::string m_strStartDt     ;
    std::string m_strEndDt       ;

    CPqMsg_requestMsgTaskDetails()
    {
        m_strDevID   = "";
        m_strTaskID   = "";
        m_strTaskType = "" ;
        m_strStartDt  = "" ;
        m_strEndDt    = "" ;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        std::cout << PqMsg_Key_TaskID    << ":" << m_strTaskID   << std::endl;
        std::cout << PqMsg_Key_TaskType << ":" << m_strTaskType << std::endl;
        std::cout << PqMsg_Key_StartDt    << ":" << m_strStartDt << std::endl;
        std::cout << PqMsg_Key_EndDt    << ":" << m_strEndDt << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_DataID      = "DataID";
const std::string PqMsg_Key_TriggerID   = "TriggerID";
//const std::string PqMsg_Key_TaskID      = "TaskID";

const std::string PqMsg_Key_LastTaskID    = "LastTaskID";
const std::string PqMsg_Key_LastTaskType  = "LastTaskType";
const std::string PqMsg_Key_LastTaskValue = "LastTaskValue";

class CTaskValue
{
public:
    std::string m_strDataID    ;  // 本条数据的id ，实际为 Sha1(triggerid+devid+taskid+startDt+EndDt)
    std::string m_strTriggerID ;  // 触发条件的ID- 比如定时任务的id
    std::string m_strTaskID    ;  // 任务ID
    std::string m_strTaskType  ;
    std::string m_strStartDt        ;  // 开始时间
    std::string m_strEndDt          ;  // 结束时间

    std::string m_strLastTaskID    ;  // 任务ID
    std::string m_strLastTaskType  ;  // 任务ID
    std::string m_strLastTaskValue ;  // 任务ID

    std::list<CValueMsg> Values;  // 任务数据

    CTaskValue()
    {
        m_strDataID    = "" ;
        m_strTriggerID = "" ;
        m_strTaskID    = "" ;
        m_strTaskType   = "";
        m_strStartDt   = "" ;
        m_strEndDt     = "" ;

        m_strLastTaskID    = "";  //
        m_strLastTaskType  = "";  //
        m_strLastTaskValue = "";  //
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DataID       << ":" << m_strDataID      << std::endl;
        std::cout << PqMsg_Key_TriggerID    << ":" << m_strTriggerID   << std::endl;
        std::cout << PqMsg_Key_TaskID       << ":" << m_strTaskID      << std::endl;
        std::cout << PqMsg_Key_TaskType   << ":" << m_strTaskType  << std::endl;
        std::cout << PqMsg_Key_StartDt      << ":" << m_strStartDt     << std::endl;
        std::cout << PqMsg_Key_EndDt        << ":" << m_strEndDt       << std::endl;

        std::cout << PqMsg_Key_LastTaskID       << ":" << m_strLastTaskID         << std::endl;
        std::cout << PqMsg_Key_LastTaskType     << ":" << m_strLastTaskType       << std::endl;
        std::cout << PqMsg_Key_LastTaskValue    << ":" << m_strLastTaskValue      << std::endl;

        for (std::list<CValueMsg>::iterator ci = Values.begin() ; ci != Values.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_TaskValues       = "TaskValues";

class CPqMsg_requestMsgTaskDetailsAck : public CAckMsgSimple
{
public:
    std::string m_strDevID;   // 通常为主控设备的ID
    //std::string m_strTaskID;  // 任务ID
    std::list<CTaskValue> TaskValues;  // 任务数据

    CPqMsg_requestMsgTaskDetailsAck()
    {
        m_strDevID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID    << ":" << m_strDevID   << std::endl;
        //std::cout << PqMsg_Key_TaskID    << ":" << m_strTaskID   << std::endl;

        for (std::list<CTaskValue>::iterator ci = TaskValues.begin() ; ci != TaskValues.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};


const std::string PqMsg_Key_PageSize     = "PageSize";
const std::string PqMsg_Key_Pages        = "Pages";

class CPqMsg_requestMsgDeviceList// : public CAckMsgSimple
{
public:
    std::string m_strGroupID;     // 组id，appkey
    int m_iPageSize;    // 每页多少个 1-n
    int m_iPages;       // 当前页 1-n

    CPqMsg_requestMsgDeviceList()
    {
        m_strGroupID      = "";
        m_iPageSize     = 0;
        m_iPages        = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_GroupID       << ":" << m_strGroupID      << std::endl;
        std::cout << PqMsg_Key_PageSize      << ":" << m_iPageSize     << std::endl;
        std::cout << PqMsg_Key_Pages         << ":" << m_iPages        << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_Status = "Status";
const std::string PqMsg_Key_Name   = "Name";
const std::string PqMsg_Key_Vendor   = "Vendor";
const std::string PqMsg_Key_Mac   = "Mac";
const std::string PqMsg_Key_Location   = "Location";
const std::string PqMsg_Key_LastOnTime   = "LastOnTime";

#if 0
typedef enum __em_device_type_t {
    em_device_type_g1 =  0,	//
    em_Device_type_ipc =  1,	//
    em_Device_type_zhikongyi_1,	//
}EM_DEVICE_TYPE_T;
#endif
typedef enum __em_device_status_t {
    em_device_status_Offline =  0,	//
    em_device_status_Online =  1,	//
}EM_DEVICE_STATUS_T;

class CDeviceMsg
{
public:
    std::string m_strDevID;   // 设备ID
    std::string m_strName;    // 名称
    std::string m_strCode;    // 型号
    std::string m_strVersion; // 版本
    std::string m_strVendor;       //
    std::string m_strMac;       //
    std::string m_strStatus;       // 状态 0/1 - EM_DEVICE_STATUS_T, 401~499-error code
    std::string m_strLastOnTime;   // 位置
    std::string m_strLocation;     // 位置
    std::string m_strDescription;  // 描述

    CDeviceMsg()
    {
        m_strDevID = "";
        m_strCode = "";
        m_strVersion= "";
        m_strStatus = "";
        m_strLastOnTime = "";
        m_strName = "";   //
        m_strVendor = "";   //
        m_strLocation = "";   //
        m_strDescription = "";   //
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID  << ":" << m_strDevID << std::endl;
        std::cout << PqMsg_Key_Type << ":" << m_strCode << std::endl;
        std::cout << UniMsg_Key_Version << ":" << m_strVersion << std::endl;
        std::cout << PqMsg_Key_Status << ":" << m_strStatus << std::endl;
        std::cout << PqMsg_Key_LastOnTime << ":" << m_strLastOnTime << std::endl;
        std::cout << PqMsg_Key_Name<< ":" << m_strName<< std::endl;
        std::cout << PqMsg_Key_Vendor << ":" << m_strVendor<< std::endl;
        std::cout << PqMsg_Key_Mac << ":" << m_strMac << std::endl;
        std::cout << PqMsg_Key_Location<< ":" << m_strLocation<< std::endl;
        std::cout << UniMsg_Key_Description << ":" << m_strDescription<< std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_TotalCount   = "TotalCount";
const std::string PqMsg_Key_Devices      = "Devices";

class CPqMsg_requestMsgDeviceListAck : public CAckMsgSimple
{
public:
    std::string m_strGroupID;     // appkey
    int m_iTotalCount;  // 设备总数 0-n
    int m_iPages;       // 当前页 1-n

    std::list<CDeviceMsg> Devices;

    CPqMsg_requestMsgDeviceListAck()
    {
        m_strGroupID      = "";
        m_iTotalCount   = 0;
        m_iPages        = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_GroupID       << ":" << m_strGroupID      << std::endl;
        std::cout << PqMsg_Key_TotalCount    << ":" << m_iTotalCount   << std::endl;
        std::cout << PqMsg_Key_Pages         << ":" << m_iPages        << std::endl;

        for (std::list<CDeviceMsg>::iterator ci = Devices.begin() ; ci != Devices.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_Shpcs               = "Shpcs";
const std::string PqMsg_Key_Shpc_cIteam         = "cItem";
const std::string PqMsg_Key_Shpc_cType          = "cType";
const std::string PqMsg_Key_Shpc_cHost          = "cHost";
const std::string PqMsg_Key_Shpc_cPort          = "cPort";
const std::string PqMsg_Key_Shpc_cRemontePort   = "cRemontePort";

const std::string PqMsg_Key_Shpc_cEncryption   = "cEncryption";
const std::string PqMsg_Key_Shpc_cCompression   = "cCompression";

class CShpc
{
public:
    std::string m_strcIteam      ;  // [G1536007353VH1-554]
    std::string m_strcType       ;  // type = tcp
    std::string m_strcHost       ;  // local_ip = 192.168.182.61
    int m_strcPort       ;  // local_port = 554
    int m_strcRemontePort;  // remote_port = 16348

    int m_iEncryption       ;  // use_encryption = 1-true, 2-false, 0(not 1 or 2)-do not use
    int m_iCompression       ;  // use_compression = 1-true, 2-false, 0(not 1 or 2)-do not use

    CShpc()
    {
        m_strcIteam       = "";
        m_strcType        = "";
        m_strcHost        = "";
        m_strcPort        = 0;
        m_strcRemontePort = 0;

        m_iEncryption = 0;
        m_iCompression = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_Shpc_cIteam          << ":" << m_strcIteam         << std::endl;
        std::cout << PqMsg_Key_Shpc_cType           << ":" << m_strcType          << std::endl;
        std::cout << PqMsg_Key_Shpc_cHost           << ":" << m_strcHost          << std::endl;
        std::cout << PqMsg_Key_Shpc_cPort           << ":" << m_strcPort          << std::endl;
        std::cout << PqMsg_Key_Shpc_cRemontePort    << ":" << m_strcRemontePort   << std::endl;

        std::cout << PqMsg_Key_Shpc_cEncryption    << ":" << m_iEncryption   << std::endl;
        std::cout << PqMsg_Key_Shpc_cCompression    << ":" << m_iCompression   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PqMsg_Key_Shpc_Func          = "Func";
const std::string PqMsg_Key_Shpc_sHost          = "sHost";
const std::string PqMsg_Key_Shpc_sPort          = "sPort";
const std::string PqMsg_Key_Shpc_sTocken        = "sTocken";

const std::string PqMsg_Key_Shpc_sTls   = "sTls";

class CPqMsg_requestMsgSetShpc
{
public:
    std::string m_strDevID  ;   // 通常为主控设备的ID
    std::string m_strFunc  ;   // 作用， 可以填写文件绝对路径

    std::string m_strsHost  ;   // server_addr
    int m_strsPort  ;   // server_port
    std::string m_strsTocken;   // privilege_token

    int m_i_sTls;  // tls_enable = 1-true, 2-false, 0(not 1 or 2)-do not use

    std::list<CShpc> Shpcs;     //

    CPqMsg_requestMsgSetShpc()
    {
        m_strDevID   = "";
        m_strFunc   = "";

        m_strsHost     = "";
        m_strsPort     = 0;
        m_strsTocken   = "";

        m_i_sTls     = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PqMsg_Key_DevID           << ":" << m_strDevID     << std::endl;
        std::cout << PqMsg_Key_Shpc_Func           << ":" << m_strFunc << std::endl;

        std::cout << PqMsg_Key_Shpc_sHost      << ":" << m_strsHost     << std::endl;
        std::cout << PqMsg_Key_Shpc_sPort      << ":" << m_strsPort     << std::endl;
        std::cout << PqMsg_Key_Shpc_sTocken    << ":" << m_strsTocken   << std::endl;

        std::cout << PqMsg_Key_Shpc_sTls    << ":" << m_i_sTls   << std::endl;

        for (std::list<CShpc>::iterator ci = Shpcs.begin() ; ci != Shpcs.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CPqMsg_requestMsgSetShpcAck : public CPqMsg_requestMsgSetShpc, public CAckMsgSimple
{
public:
    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();
        CPqMsg_requestMsgSetShpc::dump();
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif /* _PqMsg_H */
