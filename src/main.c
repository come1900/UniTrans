/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * main.c - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: main.c 5884 2014-02-26 04:19:34Z WuJunjie $
 *
 *  Notes:
 *     -
 *      使用动态库的 main (c 版)，使用方式参考：
 *           make dyn
 *           gcc -c main.c -I. -std=gnu99
 *           gcc -o uni main.o -L../outlib/ -luniTrans-linux-x86-32
 *           export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../outlib/
 *           ./uni
 *     -
 *
 *  Update:
 *     2014-02-26 04:19:34 WuJunjie Create
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

#include "UniTrans.h"
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int daemon_init(void);
void signal_proc(int signo);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int main(int argc, char * argv[])
{
    // 打印信息
    printf("main(%d,", argc);
    
    for(int i = 0; i < argc; i++)
    {
        printf(" %s", argv[i]);
    }
    printf(")\n");


    signal(SIGPIPE, signal_proc);

    signal(SIGTERM, signal_proc);
    signal(SIGKILL, signal_proc);
    signal(SIGQUIT, signal_proc);
    signal(SIGINT, signal_proc);
    
    // 初始化
    UniTransInitialize(argc, argv);
    // just exa
    //g_SimpleThread.Start();
    UniTransStart();

    // 最后的调用，
    // 完成一些例行维护和阻塞之用
    UniTransDone();

    return 0;
}

int daemon_init(void)
{
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0)
    {
        return(-1);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0)
    {
        /* Log the failure */

        //exit(EXIT_FAILURE);
        return(-1);
    }

    /* Change the current working directory */
    //        if ((chdir("/")) < 0) {
    //                /* Log the failure */
    //                exit(EXIT_FAILURE);
    //        }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return(0);
}

void signal_proc(int signo)
{
    if(signo == SIGTERM     \
        || signo == SIGKILL \
        || signo == SIGQUIT \
        || signo == SIGINT   \
        )
    {
        haveException(signo, (char *)"on exit signal");
        Term();
        exit(0);
    }
    else if(signo == SIGPIPE)
    {
        haveException(signo, (char *)"on signal:SIGPIPE");
    }
}
