/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerSC1ClientOverTcp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerSC1ClientOverTcp.cpp 5884 2015-01-05 14:04:54Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "HandlerSC1ClientOverTcp.h"

#include "SC1ClientOverTcpKeepConnector.h"
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


CHandlerSC1ClientOverTcp::CHandlerSC1ClientOverTcp(CMaintainAgentLog *pLog)
        :SocketHandler(pLog)
{
    ;
}


#define SIZE 5000
void CHandlerSC1ClientOverTcp::tprintf(TcpSocket *p,const char *format, ...)
{
    va_list ap;
    size_t n;
    char tmp[SIZE];

    va_start(ap,format);
#ifdef _WIN32

    n = vsprintf(tmp,format,ap);
#else

    n = vsnprintf(tmp,SIZE - 1,format,ap);
#endif

    va_end(ap);

    p -> SendBuf(tmp, strlen(tmp));
}


void CHandlerSC1ClientOverTcp::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerSC1ClientOverTcp Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);

        if (p->GetRemotePort()<=0)
        {
            continue;
        }

        tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
        tprintf(sendto, "\n");
        tprintf(sendto, "\tUptime:  %d days %02d:%02d:%02d\n",
                p -> Uptime() / 86400,
                (p -> Uptime() / 3600) % 24,
                (p -> Uptime() / 60) % 60,
                p -> Uptime() % 60);
        if (p3)
        {
            tprintf(sendto, "\tBytes Read: %9lu\n",p -> GetBytesReceived());
            tprintf(sendto, "\tBytes Sent: %9lu\n",p -> GetBytesSent());
        }
    }
    tprintf(sendto, "\n");
}

void CHandlerSC1ClientOverTcp::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerSC1ClientOverTcp Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CSC1ClientOverTcpKeepConnector *p4 = dynamic_cast<CSC1ClientOverTcpKeepConnector *>(p);

        if (p->GetRemotePort()<=0)
        {
            continue;
        }

        snprintf(sendto, sizeof (sendto)-1, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\n");
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tUptime:  %d days %02d:%02d:%02d\n",
                 (int)(p -> Uptime() / 86400),
                 (int)((p -> Uptime() / 3600) % 24),
                 (int)((p -> Uptime() / 60) % 60),
                 (int)(p -> Uptime() % 60));
        strDump += sendto;
        if (p3)
        {
            snprintf(sendto, sizeof (sendto)-1, "\tBytes Read: %9d\n", int(p -> GetBytesReceived()));
            strDump += sendto;
            snprintf(sendto, sizeof (sendto)-1, "\tBytes Sent: %9d\n", int(p -> GetBytesSent()));
            strDump += sendto;
        }

        if (p4)
        {
            snprintf(sendto, sizeof (sendto)-1, "\tRecvPacketNum: %9d\n", p4 -> getRecvPacketNum());
            strDump += sendto;
            snprintf(sendto, sizeof (sendto)-1, "\tSentPacketNum: %9d\n", p4 -> getSentPacketNum());
            strDump += sendto;
        }
    }
    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}


void CHandlerSC1ClientOverTcp::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CSC1ClientOverTcpKeepConnector *p3 = dynamic_cast<CSC1ClientOverTcpKeepConnector *>(p);

        if (p3
            && p3->IsConnected())
        {
            p3->update();
        }
        else
        {}

    }
}

Socket * CHandlerSC1ClientOverTcp::GetSocketByName(std::string strServerName)
{
    //int ii=0;
    //printf("Handler GetSocketByName\n");
    //printf("------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        //Socket *p3 = dynamic_cast<Socket *>(p);

        if (p->GetSockName()==strServerName
            && p->IsConnected())
        {
            return p;
        }
        else
        {
            __fline;
            printf("p->GetSockName():%s\n", p->GetSockName().c_str());
        }

        //tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        //tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
    }

    //DEB(
    printf("CHandlerSC1ClientOverTcp::GetSocketByName(%s) failed.\n", strServerName.c_str());
    //);

    return NULL;
}


CSC1ClientOverTcpKeepConnector * CHandlerSC1ClientOverTcp::GetSocketBySessionID(std::string strSessionID)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CSC1ClientOverTcpKeepConnector *p3 = dynamic_cast<CSC1ClientOverTcpKeepConnector *>(p);

        if (p3 && p3->getMySessionId()==strSessionID && p3->IsConnected())
        {
            return p3;
        }
        else
        {}
    }

    DEB(printf("CHandlerSC1ClientOverTcp::GetSocketBySessionID(%s) failed.\n", strSessionID.c_str()););

    return NULL;
}

// 鉴权
int CHandlerSC1ClientOverTcp::AuthSession(std::string strSession, int iAccessId)
{

    return -1;
}
////操作命令,协议版本0x01
//#define CMD_SIGNALLER_CONNECT		0x01
//#define CMD_SIGNALLER_STATUS		0x02
//#define CMD_GET_GPS_COORDINATE		0x03
//#define CMD_SET_TIME				0x04
//#define CMD_RESTART					0x05
//#define CMD_CENTER_CONTROL			0x06
//#define CMD_SEND_FILE				0x07
//#define CMD_GET_FILE				0x08
//#define CMD_SET_GPS_SWITCH			0x09
//// 取信号机id
//#define CMD_GET_SGINALLERID			0x20
int CHandlerSC1ClientOverTcp::SendCmd(const int cmdNo)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CSC1ClientOverTcpKeepConnector *p4 = dynamic_cast<CSC1ClientOverTcpKeepConnector *>(p);

        if (p4 && p4->IsConnected())
        {
            if (cmdNo==5)
            {
                p4->ICP_requestRestart();
                return 0;
            }
            if (cmdNo==6)
            {
                p4->ICP_requestCenterForce();
                return 0;
            }
            if (cmdNo==32)
            {
                p4->ICP_requestID();
                return 0;
            }
            else
            {
                DEB(printf("CHandlerSC1ClientOverTcp::cmdNo(%d) unknown.\n", cmdNo););
            }
        }
        else
        {}
    }


    return -1;
}
