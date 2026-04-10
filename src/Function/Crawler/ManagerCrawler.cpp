/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ManagerCrawler.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ManagerCrawler.cpp 5884 2013-12-26 02:44:42Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 02:44:42 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h> // sleep()

#include "../../Logs.h"
#include "../../Solar.h"

#include "../../Configs/ConfigGeneral.h"

#include "Crawler.h"
#include "CrawlerPromQl.h"

#include "ManagerCrawler.h"

// #define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

#define g_ManagerCrawler (*CManagerCrawler::instance())

PATTERN_SINGLETON_IMPLEMENT(CManagerCrawler);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CManagerCrawler::CManagerCrawler() :CEZThread("CManagerCrawler", THREAD_PRIORITY_DEFAULT)
{
    printf("CManagerCrawler Enter--------\n");

    // 视频检测
#if defined(_FUNC_CrawlerMmtPlayUrl)
    m_pCacheWithDb_CrawlerMmtPlayUrl = NULL;
    m_iCrawlerMmtPlayUrl_DoingNums=0;
#endif // (_FUNC_CrawlerMmtPlayUrl)

#if defined(_FUNC_CrawlerPromQL)

    m_strCfgHostCrawlerPromQL = "http://192.168.9.99:60090";

    m_strHost_cpu_usage = "";            // 主机类-CPU使用率
    m_strHost_mem_usage = "";            // 主机类-内存使用率
    m_strHost_mem_total = "";
    m_strHost_mem_available = "";

    m_strHost_disk_usage = "";           // 主机类-磁盘使用率
    m_strHost_disk_total = "";           // 主机类-磁盘总量
    m_strHost_disk_available = "";           // 主机类-磁盘使用量

    m_strHost_network_upperrate = "";    // 主机类-上行速率
    m_strHost_network_downwardrate = ""; // 主机类-下行速率
    m_strHost_up = "";                   // 主机类-
#endif // (_FUNC_CrawlerPromQL)
}

CManagerCrawler::~CManagerCrawler()
{
    // printf("CManagerCrawler Leave--------\n");
#if defined(_FUNC_CrawlerMmtPlayUrl)
    if (NULL == m_pCacheWithDb_CrawlerMmtPlayUrl)
    {
        delete m_pCacheWithDb_CrawlerMmtPlayUrl;
        m_pCacheWithDb_CrawlerMmtPlayUrl = NULL;
    }
#endif // (_FUNC_CrawlerMmtPlayUrl)

}

EZTHREAD_BOOL CManagerCrawler::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

#if defined(_FUNC_Crawler)
    static CConfigCrawler __cfg_Crawler;
    __cfg_Crawler.update();
    m_pCfgCrawler = &__cfg_Crawler;

    m_strCfgHostCrawlerPromQL = m_pCfgCrawler->getConfig().urlCrawlerPromQL;
    m_iCrawlerPlayUrlParallelNums = m_pCfgCrawler->getConfig().CrawlerPlayUrlParallelNums;
#endif // _FUNC_CrawlerPromQL

#if defined(_FUNC_CrawlerMmtPlayUrl)
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    if (m_pCacheWithDb_CrawlerMmtPlayUrl)
    {
        delete m_pCacheWithDb_CrawlerMmtPlayUrl;
        m_pCacheWithDb_CrawlerMmtPlayUrl = NULL;
    }
    if (NULL == m_pCacheWithDb_CrawlerMmtPlayUrl)
    {
        m_pCacheWithDb_CrawlerMmtPlayUrl = new CCacheKeyValue;
        assert(m_pCacheWithDb_CrawlerMmtPlayUrl);

        m_pCacheWithDb_CrawlerMmtPlayUrl->setDbfilename(__cfgGeneral.getConfig().strDataFilePath + std::string("/") + def_dbFileKeyValue_CrawlerMmtPlayUrl);
    }
#endif // (_FUNC_CrawlerMmtPlayUrl)


    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CManagerCrawler::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

void CManagerCrawler::ThreadProc()
{

    while (m_bLoop)
    {
#if defined(_FUNC_CrawlerMmtPlayUrl)
        maintainCrawlerMmtPlayUrl();
#endif//_FUNC_CrawlerMmtPlayUrl

#if defined(_FUNC_CrawlerPromQL)
        startCrawlerPromQL(std::string("IIDI"));
#endif//_FUNC_CrawlerPromQL
        sleep(1);
    }
}
#if defined(_FUNC_CrawlerMmtPlayUrl)
#define def_CacheWithDb_CrawlerMmtPlayUrl_Key             "_9f12ef198c5a5e662fe926f6cad30db8bf30e21e" // 此标识为主key// sha14str CrawlerMmtPlayUrl
#define def_CacheWithDb_CrawlerMmtPlayUrl_Key_DevId          "_DevId"

// -2 -- 已经在了
int CManagerCrawler::startCrawlerMmtPlayUrl(std::string strDevId)
{
    CCrawlerMmtPlayUrl *pCrawler = NULL;
    for (def_CrawlerContainer::iterator ci = m_lstCrawlerMmtPlayUrl.begin(); ci != m_lstCrawlerMmtPlayUrl.end(); ++ci)
    {
        if ((*ci)->isCrawed(strDevId))
        {
            pCrawler = dynamic_cast<CCrawlerMmtPlayUrl *>(*ci);

            break;
        }
    }

    // 没有找到
    if (pCrawler == NULL)
    {
        ez_printf_debug("New:%s\n", strDevId.c_str());

        pCrawler = new CCrawlerMmtPlayUrl;
        assert(pCrawler);
        pCrawler->setCrawlId(strDevId);

        std::string strThrName = PREFIX_CCrawlerMmtPlayUrl_THREAD_NAME + strDevId/*strTykdDevID*/;
        pCrawler->SetThreadName(strThrName.c_str());

        pCrawler->Start(this);

        addCrawlerMmtPlayUrl(pCrawler);

        m_pCacheWithDb_CrawlerMmtPlayUrl->modPair(strDevId+def_CacheWithDb_CrawlerMmtPlayUrl_Key, strDevId, modPair_If_Not_Exist_Then_Add);
    }
    else
    {
        return -2; // -2 -- 已经在了
    }

    // addCrawlerMmtPlayUrl()
    return 0;
}

#define def_ReturnMmtMsgValuesIpcamDialingItems 20// 一次返回条数

int CManagerCrawler::cutMmtMsgValuesIpcamDialing(CMmtMsgValuesIpcamDialingAck &MmtMsgValuesIpcamDialing)
{
    MmtMsgValuesIpcamDialing.Items = 0;
    MmtMsgValuesIpcamDialing.RemainingItems = 0;

    def_CrawlerContainer::iterator ci = m_lstCrawlerMmtPlayUrl.begin();

    for (ci = m_lstCrawlerMmtPlayUrl.begin(); ci != m_lstCrawlerMmtPlayUrl.end(); ++ci)
    {
        CCrawlerMmtPlayUrl *pCrawler = dynamic_cast<CCrawlerMmtPlayUrl *>(*ci);
        if (pCrawler)
        {
            MmtMsgValuesIpcamDialing.RemainingItems += pCrawler->havePlayUrlChkResult();
        }
    } // for

    for (ci = m_lstCrawlerMmtPlayUrl.begin(); ci != m_lstCrawlerMmtPlayUrl.end(); ++ci)
    {
        CCrawlerMmtPlayUrl *pCrawler = dynamic_cast<CCrawlerMmtPlayUrl *>(*ci);
        if (pCrawler)
        {
            CMmtMsgValueCamDialing PlayUrlChkResult;
            int iretCutResult = pCrawler->cutPlayUrlChkResult(PlayUrlChkResult);

            if (0 == iretCutResult)
            {
                MmtMsgValuesIpcamDialing.ValuesIpcamDialing.push_back(PlayUrlChkResult);
                MmtMsgValuesIpcamDialing.Items++;
                MmtMsgValuesIpcamDialing.RemainingItems--;
            }

            if (MmtMsgValuesIpcamDialing.Items >= def_ReturnMmtMsgValuesIpcamDialingItems)
            {
                break;
            }
        }
    }

    // 防止取用不及时
    m_lstCrawlerMmtPlayUrl.reverse(); // 反转

    if (MmtMsgValuesIpcamDialing.RemainingItems < 0)
    {
        ez_printf_error("MmtMsgValuesIpcamDialing.RemainingItems:%d\n", MmtMsgValuesIpcamDialing.RemainingItems);

        MmtMsgValuesIpcamDialing.RemainingItems = 0;
    }

    return 0;
}

int CManagerCrawler::ListCrawlerMmtPlayUrl(std::string &strDump, const int iLevel)
{
    char sendto[1024];

    size_t numOfCrawlerMmtPlayUrl = 0;

    for (def_CrawlerContainer::iterator ci = m_lstCrawlerMmtPlayUrl.begin(); ci != m_lstCrawlerMmtPlayUrl.end(); ++ci)
    {
        CCrawlerMmtPlayUrl *pCrawler = dynamic_cast<CCrawlerMmtPlayUrl *>(*ci);
        if (pCrawler)
        {
            numOfCrawlerMmtPlayUrl++;
        }
    }

    snprintf(sendto, sizeof (sendto)-1, "HandlerCuOverHttp Socket List\n");
    strDump = sendto;
    snprintf(sendto, sizeof (sendto)-1, "---------------------------------------------------\n");

    return 0;
}

int CManagerCrawler::maintainCrawlerMmtPlayUrl()
{
    static int __maintainCrawlerMmtPlayUrl_first_run = 1;
    if (1 == __maintainCrawlerMmtPlayUrl_first_run)
    {
        __maintainCrawlerMmtPlayUrl_first_run=0;
        int m_iStartPullAuto = 1;

        if (1==m_iStartPullAuto // 配置了自动启动
            && NULL != m_pCacheWithDb_CrawlerMmtPlayUrl)
        {
            def_uniKeyVector __val;
            // 获取id
            m_pCacheWithDb_CrawlerMmtPlayUrl->getValueLike(__val, def_CacheWithDb_CrawlerMmtPlayUrl_Key);
            def_uniKeyVector::const_iterator it = __val.begin();
            for ( ; it != __val.end() ; it++)
            {
                DBG(
                    std::cout << "::::::::" << *it << std::endl;
                );
            }
        }
    }

    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
bool CManagerCrawler::canCrawlerMmtPlayUrl()
{
    if (m_iCrawlerPlayUrlParallelNums == 0)
        return true;

    // ez_printf_debug("m_iCrawlerMmtPlayUrl_DoingNums:%d < m_iCrawlerPlayUrlParallelNums:%d\n", m_iCrawlerMmtPlayUrl_DoingNums, m_iCrawlerPlayUrlParallelNums);

    return m_iCrawlerMmtPlayUrl_DoingNums < m_iCrawlerPlayUrlParallelNums;
}

void CManagerCrawler::onCrawlerMmtPlayUrl_DoingNums(CCrawlerMmtPlayUrl *pCrawler)
{
    CEZLock __lock(m_MutexCrawlerMmtPlayUrl_DoingNums);
    m_iCrawlerMmtPlayUrl_DoingNums++;
}
void CManagerCrawler::offCrawlerMmtPlayUrl_DoingNums(CCrawlerMmtPlayUrl *pCrawler)
{
    CEZLock __lock(m_MutexCrawlerMmtPlayUrl_DoingNums);

    m_iCrawlerMmtPlayUrl_DoingNums--;

    if (m_iCrawlerMmtPlayUrl_DoingNums < 0)
    {
        ez_printf_error("DoingNums:%d\n", m_iCrawlerMmtPlayUrl_DoingNums);
    }
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int CManagerCrawler::addCrawlerMmtPlayUrl(CCrawlerMmtPlayUrl *pCrawler)
{
    CEZLock __lock(m_MutexCrawlerMmtPlayUrl);
    m_lstCrawlerMmtPlayUrl.push_back(pCrawler);

    return 0;
}
#endif // _FUNC_CrawlerMmtPlayUrl

#if defined(_FUNC_CrawlerPromQL)
int CManagerCrawler::startCrawlerPromQL(std::string strId)
{
    static std::string strIdStarted="";

    if (strIdStarted != strId)
    {
        strIdStarted = strId;

        CCrawlerPromQl *pCrawler = new CCrawlerPromQl;
        pCrawler->Start(this);
        addCrawlerPromQL(pCrawler);
    }
    else
    {
        // __fline;
        // printf("dbg ...\n");
    }

    return 0;
}

int CManagerCrawler::addCrawlerPromQL(CCrawlerPromQl *pCrawler)
{
    if (NULL == pCrawler)
    {
        return -1;
    }

    CEZLock __lock(m_MutexCrawlerPromQL);
    m_lstCrawlerPromQL.push_back(pCrawler);

    return -1;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
const std::string &CManagerCrawler::getHost_cpu_usage(std::string &Host_cpu_usage)
{
    CEZLock __lock(m_MutexHost);
    Host_cpu_usage = m_strHost_cpu_usage;
    return Host_cpu_usage;
}
const std::string &CManagerCrawler::getHost_mem_usage(std::string &Host_mem_usage)
{
    CEZLock __lock(m_MutexHost);
    Host_mem_usage = m_strHost_mem_usage;
    return Host_mem_usage;
}
const std::string &CManagerCrawler::getHost_mem_total(std::string &Host_mem_total)
{
    CEZLock __lock(m_MutexHost);
    Host_mem_total = m_strHost_mem_total;
    return Host_mem_total;
}
const std::string &CManagerCrawler::getHost_mem_available(std::string &Host_mem_available)
{
    CEZLock __lock(m_MutexHost);
    Host_mem_available = m_strHost_mem_available;
    return Host_mem_available;
}
const std::string &CManagerCrawler::getHost_disk_usage(std::string &Host_disk_usage)
{
    CEZLock __lock(m_MutexHost);
    Host_disk_usage = m_strHost_disk_usage;
    return Host_disk_usage;
}
const std::string &CManagerCrawler::getHost_disk_total(std::string &Host_disk_total)
{
    CEZLock __lock(m_MutexHost);
    Host_disk_total = m_strHost_disk_total;
    return Host_disk_total;
}
const std::string &CManagerCrawler::getHost_disk_available(std::string &Host_disk_available)
{
    CEZLock __lock(m_MutexHost);
    Host_disk_available = m_strHost_disk_available;
    return Host_disk_available;
}

const std::string &CManagerCrawler::getHost_network_upperrate(std::string &Host_network_upperrate)
{
    CEZLock __lock(m_MutexHost);
    Host_network_upperrate = m_strHost_network_upperrate;
    return Host_network_upperrate;
}
const std::string &CManagerCrawler::getHost_network_downwardrate(std::string &Host_network_downwardrate)
{
    CEZLock __lock(m_MutexHost);
    Host_network_downwardrate = m_strHost_network_downwardrate;
    return Host_network_downwardrate;
}
const std::string &CManagerCrawler::getHost_up(std::string &Host_up)
{
    CEZLock __lock(m_MutexHost);
    Host_up = m_strHost_up;
    return Host_up;
}
int CManagerCrawler::setHost_cpu_usage(const std::string &Host_cpu_usage)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_cpu_usage = Host_cpu_usage;
    return 0;
}

int CManagerCrawler::setHost_mem_usage(const std::string &Host_mem_usage)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_mem_usage = Host_mem_usage;
    return 0;
}
int CManagerCrawler::setHost_mem_total(const std::string &Host_mem_total)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_mem_total = Host_mem_total;
    return 0;
}
int CManagerCrawler::setHost_mem_available(const std::string &Host_mem_available)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_mem_available = Host_mem_available;
    return 0;
}
int CManagerCrawler::setHost_disk_usage(const std::string &Host_disk_usage)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_disk_usage = Host_disk_usage;
    return 0;
}
int CManagerCrawler::setHost_disk_total(const std::string &Host_disk_total)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_disk_total = Host_disk_total;
    return 0;
}
int CManagerCrawler::setHost_disk_available(const std::string &Host_disk_available)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_disk_available = Host_disk_available;
    return 0;
}
int CManagerCrawler::setHost_network_upperrate(const std::string &Host_network_upperrate)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_network_upperrate = Host_network_upperrate;
    return 0;
}
int CManagerCrawler::setHost_network_downwardrate(const std::string &Host_network_downwardrate)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_network_downwardrate = Host_network_downwardrate;
    return 0;
}
int CManagerCrawler::setHost_up(const std::string &Host_up)
{
    CEZLock __lock(m_MutexHost);
    m_strHost_up = Host_up;
    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif //_FUNC_CrawlerPromQL
