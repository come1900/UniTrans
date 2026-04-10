/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * main.c - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: main.c 5884 2013-05-28 09:01:25Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-05-28 09:01:25  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include        <stdio.h>
#include	<signal.h>
#include	<unistd.h>
#include	<sys/types.h>

#include	"../ez_asp_webs.h"

static void	sigintHandler(int);

/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */
#define DEFAULT_PORT 8080
#define DEFAULT_PAGE "index.asp"
#define DEFAULT_PATH "./spages"

int main(int argc, char** argv)
{
    printf("$ smartwebs: main.c,v 2.6 %s %s Exp $\n\n", __DATE__, __TIME__);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigintHandler);

    ez_asp_webs_init(DEFAULT_PAGE, DEFAULT_PATH);

    // -nclude "WebsSmartPorts.h"
    // 1<0 to use default port
    ez_asp_webs_open(DEFAULT_PORT, 2);
    printf("DEFAULT_PORT:%d, DEFAULT_PAGE:%s\n", DEFAULT_PORT, DEFAULT_PAGE);

    ez_asp_webs_proc();

    // ez_asp_webs_close();
    //
    // ez_asp_webs_cleanup();

    return 0;
}

/*
 *	Exit cleanly on interrupt
 */
static void sigintHandler(int unused)
{


    ez_asp_webs_close();

    ez_asp_webs_cleanup();
}

