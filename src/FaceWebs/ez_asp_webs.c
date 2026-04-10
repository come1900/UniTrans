/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ez_asp_webs.c - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ez_asp_webs.c 5884 2014-02-09 09:09:16Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-09 09:09:16 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include	<signal.h>
#include	<unistd.h>
#include	<sys/types.h>

#include	"uemf.h"
#include	"wsIntrn.h"

#ifdef WEBS_SSL_SUPPORT
#include	"websSSL.h"
#endif

#ifdef USER_MANAGEMENT_SUPPORT
#include	"um.h"
void	formDefineUserMgmt(void);
#endif

#include	"websFunction.h"

#include	"ez_asp_webs.h"

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

static int			finished = 0;					/* Finished flag */

static int 	initWebs(const char *pDefaultPages, const char *pDefaultPath);
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
                                int arg, char_t *url, char_t *path, char_t *query);
#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif
// found in libasp
#if 0
/*下列两个函数， 不取本机接口， 防止部分板卡获取失败*/
int ez_gethostname(char *name, size_t len)
{
#if 0
    return gethostname(name, len);
#else

    strncpy(name, "ezlibs", len);
    return len>strlen("ezlibs")?0:-1;
#endif//
}

int ez_gethostaddr(struct in_addr * host_in_addr)
{
#if 0
    struct hostent	*hp;
    char			host[128];

    if ((hp = gethostbyname(host)) == NULL)
    {
        __trip;
        error(E_L, E_LOG, T("Can't get host address"));
        return -1;
    }

    memcpy((char *) &intaddr, (char *) hp->h_addr_list[0],
           (size_t) hp->h_length);

#else
    // will be ok, so do not check ret value
    inet_aton("127.0.0.1", host_in_addr);
#endif

    return 0;
}
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 *	Initialize the web server.
 */

static int initWebs(const char *pDefaultPages, const char *pDefaultPath)
{
    struct in_addr	intaddr;
    char			host[128], dir[128], webdir[128];
    char			*cp;
    char_t			wbuf[128];

    /*
     *	Initialize the socket subsystem
     */
    socketOpen();

#ifdef USER_MANAGEMENT_SUPPORT
    /*
     *	Initialize the User Management database
     */
    umOpen();
    umRestore(T("umconfig.txt"));
#endif

    /*
     *	Define the local Ip address, host name, default home page and the 
     *	root web directory.
     */
    //if (gethostname(host, sizeof(host)) < 0)
    if (ez_gethostname_local(host, sizeof(host)) < 0)
    {
        __trip;
        error(E_L, E_LOG, T("Can't get hostname"));
        return -1;
    }
    DBG(
        __fline;
        printf("host:%s\n", host);
    );

    ez_gethostaddr(&intaddr);

    /*
     *	Set ../web as the root web. Modify this to suit your needs
     */

    getcwd(dir, sizeof(dir));

    // 上级目录
#if 0

    if ((cp = strrchr(dir, '/')))
    {
        *cp = '\0';
    }
#endif

    // 2014-05-12 16:40:27 wujj
    // 使用相对路径
    //sprintf(webdir, "%s/%s", dir, pDefaultPath/*rootWeb*/);
    // 使用绝对路径
    strcpy(webdir, pDefaultPath/*rootWeb*/);

    /*
     *	Configure the web server options before opening the web server
     */
    websSetDefaultDir(webdir);
    DBG(
        printf("webdir:%s, websGetDefaultDir:%s\n", webdir, websGetDefaultDir());
    );
    //cp = inet_ntoa(intaddr);
    cp = inet_ntoa(intaddr);
    ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
    websSetIpaddr(wbuf);
    ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
    websSetHost(wbuf);

    // 设置登录框提示信息
    websSetRealm(LoginPrompt_t);
    /*
     *	Configure the web server options before opening the web server
     */
    if (pDefaultPages)
        websSetDefaultPage(T((char *)pDefaultPages));
    else
        websSetDefaultPage(T(S_PRT_DEFAULT_HOME));

    websSetPassword(password);

    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 *	Home page handler
 */
static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
                               int arg, char_t *url, char_t *path, char_t *query)
{
    /*
     *	If the empty or "/" URL is invoked, redirect default URLs to the home page
     */
    if (*url == '\0' || gstrcmp(url, T("/")) == 0)
    {
        //websRedirect(wp, S_PRT_DEFAULT_HOME);
        websRedirect(wp, websGetDefaultPage());

        return 1;
    }
    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef B_STATS
static void memLeaks()
{
    int		fd;

    if ((fd = gopen(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0)
    {
        bstats(fd, printMemStats);
        close(fd);
    }
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 *	Print memory usage / leaks
 */

static void printMemStats(int handle, char_t *fmt, ...)
{
    va_list		args;
    char_t		buf[256];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    write(handle, buf, strlen(buf));
}
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int ez_asp_webs_proc(int argc, char *argv[])
{

    /*
     *	Basic event loop. SocketReady returns true when a socket is ready for
     *	service. SocketSelect will block until an event occurs. SocketProcess
     *	will actually do the servicing.
     */
    finished = 0;
    while (!finished)
    {
        //__trip;
        if (socketReady(-1) || socketSelect(-1, 1000))
        {
            socketProcess(-1);
        }
        websCgiCleanup();
        emfSchedProcess();
    }

    return 0;

}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


int ez_asp_webs_init(const char *pDefaultPages, const char *pDefaultPath)
{
    //int mode = 0;/*no use now*/
    /*
       *	Initialize the memory allocator. Allow use of malloc and start 
       *	with a 60K heap.  For each page request approx 8KB is allocated.
       *	60KB allows for several concurrent page requests.  If more space
       *	is required, malloc will be used for the overflow.
       */

    bopen(NULL, (60 * 1024), B_USE_MALLOC);

    /*
     *	Initialize the web server
     */
    if (initWebs(pDefaultPages, pDefaultPath) < 0)
    {
        return -1;
    }
    return 0;

}

int ez_asp_webs_open(int port, int retries)
{
#ifdef WEBS_SSL_SUPPORT
    websSSLOpen();
    /*	websRequireSSL("/"); */	/* Require all files be served via https */
#endif

#if 1

    int __iport;		/* Server __iport */
    int __iretries;/* Server __iport __iretries */

    __iport = port>0?port:S_PRT_DEFAULT_PORT;
    __iretries = retries>0?retries:S_PRT_DEFAULT_RETRIES;

    /*
     *	Open the web server on the given __iport. If that port is taken, try
     *	the next sequential port for up to "__iretries" attempts.
     */
    DBG(
        printf("Open Server:%d, %d\n", __iport, __iretries);
        fflush(stdout);
    );
    websOpenServer(__iport, __iretries);
    /*
     * 	First create the URL handlers. Note: handlers are called in sorted order
     *	with the longest path handler examined first. Here we define the security 
     *	handler, forms handler and the default web page handler.
     */
    websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler,
                         WEBS_HANDLER_FIRST);
    websUrlHandlerDefine(T("/ezform"), NULL, 0, websFormHandler, 0);
    // do not use cgi now wujj 2013-05-31 9:21:35 begin
    //websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
    // do not use cgi now wujj 2013-05-31 9:21:35 end
    websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler,
                         WEBS_HANDLER_LAST);

    /*
     *	Now define two test procedures. Replace these with your application
     *	relevant ASP script procedures and form functions.
     */
    // websFunction.h
    websFunctionInit();

    /*
     *	Create the Form handlers for the User Management pages
     */
#ifdef USER_MANAGEMENT_SUPPORT

    formDefineUserMgmt();
#endif

    /*
     *	Create a handler for the default home page
     */
    websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0);
#endif

    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int ez_asp_webs_close()
{
    finished = 1;
    return 0;

}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int ez_asp_webs_cleanup()
{
    websFunctionCleanup();

#ifdef WEBS_SSL_SUPPORT

    websSSLClose();
#endif

#ifdef USER_MANAGEMENT_SUPPORT

    umClose();
#endif

    /*
     *	Close the socket module, report memory leaks and close the memory allocator
     */
    websCloseServer();
    socketClose();
#ifdef B_STATS

    memLeaks();
#endif

    bclose();

    return 0;

}
