/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * NetSerial.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: NetSerial.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _NetSerial_H
#define _NetSerial_H

#include <time.h>
#include <vector>

#include "ListenSocket.h"

#include "EZThread.h"
#include "EZTimer.h"

// 反应堆
#include "NetSerialHandler.h"

// tcp client, auto conn
#include "NetSerialTcpSocketConnector.h"

#define DEFAULT_NET_SERIAL_HOST "192.168.6.184"
#define DEFAULT_NET_SERIAL_HOST_PORT 50000
#define DEFAULT_NET_SERIAL_HOST_PORT_STR "50000"
#define DEFAULT_NET_SERIAL_SERVICE_NAME "192.168.6.184:50000"

typedef struct __net_serial_tcp_servercfg_t
{
    unsigned short IsUse;
    unsigned short Port;
    std::string strHost;
    // will find by name
    std::string strServerName;

    int iConnTimeOut;//超时时间
    int iConnTimes; //连接尝试次数（保留）

    int iInterval; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    int bEnableRecon ;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    std::string strUName;
    std::string strUPwd;

    __net_serial_tcp_servercfg_t()
    {
        IsUse = 0;
        Port = 0;
        strHost = "";
        // will find by name
        strServerName="";

        iConnTimeOut = 0;//超时时间
        iConnTimes = 0; //连接尝试次数（保留）

        iInterval = 0; //[in] 重连间隔，单位毫秒，参数默认值为30秒
        bEnableRecon  = 0;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

        strUName = "";
        strUPwd = "";
    }
}NetSerialSERVERCFG_T;


typedef struct __NetSerialCfg
{
    // tcp 下位机
    // 可以为任意多个 2012-7-10 16:58:48
    std::vector<NetSerialSERVERCFG_T> TcpHost;
}
NetSerialCFG_T;

#define g_NetSerial (*CNetSerial::instance())

class CNetSerial : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CNetSerial);
    CNetSerial();
    virtual ~CNetSerial();
    ////////
    void Initialize(int argc, char * argv[]);
	
	// 设置配置参数
    void SetConfig(NetSerialCFG_T *pConfig=NULL);

    EZTHREAD_BOOL Start(CEZObject * pObj=NULL, SIG_CNetSerialHandler_DATA::SigProc pProc=NULL);
    EZTHREAD_BOOL Stop(CEZObject * pObj=NULL, SIG_CNetSerialHandler_DATA::SigProc pProc=NULL);

    void ThreadProc();

    void AddTcpHost(std::string strHost, int nPort, std::string strName="");

    int ConnectorCreate();

    // 内部需要一定的时间自行处理
    // 为确保内部销毁完成，一般调用之后， 10s之后再调用Connect，
    int ConnectorDestroy();

    void Run();

    int Send2Socket(std::string strSocketName, const unsigned char *pData, size_t Len);
    Socket * GetSocketByName(std::string strServerName);


    void TimerProcMaintain();
    const std::string &GetVersion();
    unsigned int GetRunPeriod();
    time_t GetStartTime();

    EZTHREAD_BOOL OnData(Socket *pSocket, const char *pdat, size_t len);

private:
    NetSerialCFG_T m_ConfigNetSerial;
    CNetSerialLog m_Log;
    CNetSerialHandler *m_pHandlerNetSerial;

    CEZMutex m_MutexSig;
    SIG_CNetSerialHandler_DATA m_sigData;

    CEZMutex m_MutexSendData;
}
; //CNetSerial

#endif //_NetSerial_H
