/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HandlerPandoraMatrix.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HandlerPandoraMatrix.cpp 5884 2013-01-29 03:21:10Z WuJunjie $
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
#include <assert.h>

#include "StringOpr.h"

#include "EZLock.h"
#include "ez_util_platform.h"/*ms_count*/

#include "../../../Logs.h"
#include "../../../Solar.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigGeneral.h"
#include "../../../Configs/ConfigPandoraMatrix.h"
#endif //_USE_MODULES_EZCONFIG

#if defined(_FUNC_LiveInclusion)
#include "../../Probe/Probe.h"
#endif

#include "HandlerPandoraMatrix.h"
#include "HttpServerPandoraMatrix.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

#define def_tprintf_Buff_SIZE 4096
#define def_pandoraMatrixKeyValue "pandoraMatrixKeyValue.s3"
#define def_pandoraMatrixKeyValue_NodeConfig "pandoraMatrixKeyValue_NodeConfig.s3"

CHandlerPandoraMatrix::CHandlerPandoraMatrix(CMaintainAgentLog *pLog)
        :CHandlerGRestHttpdSocket(pLog)
        //:SocketHandlerEp(pLog)
{
    m_pCacheWithDb = NULL;
    m_pCacheKeyValue = NULL;

    m_pCacheWithDb_NodeConfig = NULL;
}

CHandlerPandoraMatrix::~CHandlerPandoraMatrix()
{
    if (m_pCacheWithDb)
    {
        delete m_pCacheWithDb;
    }

    if (m_pCacheKeyValue)
    {
        delete m_pCacheKeyValue;
    }

    if (m_pCacheWithDb_NodeConfig)
    {
        delete m_pCacheWithDb_NodeConfig;
    }
}

void CHandlerPandoraMatrix::tprintf(TcpSocket *p,const char *format, ...)
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

void CHandlerPandoraMatrix::List(TcpSocket *sendto)
{
    int ii=0;
    tprintf(sendto, "HandlerPandoraMatrix Socket List\n");
    tprintf(sendto, "---------------------------------------------------\n");
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerPandoraMatrix *p4 = dynamic_cast<CHttpServerPandoraMatrix *>(p);

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

void CHandlerPandoraMatrix::List(std::string &strDump, int iLevel)
{
    int ii=0;
    char sendto[1024];

    snprintf(sendto, sizeof (sendto)-1, "HandlerPandoraMatrix(%lu) Socket List\n", GetCount());
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");
    strDump += sendto;
    size_t iNumOfCHttpServerPandoraMatrix = 0;
    size_t iNumOfTcpSocket = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        TcpSocket *p3 = dynamic_cast<TcpSocket *>(p);
        CHttpServerPandoraMatrix *p4 = dynamic_cast<CHttpServerPandoraMatrix *>(p);
        if (p->GetRemotePort()<=0)
        {
            continue;
        }

        if (p3)
        {
            iNumOfTcpSocket++;
        }

        if (p4)
        {
            iNumOfCHttpServerPandoraMatrix++;
        }


        if (iLevel == enLogLevelShow_detail)
        {
            if (GetCount() <= 4)
            {
                snprintf(sendto, sizeof (sendto)-1, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
                strDump += sendto;
                snprintf(sendto, sizeof (sendto)-1, "  %9s  Name:[%s]\n", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
                strDump += sendto;
                //                snprintf(sendto, sizeof (sendto)-1, "\n");
                //                strDump += sendto;
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

                    snprintf(sendto, sizeof (sendto)-1, "\t%15s : %lu\n", "Msg Received", p4 -> getMsgNumReceived());
                    strDump += sendto;
                    snprintf(sendto, sizeof (sendto)-1, "\t%15s : %lu\n", "Msg Sent", p4 -> getMsgNumSent());
                    strDump += sendto;
                    snprintf(sendto, sizeof (sendto)-1, "\t%15s : [%s]\n", "Session", p4 -> getMySessionId().c_str());
                    strDump += sendto;
                    //snprintf(sendto, sizeof (sendto)-1, "\t%15s : Hls-%d, Rtmp-%d, Rtsp-%d\n", "Load", p4 -> GetLoadHls(), p4 -> GetLoadRtmp(), p4 -> GetLoadRtsp());
                    //strDump += sendto;
                    //snprintf(sendto, sizeof (sendto)-1, "\t%15s : %d - %d = %d\n", "Channel", p4 -> GetChannel(), p4 -> GetChannelCreated(), p4 -> GetChannelRemains());
                    //strDump += sendto;
                }
            }
            else if (GetCount() <= 31)
            {
                snprintf(sendto, sizeof (sendto)-1, "%-3d %15s:%-5d", ii, p -> GetRemoteAddress().c_str(),p -> GetRemotePort());
                strDump += sendto;
                snprintf(sendto, sizeof (sendto)-1, "  %9s  Name:[%s]\n", p -> Ready() ? "Ready" : "NOT Ready", p->GetSockName().c_str());
                strDump += sendto;
            }
            else
            {
                //snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerPandoraMatrix: [%d]\n", GetCount());
                snprintf(sendto, sizeof (sendto)-1, "\tToo many CHttpServerPandoraMatrix: [%lu]\n", GetCount());
                strDump = sendto;
                strDump += "show contents only\n";
            }//if (GetCount() <= 3)
        }
    }

    if (iLevel==enLogLevelShow_list)
    {
        //snprintf(sendto, sizeof (sendto)-1, "        -------------------------------------------\n");
        //strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerPandoraMatrix: [%lu]\n", iNumOfCHttpServerPandoraMatrix);
        strDump += sendto;
    }
    else if (iLevel==enLogLevelShow_contes)
    {
        //snprintf(sendto, sizeof (sendto)-1, "        -------------------------------------------\n");
        //strDump += sendto;
        snprintf(sendto, sizeof (sendto)-1, "\tCHttpServerPandoraMatrix: [%lu]\n", GetCount());
        strDump += sendto;
    }

    snprintf(sendto, sizeof (sendto)-1, "\n");
    strDump += sendto;
}

void CHandlerPandoraMatrix::List(CPqMsg_requestMsgDeviceListAck &DeviceLis
                                 , const std::string strGroupID
                                 , const int strPageSize
                                 , const int strPages)
{
    int ii = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++, ii++)
    {
        Socket *p = (*it).second;
        CHttpServerPandoraMatrix *pPandoraMatrix = dynamic_cast<CHttpServerPandoraMatrix *>(p);

        if (p->GetRemotePort()<=0)
        {
            continue;
        }

        if (pPandoraMatrix
            && pPandoraMatrix->getNodeID().length() >= def_Min_Node_Name_Length)
        {
            //iNumOfCHttpServerPandoraMatrix++;
            CDeviceMsg __dev;

            // vendor is groupid
            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, pPandoraMatrix->getNodeID(), UniMsg_Key_Vendor);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strVendor);

                ez_printf_debug("__strDstIKey:%s, m_strVendor:%s\n", __strDstIKey.c_str(), __dev.m_strVendor.c_str());

                // 使用groupid 过滤
                if (strGroupID.length()>0
                    && 0==iret_getPair
                    && strGroupID != __dev.m_strVendor)
                {
                    continue;
                }
                //ARG_USED(iret_getPair);
            }

            __dev.m_strDevID = pPandoraMatrix->getNodeID();
            __dev.m_strLastOnTime = ezConvertToString(pPandoraMatrix->CreateTime());

            ez_printf_debug("DevID:%s\n", __dev.m_strDevID.c_str());

            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, __dev.m_strDevID, PqMsg_Key_Type);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strCode);

                if (iret_getPair != 0)
                    __dev.m_strCode = "node.ezlibs.com"; //ezConvertToString(em_device_type_g1);
            }
            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, __dev.m_strDevID, UniMsg_Key_Version);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strVersion);

                if (iret_getPair != 0)
                    __dev.m_strVersion = "unknow"; //ezConvertToString(em_device_type_g1);
            }
            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, __dev.m_strDevID,  PqMsg_Key_Name);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strName);

                ARG_USED(iret_getPair);
            }
            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, __dev.m_strDevID, UniMsg_Key_Description);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strDescription);

                ARG_USED(iret_getPair);
            }
            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, __dev.m_strDevID, UniMsg_Key_Location);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strLocation);

                ARG_USED(iret_getPair);
            }
            {
                std::string __strDstIKey="";
                /*int iret_ValueKey = */
                CCacheKeyValue::bldCacheKey(__strDstIKey, __dev.m_strDevID, UniMsg_Key_Mac);
                int iret_getPair = m_pCacheWithDb->getPair(__strDstIKey, __dev.m_strMac);

                ARG_USED(iret_getPair);
            }

            __dev.m_strStatus = ezConvertToString(em_device_status_Online);
            DeviceLis.Devices.push_back(__dev);
            //ez_printf_debug("GetPandoraNode:%s(%d)\n", p->getNodeID().c_str(), p->GetRemotePort());
        }
    }
}

// 获取设备信息
int CHandlerPandoraMatrix::getNode(CDeviceMsg *pTheDevice, const std::string strDeviceID)
{
    int iret = -1;

    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerPandoraMatrix *pPandoraMatrix = dynamic_cast<CHttpServerPandoraMatrix *>(p);

        if (p->GetRemotePort()<=0)
        {
            continue;
        }

        if (pPandoraMatrix
            && pPandoraMatrix->GetSockName() == strDeviceID)
        {
            if (pTheDevice)
            {
                pTheDevice->m_strDevID = pPandoraMatrix->GetSockName();
                pTheDevice->m_strCode = "node.ezlibs.com";//ezConvertToString(em_device_type_g1);
                pTheDevice->m_strStatus = ezConvertToString(em_device_status_Online);
            }

            ez_printf_info("GetPandoraNode:%s(%d)\n", p->GetSockName().c_str(), p->GetRemotePort());

            iret = 0;
            break;

        }
    }

    return iret;
}

void CHandlerPandoraMatrix::Update()
{
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerPandoraMatrix *pPandoraMatrix = dynamic_cast<CHttpServerPandoraMatrix *>(p);

        if (pPandoraMatrix)
        {
            pPandoraMatrix->Update();
        }
    }
}

void CHandlerPandoraMatrix::Start()
{
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    if (NULL == m_pCacheWithDb)
    {
        m_pCacheWithDb = new CCacheKeyValue;
        assert(m_pCacheWithDb);

        //std::string strDbFileName = g_Solar.GetConfigFilePath();
        //strDbFileName+="/";
        //strDbFileName+=def_pandoraMatrixKeyValue;

        //m_pCacheWithDb->setDbfilename(strDbFileName);
        //m_pCacheWithDb->setDbfilename(def_pandoraMatrixKeyValue);

        m_pCacheWithDb->setDbfilename((__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_pandoraMatrixKeyValue));

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

    if (NULL == m_pCacheWithDb_NodeConfig)
    {
        m_pCacheWithDb_NodeConfig = new CCacheKeyValue;
        assert(m_pCacheWithDb_NodeConfig);

        //m_pCacheWithDb_NodeConfig->setKeyLen(def_defaultKeyLen);
        m_pCacheWithDb_NodeConfig->setValueLen(10*1024);
        m_pCacheWithDb_NodeConfig->setDbfilename((__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_pandoraMatrixKeyValue_NodeConfig));
    }

#if defined(_FUNC_LiveInclusion)

    EZTHREAD_BOOL bStart = g_Probe.Start();
    ARG_USED(bStart);
#endif// liv

    return ;
}

CHttpServerPandoraMatrix *CHandlerPandoraMatrix::GetPandoraNode(const std::string &strNodeName)
{
    if (strNodeName.length()<def_Min_Node_Name_Length)
    {
        return NULL;
    }

    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
        Socket *p = (*it).second;
        CHttpServerPandoraMatrix *__pMatrixNode = dynamic_cast<CHttpServerPandoraMatrix *>(p);

        if (__pMatrixNode != NULL
            && strNodeName == __pMatrixNode->GetSockName())
        {
            return __pMatrixNode;
        }
    }

    return NULL;

}

// 外部调用接口
// return
// -100 - busying
int CHandlerPandoraMatrix::Do_MatchReq(const std::string &strUrl, const CPqMsg_requestMsg &__reqMsg)
{
    CHttpServerPandoraMatrix *pMatrixNode = NULL;

    CEZLock __lock(m_MutexMatchReq);

    pMatrixNode = GetPandoraNode(__reqMsg.m_strDevID);

    if (NULL == pMatrixNode)
    {
        ez_printf_warning("[%s] Not found.\n", __reqMsg.m_strDevID.c_str());

        return -3;
    }

    //转给具体节点处理
    CPqStack __StackSend;
    __StackSend.Encode(__reqMsg);
    pMatrixNode->Do_MatchReq(strUrl, __StackSend.ToString());

    return 0;
}


