/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerInterUtcs_YiHeTech.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerInterUtcs_YiHeTech.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 16:35:52  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerInterUtcs_YiHeTech_H
#define _HttpServerInterUtcs_YiHeTech_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "../Stack/YiHe1Stack.h"

class CHttpServerInterUtcs_YiHeTech : public HttpdSocket
{
public:
    CHttpServerInterUtcs_YiHeTech(ISocketHandler& h);
    ~CHttpServerInterUtcs_YiHeTech();

    void Init();
    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    void CreateHeader(size_t ContentLength=0, const std::string &matchRequest="");

    void RetGeneralMsg(const int retCode=-1, const std::string &strMsg="", const std::string &matchRequest="");

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeaderComplete();
    virtual void OnUnknowRequest();

	//virtual void OnSessionTimeout();

    std::string getMySessionId();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int SendYiHe1Msg(const std::string &strMsg, const std::string &matchRequest="");

    void update();

    size_t getMsgNumReceived();
    size_t getMsgNumSent();

private:
    std::string m_strBody;
    std::string m_requestPrefix;
    std::string m_sessionId;
    int reportId;

	time_t m_ttOndata;
	int m_iTimeout;

	size_t m_iMsgCountGet;
	size_t m_iMsgCountSend;

    int sendDeviceCtrlMsg(char *ip,
                          const int port,
                          const char *operate_id,
                          const char *value,
                          const u_char value_len,
                          const int32_t session_id);
    // get a new
    std::string generateSessionId();

    //测试接口
    int Do_TestIF( CYiHe1Stack &JsonData) ;
    //4.1	鉴权
    int Do_getSignalControlerPara( CYiHe1Stack &JsonData);
    //4.2	注册
    int Do_getCrossPara( CYiHe1Stack &JsonData);
    //4.3	心跳
    int Do_getCrossStatus( CYiHe1Stack &JsonData);
};

#endif // _HttpServerInterUtcs_YiHeTech_H

