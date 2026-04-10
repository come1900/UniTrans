/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Probe.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Probe.h 5884 2018-08-27 02:31:17Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-08-27 02:31:17  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_Probe_H)
#define _Probe_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "../../DBAgent/CacheKeyValue.h"

// 未列举的消息暂不需要
typedef enum __msg_hotplug_t {
    MSG_HOTPLUG_BEGIN = 0,		    	// Begin
    MSG_HOTPLUG_USB_PARTION_ADD,		// 添加 u盘 分区
    MSG_HOTPLUG_USB_PARTION_REMOVE,		// 去除 u盘 分区
    MSG_HOTPLUG_END,					// End
}MSG_HOTPLUG_T;

#define g_Probe (*CProbe::instance())
class CProbe : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CProbe);
    // cmd, partition, len, time
    typedef TSignal4<int, char *, int, time_t> ProbeSignal_t;
    typedef ProbeSignal_t::SigProc ProbeSignalProc_t;

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    EZTHREAD_BOOL Start(CEZObject * pObj, ProbeSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, ProbeSignalProc_t pProc);

    void ThreadProc();

    int addPeer(const std::string &strPeerID, const std::string &strPeerInfo);
    int delPeer(const std::string &strPeerID);

private:
    CProbe();
    virtual ~CProbe();

    //ProbeSignalProc_t m_SigBuffer;
    ProbeSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;

    //计数
    int				m_iUser;

    // ����
    CCacheKeyValue *m_pCacheWithDb;
    int bankCloudUpdateDevice(const std::string &strPeerID, const std::string &strPeerStatus);

};
#endif // !defined(_Probe_H)



