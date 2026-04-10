/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerMinerMtService.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: HandlerMinerMtService.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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

#include "../../Solar.h"
#include "../../Configs/ConfigGeneral.h"

#include "HandlerMinerMtService.h"
#include "HttpServerMinerMtService.h"

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

#define tprintfBuffSIZE 5000
#define def_MinerMtServiceKeyValue "keyValue_MinerMtService.s3"

#define ARG_USED(x) (void)&x;

CHandlerMinerMtService::CHandlerMinerMtService(CMaintainAgentLog *pLog)
        :SocketHandlerEp(pLog)
{
    m_pCacheWithDb = NULL;
    m_pCacheKeyValue = NULL;

}

CHandlerMinerMtService::~CHandlerMinerMtService()
{
    if (m_pCacheWithDb)
    {
        delete m_pCacheWithDb;
    }

    if (m_pCacheKeyValue)
    {
        delete m_pCacheKeyValue;
    }
}


#define SIZE 5000
void CHandlerMinerMtService::tprintf(TcpSocket *p,const char *format, ...)
{
    va_list ap;
    size_t n;
    char tmp[tprintfBuffSIZE];

    va_start(ap,format);
#ifdef _WIN32

    n = vsprintf(tmp,format,ap);
#else

    n = vsnprintf(tmp,tprintfBuffSIZE - 1,format,ap);
#endif

    va_end(ap);

    //p -> SendBuf(tmp, strlen(tmp));
    p -> SendBuf(tmp, n);
}

void CHandlerMinerMtService::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerMinerMtService Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        //CHttpServerMinerMtService *p4 = dynamic_cast<CHttpServerMinerMtService *>(p);

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



void CHandlerMinerMtService::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerMinerMtService Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    int iNumOfCHttpServerMinerMtService = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerMinerMtService *p4 = dynamic_cast<CHttpServerMinerMtService *>(p);
        if (p4)
        {
            iNumOfCHttpServerMinerMtService++;
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
//            if (p4)
//            {
//                snprintf(sendto, sizeof (sendto)-1, "\tMsg Received: %9d\n", int(p4 -> getMsgNumReceived()));
//                strDump += sendto;
//                snprintf(sendto, sizeof (sendto)-1, "\tBytes Sent: %9d\n", int(p4 -> getMsgNumSent()));
//                strDump += sendto;
//            }

        }
        else
        {}
    }

    if (iLevel==enLogLevelShow_contes)
    {
        snprintf(sendto, sizeof (sendto)-1, "        -------------------------------------------\n");
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerMinerMtService: [%d]\n", iNumOfCHttpServerMinerMtService);
        strDump += sendto;
    }

    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerMinerMtService::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerMinerMtService *pMinerMtService = dynamic_cast<CHttpServerMinerMtService *>(p);

        if (pMinerMtService)
        {
            pMinerMtService->Update();
        }
    }
}

void CHandlerMinerMtService::Start()
{
    if (NULL == m_pCacheWithDb)
    {
        m_pCacheWithDb = new CCacheKeyValue;
        assert(m_pCacheWithDb);

        //std::string strDbFileName = g_Solar.GetConfigFilePath();
        //strDbFileName+="/";
        //strDbFileName+=def_MinerMtServiceKeyValue;

        CConfigGeneral __cfgGeneral;
        __cfgGeneral.update();

        m_pCacheWithDb->setDbfilename((__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_MinerMtServiceKeyValue));
        //m_pCacheWithDb->setDbfilename(def_MinerMtServiceKeyValue);

        // empty
        if (m_pCacheWithDb->numPair()<1)
        {
            m_pCacheWithDb->addPair("701818788", "c48faffaedf5d4cae01c6a21c11476fc118e32b0");
            m_pCacheWithDb->addPair("701918788", "9cba8e3fe970ddbca35424db80197a0167d68813");
            m_pCacheWithDb->addPair("701819701", "ebd69a6ccd6f81ef4e6c52e37761750b42f0fe50");
        }
    }

    if (NULL == m_pCacheKeyValue)
    {
        m_pCacheKeyValue = new CCacheKeyValue;
        assert(m_pCacheKeyValue);
    }

    return ;
}

