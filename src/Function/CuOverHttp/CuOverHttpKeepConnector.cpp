/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CuOverHttpKeepConnector.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: CuOverHttpKeepConnector.cpp 5884 2013-12-18 03:38:04Z WuJunjie $
 *
 *  Notes:
 *     -
 *      支持多实例 
 *     -
 *
 *  Update:
 *     2013-12-18 03:38:04 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include "../../Logs.h"

#include "../Stack/SmsStack.h"

#include "CuOverHttpKeepConnector.h"

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


//事务处理状态
typedef enum en_icp_logic_status
{
    enAppLogicStatus_Begin  =  0,
    enAppLogicStatus_Logining  =  1,
    enAppLogicStatus_Logined  =  2,

    enAppLogicStatus_Heartbeating,
    enAppLogicStatus_Heartbeated,

    enAppLogicStatus_Exiting,
    enAppLogicStatus_End,
}enAppLogicStatus_PHKC;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define BUF_LEN 24*1024
#define DCB_CALLBACK(x) int http_##x##_callback_CuOverHttpKeepConnector(http_parser *parser, const char *at, size_t length)
#define CB_CALLBACK(x)  int http_##x##_callback_CuOverHttpKeepConnector(http_parser *parser)
#define TOKEN "---------------------------------- "

DCB_CALLBACK(header_field)
{
#define CODES \
    char str_buf[BUF_LEN];\
	if (length<BUF_LEN) { \
    memcpy(str_buf, at, length);\
	str_buf[length] = '\0'; } \
	else str_buf[0] = '\0';

    CODES
    DEB(
        printf("%s : ", str_buf);
    );

    if (parser && parser->data)
    {
        ((CCuOverHttpKeepConnector *)(parser->data))->OnHeaderField(str_buf);
    }

    return 0;
}

DCB_CALLBACK(header_value)
{
    CODES
    DEB(
        printf("%s : ", str_buf);
    );

    if (parser && parser->data)
    {
        ((CCuOverHttpKeepConnector *)(parser->data))->OnHeaderValue(str_buf);
    }

    return 0;
}

DCB_CALLBACK(body)
{
    CODES
    DEB(
        printf("%s : ", str_buf);
    );

    if (parser && parser->data)
    {
        ((CCuOverHttpKeepConnector *)(parser->data))->OnBody(str_buf);
    }

    return 0;
}

DCB_CALLBACK(url)
{
    CODES
    DEB(
        printf("%s : ", str_buf);
    );
    return 0;
}

CB_CALLBACK(message_begin)
{
    DEB(
        printf(TOKEN"message_begin""\n");
    );
    return 0;
}

CB_CALLBACK(message_complete)
{
    DEB(
        printf(TOKEN"message_complete""\n");
    );

    // 结束了， 初始化，等待下面的数据
    http_parser_init(parser, HTTP_RESPONSE);

    return 0;
}

CB_CALLBACK(headers_complete)
{
    DEB(
        printf(TOKEN"headers_complete""\n");
    );

    return 0;
}

// 登录重登陆次数 超过则重建
// 有些设备需要重连
#define LOGIN_COUNT_DOWN 3

//////////////////////////////////////////////////
CCuOverHttpKeepConnector::CCuOverHttpKeepConnector(ISocketHandler& h, std::string strSocketName)
        :TcpSocket(h)
        //,CEZThread(CCuOverHttpKeepConnector_THREAD_ATTR)
        ,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(2);

    SetDeleteByHandler();

    SetSockName(strSocketName);

    DisableInputBuffer();

    m_tOnData = 0;
    m_strUName = "";
    m_strUPwd = "";
    m_ttConnectTimeOut = TCP_CONN_TIMEOUT;

    m_enCurMsgStatus = enAppLogicStatus_Begin;

    m_tOnMsg = time(NULL);
    m_ttMsgTimeOut = TCP_MSGG_TIMEOUT;

    m_pHttpParserCli = (http_parser*)malloc(sizeof(http_parser));
    m_pHttpParserSettings = (http_parser_settings*) malloc(sizeof(http_parser_settings));
    assert(m_pHttpParserCli);
    assert(m_pHttpParserSettings);

    // 仅解析反馈的数据
    http_parser_init(m_pHttpParserCli, HTTP_RESPONSE);
    // 初始化之后， obj为NULL 单独赋值
    m_pHttpParserCli->data = this;

#define NCB(n) http_##n##_callback_CuOverHttpKeepConnector
    #define SCB(x) m_pHttpParserSettings->on_##x = NCB(x)

    SCB(url);
    SCB(header_field);
    SCB(header_value);
    SCB(body);
    SCB(message_begin);
    SCB(headers_complete);
    SCB(message_complete);

    //m_iRecvPacket = 0;
    m_iSentPacket = 0;
}

CCuOverHttpKeepConnector::~CCuOverHttpKeepConnector()
{
    if (m_pHttpParserCli)
    {
        free (m_pHttpParserCli);
    }
    if (m_pHttpParserSettings)
    {
        free(m_pHttpParserSettings);
    }
}

CCuOverHttpKeepConnector *CCuOverHttpKeepConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CCuOverHttpKeepConnector *p = new CCuOverHttpKeepConnector(Handler());
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);

    return p;
}


void CCuOverHttpKeepConnector::OnConnectFailed()
{
    CCuOverHttpKeepConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CCuOverHttpKeepConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CCuOverHttpKeepConnector::SetCloseAndDelete()
{
    TcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CCuOverHttpKeepConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    printf("CCuOverHttpKeepConnector::OnConnect() - %ld\n", time(NULL));
}
void CCuOverHttpKeepConnector::OnReconnect()
{
    printf("CCuOverHttpKeepConnector::OnReconnect()\n");
}

void CCuOverHttpKeepConnector::OnDisconnect()
{
    printf("CCuOverHttpKeepConnector::OnDisconnect() - %ld\n", time(NULL));
}

void CCuOverHttpKeepConnector::OnConnectTimeout()
{
    printf("CCuOverHttpKeepConnector::OnConnectTimeout()\n");
}

void CCuOverHttpKeepConnector::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);
    //m_iRecvPacket++;

    DBG(
        __fline;
        printf("CGeneralAgentTcpSocket::OnRawData:%d\n", len);

        std::string strData((char *)buf, len);
        printf("------------------------------------------\n");
        printf("%s", strData.c_str());
        printf("\n------------------------------------------\n");
    );
    //    EZ_LOGS(
    //        std::string strData((char *)buf, len);
    //        ez_log("%s(%d)--\n", __FILE__, __LINE__);
    //        ez_log("------------------------------------------\n");
    //        ez_log("%s", strData.c_str());
    //        ez_log("\n------------------------------------------\n");
    //    );

    m_pHttpParserCli->data = this;
    int ret = http_parser_execute(m_pHttpParserCli, m_pHttpParserSettings, buf, len);

    ARG_USED(ret);

    DBG(
        __fline;
        printf("http_parser_execute ret : %d\n", ret);
    );
}

void CCuOverHttpKeepConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CCuOverHttpKeepConnector::OnCommand, len:%d\n", ulCmdDataLen); );
}

int CCuOverHttpKeepConnector::OnHeaderField(char *body)
{
    DBG(
        __fline;
        printf("!!!!!!!!OnHeader : \n%s\n", body);
    );

    if (strstr(body, "Match-request"))
    {
        m_bHaveMatchRequest = true;
    }
    else
    {
        m_bHaveMatchRequest = false;
    }

    return 0;
}
int CCuOverHttpKeepConnector::OnHeaderValue(char *body)
{
    DBG(
        __fline;
        printf("!!!!!!!!OnHeader : \n%s\n", body);
    );

    if (m_bHaveMatchRequest)
    {
        m_strMatchRequest = body;
        DBG(
            __fline;
            std::cout << "Request:" << m_strMatchRequest << std::endl;
        );
        m_bHaveMatchRequest = false;
    }

    return 0;
}
int CCuOverHttpKeepConnector::OnBody(char *body)
{
    DBG(
        __fline;
        printf("!!!!!!!!OnBody : \n%s\n", body);
    );

    CSmsStack __CezlibsWebServiceIF;

    __CezlibsWebServiceIF.Parse(body);

    time(&m_tOnMsg);

    ez_printf_debug("req:%s\n", m_strMatchRequest.c_str());

    if (m_strMatchRequest=="/Sms/requestHeartbeat")
    {
        m_enCurMsgStatus = enAppLogicStatus_Heartbeated;
    }
    else if (m_strMatchRequest=="/Sms/requestAuth")
    {
        CSmsMsg_requestAuthACK MsgAuthACK;

        if (__CezlibsWebServiceIF.Decode(MsgAuthACK))
        {
            DBG(
                MsgAuthACK.dump();
            );

            m_sessionId = MsgAuthACK.m_strSessionID;

            m_enCurMsgStatus = enAppLogicStatus_Logined;

        }
        else
        {
            ez_printf_error("__CezlibsWebServiceIF.Decode failed %s\n", body);
        }
    }
    else if (m_strMatchRequest=="/Sms/bcastMsg")
    {
        ez_printf_info("MatchRequest:%s\n\n", m_strMatchRequest.c_str());
        ez_printf_info("OnBody:\n%s\n", body);
    }
    else
    {
        ez_printf_error("unknow req:%s\n", m_strMatchRequest.c_str());
    }

    return 0;
}
std::string CCuOverHttpKeepConnector::getMySessionId()
{
    //__fline;
    //cout << "getMySessionId:" << m_sessionId << endl;

    return m_sessionId;
}

void CCuOverHttpKeepConnector::update()
{
    time_t ttNow = time(NULL);
    int iRet = -1;

    if(m_b_connected==false)
    {
        return;
    }
    // 连接之后登录
    if (IsConnected()
        && m_enCurMsgStatus<enAppLogicStatus_Logining)
    {
        iRet = ICP_requestAuth();

        DBG(
            __fline;
            printf("Login ....\n");
        );
        return;
    }

    // 登录超时
    if (m_enCurMsgStatus==enAppLogicStatus_Logining && (ttNow-m_tOnMsg>m_ttMsgTimeOut))
    {
        m_enCurMsgStatus=enAppLogicStatus_Begin;

        __fline;
        printf("Login timeout.\n");

        ez_printf_error("Login timeout, CountDown:%d\n"
                        , LOGIN_COUNT_DOWN-(m_iSentPacket/* - m_iRecvPacket*/)
                       );

        if (m_iSentPacket >= (/*m_iRecvPacket+*/LOGIN_COUNT_DOWN) )
        {
            // 多于10个包没有反馈也重启， 防止连接建立，但是没有反馈的情况
            // 在网络型信号机中发现此情况
            SetCloseAndDelete();
        }

        return;
    }

    // 命令超时
    if (m_enCurMsgStatus==enAppLogicStatus_Heartbeating && (ttNow-m_tOnMsg>m_ttMsgTimeOut))
    {
        m_enCurMsgStatus=enAppLogicStatus_Logined;

        __fline;
        printf("Heartbeat timeout.\n");

        return;
    }

    if (ttNow-m_tOnMsg > m_ttConnectTimeOut)
    {
        __fline;
        printf("m_tOnMsg timeout.\n");

        m_enCurMsgStatus=enAppLogicStatus_Begin;

        // 遇到单片机之类的tcpserver;
        // 不返回断开包时的异常
        SetCloseAndDelete();
        // 需要在CAgentCuOverHttp::Run()中重连配合

        return;
    }

    DBG(
        __fline;
        printf("m_enCurMsgStatus:%d ttNow-m_tOnMsg:%ld, m_ttMsgTimeOut:%ld\n", m_enCurMsgStatus, ttNow-m_tOnMsg, m_ttMsgTimeOut);
    );

    // 心跳
    if (m_enCurMsgStatus>=enAppLogicStatus_Logined
        && m_enCurMsgStatus != enAppLogicStatus_Heartbeating /*正在等心跳返回消息*/
        &&(ttNow-m_tOnMsg)>(m_ttConnectTimeOut/2.0-0.5))
    {
        DBG(
            __fline;
            printf("KeepLive , ICP_requestHeartbeat\n");
        );

        ICP_requestHeartbeat();
    }
}

//        "POST %s HTTP/1.1\r\n"
const char * const http_req_head =
    "Accept: */*\r\n"
    "User-Agent: Mozilla/5.0 (ezutil/1.5 libezutil 1.5)\r\n"
    "Host: sms.ezlibs.com\r\n"
    "Connection: Keep-Alive\r\n"
    "Content-type: application/json\r\n"
    ;

//       "Content-length: %d\r\n\r\n"
//   "%s";

// http + json
int CCuOverHttpKeepConnector::SendSmsMsg(const std::string &strUri, const std::string &strMsg, const std::string &matchRequest)
{
    char slask[40]; // temporary
    //std::string strUri = "/Sms/requestAuth";

    std::string strHead = "POST " + strUri+ " HTTP/1.1\r\n";

    strHead += http_req_head;

    if (matchRequest.size()>0)
    {
        strHead += "Match-request: ";
        strHead += matchRequest.c_str();
        strHead += "\r\n";
    }

    snprintf(slask, sizeof(slask)-1, "Content-length: %d\r\n\r\n", strMsg.size());
    strHead += slask;

    Send(strHead);
    Send(strMsg);

    time(&m_tOnMsg);
           
    ez_printf_debug("SendSmsMsg:%d.\n", strMsg.size());

    return 0;
}

bool CCuOverHttpKeepConnector::IsLogined()
{
    return m_enCurMsgStatus>=enAppLogicStatus_Logined;

}
int CCuOverHttpKeepConnector::ICP_requestAuth()
{
    CSmsStack __smsStack;
    CSmsMsg_requestAuth Msg;
    const char *pUri = "/Sms/requestAuth";


    Msg.m_strAccount  	    = "admin";
    Msg.m_strPassword 		= "passwd";
    Msg.m_strSessionID	    = "null";

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

    __smsStack.Encode(Msg);

    //strBody = __smsStack.ToString();
    // (const std::string &strUri, const std::string &strMsg, const std::string &matchRequest)
    SendSmsMsg(pUri, __smsStack.ToString(), "");

    m_enCurMsgStatus = enAppLogicStatus_Logining;
    m_iSentPacket++;

    return 0;
}

int CCuOverHttpKeepConnector::ICP_requestHeartbeat()
{
    CSmsStack __smsStack;
    CSmsMsg_requestAuth Msg;

    //  唯一不同点
    const char *pUri = "/Sms/requestHeartbeat";


    Msg.m_strAccount  	    = "admin";
    Msg.m_strPassword 		= "passwd";
    Msg.m_strSessionID	    = m_sessionId;

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

    __smsStack.Encode(Msg);

    //strBody = __smsStack.ToString();
    // (const std::string &strUri, const std::string &strMsg, const std::string &matchRequest)
    SendSmsMsg(pUri, __smsStack.ToString(), "");

    m_enCurMsgStatus = enAppLogicStatus_Heartbeating;

    return 0;
}

