/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevLock_D1.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevLock_D1.cpp 5884 2014-02-12 02:20:06Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-12 02:20:06 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "DevLock_D1.h"

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

#define MOTOR_MAGIC 'L'
#define RELAY1 		_IOW(MOTOR_MAGIC, 0,int)
#define RELAY2 		_IOW(MOTOR_MAGIC, 1,int)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PATTERN_SINGLETON_IMPLEMENT(CDevLock_D1);

CDevLock_D1::CDevLock_D1():CEZThread("CDevLock_D1", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iLockStatus = 0;
    // 第一版本 1通道
    m_iCh = 1;
    m_iLastError = EnLockLastError_NO;

    //-1 if an error occurred
    fd = open("/dev/relay", O_RDWR);
    if (fd < 0)
    {
        perror("open device relay error");
        //exit(1);
    }

    m_iUser = 0;
}

CDevLock_D1::~CDevLock_D1()
{
    if (fd>=0)
    {
        close(fd);
    }
}

bool CDevLock_D1::bValid()
{

    if (fd < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

int CDevLock_D1::GetStatus(int iCh)
{
    return m_iLockStatus;
}

		int CDevLock_D1::Lock(int iCh)
{
    int retval ;

    if (fd<0)
    {
        return EnLockLastError_FD_NOT_OPENED;
    }

    CEZLock __lock(m_MutexOpr);

    retval = ioctl(fd, RELAY1,1);
    DBG(
        printf("retval:%d\n", retval);
    );

    if (retval != 0)
    {
        m_iLastError = EnLockLastError_IOCTL_SET1_FAILED;
        return EnLockLastError_IOCTL_SET1_FAILED;
    }

    sleep(1);
    retval = ioctl(fd, RELAY1,0);
    DBG(
        printf("retval:%d\n", retval);
    );

    if (retval != 0)
    {
        m_iLastError = EnLockLastError_IOCTL_SET0_FAILED;
        return EnLockLastError_IOCTL_SET0_FAILED;
    }


    m_iLockStatus = 0;//EnLockStatus_Locked;
    m_iLastError = EnLockLastError_NO;

    return m_iLastError;
}

int CDevLock_D1::Unlock(int iCh)
{
    int retval ;

    if (fd<0)
    {
        return EnLockLastError_FD_NOT_OPENED;
    }

    CEZLock __lock(m_MutexOpr);

    retval = ioctl(fd, RELAY1,1);
    DBG(
        printf("retval:%d\n", retval);
    );

    if (retval != 0)
    {
        m_iLastError = EnLockLastError_IOCTL_SET1_FAILED;
        return EnLockLastError_IOCTL_SET1_FAILED;
    }

    sleep(1);
    retval = ioctl(fd, RELAY1,0);
    DBG(
        printf("retval:%d\n", retval);
    );

    if (retval != 0)
    {
        m_iLastError = EnLockLastError_IOCTL_SET0_FAILED;
        return EnLockLastError_IOCTL_SET0_FAILED;
    }


    m_iLockStatus = 0;
    m_iLastError = EnLockLastError_NO;

    return m_iLastError;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
EZTHREAD_BOOL CDevLock_D1::Start(CEZObject * pObj, ProducersSignalProc_t pProc)
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
        __trip;

        bRet = EZTHREAD_BOOL_TRUE;
    }
    m_iUser++;

    __fline;
    printf("m_iUser:%d\n", m_iUser);

    return bRet;

}
EZTHREAD_BOOL CDevLock_D1::Stop(CEZObject * pObj, ProducersSignalProc_t pProc)
{
    printf("CDevHDiskReader::Stop\n");

    CEZLock __lock(m_MutexSigBuffer);

    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        m_iUser--;
        // good
    }
    else
    {
        // error
        __trip;
    }

    EZTHREAD_BOOL bRet ;
    if (m_iUser==0)
    {
        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
    }
    else
    {
        bRet = EZTHREAD_BOOL_FALSE;
    }

    return bRet;
}

void CDevLock_D1::ThreadProc()
{
    int ii=0;
    while (m_bLoop)
    {
        __trip;

        // 发送消息 (unsigned int msg, EZTHREAD_PARAM wpa = 0, EZTHREAD_PARAM lpa = 0, unsigned int priority = 0);
        //g_Consumers.SendMessage(100, ii, ii-1, 0);

        // 回调
        m_SigBuffer(/*HDISKREADER_CMD_FINISHED*/1, NULL, ii, time(NULL));

        ii++;

        sleep(1);
    }
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
