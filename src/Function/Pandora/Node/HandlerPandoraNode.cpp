/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerPandoraNode.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerPandoraNode.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "../../../Logs.h"

#include "HandlerPandoraNode.h"
#include "PandoraNodeKeepConnector.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

#define TMP_BUFF_SIZE 1024


CHandlerPandoraNode::CHandlerPandoraNode(CMaintainAgentLog *pLog)
        :SocketHandler(pLog)
{
    m_iTryConnTimes = 0;
    m_pCfgPandoraNode = NULL;
}

CHandlerPandoraNode::~CHandlerPandoraNode()
{
    if (m_pCfgPandoraNode != NULL)
    {
        delete m_pCfgPandoraNode;
    }
}

void CHandlerPandoraNode::tprintf(TcpSocket *p,const char *format, ...)
{
    va_list ap;
    size_t n;
    ARG_USED(n);

    char tmp[TMP_BUFF_SIZE];

    va_start(ap,format);
#ifdef _WIN32

    n = vsprintf(tmp,format,ap);
#else

    n = vsnprintf(tmp,TMP_BUFF_SIZE - 1,format,ap);
#endif

    va_end(ap);

    p -> SendBuf(tmp, strlen(tmp));
}


void CHandlerPandoraNode::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerPandoraNode Socket List\n");
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

void CHandlerPandoraNode::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[TMP_BUFF_SIZE];

    snprintf(sendto, sizeof (sendto)-1, "HandlerPandoraNode Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CPandoraNodeKeepConnector *p4 = dynamic_cast<CPandoraNodeKeepConnector *>(p);

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
            snprintf(sendto, sizeof (sendto)-1, "\t%10s: [%d]\n", "Bytes Read", int(p -> GetBytesReceived()));
            //snprintf(sendto, sizeof (sendto)-1, "\t: %9d\n", int(p -> GetBytesReceived()));
            strDump += sendto;
            snprintf(sendto, sizeof (sendto)-1, "\t%10s: [%d]\n", "Bytes Sent", int(p -> GetBytesSent()));
            //snprintf(sendto, sizeof (sendto)-1, "\tBytes Sent: %9d\n", int(p -> GetBytesSent()));
            strDump += sendto;
        }

        if (p4)
        {
            snprintf(sendto, sizeof (sendto)-1, "\t%10s: [%s]\n", "Session", p4 -> getMySessionId().c_str());
            strDump += sendto;
        }
    }
    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerPandoraNode::Start()
{
    if (NULL == m_pCfgPandoraNode)
    {
        m_pCfgPandoraNode = new CConfigPandoraNode;
        assert(m_pCfgPandoraNode);
        m_pCfgPandoraNode->update();

        ez_printf_debug("m_iTryConnTimes:%d\n", m_iTryConnTimes);
    }// if
}

void CHandlerPandoraNode::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CPandoraNodeKeepConnector *p3 = dynamic_cast<CPandoraNodeKeepConnector *>(p);

        if (p3
            && p3->IsConnected())
        {

            //if (p3->IsOnData())
            {
                m_iTryConnTimes = 0;

                //ez_printf_debug("m_iTryConnTimes:%d\n", m_iTryConnTimes);
            }

            p3->Update();
        }
        else
        {
            //ez_printf_debug("No worker\n");
        }

    }

    m_iTryConnTimes++;
    //ez_printf_debug("m_iTryConnTimes:%d\n", m_iTryConnTimes);
}

Socket * CHandlerPandoraNode::GetSocketByName(std::string strServerName)
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
    printf("CHandlerPandoraNode::GetSocketByName(%s) failed.\n", strServerName.c_str());
    //);

    return NULL;
}


CPandoraNodeKeepConnector * CHandlerPandoraNode::GetSocketBySessionID(std::string strSessionID)
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CPandoraNodeKeepConnector *p3 = dynamic_cast<CPandoraNodeKeepConnector *>(p);

        if (p3 && p3->getMySessionId()==strSessionID && p3->IsConnected())
        {
            return p3;
        }
        else
        {}
    }

    DEB(printf("CHandlerPandoraNode::GetSocketBySessionID(%s) failed.\n", strSessionID.c_str()););

    return NULL;
}

CConfigPandoraNode *CHandlerPandoraNode::getCfgPandoraNode()
{
    return m_pCfgPandoraNode;
}

unsigned int CHandlerPandoraNode::getTryConnTimes()
{
    return m_iTryConnTimes;
}

