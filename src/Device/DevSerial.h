/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevSerial.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevSerial.h 5884 2013-07-16 07:30:00Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-16 07:30:00 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __DevSerial_H__
#define __DevSerial_H__

#include <vector>

#include "EZThread.h"
#include "EZSignals.h"

#include "ezBuffer.h"

#include "ezserial.h"

#define GeminiLOCK_SERIAL_DEVICE_ID 1

//#define COM_NAME_LENGTH         64
class SERIAL_ATTR_T
{
public:
    int baud;
    int data_bits; //7- 8-
    int parity; // checkparity 0-noparity 1-odd(奇) 2-even(偶)
    int stop_bits; // 1, 15, 2

    SERIAL_ATTR_T()
    {
        baud = 115200;
        data_bits = 8;
        parity = 0;
        stop_bits = 1;
    }

};

class DEV_SERIAL_ATTR_T:public SERIAL_ATTR_T
{
public:
    ezserial_t *m_pSerial;
    int id;                                                       // id of com record in db
    std::string serial_dev_name;//[COM_NAME_LENGTH]; // /dev/ttyUSB0 ...

    CezBuffer *m_pBuffer;
    size_t m_stMaxBufferSize;
    unsigned long  m_msLastOndata;// 上次数据到达，取counter
    unsigned long  m_msOndataTimeout;// data timeout

    unsigned long  m_msLastSend;// 上次数据发送，取counter

    DEV_SERIAL_ATTR_T()
    {
        id = 0;
        m_pSerial = NULL;
        serial_dev_name = "";

        m_stMaxBufferSize = 0;
		m_msOndataTimeout = 0;

        m_pBuffer = NULL;

        m_msLastOndata = 0;// 上次数据到达，取counter
        m_msLastSend = 0;// 上次数据发送，取counter
    }

};


#define g_DevSerial (*CDevSerial::instance())

class CDevSerial : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CDevSerial);
    typedef TSignal3<int, void *, int> SIG_DevSerial_DATA;

    // name is the key
    // m_pSerial will be modify
    int AddSerial(DEV_SERIAL_ATTR_T &SerialAttr);
    int RemoveSerial(const std::string &strSerialName);

    //static CDevSerial* instance(void);

    // id data datalen
    EZTHREAD_BOOL Start(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc);

    void ThreadProc();

    void OnData(int id, uint8_t *buf, int length);
    int SendData(int id, uint8_t *buf, int length);

    //默认不缓存数据
    void bufferSerial(int id, size_t stMaxBufferSize=0);
	
	// 检测热拔插串口的变化
    void OnPartition(int iCMD, char *pPacket, int iCh, time_t time);
    void StartHotPlug();
    void StopHotPlug();
    CEZMutex m_LockLis;
	// 

private:
    CDevSerial();
    virtual ~CDevSerial();
    
    //static CDevSerial* _instance;

    CEZMutex m_MutexSig;
    SIG_DevSerial_DATA m_sigData;

    bool m_bRunning;

    CEZMutex m_MutexSerial;
    std::vector<DEV_SERIAL_ATTR_T> m_vecSerial;

    void SetDefaultConfig();
};

#endif //__DevSerial_H__
