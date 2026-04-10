/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DevSerial.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: DevSerial.cpp 5884 2013-07-16 07:30:08Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-16 07:30:08 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <iostream>

#include "ez_system_api.h"
#include "../Logs.h"
#include "../Solar.h"

#include "EZThread.h"
#include "EZTimer.h"

#include "DevSerial.h"

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

//CDevSerial* CDevSerial::_instance = NULL;
//
//CDevSerial* CDevSerial::instance(void)
//{
//    if(NULL == _instance)
//    {
//        _instance = new CDevSerial();
//    }
//    return _instance;
//}
PATTERN_SINGLETON_IMPLEMENT(CDevSerial);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CDevSerial::CDevSerial() :CEZThread("CDevSerial", THREAD_PRIORITY_DEFAULT), m_sigData(1)
{
    ezserial_init();
}

CDevSerial::~CDevSerial()
{
    for (std::vector<DEV_SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (iter->m_pSerial != NULL)
        {
            ezserial_close(iter->m_pSerial);
            iter->m_pSerial = NULL;
            //m_vecSerial.erase(iter);
            //return 0;
        }

        if (iter->m_pBuffer != NULL)
        {
            delete iter->m_pBuffer;
        }

    }

    ezserial_cleanup();
}

EZTHREAD_BOOL CDevSerial::Start(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Attach(pObj, pProc) < 0)
    {
        ez_printf_error("attach error\n");

        return EZTHREAD_BOOL_FALSE;
    }

    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    // 监听热拔插设备信息
    StartHotPlug();

    // 默认不打开串口
#if 0
    if(m_vecSerial.size()<1)
    {
        SetDefaultConfig();
    }
#endif
    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CDevSerial::Stop(CEZObject * pObj, SIG_DevSerial_DATA::SigProc pProc)
{
    CEZLock _lock(m_MutexSig);

    if(m_sigData.Detach(pObj, pProc) == 0)
    {
        return EZTHREAD_BOOL_TRUE;
    }

#if 0
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
#else

    return EZTHREAD_BOOL_FALSE;
#endif

}

void CDevSerial::ThreadProc()
{
    while (m_bLoop)
    {
        // 目前直接用ezserial的线程转发数据
        //sleep(100);
        ez_sleep(1, 100);
    }
}

void CDevSerial::OnData(int id, uint8_t *buf, int length)
{
    //ez_printf_debug("CDevSerial::%s(%d, .., %d)\n", __FUNCTION__, id, length);

    int dataBuffered=0;

    // 缓存数据
    for (std::vector<DEV_SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (id == iter->id)
        {
            if (iter->m_stMaxBufferSize > 0)
            {
                //buffer
                dataBuffered = 1;

                if (iter->m_pBuffer == NULL)
                {
                    iter->m_pBuffer = new CezBuffer;
                }

                // 超时判断
                if (iter->m_msOndataTimeout>0// 设置了才做
                    && iter->m_pBuffer->Size()>0//有数据做
                    && ( (g_Solar.GetRunPeriodMs()-iter->m_msLastOndata) > iter->m_msOndataTimeout)// 超时
                   )
                {
                    //ez_printf_debug(":: %lu, %lu, %lu\n", g_Solar.GetRunPeriodMs(), iter->m_msLastOndata, iter->m_msOndataTimeout);

                    m_sigData(id, (char *)iter->m_pBuffer->Buf(), iter->m_pBuffer->Size());

                    iter->m_pBuffer->Reset();
                }

                iter->m_pBuffer->Append(buf, length);
                iter->m_msLastOndata = g_Solar.GetRunPeriodMs();

                // 长度判断
                if ((iter->m_pBuffer->Size())
                    >= (iter->m_stMaxBufferSize))
                {
                    m_sigData(id, (char *)iter->m_pBuffer->Buf(), iter->m_pBuffer->Size());

                    iter->m_pBuffer->Reset();
                }

                // 超时和长度都未满足， 则缓存
            }

            break;
        }//if (id == iter->id)
    }

    //没有使用缓存机制
    if (dataBuffered==0
        && length > 0
        && buf)
    {
        m_sigData(id, buf, length);
    } // if

}

static void CDevSerial_SerialCallback(int id,
                                      uint8_t *buf,
                                      int length)
{
    g_DevSerial.OnData(id, buf, length);
    // or
    //CDevSerial::instance()->OnData(id, buf, length);
}

int CDevSerial::SendData(int id, uint8_t *buf, int length)
{
    CEZLock _lock(m_MutexSerial);

    for (std::vector<DEV_SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (id == iter->id)
        {
            ez_printf_debug("%s : ID(%d)-L(%d)\n", __FUNCTION__, id, length);

            ezserial_putdata(iter->m_pSerial, buf, length);

            iter->m_msLastSend = g_Solar.GetRunPeriodMs();

            return length;
        }
    }

    return -1;
}

// m_pSerial will be modify
int CDevSerial::AddSerial(DEV_SERIAL_ATTR_T &SerialAttr)
{
    CEZLock _lock(m_MutexSerial);

    // close the old one
    for (std::vector<DEV_SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (SerialAttr.serial_dev_name == iter->serial_dev_name)
        {
            // close
            ezserial_close(iter->m_pSerial);
            m_vecSerial.erase(iter);

            break;
        }
    }

    SerialAttr.m_pSerial = ezserial_open(SerialAttr.serial_dev_name.c_str()
                                         , CDevSerial_SerialCallback
                                         , SerialAttr.id
                                         , SerialAttr.baud
                                         , SerialAttr.data_bits
                                         , SerialAttr.parity
                                         , SerialAttr.stop_bits);

    if (SerialAttr.m_pSerial)
    {
        m_vecSerial.push_back(SerialAttr);
        return 0;
    }

    ez_printf_error("ezserial_open(%s) failed:%s\n", SerialAttr.serial_dev_name.c_str(), ezserial_geterrormsg());

    return -1;
}

int CDevSerial::RemoveSerial(const std::string &strSerialName)
{
    CEZLock _lock(m_MutexSerial);

    for (std::vector<DEV_SERIAL_ATTR_T>::iterator iter=m_vecSerial.begin(); iter != m_vecSerial.end(); iter++)
    {
        if (strSerialName == iter->serial_dev_name)
        {
            ezserial_close(iter->m_pSerial);
            m_vecSerial.erase(iter);
            return 0;
        }
    }

    ez_printf_error("RemoveSerial(%s) not found.\n", strSerialName.c_str());

    return -1;
}
void CDevSerial::SetDefaultConfig()
{
    DEV_SERIAL_ATTR_T __cfg;

    __cfg.baud = 9600;
    __cfg.data_bits = 8;
    __cfg.parity = 0;
    __cfg.stop_bits = 1;                                                // 1, 15, 2
    __cfg.m_pSerial = NULL;

    __cfg.id = 33;              // conn at com33
    __cfg.serial_dev_name = "/dev/ttyO2";
    AddSerial(__cfg);

    __cfg.id = 34;              // conn at com??
    __cfg.serial_dev_name = "/dev/ttyO3";
    AddSerial(__cfg);

    __cfg.id = 35;              // conn at com??
    __cfg.serial_dev_name = "/dev/ttyO4";
    AddSerial(__cfg);
}

#include "DevHotPlug.h"

void CDevSerial::OnPartition(int iCMD, char *pPacket, int iCh, time_t msg_time)
{
    CEZLock l(m_LockLis);

    ez_printf_debug("CALLBACK - OnPartition: %d, 0x%0x, %d, %ld\n", iCMD, pPacket, iCh, msg_time);
}

void CDevSerial::StartHotPlug()
{
    //ez_printf_debug("StartHotPlug\n");

    g_DevHotPlug.Start(this, (CDevHotPlug::DevHotPlugSignalProc_t)&CDevSerial::OnPartition);
}

void CDevSerial::StopHotPlug()
{
    g_DevHotPlug.Stop(this, (CDevHotPlug::DevHotPlugSignalProc_t)&CDevSerial::OnPartition);
}

