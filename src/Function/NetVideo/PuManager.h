/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuManager.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: PuManager.h 5884 2019-05-15 02:24:58Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-05-15 02:24:58  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "../../Device/NetVideo/PuClient.h"

#if !defined(_PuManager_H)
#define _PuManager_H

#define PREFIX_CPuManager_THREAD_NAME "PuManager-"

// 
#define g_NetVideo_PuManager (*CPuManager::instance())

class CPuManager : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CPuManager);

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

//public:
//    // msg, 
//    typedef TSignal2<int, void *> PuManagerSignal_t;
//    typedef PuManagerSignal_t::SigProc PuManagerSignalProc_t;

private:
    CPuManager();
    virtual ~CPuManager();

//    PuManagerSignal_t m_SigBuffer;
//    CEZMutex m_MutexSigBuffer;
public:
	void OnPuClientMsg(int iMsg, void *pPuClient);

private:
    CEZMutex m_MutexPuClient;
    std::vector<CPuClient *> m_v_pPuClient;
public:
    int addPuClient(CPuClient *pPuClient);
    int delPuClient(CPuClient *pPuClient);
    int delPuClient(int idx);

};

#endif // !defined(_PuManager_H)
