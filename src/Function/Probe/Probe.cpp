/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Probe.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Probe.cpp 5884 2018-08-27 02:31:08Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-08-27 02:31:08  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/user.h>
#include <asm/types.h>
#include <linux/netlink.h>
//
#include <str_opr.h>
#include "ez_socket.h"

#include "../../Logs.h"
#include "../../Solar.h"

#include "Probe.h"

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

PATTERN_SINGLETON_IMPLEMENT(CProbe);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CProbe::CProbe() :CEZThread("CProbe", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_pCacheWithDb = NULL;
    m_iUser = 0;
    //ez_trace("CProbe Enter--------\n");
}

CProbe::~CProbe()
{
    if (m_pCacheWithDb)
    {
        delete m_pCacheWithDb;
    }

    //ez_trace("CProbe Leave--------\n");

}

#define def_dbFileKeyValue_CProbe "keyValue_CProbe.s3"
EZTHREAD_BOOL CProbe::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    if (NULL == m_pCacheWithDb)
    {
        m_pCacheWithDb = new CCacheKeyValue;
        assert(m_pCacheWithDb);

        std::string strDbFileName = g_Solar.GetConfigFilePath();
        strDbFileName+="/";
        strDbFileName+=def_dbFileKeyValue_CProbe;

        m_pCacheWithDb->setDbfilename(strDbFileName);
    }

    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CProbe::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CProbe::Start(CEZObject * pObj, ProbeSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        __fline;
        printf("attach error\n");
        return bRet;
    }

    if (m_iUser==0)
    {
        if (NULL == m_pCacheWithDb)
        {
            m_pCacheWithDb = new CCacheKeyValue;
            assert(m_pCacheWithDb);

            std::string strDbFileName = g_Solar.GetConfigFilePath();
            strDbFileName+="/";
            strDbFileName+=def_dbFileKeyValue_CProbe;

            m_pCacheWithDb->setDbfilename(strDbFileName);
        }

        CreateThread();

        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {

        bRet = EZTHREAD_BOOL_TRUE;

        DBG(
            __fline;
            printf("no need, CreateThread, have %d users.\n", m_iUser);
        );

    }

    m_iUser++;

    ez_printf_debug("m_iUser:%d\n", m_iUser);

    return bRet;
}
EZTHREAD_BOOL CProbe::Stop(CEZObject * pObj, ProbeSignalProc_t pProc)
{
    ez_printf_info("CProbe::Stop\n");
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iUser>0)
    {
        m_iUser--;
    }


    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
        // m_iUser--;
        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        // error
        __trip;
        printf("Detach failed.\n");
    }

    if (m_iUser==0)
    {

        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            __trip;
            printf("DestroyThread failed.\n");
        }
        else
        {
            DBG(
                printf("DestroyThread OK.\n");
            );
        }
    }
    else
    {
        DBG(
            __fline;
            printf("no need, DestroyThread, have %d users.\n", m_iUser);
        );
    }

    return bRet;
}

void CProbe::ThreadProc()
{
    while (m_bLoop)
    {
        ez_printf_debug("ThreadProc.\n");

        def_uniKeyVector __newOne;
        // ȡ����
        m_pCacheWithDb->getKey(__newOne);

        def_uniKeyVector::const_iterator it = __newOne.begin();
        for ( ; it != __newOne.end() ; it++)
        {
            std::string strKey = *it;
            std::string strValue;
            m_pCacheWithDb->getPair(strKey, strValue);

            // �����������
            int iret = ez_rtsp_valid(strValue.c_str(), 0);
            ez_printf_debug("ez_rtsp_valid(%d):%s\n", iret, strValue.c_str());

            // good
            if (0== iret)
            {
                bankCloudUpdateDevice(strKey, "Y");
                ez_printf_debug("bankCloudUpdateDevice:%s, %s\n", strKey.c_str(), "Y");
            }
            else
            {
                bankCloudUpdateDevice(strKey, "N");
                ez_printf_error("bankCloudUpdateDevice:%s, %s\n", strKey.c_str(), "N");
            }

        }

        sleep(1);

        int iret = 0;

        ARG_USED(iret);
    }
}

int CProbe::addPeer(const std::string &strPeerID, const std::string &strPeerInfo)
{
    if (strPeerID.length()<1)
    {
        return -1;
    }

    int iret_modPair = m_pCacheWithDb->modPair(strPeerID, strPeerInfo, modPair_If_Not_Exist_Then_Add);

    if (0 == iret_modPair)
    {
        ez_printf_debug("Key:%s, Val:%s\n",  strPeerID.c_str(), strPeerInfo.c_str());
    }

    return 0;
}

int CProbe::delPeer(const std::string &strPeerID)
{
    if (strPeerID.length()<1)
    {
        return -1;
    }

    int iret = m_pCacheWithDb->delPair(strPeerID);

    return iret;
}

int CProbe::bankCloudUpdateDevice(const std::string &strPeerID, const std::string &strPeerStatus)
{
    const char *hname = "180.109.118.37";
    const char *hport = "18080";
    const char *page = "/bankCloud/updateDevice";

    std::string p_post_string = "DevId=" + strPeerID+ "&DevStatus="+strPeerStatus;
    char recvline[HTTP_MAXLINE + 1];
    int iFastMode = 1;

    int iret = ez_http_post_to_host(hname, hport, page, p_post_string.c_str(), recvline, HTTP_MAXLINE, iFastMode);

    printf("recv(%d):\n--------------------\n%s\n---------------------\n", iret, recvline);

    return 0;
}

