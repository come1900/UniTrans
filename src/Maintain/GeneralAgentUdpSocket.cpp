/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentUdpSocket.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentUdpSocket.cpp 5884 2012-07-19 04:21:23Z WuJunjie $
 *
 *  Explain:
 *     -udp实例-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 *     2012-7-19 16:19:02 支持向处理线程发送消息的模型
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include <stdio.h>
#include <Utility.h>

#ifdef EXAMPLE_GENERALSOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif //EXAMPLE_GENERALSOCKETPROCESSOR

#include "GeneralAgentUdpSocket.h"

CGeneralAgentUdpSocket::CGeneralAgentUdpSocket(ISocketHandler& h, std::string strSocketName)
        :UdpSocket(h)
{
	SetSockName(strSocketName);
}

void CGeneralAgentUdpSocket::OnRawData(const char *p,size_t l,struct sockaddr *sa_from,socklen_t sa_len)
{
	//向处理线程发送消息
#ifdef EXAMPLE_GENERALSOCKETPROCESSOR
    g_GeneralSocketProcessor.SendSocketMsg(0, this, p, l, sa_from, sa_len);
#else
#if 1
    char buf[1024];
    struct sockaddr_in sa;
    memcpy(&sa,sa_from,sa_len);
    ipaddr_t a;
    memcpy(&a,&sa.sin_addr,4);
    std::string ip;
    Utility::l2ip(a,ip);

    snprintf(buf, sizeof(buf), "Received %d bytes from: %s:%d\n", l,ip.c_str(),ntohs(sa.sin_port));
    printf(buf);

    printf("%s\n",static_cast<std::string>(p).substr(0,l).c_str());

    sendto(this->GetSocket(), buf, strlen(buf), 0, sa_from, sa_len);
    sendto(this->GetSocket(), static_cast<std::string>(p).substr(0,l).c_str(), static_cast<std::string>(p).substr(0,l).size(), 0, sa_from, sa_len);
#endif

#endif
}

/** Outgoing traffic counter. */
uint64_t CGeneralAgentUdpSocket::GetBytesSent(bool clear )
{
    return m_BytesSent;
}

/** Incoming traffic counter. */
uint64_t CGeneralAgentUdpSocket::GetBytesReceived(bool clear )
{
    return m_BytesReceived;
}
