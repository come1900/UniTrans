/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHttpSocketConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHttpSocketConnector.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include "GeneralAgentHttpSocketConnector.h"

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

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define BUF_LEN 1000
#define DCB_CALLBACK(x) int http_##x##_callback(http_parser *parser, const char *at, size_t length)

DCB_CALLBACK(header_field)
{
#define CODES \
    char str_buf[BUF_LEN];\
    memcpy(str_buf, at, length);\
    str_buf[length] = '\0';

    CODES
    printf("%s : ", str_buf);
    return 0;
}

DCB_CALLBACK(header_value)
{
    CODES
    printf("%s\n", str_buf);
    return 0;
}

DCB_CALLBACK(body)
{
    CODES
    printf("body : \n%s\n", str_buf);
    return 0;
}

DCB_CALLBACK(url)
{
    CODES
    printf("url : %s\n", str_buf);
    return 0;
}

#define CB_CALLBACK(x) int http_##x##_callback(http_parser *parser)
#define TOKEN "---------------------------------- "
CB_CALLBACK(message_begin)
{
    printf(TOKEN"message_begin""\n");
    return 0;
}

CB_CALLBACK(message_complete)
{
    printf(TOKEN"message_complete""\n");

    // 结束了， 初始化，等待下面的数据
    http_parser_init(parser, HTTP_RESPONSE);

    return 0;
}

CB_CALLBACK(headers_complete)
{
    printf(TOKEN"headers_complete""\n");
    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CGeneralAgentHttpSocketConnector::CGeneralAgentHttpSocketConnector(ISocketHandler& h
        , std::string strSocketName
        , time_t Timeout
        , bool bReconn)
        :CGeneralAgentTcpSocket(h, strSocketName)
        ,m_b_connected(false)
        , m_strSocketName(strSocketName)
{
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();

    SetSockName(strSocketName);

    m_pHttpParserCli = (http_parser*)malloc(sizeof(http_parser));
    m_pHttpParserSettings = (http_parser_settings*) malloc(sizeof(http_parser_settings));
    assert(m_pHttpParserCli);
    assert(m_pHttpParserSettings);

    // 仅解析反馈的数据
    http_parser_init(m_pHttpParserCli, HTTP_RESPONSE);

#define NCB(n) http_##n##_callback
    #define SCB(x) m_pHttpParserSettings->on_##x = NCB(x)

    SCB(url);
    SCB(header_field);
    SCB(header_value);
    SCB(body);
    SCB(message_begin);
    SCB(headers_complete);
    SCB(message_complete);
}


CGeneralAgentHttpSocketConnector::~CGeneralAgentHttpSocketConnector()
{}

CGeneralAgentHttpSocketConnector *CGeneralAgentHttpSocketConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CGeneralAgentHttpSocketConnector *p = new CGeneralAgentHttpSocketConnector(Handler(), m_strSocketName, m_ttTimeout, m_bReconn);
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);
    return p;
}


void CGeneralAgentHttpSocketConnector::OnConnectFailed()
{
    CGeneralAgentHttpSocketConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CGeneralAgentHttpSocketConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CGeneralAgentHttpSocketConnector::SetCloseAndDelete()
{
    CGeneralAgentTcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CGeneralAgentHttpSocketConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    printf("CGeneralAgentHttpSocketConnector::OnConnect()\n");
}
void CGeneralAgentHttpSocketConnector::OnReconnect()
{
    printf("CGeneralAgentHttpSocketConnector::OnReconnect()\n");
}

void CGeneralAgentHttpSocketConnector::OnDisconnect()
{
    printf("CGeneralAgentHttpSocketConnector::OnDisconnect()\n");
}

void CGeneralAgentHttpSocketConnector::OnConnectTimeout()
{
    printf("CGeneralAgentHttpSocketConnector::OnConnectTimeout()\n");
}


void CGeneralAgentHttpSocketConnector::OnRawData(const char *buf,size_t len)
{
    //m_tOnData = time(NULL);

    //DBG(
    __fline;
    printf("CGeneralAgentTcpSocket::OnRawData:%d\n", len);

    std::string strData((char *)buf, len);
    printf("------------------------------------------\n");
    printf("%s", strData.c_str());
    printf("\n------------------------------------------\n");
    //);

    int ret = http_parser_execute(m_pHttpParserCli, m_pHttpParserSettings, buf, len);
    printf("\nhttp_parser_execute ret : %d\n", ret);
    //////////
    //有数据就调用， 另一种方式
    //OnCommand((char *)buf, (unsigned int)len);
    ///////////
}

void CGeneralAgentHttpSocketConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CGeneralAgentHttpSocketConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();
        m_b_connected = false;

        DBG( printf("CGeneralAgentHttpSocketConnector::OnCommand q, SetCloseAndDelete\n"); );
    }

    // 测试用， echo
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='e')
    {
        char strEchoBuf[128];
        sprintf(strEchoBuf, "CGeneralAgentHttpSocketConnector echo,cur time:%ld\n", time(NULL));
        SendBuf(strEchoBuf, strlen(strEchoBuf)+1 );
    }
    //////////
}

#ifdef EXAMPLE_DATAPARSE_EXAMPLE
// 通用头 pCmd
// 数据 pData， 长度见 pCmd->u8DataLen
void CGeneralAgentHttpSocketConnector::OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData)
{
    CGeneralAgentTcpSocket::OnCommand_WifiAgent(pCmd, pData);
    printf("CGeneralAgentHttpSocketConnector::OnCommand_WifiAgent\n");
}
#endif

