/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FaceWebs.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: FaceWebs.cpp 5884 2013-07-01 04:32:32Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-01 04:32:32 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*sleep*/
#include <iostream>

#include "../Logs.h"

#include "../Configs/ConfigFaceWebs.h"
#include	"./ez_asp_webs.h"

#include "FaceWebs.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

PATTERN_SINGLETON_IMPLEMENT(CFaceWebs);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CFaceWebs::CFaceWebs() :CEZThread("CFaceWebs", THREAD_PRIORITY_DEFAULT)
{
    DBG(
        ez_trace("CFaceWebs Enter--------\n");
    );
}

CFaceWebs::~CFaceWebs()
{
    DBG(
        ez_trace("CFaceWebs Leave--------\n");
    );
}

EZTHREAD_BOOL CFaceWebs::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

#ifdef XP_FaceWebs

    LOG4CPLUS_INFO(LOG_SOLAR, "FaceWebs starting ...");
    StartSmartWebs();
#endif//XP_FaceWebs

    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CFaceWebs::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

#ifdef XP_FaceWebs

    StopSmartWebs();
#endif//XP_FaceWebs

    return EZTHREAD_BOOL_TRUE;
}
void CFaceWebs::ThreadProc()
{
    while (m_bLoop)
    {
        ez_asp_webs_proc();
    }
}

#ifdef XP_FaceWebs
int CFaceWebs::StartSmartWebs()
{
    CConfigFaceWebs __cfgTmp;
    __cfgTmp.update();

    ez_asp_webs_init(__cfgTmp.getConfig().strDefaultPages.c_str(), __cfgTmp.getConfig().strRootPath.c_str());

    ez_asp_webs_open(__cfgTmp.getConfig().SPort, __cfgTmp.getConfig().iRetries);
    LOG4CPLUS_INFO(LOG_SOLAR, "FaceWebs Listen:" << __cfgTmp.getConfig().SPort<<":"<< __cfgTmp.getConfig().iRetries);

    ez_printf_info("FaceWebs\t\tReady:%d, pages:%s, path:%s\n", __cfgTmp.getConfig().SPort, __cfgTmp.getConfig().strDefaultPages.c_str(), __cfgTmp.getConfig().strRootPath.c_str());

    return 0;
}

int CFaceWebs::StopSmartWebs()
{
    ez_asp_webs_close();

    ez_asp_webs_cleanup();

    return 0;
}
#endif//XP_FaceWebs

