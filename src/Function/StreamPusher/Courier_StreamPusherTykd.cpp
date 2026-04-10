/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Courier_StreamPusherTykd.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Courier_StreamPusherTykd.cpp 5884 2019-06-13 10:04:46Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-06-13 10:04:46  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h> // sleep
#include <StringOpr.h>
//#include "wsApiVideoCloud1.h"
#include "uniFacePP.h"

#include "../../Logs.h"
#include "../../Solar.h"
#include "../../Configs/ConfigPullTykdMobileService.h"
#ifdef _FUNC_LiveStatusServer
#include "../LiveStatusServer/AgentLiveStatusServer.h"
#endif// _FUNC_LiveStatusServer 

#include "Courier_StreamPusherTykd.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
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

const int Period_saveDevStatusReq = 10;

CCourier_StreamPusherTykd::CCourier_StreamPusherTykd(def_PusherContainer *pPusher):CEZThread("Courier_StreamPusherTykd", THREAD_PRIORITY_DEFAULT)
{
    m_pPusherCt = pPusher;

    m_strStatusUploadWS = "";
    m_iStatusUploadPeriod = 10;
}

CCourier_StreamPusherTykd::~CCourier_StreamPusherTykd()
{}

EZTHREAD_BOOL CCourier_StreamPusherTykd::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    // ≤‚ ‘ª∑æ≥
    //static std::string g_strEndpoint = "http://192.168.5.81:18086/storageGateway/cxf/StorageGatewayServices?wsdl";
    //setEndpoint_wsApiVideoCloud1(g_strEndpoint);

#if defined(_SUPPORT_CFG_StatusUploadWS)
    CConfigPullTykdMobileService __cfg;
    __cfg.update();

    //setEndpoint_wsApiVideoCloud1(__cfg.getConfig().strStatusUploadWS);
    m_strStatusUploadWS = __cfg.getConfig().strStatusUploadWS;
    m_iStatusUploadPeriod = __cfg.getConfig().iStatusUploaderiod;

    //ez_printf_info("%s:%s\n",  "setEndpoint_wsApiVideoCloud1", m_strStatusUploadWS.c_str());
#endif

    if (m_iStatusUploadPeriod<1 || m_strStatusUploadWS.empty())
    {
        //ez_printf_info("m_strStatusUploadWS.empty, do not use CCourier_StreamPusherTykd\n");
        return EZTHREAD_BOOL_FALSE;
    }
    else
    {
        //ez_printf_info("%s:%s\n",  "setEndpoint_wsApiVideoCloud1", m_strStatusUploadWS.c_str());
    }

    // Ã©÷›
    // http://218.90.248.99:17070/cloudVideo/cxf/StorageGatewayServices?wsdl
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CCourier_StreamPusherTykd::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}
void CCourier_StreamPusherTykd::ThreadProc()
{
    while (m_bLoop)
    {
        int iret = Do_saveDevStatusReq();
        ARG_USED(iret);

        sleep(m_iStatusUploadPeriod);
    }
}

int CCourier_StreamPusherTykd::Do_saveDevStatusReq()
{
    static unsigned long  __ssLastSave = 0;
    unsigned long __ssNow = g_Solar.GetRunPeriod();

    if (__ssNow < __ssLastSave+Period_saveDevStatusReq)
    {
        return 0;
    }

    if (NULL==m_pPusherCt)
    {
        return -1;
    }

    def_DevStatusVector __VectorDevStatus;
    DevStatus __DevStatus;

    for (def_PusherContainer::iterator it = m_pPusherCt->begin(); it != m_pPusherCt->end(); it++)
    {
        CPusher *p = (*it);
        if (NULL==p)
        {
            continue;
        }
#if 1

        __DevStatus.DevId     = p->getTykdDevID();
//#if defined(_VEND_LiveStreammingNewMedia_ZSDC)
        //__DevStatus.DevStatus     = "1";
//#else//_VEND_LiveStreammingNewMedia_ZSDC

#ifdef _FUNC_LiveStatusServer
        time_t __ttLastUpdate = g_AgentLiveStatusServer.GetHandler()->getLastUpdate(p->getIdDst());
        //ARG_USED(__ttLastUpdate);
        int __iLived = time(NULL) - __ttLastUpdate;//≥¨ ±≈–∂œ

        if (__ttLastUpdate > 0 && __iLived < 60 /*60√Î≥¨ ±*/)
        {
            __DevStatus.DevStatus = "1"; //ezConvertToString(__ttLastUpdate>0);
        }
        else
        {
            //__DevStatus.DevStatus = ezConvertToString(p->getDevStatus()==em_terminal_status_online);
            __DevStatus.DevStatus = "0";
        }

        ez_printf_debug("%s::ttLastUpdate:%d, __iLived:%d, DevStatus:%s\n", p->getIdDst().c_str(), __ttLastUpdate, __iLived, __DevStatus.DevStatus.c_str());
#else
        __DevStatus.DevStatus = ezConvertToString(p->getDevStatus()==em_terminal_status_online);
        //ez_printf_debug("%s:: DevStatus:%s\n", p->getIdDst().c_str(), __DevStatus.DevStatus.c_str());
#endif // _FUNC_LiveStatusServer

        //__DevStatus.DevStatus     = ezConvertToString(p->getDevStatus()==em_terminal_status_online);
//#endif //_VEND_LiveStreammingNewMedia_ZSDC

        __DevStatus.VideoStatus   = __DevStatus.DevStatus;
        __DevStatus.CloudStatus   = __DevStatus.DevStatus;
        __DevStatus.NetStatus     = __DevStatus.DevStatus;
        __VectorDevStatus.push_back(__DevStatus);
#else
        //CPusherTykd2Rtmp *p1 = dynamic_cast<CPusherTykd2Rtmp *>(p);

        const std::string strDevId       = p->getTykdDevID();
        const std::string strDevStatus = ezConvertToString(p->PushStatus()==Pusher_PushStatus_T_Pushing);
        const std::string strVideoStatus = strDevStatus;
        const std::string strCloudStatus = strDevStatus;			
        int iret = saveDevStatusReq( \
                                     strDevId       ,
                                     strDevStatus   ,
                                     strVideoStatus ,
                                     strCloudStatus
                                   );
        if (0 == iret)
        {
            //ez_printf_debug("saveDevStatusReq -- [%s:%d]\n", strDevId.c_str(), iret);
        }
        else
        {
            ez_printf_error("%s::[%s:%d]\n", __FUNCTION__, strDevId.c_str(), iret);
        }
#endif
    }// for
    
    //int iret = saveDevStatusListReq( __VectorDevStatus );
    ez_printf_debug("ws:%s\n", m_strStatusUploadWS.c_str());

    int iret = uniFace_saveDevStatusListReq(m_strStatusUploadWS.c_str(), __VectorDevStatus );
    if (0 == iret || 0 == __VectorDevStatus.size())
    {
        ez_printf_debug("%s:%d, size:%ld\n", __FUNCTION__, iret, __VectorDevStatus.size());
    }
    else
    {
        ez_printf_error("%s:%d, size:%ld\n", __FUNCTION__, iret, __VectorDevStatus.size());
    }

    __ssLastSave = g_Solar.GetRunPeriod();

    return 0;
}
