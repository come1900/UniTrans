/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuOverHttpKeepConnector.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: PuOverHttpKeepConnector.cpp 5884 2013-12-18 03:38:04Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-18 03:38:04 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include "../Stack/SmsStack.h"

#include "PuOverHttpKeepConnector.h"

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

CPuOverHttpKeepConnector *g_pPuOverHttpKeepConnector = NULL;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define BUF_LEN 1000
#define DCB_CALLBACK(x) int http_##x##_callback_PuOverHttpKeepConnector(http_parser *parser, const char *at, size_t length)
#define CB_CALLBACK(x)  int http_##x##_callback_PuOverHttpKeepConnector(http_parser *parser)
#define TOKEN "---------------------------------- "

DCB_CALLBACK(header_field)
{
#define CODES \
    char str_buf[BUF_LEN];\
    memcpy(str_buf, at, length);\
    str_buf[length] = '\0';

    CODES
    DEB(
        printf("%s : ", str_buf);
    );
    if (g_pPuOverHttpKeepConnector)
    {
        g_pPuOverHttpKeepConnector->OnHeaderField(str_buf);
    }
    return 0;
}

DCB_CALLBACK(header_value)
{
    CODES
    DEB(
        __fline;
        printf("%s : ", str_buf);
    );
    if (g_pPuOverHttpKeepConnector)
    {
        g_pPuOverHttpKeepConnector->OnHeaderValue(str_buf);
    }
    return 0;
}

DCB_CALLBACK(body)
{
    CODES
    DEB(
        printf("%s : ", str_buf);
    );

    if (g_pPuOverHttpKeepConnector)
    {
        g_pPuOverHttpKeepConnector->OnBody(str_buf);
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


//////////////////////////////////////////////////
CPuOverHttpKeepConnector::CPuOverHttpKeepConnector(ISocketHandler& h, std::string strSocketName)
        :TcpSocket(h)
        //,CEZThread(CPuOverHttpKeepConnector_THREAD_ATTR)
        ,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();

    SetSockName(strSocketName);

    DisableInputBuffer();

    m_tOnData = 0;

    m_strUName = "";
    m_strUPwd = "";
    m_ttConnectTimeOut = 5;

    m_enOdipStatus = enAppLogicStatus_Begin;

    m_tOnOdip = time(NULL);
    m_ttOdipTimeOut = 2;


    m_pHttpParserCli = (http_parser*)malloc(sizeof(http_parser));
    m_pHttpParserSettings = (http_parser_settings*) malloc(sizeof(http_parser_settings));
    assert(m_pHttpParserCli);
    assert(m_pHttpParserSettings);

    // 仅解析反馈的数据
    http_parser_init(m_pHttpParserCli, HTTP_RESPONSE);

#define NCB(n) http_##n##_callback_PuOverHttpKeepConnector
    #define SCB(x) m_pHttpParserSettings->on_##x = NCB(x)

    SCB(url);
    SCB(header_field);
    SCB(header_value);
    SCB(body);
    SCB(message_begin);
    SCB(headers_complete);
    SCB(message_complete);

    g_pPuOverHttpKeepConnector = this;
}


CPuOverHttpKeepConnector::~CPuOverHttpKeepConnector()
{}

CPuOverHttpKeepConnector *CPuOverHttpKeepConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CPuOverHttpKeepConnector *p = new CPuOverHttpKeepConnector(Handler());
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);

    return p;
}


void CPuOverHttpKeepConnector::OnConnectFailed()
{
    CPuOverHttpKeepConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CPuOverHttpKeepConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CPuOverHttpKeepConnector::SetCloseAndDelete()
{
    TcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CPuOverHttpKeepConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    m_enOdipStatus = enAppLogicStatus_Begin;


    printf("CPuOverHttpKeepConnector::OnConnect() - %ld\n", time(NULL));
}

void CPuOverHttpKeepConnector::OnReconnect()
{
    printf("CPuOverHttpKeepConnector::OnReconnect()\n");
}

void CPuOverHttpKeepConnector::OnDisconnect()
{
    printf("CPuOverHttpKeepConnector::OnDisconnect() - %ld\n", time(NULL));
}

void CPuOverHttpKeepConnector::OnConnectTimeout()
{
    printf("CPuOverHttpKeepConnector::OnConnectTimeout()\n");
}

void CPuOverHttpKeepConnector::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);

    DBG(
        __fline;
        printf("CGeneralAgentTcpSocket::OnRawData:%d\n", len);

        std::string strData((char *)buf, len);
        printf("------------------------------------------\n");
        printf("%s", strData.c_str());
        printf("\n------------------------------------------\n");
    );

    int ret = http_parser_execute(m_pHttpParserCli, m_pHttpParserSettings, buf, len);

    ARG_USED(ret);

    DBG(
        __fline;
        printf("http_parser_execute ret : %d\n", ret);
    );
    //////////
    //有数据就调用， 另一种方式
    //OnCommand((char *)buf, (unsigned int)len);
    ///////////
}

void CPuOverHttpKeepConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CPuOverHttpKeepConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己

    //////////
}

//void CPuOverHttpKeepConnector::ThreadProc()
//{}

void CPuOverHttpKeepConnector::update()
{
    time_t ttNow = time(NULL);
    int iRet = -1;

    if(m_b_connected==false)
    {
        return;
    }
    // 连接之后登录
    if (IsConnected()
        && m_enOdipStatus<enAppLogicStatus_Logining)
    {
        iRet = ICP_requestAuth();

        DBG(
            __fline;
            printf("Login ....\n");
        );
        return;
    }

    // 登录超时
    if (m_enOdipStatus==enAppLogicStatus_Logining && (ttNow-m_tOnOdip>m_ttOdipTimeOut))
    {
        m_enOdipStatus=enAppLogicStatus_Begin;

        __fline;
        printf("Login timeout.\n");

        return;
    }
	
    // 命令超时
    if (m_enOdipStatus==enAppLogicStatus_Heartbeating && (ttNow-m_tOnOdip>m_ttOdipTimeOut))
    {
        m_enOdipStatus=enAppLogicStatus_Logined;

        __fline;
        printf("Heartbeat timeout.\n");

        return;
    }

    if (ttNow-m_tOnOdip > m_ttConnectTimeOut)
    {
        m_enOdipStatus=enAppLogicStatus_Begin;

        __fline;
        printf("m_tOnOdip timeout.\n");

        return;
    }

    DBG(
        __fline;
        printf("m_enOdipStatus:%d ttNow-m_tOnOdip:%ld, m_ttOdipTimeOut:%ld\n", m_enOdipStatus, ttNow-m_tOnOdip, m_ttOdipTimeOut);
    );

    // 心跳
    if (m_enOdipStatus>=enAppLogicStatus_Logined
		&& m_enOdipStatus != enAppLogicStatus_Heartbeating /*正在等心跳返回消息*/
        &&(ttNow-m_tOnOdip)>(m_ttConnectTimeOut/2.0-0.5))
    {
        //DBG(
            __fline;
            printf("KeepLive , ICP_requestHeartbeat\n");
        //);

        ICP_requestHeartbeat();
    }
}


int CPuOverHttpKeepConnector::OnBody(char *body)
{
    DBG(
        __fline;
        printf("!!!!!!!!OnBody : \n%s\n", body);
    );

    CSmsStack __CezlibsWebServiceIF;

    __CezlibsWebServiceIF.Parse(body);

    time(&m_tOnOdip);

    CSmsMsg_requestAuthACK MsgAuthACK;

    if (__CezlibsWebServiceIF.Decode(MsgAuthACK))
    {
        DBG(
            MsgAuthACK.dump();
        );
        m_sessionId = MsgAuthACK.m_strSessionID;

        m_enOdipStatus = enAppLogicStatus_Logined;

    }

    return 0;
}

    int CPuOverHttpKeepConnector::OnHeaderField(char *str)
    	{

		if (strstr(str, "Match-request"))
			{m_bHaveMatchRequest = true;
			}
    	
    return 0;
}
    int CPuOverHttpKeepConnector::OnHeaderValue(char *str)    	{

		if (m_bHaveMatchRequest)
			{
			m_strMatchRequest = str;
			__fline;
			std::cout << "m_strMatchRequest" << m_strMatchRequest << std::endl;
			m_bHaveMatchRequest = false;
			}
		
    	
    return 0;
}


std::string CPuOverHttpKeepConnector::getMySessionId()
{
    return m_sessionId;
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
int CPuOverHttpKeepConnector::SendSmsMsg(const std::string &strUri, const std::string &strMsg, const std::string &matchRequest)
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

    //strHead += strMsg;

    Send(strHead);
    Send(strMsg);

    return 0;
}

bool CPuOverHttpKeepConnector::IsLogined()
{
    return m_enOdipStatus>=enAppLogicStatus_Logined;

}
int CPuOverHttpKeepConnector::ICP_requestAuth()
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

    m_enOdipStatus = enAppLogicStatus_Logining;

    return 0;
}

int CPuOverHttpKeepConnector::ICP_requestHeartbeat()
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

    m_enOdipStatus = enAppLogicStatus_Heartbeating;

    return 0;
}

