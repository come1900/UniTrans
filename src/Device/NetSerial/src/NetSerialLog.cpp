/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DoubleOutLog.h.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DoubleOutLog.h.cpp 5884 2012-09-19 11:47:19Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-19 11:47:19  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include "ISocketHandler.h"
#include "NetSerialLog.h"
#include <stdio.h>


void CNetSerialLog::error(ISocketHandler *,Socket *sock,const std::string& call,int err,const std::string& sys_err,loglevel_t lvl)
{
	 return;
	
    if (sock)
    {
        printf("fd %d :: %s: %d %s (%d)\n",
               sock -> GetSocket(),
               call.c_str(),err,sys_err.c_str(),lvl);
    }
    else
    {
        printf("%s: %d %s (%d)\n",
               call.c_str(),err,sys_err.c_str(),lvl);
    }
}



