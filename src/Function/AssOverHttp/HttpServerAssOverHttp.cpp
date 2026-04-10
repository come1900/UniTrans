/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerAssOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerAssOverHttp.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:34  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#include <stdio.h>
#ifdef HAVE_LIBUUID
#include <uuid/uuid.h>
#endif

#include "HttpServerAssOverHttp.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigAssOverHttp.h"
#endif //_USE_MODULES_EZCONFIG
#include "str_opr.h"
#include "StringOpr.h"
#include "HandlerAssOverHttp.h"
#ifdef _DEBUG
	#define DEB(x)
	#define DBG(x)
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


CHttpServerAssOverHttp::CHttpServerAssOverHttp(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";
    m_sessionId = "";
}

void CHttpServerAssOverHttp::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerAssOverHttp::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerAssOverHttp::~CHttpServerAssOverHttp()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerAssOverHttp::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerAssOverHttp::OnAccept()
{
    printf("CHttpServerAssOverHttp::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}
void CHttpServerAssOverHttp::OnDisconnect()
{
    printf("CHttpServerAssOverHttp::OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    //printf("CHttpServerAssOverHttp::OnDisconnect()\n");
}

void CHttpServerAssOverHttp::Exec()
{
    CreateHeader();
    GenerateDocument();
}
void CHttpServerAssOverHttp::OnHeaderComplete()
{
    m_strBody = "";
}

void CHttpServerAssOverHttp::OnUnknowRequest()
{}

void CHttpServerAssOverHttp::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    {
        size_t x = 0;
        for (size_t i = 0; i < GetUri().size(); i++)
            if (GetUri()[i] == '.')
                x = i;
        std::string ext = GetUri().substr(x + 1);
        if (ext == "gif" || ext == "jpg" || ext == "png")
            AddResponseHeader("Content-type", "image/" + ext);
        else
            AddResponseHeader("Content-type", "text/" + ext);
    }
    //AddResponseHeader("Connection", "close");
    AddResponseHeader("Connection", "Keep-Alive");

    if (m_sessionId.size()==MY_SESSION_ID_LEN)
    {
        AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
    }
    else
    {
        // debug only
        m_sessionId = "dbg only";
        AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
        m_sessionId="";
    }

    if (ContentLength>0)
    {
        AddResponseHeader( "Content-length", Utility::l2string((long)ContentLength) );
    }

    if (matchRequest != "")
    {
        AddResponseHeader( "Match-request", matchRequest);
    }
    SendResponse();
}

void CHttpServerAssOverHttp::GenerateDocument()
{
    std::string strSendBuf = "Hello CHttpServerAssOverHttp.\n\r";

    strSendBuf += GetHttpDate()+"\r\n";
    strSendBuf += "Your Body:"+m_strBody+"\n\r";

    strSendBuf = "[{\"result\":\"100\",\"reason\":\"CHttpServerAssOverHttp not support url:" + GetUrl() + "\"}]";

    //Send(strSendBuf);
    ReturnHttpMsg(strSendBuf);
}

/** Chunk of http body data recevied. */
void CHttpServerAssOverHttp::OnData(const char *p,size_t l)
{
    if (m_strBody.size()>4096 || l>4096)
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too big");
        std::string strBadboy = "Bad boy.";

        ReturnHttpMsg(strBadboy);

        //Reset(); // prepare for next request
        //SetCloseAndDelete();
        //m_strBody = "";

        return;
    }

    m_strBody += p;
}

void CHttpServerAssOverHttp::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#ifdef _USE_MODULES_EZCONFIG
    CConfigAssOverHttp __cfg;
    __cfg.update();
#endif//

    CSmsStack __CezlibsWebServiceIF;

    //LOG4CPLUS_DEBUG(LOG_OF_SMS, "m_strBody:" << m_strBody);

    if (m_strBody.size()<strlen("[{}]"))
    {
        LOG4CPLUS_INFO(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ")" << " [" << m_strBody << "] not json");
        ez_printf_error("Not JSON, Data(%d):%s\n", m_strBody.size(), m_strBody.c_str());
    }
    else
    {
        // have []  of json string
#if 0
        size_t ii;
        for(ii=m_strBody.length()-1;ii>0; ii--)
        {
            if (m_strBody.at(ii)==']')
            {
                break;
            }
        }
        //if (ii !=m_strBody.length()-1)
        if (ii > 3)
        {
            //m_strBody.erase(ii+1,m_strBody.length()-1);
            m_strBody.erase(ii,m_strBody.length()-1);
        }

        for(ii=0;ii<m_strBody.length()-1; ii++)
        {
            if (m_strBody.at(ii)=='[')
            {
                m_strBody.erase(0,ii+1);
                break;
            }
        }
#endif //
        LOG4CPLUS_INFO(LOG_OF_SMS, "Html request:"
                       << "\n\t\t Method: "<<GetMethod()
                       << "\n\t\t URL: "<<GetUrl()
                       << "\n\t\t Http version: "<<GetHttpVersion()
                       << "\n\t\t Body: "<<m_strBody << "\n\t\t\t");

        ez_printf_info("Html request(%d):%s\n", m_strBody.size(), m_strBody.c_str());


        __CezlibsWebServiceIF.Parse(m_strBody);
        //CreateHeader();
    }

    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Request URL:" << GetUrl());
    ez_printf_info("Html request:%s\n", GetUrl().c_str());
    //exa:
    //	curl http://192.168.6.112:60089/AssOverHttp/registerURL -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    //	curl http://192.168.6.112:60089/AssOverHttp/registerURL -d ""
    //	curl http://192.168.6.112:60089/AssOverHttp/registerURL
    // test interface
#ifdef _USE_MODULES_EZCONFIG

    if (GetUrl()==__cfg.getConfig().strURL_registerURL)
    {
        Do_registerURL(__CezlibsWebServiceIF);
    }
    //exa:
    //	curl http://localhost:60600/AssOverHttp/TestIF -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    // test interface
    else if (GetUrl()=="/Sms/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    //鉴权接口
    //	http://localhost:60600/Sms/requestAuth
    else if (GetUrl()==__cfg.getConfig().strURIrequestAuth)
    {
        Do_requestAuth(__CezlibsWebServiceIF);
    }//鉴权接口
    //注册接口
    //	http://localhost:60600/Sms/requestRegister
    else if (GetUrl()==__cfg.getConfig().strURIrequestRegister)
    {
        Do_requestRegister(__CezlibsWebServiceIF);
    }//接口

    //心跳接口
    //	http://localhost:60600/Sms/requestHeartbeat
    else if (GetUrl()==__cfg.getConfig().strURIrequestHeartbeat)
    {
        Do_requestHeartbeat(__CezlibsWebServiceIF);
    }//心跳接口
    //注销接口
    //	http://localhost:60600/Sms/requestCancel
    else if (GetUrl()==__cfg.getConfig().strURIrequestCancel)
    {
        Do_requestCancel(__CezlibsWebServiceIF);
    }//注销接口
    //中转接口
    //	http://localhost:60600/Sms/requestTransit
    else if (GetUrl()==__cfg.getConfig().strURIrequestTransit)
    {
        Do_requestTransit(__CezlibsWebServiceIF);
    }//中转接口
#else // not define _USE_MODULES_EZCONFIG
    if (GetUrl()=="/Sms/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
#endif //_USE_MODULES_EZCONFIG

#if 0
    //	http://218.95.39.172:60600/AssOverHttp/requestReportDeviceConfigParam
    else if (GetUrl()==__cfg.getConfig().strURL_requestAuthByHost)
    {
        Do_requestAuthByHost(__CezlibsWebServiceIF);
    }
    //5.2.6	心跳接口 getCtrlDeviceState
    //	http://192.168.10.216:60600/AssOverHttp/heartbeat
    else if (GetUrl()==__cfg.getConfig().strURL_heartbeat)
    {
        Do_heartbeat(__CezlibsWebServiceIF);
    }
    //5.2.8	获取设备状态接口 getDeviceStateList
    //	http://218.95.39.172:60600/AssOverHttp/getDeviceStateList
    //
    else if (GetUrl()==__cfg.getConfig().strURL_registerURL)
    {
        Do_registerURL(__CezlibsWebServiceIF);
    }
    //5.2.4	请求配置上报接口
    //	http://218.95.39.172:60600/AssOverHttp/requestReportDeviceConfigParam
    else if (GetUrl()==__cfg.getConfig().strURL_stopHostNetwork)
    {
        Do_stopHostNetwork(__CezlibsWebServiceIF);
    }
    //5.2.6	获取网关状态接口 getCtrlDeviceState
    //	http://218.95.39.172:60600/AssOverHttp/getCtrlDeviceState
    else if (GetUrl()==__cfg.getConfig().strURL_notifyUpdate)
    {
        Do_notifyUpdate(__CezlibsWebServiceIF);
    }
    //5.2.8	获取设备状态接口 getDeviceStateList
    //	http://218.95.39.172:60600/AssOverHttp/getDeviceStateList
    //
    else if (GetUrl()==__cfg.getConfig().strURL_getDeviceStateList)
    {
        Do_getDeviceStateList(__CezlibsWebServiceIF);
    }
    //5.2.9	控制命令接口 controlDevice
    //	http://218.95.39.172:60600/AssOverHttp/controlDevice
    else if (GetUrl()==__cfg.getConfig().strURL_controlDevice)
    {
        Do_controlDevice(__CezlibsWebServiceIF);
    }
#endif
    else
    {
        GenerateDocument();
    }

    Reset(); // prepare for next request
    //SetCloseAndDelete();
    m_strBody = "";
}

int CHttpServerAssOverHttp::ReturnHttpMsg(const std::string &strMsg, const std::string &matchRequest)
{
    cout << "strMsg.size() :" <<  strMsg.size()  << endl;
    CreateHeader(strMsg.size(), matchRequest);

    cout << "strMsg :" <<  strMsg  << endl;

    Send(strMsg);
    Reset();
    //SetCloseAndDelete();
    m_strBody = "";

    return 0;
}

std::string CHttpServerAssOverHttp::generateSessionId()
{
    char uuidBuff[37] = {0};

    // single thread, do not add lock here
#ifdef HAVE_LIBUUID

    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuidBuff);
#else

    static unsigned short idNum = 100;
    sprintf(uuidBuff, "ezlibs.com-%8d", idNum);
    idNum ++;
#endif //HAVE_LIBUUID

    return uuidBuff;
}

std::string CHttpServerAssOverHttp::getMySessionId()
{
    return m_sessionId;
}

// curl http://localhost:60600/Sms/TestIF -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerAssOverHttp::Do_TestIF( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_TestIF");
    __fline;
    cout << "Do_TestIF, your json:" << JsonData.ToString()<< endl;

    std::string strSendBuf;

    strSendBuf = "[{\"result\":\"0\",\"reason\":\"Do_TestIF, Your URL:" + GetUrl() + "\"}]";

    ReturnHttpMsg(strSendBuf);

    return 0;
}

// WEB子系统向连辅子系统注册接口地址的接口
// curl http://sms.ezlibs.com:60600/AssOverHttp/registerURL -d "{ \"AuthCode\" : \"VAuthCode\", \"AuthName\" : \"VAuthName\", \"LocalDateTime\" : \"VLocalDateTime\", \"LocalIp\" : \"VLocalIp\", \"Password\" : \"VPassword\", \"ProductID\" : \"VProductID\" }\n"
int CHttpServerAssOverHttp::Do_registerURL( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_registerURL");
    CSmsMsg_RegisterReq __reqMsg;

    //todo 使用消息
    if (JsonData.Decode(__reqMsg))
    {
        __reqMsg.dump();

        LOG4CPLUS_DEBUG(LOG_OF_SMS,  "ProductID   :" << __reqMsg.ProductID
                        << "AuthCode :" 	 << __reqMsg.AuthCode
                        << "AuthName    :" << __reqMsg.AuthName
                        << "Password:" 	 << __reqMsg.Password
                        << "LocalIp   :" << __reqMsg.m_LocalStatus.LocalIp
                        << "LocalDateTime :" 	 << __reqMsg.m_LocalStatus.LocalDateTime
                       );
    }

    CSmsStack __CSmsStack;
    CSmsMsg_RegisterAck Msg;
    CSmsMsg_ServiceAgent __service;

#if 1
    //正常
    __service.ServiceName 	 = "PubServiceGetPubIP4";
    __service.ServiceLocation  = "http://pubservice.ezlibs.com:60089/PubService/getPubIP4";
    Msg.m_Service.push_back(__service);
    __service.ServiceName 	 = "KeepConnect";
    __service.ServiceLocation  = "fw001.ezlibs.com:10000";
    Msg.m_Service.push_back(__service);
    Msg.m_ackInfo.Result = "0";
    Msg.m_ackInfo.Reason = "ok";
    Msg.m_Session = "00000000000000000000";

#else
    // 未授权
    __service.ServiceName 	 = "shutdown";
    __service.ServiceLocation  = "http://www.ezlibs.com/shutdown";
    Msg.m_Service.push_back(__service);
    Msg.m_ackInfo.Result = "1"; //
    Msg.m_ackInfo.Reason = "Unauthorized";
    Msg.m_Session = "00000000000000000000";

#endif

    __CSmsStack.Encode(Msg);
    std::cout << __CSmsStack.ToString() << std::endl;

    //    CIPC_registerURL __op;
    //    JsonData.Decode(__op);
    //    __op.dump();
    //    m_sessionId = __op.sessionId;
    //
    //    // 构造返回消息
    //    CezlibsWebServiceIF __CezlibsWebServiceIF;
    //
    //    CIPC_registerURL_ACK Msg;
    //
    //    if (static_cast<CHandlerAssOverHttp&>(Handler()).AuthSession(m_sessionId, EAW_REGISTERURL) ==0)
    //    {
    //
    //        Msg.result = "0";
    //        Msg.reason = "Successful";
    //        m_requestPrefix = __op.baseURL;//TODO 作为全局变量存储该值
    //    }
    //    else
    //    {
    //        Msg.result = "1";
    //        Msg.reason = "Authentication failed";
    //    }
    //    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CSmsStack.ToString()+"]";
    //
    //
    //    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    //    LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);

    return 0;
}

//4.1	鉴权
// curl http://localhost:60600/Sms/requestAuth -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
int CHttpServerAssOverHttp::Do_requestAuth( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestAuth");

    CSmsMsg_requestAuth MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        MsgAuth.dump();
    }

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result	    = "0";
    Msg.Reason	    = "ok";
    m_sessionId = generateSessionId();
    Msg.m_strSessionID      = m_sessionId;//"6f61dfe3-1ec9-4123-826e-37862efd901d";
    Msg.m_strSessionTimeout = "120";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    CMsgService __Service;

    __Service.m_strServiceID   = "2011221100000000015501";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss1.faceaip.com";
    __Service.m_strPort        = "12345";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    __Service.m_strServiceID   = "2011221100000000015502";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss2.faceaip.com";
    __Service.m_strPort        = "12346";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    CSmsStack_requestAuthACK.Encode(Msg);

    //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    ReturnHttpMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/);

    //SetCloseAndDelete();

    return 0;
}
int CHttpServerAssOverHttp::Do_requestRegister( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestRegister");

    CSmsMsg_requestAuth MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        MsgAuth.dump();
    }


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result	    = "0";
    Msg.Reason	    = "ok";
    Msg.m_strSessionID      = "6f61dfe3-1ec9-4123-826e-37862efd901d";
    Msg.m_strSessionTimeout = "120";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    CMsgService __Service;

    __Service.m_strServiceID   = "2011221100000000015501";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss1.faceaip.com";
    __Service.m_strPort        = "12345";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    __Service.m_strServiceID   = "2011221100000000015502";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss2.faceaip.com";
    __Service.m_strPort        = "12346";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    CSmsStack_requestAuthACK.Encode(Msg);

    //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    ReturnHttpMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/);

    SetCloseAndDelete();

    return 0;
}

int CHttpServerAssOverHttp::Do_requestHeartbeat( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestHeartbeat");

    return 0;
}
int CHttpServerAssOverHttp::Do_requestCancel( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestCancel");

    return 0;
}
int CHttpServerAssOverHttp::Do_requestTransit( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestTransit");

    return 0;
}


#if 0
// WEB子系统认证接口
int CHttpServerAssOverHttp::Do_requestAuth( CezlibsWebServiceIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestAuth");
    //    CIPC_requestAuth __op;
    //    JsonData.Decode(__op);
    //    __op.dump();
    //
    //    // 构造返回消息
    //    CezlibsWebServiceIF __CezlibsWebServiceIF;
    //
    //    CAuthResponse Msg;
    //
    //    if(m_sessionId != "")
    //    {
    //        Msg.result = "1";
    //        Msg.reason = "Don't allow more internal connection";
    //        Msg.sessionId = "";
    //    }
    //    else
    //    {
    //        if(__op.authUser == "ipcServer" && __op.authPassword == "123456")
    //        {
    //            Msg.result = "0";
    //            Msg.reason = "Successful";
    //            m_sessionId = generateSessionId();//"hahahahaha";
    //            Msg.sessionId = m_sessionId;
    //
    //            CSession __session;
    //            __session.m_strSession = m_sessionId;
    //
    //            // 权限示例，赋权
    //            __session.m_vAccess.push_back(EAW_HEARTBEAT);
    //            __session.m_vAccess.push_back(EAW_NOTIFYUPDATE);
    //            __session.m_vAccess.push_back(EAW_STOPHOSTNET);
    //            __session.m_vAccess.push_back(EAW_REGISTERURL);
    //            __session.m_vAccess.push_back(EAW_CTRLDEVICE);
    //            //__session.m_ttLastRequestTime = time(NULL);
    //
    //            static_cast<CHandlerAssOverHttp&>(Handler()).newSession(__session);
    //        }
    //        else
    //        {
    //            Msg.result = "1";
    //            Msg.reason = "UserName or password is wrong";
    //            Msg.sessionId = "";
    //        }
    //    }
    //    __CezlibsWebServiceIF.Encode(Msg);
    //    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";
    //
    //
    //    //Send(strSnd);
    //    ReturnHttpMsg(strSnd);
    //    LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);

    return 0;

}

// WEB子系统认证接口
int CHttpServerAssOverHttp::Do_requestAuthByHost( CezlibsWebServiceIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestAuth");
    //    CIPC_requestAuth __op;
    //    JsonData.Decode(__op);
    //    __op.dump();
    //
    //    // 构造返回消息
    //    CezlibsWebServiceIF __CezlibsWebServiceIF;
    //
    //    CAuthResponse Msg;
    //
    //    if(m_sessionId != "")
    //    {
    //        Msg.result = "1";
    //        Msg.reason = "Don't allow more internal connection";
    //        Msg.sessionId = "";
    //    }
    //    else
    //    {
    //        if(__op.authUser == "ipcServer" && __op.authPassword == "123456")
    //        {
    //            Msg.result = "0";
    //            Msg.reason = "Successful";
    //            m_sessionId = generateSessionId();//"hahahahaha";
    //            Msg.sessionId = m_sessionId;
    //
    //            CSession __session;
    //            __session.m_strSession = m_sessionId;
    //
    //            // 权限示例，赋权
    //            __session.m_vAccess.push_back(EAW_HEARTBEAT);
    //            __session.m_vAccess.push_back(EAW_NOTIFYUPDATE);
    //            __session.m_vAccess.push_back(EAW_STOPHOSTNET);
    //            __session.m_vAccess.push_back(EAW_REGISTERURL);
    //            __session.m_vAccess.push_back(EAW_CTRLDEVICE);
    //            //__session.m_ttLastRequestTime = time(NULL);
    //
    //            static_cast<CHandlerAssOverHttp&>(Handler()).newSession(__session);
    //        }
    //        else
    //        {
    //            Msg.result = "1";
    //            Msg.reason = "UserName or password is wrong";
    //            Msg.sessionId = "";
    //        }
    //    }
    //    __CezlibsWebServiceIF.Encode(Msg);
    //    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";
    //
    //
    //    //Send(strSnd);
    //    ReturnHttpMsg(strSnd);
    //    LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);

    return 0;

}

enum EnAuthorityWebService
{
    EAW_HEARTBEAT	=  1,		// 控制面板		_01
    EAW_NOTIFYUPDATE,			// 控制面板		_02
    EAW_STOPHOSTNET,			// 控制面板		_03
    EAW_REGISTERURL,			// 控制面板		_04
    EAW_CTRLDEVICE,				// 控制面板		_05
    EAW_SHUTDOWN,				// 关闭系统		_06
    EAW_END			= -1,
};
// WEB子系统心跳接口
int CHttpServerAssOverHttp::Do_heartbeat( CezlibsWebServiceIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_heartbeat");
    CIPC_heartbeat __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    cout << "__op.sessionId = " << __op.sessionId << endl;

    // 构造返回消息
    CezlibsWebServiceIF __CezlibsWebServiceIF;

    CIPC_heartbeat_ACK Msg;

    if (static_cast<CHandlerAssOverHttp&>(Handler()).AuthSession(m_sessionId, EAW_HEARTBEAT) ==0)
    {
        __fline;
        cout << "AuthSession succeeded." << endl;
        Msg.result = "0";
        Msg.reason = "Heartbeat success";
    }
    else
    {
        __trip;
        cout << "AuthSession FAILED!" << endl;
        Msg.result = "1";
        Msg.reason = "ERR_SESSIONID_LOSE_MATCH";
    }

    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";

    //Send(strSnd);
    ReturnHttpMsg(strSnd, GetUrl());
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);
    ez_printf_info("SendBack:%s\n", strSnd.c_str());

    return 0;

}

//停止主机外网服务接口
int CHttpServerAssOverHttp::Do_stopHostNetwork( CezlibsWebServiceIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_stopHostNetwork");
    CIPC_stopHostNetwork __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    // 构造返回消息
    CezlibsWebServiceIF __CezlibsWebServiceIF;
    CIPC_stopHostNetwork_ACK Msg;

    if(static_cast<CHandlerAssOverHttp&>(Handler()).AuthSession(m_sessionId, EAW_STOPHOSTNET) ==0)
    {

        Msg.result = "0";
        Msg.reason = "OK";
    }
    else
    {
        Msg.result = "1";
        Msg.reason = "ERR_SESSIONID_LOSE_MATCH";
    }

    if(Msg.result != "0")
    {
        __CezlibsWebServiceIF.Encode(Msg);
        std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";

        //Send(strSnd);
        ReturnHttpMsg(strSnd);

        LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);
    }
    return 0;
}
//推送配置更新接口
int CHttpServerAssOverHttp::Do_notifyUpdate( CezlibsWebServiceIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "NOTIFY_UPDATE");
    CIPC_notifyUpdate __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    // 构造返回消息
    CezlibsWebServiceIF __CezlibsWebServiceIF;
    CIPC_notifyUpdate_ACK Msg;
    //CHandlerAssOverHttp handler = dynamic_cast<CHandlerAssOverHttp&>(Handler());

    if(static_cast<CHandlerAssOverHttp&>(Handler()).AuthSession(m_sessionId, EAW_NOTIFYUPDATE) ==0)
    {
        Msg.result = "0";
        Msg.reason = "Success";
    }
    else
    {
        Msg.result = "1";
        Msg.reason = "ERR_SESSIONID_LOSE_MATCH";
    }

    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";
    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);
    return 0;

}

int CHttpServerAssOverHttp::Do_getDeviceStateList( CezlibsWebServiceIF &JsonData)
{
#if 0
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_getDeviceStateList");

    CIPC_getDeviceStateList  __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;
    // 开始使用数据

    // 构造返回消息
    CezlibsWebServiceIF __CezlibsWebServiceIF;

    CIPC_getDeviceStateList_ACK Msg;
    Msg.result = "0";
    Msg.reason = "OK";

    CIPC_getDeviceStateList_ACK_operate _operate;
    _operate.operate_id      = TEST_OPERATE_ID_SW;
    _operate.operate_ranage  = ""; // 开关类的此无需，务必空， 不得填值
    _operate.operate_type    = "0"; //开关
    _operate.operate_value   = "2"; // 关
    _operate.operate_explain = "大灯";
    ezGBKToUTF8(_operate.operate_explain);
    char pBuff[32];
    _operate.value_time       = get_date_time_string(pBuff, NULL);

    Msg.m_operate.push_back(_operate);

    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";


    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);

#endif

    return 0;
}


//发送设备操作报文
int CHttpServerAssOverHttp::sendDeviceCtrlMsg(char *ip,
        const int port,
        const char *operate_id,
        const char *value,
        const u_char value_len,
        const int32_t session_id)
{

    return 0;
}

int CHttpServerAssOverHttp::Do_controlDevice(CezlibsWebServiceIF &JsonData)
{
    CIPC_controlDevice   __op;
    CIPC_controlDevice_operate op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    /*
      *构造返回消息，此接口只负责发送控制请求报文和回复错误信息。
      *控制结果(响应消息)等收到来自主机的回复后再发送给web端。
      *所以，该接口需要保存一个web通信对象和主机的配对关系，
      *在收到主机的回复消息时，便于查询相关的web通信对象。
      */
    CezlibsWebServiceIF __CezlibsWebServiceIF;
    CIPC_controlDevice_ACK Msg;

    if(static_cast<CHandlerAssOverHttp&>(Handler()).AuthSession(m_sessionId, EAW_CTRLDEVICE) ==0)
    {

        Msg.result  = "0" ;
        Msg.reason  = "OK" ;
        return 0;
    }
    else
    {
        Msg.result  = "1" ;
        Msg.reason  = "ERR_SESSIONID_LOSE_MATCH" ;
    }

    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";

    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);
    return 0;
}

int CHttpServerAssOverHttp::Do_getPubIP4( CezlibsWebServiceIF &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_getPubIP4");

    std::string strSendBuf;
    if (JsonData.isValid())
    {
        __fline;
        cout << "Do_TestIF, your json:" << JsonData.ToString()<< endl;
        strSendBuf = "[{\"result\":\"0\",\"reason\":\"Do_TestIF, Your URL:" + GetUrl() + "\"}]";
    }
    else
    {
        //strSendBuf = 	GetRemoteAddress();
        strSendBuf = 	"PUIP="+GetRemoteAddress()+" ";

    }

    ReturnHttpMsg(strSendBuf);

    SetCloseAndDelete();

    return 0;
}
#endif

