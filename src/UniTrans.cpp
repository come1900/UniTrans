/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UniTrans.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: UniTrans.cpp 5884 2014-02-26 02:21:27Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-26 02:21:27 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "./Console/LocalConsole.h"

#include "Solar.h"

#include "UniTrans.h"

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

int UniTransInitialize(int argc, char *argv[])
{
    // 初始化
DBG(
    printf(">>>>>>>>>>>>> UniTransInitialize\n");
    printf("$ %s,v1 %s %s Exp $\n\n", __FILE__, __DATE__, __TIME__);
);
    return g_Solar.Initialize(argc, argv);
}
int UniTransStart()
{
    return g_LocalConsole.Start();
}

// 最后的调用，
// 完成一些例行维护和阻塞之用
int UniTransDone()
{
    return g_Solar.Done();
}

int Term()
{
    g_Solar.Term();

    return 0;
}

int haveException(int ExcNo, char *pMsg)
{
    g_Solar.haveException(ExcNo, pMsg);

    return 0;
}

