/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerWebService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerWebService.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#endif //HAVE_LIBUUID

#include "HttpServerWebService.h"
#include "../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../Configs/ConfigWebService.h"
#endif //_USE_MODULES_EZCONFIG
#include "str_opr.h"
#include "StringOpr.h"
#include "HandlerWebService.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
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

CHttpServerWebService::CHttpServerWebService(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";
    m_sessionId = "";
}

void CHttpServerWebService::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_WEBSERVICES, "CHttpServerWebService::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerWebService::~CHttpServerWebService()
{
    LOG4CPLUS_INFO(LOG_WEBSERVICES, "CHttpServerWebService::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerWebService::Exec()
{
    CreateHeader();
    GenerateDocument();
}
void CHttpServerWebService::OnHeaderComplete()
{
    m_strBody = "";
}

void CHttpServerWebService::OnUnknowRequest()
{

}

void CHttpServerWebService::CreateHeader(size_t ContentLength, const std::string &matchRequest)
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

void CHttpServerWebService::GenerateDocument()
{
    std::string strSendBuf = "Hello CHttpServerWebService.\n\r";

    strSendBuf += GetHttpDate()+"\r\n";
    strSendBuf += "Your Body:"+m_strBody+"\n\r";

    strSendBuf = "[{\"result\":\"100\",\"reason\":\"CHttpServerWebService not support url:" + GetUrl() + "\"}]";

    //Send(strSendBuf);
    ReturnHttpMsg(strSendBuf);
}

/** Chunk of http body data recevied. */
void CHttpServerWebService::OnData(const char *p,size_t l)
{
    if (m_strBody.size()>4096 || l>4096)
    {
        LOG4CPLUS_ERROR(LOG_WEBSERVICES, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too big");
        std::string strBadboy = "Bad boy.";

        ReturnHttpMsg(strBadboy);

        //Reset(); // prepare for next request
        //SetCloseAndDelete();
        //m_strBody = "";

        return;
    }

    m_strBody += p;
}

void CHttpServerWebService::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#ifdef _USE_MODULES_EZCONFIG
    CConfigWebService __cfg;
    __cfg.update();
#endif //_USE_MODULES_EZCONFIG

    //LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "m_strBody:" << m_strBody);

    if (m_strBody.size()<strlen("[{}]"))
    {
        LOG4CPLUS_ERROR(LOG_WEBSERVICES, "m_strBody(" << m_strBody.size() << ") should 3 or bigger");

        m_strBody =  "[{\"result\":\"101\",\"reason\":\"m_strBody.size() < 3\"}]";

        ReturnHttpMsg(m_strBody);

        //m_strBody = "";

        return;
    }

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

    LOG4CPLUS_INFO(LOG_WEBSERVICES, "Html request:"
                   << "\n\t\t Method: "<<GetMethod()
                   << "\n\t\t URL: "<<GetUrl()
                   << "\n\t\t Http version: "<<GetHttpVersion()
                   << "\n\t\t Body: "<<m_strBody << "\n\t\t\t");

    CWebServiceMsg __CezlibsWebServiceIF;
    __CezlibsWebServiceIF.Parse(m_strBody);

    //CreateHeader();

    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Request URL:" << GetUrl());

    //exa:
    //	curl http://192.168.10.125:60088/WebService/TestIF -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    // test interface
    if (GetUrl()=="/WebService/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    else if (GetUrl()=="/WebService/Register")
    {
        Do_requestAuth(__CezlibsWebServiceIF);
    }//主机认证接口
#if 0
    //认证接口
    //	http://218.95.39.172:60088/WebService/requestReportDeviceConfigParam
    else if (GetUrl()==__cfg.getConfig().strURL_requestAuth)
    {
        Do_requestAuth(__CezlibsWebServiceIF);
    }//主机认证接口
    //	http://218.95.39.172:60088/WebService/requestReportDeviceConfigParam
    else if (GetUrl()==__cfg.getConfig().strURL_requestAuthByHost)
    {
        Do_requestAuthByHost(__CezlibsWebServiceIF);
    }
    //5.2.6	心跳接口 getCtrlDeviceState
    //	http://192.168.10.216:60088/WebService/heartbeat
    else if (GetUrl()==__cfg.getConfig().strURL_heartbeat)
    {
        Do_heartbeat(__CezlibsWebServiceIF);
    }
    //5.2.8	获取设备状态接口 getDeviceStateList
    //	http://218.95.39.172:60088/WebService/getDeviceStateList
    //
    else if (GetUrl()==__cfg.getConfig().strURL_registerURL)
    {
        Do_registerURL(__CezlibsWebServiceIF);
    }
    //5.2.4	请求配置上报接口
    //	http://218.95.39.172:60088/WebService/requestReportDeviceConfigParam
    else if (GetUrl()==__cfg.getConfig().strURL_stopHostNetwork)
    {
        Do_stopHostNetwork(__CezlibsWebServiceIF);
    }
    //5.2.6	获取网关状态接口 getCtrlDeviceState
    //	http://218.95.39.172:60088/WebService/getCtrlDeviceState
    else if (GetUrl()==__cfg.getConfig().strURL_notifyUpdate)
    {
        Do_notifyUpdate(__CezlibsWebServiceIF);
    }
    //5.2.8	获取设备状态接口 getDeviceStateList
    //	http://218.95.39.172:60088/WebService/getDeviceStateList
    //
    else if (GetUrl()==__cfg.getConfig().strURL_getDeviceStateList)
    {
        Do_getDeviceStateList(__CezlibsWebServiceIF);
    }
    //5.2.9	控制命令接口 controlDevice
    //	http://218.95.39.172:60088/WebService/controlDevice
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

int CHttpServerWebService::ReturnHttpMsg(const std::string &strMsg, const std::string &matchRequest)
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

std::string CHttpServerWebService::generateSessionId()
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

std::string CHttpServerWebService::getMySessionId()
{
    return m_sessionId;
}
// curl http://localhost:60088/WebService/TestIF -d "[{\"reason\":\"I Love You.\",\"result\":\"0\"}]"
int CHttpServerWebService::Do_TestIF( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_TestIF");
    __fline;
    cout << "Do_TestIF, Valid JSON:" << JsonData.ToString() << endl;

    std::string strSendBuf;

    strSendBuf = "[{\"result\":\"0\",\"reason\":\"Do_TestIF, Uri:" + GetUrl() + "\"}]";

    ReturnHttpMsg(strSendBuf);

    return 0;
}

// WEB子系统认证接口
int CHttpServerWebService::Do_Register( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_Register");
    CIPC_requestAuth __op;
    JsonData.Decode(__op);
    __op.dump();

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;

    CAuthResponse Msg;

    if(m_sessionId != "")
    {
        Msg.result = "1";
        Msg.reason = "Don't allow more internal connection";
        Msg.sessionId = "";
    }
    else
    {
        if(__op.authUser == "ipcServer" && __op.authPassword == "123456")
        {
            Msg.result = "0";
            Msg.reason = "Successful";
            m_sessionId = generateSessionId();//"hahahahaha";
            Msg.sessionId = m_sessionId;

            CSession __session;
            __session.m_strSession = m_sessionId;

            // 权限示例，赋权
            __session.m_vAccess.push_back(EAW_HEARTBEAT);
            __session.m_vAccess.push_back(EAW_NOTIFYUPDATE);
            __session.m_vAccess.push_back(EAW_STOPHOSTNET);
            __session.m_vAccess.push_back(EAW_REGISTERURL);
            __session.m_vAccess.push_back(EAW_CTRLDEVICE);
            //__session.m_ttLastRequestTime = time(NULL);

            static_cast<CHandlerWebService&>(Handler()).newSession(__session);
        }
        else
        {
            Msg.result = "1";
            Msg.reason = "UserName or password is wrong";
            Msg.sessionId = "";
        }
    }
    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";


    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);

    return 0;

}


// WEB子系统向连辅子系统注册接口地址的接口
int CHttpServerWebService::Do_registerURL( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_registerURL");
    CIPC_registerURL __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;

    CIPC_registerURL_ACK Msg;

    if (static_cast<CHandlerWebService&>(Handler()).AuthSession(m_sessionId, EAW_REGISTERURL) ==0)
    {

        Msg.result = "0";
        Msg.reason = "Successful";
        m_requestPrefix = __op.baseURL;//TODO 作为全局变量存储该值
    }
    else
    {
        Msg.result = "1";
        Msg.reason = "Authentication failed";
    }
    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";


    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);

    return 0;
}
// WEB子系统认证接口
int CHttpServerWebService::Do_requestAuth( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_requestAuth");
    CIPC_requestAuth __op;
    JsonData.Decode(__op);
    __op.dump();

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;

    CAuthResponse Msg;

    if(m_sessionId != "")
    {
        Msg.result = "1";
        Msg.reason = "Don't allow more internal connection";
        Msg.sessionId = "";
    }
    else
    {
        if(__op.authUser == "ipcServer" && __op.authPassword == "123456")
        {
            Msg.result = "0";
            Msg.reason = "Successful";
            m_sessionId = generateSessionId();//"hahahahaha";
            Msg.sessionId = m_sessionId;

            CSession __session;
            __session.m_strSession = m_sessionId;

            // 权限示例，赋权
            __session.m_vAccess.push_back(EAW_HEARTBEAT);
            __session.m_vAccess.push_back(EAW_NOTIFYUPDATE);
            __session.m_vAccess.push_back(EAW_STOPHOSTNET);
            __session.m_vAccess.push_back(EAW_REGISTERURL);
            __session.m_vAccess.push_back(EAW_CTRLDEVICE);
            //__session.m_ttLastRequestTime = time(NULL);

            static_cast<CHandlerWebService&>(Handler()).newSession(__session);
        }
        else
        {
            Msg.result = "1";
            Msg.reason = "UserName or password is wrong";
            Msg.sessionId = "";
        }
    }
    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";


    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);

    return 0;

}

// WEB子系统认证接口
#if 1
int CHttpServerWebService::Do_requestAuthByHost( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_requestAuth");
    CIPC_requestAuth __op;
    JsonData.Decode(__op);
    __op.dump();

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;

    CAuthResponse Msg;

    if(m_sessionId != "")
    {
        Msg.result = "1";
        Msg.reason = "Don't allow more internal connection";
        Msg.sessionId = "";
    }
    else
    {
        if(__op.authUser == "ipcServer" && __op.authPassword == "123456")
        {
            Msg.result = "0";
            Msg.reason = "Successful";
            m_sessionId = generateSessionId();//"hahahahaha";
            Msg.sessionId = m_sessionId;

            CSession __session;
            __session.m_strSession = m_sessionId;

            // 权限示例，赋权
            __session.m_vAccess.push_back(EAW_HEARTBEAT);
            __session.m_vAccess.push_back(EAW_NOTIFYUPDATE);
            __session.m_vAccess.push_back(EAW_STOPHOSTNET);
            __session.m_vAccess.push_back(EAW_REGISTERURL);
            __session.m_vAccess.push_back(EAW_CTRLDEVICE);
            //__session.m_ttLastRequestTime = time(NULL);

            static_cast<CHandlerWebService&>(Handler()).newSession(__session);
        }
        else
        {
            Msg.result = "1";
            Msg.reason = "UserName or password is wrong";
            Msg.sessionId = "";
        }
    }
    __CezlibsWebServiceIF.Encode(Msg);
    std::string strSnd = "["+__CezlibsWebServiceIF.ToString()+"]";


    //Send(strSnd);
    ReturnHttpMsg(strSnd);
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);

    return 0;

}
#endif
// WEB子系统心跳接口
int CHttpServerWebService::Do_heartbeat( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_heartbeat");
    CIPC_heartbeat __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    cout << "__op.sessionId = " << __op.sessionId << endl;

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;

    CIPC_heartbeat_ACK Msg;

    if (static_cast<CHandlerWebService&>(Handler()).AuthSession(m_sessionId, EAW_HEARTBEAT) ==0)
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
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);

    return 0;

}

//停止主机外网服务接口
int CHttpServerWebService::Do_stopHostNetwork( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_stopHostNetwork");
    CIPC_stopHostNetwork __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;
    CIPC_stopHostNetwork_ACK Msg;

    if(static_cast<CHandlerWebService&>(Handler()).AuthSession(m_sessionId, EAW_STOPHOSTNET) ==0)
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

        LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);
    }
    return 0;
}
//推送配置更新接口
int CHttpServerWebService::Do_notifyUpdate( CWebServiceMsg &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "NOTIFY_UPDATE");
    CIPC_notifyUpdate __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;
    CIPC_notifyUpdate_ACK Msg;
    //CHandlerWebService handler = dynamic_cast<CHandlerWebService&>(Handler());

    if(static_cast<CHandlerWebService&>(Handler()).AuthSession(m_sessionId, EAW_NOTIFYUPDATE) ==0)
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
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "SendBack:" << strSnd);
    return 0;

}

int CHttpServerWebService::Do_getDeviceStateList( CWebServiceMsg &JsonData)
{
#if 0
    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Do_getDeviceStateList");

    CIPC_getDeviceStateList  __op;
    JsonData.Decode(__op);
    __op.dump();
    m_sessionId = __op.sessionId;
    // 开始使用数据

    // 构造返回消息
    CWebServiceMsg __CezlibsWebServiceIF;

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
int CHttpServerWebService::sendDeviceCtrlMsg(char *ip,
        const int port,
        const char *operate_id,
        const char *value,
        const u_char value_len,
        const int32_t session_id)
{

    return 0;
}

int CHttpServerWebService::Do_controlDevice(CWebServiceMsg &JsonData)
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
    CWebServiceMsg __CezlibsWebServiceIF;
    CIPC_controlDevice_ACK Msg;

    if(static_cast<CHandlerWebService&>(Handler()).AuthSession(m_sessionId, EAW_CTRLDEVICE) ==0)
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

