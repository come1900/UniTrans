/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveCast.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveCast.cpp 5884 2016-11-02 09:21:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-02 09:21:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigLiveCast.h"

#include "../UniTransDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */

template<>
void exchangeTable<ConfigLiveCast>(CConfigExchange& xchg, CConfigTable& table, ConfigLiveCast& config, int index, int app)
{
    xchg.exchange(table, "IssueRTMP", config.strDefaultUrlPrefixRTMP, DEFAULT_RTMP_URL);
    xchg.exchange(table, "IssueRTSP", config.strDefaultUrlPrefixRTSP, DEFAULT_RTSP_URL);
    xchg.exchange(table, "IssueHLS", config.strDefaultUrlPrefixHLS, DEFAULT_HLS_URL_HEADER);

    xchg.exchange(table, "PubPushRTMP", config.strPubPushRTMP, DEFAULT_PubPushRTMP_URL);
    xchg.exchange(table, "PubPushRTSP", config.strPubPushRTSP, DEFAULT_PubPushRTSP_URL);
}

