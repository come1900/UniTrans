/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigCrawler.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigCrawler.cpp 5884 2014-10-20 08:59:01Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-20 08:59:01  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigCrawler.h"

template<> void exchangeTable<ConfigCrawler>(CConfigExchange& xchg, CConfigTable& table, ConfigCrawler& config, int index, int app)
{
    xchg.exchange(table, "periodCrawlerPromQL"    , config.periodCrawlerPromQL,       0, 3600 , 20); // 0 - do not start
    xchg.exchange(table, "urlCrawlerPromQL"       , config.urlCrawlerPromQL    , "http://192.168.9.99:60090");
    xchg.exchange(table, "periodCrawlerPlayUrl"   , config.periodCrawlerPlayUrl,       0, 3600 , 600); // 0 - do not start
    xchg.exchange(table, "urlCrawlerPlayUrl"      , config.urlCrawlerPlayUrl   , "http://4.49.7.21:9000/third/getPlayUrl");
    xchg.exchange(table, "CrawlerPlayUrlParallelNums"   , config.CrawlerPlayUrlParallelNums,       0, 65535 , 20); // 0 - do not start
}
