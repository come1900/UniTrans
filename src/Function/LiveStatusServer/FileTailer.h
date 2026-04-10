/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileTailer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileTailer.h 5884 2019-09-20 02:01:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-09-20 02:01:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_FileTailer_H)
#define _FileTailer_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

//#define _useSingletonPattern_FileTailer 1

#if defined(_useSingletonPattern_FileTailer)
#define g_FileTailer (*CFileTailer::instance())
#endif //_useSingletonPattern_FileTailer

class CFileTailer : public CEZThread
{
public:
#if defined(_useSingletonPattern_FileTailer)
    PATTERN_SINGLETON_DECLARE(CFileTailer);
#endif //_useSingletonPattern_FileTailer

    typedef TSignal2<int, const char *> FileTailerSignal_t;
    // cmd, online
    typedef FileTailerSignal_t::SigProc FileTailerSignalProc_t;

    EZTHREAD_BOOL Start(CEZObject * pObj, FileTailerSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, FileTailerSignalProc_t pProc);

    void ThreadProc();

    CFileTailer(std::string strFilePathName="");
    virtual ~CFileTailer();

private:
    std::string m_strFilePathName;

    //FileTailerSignalProc_t m_SigBuffer;
    FileTailerSignal_t m_SigBuffer;

    CEZMutex m_MutexSigBuffer;

    //¼ÆÊý
    int				m_iUser;

};

#endif // !defined(_FileTailer_H)