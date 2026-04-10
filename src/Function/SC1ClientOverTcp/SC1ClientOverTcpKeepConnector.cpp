/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SC1ClientOverTcpKeepConnector.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SC1ClientOverTcpKeepConnector.cpp 5884 2015-01-05 14:04:36Z WuJunjie $
 *
 *  Notes:
 *     -
 *      支持多实例 
 *     -
 *
 *  Update:
 *     2013-12-18 03:38:04 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include <crc.h> // ezutil
#include <str_opr.h> /* dump */

#include "../../Logs.h"

#include "../Stack/SmsStack.h"

#include "SC1ClientOverTcpKeepConnector.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
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


//事务处理状态
typedef enum en_icp_logic_status
{
    enAppLogicStatus_Begin  =  0,
    enAppLogicStatus_Logining  =  1,
    enAppLogicStatus_Logined  =  2,

    enAppLogicStatus_Heartbeating,
    enAppLogicStatus_Heartbeated,

    enAppLogicStatus_Exiting,
    enAppLogicStatus_End,
}enAppLogicStatus_PHKC;

// 登录时， 没有返回的包数量
// 有些设备需要重连
#define LOGIN_COUNT_DOWN 6

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//////////////////////////////////////////////////
CSC1ClientOverTcpKeepConnector::CSC1ClientOverTcpKeepConnector(ISocketHandler& h, std::string strSocketName)
        :TcpSocket(h)
        //,CEZThread(CSC1ClientOverTcpKeepConnector_THREAD_ATTR)
        ,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();

    SetSockName(strSocketName);

    DisableInputBuffer();

    m_tOnData = 0;
    m_strUName = "";
    m_strUPwd = "";
    m_ttConnectTimeOut = 6;

    m_enCurMsgStatus = enAppLogicStatus_Begin;

    m_tOnMsg = time(NULL);
    m_ttMsgTimeOut = TCP_MSGG_TIMEOUT;

    m_iRecvPacket = 0;
    m_iSentPacket = 0;
}

CSC1ClientOverTcpKeepConnector::~CSC1ClientOverTcpKeepConnector()
{}

CSC1ClientOverTcpKeepConnector *CSC1ClientOverTcpKeepConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CSC1ClientOverTcpKeepConnector *p = new CSC1ClientOverTcpKeepConnector(Handler());
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);

    return p;
}


void CSC1ClientOverTcpKeepConnector::OnConnectFailed()
{
    CSC1ClientOverTcpKeepConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CSC1ClientOverTcpKeepConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CSC1ClientOverTcpKeepConnector::SetCloseAndDelete()
{
    TcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CSC1ClientOverTcpKeepConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    printf("CSC1ClientOverTcpKeepConnector::OnConnect() - %ld\n", time(NULL));
}
void CSC1ClientOverTcpKeepConnector::OnReconnect()
{
    printf("CSC1ClientOverTcpKeepConnector::OnReconnect()\n");
}

void CSC1ClientOverTcpKeepConnector::OnDisconnect()
{
    printf("CSC1ClientOverTcpKeepConnector::OnDisconnect() - %ld\n", time(NULL));
}

void CSC1ClientOverTcpKeepConnector::OnConnectTimeout()
{
    printf("CSC1ClientOverTcpKeepConnector::OnConnectTimeout()\n");
}

void CSC1ClientOverTcpKeepConnector::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);

    DBG(
        __fline;
        printf("CSC1ClientOverTcpKeepConnector::OnRawData:%d\n", len);
        dumpBuffer(stdout
                   , (unsigned char *)buf, len,
                   SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
    );
    EZ_LOGS(
        ez_log("%s(%d)--\n", __FILE__, __LINE__);
        ez_log("------------------------------------------\n");
        ez_log("%d", len);
        ez_log("\n------------------------------------------\n");
    );

    OnData(buf, len);
}

void CSC1ClientOverTcpKeepConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CSC1ClientOverTcpKeepConnector::OnCommand, len:%d\n", ulCmdDataLen); );
}

int CSC1ClientOverTcpKeepConnector::OnData(const char *buf, size_t len)
{
    //注释掉之后， 快速发送数据。。。
    time(&m_tOnMsg);
    bool bDumpData = false;

    if (len<8)
    {
        __trip;
        std::cout << "Error, Short msg(" << len << ")." << std::endl;
        return -1;
    }
    m_iRecvPacket++;

    if (buf[7]==0x02) //hb
    {
        m_enCurMsgStatus = enAppLogicStatus_Heartbeated;
        //__fline;
        //std::cout << "enAppLogicStatus_Heartbeated." << std::endl;
    	//bDumpData = true;
    	
    	if (len > 107 + 3)
    	{
	        __fline;
	        std::cout << "on ext data." << std::endl;
	    	bDumpData = true;
	    	
	    		unsigned char vip_card_data[18];
	    	if (len-110>=sizeof(vip_card_data))
	    	{
	    		memcpy(vip_card_data, buf+110-2, sizeof(vip_card_data));
#if 1
				printf("id:0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x \n"
					, vip_card_data[0]
					, vip_card_data[1]
					, vip_card_data[2]
					, vip_card_data[3]
					, vip_card_data[4]
					, vip_card_data[5]
					, vip_card_data[6]
					, vip_card_data[7]);
				
//				unsigned int tmp32;
//				tmp32= vip_card_data[10]<<24|vip_card_data[15]<<16|vip_card_data[16]<<8|vip_card_data[17]; 
////DBG(								
//				printf("经度:%d\r\n",tmp32);
////);
//				tmp32= vip_card_data[14]<<24|vip_card_data[19]<<16|vip_card_data[20]<<8|vip_card_data[21]; 
////DBG(								
//				printf("维度:%d\r\n",tmp32);
////);
//				tmp32= vip_card_data[18]<<24|vip_card_data[23]<<16|vip_card_data[24]<<8|vip_card_data[25]; 
////DBG(								
//				printf("速度:%d\r\n",tmp32);
////);
#endif	    		
	    			
	    	}
   		}
    }
    else if (buf[7]==0x01) //连接
    {
        m_enCurMsgStatus = enAppLogicStatus_Logined;

        __fline;
        std::cout << "enAppLogicStatus_Logined." << std::endl;
    }
    else if (buf[7]==0x20) //查设备id
    {
    	bDumpData = true;
    }
    else
    {
    	bDumpData = true;
        //__trip;
        //std::cout << "Unknow msg." << std::endl;
        //DBG(
        //);
    }

    if (bDumpData)
    {
        //__trip;
        //std::cout << "Unknow msg." << std::endl;
        //DBG(
        __fline;
        printf("bDumpData:%d\n", len);
        dumpBuffer(stdout
                   , (unsigned char *)buf, len,
                   SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
        //);
    }

    return 0;
}
std::string CSC1ClientOverTcpKeepConnector::getMySessionId()
{
    //__fline;
    //cout << "getMySessionId:" << m_sessionId << endl;

    return m_sessionId;
}

unsigned int CSC1ClientOverTcpKeepConnector::getRecvPacketNum()
{
    return m_iRecvPacket;
}
unsigned int CSC1ClientOverTcpKeepConnector::getSentPacketNum()
{
    return m_iSentPacket;
}

void CSC1ClientOverTcpKeepConnector::update()
{
    // 调试重连
    // 不使用自动机制
    //return ;

    time_t ttNow = time(NULL);
    int iRet = -1;

    if(m_b_connected==false)
    {
        return;
    }
    // 连接之后登录
    if (IsConnected()
        && m_enCurMsgStatus<enAppLogicStatus_Logining)
    {
        iRet = ICP_requestAuth();

        DBG(
            __fline;
            printf("Login ....\n");
        );
        //return;
    }

    // 登录超时
    if (m_enCurMsgStatus==enAppLogicStatus_Logining && (ttNow-m_tOnMsg>m_ttMsgTimeOut))
    {
        m_enCurMsgStatus=enAppLogicStatus_Begin;

        __fline;
        printf("Login timeout, CountDown:%d\n", LOGIN_COUNT_DOWN-(m_iSentPacket - m_iRecvPacket));

        EZ_LOGS(
            ez_log("%s(%d)--\n", __FILE__, __LINE__);
            ez_log("Login timeout, CountDown:%d\n", LOGIN_COUNT_DOWN-(m_iSentPacket - m_iRecvPacket));
        );

        if (m_iSentPacket >= (m_iRecvPacket+LOGIN_COUNT_DOWN) )
        {
            // 多于10个包没有反馈也重启， 防止连接建立，但是没有反馈的情况
            // 在网络型信号机中发现此情况
            SetCloseAndDelete();
        }

        return;
    }

    // 命令超时
    if (m_enCurMsgStatus==enAppLogicStatus_Heartbeating && (ttNow-m_tOnMsg>m_ttMsgTimeOut))
    {
        m_enCurMsgStatus=enAppLogicStatus_Logined;

        __fline;
        printf("Heartbeat timeout.\n");

        return;
    }

    if (ttNow-m_tOnMsg > m_ttConnectTimeOut)
    {
        __fline;
        printf("m_tOnMsg timeout.\n");

        // 遇到单片机之类的tcpserver;
        // 不返回断开包时的异常
        SetCloseAndDelete();
        // 需要在CAgentCuOverHttp::Run()中重连配合

        m_enCurMsgStatus=enAppLogicStatus_Begin;

        return;
    }

    DBG(
        __fline;
        printf("m_enCurMsgStatus:%d ttNow-m_tOnMsg:%ld, m_ttMsgTimeOut:%ld\n", m_enCurMsgStatus, ttNow-m_tOnMsg, m_ttMsgTimeOut);
    );

    // 心跳
    if (m_enCurMsgStatus>=enAppLogicStatus_Logined
        && m_enCurMsgStatus != enAppLogicStatus_Heartbeating /*正在等心跳返回消息*/
        &&(ttNow-m_tOnMsg)>(m_ttConnectTimeOut/2.0-0.5))
    {
        DBG(
            __fline;
            printf("KeepLive , ICP_requestHeartbeat\n");
        );

        ICP_requestHeartbeat();
    }
}

int CSC1ClientOverTcpKeepConnector::SendMessage(const unsigned char *pBuf, const unsigned int iLen, int iPureData)
{
    // 可能有多个对象调用
    CEZLock __lock(m_MutexSend);

    unsigned char  *tmpBuf;
    tmpBuf = (unsigned char *)malloc (iLen+6+3);
    if (NULL == tmpBuf)
    {
        return -1;
    }


    tmpBuf[0] = 0x7e; // 包头
    tmpBuf[0+1] = 0x01;// 版本号 类型   网络 01   遥控器 20  车检器 30  vip盒子 40
    tmpBuf[0+1+1] = 0x65; // 101 发送地址   发送方
    tmpBuf[0+1+1+1] = 0x6e;// 110 接收地址 接收方   
    tmpBuf[0+1+1+1+1] = 0x00; // 控制方式
    tmpBuf[0+1+1+1+1+1] = 0x01; //区域号， 暂时没用
    tmpBuf[0+1+1+1+1+1+1] = 0x04; // 路口号， 暂时没用
    //tmpBuf[0+1+1+1+1+1+1+1]
    //tmpBuf[0+1+1+1+1+1+1+1+iLen]
    //tmpBuf[0+1+1+1+1+1+1+1+iLen+1]

    memcpy(&tmpBuf[0+1+1+1+1+1+1+1], pBuf, iLen);

    unsigned char crc = ez_crc8(0x33, tmpBuf+1, iLen+6);

    tmpBuf[0+1+1+1+1+1+1+1+iLen] = crc;
    tmpBuf[0+1+1+1+1+1+1+1+iLen+1] = 0x7e;

    SendBuf((char *)tmpBuf, iLen+3+6);

    free (tmpBuf);

    m_iSentPacket++;

    return iLen+3+6;
}

bool CSC1ClientOverTcpKeepConnector::IsLogined()
{
    return m_enCurMsgStatus>=enAppLogicStatus_Logined;

}
int CSC1ClientOverTcpKeepConnector::ICP_requestAuth()
{
	// 0x01 + id
	// Ver c
    unsigned char __Packet[]={0x01, 0x9d, 0x6b, 0xbf, 0x52, 0x55, 0xc5, 0x0c, 0xc2};
    //a
    //unsigned char __Packet[]={0x01, 0x9d, 0xc3, 0x83, 0xe5, 0x55, 0xc5, 0x0c, 0x2e};

    SendMessage(__Packet, sizeof(__Packet));

    m_enCurMsgStatus = enAppLogicStatus_Logining;

    return 0;
}

int CSC1ClientOverTcpKeepConnector::ICP_requestHeartbeat()
{
    unsigned char __Packet[]={0x02};

    SendMessage(__Packet, sizeof(__Packet));


    m_enCurMsgStatus = enAppLogicStatus_Heartbeating;

    return 0;
}
// 重启
int CSC1ClientOverTcpKeepConnector::ICP_requestRestart()
{
    unsigned char __Packet[]={0x05};

    SendMessage(__Packet, sizeof(__Packet));

    return 0;
}

// 直控
int CSC1ClientOverTcpKeepConnector::ICP_requestCenterForce()
{
    // 06 == 直控
    // rx_buffer[7] ControlType		//控制类型  0:自主控制 105:全红 106:关灯 107:黄闪 108:锁定相位
    // rx_buffer[8] PassTime			//放行时间 -- rx_buffer[8]
    // rx_buffer[9] PassPhaseNumber	//放行相位数量
    // rx_buffer[10..] PassPhaseArr		//放行相位数组

    unsigned char __Packet[]={  \
                                0x06/*CENT CTRL*/
                                , 0x6c/*ControlType*/
                                , 30/*PassTime*/
                                , 6/*PassPhaseNumber*/
                                , 1/*PassPhaseArr*/
                                , 2/*PassPhaseArr*/
                                , 3/*PassPhaseArr*/
                                , 4/*PassPhaseArr*/
                                , 5/*PassPhaseArr*/
                                , 6/*PassPhaseArr*/
                             };

    SendMessage(__Packet, sizeof(__Packet));

    //DBG(
    __fline;
    printf(" ICP_requestCenterForce \n");
    //);

    return 0;
}

// 获取设备ID
int CSC1ClientOverTcpKeepConnector::ICP_requestID()
{
    unsigned char __Packet[]={0x20};

    SendMessage(__Packet, sizeof(__Packet));

    return 0;
}
