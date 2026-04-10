/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveStreammingServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveStreammingServer.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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
#include "../../Logs.h"

#include "../Pandora/PandoraDef.h"
#include "../Pandora/StreamContainer.h"

//#include "../../Solar.h"
#include "../../Configs/ConfigGeneral.h"

#include "HandlerLiveStreammingServer.h"

#include "HttpServerLiveStreammingServer.h"

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

#define tprintfBuffSIZE 5000

#define def_dbFileKeyValue "keyValue_liveStreamming.s3"
#define def_tprintf_Buff_SIZE 4096

CHandlerLiveStreammingServer::CHandlerLiveStreammingServer(CMaintainAgentLog *pLog)
        :CHandlerGRestHttpdSocket(pLog)
        //:SocketHandler(pLog)
{
    //m_pCacheWithDb = NULL;
    //m_pCacheKeyValue = NULL;

    m_pPssCfg = NULL;
    m_pPullCfg = NULL;
}


CHandlerLiveStreammingServer::~CHandlerLiveStreammingServer()
{
    if (m_pPssCfg)
    {
        delete m_pPssCfg;
    }
    if (m_pPullCfg)
    {
        delete m_pPullCfg;
    }
}

void CHandlerLiveStreammingServer::tprintf(TcpSocket *p,const char *format, ...)
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

    //p -> SendBuf(tmp, strlen(tmp));
    p -> SendBuf(tmp, n);
}

void CHandlerLiveStreammingServer::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerLiveStreammingServer Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerLiveStreammingServer *p4 = dynamic_cast<CHttpServerLiveStreammingServer *>(p);

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
            //tprintf(sendto, "\tSession: [%s]\n", p4 -> getMySessionId().c_str());
        }
    }
    tprintf(sendto, "\n");
}



void CHandlerLiveStreammingServer::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerLiveStreammingServer Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    int iNumOfCHttpServerLiveStreammingServer = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerLiveStreammingServer *p4 = dynamic_cast<CHttpServerLiveStreammingServer *>(p);
        if (p4)
        {
            iNumOfCHttpServerLiveStreammingServer++;
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

                //snprintf(sendto, sizeof (sendto)-1, "\tSession: [%s]\n", p4 -> getMySessionId().c_str());
                //strDump += sendto;
            }

        }
        else
        {}
    }

    if (iLevel==enLogLevelShow_contes)
    {
        snprintf(sendto, sizeof (sendto)-1, "        -------------------------------------------\n");
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerLiveStreammingServer: [%d]\n", iNumOfCHttpServerLiveStreammingServer);
        strDump += sendto;
    }

    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerLiveStreammingServer::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerLiveStreammingServer *pLiveStreammingServer = dynamic_cast<CHttpServerLiveStreammingServer *>(p);

        if (pLiveStreammingServer)
        {
            pLiveStreammingServer->Update();
        }
    }
}

void CHandlerLiveStreammingServer::Start()
{
    if (NULL == m_pCacheWithDb)
    {
        m_pCacheWithDb = new CCacheKeyValue;
        assert(m_pCacheWithDb);

        //std::string strDbFileName = g_Solar.GetConfigFilePath();
        //strDbFileName+="/";
        //strDbFileName+=def_dbFileKeyValue;

        CConfigGeneral __cfgGeneral;
        __cfgGeneral.update();

        m_pCacheWithDb->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_dbFileKeyValue);

        // 初始化参数
        std::string __strKey = "701819701";
        std::string __Values ;
        int iretGet = this->getAppKey(__strKey, __Values);

        //if (m_pCacheWithDb->numPair()<1)
        if (iretGet < 0)
        {
            this->setAppKey("701818788", "c48faffaedf5d4cae01c6a21c11476fc118e32b0");
            this->setAppKey("701918788", "9cba8e3fe970ddbca35424db80197a0167d68813");
            this->setAppKey("701819701", "ebd69a6ccd6f81ef4e6c52e37761750b42f0fe50");
        }

        __strKey = "admin";
        //std::string __Values ;
        iretGet = this->getAccount(__strKey, __Values);

        if (iretGet < 0)
        {
            this->setAccount("admin", "d4e8e6deaa7b1f8381e09e3e6b83e36f0b681c5c");
        }
    }

    if (NULL == m_pCacheKeyValue)
    {
        m_pCacheKeyValue = new CCacheKeyValue;
        assert(m_pCacheKeyValue);
    }

    if (NULL == m_pPssCfg)
    {
        m_pPssCfg = new CConfigLiveStreammingServer;
        assert(m_pPssCfg);

        m_pPssCfg->update();
    }

    if (NULL == m_pPullCfg)
    {
        m_pPullCfg = new CConfigPullTykdMobileService;
        assert(m_pPullCfg);

        m_pPullCfg->update();
    }

    return ;
}

void CHandlerLiveStreammingServer::Disconnect()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p0 = (*it).second;
        TcpSocket *p = dynamic_cast<TcpSocket *>(p0);
        if (p && p -> Uptime() > 60 )
        {
            //tprintf(p, "Goodbye\n");
            p -> SetCloseAndDelete();
        }
    }
}

