/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * NetSerialTcpSocketServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: NetSerialTcpSocketServer.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string.h>
#include <ez_log.h>

#include "NetSerialTcpSocketConnector.h"

CNetSerialTcpSocketConnector::CNetSerialTcpSocketConnector(ISocketHandler& h
	, std::string strSocketName
	, time_t Timeout
        , bool bReconn)
        :CNetSerialTcpSocket(h, strSocketName)
        , m_strSocketName(strSocketName)
        ,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();
    
    SetSockName(strSocketName);
}


CNetSerialTcpSocketConnector::~CNetSerialTcpSocketConnector()
{}

CNetSerialTcpSocketConnector *CNetSerialTcpSocketConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CNetSerialTcpSocketConnector *p = new CNetSerialTcpSocketConnector(Handler(), m_strSocketName, m_ttTimeout, m_bReconn);
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);
    return p;
}


void CNetSerialTcpSocketConnector::OnConnectFailed()
{
    CNetSerialTcpSocketConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CNetSerialTcpSocketConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CNetSerialTcpSocketConnector::SetCloseAndDelete()
{
    CNetSerialTcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CNetSerialTcpSocketConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    ez_printf_debug("CNetSerialTcpSocketConnector::OnConnect()\n");
}
void CNetSerialTcpSocketConnector::OnReconnect()
{
    ez_printf_debug("CNetSerialTcpSocketConnector::OnReconnect()\n");
}

void CNetSerialTcpSocketConnector::OnDisconnect()
{
    ez_printf_debug("CNetSerialTcpSocketConnector::OnDisconnect()\n");
}

void CNetSerialTcpSocketConnector::OnConnectTimeout()
{
    ez_printf_debug("CNetSerialTcpSocketConnector::OnConnectTimeout()\n");
}

void CNetSerialTcpSocketConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    //DBG_CODE( printf("CNetSerialTcpSocketConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='q')
    {
        SetCloseAndDelete();
        m_b_connected = false;

        //DBG_CODE( printf("CNetSerialTcpSocketConnector::OnCommand q, SetCloseAndDelete\n"); );
    }

    // 测试用， echo
    if (pCmdData && ulCmdDataLen==1 && *pCmdData=='e')
    {
        char strEchoBuf[128];
        sprintf(strEchoBuf, "CNetSerialTcpSocketConnector echo,cur time:%ld\n", time(NULL));
        SendBuf(strEchoBuf, strlen(strEchoBuf)+1 );
    }
    //////////
}

