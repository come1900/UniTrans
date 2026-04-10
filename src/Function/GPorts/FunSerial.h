/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunSerial.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunSerial.h 5884 2014-02-13 03:40:57Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-13 03:40:57 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <vector>
#include <ez_hdlc.h>
#include "ezBuffer.h"

#include "EZThread.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigSerial.h"
#endif//_USE_MODULES_EZCONFIG

#include "../../Device/DevLock_D1.h"
#include "CH9329_def.h"

#ifndef _FunSerial_H
#define _FunSerial_H

#define def_SerialFunc_COD_HDLC                      "CodHdlc"                    // 质控板开发中， 未使用
#define def_SerialFunc_COD_HDLC_Permanganate         "CodHdlc_Permanganate"       // 质控板开发中， 未使用
#define def_SerialFunc_COD_HDLC_AmmoniaNitrogen      "CodHdlc_AmmoniaNitrogen"    // 质控板开发中， 未使用
#define def_SerialFunc_COD_HDLC_PhosphorusNitrogen   "CodHdlc_PhosphorusNitrogen" // 质控板开发中， 未使用
#define def_SerialFunc_COD_31                        "Cod31"                      // 质控板-- 桐庐
#define def_SerialFunc_Cod31_Permanganate            "Cod31_Permanganate"         // 质控板-- 桐庐
#define def_SerialFunc_Cod31_AmmoniaNitrogen         "Cod31_AmmoniaNitrogen"      // 质控板-- 桐庐
#define def_SerialFunc_Cod31_PhosphorusNitrogen      "Cod31_PhosphorusNitrogen"   // 质控板-- 桐庐
#define def_SerialFunc_BBE_FLUOROPROBE               "BBE_FLUOROPROBE"            // BBE 藻类传感器
#define def_SerialFunc_QZ1_IO1                       "QZ1_IO1"                    // 取证io板 1型
#define def_SerialFunc_QZ2_TONGLU1                   "QZ2_TONGLU1"                // 桐庐取证板 1型
#define def_SerialFunc_EK_3030E                      "EK_3030E"                   // 精创温度控制器EK-3030E
#define def_SerialFunc_MQPC_10                       "MQPC_10"                    // 水质五参数仪器
#define def_SerialFunc_SerialComm                    "PKTH100B"                   // 温湿度
#define def_SerialFunc_FishTaximeter1                "FISH_TAXIMETER1"            // 鱼毒性仪1
#define def_SerialFunc_FishTaximeter_BBE             "FISH_TAXIMETER_BBE"         // 鱼毒性仪- BBE
#define def_SerialFunc_WXT530                        "WXT530"                     // 气象6参数 Vaisala Weather Transmitter

#define def_SerialFunc_Carry8I8O                     "Carry8I8O"
#define def_SerialFunc_WP9038ADAM                    "WP9038ADAM"
#define def_SerialFunc_WP8027ADAM                    "WP8027ADAM"                 // 16路光电隔离数字量输出
#define def_SerialFunc_QC_SA_1                       "QC_SA_1"                    // 鱼毒性仪1
#define def_SerialFunc_NoiseMeter_RSZS_01            "NM_RSZS_01"                 // 噪音仪 RS-ZS-N01-BYH
#define def_SerialFunc_NoiseMeter_PG760_01           "NM_PG760_01"                // 噪音仪 PG-760/ZY-CG

#define def_SerialFunc_CH9329_V1                     "CH9329"                     // 键盘串口 20220618
#define def_SerialFunc_GXE1009_V1                    "GXE1009"                    // 国芯物联RFID

#define def_SerialFunc_SerialComm_Period 5   // 每? 秒采集一次温度

#define def_SerialFunc_StatusEvents_Period 1   // 状态驱动 周期1 秒

#define def_FishTaximeter1_Data_Num 15

// 数据的无效值
#define InvalidFloatPara -99999
#define InvalidIntPara -99999

#define g_FunSerial  (*CFunSerial::instance())

enum EnStatus_GXE1009_V1
{
    EnStatus_GXE1009_V1_Linked = 1,                  // 串口打开
    EnStatus_GXE1009_V1_GetBaseVersion,              //
    EnStatus_GXE1009_V1_GetBaseVersion_Done,         //
    EnStatus_GXE1009_V1_MsgBaseGetCapabilities,      //
    EnStatus_GXE1009_V1_MsgBaseGetCapabilities_Done, // Connect OK
    EnStatus_GXE1009_V1_MsgBaseStop,                 //
    EnStatus_GXE1009_V1_MsgBaseStop_Done,            // 先停止
    EnStatus_GXE1009_V1_MsgBaseInventoryEpc,         //
    EnStatus_GXE1009_V1_MsgBaseInventoryEpc_Done,    //
};

class CFunSerial:public CEZObject
{
public:
    PATTERN_SINGLETON_DECLARE(CFunSerial);

    CFunSerial();
    virtual ~CFunSerial();

    int Start();
    int Stop();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int Sendto(int iSerialID, char *pBuff, size_t iBuffLen);
    int Sendto_Modbus(int iSerialID, char *pBuff, size_t iBuffLen);

    int Sendto_CH9329(const int iSerialID, const unsigned char *pBuff, const size_t iBuffLen);
    int Sendto_CH9329_mouseKey(const int iSerialID,
                               const unsigned char mouseKey = 0,
                               const unsigned char xMove = 0,
                               const unsigned char yMove = 0,
                               const unsigned char rollerMove = 0);
    int Sendto_CH9329_keyVal(const int iSerialID, const unsigned char usualKey, const unsigned char controlKey = 0);
    int Sendto_CH9329_String(const int iSerialID, const char *strInput, const size_t strLen);
    int Sendto_CH9329_AsciiCtrl(const int iSerialID, std::string strCtrl);
    // GXE1009_V1
    int Sendto_xmoden(int iSerialID, const unsigned char *pBuff, size_t iBuffLen, const size_t iSkipHead=1);
    // not ok now
    int Sendto_Modbus_TNP(int iSerialID, char *pBuff, size_t iBuffLen);
    int SendtoBase64(const int iSerialID, const char *pBuffBase64);
    // 发往具有某功能的接口中
    int Sendto(const std::string &strSerialFunc, char *pBuff, size_t iBuffLen);
    int Sendto_Modbus(const std::string &strSerialFunc, char *pBuff, size_t iBuffLen);
    int Sendto_Hdlc2(int iSerialID, char *pBuff, size_t iBuffLen);
    int Sendto_Hdlc2(const std::string &strSerialFunc, char *pBuff, size_t iBuffLen);
    int OnHdlc2(unsigned char *buf, int len, void *arg);
    int OnCodHdlc(unsigned char *buf, int len, void *arg);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    EZTHREAD_BOOL OnSerial(int id, const char *pdat, size_t len);
    int OnData_QZ2_TONGLU1(unsigned char *buf, int len, void *arg);
    int OnData_FishTaximeter1(const unsigned char *buf, int len, unsigned short &devid);
    int OnData_WXT_530(const unsigned char *buf, int len, unsigned short &devid);
    int OnData_FishTaximeter_BBE(const unsigned char *buf, int len, unsigned short &devid);
    int OnData_QC_SA_1(const unsigned char *buf, int len, unsigned short &devid);

    int OnData_CH9329_V1(const unsigned char *buf, const size_t len, unsigned short &devid);
    int OnData_GXE1009_V1(const unsigned char *buf, const size_t len, unsigned short &devid);

    void TimerProcGXE1009_V1();
    void TimerProcPKTH100B();
    //float getTemperature();
    //float getHumidity();

    // io board
    int pubIOOut(int SerialId, int channel, int iOnOff);
    int pubIOOut_Carry8I8O(int SerialId, int channel, int iOnOff);
    int pubIOOut_WP9038ADAM(int SerialId, int channel, int iOnOff, int iOutMax=4);
    int pubIOOut_QZ1_IO1(int SerialId, int channel, int iOnOff);

    int pubQC1_Start(int SerialId, int channel, int iOnOff);
private:
    bool m_bHaveStarted;
    // start stop mutex
    CEZMutex m_LockFunSerial;

    // 操作串口的互斥量
    CEZMutex m_LockSerialOpr;//write

    CEZMutex m_LockSerialBuffer;
    CezBuffer *m_pSerialBuffer;

    CConfigSerial *m_pSerialCfg;

    CEZTimer          m_TimerSerialComm;          // 定时驱动式

    CEZTimer          m_TimerStatusEvents;          // 状态驱动式， 
    //
    unsigned char m_asciiKeyMap[def_CH9329_asciiKeyMap_Size];
    int ini_asciiKeyMap_Ch9329(unsigned char *asciiKeyMap);
    unsigned char getUsualKey(const unsigned char *asciiKeyMap, unsigned char theKey);
    unsigned char getUsualKey_NeedShift(unsigned char theKey);
    unsigned char getControlKey(std::string strCtrl);
public:
    float m_fTemperature;// 温度 1
    float m_fHumidity; // 湿度1
    time_t m_ttOnTempHum;// 数据刷新时间

    float m_fpH  ; //  酸碱
    float m_fCond; //  电导率
    float m_fDo  ; //  溶解氧
    float m_fTurb; //  浊度
    float m_fTemp; //  温度2
    time_t m_ttOnPh;// 数据刷新时间

    //DI and DO status
    unsigned int m_iDI;
    unsigned int m_iDO;
    time_t m_ttOnDIO;

    float m_fWindSpeed;
    float m_fWindDirection;
    float m_fAirPressure;
    float m_fTemperature_3;// 温度 3
    float m_fHumidity_3;// 湿度3
    float m_fRainfall;// 雨量
    time_t m_ttOnWeather;

    float m_fAlgaeConBlue   ; //藻类浓度-蓝藻
    float m_fAlgaeConGreen  ; //藻类浓度-绿藻
    float m_fAlgaeConDino   ; //藻类浓度-硅甲藻
    float m_fAlgaeConHide   ; //藻类浓度-隐藻
    float m_fAlgaeConYellow ; //藻类浓度-黄色物质
    float m_fTransmittance;// 透光率
    float m_fDepth;// 深度
    time_t m_ttOnBBE;// 数据刷新时间
    int m_iBBEStatus;// BBE状态

    float m_fCod_Permanganate;    //  高锰酸盐
    float m_fCod_AmmoniaNitrogen; //  氨氮
    float m_fCod_Phosphorus;      //  总磷
    float m_fCod_Nitrogen;        //  总氮
    unsigned char m_codStatus; //
    time_t m_ttOnCod;// 数据刷新时间
#if 0
    float         m_fFishTaximeter1_Activated                ;// 活性
    float         m_fFishTaximeter1_activated_sp             ;// 特殊活性
    float         m_fFishTaximeter1_TopCoverage              ;// 顶部覆盖率
    float         m_fFishTaximeter1_Row_1                    ;//  Row #1 activity
    float         m_fFishTaximeter1_Row_2                    ;//  Row #2 activity
    float         m_fFishTaximeter1_Row_3                    ;//  Row #3 activity
    float         m_fFishTaximeter1_Row_4                    ;//  Row #4 activity
    float         m_fFishTaximeter1_Row_5                    ;//  Row #5 activity
    float         m_fFishTaximeter1_Row_6                    ;//  Row #6 activity
    float         m_fFishTaximeter1_Row_7                    ;//  Row #7 activity
    float         m_fFishTaximeter1_Row_8                    ;//  Row #8 activity
    float         m_fFishTaximeter1_temperature              ;// 温度
    float         m_fFishTaximeter1_flow                     ;// 流量
    float         m_fFishTaximeter1_Heat                     ;// 加热
    float         m_fFishTaximeter1_HeatWalt                 ;// 加热功率
#endif
    float m_fFishTaximeter1_Data[def_FishTaximeter1_Data_Num];
    int m_iFishTaximeter1_Alarm_toxicity           ;// 毒性警报
    int m_iFishTaximeter1_Alarm_Hardware           ;// 硬件警报
    int m_iFishTaximeter1_Alarm_flow               ;// 流量警报
    int m_iFishTaximeter1_Alarm_Odoor              ;// 开门警报
    int m_iFishTaximeter1_Alarm_Level              ;// 警报级别
    int m_iFishTaximeter1_Feeding                  ;// 喂食
    time_t m_ttOnFishTaximeter1;// 数据刷新时间

    time_t m_ttOnQC_SA_1;// 数据刷新时间

    float m_fNoise;// 噪音
    time_t m_ttOnNoise;// 数据刷新时间

    int m_iGXE1009_V1_Func;//当前开启的功能
    CEZMutex m_Lock_iGXE1009_V1_States;
    EnStatus_GXE1009_V1 m_iGXE1009_V1_States; //当前状态
};

#endif // !defined(_FunSerial_H)
