/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralSocketProcessor.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralSocketProcessor.cpp 5884 2012-07-18 02:51:38Z WuJunjie $
 *
 *  Explain:
 *     -单独的消息处理对象-
 *
 *  Update:
 *     2012-07-18 02:51:38  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <Utility.h>

#include "GeneralSocketProcessor.h"

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

PATTERN_SINGLETON_IMPLEMENT(CGeneralSocketProcessor);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
//使用自己的队列, 原始队列不用
CGeneralSocketProcessor::CGeneralSocketProcessor() :CEZThread("CGeneralSocketProcessor", THREAD_PRIORITY_DEFAULT/*, 100*/)
{
    printf("CGeneralSocketProcessor Enter--------\n");

    m_pSocketMsgQue = new CSocketMsgQue(0);// 0 - do not limit size
}

CGeneralSocketProcessor::~CGeneralSocketProcessor()
{
    printf("CGeneralSocketProcessor Leave--------\n");

    if(m_pSocketMsgQue)
    {
        m_pSocketMsgQue->ClearMessage();
        delete m_pSocketMsgQue;
    }
}

EZTHREAD_BOOL CGeneralSocketProcessor::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CGeneralSocketProcessor::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}
#define MSG_TYPE_PRODUCER 100
#define MSG_TYPE_TIMER 101

void CGeneralSocketProcessor::ThreadProc()
{
    SOCKET_MSG_NODE msg;

    while (m_bLoop)
    {
        //__trip;
        if (EZTHREAD_BOOL_FALSE == RecvSocketMessage(&msg, EZTHREAD_BOOL_TRUE))
        {
            __trip ;
            sleep(1);
            continue;
        }

        // 收到消息，在此加入处理代码
        // 下列代码打印
        {
            __fline;
            printf("CGeneralSocketProcessor::ThreadProc RecvSocketMessage\n");

            char buf[1024];
            struct sockaddr_in sa;
            memcpy((char *)&sa,&msg.sa_from,msg.sa_len);
            ipaddr_t a;
            memcpy(&a,&sa.sin_addr,4);
            std::string ip;
            Utility::l2ip(a,ip);

            snprintf(buf, sizeof(buf), "Received %d bytes from: %s:%d\n", msg.iLen,ip.c_str(),ntohs(sa.sin_port));
            printf(buf);
            if (msg.pData)
            {
                __fline;
                printf("%s\n",static_cast<std::string>(msg.pData).substr(0,msg.iLen).c_str());

                // 返回数据示例
                if (msg.pSocket)
                {
                    //sendto(msg.pSocket->GetSocket(), buf, strlen(buf), 0, sa_from, sa_len);
                    sendto(msg.pSocket->GetSocket(), static_cast<std::string>(msg.pData).substr(0,msg.iLen).c_str(), static_cast<std::string>(msg.pData).substr(0,msg.iLen).size(), 0, &msg.sa_from,msg.sa_len);
                }
            }
            else
            {
                __trip;
            }
        }

        switch(msg.msg)
        {
            case MSG_TYPE_PRODUCER:
            {
                printf("\nReceive msg from PRODUCER\n");
                g_ThreadManager.DumpThreads();

                break;
            }
            case MSG_TYPE_TIMER:
            {
                printf("\nReceive msg from TIMER\n");

                break;
            }
            default:
            {
                break;
            }
        }

        // 这个对象申请的，故而这个对象删除
        if (msg.pData)
        {
            //__trip; // dbg
            delete []msg.pData;
        }

        // __trip;
    }
}

EZTHREAD_BOOL  CGeneralSocketProcessor::SendSocketMsg(unsigned int iMsg,Socket *pSocket, const char *pData,size_t lDataLen,struct sockaddr *sa_from/*=NULL*/,socklen_t sa_len/*=0*/)
{

    char *pBuff;
    if(pData&&lDataLen>0)
    {
        pBuff = new char[lDataLen];
        memcpy(pBuff, pData, lDataLen);
    }
    else
    {
        pBuff = NULL;
    }

    if(m_pSocketMsgQue)
    {
        return m_pSocketMsgQue->SendMessage( iMsg, pSocket, pBuff, lDataLen, sa_from, sa_len);
    }
    return EZTHREAD_BOOL_FALSE;
}

EZTHREAD_BOOL  CGeneralSocketProcessor::RecvSocketMessage (SOCKET_MSG_NODE *pMsg, EZTHREAD_BOOL wait/* = EZTHREAD_BOOL_TRUE*/)
{
    if(m_pSocketMsgQue)
    {
        return m_pSocketMsgQue->RecvMessage(pMsg, wait);
    }
    return EZTHREAD_BOOL_FALSE;
}

