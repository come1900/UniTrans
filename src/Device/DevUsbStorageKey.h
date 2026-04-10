// DevUsbStorageKey.h: interface for the CDevUsbStorageKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVUSBSTORAGEKEY_H__7AA351FC_02F1_4C02_959C_74FBE94D83F7__INCLUDED_)
#define AFX_DEVUSBSTORAGEKEY_H__7AA351FC_02F1_4C02_959C_74FBE94D83F7__INCLUDED_

#include <string>
#include <list>
#include <iostream>

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

class PartionNode
{
public:
    int iCmd;
    std::string strPartition;
    time_t ttPushed;

    void dump();
};

// 未列举的消息暂不需要
typedef enum __msg_usb_storage_key_t {
    MSG_USB_STORAGE_KEY_BEGIN = 0,		    	// Begin
    MSG_USB_STORAGE_KEY_FOUNDED,		// 发现key
    MSG_USB_STORAGE_KEY_ON_KEY_CONTENTS,		// key内容
    MSG_USB_STORAGE_KEY_END,					// End
}MSG_USB_STORAGE_KEY_T;

#define g_DevUsbStorageKey (*CDevUsbStorageKey::instance())
class CDevUsbStorageKey : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CDevUsbStorageKey);
    typedef TSignal4<int, const char *, int, time_t> DevUsbStorageKeySignal_t;
    // cmd, packet, len, time
    typedef DevUsbStorageKeySignal_t::SigProc DevUsbStorageKeySignalProc_t;

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    EZTHREAD_BOOL Start(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc);

    void ThreadProc();

    void OnPartition(int iCMD, char *pPacket, int iCh, time_t time);

    void StartHotPlug();
    void StopHotPlug();

private:
    CDevUsbStorageKey();
    virtual ~CDevUsbStorageKey();

    //DevUsbStorageKeySignalProc_t m_SigBuffer;
    DevUsbStorageKeySignal_t m_SigBuffer;

    CEZMutex m_MutexSigBuffer;

    //计数
    int				m_iUser;

    std::list<PartionNode> m_ListHotMsg;
    CEZMutex m_LockLis;
};
#endif // !defined(AFX_DEVUSBSTORAGEKEY_H__7AA351FC_02F1_4C02_959C_74FBE94D83F7__INCLUDED_)



