/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Crawler.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Crawler.cpp 5884 2021-04-18 09:02:41Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-18 09:02:41  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>

#include "Crawler.h"

#define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

CCrawler::CCrawler() : CEZThread("CCrawler", THREAD_PRIORITY_JWORKER)
{
    //printf("CCrawler Enter--------\n");

    m_pCrawlerManager = NULL;
    // m_strCrawlerId = "";
}

CCrawler::~CCrawler()
{
    //printf("CCrawler Leave--------\n");
}

EZTHREAD_BOOL CCrawler::Start(CEZObject * pObj)
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CCrawler::Stop(CEZObject * pObj)
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

void CCrawler::ThreadProc()
{
    while (m_bLoop)
    {
        // __fline;
        // printf("todo ...\n");

        sleep(1);
    }
}

bool CCrawler::isCrawed(std::string strCrawId) // 判断是否已经在内
{
    return false;
}
