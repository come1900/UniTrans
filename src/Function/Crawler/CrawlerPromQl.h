/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CrawlerPromQl.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CrawlerPromQl.h 5884 2021-04-18 09:46:33Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-18 09:46:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "Crawler.h"

#if !defined(_CrawlerPromQl_H)
#define _CrawlerPromQl_H

class CCrawlerPromQl : public CCrawler
{
public:

    CCrawlerPromQl();
    virtual ~CCrawlerPromQl();
    virtual EZTHREAD_BOOL Start(CEZObject * pObj=NULL);
    virtual EZTHREAD_BOOL Stop(CEZObject * pObj=NULL);
    virtual void ThreadProc();
    void doCrawl();

private:

};

#endif // !defined(_CrawlerPromQl_H)
