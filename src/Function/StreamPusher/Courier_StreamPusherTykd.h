/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Courier_StreamPusherTykd.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Courier_StreamPusherTykd.h 5884 2019-06-13 10:04:40Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-06-13 10:04:40  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_Courier_StreamPusherTykd_H)
#define _Courier_StreamPusherTykd_H

#include "EZThread.h"
#include "EZTimer.h"

#include <string>

#include "Pusher.h"

class CCourier_StreamPusherTykd : public CEZThread
{
public:
    //CCourier_StreamPusherTykd();
    CCourier_StreamPusherTykd(def_PusherContainer *pPusherCt);
    virtual ~CCourier_StreamPusherTykd();

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

private:
    int Do_saveDevStatusReq();

    def_PusherContainer *m_pPusherCt;

    std::string m_strStatusUploadWS;
    int m_iStatusUploadPeriod;
};

#endif // !defined(_Courier_StreamPusherTykd_H)
