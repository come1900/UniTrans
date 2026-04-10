/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ez_asp_webs.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ez_asp_webs.h 5884 2014-02-09 09:08:51Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-09 09:08:51 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _ez_asp_webs_H
#define _ez_asp_webs_H 1


#ifdef __cplusplus
extern "C"
{
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// 定制信息
#ifdef _PROJECT_NAME
#define LoginPrompt_t  T(_PROJECT_NAME)	/* prompt */
#else
#define LoginPrompt_t  T("FaceWebs")	/* prompt */
#endif

#define S_PRT_DEFAULT_HOME		T("index.htm") /* Default home page */
#define S_PRT_DEFAULT_PORT		880		/* Default HTTP port */
#define S_PRT_DEFAULT_SSL_PORT	4433		/* Default HTTPS port */
#define S_PRT_DEFAULT_RETRIES		5		/* Server port g_iretries */
//#define rootWeb  T("./spages")			/* Root web directory */
#define password  T("")				/* Security password */

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int ez_asp_webs_init(const char *pDefaultPages, const char *pDefaultPath);

int ez_asp_webs_open(int port, int retries);

int ez_asp_webs_proc();

int ez_asp_webs_close();

int ez_asp_webs_cleanup();

#ifdef __cplusplus
}
#endif

#endif /* _ez_asp_webs_H */
