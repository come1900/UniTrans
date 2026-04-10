/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CrawlerMmtPlayUrl.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CrawlerMmtPlayUrl.h 5884 2021-04-18 09:46:33Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-18 09:46:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../Stack/promQL/promQLStack.h"

#include "Crawler.h"

#if !defined(_CrawlerMmtPlayUrl_H)
#define _CrawlerMmtPlayUrl_H

class CCrawlerMmtPlayUrl : public CCrawler
{
public:

    CCrawlerMmtPlayUrl();
    virtual ~CCrawlerMmtPlayUrl();
    virtual EZTHREAD_BOOL Start(CEZObject * pObj=NULL);
    virtual EZTHREAD_BOOL Stop(CEZObject * pObj=NULL);
    virtual void ThreadProc();
    void doCrawl();

    virtual bool isCrawed(std::string strCrawId);// 判断是否已经在内

    void setCrawlId(const std::string strCrawId);//

    int cutPlayUrlChkResult(CMmtMsgValueCamDialing &PlayUrlChkResult);//
    int havePlayUrlChkResult();//检查还有多少新的检测结果
    void set_ttPointRtmpOK(time_t ttPointRtmpOK);//

private:
    std::string m_strCrawlId;

    // CEZMutex m_MutexttPointRtmpOK;
    time_t m_ttChkPointRtmp;      // 检查时间
    int m_iChkResultRtmp;         // 检查结果 -1 初始状态， 0-不能播放， 1-可以播放
    time_t m_ttChkPointRtmpCuted; // 结果取走时间
};

#endif // !defined(_CrawlerMmtPlayUrl_H)
