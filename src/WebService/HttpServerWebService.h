/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerWebService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerWebService.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _HttpServerWebService_H
#define _HttpServerWebService_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "WebServiceStack.h"

#define MY_SESSION_ID_LEN 36

class CHttpServerWebService : public HttpdSocket
{
public:
    CHttpServerWebService(ISocketHandler& h);
    ~CHttpServerWebService();

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
    int Do_TestIF( CWebServiceMsg &JsonData) ;
    int Do_Register( CWebServiceMsg &JsonData) ;
	
    // WEB子系统向连辅子系统注册接口地址的接口
    int Do_registerURL( CWebServiceMsg &JsonData)						;
    // WEB子系统认证接口
    int Do_requestAuth( CWebServiceMsg &JsonData)						;
    // 主机认证接口
    int Do_requestAuthByHost( CWebServiceMsg &JsonData)						;
    // 心跳接口
    int Do_heartbeat( CWebServiceMsg &JsonData)						;
    //停止主机外网服务接口
    int Do_stopHostNetwork( CWebServiceMsg &JsonData)  ;
    //推送配置更新接口
    int Do_notifyUpdate( CWebServiceMsg &JsonData)              ;
    //上报设备状态接口
    int Do_reportDeviceStateList( CWebServiceMsg &JsonData)                   ;
    //获取设备状态接口 getDeviceStateList
    int Do_getDeviceStateList( CWebServiceMsg &JsonData)              ;
    //控制命令接口 controlDevice
    int Do_controlDevice( CWebServiceMsg &JsonData)                   ;

};

#endif // _HttpServerWebService_H

