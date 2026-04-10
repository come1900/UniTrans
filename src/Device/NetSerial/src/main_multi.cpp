/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * main_multi.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: main_multi.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -main_multi 自创建线程， 不使用main线程-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 *     2013-07-22 17:17:41 WuJunjie ver 1.3.2
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <EZThread.h>
#include <EZTimer.h>

#include "NetSerial.h"
#ifndef _DEBUG_THIS
    #define _DEBUG_THIS
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
const char * const copy_right = "@(#) Copyright (c) 2013 ezlibs.com.  All rights reserved";
const char * const version_string = "v 1.3.2 - ";
const char * const desc="P_2013-5-20.gm.1000";

#define USE_CARRY_IO 1

class CDataUsageClass : public CEZObject
{
public:
    void OnData(Socket *pSocket, const char *pdat, size_t len)
    {
        __fline;
        printf("CDataUsageClass::OnData From:%s Len:%d Char[0]:%c\n", pSocket->GetSockName().c_str(), len, pdat[0]);
    }

    void Start()
    {
        g_NetSerial.Start(this, (SIG_CNetSerialHandler_DATA::SigProc)&CDataUsageClass::OnData);
    }
};

int main(int argc, char *argv[])
{
    //ShowVersion();

    //
    //g_TimerManager.Start();
    //g_ThreadManager.RegisterMainThread(ThreadGetID());


    g_NetSerial.Initialize(argc, argv);

#if 0
    //内部 使用
    g_NetSerial.Start();
#else
    // 第三方观察的例子
    CDataUsageClass __usage;
    __usage.Start();
#endif
    // main线程等待

    char strSendData[100]={0};
    int iTimes=0;
    //
    char strName[256];
    snprintf(strName, sizeof(strName)-1, "%s:%d", DEFAULT_NET_SERIAL_HOST, DEFAULT_NET_SERIAL_HOST_PORT);

#ifdef USE_CARRY_IO
    //全开：
    char buff_all_on[] ={0x55, 0x01, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x67};
    //全关：
    char buff_all_off[]={0x55, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, 0x69};
    //unsigned int ulCmd = 0;
    char *pSend = buff_all_on;
#endif // USE_CARRY_IO

    // 测试数据发送
    while (1)
    {
#ifdef USE_CARRY_IO
        if (iTimes%2==0)
        {
            pSend = buff_all_on;
        }
        else
        {
            pSend = buff_all_off;
        }

        printf("press any key to g_NetSerial.Send2Socket(%s, %s) ...\n", strName, strSendData);
        getchar();

        g_NetSerial.Send2Socket(strName, pSend, sizeof(buff_all_off));

#else

        snprintf(strSendData, sizeof(strSendData)-1, "mainMsg-%d\n", iTimes);

        printf("press any key to g_NetSerial.Send2Socket(%s, %s) ...\n", strName, strSendData);
        getchar();

        g_NetSerial.Send2Socket(strName, strSendData, strlen(strSendData));
#endif

        iTimes++;

    }

    return 0;
}
