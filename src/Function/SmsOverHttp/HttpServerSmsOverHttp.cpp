/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerSmsOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerSmsOverHttp.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#include "ez_util_platform.h"

#include "HttpServerSmsOverHttp.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigSmsOverHttp.h"
#endif //_USE_MODULES_EZCONFIG
#include "str_opr.h"
#include "StringOpr.h"
#include "HandlerSmsOverHttp.h"

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

//5152f876-c738-4e96-9c2b-b314501c40f1
#define MY_SESSION_ID_LEN 36
// 30s
#define conn_TIMEOUT_DEFAULT_CHttpServerSmsOverHttp 30
#define conn_TIMEOUT_DEFAULT_CHttpServerSmsOverHttp_str "30"

CHttpServerSmsOverHttp::CHttpServerSmsOverHttp(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";
    m_sessionId = "";

    m_iTimeout = conn_TIMEOUT_DEFAULT_CHttpServerSmsOverHttp;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;
}

void CHttpServerSmsOverHttp::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerSmsOverHttp::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerSmsOverHttp::~CHttpServerSmsOverHttp()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerSmsOverHttp::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerSmsOverHttp::OnAccept()
{
    printf("CHttpServerSmsOverHttp::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    fprintf(stdout, "CHttpServerSmsOverHttp::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}
void CHttpServerSmsOverHttp::OnDisconnect()
{
    //printf("CHttpServerSmsOverHttp::OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    //printf("CHttpServerSmsOverHttp::OnDisconnect()\n");
    //ez_printf_info("%s(%d) -- OnDisconnect:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
    printf("%s(%d) -- OnDisconnect:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerSmsOverHttp::Exec()
{
    CreateHeader();
    //RetGeneralMsg();
    RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}
void CHttpServerSmsOverHttp::OnHeaderComplete()
{
    m_strBody = "";
}

void CHttpServerSmsOverHttp::OnUnknowRequest()
{}

void CHttpServerSmsOverHttp::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
#if 0
    // 下方代码当此服务器主动发消息，此时uri为空串，会出问题
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
#else
    // we are json
    // http://www.ietf.org/rfc/rfc4627.txt
    AddResponseHeader("Content-type", "application/json");
#endif

    //AddResponseHeader("Connection", "close");
    AddResponseHeader("Connection", "Keep-Alive");

    if (m_sessionId.size()==MY_SESSION_ID_LEN)
    {
        AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
    }
    else
    {
        // debug only
        m_sessionId = "joy-dbg-only";
        AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
        m_sessionId="";
    }

    if (ContentLength>0)
    {
        AddResponseHeader( "Content-length", Utility::l2string((long)ContentLength) );
    }

    if (matchRequest.size()>0)
    {
        AddResponseHeader( "Match-request", matchRequest);
    }
    SendResponse();
}

void CHttpServerSmsOverHttp::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
{
    std::stringstream __ss;
    //std::string strSendBuf ="";
    if (-1 == retCode)
    {
        __ss << "[{\"result\":\"100\",\"reason\":\"CHttpServerSmsOverHttp not support url:" + GetUrl() + "\"}]";
    }
    else
    {
        __ss << "[{\"result\":\""
			<< retCode
			<< "\",\"reason\":\"CHttpServerSmsOverHttp not support url:" 
			<< strMsg 
			<< "\"}]";
    }

    SendSmsMsg(__ss.str(), matchRequest);
}


/** Chunk of http body data recevied. */
void CHttpServerSmsOverHttp::OnData(const char *p,size_t l)
{
    if (m_strBody.size()>4096 || l>4096)
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too big");
        std::string strBadboy = "Bad boy.";

        SendSmsMsg(strBadboy);

        //Reset(); // prepare for next request
        //SetCloseAndDelete();
        //m_strBody = "";

        return;
    }

    m_strBody += p;

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerSmsOverHttp::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#ifdef _USE_MODULES_EZCONFIG
    CConfigSmsOverHttp __cfg;
    __cfg.update();
#endif//

    CSmsStack __CezlibsWebServiceIF;

    //LOG4CPLUS_DEBUG(LOG_OF_SMS, "m_strBody:" << m_strBody);

    if (m_strBody.size()<strlen("[{}]"))
    {
        LOG4CPLUS_INFO(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ")" << " [" << m_strBody << "] not json");
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


        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);
        if (bRet)
        {
            m_iMsgCountGet ++;
        }
        else
        {
            LOG4CPLUS_INFO(LOG_OF_SMS, "__CezlibsWebServiceIF.Parse failed.");
        }
    }

    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Request URL:" << GetUrl());
    EZ_LOGS(
        ez_log("%s(%d) -- OnMsg:%s\n", __FILE__, __LINE__, GetUrl().c_str());
    );
    LOG4CPLUS_INFO(LOG_OF_SMS, "Html request:"
                   << "\n\t\t Method: "<<GetMethod()
                   << "\n\t\t URL: "<<GetUrl()
                   << "\n\t\t Http version: "<<GetHttpVersion()
                   << "\n\t\t Body: "<<m_strBody << "\n\t\t\t");

    EZ_LOGS(
        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, m_strBody.c_str());
        //ez_log("------------------------------------------\n");
        //ez_log("%s\n", m_strBody.c_str());
        //ez_log("\n------------------------------------------\n");
    );

	ez_printf_info("GetUrl:%s\n", GetUrl().c_str());

    //exa:
    //	curl http://localhost:60088/sms/register -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    //	curl http://localhost:60088/sms/register -d ""
    //	curl http://localhost:60088/sms/register
    // test interface
#ifdef _USE_MODULES_EZCONFIG

    if (GetUrl()==__cfg.getConfig().strURL_registerURL)
    {
        Do_registerURL(__CezlibsWebServiceIF);
    }
    //exa:
    //	curl http://localhost:60088/sms/testIF -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    // test interface
    else if (GetUrl()=="/sms/testIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    //鉴权接口
    //	curl http://localhost:60088/sms/requestAuth -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
    else if (GetUrl()==__cfg.getConfig().strURIrequestAuth)
    {
        Do_requestAuth(__CezlibsWebServiceIF);
    }//鉴权接口
    //注册接口
    //	http://localhost:60088/sms/requestRegister
    else if (GetUrl()==__cfg.getConfig().strURIrequestRegister)
    {
        Do_requestRegister(__CezlibsWebServiceIF);
    }//接口

    //心跳接口
    //	curl http://localhost:60088/sms/requestHeartbeat -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
    else if (GetUrl()==__cfg.getConfig().strURIrequestHeartbeat)
    {
        Do_requestHeartbeat(__CezlibsWebServiceIF);
    }//心跳接口
    //注销接口
    //	http://localhost:60088/Sms/requestCancel
    // curl http://localhost:60088/sms/requestCancel -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
    else if (GetUrl()==__cfg.getConfig().strURIrequestCancel)
    {
        Do_requestCancel(__CezlibsWebServiceIF);
    }//注销接口
    //中转接口
    //	http://localhost:60088/Sms/requestTransit
    else if (GetUrl()==__cfg.getConfig().strURIrequestTransit)
    {
        Do_requestTransit(__CezlibsWebServiceIF);
    }//中转接口
#else // not define _USE_MODULES_EZCONFIG
    if (GetUrl()=="/sms/testIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
#endif //_USE_MODULES_EZCONFIG

    else
    {
        RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
//        RetGeneralMsg();
    }

    Reset(); // prepare for next request
    //SetCloseAndDelete();
    m_strBody = "";
}

void CHttpServerSmsOverHttp::update()
{
    unsigned long ttnow = ez_sec_get_tick_count();//time(NULL);
#if 1

    DBG(
        cout << ttnow-m_ttOndata << " tout:" << m_iTimeout << endl;
    );
    if (ttnow-m_ttOndata>m_iTimeout)
    {
        __fline;
        std::cout << "timeout :" <<  getMySessionId()  << std::endl << "SetCloseAndDelete ..."<< std::endl;

        SetCloseAndDelete();
    }
#endif
}

size_t CHttpServerSmsOverHttp::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerSmsOverHttp::getMsgNumSent()
{
    return m_iMsgCountSend;
}


// http + json
int CHttpServerSmsOverHttp::SendSmsMsg(const std::string &strMsg, const std::string &matchRequest)
{
    DBG(
        cout << "strMsg.size() :" <<  strMsg.size()  << endl;
        cout << "strMsg :" <<  strMsg  << endl;
    );
    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    EZ_LOGS(
        ez_log("%s(%d) -- SendSmsMsg:%s\n", __FILE__, __LINE__, GetUrl().c_str());
    );

    EZ_LOGS(
        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, strMsg.c_str());
    );

    Reset();
    //SetCloseAndDelete();
    m_strBody = "";

    m_iMsgCountSend++;

    return 0;
}

std::string CHttpServerSmsOverHttp::generateSessionId()
{
    char uuidBuff[MY_SESSION_ID_LEN+1] = {0};

    // single thread, do not add lock here
#ifdef HAVE_LIBUUID

    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuidBuff);
#else
    // 存在安全漏洞
    static unsigned short idNum = time(NULL);
    sprintf(uuidBuff, "6933c3d9-74f6-489e-a614-32de%08d", idNum);
    idNum ++;
#endif //HAVE_LIBUUID

    return uuidBuff;
}

std::string CHttpServerSmsOverHttp::getMySessionId()
{
    //__fline;
    //cout << "getMySessionId:" << m_sessionId << endl;

    return m_sessionId;
}

// curl http://localhost:60088/Sms/TestIF -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerSmsOverHttp::Do_TestIF( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_TestIF");
    __fline;
    cout << "Do_TestIF, your json:" << JsonData.ToString()<< endl;

    std::string strSendBuf;

    strSendBuf = "[{\"result\":\"0\",\"reason\":\"Do_TestIF, Your URL:" + GetUrl() + "\"}]";

    SendSmsMsg(strSendBuf);

    return 0;
}

// WEB子系统向连辅子系统注册接口地址的接口
// curl http://sms.ezlibs.com:60088/SmsOverHttp/registerURL -d "{ \"AuthCode\" : \"VAuthCode\", \"AuthName\" : \"VAuthName\", \"LocalDateTime\" : \"VLocalDateTime\", \"LocalIp\" : \"VLocalIp\", \"Password\" : \"VPassword\", \"ProductID\" : \"VProductID\" }\n"
int CHttpServerSmsOverHttp::Do_registerURL( CSmsStack &JsonData)
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
    //    if (static_cast<CHandlerSmsOverHttp&>(Handler()).AuthSession(m_sessionId, EAW_REGISTERURL) ==0)
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
    SendSmsMsg(strSnd);
    //    LOG4CPLUS_DEBUG(LOG_OF_SMS, "SendBack:" << strSnd);

    return 0;
}

// curl http://localhost:60088/sms/requestAuth -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
// curl http://localhost:60088/sms/requestRegister -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
// curl http://localhost:60088/sms/requestHeartbeat -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
// curl http://localhost:60088/sms/requestCancel -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
// curl http://localhost:60088/sms/requestTransit -d "{ \"Dest\" : { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, \"From\" : { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, \"Msg\" : { \"Encode\" : \"string\", \"Len\" : \"0\", \"Type\" : \"string\", \"Value\" : \"hello json\" }, \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";
//4.1	鉴权
#define SERVICE_PROTOCOL_SERVER_GEMINILOCK "ss_GeminiLock_http" // 双子锁服务协议
int CHttpServerSmsOverHttp::Do_requestAuth( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestAuth");

    CSmsMsg_requestAuth MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        DBG(
            MsgAuth.dump();
        );
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
        return -1;
    }

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
    // 重新认证, 返回新sid
    m_sessionId = generateSessionId();
    Msg.m_strSessionID      = m_sessionId;//"6f61dfe3-1ec9-4123-826e-37862efd901d";

	std::ostringstream __ostr; __ostr << m_iTimeout;
    Msg.m_strSessionTimeout = __ostr.str();

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    CMsgService __Service;

    __Service.m_strServiceID   = "2011221100000000015501";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss1.faceaip.com";
    __Service.m_strPort        = "60600";
    __Service.m_strProtocol    = SERVICE_PROTOCOL_SERVER_GEMINILOCK;
    Msg.m_vLocalService.push_back(__Service);

    __Service.m_strServiceID   = "2011221100000000015502";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss.faceaip.com";
    __Service.m_strPort        = "60600";
    __Service.m_strProtocol    = SERVICE_PROTOCOL_SERVER_GEMINILOCK;
    Msg.m_vLocalService.push_back(__Service);

    //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    CSmsStack_requestAuthACK.Encode(Msg);

    //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    //SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, "requestAuth");
    SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, GetUrl());

    //SetCloseAndDelete();

    return 0;
}
int CHttpServerSmsOverHttp::Do_requestRegister( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestRegister");

    CSmsMsg_requestAuth MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        DBG(
            MsgAuth.dump();
        );
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
        return -1;
    }

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
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

    SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/);

    //SetCloseAndDelete();

    return 0;
}

int CHttpServerSmsOverHttp::Do_requestHeartbeat( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestHeartbeat");

    CSmsMsg_requestAuth MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        DBG(
            MsgAuth.dump();
        );
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
        return -1;
    }


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
    // 重新认证, 返回新sid
    //m_sessionId = generateSessionId();
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

    SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, GetUrl());

    //SetCloseAndDelete();

    return 0;
}
int CHttpServerSmsOverHttp::Do_requestCancel( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestHeartbeat");

    CSmsMsg_requestAuth MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        DBG(
            MsgAuth.dump();
        );
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
        return -1;
    }


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
    // 重新认证, 返回新sid
    //m_sessionId = generateSessionId();
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

    SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, GetUrl());

    //SetCloseAndDelete();

    return 0;
}
int CHttpServerSmsOverHttp::Do_requestTransit( CSmsStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_requestTransit");

    CSmsMsg_requestSend MsgAuth;

    if (JsonData.Decode(MsgAuth))
    {
        DBG(
            MsgAuth.dump();
        );
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
		//SetCloseAndDelete();
        return -1;
    }

    // 测试用， 目前客户端仅支持认证返回，
    // todo: 不同的返回如何区分  : AddResponseHeader( "Match-request", matchRequest);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestSendACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
    m_sessionId = generateSessionId();
    Msg.m_strSessionID      = m_sessionId;//"6f61dfe3-1ec9-4123-826e-37862efd901d";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    Msg.m_From.m_strServiceID   = "2011221100000000015501";
    Msg.m_From.m_strDescription = "ServiceManagementServer";
    Msg.m_From.m_strURI         = "ss1.faceaip.com";
    Msg.m_From.m_strPort        = "12345";
    Msg.m_From.m_strProtocol    = "ss_GeminiLock_http";

    Msg.m_Dest.m_strServiceID   = "2011221100000000015501";
    Msg.m_Dest.m_strDescription = "ServiceManagementServer";
    Msg.m_Dest.m_strURI         = "ss1.faceaip.com";
    Msg.m_Dest.m_strPort        = "12345";
    Msg.m_Dest.m_strProtocol    = "ss_GeminiLock_http";

    CSmsStack_requestAuthACK.Encode(Msg);

    SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, "CSmsMsg_requestSendACK");

    //SetCloseAndDelete();

    return 0;
}

int CHttpServerSmsOverHttp::AckMsg()
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "AckMsg");

    // 测试用， 目前客户端仅支持认证返回，
    // todo: 不同的返回如何区分  : AddResponseHeader( "Match-request", matchRequest);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CSmsStack CSmsStack_requestAuthACK;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
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

    SendSmsMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, "/Sms/bcastMsg");

    //SetCloseAndDelete();

    return 0;
}

