/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CrawlerMmtPlayUrl.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CrawlerMmtPlayUrl.cpp 5884 2021-04-18 09:46:37Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-18 09:46:37  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include "../../Logs.h"
#include "../../Solar.h"

#include <ez_rtmpUtil.h>
#include <uniFace/uniFacePP.h>

#include "../Stack/promQL/promQLStack.h"

#include "CrawlerMmtPlayUrl.h"
#include "ManagerCrawler.h"

#define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>


#define def_ErrMsg_invalidPlayUrl -2

CCrawlerMmtPlayUrl::CCrawlerMmtPlayUrl()
{
    m_strCrawlId = "";

    m_ttChkPointRtmp = 0;
    m_iChkResultRtmp = def_ErrMsg_invalidPlayUrl;// 置为播放地址失败
    m_ttChkPointRtmpCuted = 0;
}

CCrawlerMmtPlayUrl::~CCrawlerMmtPlayUrl()
{}

EZTHREAD_BOOL CCrawlerMmtPlayUrl::Start(CEZObject * pObj)
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    if(pObj)
    {
        // m_pCrawlerManager = dynamic_cast<CManagerCrawler *>(pObj);
        m_pCrawlerManager = (CManagerCrawler *)(pObj);
    }

    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CCrawlerMmtPlayUrl::Stop(CEZObject * pObj)
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

void CCrawlerMmtPlayUrl::ThreadProc()
{
    while (m_bLoop)
    {
        int periodCrawlerPlayUrl = m_pCrawlerManager->m_pCfgCrawler->getConfig().periodCrawlerPlayUrl;

        if (periodCrawlerPlayUrl < 1)
        {
            break;
        }

        // 第一次的检测快速推进
        if (m_ttChkPointRtmp == 0)
        {
            sleep(1);
        }
        else
        {
            sleep(periodCrawlerPlayUrl);
        }

        // 数据已经取， 检测
        if (m_ttChkPointRtmpCuted == m_ttChkPointRtmp)
        {
            time_t timeCutted = time(NULL) - m_ttChkPointRtmpCuted; // 已经逝去了多久
            if (timeCutted < periodCrawlerPlayUrl)
            {
                sleep(periodCrawlerPlayUrl - timeCutted);
            }

            while (!m_pCrawlerManager->canCrawlerMmtPlayUrl())
            {
                sleep(1);
            }

            m_pCrawlerManager->onCrawlerMmtPlayUrl_DoingNums();
            doCrawl();
            m_pCrawlerManager->offCrawlerMmtPlayUrl_DoingNums();
        }

        // if (m_ttChkPointRtmp == 0)
        // {
        //     sleep(periodCrawlerPlayUrl);
        // }
    }
}

void CCrawlerMmtPlayUrl::doCrawl()
{
    if (NULL == m_pCrawlerManager)
    {
        ez_printf_error("m_pCrawlerManager needed!\n");
        return;
    }

    ez_printf_debug("doCrawl:%s\n", m_strCrawlId.c_str());

    // 置为不能播放
    // m_ttChkPointRtmp = time(NULL);
    // 默认地址获取不到
    m_iChkResultRtmp = def_ErrMsg_invalidPlayUrl;

    do
    {
        sleep(3);

        const std::string endPoint = m_pCrawlerManager->m_pCfgCrawler->getConfig().urlCrawlerPlayUrl;
        char *pAppKey = NULL;
        char *pAppSecret = NULL;
        std::string strResponse;

        ez_printf_debug("mmtGetPlayUrl:%s\n", m_strCrawlId.c_str());
        int iret_get = uniFace_mmtGetPlayUrl(endPoint.c_str(), pAppKey, pAppSecret, m_strCrawlId.c_str(), strResponse);
        // ARG_USED(iret_get);
        if (iret_get < 0)
        {
            // m_iChkResultRtmp = -2;// 使用默认值
            ez_printf_error("uniFace_mmtGetPlayUrl failed:%d\n", iret_get);
            break;
        }

        // ez_printf_debug("strResponse:%s\n", strResponse.c_str());

        CpromQLStack __uStack;
        bool bRet = __uStack.Parse(strResponse);

        if (false == bRet)
        {
            ez_printf_error("__uStack.Parse failed.\n");
            break;
        }

        CMmtMsgPlayUrlList MsgPlayUrlList;
        bool bDecode = __uStack.Decode(MsgPlayUrlList);

        if (!bDecode)
        {
            ez_printf_error("__uStack.Decode failed.\n");
            break;
        }

        if (MsgPlayUrlList.Result != 0)
        {
            ez_printf_error("MsgPlayUrlList.Result(%d) != 0\n", MsgPlayUrlList.Result);
            break;
        }

        std::string urlRtsp = "";
        std::string urlRtmp = "";
        std::string urlFlv = "";
        std::string urlHls = "";
        for (std::list<CMmtMsgPlayUrl>::iterator ciUrl = MsgPlayUrlList.UrlList.begin(); ciUrl != MsgPlayUrlList.UrlList.end(); ++ciUrl)
        {
            // ciUrl->dump();
            if ("Rtsp" == ciUrl->Type)
            {
                urlRtsp = ciUrl->Url;
            }
            else if ("Rtmp" == ciUrl->Type)
            {
                urlRtmp = ciUrl->Url;
            }
            else if ("Hls" == ciUrl->Type)
            {
                urlHls = ciUrl->Url;
            }
            else if ("Flv" == ciUrl->Type)
            {
                urlFlv = ciUrl->Url;
            }
        }

        // ez_printf_debug("%s:%s\n", "urlRtsp", urlRtsp.c_str());
        ez_printf_debug("%s(%s):%s\n", "urlRtmp", m_strCrawlId.c_str(), urlRtmp.c_str());
        // ez_printf_debug("%s:%s\n", "urlFlv", urlFlv.c_str());
        // ez_printf_debug("%s:%s\n", "urlHls", urlHls.c_str());

        int timeout = 4;
        int duration = 35; //20是播放地址有效期

        // 关键数据检查
        if (urlRtmp.length() < sizeof("rtmp://0.0.0.0"))
        {
            ez_printf_error("Invalid urlRtmp:%s\n", urlRtmp.c_str());
            break;
        }

        unsigned long ulStartPoint = g_Solar.GetRunPeriod();
        unsigned long ulNowPoint = g_Solar.GetRunPeriod();

        do
        {
            // 1s后再试
            sleep(1);

            int iret_ez_detect_rtmp = ez_detect_rtmp(urlRtmp.c_str(), timeout, duration);
            ez_printf_debug("ez_detect_rtmp(%d):%s,%s\n", iret_ez_detect_rtmp, m_strCrawlId.c_str(), iret_ez_detect_rtmp == 0 ? "Succeed" : "Failed");

            if (0 == iret_ez_detect_rtmp)
            {
                //OK
                // m_ttChkPointRtmp = time(NULL);
                m_iChkResultRtmp = 1;
                break;
            }
            else
            {
                m_iChkResultRtmp = 0;
            }

            ulNowPoint = g_Solar.GetRunPeriod();

            // ez_printf_debug("Waite Next:%ld, %ld, %ld, %s, %s\n", ulNowPoint, ulStartPoint, duration, m_strCrawlId.c_str(), urlRtmp.c_str());
            ez_printf_debug("Next(%d), %s, %s\n", (ulNowPoint > ulStartPoint + duration), m_strCrawlId.c_str(), urlRtmp.c_str());

        } while (ulNowPoint > ulStartPoint + duration);
#if 1
        unsigned long ulCheckUsetime = 5;
        unsigned long ulEndPoint = g_Solar.GetRunPeriod(); // 结束时间
        if (ulStartPoint + ulCheckUsetime > ulEndPoint)
        {
            ez_printf_debug("%s sleep:%ld\n", m_strCrawlId.c_str(), ulStartPoint + ulCheckUsetime - ulEndPoint);
            sleep(ulStartPoint + ulCheckUsetime - ulEndPoint);
        }
#endif
    } while (0);

    // 放在其他地方赋值造成不同步： 上述的检测耗时，造成检测的时候获取到了不成功的结果
    m_ttChkPointRtmp = time(NULL);
}

bool CCrawlerMmtPlayUrl::isCrawed(std::string strCrawlId) // 判断是否已经在内
{
    return m_strCrawlId==strCrawlId;
}

void CCrawlerMmtPlayUrl::setCrawlId(const std::string strCrawlId) // 
{
    m_strCrawlId=strCrawlId;

    return;
}

// void set_ttPointRtmpOK(time_t ttPointRtmpOK)
// {
//     CEZLock __lock(m_MutexttPointRtmpOK);
//     m_ttPointRtmpOK = ttPointRtmpOK;
// }

int  CCrawlerMmtPlayUrl::cutPlayUrlChkResult(CMmtMsgValueCamDialing &PlayUrlChkResult)
{
    // ez_printf_debug("On:%s\n", __FUNCTION__);

    // ez_printf_debug("m_ttChkPointRtmpCuted:%ld, m_ttChkPointRtmp:%ld, m_iChkResultRtmp:%d\n", m_ttChkPointRtmpCuted, m_ttChkPointRtmp, m_iChkResultRtmp);

    // 还没有检测
    if (m_ttChkPointRtmp < 100)
        return -1;

    // 说明没有上新
    if (m_ttChkPointRtmpCuted == m_ttChkPointRtmp)
        return -1;

    ez_printf_debug("m_ttChkPointRtmpCuted:%ld, m_ttChkPointRtmp:%ld, m_iChkResultRtmp:%d\n", m_ttChkPointRtmpCuted, m_ttChkPointRtmp, m_iChkResultRtmp);

    m_ttChkPointRtmpCuted = m_ttChkPointRtmp;

    // 获取播放地址失败的， 不返回
    if (m_iChkResultRtmp == def_ErrMsg_invalidPlayUrl)
        return -1;

    PlayUrlChkResult.DevId = m_strCrawlId;
    CDialingResult DialingResult;

    DialingResult.m_strResult = "play_not_checked";
    DialingResult.m_strResultMsg="";
    DialingResult.m_strTime=m_ttChkPointRtmp;

    // CDialingResult DialingResult;
    if (m_iChkResultRtmp == 1)
    {
        DialingResult.m_strResult="play_ok";

        CDialingResult VideoChkResult;
        VideoChkResult.m_strResult = "video_ok";
        VideoChkResult.m_strResultMsg = "";
        VideoChkResult.m_strTime = DialingResult.m_strTime;
        PlayUrlChkResult.DialingResults.push_back(VideoChkResult);
    }
    else if (m_iChkResultRtmp == 0)
    {
        DialingResult.m_strResult="play_failed";
    }

    PlayUrlChkResult.DialingResults.push_back(DialingResult);

    return 0;
}

int  CCrawlerMmtPlayUrl::havePlayUrlChkResult()//检查还有多少新的检测结果
{
    // 说明没有上新
    if (m_ttChkPointRtmpCuted == m_ttChkPointRtmp)
        return 0;

    return 1;
}
