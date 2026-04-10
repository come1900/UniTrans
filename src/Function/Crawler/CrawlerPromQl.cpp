/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CrawlerPromQl.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CrawlerPromQl.cpp 5884 2021-04-18 09:46:37Z WuJunjie $
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

#include <uniFace/uniFacePP.h>
#include "CrawlerPromQl.h"
#include "ManagerCrawler.h"

#define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

CCrawlerPromQl::CCrawlerPromQl()
{}

CCrawlerPromQl::~CCrawlerPromQl()
{}

EZTHREAD_BOOL CCrawlerPromQl::Start(CEZObject * pObj)
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

EZTHREAD_BOOL CCrawlerPromQl::Stop(CEZObject * pObj)
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }
    return EZTHREAD_BOOL_TRUE;
}

void CCrawlerPromQl::ThreadProc()
{
    while (m_bLoop)
    {
        doCrawl();

        sleep(5);
    }
}

void CCrawlerPromQl::doCrawl()
{
    if (NULL == m_pCrawlerManager)
    {
        ez_printf_error("m_pCrawlerManager needed!\n");
        return;
    }

    std::string theQuer = "";
    std::string querRet = "";
    int iret = 0;
    // # 6.3.1	up
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("up");
        iret = uniFace_Promql_get(theQuer, querRet);
        //ez_printf_debug("uniFace_Promql_get(%d):[UP][%s]\n", iret, theQuer.c_str());
        //ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_up(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }

    // # 6.3.1	CPU 使用率
    // # theQuer = '100 * (1 - sum by (instance)(increase(node_cpu_seconds_total{mode="idle"}[5m])) / sum by (instance)(increase(node_cpu_seconds_total[5m])))'
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("100*(1-sum+by(instance)(increase(node_cpu_seconds_total{mode='idle'}[5m]))%2Fsum(increase(node_cpu_seconds_total[5m]))by(instance))");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_cpu_usage(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }

    // # 6.3.2	内存使用率, 用一条如下命令查询出问题， 分开查询自行计算, Why??-- 触类旁通：+号在url里面表示空格
    // ###### theQuer = '100 - ((node_memory_MemFree_bytes+node_memory_Cached_bytes+node_memory_Buffers_bytes)/node_memory_MemTotal_bytes) * 100'
    // # 将 + 号转义：%2B
    // # theQuer = '100 - ((node_memory_MemFree_bytes%2Bnode_memory_Cached_bytes%2Bnode_memory_Buffers_bytes)/node_memory_MemTotal_bytes) * 100'
    // # theQuer = 'node_memory_MemFree_bytes'
    // # theQuer = 'node_memory_Cached_bytes'
    // # theQuer = 'node_memory_Buffers_bytes'
    // # theQuer = 'node_memory_MemTotal_bytes'
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("100-((node_memory_MemFree_bytes%2Bnode_memory_Cached_bytes%2Bnode_memory_Buffers_bytes)/node_memory_MemTotal_bytes)*100");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_mem_usage(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    { // 
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("node_memory_MemTotal_bytes");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_mem_total(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    { // 
        // theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("node_memory_MemAvailable_bytes");
        // theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("node_memory_MemFree_bytes");
        // centos6和centos7下内存监控指标采集计算公式不相同，最后采用统一计算方法
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("node_memory_MemFree_bytes%2Bnode_memory_Cached_bytes%2Bnode_memory_Buffers_bytes");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_mem_available(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    // # 6.3.3	磁盘使用率--- ing
    // # theQuer = '100*(1-  sum by (instance)(node_filesystem_free_bytes{fstype!~"rootfs|selinuxfs|autofs|rpc_pipefs|tmpfs|udev|none|devpts|sysfs|debugfs|fuse.*"}) / sum by (instance)(node_filesystem_size_bytes{fstype!~"rootfs|selinuxfs|autofs|rpc_pipefs|tmpfs|udev|none|devpts|sysfs|debugfs|fuse.*"}) )'
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("100*(1-sum(node_filesystem_free_bytes{fstype!~\"rootfs|selinuxfs|autofs|rpc_pipefs|tmpfs|udev|none|devpts|sysfs|debugfs|fuse.*\"})by(instance)/sum(node_filesystem_size_bytes{fstype!~\"rootfs|selinuxfs|autofs|rpc_pipefs|tmpfs|udev|none|devpts|sysfs|debugfs|fuse.*\"})by(instance))");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_disk_usage(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("sum(node_filesystem_size_bytes{fstype!~\"rootfs|selinuxfs|autofs|rpc_pipefs|tmpfs|udev|none|devpts|sysfs|debugfs|fuse.*\"})by(instance)");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_disk_total(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("sum(node_filesystem_free_bytes{fstype!~\"rootfs|selinuxfs|autofs|rpc_pipefs|tmpfs|udev|none|devpts|sysfs|debugfs|fuse.*\"})by(instance)");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_disk_available(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    // # 6.3.4	上行带宽
    // # theQuer = 'sum by (instance) (irate(node_network_receive_bytes_total{device!~"bond.*?|lo"}[5m])/128)'
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("sum(irate(node_network_receive_bytes_total{device!~\"bond.*?|lo\"}[5m])/128)by(instance)");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_network_upperrate(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
    // # 6.3.5	下行带宽
    // # theQuer = 'sum by (instance) (irate(node_network_transmit_bytes_total{device!~"bond.*?|lo"}[5m])/128)'
    {
        theQuer = m_pCrawlerManager->m_strCfgHostCrawlerPromQL + "/api/v1/query?query=" + std::string("sum(irate(node_network_transmit_bytes_total{device!~\"bond.*?|lo\"}[5m])/128)by(instance)");
        iret = uniFace_Promql_get(theQuer, querRet);
        // ez_printf_debug("uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        // ez_printf_debug("querRet:%s\n", querRet.c_str());
        if (0 == iret && querRet.size() > 5)
        {
            m_pCrawlerManager->setHost_network_downwardrate(querRet);
        }
        else
        {
            ez_printf_error("Failed uniFace_Promql_get(%d):%s\n", iret, theQuer.c_str());
        }
    }
}
