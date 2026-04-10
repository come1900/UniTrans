/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevHotPlug.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevHotPlug.cpp 5884 2014-03-07 04:24:55Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-03-07 04:24:55 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/user.h>
#include <asm/types.h>
#include <linux/netlink.h>
//
#include <str_opr.h>
#include "../Logs.h"

#include "DevHotPlug.h"

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

#define HOTPLUG_BUFFER_SIZE		1024
#define HOTPLUG_NUM_ENVP		32
#define OBJECT_SIZE			512


PATTERN_SINGLETON_IMPLEMENT(CDevHotPlug);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CDevHotPlug::CDevHotPlug() :CEZThread("CDevHotPlug", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    //ez_trace("CDevHotPlug Enter--------\n");
}

CDevHotPlug::~CDevHotPlug()
{
    //ez_trace("CDevHotPlug Leave--------\n");

}

EZTHREAD_BOOL CDevHotPlug::Start(CEZObject * pObj, DevHotPlugSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        __fline;
        printf("attach error\n");
        return bRet;
    }

    if (m_iUser==0)
    {
        CreateThread();

        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {

        bRet = EZTHREAD_BOOL_TRUE;

        DBG(
            __fline;
            printf("no need, CreateThread, have %d users.\n", m_iUser);
        );

    }

    m_iUser++;

    ez_printf_debug("m_iUser:%d\n", m_iUser);

    return bRet;
}
EZTHREAD_BOOL CDevHotPlug::Stop(CEZObject * pObj, DevHotPlugSignalProc_t pProc)
{
    ez_printf_info("CDevHotPlug::Stop\n");
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iUser>0)
    {
        m_iUser--;
    }


    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
        // m_iUser--;
        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        // error
        __trip;
        printf("Detach failed.\n");
    }

    if (m_iUser==0)
    {

        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            __trip;
            printf("DestroyThread failed.\n");
        }
        else
        {
            DBG(
                printf("DestroyThread OK.\n");
            );
        }
    }
    else
    {
        DBG(
            __fline;
            printf("no need, DestroyThread, have %d users.\n", m_iUser);
        );
    }

    return bRet;
}
#define HotPlugUSB_PARTION 100
void CDevHotPlug::ThreadProc()
{
    //    int ii=0;

    int sock;
    struct sockaddr_nl snl;
    int retval;

    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (sock == -1)
    {
        printf("error getting socket, exit\n");
        //exit(1);
        return ;
    }

    retval = bind(sock, (struct sockaddr *) &snl,
                  sizeof(struct sockaddr_nl));
    if (retval < 0)
    {
        printf("bind failed, exit\n");
        //goto exit;

        close(sock);
        //exit(1);
        return ;

    }

    retval = bind(sock, (struct sockaddr *) &snl,
                  sizeof(struct sockaddr_nl));
    if (retval < 0)
    {
        printf("bind failed, exit %d, %s\n", __LINE__, strerror(errno));
        //exit(1);
        return ;
    }
    else
    {
        DBG(
            printf("double bind success\n");
        );
    }

    while (m_bLoop)
    {
        // buff最后可以人为加上结束符
        static char buffer[HOTPLUG_BUFFER_SIZE + OBJECT_SIZE+1];
        ssize_t bufferlen = HOTPLUG_BUFFER_SIZE + OBJECT_SIZE;
        ssize_t buflen = 0;

        ez_printf_debug("do recv.\n");

        buflen = recv(sock, buffer, bufferlen/*sizeof(buffer)*/, 0);
        if (buflen <  0)
        {
            ez_printf_error("error receiving\n");
            continue;
        }

        //if ((size_t)buflen > sizeof(buffer)-1)
        //    buflen = sizeof(buffer)-1;

        //char strDisk[256];

        int iret = OnUEventData(buffer, buflen);

        ARG_USED(iret);
        // 发送消息 (unsigned int msg, EZTHREAD_PARAM wpa = 0, EZTHREAD_PARAM lpa = 0, unsigned int priority = 0);
        //g_Consumers.SendMessage(100, ii, ii-1, 0);

        // 回调
        // // cmd, packet, len, time
        //        if (iret == HotPlugUSB_PARTION)
        //        {
        //            m_SigBuffer(/*HDISKREADER_CMD_FINISHED*/1, buffer, buflen, time(NULL));
        //        }
    }

    //delete cfgnetcommon;
}

int CDevHotPlug::OnUEventData(char *pData, int len)
{
    if (len<=0
        || pData==NULL)
    {
        return -1;
    }
    //static char object[OBJECT_SIZE];
    //const char *devpath;
    //const char *action;
    const char *envp[HOTPLUG_NUM_ENVP];
    int iret = -1;
    int i;
    char *pos;
    size_t bufpos;

    size_t buflen = len;
    char *buffer = pData;

    buffer[buflen] = '\0'; 

    /* save start of payload */
    bufpos = strlen(buffer) + 1;

    ez_printf_debug("OnUEvent:%s\n", pData);

    /* action string */
    //action = buffer;
    pos = strchr(buffer, '@');
    if (!pos)
    {
        return iret;
        //continue;
    }
    pos[0] = '\0';

    /* sysfs path */
    //devpath = &pos[1];

    /* hotplug events have the environment attached - reconstruct envp[] */
    for (i = 0; (bufpos < (size_t)buflen) && (i < HOTPLUG_NUM_ENVP-1); i++)
    {
        int keylen;
        char *key;

        key = &buffer[bufpos];
        keylen = strlen(key);
        envp[i] = key;
        bufpos += keylen + 1;
    }
    envp[i] = NULL;


    /* print payload environment */
//    DBG(
        //printf("[%ld] received '%s' from '%s'\n", time(NULL), action, devpath);

        for (i = 0; envp[i] != NULL; i++)
        printf("\t%d -- [%s]\n", i, envp[i]);

        printf("\n");
//    )
        ;
    /* 分拣消息 */
    bool bActionAdd = false;
    bool bActionMove = false;
    bool bDevtypePartition = false;
    const char *pSubstr = NULL;
    const char *pDevName = NULL;
    for (i = 0; envp[i] != NULL; i++)
    {
        //printf("\t%d -- [%s]\n", i, envp[i]);

        pSubstr = strstr(envp[i], "ACTION=add");
        if (pSubstr != NULL)
        {
            bActionAdd = true;
        }

        pSubstr = strstr(envp[i], "ACTION=remove");
        if (pSubstr != NULL)
        {
            bActionMove = true;
        }

        pSubstr = strstr(envp[i], "DEVTYPE=partition");
        if (pSubstr != NULL)
        {
            bDevtypePartition = true;
        }


        if (NULL == pDevName)
        {
            pDevName = strstr(envp[i], "DEVNAME=");
            if (pDevName)
            {
                //__trip;
            }

        }

    }

    if (bActionAdd
        && bDevtypePartition
        && NULL!=pDevName)
    {
        // Add Partition
        DBG(
            __fline;
            printf("\tAdd Partition -- [%s]\n", pDevName);
        );

        char PartitionName[64] = {'\0'};
        char tmp[64] = {'\0'};
        strncpy(tmp, pDevName, sizeof(PartitionName)-1);

        r_trim_c(tmp, ']', 0);
        r_split_str(tmp, PartitionName, "=", 0);

        DBG(
            __fline;
            printf("\tPartition Name: -- [%s]\n", PartitionName);
        );
        m_SigBuffer(MSG_HOTPLUG_USB_PARTION_ADD, PartitionName, strlen(PartitionName), time(NULL));

    }

    if (bActionMove
        && bDevtypePartition
        && NULL!=pDevName)
    {
        // Add Partition
        DBG(
            __fline;
            printf("\t remove Partition -- [%s]\n", pDevName);
        );

        char PartitionName[64] = {'\0'};
        char tmp[64] = {'\0'};
        strncpy(tmp, pDevName, sizeof(PartitionName)-1);

        r_trim_c(tmp, ']', 0);
        r_split_str(tmp, PartitionName, "=", 0);

        DBG(
            __fline;
            printf("\tPartition Name: -- [%s]\n", PartitionName);
        );

        m_SigBuffer(MSG_HOTPLUG_USB_PARTION_REMOVE, PartitionName, strlen(PartitionName), time(NULL));
    }
    //    else
    //    {
    //        // Add Partition
    //        //printf("\tnot a Partition \n");
    //    }

    return 0;
}
