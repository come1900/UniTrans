/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PusherTykd2Rtmp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PusherTykd2Rtmp.h 5884 2016-06-01 08:52:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-06-01 08:52:44  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_PusherTykd2Rtmp_H)
#define _PusherTykd2Rtmp_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "Pusher.h"

//#define def_GetPlayUrlmethod_getPlayUrlHX 0
//#define def_GetPlayUrlmethod_getPssServerPlayUrlHXReq 1
//#define def_GetPlayUrlmethod_getPssStoragePlayUrlHXReq 2


class CPusherTykd2Rtmp : public CPusher//CEZThread
{
public:
    EZTHREAD_BOOL Start(CEZObject * pObj, PusherSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, PusherSignalProc_t pProc);

    CPusherTykd2Rtmp();
    virtual ~CPusherTykd2Rtmp();

    const std::string &getUrlHls ();
    const std::string &getUrlHlsFix ();
    const std::string &getUrlRtmp ();
    const std::string &getUrlRtmpFix ();
    const std::string &getUrlRtsp ();
    const std::string &getUrlRtspFix ();


    const std::string &getTitle()      ;
    const std::string &getMemo ()      ;
    const std::string &getAddr ()      ;
    const std::string &getEndDt()       ;

    std::string m_strTitle      ;
    std::string m_strMemo       ;
    std::string m_strAddr       ;
    std::string m_strStartDt       ;
    std::string m_strEndDt       ;

    // 获取播放地址方法
    int m_iGetPlayUrlmethod;
private:
    void doPull();
    virtual void onStoped();

    std::string m_strUrlHls;
    std::string m_strUrlHlsFix;
    std::string m_strUrlRtmp ;
    std::string m_strUrlRtmpFix;
    std::string m_strUrlRtsp ;
    std::string m_strUrlRtspFix;

    std::string m_strShaChkSum;

    std::string m_strTykdWsUrl;
};
#endif // !defined(_PusherTykd2Rtmp_H)

