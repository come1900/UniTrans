/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevLock_D1.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevLock_D1.h 5884 2014-02-12 02:20:14Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-12 02:20:14 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#ifndef _DevLock_D1_H
#define _DevLock_D1_H

enum EnLockLastError
{
    EnLockLastError_NO = 0,
    EnLockLastError_FD_NOT_OPENED = 1,
    EnLockLastError_IOCTL_SET1_FAILED ,
    EnLockLastError_IOCTL_SET0_FAILED ,
};

#define g_DevLock_D1  (*CDevLock_D1::instance())

class CDevLock_D1 : public CEZThread
{
public:
	PATTERN_SINGLETON_DECLARE(CDevLock_D1);

	CDevLock_D1();
	virtual ~CDevLock_D1();

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    typedef TSignal4<int, int *, int, time_t> ProducersSignal_t;
    // cmd, packet, ch, user
    typedef ProducersSignal_t::SigProc ProducersSignalProc_t;
    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    EZTHREAD_BOOL Start(CEZObject * pObj, ProducersSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, ProducersSignalProc_t pProc);

    void ThreadProc();

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
	int Lock(int iCh=0);
	int Unlock(int iCh=0);
	int GetStatus(int iCh=0);
	
	// 是否可用
	bool bValid();
	// 有？路
	int GetCh();

protected:

	int Open();

	int m_iLockStatus;
	int m_iCh;

	CEZMutex m_MutexOpr;

	int fd;
	int m_iLastError;

private:
	CEZMutex m_MutexSig;
    //ProducersSignal_t m_sigData;
    ProducersSignal_t m_SigBuffer;

    CEZMutex m_MutexSigBuffer;
    //计数
    int				m_iUser;

};

#endif // !defined(_DevLock_D1_H)
