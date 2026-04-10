/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PusherTykd2Rtsp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PusherTykd2Rtsp.h 5884 2016-06-14 11:08:11Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-06-14 11:08:11  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#if !defined(_PusherTykd2Rtsp_H)
#define _PusherTykd2Rtsp_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "Pusher.h"

class CPusherTykd2Rtsp : public CPusher//CEZThread
{
public:

    EZTHREAD_BOOL Start(CEZObject * pObj, PusherSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, PusherSignalProc_t pProc);

    CPusherTykd2Rtsp();
    virtual ~CPusherTykd2Rtsp();

private:
    void doPull();
    virtual void onStoped();

    std::string m_strTykdWsUrl;

    //int m_iVideoEncode;
    //int m_iAudioEncode;
};
#endif // !defined(_PusherTykd2Rtsp_H)

