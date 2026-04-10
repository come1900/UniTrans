/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerCuOverHttp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerCuOverHttp.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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
#include "HandlerCuOverHttp.h"

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
#define def_tprintf_Buff_SIZE 4096

CHandlerCuOverHttp::CHandlerCuOverHttp(CMaintainAgentLog *pLog)
        :SocketHandler(pLog)
{
    ;
}

void CHandlerCuOverHttp::tprintf(TcpSocket *p,const char *format, ...)
{
    va_list ap;
    size_t n;
    char tmp[def_tprintf_Buff_SIZE];

    va_start(ap,format);
#ifdef _WIN32

    n = vsprintf(tmp,format,ap);
#else

    n = vsnprintf(tmp,def_tprintf_Buff_SIZE - 1,format,ap);
#endif

    va_end(ap);

    p -> SendBuf(tmp, strlen(tmp));
}


void CHandlerCuOverHttp::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerCuOverHttp Socket List\n");
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

void CHandlerCuOverHttp::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerCuOverHttp Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CCuOverHttpKeepConnector *p4 = dynamic_cast<CCuOverHttpKeepConnector *>(p);

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
            snprintf(sendto, sizeof (sendto)-1, "\tSession: [%s]\n", p4 -> getMySessionId().c_str());
            strDump += sendto;
        }
    }
    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}


void CHandlerCuOverHttp::Update()
{    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CCuOverHttpKeepConnector *p3 = dynamic_cast<CCuOverHttpKeepConnector *>(p);

        if (p3
            && p3->IsConnected())
        {
            p3->update();
        }
        else
        {}

    }
}

Socket * CHandlerCuOverHttp::GetSocketByName(std::string strServerName)
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
    printf("CHandlerCuOverHttp::GetSocketByName(%s) failed.\n", strServerName.c_str());
    //);

    return NULL;
}


CCuOverHttpKeepConnector * CHandlerCuOverHttp::GetSocketBySessionID(std::string strSessionID)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CCuOverHttpKeepConnector *p3 = dynamic_cast<CCuOverHttpKeepConnector *>(p);

        if (p3 && p3->getMySessionId()==strSessionID && p3->IsConnected())
        {
            return p3;
        }
        else
        {}
    }

    DEB(printf("CHandlerCuOverHttp::GetSocketBySessionID(%s) failed.\n", strSessionID.c_str()););

    return NULL;
}

// ¼øÈ¨
int CHandlerCuOverHttp::AuthSession(std::string strSession, int iAccessId)
{

    return -1;
}
