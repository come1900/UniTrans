/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunSerial.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunSerial.cpp 5884 2014-02-13 03:40:53Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-13 03:40:53 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <str_opr.h>
#include <crc.h>
#include <base64.h>
#include <ez_system_api.h>
#include <ez_bit.h>
#include <ez_util_platform.h> // ez_ms_get_tick_count

#include <sys/time.h>
#include <iostream>
#include <arpa/inet.h>

#include	"../../Logs.h"

#ifdef _DEV_SERIAL
#include "../../Device/carry_io_board.h"
#include "../../Device/DevSerial.h"
#endif // _DEV_SERIAL

#include "FunSerial.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PATTERN_SINGLETON_IMPLEMENT(CFunSerial);
CFunSerial::CFunSerial():m_TimerSerialComm(def_SerialFunc_SerialComm)
{
    m_bHaveStarted = false;

    m_pSerialBuffer = new CezBuffer;
    assert(m_pSerialBuffer);

    m_pSerialCfg = new CConfigSerial;
    assert(m_pSerialCfg);

    m_fTemperature = InvalidFloatPara;
    m_fHumidity = InvalidFloatPara;
    m_ttOnTempHum = -1;

    m_fpH   = InvalidFloatPara; //  酸碱
    m_fCond = InvalidFloatPara; //  电导率
    m_fDo   = InvalidFloatPara; //  溶解氧
    m_fTurb = InvalidFloatPara; //  浊度
    m_fTemp = InvalidFloatPara; //  温度
    m_ttOnPh = -1;

    m_iDI = 0;
    m_iDO = 0;
    m_ttOnDIO = 0;

    m_fWindSpeed = InvalidFloatPara;
    m_fWindDirection = InvalidFloatPara;
    m_fAirPressure = InvalidFloatPara;
    m_fTemperature_3 = InvalidFloatPara;// 温度 3
    m_fHumidity_3 = InvalidFloatPara;// 湿度3
    m_fRainfall = InvalidFloatPara;// 雨量
    m_ttOnWeather = 0;// 数据刷新时间

    m_fAlgaeConBlue   = InvalidFloatPara;
    m_fAlgaeConGreen  = InvalidFloatPara;
    m_fAlgaeConDino   = InvalidFloatPara;
    m_fAlgaeConHide   = InvalidFloatPara;
    m_fAlgaeConYellow = InvalidFloatPara;
    m_fTransmittance = InvalidFloatPara;// 透光率
    m_fDepth = InvalidFloatPara;// 深度
    m_ttOnBBE = 0;
    m_iBBEStatus = -1;

    m_fCod_Permanganate = InvalidFloatPara;    //  高锰酸盐
    m_fCod_AmmoniaNitrogen = InvalidFloatPara; //  氨氮
    m_fCod_Phosphorus = InvalidFloatPara;      //  总磷
    m_fCod_Nitrogen = InvalidFloatPara;        //  总氮
    m_codStatus = 3; //
    m_ttOnCod = 0;// 数据刷新时间

    //鱼毒性仪
    for (size_t ii=0; ii<def_FishTaximeter1_Data_Num; ii++)
        m_fFishTaximeter1_Data[ii] = InvalidFloatPara;
    m_iFishTaximeter1_Alarm_toxicity           = InvalidIntPara;// 毒性警报
    m_iFishTaximeter1_Alarm_Hardware           = InvalidIntPara;// 硬件警报
    m_iFishTaximeter1_Alarm_flow               = InvalidIntPara;// 流量警报
    m_iFishTaximeter1_Alarm_Odoor              = InvalidIntPara;// 开门警报
    m_iFishTaximeter1_Alarm_Level              = InvalidIntPara;// 警报级别
    m_iFishTaximeter1_Feeding                  = InvalidIntPara;// 喂食
    m_ttOnFishTaximeter1                       = InvalidIntPara;// 数据刷新时间

    m_ttOnQC_SA_1                       = InvalidIntPara;//

    ini_asciiKeyMap_Ch9329(m_asciiKeyMap);
}


CFunSerial::~CFunSerial()
{
    delete m_pSerialBuffer;
    delete m_pSerialCfg;
}

#define LOCK_SERIAL_DEVICE_NAME "/dev/ttyUSB0"

int CFunSerial::Start()
{
    int iret = -1;
    ARG_USED(iret);
    if (m_bHaveStarted)
    {
        return 0;
    }

    CEZLock l(m_LockFunSerial);

    m_bHaveStarted = true;

    // 启动串口控制方式
    if (1)
    {
        int iNumSerialAdded = 0;

// #ifdef _USE_MODULES_EZCONFIG

        m_pSerialCfg->update();

        for(int ii=0; ii<m_pSerialCfg->getNumber(); ii++)
        {
            ez_printf_debug("%2d -- SerialCfg:id=%d, en=%d, name=%s\n"
                            , ii
                            , m_pSerialCfg->getConfig(ii).iChannel
                            , m_pSerialCfg->getConfig(ii).bEnable
                            , m_pSerialCfg->getConfig(ii).strSerialDevice.c_str());

            // do not use
            if (1 != m_pSerialCfg->getConfig(ii).bEnable)
            {
                //ez_printf_info("No %d serial device is not enable.\n", ii);
                ez_printf_info("Not enabled  Serial:%d--%s\n", m_pSerialCfg->getConfig(ii).iChannel, m_pSerialCfg->getConfig(ii).strSerialDevice.c_str());
                //ez_printf_info("Succeeded AddSerial:%d--%s\n", __cfg.id, __cfg.serial_dev_name.c_str());

                continue;
            }

            DEV_SERIAL_ATTR_T __cfg;

            __cfg.baud = m_pSerialCfg->getConfig(ii).baud;// 9600;
            __cfg.data_bits = m_pSerialCfg->getConfig(ii).data_bits;//8;
            __cfg.parity = m_pSerialCfg->getConfig(ii).parity; //0;
            __cfg.stop_bits = m_pSerialCfg->getConfig(ii).stop_bits;//1;
            __cfg.m_pSerial = NULL;

            __cfg.id = m_pSerialCfg->getConfig(ii).iChannel;//GeminiLOCK_SERIAL_DEVICE_ID;// conn at com33
            __cfg.serial_dev_name = m_pSerialCfg->getConfig(ii).strSerialDevice;//LOCK_SERIAL_DEVICE_NAME;

            //PKTH100B 温湿度传感器
            if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_SerialComm) != std::string::npos)
            {
                __cfg.m_stMaxBufferSize = 9; // 最大单条数据长度是9
                __cfg.m_msOndataTimeout = 500; // 最大单条数据长度是9
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_MQPC_10) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                // 不知道的时候适当设置大一点 256
                __cfg.m_stMaxBufferSize = 37; // 最大单条数据长度是9
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_EK_3030E) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 23; // 最大单条数据长度是9
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WP9038ADAM) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 6; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QZ2_TONGLU1) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_FishTaximeter1) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WXT530) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_FishTaximeter_BBE) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_BBE_FLUOROPROBE) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_COD_31) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_COD_HDLC) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 128; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QC_SA_1) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 512; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_NoiseMeter_RSZS_01) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 7; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_NoiseMeter_PG760_01) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 7; // 最大单条数据长度是6
                __cfg.m_msOndataTimeout = 500; //
            }
            // 串口控制键盘
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_CH9329_V1) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 64; // 最大单条数据长度
                __cfg.m_msOndataTimeout = 5; // 数据超时 
            }
            // rfid读卡器
            else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_GXE1009_V1) != std::string::npos)
            {
                // 如果设置为0， 则立刻返回
                __cfg.m_stMaxBufferSize = 64; // 最大单条数据长度
                __cfg.m_msOndataTimeout = 5; // 数据超时
            }

            iret = g_DevSerial.AddSerial(__cfg);
            if (iret >= 0)
            {
                ez_printf_info("Succeeded AddSerial:%d--%s\n", __cfg.id, __cfg.serial_dev_name.c_str());

                iNumSerialAdded++;

                // 数据采集
                if (EZTHREAD_BOOL_FALSE == m_TimerSerialComm.IsStarted()
                    && (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_SerialComm) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_EK_3030E) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_COD_HDLC) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_COD_31) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WP9038ADAM) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QZ2_TONGLU1) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_BBE_FLUOROPROBE) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_MQPC_10) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_FishTaximeter_BBE) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WXT530) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QC_SA_1) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_NoiseMeter_RSZS_01) != std::string::npos
                        ||m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_NoiseMeter_PG760_01) != std::string::npos)
                   )
                {
                    m_TimerSerialComm.Start(this
                                           , (EZ_TIMER_PROC)&CFunSerial::TimerProcPKTH100B
                                           , 0/* dwDueTime*/
                                           , def_SerialFunc_SerialComm_Period * 1000 /*每? 秒采集一次*/
                                           , 0/*param*/
                                           , 10*1000
                                          );

                }

                // 状态驱动
                if (EZTHREAD_BOOL_FALSE == m_TimerStatusEvents.IsStarted()
                    && (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_GXE1009_V1) != std::string::npos)
                    )
                {
                    m_TimerStatusEvents.Start(this
                                        //    , (EZ_TIMER_PROC)&CFunSerial::TimerProcPKTH100B
                                           , (EZ_TIMER_PROC)&CFunSerial::TimerProcGXE1009_V1
                                           , 0/* dwDueTime*/
                                           , def_SerialFunc_StatusEvents_Period * 1000
                                           , 0/*param*/
                                           , 10*1000
                                          );

                    m_iGXE1009_V1_States = EnStatus_GXE1009_V1_Linked;
                }
            }
            else
            {
                ez_printf_error("Failed    AddSerial:%d--%s\n", __cfg.id, __cfg.serial_dev_name.c_str());
            }

        }
// #else
//         DEV_SERIAL_ATTR_T __cfg;

//         __cfg.baud = 9600;// 9600;
//         __cfg.data_bits = 8;//8;
//         __cfg.parity = 0; //0;
//         __cfg.stop_bits = 1;//1;
//         __cfg.m_pSerial = NULL;

//         __cfg.id = GeminiLOCK_SERIAL_DEVICE_ID;// conn at com33
//         __cfg.serial_dev_name = LOCK_SERIAL_DEVICE_NAME;

//         iret = g_DevSerial.AddSerial(__cfg);
//         if (iret >= 0)
//         {
//             ez_printf_info("Succeeded AddSerial:%d--%s\n", __cfg.id, __cfg.serial_dev_name.c_str());

//             iNumSerialAdded++;
//         }
//         else
//         {
//             ez_printf_error("Failed    AddSerial:%d--%s\n", __cfg.id, __cfg.serial_dev_name.c_str());
//         }
// #endif
        if (iNumSerialAdded > 0)// use ...
        {
            ez_printf_info("There have %d serial added\n", iNumSerialAdded);

            EZTHREAD_BOOL bStart = g_DevSerial.Start(this, (CDevSerial::SIG_DevSerial_DATA::SigProc)&CFunSerial::OnSerial);

            if (EZTHREAD_BOOL_TRUE != bStart)
            {
                __trip;
            }
            else
            {}
        }
        else
        {
            ez_printf_error("There have NO serial added\n", iNumSerialAdded);
        }
    }
    else
    {
        __trip;
    }

    return 0;
}

int CFunSerial::Stop()
{
    if (m_bHaveStarted)
    {
        CEZLock l(m_LockFunSerial);

        g_DevSerial.RemoveSerial(LOCK_SERIAL_DEVICE_NAME);

        g_DevSerial.Stop(this, (CDevSerial::SIG_DevSerial_DATA::SigProc)&CFunSerial::OnSerial);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        m_bHaveStarted = false;
    }
    else
    {
        __fline;
        std::cout << "have been started." << std::endl;
    }

    return 0;
}


typedef union FLOAT_CONV
{
    float f;
    int l;// 4Byte, use htonl...
    unsigned char c[4];
}float_conv;
typedef union EZ_U16_CONV
{
    unsigned short s;// 4Byte, use htonl...
    unsigned char c[2];
}ez_u16_conv;

int cpp_fun_call_back(unsigned char *buf, int len, void *arg)
{
    CFunSerial *pObj = (CFunSerial *)arg;
    ARG_USED(pObj);

    //dumpBuffer(stdout, (unsigned char *)buf,
    //           len,
    //           SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    pObj->OnHdlc2(buf, len, arg);

    return 0;
}

int CFunSerial::OnHdlc2(unsigned char *buf, int len, void *arg)
{
    char bufDump[1024];

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("OnHdlc2(%d):\n%s\n", len, bufDump);
    return 0;
}

int CFunSerial::OnCodHdlc(unsigned char *buf, int len, void *arg)
{
    char bufDump[1024];

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("OnHdlc2(%d):\n%s\n", len, bufDump);

    std::string strSerialFunc = (char *)arg;

    if (arg==NULL || strSerialFunc.size()<1)
    {
        ez_printf_error("SerialFunc not defined.\n");
        return 0;
    }

    ez_printf_debug("SerialFunc:%s\n", strSerialFunc.c_str());

    if (buf[0] != 0xfe)
    {
        ez_printf_error("Not hb data(%d), bye.\n", len);

        return 0;
    }

    if (len != 24)
    {
        ez_printf_error("Not hb data(%d), bye.\n", len);

        return 0;
    }

    unsigned char __func = buf[5];
    ez_printf_debug("__func:0x%0x\n", __func);

    if (__func != 0x01
        || len != 24)
    {
        ez_printf_error("Not hb data(%d), bye.\n", len);

        return 0;
    }

    // 温度
    //高字节在前，低字节在后
    unsigned short __shortValue = (((unsigned char)buf[7]) & 0xff);
    __shortValue <<= 8;
    __shortValue = __shortValue |(((unsigned char)buf[6]) & 0xff);

    if(__shortValue == 0xffff)//约定的无效值
    {
        ez_printf_debug("Temperature ignored\n");

        m_fTemperature = InvalidFloatPara;
        //m_ttOnTempHum = time(NULL);// 数据刷新时间
    }
    else if(__shortValue>=2048)//最高位为一说明是个负温
    {
        __shortValue^=0xffff;
        __shortValue += 1;

        m_fTemperature = ((float)__shortValue/10) * (-1);
        m_ttOnTempHum = time(NULL);// 数据刷新时间
    }
    else
    {
        m_fTemperature = ((float)__shortValue/10) ;
        m_ttOnTempHum = time(NULL);// 数据刷新时间
    }

    ez_printf_debug("Temperature:%f\n", m_fTemperature);

    // cod value
    unsigned char *__pdat = (unsigned char *)(buf+14);
    //unsigned char __pdat[8];
    //memcpy(__pdat, buf+14, 8);

    m_ttOnCod = time(NULL);

    //
    if (strSerialFunc.find(def_SerialFunc_COD_HDLC_Permanganate) != std::string::npos)
    {
        __pdat[0]-=0x30;
        __pdat[1]-=0x30;
        __pdat[2]-=0x30;
        __pdat[3]-=0x30;

        unsigned long __ulValue = 0;//ez_bcd_2_dec(__pdat, 4);
        __ulValue += (__pdat[0]*4096);
        __ulValue += (__pdat[1]*256);
        __ulValue += (__pdat[2]*16);
        __ulValue += (__pdat[3]);

        m_fCod_Permanganate = __ulValue/10.0;
        ez_printf_debug("m_fCod_Permanganate:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_Permanganate, __pdat[0], __pdat[1], __pdat[2], __pdat[3]);
    }
    else if (strSerialFunc.find(def_SerialFunc_COD_HDLC_AmmoniaNitrogen) != std::string::npos)
    {
        __pdat[0]-=0x30;
        __pdat[1]-=0x30;
        __pdat[2]-=0x30;
        __pdat[3]-=0x30;

        unsigned long __ulValue = 0;//ez_bcd_2_dec(__pdat, 4);
        __ulValue += (__pdat[0]*4096);
        __ulValue += (__pdat[1]*256);
        __ulValue += (__pdat[2]*16);
        __ulValue += (__pdat[3]);

        m_fCod_AmmoniaNitrogen = __ulValue/100.0;
        ez_printf_debug("m_fCod_AmmoniaNitrogen:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_AmmoniaNitrogen, __pdat[0], __pdat[1], __pdat[2], __pdat[3]);
    }
    else if (strSerialFunc.find(def_SerialFunc_COD_HDLC_PhosphorusNitrogen) != std::string::npos)
    {
        m_codStatus = 3;
        float_conv floatPara;
        //
        floatPara.c[3] = __pdat[0];
        floatPara.c[2] = __pdat[1];
        floatPara.c[1] = __pdat[2];
        floatPara.c[0] = __pdat[3];
        m_fCod_Nitrogen = floatPara.f;
        ez_printf_debug("m_fCod_Nitrogen:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_Nitrogen, __pdat[0], __pdat[1], __pdat[2], __pdat[3]);

        //
        floatPara.c[3] = __pdat[4];
        floatPara.c[2] = __pdat[5];
        floatPara.c[1] = __pdat[6];
        floatPara.c[0] = __pdat[7];
        m_fCod_Phosphorus = floatPara.f;
        ez_printf_debug("m_fCod_Phosphorus:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_Phosphorus, __pdat[4], __pdat[5], __pdat[6], __pdat[7]);

    }
    else
    {
        ez_printf_warning("SerialFunc:%s undefined.\n", strSerialFunc.c_str());
        m_ttOnCod = 0;
    }

    return 0;
}

EZTHREAD_BOOL CFunSerial::OnSerial(int id, const char *pdat, size_t len)
{
    //ez_printf_debug("OnSerial(%d, .., %d)\n", id, len);

    if((pdat == NULL) || (len <= 0))
    {
        ez_printf_error("Data error!\n", id, len);

        return false;
    }

    std::string strSerialFunc = "";

    for(int ii=0; ii<m_pSerialCfg->getNumber(); ii++)
    {
        if (id == m_pSerialCfg->getConfig(ii).iChannel)
        {
            strSerialFunc=m_pSerialCfg->getConfig(ii).strFunction;
        }
    }

    if (strSerialFunc.size()<1)
    {
        ez_printf_error("SerialFunc not defined.\n");
        return EZTHREAD_BOOL_FALSE;
    }

    //ez_printf_debug("SerialFunc:%s\n", strSerialFunc.c_str());

    // PKTH100B智能温湿度变送器
    if (strSerialFunc.find(def_SerialFunc_SerialComm) != std::string::npos)
        //if (len==9)
    {
        DBG(
            for (size_t ii=0; ii<len; ii++)
        {
            printf(" 0x%02x", (unsigned char)pdat[ii])
                ;
            }
            printf("\n");
        );
        // 温度
        //高字节在前，低字节在后
        unsigned short __shortValue = (((unsigned char)pdat[3]) & 0xff);
        __shortValue <<= 8;
        __shortValue = __shortValue |(((unsigned char)pdat[4]) & 0xff);

        if(__shortValue>=2048)//最高位为一说明是个负温
        {
            __shortValue^=0xffff;
            __shortValue += 1;

            m_fTemperature = ((float)__shortValue/10) * (-1);
        }
        else
        {
            m_fTemperature = ((float)__shortValue/10) ;
        }

        ez_printf_debug("Temperature:%f\n", m_fTemperature);

        //湿度
        __shortValue=(((unsigned char)pdat[5]) & 0xff);
        __shortValue<<=8;
        __shortValue=__shortValue |(((unsigned char)pdat[6]) & 0xff);
        m_fHumidity = (float)__shortValue/10;
        ez_printf_debug("Humidity:%f\n", m_fHumidity);

        m_ttOnTempHum = time(NULL);
    }
    // 温度--     精创温度控制器EK-3030E
    else if (strSerialFunc.find(def_SerialFunc_EK_3030E) != std::string::npos)
    {
        //高字节在前，低字节在后
        unsigned short __shortValue = (((unsigned char)pdat[5]) & 0xff);
        __shortValue <<= 8;
        __shortValue = __shortValue |(((unsigned char)pdat[6]) & 0xff);
        m_fTemperature = ((float)__shortValue/10) ;
        m_ttOnTempHum = time(NULL);

        ez_printf_debug("Temperature:%f\n", m_fTemperature);
    }
    else if (strSerialFunc.find(def_SerialFunc_MQPC_10) != std::string::npos)
    {
        DBG(
            for (size_t ii=0; ii<len; ii++)
        {
            fprintf(stderr, " %02x", (unsigned char)pdat[ii])
                ;
            }
            fprintf(stderr, "\n");
        );

        if (len>=37)
        {
            unsigned char u8_DeviceID  = pdat[0];
            unsigned char u8_FuncCode  = pdat[1];
            unsigned char u8_DataBytes = pdat[2];

            ez_printf_debug("DeviceID:%d, FuncCode:%d, DataBytes:%d\n"
                            , u8_DeviceID
                            , u8_FuncCode
                            , u8_DataBytes
                           );

            float_conv floatPara;

            floatPara.c[1] = pdat[3];
            floatPara.c[0] = pdat[4];
            floatPara.c[3] = pdat[5];
            floatPara.c[2] = pdat[6];
            m_fpH = floatPara.f;
            ez_printf_debug("PH:%f\n", m_fpH);

            floatPara.c[1] = pdat[7];
            floatPara.c[0] = pdat[8];
            floatPara.c[3] = pdat[9];
            floatPara.c[2] = pdat[10];
            m_fCond = floatPara.f;
            ez_printf_debug("Cond:%f\n", m_fCond);

            floatPara.c[1] = pdat[11];
            floatPara.c[0] = pdat[12];
            floatPara.c[3] = pdat[13];
            floatPara.c[2] = pdat[14];
            m_fDo = floatPara.f;
            ez_printf_debug("Do:%f\n", m_fDo);

            floatPara.c[1] = pdat[15];
            floatPara.c[0] = pdat[16];
            floatPara.c[3] = pdat[17];
            floatPara.c[2] = pdat[18];
            m_fTurb = floatPara.f;
            ez_printf_debug("Turb:%f\n", m_fTurb);

            floatPara.c[1] = pdat[19];
            floatPara.c[0] = pdat[20];
            floatPara.c[3] = pdat[21];
            floatPara.c[2] = pdat[22];
            m_fTemp = floatPara.f;
            ez_printf_debug("Temp:%f\n", m_fTemp);

            m_ttOnPh = time(NULL);
        }
    }
    else if (strSerialFunc.find(def_SerialFunc_COD_HDLC) != std::string::npos)
    {
#if 0
        static HDLC_HANDLER_T *p_handler = NULL;
        if (NULL == p_handler)
        {
            p_handler = hdlc_new();
        }
        assert(p_handler);
        p_handler->pHdlcCallback = cpp_fun_call_back;

        int iret_hdlc_ondata = hdlc_ondata(p_handler, (unsigned char*)pdat, len, (void *)strSerialFunc.c_str());
        ez_printf_debug("hdlc_ondata:%d\n", iret_hdlc_ondata);
#else

        OnCodHdlc((unsigned char*)pdat, len, (void *)strSerialFunc.c_str());
#endif
        //ez_printf_debug("::CFunSerial::OnSerial(%d, .., %d) HDLC2 func(%s) defined.\n", id, len, strSerialFunc.c_str());
    }
    else if (strSerialFunc.find(def_SerialFunc_WP9038ADAM) != std::string::npos)
    {
        //ez_printf_debug("::CFunSerial::OnSerial(%d, .., %d) WP9038ADAM func(%s) defined.\n", id, len, strSerialFunc.c_str());

        if (pdat[1] == 1)
        {
            m_iDO = (pdat[3]&0x0f);
            ez_printf_debug("::m_iDO:0x%0x\n", m_iDO);
        }
        else if (pdat[1] == 2) //
        {
            m_iDI = (pdat[3]&0x0f);
            ez_printf_debug("::m_iDI:0x%0x\n", m_iDI);
        }

        m_ttOnDIO = time(NULL);
    }
    else if (strSerialFunc.find(def_SerialFunc_WP8027ADAM) != std::string::npos)
    {
        //ez_printf_debug("::CFunSerial::OnSerial(%d, .., %d) WP9038ADAM func(%s) defined.\n", id, len, strSerialFunc.c_str());

        if (pdat[1] == 1)
        {
            m_iDO = (unsigned char)(pdat[3]&0xff);
            m_iDO = m_iDO<<8;

            m_iDO |= (unsigned char)(pdat[4]&0xff);
            ez_printf_debug("::m_iDO:0x%0x\n", m_iDO);
        }

        m_ttOnDIO = time(NULL);
    }
    else if (strSerialFunc.find(def_SerialFunc_QZ2_TONGLU1) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        OnData_QZ2_TONGLU1((unsigned char*)pdat, len, NULL);
    }
    else if (strSerialFunc.find(def_SerialFunc_FishTaximeter1) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        unsigned short devid;
        OnData_FishTaximeter1((unsigned char*)pdat, len, devid);
    }
    else if (strSerialFunc.find(def_SerialFunc_FishTaximeter_BBE) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        unsigned short devid;
        OnData_FishTaximeter_BBE((unsigned char*)pdat, len, devid);
    }
    else if (strSerialFunc.find(def_SerialFunc_WXT530) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        unsigned short devid=id;
        OnData_WXT_530((unsigned char*)pdat, len, devid);
    }
    else if (strSerialFunc.find(def_SerialFunc_QC_SA_1) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        unsigned short devid;
        OnData_QC_SA_1((unsigned char*)pdat, len, devid);
    }
    else if (strSerialFunc.find(def_SerialFunc_NoiseMeter_RSZS_01) != std::string::npos)
    {
#if 0
        char bufDump[512] = {'\0'};
        dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)pdat,
                          len,
                          SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
        ez_printf_debug("OnData_%s(%d):\n%s\n", def_SerialFunc_NoiseMeter_PG760_01, len, bufDump);
#endif

        unsigned short __shortValue = (((unsigned char)pdat[3]) & 0xff);
        __shortValue <<= 8;
        __shortValue = __shortValue |(((unsigned char)pdat[4]) & 0xff);
        m_fNoise = __shortValue/10.0;
        m_ttOnNoise = time(NULL);

        ez_printf_debug("%s:%d(%f), 0x%0x, 0x%0x\n", def_SerialFunc_NoiseMeter_RSZS_01, __shortValue, m_fNoise, (((unsigned char)pdat[3]) & 0xff), (((unsigned char)pdat[4]) & 0xff));
    }
    else if (strSerialFunc.find(def_SerialFunc_NoiseMeter_PG760_01) != std::string::npos)
    {
#if 0
        char bufDump[512] = {'\0'};
        dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)pdat,
                          len,
                          SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
        ez_printf_debug("OnData_%s(%d):\n%s\n", def_SerialFunc_NoiseMeter_PG760_01, len, bufDump);
#endif

        unsigned short __shortValue = (((unsigned char)pdat[3]) & 0xff);
        __shortValue <<= 8;
        __shortValue = __shortValue |(((unsigned char)pdat[4]) & 0xff);
        m_fNoise = __shortValue/10.0;
        m_ttOnNoise = time(NULL);

        ez_printf_debug("%s:%d(%f), 0x%0x, 0x%0x\n", def_SerialFunc_NoiseMeter_PG760_01, __shortValue, m_fNoise, (((unsigned char)pdat[3]) & 0xff), (((unsigned char)pdat[4]) & 0xff));
    }
    else if (strSerialFunc.find(def_SerialFunc_BBE_FLUOROPROBE) != std::string::npos)
    {
        ez_printf_debug("Func:%s\n", strSerialFunc.c_str());

        char bufDump[1024];
#if 0

        dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)pdat,
                          len,
                          SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
        ez_printf_debug("OnData_BBE_FLUOROPROBE(%d):\n%s\n", len, bufDump);
#endif

        snprintf(bufDump, len, "%s", pdat);
        bufDump[len] = '\0';
        trim_blank(bufDump, len);

        ez_printf_debug("OnData_BBE_FLUOROPROBE(%d):\n%s\n", len, bufDump);
#if 0

        if (strstr(bufDump, "141") != NULL)
        {
            m_iBBEStatus = 141;
            ez_printf_debug("++++++++++++++++++++++141\n");
        }
        else if (strstr(bufDump, "181") != NULL)
        {
            m_iBBEStatus = 181;
            ez_printf_debug("++++++++++++++++++++++181\n");
        }
        else if (strstr(bufDump, "191") != NULL)
        {
            // 这里解析结果
            m_iBBEStatus = 191;
            ez_printf_debug("++++++++++++++++++++++191\n");

            m_ttOnBBE = time(NULL);
        }
#endif

        const char *pMark = NULL;
        const char *pTmp = NULL;
        if (strstr(bufDump, "553") != NULL)
        {
            m_ttOnBBE = time(NULL);
            //------------------
            pMark = "C12";// 深度（米）
            pTmp = strstr(bufDump, pMark) ;

            if (pTmp != NULL)
            {
                int okNum = sscanf(pTmp+strlen(pMark), "%f", &m_fDepth);
                if (okNum != 1)
                {
                    m_fDepth = InvalidFloatPara;

                    ez_printf_error("okNum:%d\n", okNum);
                }

                ez_printf_debug("m_fDepth:%f\n", m_fDepth);
            } // dep
            //------------------
            pMark = "C13";// 透光率（%）
            pTmp = strstr(bufDump, pMark) ;

            if (pTmp != NULL)
            {
                int okNum = sscanf(pTmp+strlen(pMark), "%f", &m_fTransmittance);
                if (okNum != 1)
                {
                    m_fTransmittance = InvalidFloatPara;

                    ez_printf_error("okNum:%d\n", okNum);
                }
                else
                {
                    m_fTransmittance = abs(m_fTransmittance);

                    if (m_fTransmittance > 100)
                    {
                        m_fTransmittance = 100;
                    }

                }

                ez_printf_debug("m_fTransmittance:%f\n", m_fTransmittance);
            } // dep
            //------------------
            pMark = "C14";// 藻类
            pTmp = strstr(bufDump, pMark) ;
            float para_5;
            float para_6;
            float para_7;

            if (pTmp != NULL)
            {
                int okNum = sscanf(pTmp+strlen(pMark), "%f %f %f %f %f %f %f %f"
                                   , &m_fAlgaeConGreen
                                   , &m_fAlgaeConBlue
                                   , &m_fAlgaeConDino
                                   , &m_fAlgaeConHide
                                   , &para_5
                                   , &para_6
                                   , &para_7
                                   , &m_fAlgaeConYellow
                                  );
                if (okNum != 8)
                {
                    m_fAlgaeConGreen = InvalidFloatPara;
                    m_fAlgaeConBlue = InvalidFloatPara;
                    m_fAlgaeConDino = InvalidFloatPara;
                    m_fAlgaeConHide = InvalidFloatPara;
                    m_fAlgaeConYellow = InvalidFloatPara;

                    ez_printf_error("okNum:%d\n", okNum);
                }

                ez_printf_debug("m_fAlgaeConGreen:%f\n", m_fAlgaeConGreen);
            } // dep

        }
        // 水温从五参数获取，
        if (strstr(bufDump, "555") != NULL)
        {
            m_ttOnPh = time(NULL); // 用这个值控制上报
            //------------------
            pMark = "C5";// 水温
            pTmp = strstr(bufDump, pMark) ;

            if (pTmp != NULL)
            {
                int okNum = sscanf(pTmp+strlen(pMark), "%f", &m_fTemp);
                if (okNum != 1)
                {
                    m_fTemp = InvalidFloatPara;

                    ez_printf_error("okNum:%d\n", okNum);
                }

                ez_printf_debug("m_fTemp:%f\n", m_fTemp);
            } // temp ch 2
        }// 555
    }
    else if (strSerialFunc.find(def_SerialFunc_COD_31) != std::string::npos)
    {
        ez_printf_debug("Func:%s\n", strSerialFunc.c_str());

        char bufDump[1024];

        dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)pdat,
                          len,
                          SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

        ez_printf_debug("OnData_COD_31(%d):\n%s\n", len, bufDump);

        size_t dataStartAt=0;
        for (dataStartAt=0; dataStartAt<(len-1/**/); dataStartAt++)
        {
            unsigned char __currVal = *(pdat+dataStartAt);
            if (__currVal == 0xfe)
            {
                break;
            }
        }

        do
        {
            // 只有一个开始位，不处理
            if (dataStartAt == (len-1))
            {
                ez_printf_error("data not valid(len:%d, dataStartAt:%d).\n", len, dataStartAt);
                break;
            }

            size_t iCod31PktLen = *(pdat+dataStartAt+1);
            ez_printf_debug("iCod31PktLen:%d\n", iCod31PktLen);

            if (iCod31PktLen+dataStartAt>len)
            {
                ez_printf_error("data not valid(len:%d, dataStartAt:%d, iCod31PktLen:%d).\n", len, dataStartAt, iCod31PktLen);
                break;
            }

            // 包长度包含状态和数值
            if (iCod31PktLen < 12)
            {
                ez_printf_error("data not valid(len:%d, dataStartAt:%d, iCod31PktLen:%d).\n", len, dataStartAt, iCod31PktLen);
                break;
            }

            m_codStatus = (*(pdat+dataStartAt+6)) - 0x30;/*定义的是ascii*/
            ez_printf_debug("m_codStatus(%d)\n", m_codStatus);

            unsigned char *__pdat = (unsigned char *)(pdat+dataStartAt+7);

            m_ttOnCod = time(NULL);

            //
            if (strSerialFunc.find(def_SerialFunc_Cod31_Permanganate) != std::string::npos)
            {
                __pdat[0]-=0x30;
                __pdat[1]-=0x30;
                __pdat[2]-=0x30;
                __pdat[3]-=0x30;

                unsigned long __ulValue = 0;//ez_bcd_2_dec(__pdat, 4);
                __ulValue += (__pdat[0]*4096);
                __ulValue += (__pdat[1]*256);
                __ulValue += (__pdat[2]*16);
                __ulValue += (__pdat[3]);

                m_fCod_Permanganate = __ulValue/10.0;
                ez_printf_debug("m_fCod_Permanganate:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_Permanganate, __pdat[0], __pdat[1], __pdat[2], __pdat[3]);
            }
            else if (strSerialFunc.find(def_SerialFunc_Cod31_AmmoniaNitrogen) != std::string::npos)
            {
                __pdat[0]-=0x30;
                __pdat[1]-=0x30;
                __pdat[2]-=0x30;
                __pdat[3]-=0x30;

                unsigned long __ulValue = 0;//ez_bcd_2_dec(__pdat, 4);
                __ulValue += (__pdat[0]*4096);
                __ulValue += (__pdat[1]*256);
                __ulValue += (__pdat[2]*16);
                __ulValue += (__pdat[3]);

                m_fCod_AmmoniaNitrogen = __ulValue/100.0;
                ez_printf_debug("m_fCod_AmmoniaNitrogen:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_AmmoniaNitrogen, __pdat[0], __pdat[1], __pdat[2], __pdat[3]);
            }
            else if (strSerialFunc.find(def_SerialFunc_Cod31_PhosphorusNitrogen) != std::string::npos)
            {
                // 包长度包含状态和数值
                if (iCod31PktLen < 15)
                {
                    ez_printf_error("data not valid(len:%d, dataStartAt:%d, iCod31PktLen:%d).\n", len, dataStartAt, iCod31PktLen);
                    break;
                }
                __pdat--;// 没有状态字节

                m_codStatus = 3;
                float_conv floatPara;
                //
                floatPara.c[3] = __pdat[0];
                floatPara.c[2] = __pdat[1];
                floatPara.c[1] = __pdat[2];
                floatPara.c[0] = __pdat[3];
                m_fCod_Nitrogen = floatPara.f;
                ez_printf_debug("m_fCod_Nitrogen:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_Nitrogen, __pdat[0], __pdat[1], __pdat[2], __pdat[3]);

                //
                floatPara.c[3] = __pdat[4];
                floatPara.c[2] = __pdat[5];
                floatPara.c[1] = __pdat[6];
                floatPara.c[0] = __pdat[7];
                m_fCod_Phosphorus = floatPara.f;
                ez_printf_debug("m_fCod_Phosphorus:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n", m_fCod_Phosphorus, __pdat[4], __pdat[5], __pdat[6], __pdat[7]);
#if 0

                floatPara.f = 0.08;
                ez_printf_debug("floatPara:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n"
                                , floatPara.f
                                , (unsigned char)floatPara.c[0]
                                , (unsigned char)floatPara.c[1]
                                , (unsigned char)floatPara.c[2]
                                , (unsigned char)floatPara.c[3]);
                floatPara.f = 0.18;
                ez_printf_debug("floatPara:%f, 0x%0x, 0x%0x, 0x%0x, 0x%0x\n"
                                , floatPara.f
                                , (unsigned char)floatPara.c[0]
                                , (unsigned char)floatPara.c[1]
                                , (unsigned char)floatPara.c[2]
                                , (unsigned char)floatPara.c[3]);
#endif

            }
            else
            {
                ez_printf_warning("SerialFunc:%s undefined.\n", strSerialFunc.c_str());
                m_ttOnCod = 0;
            }

        }
        while (0);
    }
    else if (strSerialFunc.find(def_SerialFunc_CH9329_V1) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        unsigned short devid=id;
        OnData_CH9329_V1((unsigned char*)pdat, len, devid);
    }
    else if (strSerialFunc.find(def_SerialFunc_GXE1009_V1) != std::string::npos)
    {
        //ez_printf_debug("Func:%s\n", strSerialFunc.c_str());
        unsigned short devid=id;
        // OnData_WXT_530((unsigned char*)pdat, len, devid);
        OnData_GXE1009_V1((unsigned char*)pdat, len, devid);
    }
    else
    {
        ez_printf_error("Unknow SerialFunc:%s\n", strSerialFunc.c_str());
    }

    return EZTHREAD_BOOL_TRUE;
}

int CFunSerial::OnData_QZ2_TONGLU1(unsigned char *buf, int len, void *arg)
{
#if 0
    char bufDump[1024];

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("OnData_QZ2_TONGLU1(%d):\n%s\n", len, bufDump);
#endif

    int iret = 0;
    int iMinLen = 5; //head, len,

    do
    {

        if (len < iMinLen)
        {
            ez_printf_error("Too short:%d\n", len);

            iret = -1;
            break;
        }

        //整条数据的长度，包括帧头和帧尾
        //数据长度=高字节*256+低字节
        int iPktLen = 256*buf[2] + buf[3];
        ez_printf_debug("%s:%d\n", "iPktLen", iPktLen);

        if (iPktLen > len)
        {
            ez_printf_error("E %s:%d\n", "iPktLen", iPktLen);

            iret = -2;
            break;
        }

        //数据类型个数
        int iDataNum = buf[4];
        ez_printf_debug("%s:%d\n", "iDataNum", iDataNum);

        iMinLen+= (iDataNum*2);
        if (len < iMinLen || iDataNum<1)
        {
            ez_printf_error("%s:%d\n", "iDataNum", iDataNum);

            iret = -3;
            break;
        }

        //std::vector<int> ivOffset;

        for (int ii=0; ii<iDataNum; ii++)
        {
            int iOffset = 256*buf[5+ii*2] + buf[5+ii*2+1];
            ez_printf_debug("%s_%02d:%d\n", "iOffset", ii, iOffset);

            //这里的检查只保证不越界,  允许数据长度为0的情况
            // 数据标识都是两个字节， 一并这里检查 防止下面的越界
            if (iOffset>(iPktLen-2))
            {
                ez_printf_error("%s_%02d:%d\n", "iOffset", ii, iOffset);
                iret = -4;
                break;
            }

            //ivOffset.push_back(iOffset);

            if (buf[iOffset]==0x41 && buf[iOffset+1]==0x30) // 三、	站点信息
            {
                ez_printf_debug("info\n");

                // 20 Byte
                if ((iOffset+20)>(iPktLen-1))
                {
                    ez_printf_error("Len error:%d, %d\n", (iOffset+20), iPktLen);
                    iret = -5;
                    break;
                }

                // OK, but do not use now
                //int __year = (int) ez_bcd_2_dec((unsigned char *)(buf+iOffset+6), 1);
                //ez_printf_debug("%s:%d\n", "__year", __year);
            }
            else if (buf[iOffset]==0x44 && buf[iOffset+1]==0x32) // 五、	多参数MQPC-10（42字节）（D2）
            {
                ez_printf_debug("MQPC-10\n");

                // 42 Byte
                if ((iOffset+42)>(iPktLen-1))
                {
                    ez_printf_error("Len error:%d, %d\n", (iOffset+42), iPktLen);

                    iret = -5;
                    break;
                }

                unsigned char *pdat = (unsigned char *)(buf+iOffset+2);
                {
                    unsigned char u8_DevNum    = pdat[0];
                    unsigned char u8_DevNo       = pdat[1];// 编号
                    unsigned char u8_DevStatus = pdat[2];
                    unsigned char u8_ErrorCode = pdat[3];

                    ez_printf_debug("DevNum:%d, DevNo:%d, DevStatus:%d, ErrorCode:%d\n"
                                    , u8_DevNum
                                    , u8_DevNo
                                    , u8_DevStatus
                                    , u8_ErrorCode
                                   );

                    float_conv floatPara;
                    // 水温
                    floatPara.c[3] = pdat[4];
                    floatPara.c[2] = pdat[5];
                    floatPara.c[1] = pdat[6];
                    floatPara.c[0] = pdat[7];
                    m_fTemp = floatPara.f;

                    //ez_printf_debug("0x%0x 0x%0x 0x%0x 0x%0x\n", pdat[4], pdat[5], pdat[6], pdat[7]);
                    ez_printf_debug("Temp:%f\n", m_fTemp);

                    floatPara.c[3] = pdat[8];
                    floatPara.c[2] = pdat[9];
                    floatPara.c[1] = pdat[10];
                    floatPara.c[0] = pdat[11];
                    m_fCond = floatPara.f;
                    ez_printf_debug("Cond:%f\n", m_fCond);

                    floatPara.c[3] = pdat[12];
                    floatPara.c[2] = pdat[13];
                    floatPara.c[1] = pdat[14];
                    floatPara.c[0] = pdat[15];

                    floatPara.c[3] = pdat[16];
                    floatPara.c[2] = pdat[17];
                    floatPara.c[1] = pdat[18];
                    floatPara.c[0] = pdat[19];
                    m_fDo = floatPara.f;
                    ez_printf_debug("Do:%f\n", m_fDo);

                    floatPara.c[3] = pdat[20];
                    floatPara.c[2] = pdat[21];
                    floatPara.c[1] = pdat[22];
                    floatPara.c[0] = pdat[23];
                    m_fpH = floatPara.f;
                    ez_printf_debug("PH:%f\n", m_fpH);

                    floatPara.c[3] = pdat[24];
                    floatPara.c[2] = pdat[25];
                    floatPara.c[1] = pdat[26];
                    floatPara.c[0] = pdat[27];

                    floatPara.c[3] = pdat[28];
                    floatPara.c[2] = pdat[29];
                    floatPara.c[1] = pdat[30];
                    floatPara.c[0] = pdat[31];
                    m_fTurb = floatPara.f;
                    ez_printf_debug("Turb:%f\n", m_fTurb);

                    m_ttOnPh = time(NULL);
                }
            }
            else if (buf[iOffset]==0x44 && buf[iOffset+1]==0x34) // 六、	GPS信息（22字节）（D4）
            {
                //#           GpsLatitude     纬度
                //#           GpsLongitude    经度
                //#           GpsDateTime     时间
                ez_printf_debug("GPS\n");
            }
            else if (buf[iOffset]==0x44 && buf[iOffset+1]==0x39) // 八、	Vaisala WXT520&WXT530气象站（D9）
            {
                //#           WindSpeed       风速
                //#           WindDirection   风向
                //#           AirPressure     气压
                ez_printf_debug("Vaisala WXT520&WXT530\n");

                // 49 Byte
                if ((iOffset+49)>(iPktLen-1))
                {
                    ez_printf_error("Len error:%d, %d\n", (iOffset+42), iPktLen);

                    iret = -6;
                    break;
                }
                unsigned char *pdat = (unsigned char *)(buf+iOffset+2);
                unsigned short __usValue = pdat[5];
                __usValue = __usValue << 8;
                __usValue += pdat[6];
                m_fWindSpeed = __usValue/100.0;
                ez_printf_debug("WindSpeed:%f\n", m_fWindSpeed);

                __usValue = pdat[7];
                __usValue = __usValue << 8;
                __usValue += pdat[8];
                m_fWindDirection = __usValue/10.0;
                ez_printf_debug("WindDirection:%f\n", m_fWindDirection);

                __usValue = pdat[27];
                __usValue = __usValue << 8;
                __usValue += pdat[28];
                m_fAirPressure = __usValue/10.0;
                ez_printf_debug("AirPressure:%f\n", m_fAirPressure);

                // 温度 3
                __usValue = pdat[33];
                __usValue = __usValue << 8;
                __usValue += pdat[34];
                m_fTemperature_3 = __usValue/10.0-50;
                ez_printf_debug("m_fTemperature_3:%f\n", m_fTemperature_3);

                // 湿度3
                __usValue = pdat[39];
                __usValue = __usValue << 8;
                __usValue += pdat[40];
                m_fHumidity_3 = __usValue;
                ez_printf_debug("m_fHumidity_3:%f\n", m_fHumidity_3);

                // 雨量
                __usValue = pdat[45];
                __usValue = __usValue << 8;
                __usValue += pdat[46];
                m_fRainfall = __usValue;
                ez_printf_debug("m_fRainfall:%f\n", m_fRainfall);


                m_ttOnWeather = time(NULL);
            }
            else if (buf[iOffset]==0x46 && buf[iOffset+1]==0x34) // 九、	BBE藻类分析仪（65字节-F4）
            {
                //#           AlgaeConBlue    藻类浓度-蓝藻
                //#           AlgaeConGreen   藻类浓度-绿藻
                //#           AlgaeConDino    藻类浓度-硅甲藻
                //#           AlgaeConHide    藻类浓度-隐藻
                //#           AlgaeConYellow  藻类浓度-黄色物质
                ez_printf_debug("BBE\n");
                // 65 Bytes
                if ((iOffset+65)>(iPktLen-1))
                {
                    ez_printf_error("Len error:%d, %d\n", (iOffset+65), iPktLen);

                    iret = -6;
                    break;
                }
                unsigned char *pdat = (unsigned char *)(buf+iOffset+2);

                float_conv floatPara;
                int __startB = 31;
                // 蓝藻
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fAlgaeConBlue = floatPara.f;

                //ez_printf_debug("0x%0x 0x%0x 0x%0x 0x%0x\n", pdat[4], pdat[5], pdat[6], pdat[7]);
                ez_printf_debug("m_fAlgaeConBlue:%f\n", m_fAlgaeConBlue);

                //#           AlgaeConGreen   藻类浓度-绿藻
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fAlgaeConGreen = floatPara.f;
                ez_printf_debug("m_fAlgaeConGreen:%f\n", m_fAlgaeConGreen);
                //#           AlgaeConDino    藻类浓度-硅甲藻
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fAlgaeConDino = floatPara.f;
                ez_printf_debug("m_fAlgaeConDino:%f\n", m_fAlgaeConDino);
                //#           AlgaeConHide    藻类浓度-隐藻
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fAlgaeConHide = floatPara.f;
                ez_printf_debug("m_fAlgaeConHide:%f\n", m_fAlgaeConHide);

                //#           AlgaeConYellow  藻类浓度-黄色物质
                __startB=59;
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fAlgaeConYellow = floatPara.f;
                ez_printf_debug("m_fAlgaeConYellow:%f\n", m_fAlgaeConYellow);

                // 深度
                __startB=23;
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fDepth = floatPara.f;
                ez_printf_debug("m_fDepth:%f\n", m_fDepth);

                // 透光率, %
                floatPara.c[3] = pdat[__startB++];
                floatPara.c[2] = pdat[__startB++];
                floatPara.c[1] = pdat[__startB++];
                floatPara.c[0] = pdat[__startB++];
                m_fTransmittance = abs(floatPara.f);

                if (m_fTransmittance > 100)
                {
                    m_fTransmittance = 100;
                }
                ez_printf_debug("m_fTransmittance:%f\n", m_fTransmittance);

                m_ttOnBBE=time(NULL);
            }
            else
            {
                // none of your bussiness
            }
            //
        }
        //以下空白
        break;
    }
    while (0);

    return iret;
}
int CFunSerial::OnData_FishTaximeter1(const unsigned char *buf, int len, unsigned short &devid)
{
DBG(
    char bufDump[1024];

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("%s(%d):\n%s\n", __FUNCTION__, len, bufDump);
);
    int iret = 0;
    int iMinLen = 9+3; //head, tail,

    do
    {

        if (len < iMinLen)
        {
            ez_printf_error("Too short:%d\n", len);

            iret = -1;
            break;
        }

        size_t iDataNum = buf[8];
        ez_printf_debug("%s:%d\n", "iDataNum", iDataNum);
        if (iDataNum < 1)
        {
            ez_printf_error("iDataNum:%d\n", iDataNum);

            iret = -2;
            break;
        }

        if (78 != len)
        {
            ez_printf_error("E %s:%d\n", "Len", len);

            iret = -3;
            break;
        }
        ////6设备地址2字节	(00，01)
        unsigned short __devid = 0;
        //memcpy(&__devid, buf+6, 2);
        __devid = buf[6]*0xff + buf[7];
        devid = __devid;

        ez_printf_debug("%s:%d\n", "__devid", __devid);
        // std::string m_strDevID = std::string(def_DevIdUdpServerPandoraMatrix) + ezConvertToString(__devid);

        size_t ii=0;
        for (ii=0; ii<15; ii++)
        {
            const unsigned char *pdat = (buf+9+ii*4);
            float_conv floatPara;
//#if 0

//            floatPara.c[0] = pdat[0];
//            floatPara.c[1] = pdat[1];
//            floatPara.c[2] = pdat[2];
//            floatPara.c[3] = pdat[3];
//
//            __data[ii] = floatPara.f;
//#else// 通用
            // big endia
            floatPara.c[3] = pdat[0];
            floatPara.c[2] = pdat[1];
            floatPara.c[1] = pdat[2];
            floatPara.c[0] = pdat[3];

            floatPara.l = htonl(floatPara.l);

            m_fFishTaximeter1_Data[ii] = floatPara.f;

//#endif
            //ez_printf_debug("0x%0x 0x%0x 0x%0x 0x%0x\n", pdat[0], pdat[1], pdat[2], pdat[3]);
            //ez_printf_debug("__data[%d]:%f\n", ii, m_fFishTaximeter1_Data[ii]);
        }

        //const unsigned char *pdat = (buf+9+ii*4-1);//-1??
        const unsigned char *pdat = (buf+9+ii*4);
        m_iFishTaximeter1_Alarm_toxicity = *(pdat+0);
        //ez_printf_debug("m_cFishTaximeter1_Alarm_toxicity:%d\n", m_cFishTaximeter1_Alarm_toxicity);
        m_iFishTaximeter1_Alarm_Hardware = *(pdat+1);
        //ez_printf_debug("m_cFishTaximeter1_Alarm_Hardware:%d\n", m_cFishTaximeter1_Alarm_Hardware);
        m_iFishTaximeter1_Alarm_flow = *(pdat+2);
        //ez_printf_debug("m_cFishTaximeter1_Alarm_flow:%d\n", m_cFishTaximeter1_Alarm_flow);
        m_iFishTaximeter1_Alarm_Odoor = *(pdat+3);
        //ez_printf_debug("m_cFishTaximeter1_Alarm_Odoor:%d\n", m_cFishTaximeter1_Alarm_Odoor);
        m_iFishTaximeter1_Alarm_Level = *(pdat+4);
        //ez_printf_debug("m_cFishTaximeter1_Alarm_Level:%d\n", m_cFishTaximeter1_Alarm_Level);
        m_iFishTaximeter1_Feeding = *(pdat+5);
        //ez_printf_debug("m_cFishTaximeter1_Feeding:%d\n", m_cFishTaximeter1_Feeding);

        m_ttOnFishTaximeter1 = time(NULL);
    }
    while (0);

    return iret;
}
int CFunSerial::OnData_WXT_530(const unsigned char *buf, int len, unsigned short &devid)
{
    char bufDump[1024] = {0};

    memcpy(bufDump, buf, len);

DBG(
    ez_printf_debug("[%s]\n", bufDump);
    ez_printf_debug("devid:%d\n", devid);

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("%s(%d):\n%s\n", __FUNCTION__, len, bufDump);
);

    {
        const char *pMark = "Sm=";
        const char *pPosition = strstr(bufDump, pMark);
        if (pPosition)
        {
            /*float _fWindSpeed*/m_fWindSpeed = atof(pPosition+strlen(pMark));
            m_ttOnWeather = time(NULL);
            ez_printf_debug("%s:%f\n", "m_fWindSpeed", m_fWindSpeed);
        }
    }

    {
        const char *pMark = "Dm=";
        const char *pPosition = strstr(bufDump, pMark);
        if (pPosition)
        {
            m_fWindDirection = atof(pPosition+strlen(pMark));
            m_ttOnWeather = time(NULL);
            ez_printf_debug("%s:%f\n", "m_fWindDirection", m_fWindDirection);
        }
    }

    {
        const char *pMark = "Pa=";
        const char *pPosition = strstr(bufDump, pMark);
        if (pPosition)
        {
            m_fAirPressure = atof(pPosition+strlen(pMark));
            m_ttOnWeather = time(NULL);
            ez_printf_debug("%s:%f\n", "m_fAirPressure", m_fAirPressure);
        }
    }
    {
        const char *pMark = "Ta=";
        const char *pPosition = strstr(bufDump, pMark);
        if (pPosition)
        {
            m_fTemperature_3 = atof(pPosition+strlen(pMark));
            m_ttOnWeather = time(NULL);
            ez_printf_debug("%s:%f\n", "m_fTemperature_3", m_fTemperature_3);
        }
    }

    {
        const char *pMark = "Ua=";
        const char *pPosition = strstr(bufDump, pMark);
        if (pPosition)
        {
            m_fHumidity_3 = atof(pPosition+strlen(pMark));
            m_ttOnWeather = time(NULL);
            ez_printf_debug("%s:%f\n", "m_fHumidity_3", m_fHumidity_3);
        }
    }
    {
        const char *pMark = "Ri=";
        const char *pPosition = strstr(bufDump, pMark);
        if (pPosition)
        {
            m_fRainfall = atof(pPosition+strlen(pMark));
            m_ttOnWeather = time(NULL);
            ez_printf_debug("%s:%f\n", "m_fRainfall", m_fRainfall);
        }
    }

    return 0;
}

#pragma pack(1)
typedef struct __rfid_protocol_t
{
    uint8_t mark; //协议起始标识
    uint32_t cmd;
    uint16_t len; // 仅指数据长度， 为总长度 - 1(mark) - 4(cmd) -2(len) -2(crc)
    uint8_t data[0];
} rfid_protocol_t;
#pragma pack()

#include <arpa/inet.h>

int CFunSerial::OnData_GXE1009_V1(const unsigned char *buf, const size_t len, unsigned short &devid)
{
    if (NULL == buf || len < sizeof(rfid_protocol_t) /*头+data+crc， 1+4+2字节以上*/)
    {
        ez_printf_error("data not valid. len:%d\n", len);
        return -1;
    }


    bool dumpBuf = false; // 是否打印串口数据
    ARG_USED(dumpBuf);


    if (buf[0] != 0x5a)
    {
        return -1;
    }

    unsigned short crcCal = ez_crc16_crc16_xmoden((unsigned char *)(buf+1), len-3);
    unsigned short crcGet = buf[len-2];
    crcGet = (crcGet<<8);
    crcGet |= (buf[len-1]);

    if (crcCal!=crcGet)
    {
        // ez_printf_error("CRC error:crcCal:0x%0x crcGet:0x%0x\n", crcCal, crcGet);
        return -1;
    }

    rfid_protocol_t onFrame;
    memcpy(&onFrame, buf, sizeof(rfid_protocol_t));

    // unsigned int ctrlGet = (unsigned int)(*(buf + 1));
    // memcpy(&ctrlGet, (buf + 1), 4);

    // float_conv floatPara;
    // //
    // floatPara.c[0] = buf[1];
    // floatPara.c[1] = buf[2];
    // floatPara.c[2] = buf[3];
    // floatPara.c[3] = buf[4];

    // // ctrlGet
    // ez_printf_debug("ctrlGet:0x%0x, floatPara.l:0x%0x\n", ctrlGet, floatPara.l);

    // MsgAppGetBaseVersion
    if (buf[1] == 0x00 && buf[2] == 0x01 && buf[3] == 0x01 && buf[4] == 0x01)
    {
        ez_printf_debug("on MsgAppGetBaseVersion\n");
        m_iGXE1009_V1_States = EnStatus_GXE1009_V1_GetBaseVersion_Done;
    }
    // MsgBaseGetCapabilities
    else if (buf[1] == 0x00 && buf[2] == 0x01 && buf[3] == 0x02 && buf[4] == 0x00)
    {
        ez_printf_debug("on MsgBaseGetCapabilities\n");
        m_iGXE1009_V1_States = EnStatus_GXE1009_V1_MsgBaseGetCapabilities_Done;
    }
    // MsgBaseStop
    else if (buf[1] == 0x00 && buf[2] == 0x01 && buf[3] == 0x02 && buf[4] == 0xff)
    {
        ez_printf_debug("on MsgBaseStop\n");
        m_iGXE1009_V1_States = EnStatus_GXE1009_V1_MsgBaseStop_Done;
    }
    // EnStatus_GXE1009_V1_MsgBaseInventoryEpc
    else if (buf[1] == 0x00 && buf[2] == 0x01 && buf[3] == 0x02 && buf[4] == 0x10)
    {
        ez_printf_debug("on MsgBaseInventoryEpc\n");
        m_iGXE1009_V1_States = EnStatus_GXE1009_V1_MsgBaseInventoryEpc_Done;
    }
    // TagLog
    else if (buf[1] == 0x00 && buf[2] == 0x01 && buf[3] == 0x12 && buf[4] == 0x00)
    {
        // dumpBuf = true;// 调试中， 打印数据
        // ez_u16_conv EPCID;
        // EPCID.c[0] = buf[9];
        // EPCID.c[1] = buf[10];

        ez_u16_conv tempreture;
        tempreture.c[1] = buf[19];
        tempreture.c[0] = buf[20];
        ez_printf_debug("On TagLog:T(%d), Epcid(%02x%02x), ANT(%d)\n", tempreture.s, buf[9], buf[10], buf[13]);

        m_iGXE1009_V1_States = EnStatus_GXE1009_V1_MsgBaseInventoryEpc_Done; // 这个也算是发送了。。
    }

    if (dumpBuf)
    {
        char bufDump[1024] = {0};

        dumpBuffer2Buffer(bufDump, sizeof(bufDump) - 1, (unsigned char *)buf,
                          len,
                          SHOW_BINAR);
                        //   SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

        ez_printf_debug("%s(%d):\n%s\n", __FUNCTION__, len, bufDump);
    }

    return 0;
}

int CFunSerial::OnData_CH9329_V1(const unsigned char *buf, const size_t len, unsigned short &devid)
{
    if (NULL == buf || len < 3 /*头+data+crc， 1+4+2字节以上*/)
    {
        ez_printf_error("data not valid. len:%d\n", len);
        return -1;
    }


    bool dumpBuf = false; // 是否打印串口数据
    ARG_USED(dumpBuf);

    return 0;
}

#define def_bbeString_Len_max 20
int CFunSerial::OnData_FishTaximeter_BBE(const unsigned char *buf, int len, unsigned short &devid)
{
DBG(
    char bufDump[1024] = {0};

    memcpy(bufDump, buf, len);
    ez_printf_debug("[%s]\n", bufDump);

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("%s(%d):\n%s\n", __FUNCTION__, len, bufDump);
);

    //float         m_fFishTaximeter1_Data[def_FishTaximeter1_Data_Num];
    //m_iFishTaximeter1_Alarm_toxicity           ;// 毒性警报
    //m_iFishTaximeter1_Alarm_Hardware           ;// 硬件警报
    //m_iFishTaximeter1_Alarm_flow               ;// 流量警报
    //m_iFishTaximeter1_Alarm_Odoor              ;// 开门警报
    //m_iFishTaximeter1_Alarm_Level              ;// 警报级别
    //m_iFishTaximeter1_Feeding                  ;// 喂食

    char *pBBE_FishData=(char *)buf;
    pBBE_FishData[len-1] = '\0';
    ez_printf_debug("[%s]\n", pBBE_FishData);

    char strName[def_bbeString_Len_max] =  {0};
    char strDate[def_bbeString_Len_max] =  {0};
    char strTime[def_bbeString_Len_max] =  {0};
    char strAmPm[def_bbeString_Len_max] =  {0};
    char strValue_1[def_bbeString_Len_max] =  {0};

    sscanf( pBBE_FishData, "%s %f %f %s %s %s %s %f %f %f %f %f %f %f %f %f %f %f %f %f" \
            , strName
            , m_fFishTaximeter1_Data+0
            , m_fFishTaximeter1_Data+1
            , strDate
            , strTime
            , strAmPm
            , strValue_1
            , m_fFishTaximeter1_Data+2
            , m_fFishTaximeter1_Data+3
            , m_fFishTaximeter1_Data+4
            , m_fFishTaximeter1_Data+5
            , m_fFishTaximeter1_Data+6
            , m_fFishTaximeter1_Data+7
            , m_fFishTaximeter1_Data+8
            , m_fFishTaximeter1_Data+9
            , m_fFishTaximeter1_Data+10
            , m_fFishTaximeter1_Data+11
            , m_fFishTaximeter1_Data+12
            , m_fFishTaximeter1_Data+13
            , m_fFishTaximeter1_Data+14
          );

    m_ttOnFishTaximeter1 = time(NULL);

    return 0;
}

int CFunSerial::OnData_QC_SA_1(const unsigned char *buf, int len, unsigned short &devid)
{
#if 1
    char bufDump[1024];

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("%s(%d):\n%s\n", __FUNCTION__, len, bufDump);
#endif

    int iret = 0;
    int iMinLen = 8; //HB

    do
    {

        if (len < iMinLen)
        {
            ez_printf_error("Too short:%d\n", len);

            iret = -1;
            break;
        }

        size_t iDataNum = buf[1];
        ez_printf_debug("%s:%d\n", "iDataNum", iDataNum);
        if (iDataNum < 1)
        {
            ez_printf_error("iDataNum:%d\n", iDataNum);

            iret = -2;
            break;
        }

        // 单片机地址
        unsigned short __devid = 0;
        __devid = buf[4];
        devid = __devid;

        ez_printf_debug("%s:%d\n", "__devid", __devid);

        unsigned char __cmd = buf[5];
        if (0x01 == __cmd) // 0x01(通信心跳)
        {
            // 30总计字节 但是现在有34字节
            //if (30 != len)
            if (len < 30)
            {
                iret = -4;

                ez_printf_error("data len(%d) not valid, should be 30.\n", len);

                break;
            }
            //0xXX（质控主机状态：0：空闲； 1：忙）
            unsigned char __status = buf[6];
            ez_printf_debug("%s:%d\n", "__status", __status);
            //0xXX(返回结果0：执行成功；1：正在执行；>1：执行失败)
            unsigned char __execResult = buf[7];
            ez_printf_debug("%s:%d\n", "__execResult", __execResult);
            //0xXX(纯水水位0：无1：有)
            unsigned char __waterLevel_pure = buf[8];
            ez_printf_debug("%s:%d\n", "__waterLevel_pure", __waterLevel_pure);
            //0xXX(母液水位0：无1：有)
            unsigned char __waterLevel_Mother = buf[9];
            ez_printf_debug("%s:%d\n", "__waterLevel_Mother", __waterLevel_Mother);
            //0xXX (温度数据低字节) 0xXX (温度数据高字节)
            //0xXX(TP数据类型0：标液；1：质控) 0xXX, 0xXX,0xXX,0xXX(TP数据)
            //0xXX(TN数据类型0：标液；1：质控) 0xXX, 0xXX,0xXX,0xXX(TN数据)
            //0xXX(NH4数据类型0：标液；1：质控) 0xXX, 0xXX,0xXX,0xXX(NH4数据)
            //0xXX(COD数据类型0：标液；1：质控) 0xXX, 0xXX,0xXX,0xXX(COD数据)
        } // 0x01(通信心跳)

        m_ttOnQC_SA_1 = time(NULL);
    }
    while (0);

    return iret;
}

int CFunSerial::Sendto(int iSerialID, char *pBuff, size_t iBuffLen)
{
    CEZLock l(m_LockSerialOpr);

    int iret = g_DevSerial.SendData(iSerialID, (uint8_t *)pBuff, iBuffLen);
    return iret;
}

int CFunSerial::Sendto_Modbus(int iSerialID, char *pBuff, size_t iBuffLen)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || iSerialID < 0)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return 0;
    }

    int iret = 0;
    char *buftmp = (char *)malloc(iBuffLen+2);
    if (buftmp==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    unsigned short usret = ez_modbus_crc16((unsigned char *)pBuff, iBuffLen);

    memcpy(buftmp, pBuff, iBuffLen);
    buftmp[iBuffLen] = (usret>>8)&0xff;
    buftmp[iBuffLen+1] = usret&0xff;

    iret = Sendto(iSerialID, buftmp, iBuffLen+2);

    ez_printf_debug("::%d == Sendto(%d, %d)\n", iret, iSerialID, iBuffLen+2);

    free (buftmp);
    return iret;
}

int CFunSerial::Sendto_CH9329(const int iSerialID, const unsigned char *pBuff, const size_t iBuffLen)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || iSerialID < 0)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return -1;
    }

    int iret = 0;
    unsigned char *buftmp = (unsigned char *)malloc(iBuffLen+1);
    assert(buftmp);

    memcpy(buftmp, pBuff, iBuffLen);
    buftmp[iBuffLen]=0;
    for (size_t i = 0; i < iBuffLen; i++)
    {
        buftmp[iBuffLen]+=pBuff[i];
    }
    ez_printf_debug("buftmp[iBuffLen]:0x%0x\n", buftmp[iBuffLen]);

    iret = Sendto(iSerialID, (char *)buftmp, iBuffLen+1);

    ez_printf_debug("::%d == Sendto(%d, %d)\n", iret, iSerialID, iBuffLen+2);

    free(buftmp);
    buftmp = NULL;

    return iret;
}

// CMD_SEND_MS_REL_DATA
// 相对鼠标数据包
int CFunSerial::Sendto_CH9329_mouseKey(const int iSerialID,
                                       const unsigned char mouseKey,
                                       const unsigned char xMove,
                                       const unsigned char yMove,
                                       const unsigned char rollerMove)
{
    ez_printf_debug("%s-[%s:0x%0x],[%s:0x%0x],[%s:0x%0x],[%s:0x%0x]\n"
        , __FUNCTION__
        , "mouseKey", mouseKey
        , "xMove", xMove
        , "yMove", yMove
        , "rollerMove", rollerMove);

    // unsigned char mouseKey = 0;
    // unsigned char xMove = 0;
    // unsigned char yMove = 0;
    // unsigned char rollerMove = 0;
    int iret = 0;

    unsigned char mouseKeyValue[] = {0x57,0xAB,0x00,0x05,0x05,0x01, mouseKey, xMove, yMove, rollerMove/*,sum*/};

    iret = Sendto_CH9329(iSerialID, mouseKeyValue, sizeof(mouseKeyValue)); //

    if (iret > 0 && mouseKey > 0 && mouseKey <= 4)
    {
        unsigned char mouseKeyValueUP[] = {0x57,0xAB,0x00,0x05,0x05,0x01, 0, 0, 0, 0/*,sum*/};
        iret = Sendto_CH9329(iSerialID, mouseKeyValueUP, sizeof(mouseKeyValueUP)); //
        iret = Sendto_CH9329(iSerialID, mouseKeyValueUP, sizeof(mouseKeyValueUP)); //
    }

    return iret;
}

// 发送普通键盘数据 CMD_SEND_KB_GENERAL_DATA
// usualKey 对应文档中 - HID Code， 比如a的HIDCode是4

int CFunSerial::Sendto_CH9329_keyVal(const int iSerialID, const unsigned char usualKey, const unsigned char controlKey)
{
    ez_printf_debug("%s-usualKey:0x%0x\n", __FUNCTION__, usualKey);
    int iret = 0;

    unsigned char keyPress[] = {0x57, 0xAB, 0x00, 0x02, 0x08, controlKey, 0x00, usualKey, 0x00, 0x00, 0x00, 0x00, 0x00 /*,sum*/};
    keyPress[5] = controlKey;
    keyPress[7] = usualKey;
    iret = Sendto_CH9329(iSerialID, keyPress, sizeof(keyPress)); //

    if (iret > 0)
    {
        unsigned char keyRelease[] = {0x57, 0xAB, 0x00, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 /*,0x0C*/};
        iret = Sendto_CH9329(iSerialID, keyRelease, sizeof(keyRelease)); //
        iret = Sendto_CH9329(iSerialID, keyRelease, sizeof(keyRelease)); //
    }
    return 0;
}

int CFunSerial::Sendto_CH9329_String(const int iSerialID, const char *strInput, const size_t strLen)
{
    // 发送小写字母
    for (size_t i = 0; i < strLen; i++)
    {
        #if 0
        if (strInput[i] >= 'a' && strInput[i] <= 'z')
        {
            this->Sendto_CH9329_keyVal(iSerialID, strInput[i] - 'a' + 4);
        }
        else if (strInput[i] >= 'A' && strInput[i] <= 'Z')
        {
            this->Sendto_CH9329_keyVal(iSerialID, strInput[i] - 'A' + 4, 0x02 /*Shift*/);
        }
        else if (strInput[i] == ' ')
        {
            this->Sendto_CH9329_keyVal(iSerialID, 0x2c);
        }
        else if (strInput[i] == '\n')
        {
            this->Sendto_CH9329_keyVal(iSerialID, 0x40);
        }
        else
        {
            ez_printf_debug("unsupport:0x%0x\n", strInput[i]);
        }
        #endif

        unsigned char usualKey = getUsualKey(m_asciiKeyMap, strInput[i]);
        if (usualKey != 0)
        {
            unsigned char controlKey = getUsualKey_NeedShift(strInput[i]);
            this->Sendto_CH9329_keyVal(iSerialID, usualKey, controlKey);
        }
        else
        {
            ez_printf_debug("unsupport:0x%0x\n", strInput[i]);
        }
    }

    return 0;
}

int CFunSerial::Sendto_CH9329_AsciiCtrl(const int iSerialID, std::string strCtrl)
{
    unsigned char ctrlKey = getControlKey(strCtrl);

    if (ctrlKey != 0)
        this->Sendto_CH9329_keyVal(iSerialID, ctrlKey);
        
    return 0;
}

// GXE1009_V1
int CFunSerial::Sendto_xmoden(int iSerialID, const unsigned char *pBuff, size_t iBuffLen, const size_t iSkipHead)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || iSerialID < 0)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return 0;
    }

    int iret = 0;
    char *buftmp = (char *)malloc(iBuffLen+2);
    if (buftmp==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    unsigned short usret = ez_crc16_crc16_xmoden((unsigned char *)pBuff+iSkipHead, iBuffLen-iSkipHead);

    memcpy(buftmp, pBuff, iBuffLen);
    buftmp[iBuffLen] = (usret>>8)&0xff;
    buftmp[iBuffLen+1] = usret&0xff;

    iret = Sendto(iSerialID, buftmp, iBuffLen+2);

    ez_printf_debug("::%d == Sendto(%d, %d)\n", iret, iSerialID, iBuffLen+2);

    free (buftmp);
    return iret;
}


int CFunSerial::Sendto_Modbus_TNP(int iSerialID, char *pBuff, size_t iBuffLen)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || iSerialID < 0)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return 0;
    }

    int iret = 0;
    char *buftmp = (char *)malloc(iBuffLen+2);
    if (buftmp==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    unsigned short usret = ez_modbus_crc16((unsigned char *)pBuff, iBuffLen);

    memcpy(buftmp, pBuff, iBuffLen);

    buftmp[iBuffLen+1] = (usret>>8)&0xff;
    buftmp[iBuffLen] = usret&0xff;

    iret = Sendto(iSerialID, buftmp, iBuffLen+2);

    ez_printf_info("::%d == Sendto(%d, %d)\n", iret, iSerialID, iBuffLen+2);

    free (buftmp);
    return iret;
}


int CFunSerial::SendtoBase64(const int iSerialID, const char *pBuffBase64)
{
    if (pBuffBase64==NULL
        || *pBuffBase64=='\0'
        || iSerialID < 0)
    {
        ez_printf_error("error CFunSerial::SendtoBase64\n");

        return 0;
    }

    ez_printf_info(":: CFunSerial::SendtoBase64(%d, %s)\n", iSerialID, pBuffBase64);

    int iret = 0;

    int data_len = ez_base64decode_len(pBuffBase64);

    char *buftmp = (char *)malloc(data_len);

    if (buftmp==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    memset(buftmp, 0, data_len);

    int iret_ez_base64decode = ez_base64decode(buftmp, pBuffBase64);

    ez_printf_info("%d::%d -- (%s)::\n", iret_ez_base64decode, data_len, pBuffBase64);

    if (iret_ez_base64decode > 0)// real datalen
    {
        iret = Sendto(iSerialID, buftmp, iret_ez_base64decode);

        ez_printf_info("::%d == SendData(%d, %d)\n", iret, iSerialID, iret_ez_base64decode);
    }

    free (buftmp);

    return iret;
}

int CFunSerial::Sendto(const std::string &strSerialFunc, char *pBuff, size_t iBuffLen)
{
    int iret = -1;

    for(int ii=0; ii<m_pSerialCfg->getNumber(); ii++)
    {
        if (1 == m_pSerialCfg->getConfig(ii).bEnable
            && m_pSerialCfg->getConfig(ii).strFunction.find(strSerialFunc) != std::string::npos)
        {
            iret = this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, pBuff, iBuffLen);
        }
    }

    return iret;
}

int CFunSerial::Sendto_Modbus(const std::string &strSerialFunc, char *pBuff, size_t iBuffLen)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || strSerialFunc.length() < 1)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return 0;
    }

    int iret = 0;
    char *buftmp = (char *)malloc(iBuffLen+2);
    if (buftmp==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    unsigned short usret = ez_modbus_crc16((unsigned char *)pBuff, iBuffLen);

    memcpy(buftmp, pBuff, iBuffLen);
    buftmp[iBuffLen] = (usret>>8)&0xff;
    buftmp[iBuffLen+1] = usret&0xff;

    iret = Sendto(strSerialFunc, buftmp, iBuffLen+2);

    ez_printf_debug("::%d == Sendto(%s, %d)\n", iret, strSerialFunc.c_str(), iBuffLen+2);

    free (buftmp);
    return iret;
}

int CFunSerial::Sendto_Hdlc2(int iSerialID, char *pBuff, size_t iBuffLen)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || iSerialID < 0)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return 0;
    }

    int iret = 0;
    int dest_len = ez_hdlc_get_dst_buf_len((unsigned char *)pBuff, (int)iBuffLen, EZ_HDLC_MODE_VERIFY_MOD_BUS);

    char *dest = (char *)malloc(dest_len);
    if (dest ==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    iret = ez_hdlc_make_packet((unsigned char *)dest, (int)dest_len, (unsigned char *)pBuff, (int)iBuffLen, EZ_HDLC_MODE_VERIFY_MOD_BUS);

    iret = Sendto(iSerialID, dest, dest_len);

    ez_printf_debug("::%d == Sendto(%d, %d)\n", iret, iSerialID, dest_len);

    free (dest);
    return iret;
}


int CFunSerial::Sendto_Hdlc2(const std::string &strSerialFunc, char *pBuff, size_t iBuffLen)
{
    if (pBuff == NULL
        || iBuffLen <= 0
        || strSerialFunc.length() < 1)
    {
        ez_printf_error("error %s\n", __FUNCTION__);

        return 0;
    }

    int iret = 0;
    int dest_len = ez_hdlc_get_dst_buf_len((unsigned char *)pBuff, (int)iBuffLen, EZ_HDLC_MODE_VERIFY_MOD_BUS);

    char *dest = (char *)malloc(dest_len);
    if (dest ==NULL)
    {
        ez_printf_error("error !\n");
        return -1;
    }

    iret = ez_hdlc_make_packet((unsigned char *)dest, (int)dest_len, (unsigned char *)pBuff, (int)iBuffLen, EZ_HDLC_MODE_VERIFY_MOD_BUS);

    iret = Sendto(strSerialFunc, dest, dest_len);

    ez_printf_debug("::%d == Sendto(%s, %d)\n", iret, strSerialFunc.c_str(), dest_len);

    free (dest);
    return iret;
}

void CFunSerial::TimerProcGXE1009_V1()
{
    for (int ii = 0; ii < m_pSerialCfg->getNumber(); ii++)
    {
        if (1 == m_pSerialCfg->getConfig(ii).bEnable 
            && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_GXE1009_V1) != std::string::npos)
        {
            // m_iGXE1009_V1_States
            // 第一步
            if (m_iGXE1009_V1_States < EnStatus_GXE1009_V1_GetBaseVersion_Done)
            {
                ez_printf_debug("EnStatus_GXE1009_V1_GetBaseVersion\n");

                unsigned char aaa[] = {0x5A, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00};
                this->Sendto_xmoden(m_pSerialCfg->getConfig(ii).iChannel, aaa, sizeof(aaa));
            }
            else if (m_iGXE1009_V1_States < EnStatus_GXE1009_V1_MsgBaseGetCapabilities_Done)
            {
                ez_printf_debug("EnStatus_GXE1009_V1_MsgBaseGetCapabilities\n");

                unsigned char aaa[] = {0x5A, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00};
                this->Sendto_xmoden(m_pSerialCfg->getConfig(ii).iChannel, aaa, sizeof(aaa));
            }
            else if (m_iGXE1009_V1_States < EnStatus_GXE1009_V1_MsgBaseStop_Done)
            {
                ez_printf_debug("EnStatus_GXE1009_V1_MsgBaseStop\n");

                unsigned char aaa[] = {0x5A, 0x00, 0x01, 0x02, 0xFF, 0x00, 0x00};
                this->Sendto_xmoden(m_pSerialCfg->getConfig(ii).iChannel, aaa, sizeof(aaa));
            }
            else if (m_iGXE1009_V1_States < EnStatus_GXE1009_V1_MsgBaseInventoryEpc_Done)
            {
                ez_printf_debug("EnStatus_GXE1009_V1_MsgBaseInventoryEpc\n");

                // 读取第一个
                // unsigned char aaa[] = {0x5A, 0x00, 0x01, 0x02, 0x10, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x01, 0x12, 0x02};
                // 读取全部
                unsigned char aaa[] = {0x5A, 0x00, 0x01, 0x02, 0x10, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x01, 0x12, 0x02};
                this->Sendto_xmoden(m_pSerialCfg->getConfig(ii).iChannel, aaa, sizeof(aaa));
            }
        }
    }
}

void CFunSerial::TimerProcPKTH100B()
{
    for(int ii=0; ii<m_pSerialCfg->getNumber(); ii++)
    {
        if (1 == m_pSerialCfg->getConfig(ii).bEnable
            && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_SerialComm) != std::string::npos)
        {
            char bufGet[] = {0x01, 0x03, 0x00 , 0x00 , 0x00 , 0x02};

            this->Sendto_Modbus(m_pSerialCfg->getConfig(ii).iChannel, bufGet, sizeof(bufGet));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_MQPC_10) != std::string::npos)
        {
            //五参数读取
            unsigned char bufGet2[] = {0x01, 0x03, 0x00 , 0x10 , 0x00, 0x10, 0x45, 0xc3};

            this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet2, sizeof(bufGet2));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_EK_3030E) != std::string::npos)
        {
            //精创温度控制器
            unsigned char bufGet2[] = {0x02, 0x42, 0x09, 0x00, 0x00, 0x99, 0xBA};

            this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet2, sizeof(bufGet2));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_COD_HDLC) != std::string::npos)
        {
            ez_printf_debug("::%s\n", def_SerialFunc_COD_HDLC);

            //质控控制器
            //unsigned char bufGet2[] = {0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            // 高锰酸盐心跳
            unsigned char bufGet2[] = {0x01, 0x02, 0x01, 0x01, 0x04};

            this->Sendto_Hdlc2(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet2, sizeof(bufGet2));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WP9038ADAM) != std::string::npos)
        {
            ez_printf_debug("::%s\n", def_SerialFunc_WP9038ADAM);

            //IO控制器- get DO
            unsigned char bufGet_DO[] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x04, 0x3D, 0xC9};
            //IO控制器- get DI
            unsigned char bufGet_DI[] = {0x01, 0x02, 0x00, 0x00, 0x00, 0x04, 0x79, 0xC9};
            static unsigned int sndT = 0;
            if (sndT%2==0)
            {
                this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet_DO, sizeof(bufGet_DO));
                ez_printf_debug("::%s\n", "bufGet_DO");
            }
            else
            {
                this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet_DI, sizeof(bufGet_DI));
                ez_printf_debug("::%s\n", "bufGet_DI");
            }
            sndT++;
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WP8027ADAM) != std::string::npos)
        {
            ez_printf_debug("::%s\n", def_SerialFunc_WP8027ADAM);

            //IO控制器- get DO
            unsigned char bufGet_DO[] = {0x01, 0x01, 0x00, 0x00, 0x00, 0x10, 0x3D, 0xC6};

            this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet_DO, sizeof(bufGet_DO));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QZ2_TONGLU1) != std::string::npos)
        {
            //桐庐综合取证板
            //unsigned char bufGet2[] = {0x02, 0x42, 0x09, 0x00, 0x00, 0x99, 0xBA};

            //this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGet2, sizeof(bufGet2));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_BBE_FLUOROPROBE) != std::string::npos)
        {
            static time_t __ttLastSnd = time(NULL);
            time_t __ttNow = time(NULL);

            if (__ttNow>__ttLastSnd+60)//XXs 定时器
            {
                //BBE藻类传感器
                const char *bufBBE[] =
                    {//"000\n\r" // 测试连接
                        //, "134 1 702\n\r"      // 激活自动拟合计算
                        "140\n\r" // 开始测量
//                        "140\r\n" // 开始测量
                        //, "180\n\r" // 开始测量
                        //, "190\n\r" // 开始计算
                    };

                int bufBBE_Size = sizeof(bufBBE)/sizeof(const char *);

                static  int iSnd = 0;
                const char *bufGetBBE=NULL;
                bufGetBBE = bufBBE[iSnd%bufBBE_Size];

                int iret_sndto = this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGetBBE, strlen(bufGetBBE));
                __ttLastSnd = time(NULL);

                ez_printf_debug("%d, Send(%d) of %s %d -- %s\n", bufBBE_Size, iSnd, "BBE_FLUOROPROBE", iret_sndto, bufGetBBE);

                // 最后一个 重复发送
                //if (iSnd<(bufBBE_Size-1))
                iSnd++;
            }// XXs 定时器
if (0)
{
            static time_t __ttLastSnd2 = time(NULL);
            time_t __ttNow2 = time(NULL);

            if (__ttNow2>__ttLastSnd2+5)//XXs 定时器
            {
                //BBE藻类传感器
                const char *bufBBE[] =
                    {//"000\n\r" // 测试连接
                        //, "134 1 702\n\r"      // 激活自动拟合计算
                        "555\n\r" // 开始测量
                        //, "180\n\r" // 开始测量
                        //, "190\n\r" // 开始计算
                    };

                int bufBBE_Size = sizeof(bufBBE)/sizeof(const char *);

                static  int iSnd = 0;
                const char *bufGetBBE=NULL;
                bufGetBBE = bufBBE[iSnd%bufBBE_Size];

                int iret_sndto = this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)bufGetBBE, strlen(bufGetBBE));
                __ttLastSnd2 = time(NULL);

                ez_printf_debug("%d, Send(%d) of %s %d -- %s\n", bufBBE_Size, iSnd, "BBE_FLUOROPROBE", iret_sndto, bufGetBBE);

                // 最后一个 重复发送
                //if (iSnd<(bufBBE_Size-1))
                iSnd++;
            }// XXs 定时器

}

        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_COD_31) != std::string::npos)
        {
            //桐庐质控板
            //会主动周期报数据， 无需发送查询指令
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_FishTaximeter_BBE) != std::string::npos)
        {
            //
            unsigned char __bufSnd[] = {'D'};

            this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WXT530) != std::string::npos)
        {
            static unsigned int iii=0;

            if (iii%5 == 0)
            {
                //
                unsigned char __bufSnd[] = {'0', 'R', '1', '\r', '\n'}; // wind data message

                this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
            }
            if (iii%6 == 0)
            {
                //
                unsigned char __bufSnd[] = {'0', 'R', '2', '\r', '\n'}; // wind data message

                this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
            }
            if (iii%7 == 0)
            {
                //
                unsigned char __bufSnd[] = {'0', 'R', '3', '\r', '\n'}; // wind data message

                this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
            }
            if (iii%8 == 0)
            {
                //
                unsigned char __bufSnd[] = {'0', 'R', '5', '\r', '\n'}; // wind data message

                this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
            }

            iii++;
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QC_SA_1) != std::string::npos)
        {
            //
            unsigned char __bufSnd[] = {0xFE, 0x08, 0x01, 0x02, 0x01, 0x01};

            this->Sendto_Modbus(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_NoiseMeter_RSZS_01) != std::string::npos)
        {
            //
            unsigned char __bufSnd[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0a};

            this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
        }
        else if (1 == m_pSerialCfg->getConfig(ii).bEnable
                 && m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_NoiseMeter_PG760_01) != std::string::npos)
        {
            //
            unsigned char __bufSnd[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39};

            this->Sendto(m_pSerialCfg->getConfig(ii).iChannel, (char *)__bufSnd, sizeof(__bufSnd));
        }

    }
}
#if 0
float CFunSerial::getTemperature()
{
    return m_fTemperature;
}

float CFunSerial::getHumidity()
{
    return m_fHumidity;
}
#endif
// SerialId 可以不用
int CFunSerial::pubIOOut(int SerialId, int channel, int iOnOff)
{
    for(int ii=0; ii<m_pSerialCfg->getNumber(); ii++)
    {
        if (1 != m_pSerialCfg->getConfig(ii).bEnable)
            continue;
        //
        if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_Carry8I8O) != std::string::npos)
        {
            return pubIOOut_Carry8I8O(m_pSerialCfg->getConfig(ii).iChannel, channel, iOnOff);
        }
        else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WP9038ADAM) != std::string::npos)
        {
            return pubIOOut_WP9038ADAM(m_pSerialCfg->getConfig(ii).iChannel, channel, iOnOff, 4);
        }
        else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_WP8027ADAM) != std::string::npos)
        {
            return pubIOOut_WP9038ADAM(m_pSerialCfg->getConfig(ii).iChannel, channel, iOnOff, 16);
        }
        else if (m_pSerialCfg->getConfig(ii).strFunction.find(def_SerialFunc_QZ1_IO1) != std::string::npos)
        {
            return pubIOOut_QZ1_IO1(m_pSerialCfg->getConfig(ii).iChannel, channel, iOnOff);
        }
    } // for

    return -1;
}

int CFunSerial::pubIOOut_Carry8I8O(int SerialId, int channel, int iOnOff)
{
    //全开：
    unsigned char buff_all_on[] ={0x55, 0x01, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x67};
    //全关：
    unsigned char buff_all_off[]={0x55, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, 0x69};

    int send_buff_len = sizeof(buff_all_on);

    unsigned char *send_buff = (iOnOff==1 ? buff_all_on : buff_all_off);

    if (channel>=0)
    {
        carry_makebuf_set_out(send_buff, send_buff_len, (channel/8)+1, (channel%8)+1, iOnOff);
    }
    else
    {
        // 全开或者全关
    }

    int iret = Sendto(SerialId, (char *)send_buff, send_buff_len);

    return iret;
}

//channel 0-255
// iOutMax - 该模块最大输出数量, 9038 - 4, 8027 - 16
int CFunSerial::pubIOOut_WP9038ADAM(int SerialId, int channel, int iOnOff, int iOutMax)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    //开：
    unsigned char buff_all_on[] ={0x01, 0x05, 0x00, 0x00, 0xff, 0x00};
    //关：
    unsigned char buff_all_off[]={0x01, 0x05, 0x00, 0x00, 0x00, 0x00};

    unsigned char *send_buff = (iOnOff==1 ? buff_all_on : buff_all_off);
    int send_buff_len = sizeof(buff_all_on);

    send_buff[0] = (channel/iOutMax)+1; // 地址从1开始
    send_buff[3] = channel%iOutMax;//通道

    ez_printf_info("add:%d, ch:%d\n", send_buff[0], send_buff[3]);

    int iret = Sendto_Modbus(SerialId, (char *)send_buff, send_buff_len);

    ez_printf_info("Sendto_Modbus(%d, %d)=%d\n", SerialId, send_buff_len, iret);

    return iret;
}

//channel 0-255
int CFunSerial::pubIOOut_QZ1_IO1(int SerialId, int channel, int iOnOff)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    //开：
    unsigned char buff_all_on[] ={0xFD, 0x01, 0x06, 0x00, 0xff, 0xFB};
    //关：
    unsigned char buff_all_off[]={0xFD, 0x01, 0x06, 0x00, 0x00, 0xFB};

    unsigned char *send_buff = (iOnOff==1 ? buff_all_on : buff_all_off);
    int send_buff_len = sizeof(buff_all_on);

    // 最多16个io
    send_buff[1] = (channel/16)+1; // 地址从1开始
    send_buff[3] = channel%16;//通道

    ez_printf_info("add:%d, ch:%d\n", send_buff[1], send_buff[3]);

    int iret = Sendto_Modbus(SerialId, (char *)send_buff, send_buff_len);

    ez_printf_info("Sendto_Modbus(%d, %d)=%d\n", SerialId, send_buff_len, iret);

    return iret;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// 下标是ascii， 值为code
int CFunSerial::ini_asciiKeyMap_Ch9329(unsigned char *asciiKeyMap)
{
    unsigned char ii = 0;

    memset(asciiKeyMap, def_CH9329_KeyValueNotValid, def_CH9329_asciiKeyMap_Size);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // visible characters, sorted by ascii
    asciiKeyMap[' '] = 0x2c; // 32	20	(Space)	空格
    asciiKeyMap['!'] = 0x1e; // 33	21	!
    asciiKeyMap['"'] = 0x34; // 34	22	"
    asciiKeyMap['#'] = 0x20; // 35	23	#
    asciiKeyMap['$'] = 0x21; // 36	24	$
    asciiKeyMap['%'] = 0x22; // 37	25	%
    asciiKeyMap['&'] = 0x24; // 38	26	&
    asciiKeyMap['\''] = 0x34; // 39	27	'
    asciiKeyMap['('] = 0x26; // 40	28	(
    asciiKeyMap[')'] = 0x27; // 41	29	)
    asciiKeyMap['*'] = 0x25; // 42	2A	*
    asciiKeyMap['+'] = 0x2e; // 43	2B	+
    asciiKeyMap[','] = 0x36; // 44	2C	,
    asciiKeyMap['-'] = 0x2d; // 45	2D	-
    asciiKeyMap['.'] = 0x37; // 46	2E	.
    asciiKeyMap['/'] = 0x38; // 47	2F	/
    asciiKeyMap['0'] = 0x27; // 48	30	0

    // 1-9
    //
    for (ii = '1'; ii <= '9'; ii++)
    {
        asciiKeyMap[ii] = ii - '1' + 0x1e; // 1 is 0x1e
    }
    asciiKeyMap[':'] = 0x33; // 58	3A	:
    asciiKeyMap[';'] = 0x33; // 59	3B	;
    asciiKeyMap['<'] = 0x36; // 60	3C	<
    asciiKeyMap['='] = 0x2e; // 61	3D	=
    asciiKeyMap['>'] = 0x37; // 62	3E	>
    asciiKeyMap['?'] = 0x38; // 63	3F	?
    asciiKeyMap['@'] = 0x1f; // 64	40	@

    // a-z, A-Z
    for (ii = 'a'; ii <= 'z'; ii++)
    {
        asciiKeyMap[ii] = ii - 'a' + 0x04;               // a is 0x04
        asciiKeyMap['A' + (ii - 'a')] = ii - 'a' + 0x04; // A is 0x04 + shift
    }

    asciiKeyMap['['] = 0x2f; // 91	5B	[
    asciiKeyMap['\\'] = 0x32; // 92	5C	\ .
    asciiKeyMap[']'] = 0x30; // 93	5D	]
    asciiKeyMap['^'] = 0x23; // 94	5E	^
    asciiKeyMap['_'] = 0x2d; // 95	5F	_
    asciiKeyMap['`'] = 0x35; // 96	60	`

    asciiKeyMap['{'] = 0x2f; // 123	7B	{
    asciiKeyMap['|'] = 0x32; // 124	7C	|
    asciiKeyMap['}'] = 0x30; // 125	7D	}
    asciiKeyMap['~'] = 0x35; // 126	7E	~

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    return 0;
}

// asciiKeyMap 必须是0xff大小
unsigned char CFunSerial::getUsualKey(const unsigned char *asciiKeyMap, unsigned char theKey)
{
    return asciiKeyMap[theKey];
}

unsigned char CFunSerial::getUsualKey_NeedShift(unsigned char theKey)
{
    if ( (theKey>='A' && theKey<='Z') \
        ||'~' == theKey || '!' == theKey || '@' == theKey || '#' == theKey    \
        || '$' == theKey || '%' == theKey || '^' == theKey || '&' == theKey \
        || '*' == theKey || '(' == theKey || ')' == theKey || '_' == theKey \
        || '+' == theKey || '{' == theKey || '}' == theKey || '|' == theKey \
        || ':' == theKey || '"' == theKey || '<' == theKey || '>' == theKey \
        || '?' == theKey \
        )
        return 0x02; // shift

    return def_CH9329_KeyValueNotValid; // 不需要
}

unsigned char CFunSerial::getControlKey(std::string strCtrl)
{
    if ("CR" == strCtrl) //回车键
        return 0x28;
    if ("BS" == strCtrl) //退格
        return 0x2a;
    if ("DEL" == strCtrl) //删除
        return 0x4c;
    if ("ESC" == strCtrl) //退出
        return 0x29;
    if ("UP" == strCtrl) //箭头：上
        return 0x60;
    if ("DOWN" == strCtrl) //箭头：下
        return 0x5a;
    if ("LEFT" == strCtrl) //箭头：左
        return 0x5c;
    if ("RIGHT" == strCtrl) //箭头：→
        return 0x5e;
    if ("PGUP" == strCtrl) // pageup
        return 0x4b;
    if ("PGDN" == strCtrl) // pageup
        return 0x4e;
    if ("HOME" == strCtrl) // pageup
        return 0x4a;
    if ("END" == strCtrl) // pageup
        return 0x4d;
    if ("INS" == strCtrl) // insert
        return 0x49;
    if ("TAB" == strCtrl) // tab
        return 0x2b;

    return def_CH9329_KeyValueNotValid; // 不需要
}
