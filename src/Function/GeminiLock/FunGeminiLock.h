/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunGeminiLock.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunGeminiLock.h 5884 2014-02-13 03:40:57Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-13 03:40:57 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <vector>
#include "ezBuffer.h"

#include "EZThread.h"
#include "../../Configs/ConfigGeminiLock.h"

#include "../../Device/DevLock_D1.h"

#ifndef _FunGeminiLock_H
#define _FunGeminiLock_H

#define g_FunGeminiLock  (*CFunGeminiLock::instance())

class Socket;

enum EnLockStatus
{
    EnLockStatus_Ready			=  0,		// 锁处于备用状态		_01
    EnLockStatus_Opened			,			// 锁处于开（可用状态）		_01
    EnLockStatus_Locked			,			// 锁处于闭合（使用中）状态		_02
};

enum EnOnGeminiKeyError
{
    OnGeminiKey_DataInvalidLen = 100,
    OnGeminiKey_NotAGeminiKey,
    OnGeminiKey_DecodeError,
    OnGeminiKey_EmptyKey,
};

enum EnOnGeminiLock
{
    OnGeminiLock_DoLock = 200,
    OnGeminiLock_DoLock_ButLocked,// = -OnGeminiLock_DoLock-1,

    OnGeminiLock_DoUnLock,// = 20001,
    OnGeminiLock_DoUnLock_ButUnLocked,// = -OnGeminiLock_DoUnLock-1,
    OnGeminiLock_DoUnLock_NotPaired,// = -OnGeminiLock_DoUnLock-1,
};

// 锁校验、日志数据
class CLockManData
{
public:

    std::string m_strPlainStr;
    std::string m_strLockedStr;
    int m_iLockUseStatus;// 记录使用状态
    time_t m_ttLastUseTime;

    // 记录锁具继电器的当前状态
    int m_iLockStatus;
    time_t m_ttLastOprTime;

    void Fresh();
    CLockManData& operator = (const CLockManData& obj);
    /* == */
    bool operator==(const CLockManData& obj) const;
    CLockManData();
    virtual ~CLockManData();

};

class GeminiKeys;

//#define MAX_PACKET_DATA_LEN 64
class CFunGeminiLock:public CEZObject
{
public:
    PATTERN_SINGLETON_DECLARE(CFunGeminiLock);

    CFunGeminiLock();
    virtual ~CFunGeminiLock();

    int Start();
    int Stop();

    void OnCfgGeminiLock(CConfigGeminiLock* pConfigDdns, int &ret);

    // 标准接口
    int GetStatus(int iCh=0);
    // 锁数量
    int getLockNum();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int OnKey_Use(int iKeyID, const char *pdat, size_t len, int iLockID = -1);
    // 钥匙数据检查
    int OnKey_Chk(int iKeyID, const char *pdat, size_t len, GeminiKeys *pKeys = NULL);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // iCh      [0.....], -1 -- all channel
    // iOnOff    1 -- on,  0 -- off
    int SetOut(int iCh, int iOnOff);
    int FreshStatus(int iID);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //int GetStatus(int iCh);

    EZTHREAD_BOOL OnSerial(int id, const char *pdat, size_t len);
	
    void OnData(Socket *pSocket, const char *pdat, size_t len);

    // u盘回调， u盘充当钥匙
    void OnUsbData(int iCMD, const char *pPacket, int iLen, time_t time_msg);

    void TimerProcLockMaintain();
    
    unsigned int getLockDevMask();

private:
    bool m_bHaveStarted;
    // start stop mutex
    CEZMutex m_LockFunGL;

    // 操作锁的互斥量
    CEZMutex m_LockOpr;

    int Lock(int iCh=0);
    int Unlock(int iCh=0);

    std::string generateSessionId();

    //int m_iLockStatus;
    //std::vector<GeminiData> m_vGeData;

    CezBuffer m_bufOnSerial;
    //unsigned char g_bufEspBufferOnBoard[MAX_ESP_BUFF_LEN]={0};
    //size_t g_bufLen=0;
    long long m_msbufLenTimeval;

    unsigned char MakeCheckSum_Carry8i8o(unsigned char *buf, int buflen);
    unsigned int m_iLockDev;
    int m_StatusIn;
    int m_StatusOut;
    int m_iFreshedTimes;

    CConfigGeminiLock *m_pGeminiLockCfg;

    //int m_iLockStatus;

    std::vector<CLockManData> m_vLockData;

    CEZTimer          m_TimerLockMaintain;          //保活功能定时器

	int DevStatusSerial;
	int DevStatusNetSerial;
};

#endif // !defined(_FunGeminiLock_H)

