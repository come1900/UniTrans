/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerLiveStatusServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerLiveStatusServer.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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
#include "StringOpr.h"

#include "../../Logs.h"
#include "../../Solar.h"
#include "../../Configs/ConfigGeneral.h"

#include "../Pandora/PandoraDef.h"

#include "HandlerLiveStatusServer.h"

#include "HttpServerLiveStatusServer.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

#define def_liveStatusKeyValue "keyValue_liveStatus.s3"
#define def_tprintf_Buff_SIZE 4096

CHandlerLiveStatusServer::CHandlerLiveStatusServer(CMaintainAgentLog *pLog)
        :SocketHandlerEp(pLog)
{
#if defined(def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer)
     m_pCacheWithDb = NULL;
#endif //def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer

    m_pCacheKeyValue = NULL;
}

CHandlerLiveStatusServer::~CHandlerLiveStatusServer()
{
#if defined(def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer)
    if (m_pCacheWithDb)
    {
        delete m_pCacheWithDb;
    }
#endif //def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer

    if (m_pCacheKeyValue)
    {
        delete m_pCacheKeyValue;
    }
}

void CHandlerLiveStatusServer::tprintf(TcpSocket *p,const char *format, ...)
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

void CHandlerLiveStatusServer::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerLiveStatusServer Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        //CHttpServerLiveStatusServer *p4 = dynamic_cast<CHttpServerLiveStatusServer *>(p);

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



void CHandlerLiveStatusServer::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerLiveStatusServer Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    int iNumOfCHttpServerLiveStatusServer = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerLiveStatusServer *p4 = dynamic_cast<CHttpServerLiveStatusServer *>(p);
        if (p4)
        {
            iNumOfCHttpServerLiveStatusServer++;
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
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerLiveStatusServer: [%d]\n", iNumOfCHttpServerLiveStatusServer);
        strDump += sendto;
    }

    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerLiveStatusServer::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerLiveStatusServer *pLiveStatusServer = dynamic_cast<CHttpServerLiveStatusServer *>(p);

        if (pLiveStatusServer)
        {
            pLiveStatusServer->Update();
        }
    }
}

void CHandlerLiveStatusServer::Start()
{
#if defined(def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer)
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    if (NULL == m_pCacheWithDb)
    {
        m_pCacheWithDb = new CCacheKeyValue;
        assert(m_pCacheWithDb);

        m_pCacheWithDb->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_liveStatusKeyValue);

        // empty
        if (m_pCacheWithDb->numPair()<1)
        {
            m_pCacheWithDb->addPair("701818788", "c48faffaedf5d4cae01c6a21c11476fc118e32b0");
            m_pCacheWithDb->addPair("701918788", "9cba8e3fe970ddbca35424db80197a0167d68813");
            m_pCacheWithDb->addPair("701819701", "ebd69a6ccd6f81ef4e6c52e37761750b42f0fe50");
        }
    }
#endif //def_doNotUseNow_useCacheWithDb_CHandlerLiveStatusServer

    if (NULL == m_pCacheKeyValue)
    {
        m_pCacheKeyValue = new CCacheKeyValue;
        assert(m_pCacheKeyValue);
    }

    return ;
}

// 获取播放数量
int CHandlerLiveStatusServer::getPlayerNums(const std::string &strStreamID)
{
    std::string __strDstIKey;

    CCacheKeyValue::bldCacheKey(__strDstIKey, strStreamID, PssNotifyMsg_Key_Notify_PlayCount);
    int iValue = 0;
    m_pCacheKeyValue->getPair(__strDstIKey, iValue);

    return iValue;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// >0 ok
// 0 not Play
// 对应在 CAgentLiveStatusServer::OnFileTailer 中维护
time_t CHandlerLiveStatusServer::getLastPlayTime(const std::string &strStreamID)
{
    std::string __strDstIKey;

    CCacheKeyValue::bldCacheKey(__strDstIKey, strStreamID, PssNotifyMsg_Key_Notify_PlayTTime);
    time_t ttValue = 0;
    m_pCacheKeyValue->getPair(__strDstIKey, ttValue);

    return ttValue;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// >0 ok
// 0 not publish, offline ...
time_t CHandlerLiveStatusServer::getLastUpdate(const std::string &strStreamID)
{
    std::string __strDstIKey;

    CCacheKeyValue::bldCacheKey(__strDstIKey, strStreamID, PssNotifyMsg_Key_Notify_PublishTTime);
    time_t ttValue = 0;
    m_pCacheKeyValue->getPair(__strDstIKey, ttValue);

    return ttValue;
}

int CHandlerLiveStatusServer::listPublished(std::vector<std::string> &vPublishedID)
{
    def_uniKeyVector *pAllKeys = new def_uniKeyVector;
    assert(pAllKeys);

    if (NULL==pAllKeys ||NULL==m_pCacheKeyValue)
    {
        return -1;
    }

    int ret_getKey = m_pCacheKeyValue->getKey((*pAllKeys));
    ARG_USED(ret_getKey);
    def_uniKeyVector::const_iterator it = pAllKeys->begin();
    for ( ; it != pAllKeys->end() ; it++)
    {
        const std::string separator = PssNotifyMsg_Key_Notify_PublishTTime;
        std::vector<std::string> dest;

        int retSplit = Split(*it, separator, dest);
        ARG_USED(retSplit);

        if (1==dest.size() && 1==retSplit)
        {
            //ez_printf_debug("%s::%s\n", it->c_str(), dest[0].c_str());

            vPublishedID.push_back(dest[0]);
        }//if
    }// for

    delete pAllKeys;

    return 0;
}

