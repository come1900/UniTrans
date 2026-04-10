/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SocketMsgQue.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: SocketMsgQue.h 5884 2012-07-19 09:20:26Z WuJunjie $
 *
 *  Explain:
 *     -
 *       保存ppsocket消息，队列中保存socket、数据指针，数据缓存需要另外申请
 *       具体用法参考： CGeneralSocketProcessor
 *     -
 *
 *  Update:
 *     2012-07-19 09:20:26  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "SocketMsgQue.h"
#include "EZLock.h"

CSocketMsgQue::CSocketMsgQue(int size /* = 1024 */)// : m_Queue(size)
{
    m_nMaxMsg = size;
    m_nMsg = 0;
    m_bMsgFlg = true;
}

CSocketMsgQue::~CSocketMsgQue()
{}

EZTHREAD_BOOL CSocketMsgQue::SendMessage(unsigned int msg,Socket *pSocket, const char *p,size_t l,struct sockaddr *sa_from/*=NULL*/,socklen_t sa_len/*=0*/)
{
    SOCKET_MSG_NODE l_MSG;
    SOCKET_MSGQUEUE::iterator pi;

    m_Mutex.Lock();
    if(m_nMaxMsg>0
       && m_nMsg >= m_nMaxMsg)
    {
        m_Mutex.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }
    if (!m_bMsgFlg)
    {
        m_Mutex.Unlock();
        return EZTHREAD_BOOL_FALSE;
    }

    /************************************************************************
    	发送消息:
    	1、按照优先级把该消息插入队列m_Queue；
    //	2、在该队列中查找该消息节点，直到找不到该节点才退出循环并从该函数返回；
    ************************************************************************/
    l_MSG.msg = msg;
    l_MSG.pSocket = pSocket;
    l_MSG.pData = (char *)p;
    l_MSG.iLen = l;
    memcpy((char *)&l_MSG.sa_from, (char *)sa_from, sizeof(struct sockaddr));
    l_MSG. sa_len = sa_len;
    l_MSG.time = time(NULL);

    m_Queue.push_front(l_MSG);
    m_nMsg++;
    m_Mutex.Unlock();

    m_Semaphore.Post();
    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CSocketMsgQue::RecvMessage(SOCKET_MSG_NODE *pMsg, EZTHREAD_BOOL wait /* = EZTHREAD_BOOL_TRUE */)
{
    /************************************************************************
    	接收消息，如果等待则一直等到有消息时返回，否则直接返回。
    	1、从消息忙队列m_Queue取元素，如果取成功，则直接返回；
    	2、否则循环从消息忙m_Queue中取元素，直到取成功才退出循环；
    ************************************************************************/

    if(wait)
    {
        m_Semaphore.Wait();
    }

    CEZLock guard(m_Mutex);
    //	ez_trace("RecvMessage %d\n", m_nMsg);

    if (m_Queue.empty())
    {
        //		ez_tracepoint();
        return EZTHREAD_BOOL_FALSE;
    }
	#if 0
    if(!wait)
    {
        m_Semaphore.Wait();
    }
	#endif
    assert(m_nMsg);
    *pMsg = m_Queue.back();
    m_Queue.pop_back();
    m_nMsg--;

    return EZTHREAD_BOOL_TRUE;
}

void CSocketMsgQue::QuitMessage ()
{
    CEZLock guard(m_Mutex);

    m_bMsgFlg = false;
}

void CSocketMsgQue::ClearMessage()
{
    CEZLock guard(m_Mutex);

    int n = m_nMsg;
    for(int i = 0; i < n; i++)
    {
        m_Semaphore.Wait();
        m_Queue.pop_back();
        m_nMsg--;
    }
}

int CSocketMsgQue::GetMessageCount()
{
    CEZLock guard(m_Mutex);

    return m_nMsg;
}

int CSocketMsgQue::GetMessageSize()
{
    CEZLock guard(m_Mutex);

    return m_nMaxMsg;
}

void CSocketMsgQue::SetMessageSize(int size)
{
    CEZLock guard(m_Mutex);

    m_nMaxMsg = size;
}
