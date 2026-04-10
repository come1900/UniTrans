/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CMaintainHandler.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CMaintainHandler.cpp 0001 2012-04-06 09:58:05Z WuJunjie $
 *
 *  Explain:
 *     -·´Ó¦¶Ñ-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "GeneralAgentHandler.h"

#ifdef EXAMPLE_SOCKET_STATUS_AGENT
#include "SocketStatusAgent.h"
#endif
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#include "GeneralAgentTcpSocketServer.h"
#endif
#ifdef EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#include "GeneralAgentTcpSocketConnector.h"
#endif
#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER
#include "GeneralAgentUdpSocket.h"
#endif

#ifdef _FUNC_CONSOLE_OVERTCP
#include "GeneralConsoleOverTcp.h"
#endif
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

CMaintainHandler::CMaintainHandler(CMaintainAgentLog *pLog)
        :SocketHandler(pLog)
#ifdef EXAMPLE_SIGNAL_ONDATA
        , m_sigData(1)
#endif //EXAMPLE_SIGNAL_ONDATA
{
    ;
}


#define SIZE 5000
void CMaintainHandler::tprintf(TcpSocket *p,const char *format, ...)
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
    if (n>0)
    {
        p -> SendBuf(tmp, n);
    }
}


void CMaintainHandler::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "CMaintainHandler Socket List\n");
    tprintf(sendto, "------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER

        CGeneralAgentUdpSocket *pCGeneralAgentUdpSocket = dynamic_cast<CGeneralAgentUdpSocket *>(p);
#else

        Socket *pCGeneralAgentUdpSocket = NULL;
#endif

        if (p->GetRemotePort()<=0 || pCGeneralAgentUdpSocket)
        {
            //__trip;
            continue;
        }

        tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        tprintf(sendto, "  %9s  %s %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str(), p -> IsConnected() ? "Conned" : "NOT Conned");
#if 0

        CGeneralAgentTcpSocketServer *pServer = dynamic_cast<CGeneralAgentTcpSocketServer *>(p);
        CGeneralAgentTcpSocketConnector *pConnector = dynamic_cast<CGeneralAgentTcpSocketConnector *>(p);
#ifdef EXAMPLE_SOCKET_STATUS_AGENT

        CSocketStatusAgent *p4 = dynamic_cast<CSocketStatusAgent *>(p);
        if (p4)
        {
            tprintf(sendto, "CSocketStatusAgent");
        }
        else
#endif
            if (pServer)
            {
                tprintf(sendto, "CGeneralAgentTcpSocketServer");
            }
            else if (pConnector)
            {
                tprintf(sendto, "CGeneralAgentTcpSocketConnector");
            }
            else
            {
                tprintf(sendto, "Socket");
            }
#endif
        tprintf(sendto, "\n");
        tprintf(sendto, "\tUptime:  %d days %02d:%02d:%02d\n",
                p -> Uptime() / 86400,
                (p -> Uptime() / 3600) % 24,
                (p -> Uptime() / 60) % 60,
                p -> Uptime() % 60);
        if (p3 || pCGeneralAgentUdpSocket)
        {
            tprintf(sendto, "\tBytes Read: %9lu\n",p -> GetBytesReceived());
            tprintf(sendto, "\tBytes Sent: %9lu\n",p -> GetBytesSent());
        }
    }
    tprintf(sendto, "\n");
}


void CMaintainHandler::List(std::string &strDump)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "CMaintainHandler Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
#ifdef EXAMPLE_GENERALAGENTUDPSOCKETSERVER

        CGeneralAgentUdpSocket *pCGeneralAgentUdpSocket = dynamic_cast<CGeneralAgentUdpSocket *>(p);
#else

        Socket *pCGeneralAgentUdpSocket = NULL;
#endif

        if (p->GetRemotePort()<=0 || pCGeneralAgentUdpSocket)
        {
            //__trip;
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
    }
    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CMaintainHandler::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p0 = (*it).second;
        ARG_USED(p0);

#ifdef EXAMPLE_SOCKET_STATUS_AGENT

        TcpSocket *p = dynamic_cast<CSocketStatusAgent *>(p0);
        if (p)
        {
            List(p);
        }
#endif

#ifdef _FUNC_CONSOLE_OVERTCP
        CMaintainConsoleOverTcp *pCons = dynamic_cast<CMaintainConsoleOverTcp *>(p0);
        if (pCons)
        {
            pCons->Update();
        }
#endif //_FUNC_CONSOLE_OVERTCP
    }
}
#include <iostream>
Socket * CMaintainHandler::GetSocketByName(std::string strServerName)
{
    //int ii=0;
    //printf("Handler GetSocketByName\n");
    //printf("------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        TcpSocket *pTcp = dynamic_cast<TcpSocket *>(p);

        if (p->GetSockName()==strServerName )
        {
            if (!pTcp || (pTcp && p->IsConnected()))
            {
                return p;
            }
            else
            {
                std::cout << "find:" << strServerName << "but not connected"  << std::endl;
            }
        }
        else
        {
            DBG(
                std::cout << "want:" << strServerName << " Me:" << p->GetSockName() << std::endl;
            );
        }

        //tprintf(sendto, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
        //tprintf(sendto, "  %9s  %s", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
    }

    DBG(printf("CMaintainHandler::GetSocketByName(%s) failed.\n", strServerName.c_str()););

    return NULL;
}

int CMaintainHandler::CloseAndDeleteAll()
{
    int SocketNum = 0;

    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        p->SetCloseAndDelete();
        SocketNum++;
    }

    return SocketNum;
}

#ifdef EXAMPLE_SIGNAL_ONDATA
#include "EZLock.h"
bool CMaintainHandler::AddSignal(CEZObject * pObj, SIG_GeneralAgentHandler_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Attach(pObj, pProc) < 0)
    {
        printf("attach error\n");
        return false;
    }

    return true;
}

bool CMaintainHandler::DelSignal(CEZObject * pObj, SIG_GeneralAgentHandler_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Detach(pObj, pProc) < 0)
    {
        printf("attach error\n");
        return false;
    }

    return false;

}

void CMaintainHandler::OnDataCall(Socket *pSocket, const char *buf,size_t len)
{
    if (buf!=NULL && len > 0)
    {
        m_sigData(pSocket, buf, len);
    } // if
}
#endif //EXAMPLE_SIGNAL_ONDATA

