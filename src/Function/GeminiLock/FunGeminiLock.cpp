/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunGeminiLock.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunGeminiLock.cpp 5884 2014-02-13 03:40:53Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-13 03:40:53 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "str_opr.h"
#include "ez_system_api.h"
#include "ez_bit.h"
#include "ez_util_platform.h" // ez_ms_get_tick_count

#include <sys/time.h>

#include	"../../Logs.h"

#ifdef _DEV_NetSerialTCPSOCKETCONNECTOR
#include "../../Device/NetSerial/src/NetSerial.h"
#endif //_DEV_NetSerialTCPSOCKETCONNECTOR

#ifdef _DEV_SERIAL
#include "../../Device/DevSerial.h"
#endif // _DEV_SERIAL

#ifdef _USE_InterThings_gLock
#include "../InterThings/gLock/AgentInterThings_gLock.h"
#endif // _USE_InterThings_gLock

#include "../../Device/carry_io_board.h"

#include "../../Device/DevUsbStorageKey.h"

#include "FunGeminiLock.h"

#include "../GeminiKey/GeminiKeyStack.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

/* == */
bool CLockManData::operator==(const CLockManData& obj) const
{
    if (
        this->m_strPlainStr != obj.m_strPlainStr ||
        this->m_strLockedStr != obj.m_strLockedStr ||
        this->m_iLockUseStatus != obj.m_iLockUseStatus ||
        this->m_ttLastUseTime != obj.m_ttLastUseTime
    )
    {
        return false;
    }

    return true;
}

/* = */
CLockManData& CLockManData::operator = (const CLockManData& obj)
{
    this->m_strPlainStr 	= obj.m_strPlainStr ;
    this->m_strLockedStr 	= obj.m_strLockedStr;
    this->m_iLockUseStatus     = obj.m_iLockUseStatus ;
    this->m_ttLastUseTime       = obj.m_ttLastUseTime   ;

    return *this;
}

void CLockManData::Fresh()
{
    m_ttLastUseTime = 0;
    m_iLockUseStatus = EnLockStatus_Ready;
    m_strPlainStr = "";
    m_strLockedStr = "";
}

CLockManData::CLockManData()
{
    m_iLockStatus = 0;
    m_ttLastOprTime = 0;

    //m_ttLastUseTime = 0;
    //m_iLockUseStatus = EnLockStatus_Ready;
    Fresh();
}

CLockManData::~CLockManData()
{}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//状态刷新所需时间， 实际测试发现在9-15ms， 此处加倍
#define WAITE_FOR_STATUS_REFRESH_OF_MS 30
// enocean标准为100ms， 但是根据上面的定义看肯定小于上述
// 如果有新板测试， 则将此两数据*10， 在控制台输入 glock u查看即可
#define ESP_INTERBYTE_TIME_OUT 20

PATTERN_SINGLETON_IMPLEMENT(CFunGeminiLock);
CFunGeminiLock::CFunGeminiLock():m_TimerLockMaintain("TimerSetClockOff")
{
    m_pGeminiLockCfg = new CConfigGeminiLock;
    assert(m_pGeminiLockCfg);

    //gettimeofday(&g_bufLenTimeval, NULL);
    m_msbufLenTimeval = 0;
	
    m_iFreshedTimes = 0;
    m_StatusIn = 0;
    m_StatusOut = 0;

    m_iLockDev = 0;
}

int CFunGeminiLock::GetStatus(int iCh)
{
    if (iCh >= (int)m_vLockData.size())
    {
        __trip;
        return -1;
    }

    return m_vLockData[iCh].m_iLockUseStatus;
}

// 锁数量
int CFunGeminiLock::getLockNum()
{
    return m_vLockData.size();
}


CFunGeminiLock::~CFunGeminiLock()
{
    delete m_pGeminiLockCfg;
}

int CFunGeminiLock::Lock(int iCh)
{
    if (iCh >= (int)m_vLockData.size())
    {
        __trip;
        return -1;
    }

    //if (BitGet(m_iLockDev, 1) !=0 )
    {
        //__trip;
        SetOut(iCh, 1);
        // 定时器会自动检查并关闭继电器
        return 0;
    }

    g_DevLock_D1.Lock(iCh);
    //m_iLockStatus = EnLockStatus_Locked;
    //m_vLockData[iCh].m_iLockStatus = EnLockStatus_Locked;

    return 0;
}

int CFunGeminiLock::Unlock(int iCh)
{
    if (iCh >= (int)m_vLockData.size())
    {
        __trip;
        return -1;
    }

    //if (BitGet(m_iLockDev, 1) !=0 )
    {
        if (m_pGeminiLockCfg->getConfig().iLockTimeOut > 0)
        {
            // 此情况定时器会自动检查并关闭继电器
            SetOut(iCh, 1);
        }
        else if (m_pGeminiLockCfg->getConfig().iLockTimeOut == 0)
        {
            SetOut(iCh, 0);
        }
        else
        {
            LOG4CPLUS_ERROR(LOG_OF_FGL, "m_pGeminiLockCfg->getConfig().iLockTimeOut:(" << m_pGeminiLockCfg->getConfig().iLockTimeOut << ") ...");
        }

        return 0;
    }

    g_DevLock_D1.Unlock(iCh);
    //m_iLockStatus = EnLockStatus_Ready;
    //m_vLockData[iCh].m_iLockStatus = EnLockStatus_Ready;

    return 0;
}

void CFunGeminiLock::OnData(Socket *pSocket, const char *pdat, size_t len)
{
    __fline;
    printf("CDataUsageClass::OnData From:%s Len:%d Char[0]:%c\n", pSocket->GetSockName().c_str(), len, pdat[0]);
}

//#define LOCK_SERIAL_DEVICE_NAME "/dev/ttyUSB0"
int CFunGeminiLock::Start()
{
    int iret = -1;
    ARG_USED(iret);
    if (m_bHaveStarted)
    {
        return 0;
    }
    CEZLock l(m_LockFunGL);

    m_bHaveStarted = true;

    // 启动直接控制方式
    m_pGeminiLockCfg->update();
    m_pGeminiLockCfg->attach(this,(TCONFIG_PROC)&CFunGeminiLock::OnCfgGeminiLock);

    // 锁状态数据缓存
    m_vLockData.resize(m_pGeminiLockCfg->getConfig().iLockChannel);

    // 启动串口控制方式
    if (m_pGeminiLockCfg->getConfig().iLockDevType==1
        && m_pGeminiLockCfg->getConfig().strLockInterface.size()>3)
    {
        DEV_SERIAL_ATTR_T __cfg;

        __cfg.baud = 9600;// 9600;
        __cfg.data_bits = 8;//8;
        __cfg.parity = 0; //0;
        __cfg.stop_bits = 1;//1;
        __cfg.m_pSerial = NULL;

        __cfg.id = GeminiLOCK_SERIAL_DEVICE_ID;// conn at com33
        __cfg.serial_dev_name = m_pGeminiLockCfg->getConfig().strLockInterface;//LOCK_SERIAL_DEVICE_NAME;

        iret = g_DevSerial.AddSerial(__cfg);
        if (iret >= 0)
        {
            EZTHREAD_BOOL bStart = g_DevSerial.Start(this, (CDevSerial::SIG_DevSerial_DATA::SigProc)&CFunGeminiLock::OnSerial);

            if (EZTHREAD_BOOL_TRUE != bStart)
            {
                __trip;
            }
            else
            {
                m_iLockDev = BitSet(m_iLockDev, 1);

            }
        }
    }
    else
    {
        __trip;
    }

    // 启动网络串口
    g_NetSerial.Initialize(0, NULL);

    g_NetSerial.Start(this, (SIG_CNetSerialHandler_DATA::SigProc)&CFunGeminiLock::OnData);
    m_iLockDev = BitSet(m_iLockDev, 1);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 钥匙
    g_DevUsbStorageKey.Start(this, (CDevUsbStorageKey::DevUsbStorageKeySignalProc_t)&CFunGeminiLock::OnUsbData);

    m_TimerLockMaintain.Start(this
                              , (EZ_TIMER_PROC)&CFunGeminiLock::TimerProcLockMaintain
                              , 0
                              , 1 * 1000 /*每秒执行一次，请勿修改*/
                              , 0
                              , 10*1000
                             );

    // 启动服务
#ifdef _USE_InterThings_gLock
    g_AgentInterThings_gLock.Start();
#endif // _USE_InterThings_gLock

    return 0;
}

unsigned int CFunGeminiLock::getLockDevMask()
{
    return m_iLockDev;
}


void CFunGeminiLock::TimerProcLockMaintain()
{
    time_t ttNow = time(NULL);

    // 将闭合的继电器恢复
    for (size_t ii=0; ii< m_vLockData.size(); ii++)
    {
        if (m_vLockData[ii].m_iLockStatus==1
            && m_pGeminiLockCfg->getConfig().iLockTimeOut > 0
            && ttNow-m_vLockData[ii].m_ttLastOprTime >= m_pGeminiLockCfg->getConfig().iLockTimeOut)
        {
            this->SetOut(ii, 0);
        }

    }

}

int CFunGeminiLock::Stop()
{
    if (m_bHaveStarted)
    {
        CEZLock l(m_LockFunGL);

        g_DevSerial.RemoveSerial(m_pGeminiLockCfg->getConfig().strLockInterface/*LOCK_SERIAL_DEVICE_NAME*/);

        g_DevSerial.Stop(this, (CDevSerial::SIG_DevSerial_DATA::SigProc)&CFunGeminiLock::OnSerial);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        // 钥匙
        g_DevUsbStorageKey.Stop(this, (CDevUsbStorageKey::DevUsbStorageKeySignalProc_t)&CFunGeminiLock::OnUsbData);

        m_bHaveStarted = false;
    }
    else
    {
        __fline;
        std::cout << "have been started." << std::endl;
    }

    return 0;
}

void CFunGeminiLock::OnCfgGeminiLock(CConfigGeminiLock* pConfigDdns, int &ret)
{
    __fline;
    printf(" %s \n", __FUNCTION__);

    // 配置更新
    m_pGeminiLockCfg->update();
}

EZTHREAD_BOOL CFunGeminiLock::OnSerial(int id, const char *pdat, size_t len)
{
    if((pdat == NULL) || (len <= 0))
    {
        return false;
    }

    // 仅解析此串口的数据
    if (id != GeminiLOCK_SERIAL_DEVICE_ID)
    {
        return false;
    }

    // your code
    DBG(
        __fline;
        printf("CTeleSerial::OnSerial:%d -- 0x%0x\n", len, pdat[0]);
    );

    //int iRet;
    long long ms_t_now_time;
    long long sub_ms;

    // Check for timeout between two bytes
    ms_t_now_time = ez_ms_get_tick_count();
    sub_ms = ms_t_now_time-m_msbufLenTimeval;

    if (sub_ms > ESP_INTERBYTE_TIME_OUT)
    {
        //__fline;
        //printf("sub_ms:%d\n", sub_ms);

        m_bufOnSerial.Reset();
    }

    m_msbufLenTimeval = ez_ms_get_tick_count();

    // Store data
    m_bufOnSerial.Append((unsigned char *)pdat, len);

    //    dumpBuffer(stdout, (unsigned char *)m_bufOnSerial.Buf(),
    //               m_bufOnSerial.Size(),
    //               SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);

    if (*(m_bufOnSerial.Buf()) != 0x22)
    {
        //__trip;
        m_bufOnSerial.Reset();
    }

    while (*(m_bufOnSerial.Buf()) == 0x22
           && m_bufOnSerial.Size()>=8)
    {
        DBG(
            dumpBuffer(stdout, (unsigned char *)m_bufOnSerial.Buf(),
                       8/*m_bufOnSerial.Size()*/,
                       SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
        );

        unsigned char cmdBuff[8];
        m_bufOnSerial.Pop(cmdBuff, 8);
        //todo
        if (cmdBuff[7] == MakeCheckSum_Carry8i8o(cmdBuff, 7))
        {
            m_StatusIn = cmdBuff[4]&0xf;
            m_StatusOut = cmdBuff[6]&0xf;
            // 只被serial回调， 不加锁
            m_iFreshedTimes++;

        }
        else
        {
            printf("check sum error.\n");
        }
    }

    return EZTHREAD_BOOL_TRUE;
}

unsigned char CFunGeminiLock::MakeCheckSum_Carry8i8o(unsigned char *buf, int buflen)
{
    unsigned int sum = 0;
    int ii;
    for (ii=0; ii<buflen; ii++)
    {
        sum += buf[ii];
    }

    return (unsigned char)sum;
}


int CFunGeminiLock::SetOut(int channel, int iOnOff)
{
    //__trip;
    //全开：
    unsigned char buff_all_on[] ={0x55, 0x01, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x67};
    //全关：
    unsigned char buff_all_off[]={0x55, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, 0x69};

    int send_buff_len = sizeof(buff_all_on);

    unsigned char *send_buff = (iOnOff==1 ? buff_all_on : buff_all_off);

    CEZLock l(m_LockOpr);

    if (channel>=0)
    {
        DBG(
            printf("channel:%10d -- ch: %d, %s\n", channel, (channel%8)+1, iOnOff==1?"on":"off");
        );
        EZ_LOGS(
            //ez_log("%s(%d) -- OnAccept:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
            ez_log("%s(%d) -- channel:%10d -- ch: %d, %s\n", __FILE__, __LINE__, channel, (channel%8)+1, iOnOff==1?"on":"off");
        );

        carry_makebuf_set_out(send_buff, send_buff_len, 1, (channel%8)+1, iOnOff);

        if (channel >= (int)m_vLockData.size())
        {
            __trip;
            //return -1;
        }
        else
        {
            m_vLockData[channel].m_iLockStatus = iOnOff;
            time(&(m_vLockData[channel].m_ttLastOprTime));
        }

    }
    else
    {
        std::vector<CLockManData>::iterator  iter;
        for ( iter = m_vLockData.begin() ; iter != m_vLockData.end() ; iter++ )
        {
            iter->m_iLockStatus = iOnOff;
            time(&(iter->m_ttLastOprTime));
        }
    }

    // 否则直接发送  即为全开全关
    int iret = g_DevSerial.SendData(GeminiLOCK_SERIAL_DEVICE_ID, (uint8_t *)send_buff, send_buff_len);

    char strName[256];
    //snprintf(strName, sizeof(strName)-1, "%s:%d", DEFAULT_NEXT_HOST, DEFAULT_NEXT_HOST_PORT);
    snprintf(strName, sizeof(strName)-1, "%s:%d", DEFAULT_NET_SERIAL_HOST, DEFAULT_NET_SERIAL_HOST_PORT);
    g_NetSerial.Send2Socket(strName, send_buff, send_buff_len);

    DBG(
        __fline;
        printf("send_buff_len:%d\n", send_buff_len);
    );

    EZ_LOGS(
        ez_log("%s(%d) -- Send2Socket(%s):%d\n", __FILE__, __LINE__, strName, send_buff_len);
    );

    return iret;
}

int CFunGeminiLock::FreshStatus(int iID)
{
    int ret;
    int times = m_iFreshedTimes;

    printf("history -- m_StatusIn:%0x, m_StatusOut:0x%0x\n", m_StatusIn, m_StatusOut);
    // 查询指令
    unsigned char cmd[] = {0x55, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x66};
    int send_buff_len = sizeof(cmd);

    ret = g_DevSerial.SendData(GeminiLOCK_SERIAL_DEVICE_ID, (uint8_t *)cmd, send_buff_len);

    if (ret < 0)
    {
        printf("g_DevSerial.SendData faield\n");
    }
    else
    {
        int ii;
        for (ii=0; ii<WAITE_FOR_STATUS_REFRESH_OF_MS; ii++)
        {
            usleep(1000);

            if ( times!=m_iFreshedTimes)
            {
                break;
            }
        }

        printf("current -- m_StatusIn:%0x, m_StatusOut:0x%0x, used %d(ms)\n", m_StatusIn, m_StatusOut, ii);
    }

    return (times==m_iFreshedTimes) ? -1 : 0;
}

void CFunGeminiLock::OnUsbData(int iCMD, const char *pPacket, int iLen, time_t time_msg)
{
    int iret;
    ARG_USED(iret);
    //    DBG(
    __fline;
    printf("CALLBACK - OnUsbData: %d, \n\n%s\n\n, %d, %ld\n", iCMD, pPacket==NULL?"NULL":pPacket, iLen, time_msg);
    //    );

    iret = OnKey_Use(1, pPacket, iLen, -1);
}


int CFunGeminiLock::OnKey_Chk(int iKeyID, const char *pdat, size_t len, GeminiKeys *pKeys )
{
    if (len<=0 || len>=MAX_GEMINI_KEY_DATA_LEN || pdat==NULL)
    {
        return OnGeminiKey_DataInvalidLen;
    }

    char strKeyData[MAX_GEMINI_KEY_DATA_LEN] = {0};

    strncpy(strKeyData, pdat, len);
    trim_blank(strKeyData, 0);

    EZ_LOGS(
        ez_log("%s(%d) -- strKeyData:%s\n", __FILE__, __LINE__, strKeyData);
    );

    int iret;
    GeminiKeys __KeysGen;
    CGeminiKeyStack __GeminiKeyStackGen;
    iret = __GeminiKeyStackGen.GeminiKeyImport(strKeyData);
    if (iret<0)
    {
        //__trip;
        //websWrite(wp, T("<p>[%s]</p>\n"), "input is not a valid GeminiKey");
        return OnGeminiKey_NotAGeminiKey;
    }
    else
    {
        iret = __GeminiKeyStackGen.Decode(__KeysGen);
        if (iret<0)
        {
            //__trip;
            //websWrite(wp, T("<p>[%s]</p>\n"), "input is not a valid GeminiKey(Decode error)");
            LOG4CPLUS_ERROR(LOG_OF_FGL, "input is not a valid GeminiKey(Decode error)(" << iret << ") ...");

            return OnGeminiKey_DecodeError;
        }
        else
        {

            if (__KeysGen.isValid() && __KeysGen.m_vecGeminiKey.size()>0)
            {
                if (pKeys != NULL)
                {
                    *pKeys = __KeysGen;
                    return 0;
                }
            }
            else
            {
                return OnGeminiKey_EmptyKey;
            }
        }
    }

    return 0;
}
/*
enum EnOnGeminiKeyError
{
    OnGeminiKey_DataInvalidLen = 100,  //密钥数据长度不正确
    OnGeminiKey_NotAGeminiKey,         //非法数据
    OnGeminiKey_DecodeError,           //数据解析失败
    OnGeminiKey_EmptyKey,              //不存在密钥
};

enum EnOnGeminiLock
{
    OnGeminiLock_DoLock = 200,         // ok, 执行了锁操作
    OnGeminiLock_DoLock_ButLocked,     // 设备忙：欲执行锁操作，但是已经处于锁状态

    OnGeminiLock_DoUnLock,             //ok, 执行解锁操作
    OnGeminiLock_DoUnLock_ButUnLocked, // 设备忙：欲执行解锁操作，但是已经处于解锁状态
    OnGeminiLock_DoUnLock_NotPaired,   // 密钥不匹配
};
*/ 
int CFunGeminiLock::OnKey_Use(int iKeyID, const char *pdat, size_t len, int iLockID)
{
    GeminiKeys __KeysGenUnlock;
    int iret = OnKey_Chk(iKeyID, pdat, len, &__KeysGenUnlock);

    // 选定的锁，
    // todo， 多选一。。。。
    int ich_lock = 0;

    // use it
    if (iret>=0 && __KeysGenUnlock.isValid() && __KeysGenUnlock.m_vecGeminiKey.size()>0)
    {
        if (__KeysGenUnlock.m_vecGeminiKey[ich_lock].strKeyType == GeminiKeyTypeLockKey)
        {
            if (EnLockStatus_Ready == GetStatus(ich_lock))
            {
                this->Lock(ich_lock);

                LOG4CPLUS_INFO(LOG_OF_FGL, "DO g_FunGeminiLock.Lock()");

                ez_printf_debug("%s\n", "DO g_FunGeminiLock.Lock()");

                m_vLockData[ich_lock].m_iLockUseStatus = EnLockStatus_Locked;

                // 此处经过数据检查，可以保证正确
                CGeminiKeyStack CGeminiKeyStack_testLockLock;
                iret = CGeminiKeyStack_testLockLock.GeminiLock(__KeysGenUnlock
                        , m_vLockData[ich_lock].m_strPlainStr
                        , m_vLockData[ich_lock].m_strLockedStr);
                time(&(m_vLockData[ich_lock].m_ttLastUseTime));
                DBG(
                    __fline;
                    std::cout << "GeminiLock iret: " << iret << std::endl;
                    std::cout << "Plain: " << m_vLockData[ich_lock].m_strPlainStr << std::endl;
                    std::cout << "Locke: " << m_vLockData[ich_lock].m_strLockedStr << std::endl;
                );
                LOG4CPLUS_INFO(LOG_OF_FGL, "GeminiLock iret: " << iret);
                LOG4CPLUS_INFO(LOG_OF_FGL, "Plain: " << m_vLockData[ich_lock].m_strPlainStr );
                LOG4CPLUS_INFO(LOG_OF_FGL, "Locke: " << m_vLockData[ich_lock].m_strLockedStr);
                EZ_LOGS(
                    ez_log("%s(%d)--\n", __FILE__, __LINE__);
                    ez_log("------------------------------------------\n");
                    ez_log("GeminiLock iret:\n", iret);
                    ez_log("Plain:%s\n", m_vLockData[ich_lock].m_strPlainStr.c_str());
                    ez_log("Locke:%s\n", m_vLockData[ich_lock].m_strLockedStr.c_str());
                    ez_log("\n------------------------------------------\n");
                );

                return OnGeminiLock_DoLock;
            }
            else
            {
                LOG4CPLUS_INFO(LOG_OF_FGL, "Want lock, but locked");
                return OnGeminiLock_DoLock_ButLocked;
            }
        }
        else if (__KeysGenUnlock.m_vecGeminiKey[ich_lock].strKeyType == GeminiKeyTypeUnlockKey)
        {

            if (EnLockStatus_Locked == GetStatus(ich_lock))
            {
                CGeminiKeyStack CGeminiKeyStack_testImportLockUnlock;

                iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(__KeysGenUnlock
                        , m_vLockData[ich_lock].m_strPlainStr
                        , m_vLockData[ich_lock].m_strLockedStr);
                DBG(
                    __fline;
                    std::cout << "GeminiUnlock iret: " << iret << std::endl;
                    std::cout << "Plain: "<< m_vLockData[ich_lock].m_strPlainStr << std::endl;
                    std::cout << "Locke: "<< m_vLockData[ich_lock].m_strLockedStr << std::endl;
                );
                LOG4CPLUS_INFO(LOG_OF_FGL, "GeminiLock iret: " << iret);
                LOG4CPLUS_INFO(LOG_OF_FGL, "Plain: " << m_vLockData[ich_lock].m_strPlainStr );
                LOG4CPLUS_INFO(LOG_OF_FGL, "Locke: " << m_vLockData[ich_lock].m_strLockedStr);
                EZ_LOGS(
                    ez_log("%s(%d)--\n", __FILE__, __LINE__);
                    ez_log("------------------------------------------\n");
                    ez_log("GeminiLock iret:\n", iret);
                    ez_log("Plain:%s\n", m_vLockData[ich_lock].m_strPlainStr.c_str());
                    ez_log("Locke:%s\n", m_vLockData[ich_lock].m_strLockedStr.c_str());
                    ez_log("\n------------------------------------------\n");
                );

                if (iret<0)
                {
                    LOG4CPLUS_INFO(LOG_OF_FGL, "OnGeminiLock_DoUnLock_NotPaired");
                    EZ_LOGS(
                        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, "OnGeminiLock_DoUnLock_NotPaired");
                    );
                    return OnGeminiLock_DoUnLock_NotPaired;
                }
                else
                {

                    this->Unlock();
                    m_vLockData[ich_lock].Fresh();//m_iLockStatus = EnLockStatus_Ready;


                    LOG4CPLUS_INFO(LOG_OF_FGL, "DO g_FunGeminiLock.Unlock()");
                    EZ_LOGS(
                        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, "DO g_FunGeminiLock.Unlock()");
                    );
                    return OnGeminiLock_DoUnLock;
                }
            }
            else
            {
                //__trip;
                LOG4CPLUS_INFO(LOG_OF_FGL, "Want Unlock, but Unlocked");
                EZ_LOGS(
                    ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, "Want Unlock, but Unlocked");
                );
                //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "锁未被锁定， 请先锁后再开锁");
                return OnGeminiLock_DoUnLock_ButUnLocked;
            }
        }
        else
        {}

    }

    return iret;
}

