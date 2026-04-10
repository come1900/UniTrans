/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * hello.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: hello.cpp 5884 2012-12-04 02:33:59Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-12-04 02:33:59  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include "../DevLock.h"

#ifndef _DEBUG_THIS
//  #define _DEBUG_THIS
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

int main(int argc, char *argv[])
{
	int a = 0;
	int b=2;

	while (1)
	{
		b = g_DevLock_D1.Lock(a);

		printf("Lock(%d):%s\n", a, b==EnLockLastError_NO?"Succ":"Failed");

		sleep(2);
	}
	
	return 0;
}
