/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ManagerCrawler.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ManagerCrawler.h 5884 2013-12-26 02:44:48Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 02:44:48 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_StreamPusherTykd_H)
#define _StreamPusherTykd_H

#include "EZThread.h"
#include "EZMutex.h"

#include <string>

//数据持久化
#include "../../DBAgent/CacheKeyValue.h"

#if defined(_FUNC_Crawler)
#include "../../Configs/ConfigCrawler.h"
#endif // _FUNC_Crawler

#include "Crawler.h"
#if defined(_FUNC_CrawlerMmtPlayUrl)
#include "CrawlerMmtPlayUrl.h"
#endif //MmtPlayUrl

#include "CrawlerPromQl.h"

#define g_ManagerCrawler (*CManagerCrawler::instance())

class CManagerCrawler : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CManagerCrawler);

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

    CConfigCrawler *m_pCfgCrawler;

    // 视频检测
#if defined(_FUNC_CrawlerMmtPlayUrl)
public:
    int startCrawlerMmtPlayUrl(std::string strDevId);
    int cutMmtMsgValuesIpcamDialing(CMmtMsgValuesIpcamDialingAck &MmtMsgValuesIpcamDialing);

    int ListCrawlerMmtPlayUrl(std::string &strDump, const int iLevel);

    std::string m_strCfgHostCrawlerMmtPlayUrl;

    // 检测前计数
    void onCrawlerMmtPlayUrl_DoingNums(CCrawlerMmtPlayUrl *pCrawler=NULL);
    // 检测后清除
    void offCrawlerMmtPlayUrl_DoingNums(CCrawlerMmtPlayUrl *pCrawler=NULL);
    // 申请开始执行检测
    bool canCrawlerMmtPlayUrl();

private:
    int addCrawlerMmtPlayUrl(CCrawlerMmtPlayUrl *pCrawler);
    CEZMutex m_MutexCrawlerMmtPlayUrl;
    def_CrawlerContainer m_lstCrawlerMmtPlayUrl;

    // 正在检测的任务数量
    CEZMutex m_MutexCrawlerMmtPlayUrl_DoingNums;
    int m_iCrawlerMmtPlayUrl_DoingNums;
    int m_iCrawlerPlayUrlParallelNums;
    def_CrawlerContainer m_lstCrawlerMmtPlayUrlOnDuty;

    // 数据持久化
    CCacheKeyValue *m_pCacheWithDb_CrawlerMmtPlayUrl;
    // 数据维护、第一次拉起等
    int maintainCrawlerMmtPlayUrl();

#endif //_FUNC_CrawlerMmtPlayUrl

    // 服务器运维
#if defined(_FUNC_CrawlerPromQL)
public:
    int startCrawlerPromQL(std::string strId);
    std::string m_strCfgHostCrawlerPromQL;

private:
    //
    int addCrawlerPromQL(CCrawlerPromQl *pCrawler);
    CEZMutex m_MutexCrawlerPromQL;
    def_CrawlerContainer m_lstCrawlerPromQL;

    // 数据缓存，保留最新
    CEZMutex m_MutexHost;
    std::string m_strHost_up;                   // 主机类-在线
    std::string m_strHost_cpu_usage;            // 主机类-CPU使用率
    // 内存
    std::string m_strHost_mem_usage;            // 主机类-内存使用率
    std::string m_strHost_mem_total;            // 主机类-内存总量
    std::string m_strHost_mem_available;            // 主机类-内存使用量
    // 磁盘
    std::string m_strHost_disk_usage;           // 主机类-磁盘使用率
    std::string m_strHost_disk_total;           // 主机类-磁盘总量
    std::string m_strHost_disk_available;           // 主机类-磁盘使用量
    // 网络
    std::string m_strHost_network_upperrate;    // 主机类-上行速率
    std::string m_strHost_network_downwardrate; // 主机类-下行速率
public:
    const std::string &getHost_cpu_usage(std::string &Host_cpu_usage);

    const std::string &getHost_mem_usage(std::string &Host_mem_usage);
    const std::string &getHost_mem_total(std::string &Host_mem_total);
    const std::string &getHost_mem_available(std::string &Host_mem_available);

    const std::string &getHost_disk_usage(std::string &Host_disk_usage);
    const std::string &getHost_disk_total(std::string &Host_disk_total);
    const std::string &getHost_disk_available(std::string &Host_disk_available);

    const std::string &getHost_network_upperrate(std::string &Host_network_upperrate);
    const std::string &getHost_network_downwardrate(std::string &Host_network_downwardrate);
    const std::string &getHost_up(std::string &Host_up);

    int setHost_cpu_usage(const std::string &Host_cpu_usage);
    int setHost_mem_usage(const std::string &Host_mem_usage);
    int setHost_mem_total(const std::string &Host_mem_total);
    int setHost_mem_available(const std::string &Host_mem_available);
    int setHost_disk_usage(const std::string &Host_disk_usage);
    int setHost_disk_total(const std::string &Host_disk_usage);
    int setHost_disk_available(const std::string &Host_disk_available);
    int setHost_network_upperrate(const std::string &Host_network_upperrate);
    int setHost_network_downwardrate(const std::string &Host_network_downwardrate);
    int setHost_up(const std::string &Host_up);

#endif// defined(_FUNC_CrawlerPromQL)
private:
    CManagerCrawler();
    virtual ~CManagerCrawler();

};

#endif // !defined(_StreamPusherTykd_H)

