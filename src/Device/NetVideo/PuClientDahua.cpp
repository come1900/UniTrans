/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuClientDahua.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: PuClientDahua.cpp 5884 2019-05-16 05:16:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-05-16 05:16:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Logs.h"
#include "../../Solar.h"

#include "PuClientDahua.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
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

// _FUNC_LiveStreamming_Support_PuSdk_DH

#define def_Default_Dev_DH_Host "192.168.5.117"
#define def_Default_Dev_DH_Port 37777
#define def_Default_Dev_DH_Account "admin"
#define def_Default_Dev_DH_Password "admin123"
#define def_Default_Dev_DH_Channel 1//
#define def_Default_Dev_DH_Stream 1//

CPuClientDahua::CPuClientDahua()
{
    m_lLoginId = 0;// Æä½Ó¿Ú¶¨ÒåµÄ: ³É¹¦·µ»Ø·Ç0(µÇÂ¼¾ä±ú)£¬Ê§°Ü·µ»Ø0¡£
}

CPuClientDahua::~CPuClientDahua()
{
    if (m_lLoginId != 0)
        CLIENT_Logout(m_lLoginId);
}

EZTHREAD_BOOL CPuClientDahua::Start(CEZObject * pObj, PuClientSignalProc_t pProc)
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    EZTHREAD_BOOL bRet = CreateThread();

    return bRet;
}

void CPuClientDahua::onTask()
{
    CPuClient::onTask();

    //
    doLogin();
    //
    //doHeartbeat();

    //
    doHARDDISK_STATE();
}

int CPuClientDahua::doLogin()
{
    // ÒÑ¾­µÇÂ¼
    if (m_lLoginId != 0)
    {
        return 0;
    }

    int iret = -1;

    BOOL bRet_CLIENT_Init = CLIENT_Init(NULL, (long int)NULL);
    if (!bRet_CLIENT_Init)
    {
        ez_printf_error("CLIENT_Init failed!\n");
    }
    else
    {
        CLIENT_SetAutoReconnect(NULL, (long int)NULL);
    }

    m_strHost = def_Default_Dev_DH_Host;
    m_iPort = def_Default_Dev_DH_Port;
    m_strAccount = def_Default_Dev_DH_Account;
    m_strPasswd = def_Default_Dev_DH_Password;
    //int iChannel = def_Default_Dev_DH_Channel; // 1-n ...
    //int iStream = def_Default_Dev_DH_Channel; // 1-n ...

    NET_DEVICEINFO_Ex deviceInfo;
    memset(&deviceInfo, 0, sizeof(NET_DEVICEINFO_Ex));
    int error = 0;

    ez_printf_debug("doLogin as:%s:%d, %s, %s\n", m_strHost.c_str(), m_iPort, m_strAccount.c_str(), m_strPasswd.c_str());
    m_lLoginId = CLIENT_LoginEx2(m_strHost.c_str(), m_iPort, m_strAccount.c_str(), m_strPasswd.c_str(), EM_LOGIN_SPEC_CAP_TCP, NULL, &deviceInfo, &error);

    if(m_lLoginId == 0)
    {
        // µÇÂ½Ê§°Ü
        std::string strError = "";
        ez_printf_error("CLIENT_LoginEx2(%d):%s\n", error, strError.c_str());
        iret = -1;
    }
    else
    {
        setRunStatus(runStatus_PuClient_Valid);
        ez_printf_debug("CLIENT_LoginEx2 success.\n");
        iret = 0;
    }

    return iret;
}

int CPuClientDahua::doHeartbeat()
{
    // Î´µÇÂ¼
    if (m_lLoginId == 0)
    {
        return -1;
    }

    int iret = -1;

    DHDEV_VERSION_INFO devInfo = {0};
    //DH_SDCARD_STATE sdInfo = {0};
    int nRetLen = 0;
    //DWORD  dwDiskNum = 0;

    BOOL bSuccess = CLIENT_QueryDevState(m_lLoginId, DH_DEVSTATE_SOFTWARE, (char *)&devInfo, sizeof(DHDEV_VERSION_INFO), &nRetLen);
    if(bSuccess && (nRetLen == sizeof(DHDEV_VERSION_INFO)))
    {
        iret = 0;
        ez_printf_debug("Call CLIENT_QueryDevState for DHDEV_VERSION_INFO success.\n");
    }
    else
    {
        ez_printf_error("Call CLIENT_QueryDevState  for DHDEV_VERSION_INFO failed.\n");
        iret = -2;
    }

    return iret;
}


int CPuClientDahua::doHARDDISK_STATE()
{
    // Î´µÇÂ¼
    if (m_lLoginId == 0)
    {
        return -1;
    }

    int iret = -1;

    DH_HARDDISK_STATE devInfo = {0};
    //DH_SDCARD_STATE sdInfo = {0};
    int nRetLen = 0;
    //DWORD  dwDiskNum = 0;

    BOOL bSuccess = CLIENT_QueryDevState(m_lLoginId, DH_DEVSTATE_DISK, (char *)&devInfo, sizeof(DH_HARDDISK_STATE), &nRetLen);
    if(bSuccess && (nRetLen == sizeof(DH_HARDDISK_STATE)))
    {
        iret = 0;
        ez_printf_debug("Call CLIENT_QueryDevState for DH_HARDDISK_STATE success.\n");
        //m_nCurDiskType = 0;
        DWORD dwDiskNum = devInfo.dwDiskNum;

        ez_printf_debug("DiskNum:%d.\n", dwDiskNum);
        for(int i=0; i<(int)dwDiskNum; i++)
        {
            ez_printf_debug("\t\tdwVolume    :%d\n",   devInfo.stDisks[i].dwVolume    ); //Ó²ÅÌµÄÈÝÁ¿
            ez_printf_debug("\t\tdwFreeSpace :%d\n",   devInfo.stDisks[i].dwFreeSpace );
            //// 0-sleep,1-live,2-error
            int __status = devInfo.stDisks[i].dwStatus&0xF;
            ARG_USED(__status);
        }
    }
    else
    {
        ez_printf_error("Call CLIENT_QueryDevState  for DH_HARDDISK_STATE failed.\n");
        iret = -2;
    }

    return iret;
}

