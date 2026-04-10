/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuManager.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: PuManager.cpp 5884 2019-05-15 02:25:06Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-05-15 02:25:06  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>

#include "../../Logs.h"
#include "../../Solar.h"

#if defined(_FUNC_LiveStreamming_Support_PuSdk_DH)
#include "../../Device/NetVideo/PuClientDahua.h"
#endif //_FUNC_LiveStreamming_Support_PuSdk_DH

#include "PuManager.h"

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

PATTERN_SINGLETON_IMPLEMENT(CPuManager);

CPuManager::CPuManager():CEZThread(PREFIX_CPuManager_THREAD_NAME, THREAD_PRIORITY_DEFAULT)
{}

CPuManager::~CPuManager()
{}

EZTHREAD_BOOL CPuManager::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

#if defined(_FUNC_LiveStreamming_Support_PuSdk_DH)
    // µ÷ÊÔÓÃ¡£¡£¡£
    CPuClientDahua *pPuClient_DH1 = new CPuClientDahua;
    pPuClient_DH1->Start(this, (CPuClient::PuClientSignalProc_t)&CPuManager::OnPuClientMsg);
    addPuClient(pPuClient_DH1);
#endif //_FUNC_LiveStreamming_Support_PuSdk_DH

    EZTHREAD_BOOL bRet = CreateThread();

    return bRet;
}

EZTHREAD_BOOL CPuManager::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}
void CPuManager::ThreadProc()
{
    while (m_bLoop)
    {
        //ez_printf_debug("%s\n", __FUNCTION__);

        sleep(1);
    }
}

int CPuManager::addPuClient(CPuClient *pPuClient)
{
    if (NULL == pPuClient)
    {
        return -1;
    }

    CEZLock __lock(m_MutexPuClient);
    m_v_pPuClient.push_back(pPuClient);

    return -1;
}

int CPuManager::delPuClient(CPuClient *pPuClient)
{
    if (NULL == pPuClient)
    {
        return -1;
    }
    //CEZLock __lock(m_MutexCPuClient);
    unsigned int ii;
    for (ii=0; ii<m_v_pPuClient.size(); ii++)
    {
        if (m_v_pPuClient[ii] == pPuClient)
        {
            delPuClient(ii);
            return 0;
        }
    }

    return -1;
}

int CPuManager::delPuClient(int idx)
{
    CEZLock __lock(m_MutexPuClient);

    m_v_pPuClient.erase(m_v_pPuClient.begin() + idx);

    return 0;
}

void CPuManager::OnPuClientMsg(int iMsg, void *pPuClient)
{
    switch ( iMsg)
    {
        case MSG_PuClient_STOPED:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}
