/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuOverHttpKeepConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PuOverHttpKeepConnector.h 5884 2012-07-10 04:10:20Z WuJunjie $
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


#ifndef _PuOverHttpKeepConnector_H
#define _PuOverHttpKeepConnector_H

#include <TcpSocket.h>
#include <ISocketHandler.h>
/*ezutil/http_parser.h*/
#include <http_parser.h>

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

// 线程属性
//#define CPuOverHttpKeepConnector_THREAD_ATTR "CPuOverHttpKeepConnector", THREAD_PRIORITY_DEFAULT, 100

#define CPuOverHttpKeepConnector_SOCKET_NAME "PuOverHttpKeepConnector"



class CPuOverHttpKeepConnector : public TcpSocket//,public CEZThread
{
public:
    CPuOverHttpKeepConnector(ISocketHandler&, std::string strSocketName=CPuOverHttpKeepConnector_SOCKET_NAME);
    ~CPuOverHttpKeepConnector();

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

    int OnBody(char *body);

    int OnHeaderField(char *str);
    int OnHeaderValue(char *str);

private:
    CPuOverHttpKeepConnector(const CPuOverHttpKeepConnector& s) : TcpSocket(s)//,CEZThread(CPuOverHttpKeepConnector_THREAD_ATTR)
    {
        m_strUName = "";
        m_strUPwd = "";

        m_enOdipStatus = -1;

        m_tOnOdip = time(NULL);
        m_ttOdipTimeOut = 2;

        m_ttConnectTimeOut = 5;

    } // copy constructor

    CPuOverHttpKeepConnector& operator=(const CPuOverHttpKeepConnector& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CPuOverHttpKeepConnector *Reconnect();
    bool m_b_connected;
    time_t m_tOnData; ///< last Time in seconds when this socket was on data

    /////////////
    http_parser *m_pHttpParserCli;
    http_parser_settings *m_pHttpParserSettings;

    std::string m_sessionId;

    time_t m_ttOndata;
    int m_iTimeout;

    int m_enOdipStatus;
    std::string m_strUName;
    std::string m_strUPwd;
    time_t m_tOnOdip;
    time_t m_ttConnectTimeOut;
    time_t m_ttOdipTimeOut; // 命令超时时间

    bool m_bHaveMatchRequest;
    std::string m_strMatchRequest;
};


#endif // _PuOverHttpKeepConnector_H
