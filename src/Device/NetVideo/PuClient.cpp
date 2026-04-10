/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuClient.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: PuClient.cpp 5884 2019-05-15 09:25:20Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-05-15 02:25:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include "../../Logs.h"
#include "../../Solar.h"

#include "PuClient.h"

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

CPuClient::CPuClient() :CEZThread(PREFIX_CPuClient_THREAD_NAME, THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iSigUser = 0;

    m_emType = MSG_PuClient_Type_Undefined; // 类型
    m_strHost = "";
    m_iPort = 0;
    m_strAccount = "";
    m_strPasswd = "";

    m_uiRound = 0; //循环次数
    m_strPuID = "";  // 设备id

    m_tRunStatus = runStatus_PuClient_Ini;
}

CPuClient::~CPuClient()
{}

EZTHREAD_BOOL CPuClient::Start(CEZObject * pObj, PuClientSignalProc_t pProc)
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    EZTHREAD_BOOL bRet = CreateThread();

    return bRet;
}

EZTHREAD_BOOL CPuClient::Stop(CEZObject * pObj, PuClientSignalProc_t pProc)
{
    ez_printf_debug("%s\n", __FUNCTION__);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iSigUser>0)
    {
        m_iSigUser--;
    }

    if (m_iSigUser==0)
    {
        bRet = DestroyThread();

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            ez_printf_error("DestroyThread failed.\n");
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
            printf("no need, DestroyThread, have %d users.\n", m_iSigUser);
        );
    }

    return bRet;

}

void CPuClient::ThreadProc()
{
    while (m_bLoop)
    {
        sleep(1);
        onTask();
    }

    onStoped();
}

void CPuClient::onTask()
{
    //
    m_uiRound++;

    //ez_printf_debug("%s\n", __FUNCTION__);
}

void CPuClient::onStoped()
{
    m_SigBuffer(MSG_PuClient_STOPED, this);
}

int CPuClient::setPuID(const std::string &strPuID)
{
    m_strPuID = strPuID;

    return 0;
}

const std::string & CPuClient::getPuID ()
{
    return m_strPuID ;
}

int CPuClient::getInfo_Storage()
{
    int iret = 0;

    return iret;
}

int CPuClient::setRunStatus(RunStatus_PuClient_T runStatus)
{
    CEZLock __lock(m_MutexRunStatus);

    m_tRunStatus = runStatus;

    return 0;
}

RunStatus_PuClient_T CPuClient::getRunStatus()
{
    return m_tRunStatus;
}

