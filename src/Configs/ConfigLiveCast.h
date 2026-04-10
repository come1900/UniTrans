/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveCast.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveCast.h 5884 2016-11-02 09:21:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-11-02 09:21:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigLiveCast_H__
#define __ConfigLiveCast_H__

#include "ConfigBase.h"

#define def_MaxLiveCaster 4
//!
typedef struct tagConfigLiveCast
{
    // issue addr
    std::string strDefaultUrlPrefixRTMP;//分发url
    std::string strDefaultUrlPrefixRTSP;//
    std::string strDefaultUrlPrefixHLS;// 

    // push addr of internet
    std::string strPubPushRTMP;//推流地址
    std::string strPubPushRTSP;//
}
ConfigLiveCast;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigLiveCast, def_MaxLiveCaster, 4> CConfigLiveCast;

#endif //__ConfigLiveCast_H__
