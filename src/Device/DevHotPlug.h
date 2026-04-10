/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevHotPlug.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevHotPlug.h 5884 2014-03-07 04:24:02Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-03-07 04:24:02 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_DevHotPlug_H)
#define _DevHotPlug_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

// 未列举的消息暂不需要
typedef enum __msg_hotplug_t {
    MSG_HOTPLUG_BEGIN = 0,		    	// Begin
    MSG_HOTPLUG_USB_PARTION_ADD,		// 添加 u盘 分区
    MSG_HOTPLUG_USB_PARTION_REMOVE,		// 去除 u盘 分区
    MSG_HOTPLUG_END,					// End
}MSG_HOTPLUG_T;

#define g_DevHotPlug (*CDevHotPlug::instance())
class CDevHotPlug : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CDevHotPlug);
    // cmd, partition, len, time
    typedef TSignal4<int, char *, int, time_t> DevHotPlugSignal_t;
    typedef DevHotPlugSignal_t::SigProc DevHotPlugSignalProc_t;

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    EZTHREAD_BOOL Start(CEZObject * pObj, DevHotPlugSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, DevHotPlugSignalProc_t pProc);

    void ThreadProc();

private:
    CDevHotPlug();
    virtual ~CDevHotPlug();

    int OnUEventData(char *pData, int len);

    //DevHotPlugSignalProc_t m_SigBuffer;
    DevHotPlugSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;

    //计数
    int				m_iUser;
};
#endif // !defined(_DevHotPlug_H)



