/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Crawler.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Crawler.h 5884 2021-04-18 09:02:39Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-18 09:02:39  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "EZThread.h"
#include "EZTimer.h"

#if !defined(_Crawler_H)
#define _Crawler_H

#define PREFIX_CCrawler_THREAD_NAME "Crawler-"
#define PREFIX_CCrawlerMmtPlayUrl_THREAD_NAME "CrawlerMmtPlayUrl-"

class CManagerCrawler;
class CCrawler : public CEZThread
{
public:

    CCrawler();
    virtual ~CCrawler();

    virtual EZTHREAD_BOOL Start(CEZObject * pObj=NULL);
    virtual EZTHREAD_BOOL Stop(CEZObject * pObj=NULL);
    virtual void ThreadProc();

    virtual bool isCrawed(std::string strCrawId);// ????????

protected:
    CManagerCrawler * m_pCrawlerManager; // Í¨¹ýobj ¸³Öµ

    // std::string m_strCrawlerId;// ID

private:

};

typedef std::list<CCrawler *> def_CrawlerContainer;

#endif // !defined(_Crawler_H)
