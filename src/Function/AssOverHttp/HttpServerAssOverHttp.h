/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerAssOverHttp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerAssOverHttp.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HttpServerAssOverHttp_H
#define _HttpServerAssOverHttp_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "../Stack/SmsStack.h"

#define MY_SESSION_ID_LEN 36

class CHttpServerAssOverHttp : public HttpdSocket
{
public:
    CHttpServerAssOverHttp(ISocketHandler& h);
    ~CHttpServerAssOverHttp();

    void Init();

    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    void CreateHeader(size_t ContentLength=0, const std::string &matchRequest="");

    void GenerateDocument();

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeaderComplete();
    virtual void OnUnknowRequest();
    std::string getMySessionId();


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

private:
    std::string m_strBody;
    std::string m_requestPrefix;
    std::string m_sessionId;
    int reportId;

    int ReturnHttpMsg(const std::string &strMsg, const std::string &matchRequest="");
    int sendDeviceCtrlMsg(char *ip,
                          const int port,
                          const char *operate_id,
                          const char *value,
                          const u_char value_len,
                          const int32_t session_id);
    // get a new
    std::string generateSessionId();

    //测试接口
    int Do_TestIF( CSmsStack &JsonData) ;
    // WEB子系统向连辅子系统注册接口地址的接口
    int Do_registerURL( CSmsStack &JsonData)						;

    //4.1	鉴权
    int Do_requestAuth( CSmsStack &JsonData);
    //4.2	注册
    int Do_requestRegister( CSmsStack &JsonData);
    //4.3	心跳
    int Do_requestHeartbeat( CSmsStack &JsonData);
    //4.4	注销
    int Do_requestCancel( CSmsStack &JsonData);
    //4.5	转发
    int Do_requestTransit( CSmsStack &JsonData);
#if 0
    // WEB子系统认证接口
    int Do_requestAuth( CezlibsWebServiceIF &JsonData)						;
    // 主机认证接口
    int Do_requestAuthByHost( CezlibsWebServiceIF &JsonData)						;
    // 心跳接口
    int Do_heartbeat( CezlibsWebServiceIF &JsonData)						;
    //停止主机外网服务接口
    int Do_stopHostNetwork( CezlibsWebServiceIF &JsonData)  ;
    //推送配置更新接口
    int Do_notifyUpdate( CezlibsWebServiceIF &JsonData)              ;
    //上报设备状态接口
    int Do_reportDeviceStateList( CezlibsWebServiceIF &JsonData)                   ;
    //获取设备状态接口 getDeviceStateList
    int Do_getDeviceStateList( CezlibsWebServiceIF &JsonData)              ;
    //控制命令接口 controlDevice
    int Do_controlDevice( CezlibsWebServiceIF &JsonData)                   ;

    int Do_getPubIP4( CezlibsWebServiceIF &JsonData)                   ;
#endif
};

#endif // _HttpServerAssOverHttp_H

