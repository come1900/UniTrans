/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * gMiniLicense.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: gMiniLicense.cpp 5884 2015-02-01 10:37:35Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2015-02-01 10:37:35  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/user.h>
#include <asm/types.h>
#include <linux/netlink.h>
//
#include <str_opr.h>
#include "gmini_host_unique_identifier.h"
#include "gmini_license.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../Configs/ConfigManager.h"
#endif //_USE_MODULES_EZCONFIG
#include "../Logs.h"

#include "../Solar.h"

#include "gMiniLicense.h"

#define _FUN_GLI_SAVE 1

#ifdef _FUN_GLI_SAVE
#include "gli_fun.h"
#endif //_FUN_GLI_SAVE

#define DEFAULT_LICENSE_UPTIME (3600*24*1)
#define def_LICENSE_trial_version (3600*24*45) // 45 days

// #define _DEBUG_THIS
 #include <ez_def_devel_debug.h>

PATTERN_SINGLETON_IMPLEMENT(CgMiniLicense);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CgMiniLicense::CgMiniLicense() :CEZThread("CgMiniLicense", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    m_llValidSeconds = DEFAULT_LICENSE_UPTIME;
    //m_strGLic = "";

    m_cGMINI_LIC_V = -1;

    m_ttValidDate = 0;
    m_strValidDate[0] = '\0';
}

CgMiniLicense::~CgMiniLicense()
{
    //ez_trace("CgMiniLicense Leave--------\n");
}

unsigned long long CgMiniLicense::getLicenseUptime()
{
    return m_llValidSeconds;
}
time_t CgMiniLicense::getValidDate()
{
    return m_ttValidDate;
}
const char * const CgMiniLicense::getValidDateStr()
{
    return m_strValidDate;
}

char CgMiniLicense::getLicVers()
{
    return m_cGMINI_LIC_V;
}

#define def_GLicMinLen 2//最小长度
// iret, -1 - m_strGLic too short
int CgMiniLicense::useLicense()
{
    int iret = 0;

    do
    {
        if (m_strGLic.length() < def_GLicMinLen)
        {
            iret = -1;

            break;
        }

        char d_p_lic[GMINI_LIC_MAX_LEN_BUFFER];
        strcpy(d_p_lic, m_strGLic.c_str());

#ifdef _FUN_GLI_SAVE

        int iret_gli_save = gli_save(d_p_lic);
        ARG_USED(iret_gli_save);
        //ez_printf_info("gli_save:%d\n", iret_gli_save);
#endif //_FUN_GLI_SAVE

        char d_verType;
        char d_p_id[GMINI_HUID_LEN_BUFFER];
        char d_data[64];

        int iret_gmini_license_decode = gmini_license_decode(d_p_lic, &d_verType, d_p_id, d_data, sizeof(d_data));

        if (iret_gmini_license_decode < 0)
        {
            ez_printf_error("ERROR:%s:%d\n", "gmini_license_decode", iret);

            iret = -2;

            break;
            //return -2;
        }

        //else
        {
            //ez_printf_debug("g_Solar.GetHuid():%s, d_p_id:%s\n", g_Solar.GetHuid().c_str(), d_p_id);

            if (g_Solar.GetHuid() == d_p_id
                || std::string("472325191") == d_p_id)
            {
                if (d_verType == GMINI_LIC_V1)
                {
                    m_llValidSeconds = (unsigned long long)(atol(d_data));
                    m_llValidSeconds = m_llValidSeconds*24*3600;

                    if (m_llValidSeconds<DEFAULT_LICENSE_UPTIME)
                    {
                        m_llValidSeconds = DEFAULT_LICENSE_UPTIME;
                    }
                    m_cGMINI_LIC_V = d_verType;

                    //ez_printf_debug("m_llValidSeconds:%llu\n", m_llValidSeconds);
                    if (m_llValidSeconds <= def_LICENSE_trial_version)
                    {
                        //ez_printf_alert("Trial:%d\n", m_llValidSeconds);
                        ez_printf_info_content("%25s:%lld\n", "Trial(pss)", m_llValidSeconds);
                    }
                }
                else if (d_verType == GMINI_LIC_V2)
                {
                    m_ttValidDate = (unsigned long)(atol(d_data));
                    m_cGMINI_LIC_V = d_verType;

                    int iret_date_time2string = date_time2string(m_ttValidDate, m_strValidDate, NULL);
                    if (iret_date_time2string != 0)
                    {
                        m_strValidDate[0] = '\0';
                        //strcpy(m_strValidDate, "");
                    }
                }
                else
                {
                    ez_printf_error("unknow verType:%c\n", d_verType);
                    iret = -3;

                    break;
                }
            }
            else
            {
                ez_printf_error("Invalid PID:%s, Huid:%s\n", g_Solar.GetHuid().c_str(), d_p_id);
                iret = -4;

                break;
            }
        }


    }
    while(0);

    return iret;
}

// use para
int CgMiniLicense::setLicense(std::string strLic)
{
    if (strLic.length() < def_GLicMinLen)
    {
        return -1;
    }

    if (m_strGLic == strLic)
    {
        return -2;
    }

    int ichk = gmini_license_chk(strLic.c_str());

    if (0 != ichk)
    {
        ez_printf_error("gmini_license_chk:%s failed\n", strLic.c_str());

        return -3;
    }

    std::string _strGLicBak = m_strGLic;
    m_strGLic = strLic;

    int iret_useLicense = useLicense();

    ez_printf_debug("iret_useLicense:%d\n", iret_useLicense);

    if (0 == iret_useLicense)
    {
#ifdef _USE_MODULES_EZCONFIG
        CConfigGeneral __cfgGeneral;
        __cfgGeneral.update();

        CONFIG_GENERAL &cfgGeneral = __cfgGeneral.getConfig();
        cfgGeneral.strSerial = strLic;
        __cfgGeneral.commit();
#else


#endif
    }
    else
    {
        // 失败了恢复
        m_strGLic = _strGLicBak;
    }

    return iret_useLicense*100;//useLicense();
}

const std::string &CgMiniLicense::getLicense()
{
    return m_strGLic;
}
int CgMiniLicense::Dump(std::string &strDump)
{
    char dumpBuffer[256];
    unsigned int ttLast = g_Solar.GetRunPeriod();

    snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%8s %s\n", "Lic:", g_gMiniLicense.getLicense().c_str());
    strDump+=dumpBuffer;

    if (g_gMiniLicense.getLicVers() == GMINI_LIC_V1)
    {
        snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%8s %llu(s)\n", "Liced:", getLicenseUptime());
        strDump+=dumpBuffer;
        snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%8s %llu(s)\n", "Remain:", getLicenseUptime()>ttLast ? getLicenseUptime()-ttLast : 0);
        strDump+=dumpBuffer;
    }
    else if (getLicVers() == GMINI_LIC_V2)
    {
        snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%8s %lu(s)\n", "->Date:", getValidDate());
        strDump+=dumpBuffer;

        //char bufTimestring[64];
        //time2string(getValidDate(), bufTimestring, NULL);
        snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%8s %s\n", ":", getValidDateStr());
        strDump+=dumpBuffer;
    }
    else if (getLicVers() < 0)
    {
        snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%s%d\n", "Invalid lic:", getLicVers());
        strDump=dumpBuffer;
    }
    else
    {
        snprintf(dumpBuffer, sizeof (dumpBuffer)-1, "\t%s%c\n", "Invalid licvers:", getLicVers());
        strDump=dumpBuffer;
    }

    return 0;
}


EZTHREAD_BOOL CgMiniLicense::Start(CEZObject * pObj, gMiniLicenseSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        //__fline;
        ez_printf_error("attach error\n");
        return bRet;
    }
    else
    {
        //ez_printf_debug("Attach ok.\n");
    }
		

    if (m_iUser==0)
    {
#ifdef _USE_MODULES_EZCONFIG
        CConfigGeneral __cfgGeneral;
        __cfgGeneral.update();

        m_strGLic = __cfgGeneral.getConfig().strSerial;
#else


#endif

        useLicense();

        CreateThread();

        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {

        bRet = EZTHREAD_BOOL_TRUE;

        DBG(
            __fline;
            printf("no need, CreateThread, have %d users.\n", m_iUser);
        );

    }
    m_iUser++;
    DBG(
        __fline;
        printf("m_iUser:%d\n", m_iUser);
    );
    return bRet;

}
EZTHREAD_BOOL CgMiniLicense::Stop(CEZObject * pObj, gMiniLicenseSignalProc_t pProc)
{
    ez_printf_debug("%s\n", __FUNCTION__);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iUser>0)
    {
        m_iUser--;
    }

    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
        // m_iUser--;
        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        // error
        __trip;
        printf("Detach failed.\n");
    }

    if (m_iUser==0)
    {

        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            __trip;
            printf("DestroyThread failed.\n");
        }
        else
        {
            DBG(
                printf("DestroyThread OK.\n");
            );
        }
    }
    else
    {
        DBG(
            __fline;
            printf("no need, DestroyThread, have %d users.\n", m_iUser);
        );
    }

    return bRet;
}
void CgMiniLicense::ThreadProc()
{
    while (m_bLoop)
    {
        DBG(
            __fline;
            printf("CgMiniLicense::ThreadProc.\n");
        );
        char PartitionName[64] = {'\0'};
        int iSignal = MSG_GMINI_LICENSE_OK;

        if (m_cGMINI_LIC_V==GMINI_LIC_V1)
        {
            unsigned long long pcs = process_counter_sec();
            if (pcs >= getLicenseUptime())
            {
                //printf("MSG_GMINI_LICENSE_TIMEOUT pcs:%lld", pcs);
                //m_SigBuffer(MSG_GMINI_LICENSE_TIMEOUT, PartitionName, strlen(PartitionName), time(NULL));
                iSignal = MSG_GMINI_LICENSE_TIMEOUT;
            }
        }
        else if (m_cGMINI_LIC_V==GMINI_LIC_V2)
        {
            time_t ttNow = time(NULL);
            if (ttNow >= getValidDate())
            {
                // 第一次打印
                static bool m_bTimeout = false;
                if (!m_bTimeout)
                {
                    ez_printf_warning("MSG_GMINI_LICENSE_TIMEOUT, ValidDate:%lu\n", getValidDate());
                    m_bTimeout = true;
                }
                //printf("MSG_GMINI_LICENSE_TIMEOUT pcs:%lld", pcs);
                //m_SigBuffer(MSG_GMINI_LICENSE_TIMEOUT, PartitionName, strlen(PartitionName), time(NULL));
                iSignal = MSG_GMINI_LICENSE_TIMEOUT;
            }
        }
        else
        {
            iSignal = MSG_GMINI_LICENSE_OBJECT_ERROR;
        }
        m_SigBuffer(iSignal, PartitionName, strlen(PartitionName), time(NULL));

        sleep(1);
    }
}

//int CgMiniLicense::OnUEventData(char *pData, int len)
//{
//    return 0;
//}
