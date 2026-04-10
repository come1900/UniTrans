/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CuOverHttpKeepConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CuOverHttpKeepConnector.h 5884 2012-07-10 04:10:20Z WuJunjie $
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


#ifndef _CuOverHttpKeepConnector_H
#define _CuOverHttpKeepConnector_H

#include <TcpSocket.h>
#include <ISocketHandler.h>
/*ezutil/http_parser.h*/
#include <http_parser.h>

//#include "EZThread.h"
//#include "EZTimer.h"
//#include "EZSignals.h"
#include "../../UniTransDef.h"

#define CCuOverHttpKeepConnector_SOCKET_NAME "CuOverHttpKeepConnector"

class CCuOverHttpKeepConnector : public TcpSocket//,public CEZThread
{
public:
    CCuOverHttpKeepConnector(ISocketHandler&, std::string strSocketName=CCuOverHttpKeepConnector_SOCKET_NAME);
    ~CCuOverHttpKeepConnector();

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

    std::string getMySessionId();
    int SendSmsMsg(const std::string &strUri, const std::string &strMsg, const std::string &matchRequest="");

    void update();
    bool IsLogined();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 登录
    int ICP_requestAuth();
    // 心跳
    int ICP_requestHeartbeat();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    //int OnHeader(char *body);
    int OnHeaderField(char *body);
    int OnHeaderValue(char *body);
    int OnBody(char *body);

private:
    CCuOverHttpKeepConnector(const CCuOverHttpKeepConnector& s) : TcpSocket(s)//,CEZThread(CCuOverHttpKeepConnector_THREAD_ATTR)
    {
        m_strUName = "";
        m_strUPwd = "";

        m_enCurMsgStatus = -1;

        m_tOnMsg = time(NULL);
        m_ttMsgTimeOut = TCP_MSGG_TIMEOUT;

        m_ttConnectTimeOut = TCP_CONN_TIMEOUT;
    } // copy constructor
    CCuOverHttpKeepConnector& operator=(const CCuOverHttpKeepConnector& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CCuOverHttpKeepConnector *Reconnect();
    bool m_b_connected;
    time_t m_tOnData; ///< last Time in seconds when this socket was on data

    /////////////
    http_parser *m_pHttpParserCli;
    http_parser_settings *m_pHttpParserSettings;

    std::string m_sessionId;

    time_t m_ttOndata;
    int m_iTimeout;

    // 当前消息状态-- 状态机
    int m_enCurMsgStatus;
    std::string m_strUName;
    std::string m_strUPwd;
	    // 发送接收都算, 用来标记最后消息
    time_t m_tOnMsg;
    time_t m_ttMsgTimeOut; // 命令超时时间

    time_t m_ttConnectTimeOut;

    bool m_bHaveMatchRequest;
    std::string m_strMatchRequest;

    //unsigned int m_iRecvPacket;
    unsigned int m_iSentPacket;
};

#endif // _CuOverHttpKeepConnector_H
