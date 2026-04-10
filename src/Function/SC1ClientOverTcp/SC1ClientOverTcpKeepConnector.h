/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SC1ClientOverTcpKeepConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: SC1ClientOverTcpKeepConnector.h 5884 2015-01-05 14:04:49Z WuJunjie $
 *
 *  Explain:
 *     -
 *      http 连接器，
 *      保持连接，断开后自动再次连接
 *     -
 *
 *  Update:
 *     2012-07-10 04:10:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _SC1ClientOverTcpKeepConnector_H
#define _SC1ClientOverTcpKeepConnector_H

#include <TcpSocket.h>
#include <ISocketHandler.h>

#include "EZThread.h"
//#include "EZTimer.h"
//#include "EZSignals.h"
#include "../../UniTransDef.h"

#define CSC1ClientOverTcpKeepConnector_SOCKET_NAME "SC1ClientOverTcpKeepConnector"

class CSC1ClientOverTcpKeepConnector : public TcpSocket//,public CEZThread
{
public:
    CSC1ClientOverTcpKeepConnector(ISocketHandler&, std::string strSocketName=CSC1ClientOverTcpKeepConnector_SOCKET_NAME);
    ~CSC1ClientOverTcpKeepConnector();

    void SetCloseAndDelete();
    void OnConnectFailed();
    void OnDelete();

    // 从 TcpSocket::OnRead(); 中调用
    virtual void OnRawData(const char *buf,size_t len);

    /////////////////////////////
    // 加入命令解析
    virtual void OnConnect();
    virtual void OnReconnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
    /////////////////////////////
    //
    // iPureData 1. 纯数据需要计算校验码并加首尾数据
    //
    int SendMessage(const unsigned char *pBuf, const unsigned int iLen, int iPureData=1);

    std::string getMySessionId();

    unsigned int getRecvPacketNum();
    unsigned int getSentPacketNum();
//    int SendSmsMsg(const std::string &strUri, const std::string &strMsg, const std::string &matchRequest="");

    void update();
    bool IsLogined();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 登录
    int ICP_requestAuth();
    // 心跳
    int ICP_requestHeartbeat();

    // 重启
    int ICP_requestRestart();
    // 直控
    int ICP_requestCenterForce();

	// 获取ID
    int ICP_requestID();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int OnData(const char *buf,size_t len);

private:
    CSC1ClientOverTcpKeepConnector(const CSC1ClientOverTcpKeepConnector& s) : TcpSocket(s)//,CEZThread(CSC1ClientOverTcpKeepConnector_THREAD_ATTR)
    {
        m_strUName = "";
        m_strUPwd = "";

        m_enCurMsgStatus = -1;

        m_tOnMsg = time(NULL);
        m_ttMsgTimeOut = TCP_MSGG_TIMEOUT;

        m_ttConnectTimeOut = TCP_CONN_TIMEOUT;
    } // copy constructor
    CSC1ClientOverTcpKeepConnector& operator=(const CSC1ClientOverTcpKeepConnector& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CSC1ClientOverTcpKeepConnector *Reconnect();
    bool m_b_connected;
    time_t m_tOnData; ///< last Time in seconds when this socket was on data

    std::string m_sessionId;

    time_t m_ttOndata;
    int m_iTimeout;

    CEZMutex m_MutexSend;

    // 当前消息状态-- 状态机
    int m_enCurMsgStatus;
    std::string m_strUName;
    std::string m_strUPwd;
	    // 发送接收都算, 用来标记最后消息
    time_t m_tOnMsg;
    time_t m_ttMsgTimeOut; // 命令超时时间

    time_t m_ttConnectTimeOut;

    unsigned int m_iRecvPacket;
    unsigned int m_iSentPacket;
};

#endif // _SC1ClientOverTcpKeepConnector_H
