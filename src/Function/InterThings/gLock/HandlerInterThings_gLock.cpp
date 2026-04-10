/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerInterThings_gLock.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerInterThings_gLock.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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
#include "HandlerInterThings_gLock.h"

#include "HttpServerInterThings_gLock.h"
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

CHandlerInterThings_gLock::CHandlerInterThings_gLock(CMaintainAgentLog *pLog)
        :SocketHandler(pLog)
{
    ;
}


#define SIZE 5000
void CHandlerInterThings_gLock::tprintf(TcpSocket *p,const char *format, ...)
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

    //p -> SendBuf(tmp, strlen(tmp));
    p -> SendBuf(tmp, n);
}

void CHandlerInterThings_gLock::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerInterThings_gLock Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerInterThings_gLock *p4 = dynamic_cast<CHttpServerInterThings_gLock *>(p);

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
        if (p4)
        {
            tprintf(sendto, "\tSession: [%s]\n", p4 -> getMySessionId().c_str());
        }
    }
    tprintf(sendto, "\n");
}

void CHandlerInterThings_gLock::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerInterThings_gLock Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    int iNumOfCHttpServerInterThings_gLock = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerInterThings_gLock *p4 = dynamic_cast<CHttpServerInterThings_gLock *>(p);
        if (p4)
        {
            iNumOfCHttpServerInterThings_gLock++;
        }

        if (p->GetRemotePort()<=0)
        {
            continue;
        }
        if (iLevel==enLogLevelShow_detail)
        {
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

                snprintf(sendto, sizeof (sendto)-1, "\tMsg Received: %9d\n", int(p4 -> getMsgNumReceived()));
                strDump += sendto;
                snprintf(sendto, sizeof (sendto)-1, "\tBytes Sent: %9d\n", int(p4 -> getMsgNumSent()));
                strDump += sendto;

                snprintf(sendto, sizeof (sendto)-1, "\tSession: [%s]\n", p4 -> getMySessionId().c_str());
                strDump += sendto;
            }

        }
        else
        {}
    }

    if (iLevel==enLogLevelShow_contes)
    {
        snprintf(sendto, sizeof (sendto)-1, "        -------------------------------------------\n");
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerInterThings_gLock: [%d]\n", iNumOfCHttpServerInterThings_gLock);
        strDump += sendto;
    }

    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerInterThings_gLock::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerInterThings_gLock *pInterThings_gLock = dynamic_cast<CHttpServerInterThings_gLock *>(p);
#if 1

        if (pInterThings_gLock)
        {
            pInterThings_gLock->update();
        }
#endif

    }
}

Socket * CHandlerInterThings_gLock::GetSocketBySocketName(std::string strSocketName)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        //CHttpServerInterThings_gLock *pInterThings_gLock = dynamic_cast<CHttpServerInterThings_gLock *>(p);
#if 1

        if (p->GetSockName()==strSocketName
            && p->IsConnected())
        {
            return p;
        }
        else
        {}
#endif

#if 0

        if (pInterThings_gLock
            && pInterThings_gLock->getMySessionId()==strSocketName
            && pInterThings_gLock->IsConnected())
        {
            return pInterThings_gLock;
        }
        else
        {}
#endif

    }

    DEB(printf("CHandlerInterThings_gLock::GetSocketBySocketName(%s) failed.\n", strSocketName.c_str()););

    return NULL;
}


CHttpServerInterThings_gLock * CHandlerInterThings_gLock::GetSocketBySessionID(const std::string & strSessionID)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerInterThings_gLock *p3 = dynamic_cast<CHttpServerInterThings_gLock *>(p);

        if (p3 && p3->getMySessionId()==strSessionID)
        {
            return p3;
        }
        else
        {
            DBG(
                __trip;
                cout << "GetSocketBySessionID failed." << endl;

                if (p3)
            {
                cout << "strSessionID:              [[[" << strSessionID << "]]]" << std::endl;
                cout << "p3->getMySessionId():[[[" << p3->getMySessionId()
                    << "]]]" << std::endl;
                }
                else
                {
                    std::cout << "there has not CHttpServerInterThings_gLock client." << std::endl;
                }
            );
        }
    }

    DEB(printf("CHandlerInterThings_gLock::GetSocketBySessionID(%s) failed.\n", strSessionID.c_str()););

    return NULL;
}

// ¼øÈ¨
int CHandlerInterThings_gLock::AuthSession(std::string strSession, int iAccessId)
{

    return -1;
}
