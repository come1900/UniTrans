/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Main.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Main.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      通用软件 main (c++ 版)
 *      Multi Thread
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "Solar.h"

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void signal_proc(int signo);

int main(int argc, char * argv[])
{
    // 打印信息
    printf("main(%d,", argc);
    for(int i = 0; i < argc; i++)
    {
        printf(" %s", argv[i]);
    }
    printf(")\n");

#if 0

    char *pRunMode = argc>1?argv[1]:(char *)"-s";

    if (strcmp(pRunMode, "-d")==0)
    {
        if(daemonize(0, 0) == -1)
        {
            printf("can't fork self \n");
            exit(0);
        }
    }
#endif

    signal(SIGPIPE, signal_proc);
    signal(SIGTERM, signal_proc);
    signal(SIGKILL, signal_proc);
    signal(SIGQUIT, signal_proc);
    signal(SIGINT,  signal_proc);
    signal(SIGUSR1, signal_proc);
    signal(SIGALRM, signal_proc);
    //signal(SIGSEGV, signal_proc);

    signal(SIGABRT, signal_proc);
    signal(SIGFPE, signal_proc);

    signal(SIGCHLD, signal_proc);

    int iret = 0;

    // 初始化
    iret = g_Solar.Initialize(argc, argv);
    if (iret == 0)
    {
    	// ok, go on
    }
    else if (iret < 0)
    {
        __trip;
        printf("Initialize failed(%d)\n", iret);
        exit(-1);
    }
    else// if (iret > 0)
    {
        exit(0);
    }
	
    iret = g_Solar.Start();

    // 最后的调用，
    // 完成一些例行维护和阻塞之用
    iret = g_Solar.Done();

    return 0;
}

void signal_proc(int signo)
{
    if(signo == SIGTERM    \
       || signo == SIGKILL \
       || signo == SIGQUIT \
       || signo == SIGINT  \
      )
    {
        g_Solar.haveException(signo, (char *)"on exit signal");
        g_Solar.Term();
        exit(0);
    }
    else if(signo == SIGALRM)
    {
        g_Solar.haveException(signo, (char *)"on signal:SIGALRM");
    }
    else if(signo == SIGUSR1)
    {
        g_Solar.haveException(signo, (char *)"on exit signal:SIGUSR1");
        g_Solar.Term();
        exit(0);
    }
    else if(signo == SIGPIPE)
    {
        g_Solar.haveException(signo, (char *)"on signal:SIGPIPE");
    }
    else if(signo == SIGABRT) //
    {
        g_Solar.haveException(signo, (char *)"on signal:SIGABRT");
        g_Solar.Term();
        exit(0);
    }
    else if(signo == SIGIOT)
    {
        g_Solar.haveException(signo, (char *)"on signal:SIGIOT");
    }
    else if(signo == SIGCHLD)
    {
#if defined(_FUNC_do_not_use_now)
        pid_t        pid;
        int        stat;

        while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        {
            char bufMsg[64] = {0};
            snprintf(bufMsg, sizeof(bufMsg)-1, "Child %d terminated", pid);

            g_Solar.haveException(signo, (char *)bufMsg);
            
        }
#else //_FUNC_do_not_use_now
        g_Solar.haveException(signo, (char *)"on signal:SIGCHLD");
#endif //_FUNC_do_not_use_now

    }
    else
    {
        g_Solar.haveException(signo, (char *)"on other signal");
    }

}
