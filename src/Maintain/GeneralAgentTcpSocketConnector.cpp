/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocketServer.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#include "GeneralAgentTcpSocketConnector.h"

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

CGeneralAgentTcpSocketConnector::CGeneralAgentTcpSocketConnector(ISocketHandler& h
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
}


CGeneralAgentTcpSocketConnector::~CGeneralAgentTcpSocketConnector()
{}

CGeneralAgentTcpSocketConnector *CGeneralAgentTcpSocketConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CGeneralAgentTcpSocketConnector *p = new CGeneralAgentTcpSocketConnector(Handler(), m_strSocketName, m_ttTimeout, m_bReconn);
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);
    return p;
}


void CGeneralAgentTcpSocketConnector::OnConnectFailed()
{
    CGeneralAgentTcpSocketConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CGeneralAgentTcpSocketConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CGeneralAgentTcpSocketConnector::SetCloseAndDelete()
{
    CGeneralAgentTcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CGeneralAgentTcpSocketConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    printf("CGeneralAgentTcpSocketConnector::OnConnect()\n");
}
void CGeneralAgentTcpSocketConnector::OnReconnect()
{
    printf("CGeneralAgentTcpSocketConnector::OnReconnect()\n");
}

void CGeneralAgentTcpSocketConnector::OnDisconnect()
{
    printf("CGeneralAgentTcpSocketConnector::OnDisconnect()\n");
}

void CGeneralAgentTcpSocketConnector::OnConnectTimeout()
{
    printf("CGeneralAgentTcpSocketConnector::OnConnectTimeout()\n");
}

void CGeneralAgentTcpSocketConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CGeneralAgentTcpSocketConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();
        m_b_connected = false;

        DBG( printf("CGeneralAgentTcpSocketConnector::OnCommand q, SetCloseAndDelete\n"); );
    }

    // 测试用， echo
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='e')
    {
        char strEchoBuf[128];
        sprintf(strEchoBuf, "CGeneralAgentTcpSocketConnector echo,cur time:%ld\n", time(NULL));
        SendBuf(strEchoBuf, strlen(strEchoBuf)+1 );
    }
    //////////
}

    #ifdef EXAMPLE_DATAPARSE_EXAMPLE
// 通用头 pCmd
// 数据 pData， 长度见 pCmd->u8DataLen
void CGeneralAgentTcpSocketConnector::OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData)
{
    CGeneralAgentTcpSocket::OnCommand_WifiAgent(pCmd, pData);
    printf("CGeneralAgentTcpSocketConnector::OnCommand_WifiAgent\n");
}
#endif
