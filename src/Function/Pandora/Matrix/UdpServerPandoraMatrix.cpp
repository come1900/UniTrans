/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UdpServerPandoraMatrix.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: UdpServerPandoraMatrix.cpp 5884 2018-12-03 02:19:10Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-12-03 02:19:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include "str_opr.h"
#include <crc.h>
#include "StringOpr.h"

#include <Utility.h>
#include "../../../Logs.h"
#include "../../../Solar.h"

#include "HandlerPandoraMatrix.h"

#include "UdpServerPandoraMatrix.h"

typedef union FLOAT_CONV
{
    float f;
    int l;// 4Byte, use htonl...
    unsigned char c[4];
}float_conv;

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

CUdpServerPandoraMatrix::CUdpServerPandoraMatrix(ISocketHandler& h, std::string strSocketName)
        :UdpSocket(h)
{
    SetSockName(strSocketName);
}

void CUdpServerPandoraMatrix::OnRawData(const char *p,size_t l,struct sockaddr *sa_from,socklen_t sa_len)
{
    DBG(
        struct sockaddr_in sa;
        memcpy(&sa,sa_from,sa_len);
        ipaddr_t a;
        memcpy(&a,&sa.sin_addr,4);
        std::string ip;
        Utility::l2ip(a,ip);

        ez_printf_debug("Received %d bytes from: %s:%d\n", l,ip.c_str(),ntohs(sa.sin_port));
    );
    unsigned short devid;
    OnData_RTO((const unsigned char *)p, l, devid);

    char __ack[15];
    __ack[0]  = 0xFE;
    __ack[1]  = 0xFE;
    __ack[2]  = 0x4C;
    __ack[3]  = 0x0F; //len
    __ack[4]  = 0x01; //4协议版本号（01）
    __ack[5]  = p[5]; //5设备型号（05）
    __ack[6]  = p[6]; //6设备地址（01）
    __ack[7]  = p[7]; //6设备地址（01）
    __ack[8]  = 0x03; //7写数据长度


    __ack[9]  = 0x01; //8写数据1  系统通讯检测
    __ack[10] = 0x00; //9写数据2  系统远程故障1
    __ack[11] = 0x00; //10写数据3系统远程故障2

    std::string m_strDevID = std::string(def_DevIdUdpServerPandoraMatrix) + ezConvertToString(devid);
    {
        std::string __strDstIKey;
        int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, m_strDevID, def_PqMsg_ValueKey_CommDetect, "1");
        ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

        std::string __strValue;
        int iret_getPair = ((CHandlerPandoraMatrix *)(&(Handler())))->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

        if (0==iret_getPair)
        {
            ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __strValue.c_str());
            __ack[9]  = atoi(__strValue.c_str())==1 ? 1 : 0; //8写数据1  系统通讯检测
        }
    }
    {
        std::string __strDstIKey;
        int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, m_strDevID, def_PqMsg_ValueKey_Fault_1, "1");
        ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

        std::string __strValue;
        int iret_getPair = ((CHandlerPandoraMatrix *)(&(Handler())))->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

        if (0==iret_getPair)
        {
            ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __strValue.c_str());
            __ack[10]  = atoi(__strValue.c_str())==1 ? 1 : 0; //8写数据1  系统通讯检测
        }
    }
    {
        std::string __strDstIKey;
        int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, m_strDevID, def_PqMsg_ValueKey_Fault_2, "1");
        ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

        std::string __strValue;
        int iret_getPair = ((CHandlerPandoraMatrix *)(&(Handler())))->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

        if (0==iret_getPair)
        {
            ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __strValue.c_str());
            __ack[11]  = atoi(__strValue.c_str())==1 ? 1 : 0; //8写数据1  系统通讯检测
        }
    }

    unsigned short usret = ez_modbus_crc16((unsigned char *)__ack, sizeof(__ack)-3);
    __ack[12] = (usret>>8)&0xff;
    __ack[13] = usret&0xff;

    __ack[14] = 0x55; //13//结束帧

    sendto(this->GetSocket(), __ack, sizeof(__ack), 0, sa_from, sa_len);

    return;
}

/** Outgoing traffic counter. */
uint64_t CUdpServerPandoraMatrix::GetBytesSent(bool clear )
{
    return m_BytesSent;
}

/** Incoming traffic counter. */
uint64_t CUdpServerPandoraMatrix::GetBytesReceived(bool clear )
{
    return m_BytesReceived;
}
#define def_Rto_Data_Num 9
int CUdpServerPandoraMatrix::OnData_RTO(const unsigned char *buf, int len, unsigned short &devid)
{
#if 1
    char bufDump[1024];

    dumpBuffer2Buffer(bufDump, sizeof(bufDump)-1, (unsigned char *)buf,
                      len,
                      SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    ez_printf_debug("OnData_RTO(%d):\n%s\n", len, bufDump);
#endif

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

        // 数据长度
        int iPktLen = iMinLen + iDataNum*4;
        ez_printf_debug("%s:%d\n", "iPktLen", iPktLen);

        if (iPktLen != len)
        {
            ez_printf_error("E %s:%d\n", "iPktLen", iPktLen);

            iret = -3;
            break;
        }
        ////6设备地址2字节	(00，01)
        unsigned short __devid = 0;
        //memcpy(&__devid, buf+6, 2);
        __devid = buf[6]*0xff + buf[7];
        devid = __devid;

        ez_printf_debug("%s:%d\n", "__devid", __devid);
        std::string m_strDevID = std::string(def_DevIdUdpServerPandoraMatrix) + ezConvertToString(__devid);

        //8//100     炉膛平均温度
        //12//95.3   炉内温度设定值
        //16//892.5  VOC浓度1
        //20//0.05   VOC浓度2
        //24//108.2  VOC浓度3
        //28//79.9   RTO当前系统风量
        //32//789.9  RTO进口温度
        //36//30     RTO出口温度
        //40//1.5     急冷塔温度
        //float __data[9];
        const char *__pDataKey[def_Rto_Data_Num]=
            {
                def_PqMsg_ValueKey_AvgTemperatureOfFirepot   ,
                def_PqMsg_ValueKey_TemperatureOfFirepot      ,
                def_PqMsg_ValueKey_Voc1                      ,
                def_PqMsg_ValueKey_Voc2                      ,
                def_PqMsg_ValueKey_Voc3                      ,
                def_PqMsg_ValueKey_AirOfRto                  ,
                def_PqMsg_ValueKey_InTemperatureOfRto        ,
                def_PqMsg_ValueKey_OutTemperatureOfRto       ,
                def_PqMsg_ValueKey_TemperatureOfCoolingTower
            };

        for (size_t ii=0; ii<iDataNum; ii++)
        {
            const unsigned char *pdat = (buf+9+ii*4);
            float_conv floatPara;
#if 0

            floatPara.c[0] = pdat[0];
            floatPara.c[1] = pdat[1];
            floatPara.c[2] = pdat[2];
            floatPara.c[3] = pdat[3];

            __data[ii] = floatPara.f;
#else// 通用
            // big endia
            floatPara.c[3] = pdat[0];
            floatPara.c[2] = pdat[1];
            floatPara.c[1] = pdat[2];
            floatPara.c[0] = pdat[3];

            floatPara.l = htonl(floatPara.l);

            //__data[ii] = floatPara.f;

#endif
            //ez_printf_debug("0x%0x 0x%0x 0x%0x 0x%0x\n", pdat[0], pdat[1], pdat[2], pdat[3]);
            ez_printf_debug("__data[%d]:%f\n", ii, floatPara.f);

#if 1

            std::string __strDstIKey ;
            std::string __strDstValue = ezConvertToString(floatPara.f);

            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, m_strDevID, __pDataKey[ii], "1");
            ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

            if (0==iret_ValueKey)
            {
                int iret_getPair = ((CHandlerPandoraMatrix *)(&(Handler())))->m_pCacheKeyValue->modPair(__strDstIKey, __strDstValue, modPair_If_Not_Exist_Then_Add);

                if (0==iret_getPair)
                {
                    ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __strDstValue.c_str());
                }
            }

#endif

            if (ii+1 >= def_Rto_Data_Num)
            {
                break;
            }
        }


        //以下空白
        break;
    }
    while (0);

    return iret;
}

