/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveRecordServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveRecordServer.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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
#include "../../Solar.h"
#include "../../Configs/ConfigGeneral.h"

#include "../Pandora/PandoraDef.h"
#include "../Pandora/StreamContainer.h"

#include "HandlerLiveRecordServer.h"

#include "HttpServerLiveRecordServer.h"

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

#define def_liveRecordKeyValue "keyValue_liveRecord.s3"

CHandlerLiveRecordServer::CHandlerLiveRecordServer(CMaintainAgentLog *pLog)
        :CHandlerGRestHttpdSocket(pLog)
{
    //m_pCacheWithDb = NULL;
    //m_pCacheKeyValue = NULL;
#if defined(_USE_MODULES_LocalDataBase)
    m_dbaLiveRecord = NULL;
#endif // _USE_MODULES_LocalDataBase
    m_pCfgLiveRecordServer = NULL;
}
CHandlerLiveRecordServer::~CHandlerLiveRecordServer()
{
#if 0
    if (m_pCacheWithDb)
    {
        delete m_pCacheWithDb;
    }

    if (m_pCacheKeyValue)
    {
        delete m_pCacheKeyValue;
    }
#endif

#if defined(_USE_MODULES_LocalDataBase)
    if (m_dbaLiveRecord)
    {
        delete m_dbaLiveRecord;
    }
#endif // _USE_MODULES_LocalDataBase

    if (m_pCfgLiveRecordServer)
    {
        delete m_pCfgLiveRecordServer;
    }
}

void CHandlerLiveRecordServer::tprintf(TcpSocket *p,const char *format, ...)
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

void CHandlerLiveRecordServer::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerLiveRecordServer Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        //CHttpServerLiveRecordServer *p4 = dynamic_cast<CHttpServerLiveRecordServer *>(p);

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



void CHandlerLiveRecordServer::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerLiveRecordServer Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    int iNumOfCHttpServerLiveRecordServer = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerLiveRecordServer *p4 = dynamic_cast<CHttpServerLiveRecordServer *>(p);
        if (p4)
        {
            iNumOfCHttpServerLiveRecordServer++;
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
            }

        }
        else
        {}
    }

    if (iLevel==enLogLevelShow_contes)
    {
        snprintf(sendto, sizeof (sendto)-1, "        -------------------------------------------\n");
        strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerLiveRecordServer: [%d]\n", iNumOfCHttpServerLiveRecordServer);
        strDump += sendto;
    }

    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerLiveRecordServer::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerLiveRecordServer *pLiveRecordServer = dynamic_cast<CHttpServerLiveRecordServer *>(p);

        if (pLiveRecordServer)
        {
            pLiveRecordServer->Update();
        }
    }
}

void CHandlerLiveRecordServer::Start()
{
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();
    //std::string __dbFN = __cfgGeneral.getConfig().strDataFilePath + std::string("\\") + _CONFIG_LocalDataBase;

#if defined(_USE_MODULES_LocalDataBase)
    if (NULL == m_dbaLiveRecord)
    {
        m_dbaLiveRecord = new CDbLiveRecord;
        assert(m_dbaLiveRecord);
        m_dbaLiveRecord->Open(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + _CONFIG_LocalDataBase);
    }
#endif // _USE_MODULES_LocalDataBase

    if (NULL == m_pCacheWithDb)
    {
        m_pCacheWithDb = new CCacheKeyValue;
        assert(m_pCacheWithDb);

        //std::string strDbFileName = g_Solar.GetConfigFilePath();
        //strDbFileName+="/";
        //strDbFileName+=def_liveRecordKeyValue;

        m_pCacheWithDb->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_liveRecordKeyValue);

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

    if (NULL == m_pCfgLiveRecordServer)
    {
        m_pCfgLiveRecordServer = new CConfigLiveRecordServer;
        assert(m_pCfgLiveRecordServer);

        m_pCfgLiveRecordServer->update();
    }

    return ;
}

