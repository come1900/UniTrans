/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigCrawler.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigCrawler.h 5884 2014-10-20 08:59:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-20 08:59:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigCrawler_H__
#define __ConfigCrawler_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigCrawler
{
    int periodCrawlerPromQL;// 周期
    std::string urlCrawlerPromQL;
    int periodCrawlerPlayUrl;// 周期
    std::string urlCrawlerPlayUrl;
    int CrawlerPlayUrlParallelNums;//并行检测数量
}
ConfigCrawler;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigCrawler, 1, 4> CConfigCrawler;

#endif //__ConfigCrawler_H__

