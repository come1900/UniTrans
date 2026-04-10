/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunConsole.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunConsole.cpp 5884 2014-03-26 02:46:54Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-03-26 02:46:54  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ez_util_platform.h"/*ms_count*/

#include "../Solar.h"

#ifdef _DEV_NetSerialTCPSOCKETCONNECTOR
#include "../Device/NetSerial/src/NetSerial.h"
#endif //_DEV_NetSerialTCPSOCKETCONNECTOR

#include "../Maintain/GeneralAgent.h"

#ifdef EXAMPLE_THE_WEBSERVICE
#include "../WebService/AgentWebService.h"
#endif
#ifdef USE_PUBSERVICE
#include "../Function/PubService/AgentPubService.h"
#endif
#ifdef USE_SMSOVERHTTP
#include "../Function/SmsOverHttp/AgentSmsOverHttp.h"
#endif
#ifdef USE_InterUtcs_YiHeTech
#include "../Function/InterUtcs/YiHeTech/AgentInterUtcs_YiHeTech.h"
#endif
#ifdef _USE_InterThings_gLock
#include "../Function/InterThings/gLock/AgentInterThings_gLock.h"
#endif //_USE_InterThings_gLock
#ifdef USE_ASSOVERHTTP
#include "../Function/AssOverHttp/AgentAssOverHttp.h"
#endif
#if defined(USE_CUOVERHTTP)
#include "../Function/CuOverHttp/AgentCuOverHttp.h"
#endif
#ifdef USE_SC1ClientOverTcp
#include "../Function/SC1ClientOverTcp/AgentSC1ClientOverTcp.h"
#endif
#ifdef USE_PUOVERHTTP
#include "../Function/PuOverHttp/AgentPuOverHttp.h"
#endif

#ifdef FUNC_GEMINILOCK
#include "../Function/GeminiLock/FunGeminiLock.h"
#include "../Function/GeminiKey/GeminiKey.h"
#endif

#ifdef _DEV_NetSerialTCPSOCKETCONNECTOR
#include "../Device/NetSerial/src/NetSerial.h"
#endif //_DEV_NetSerialTCPSOCKETCONNECTOR

#ifdef _USE_DBAGENT_OCCI
#include "../DBAgent/ltUtcsDbAgent.h"
#endif //_DEV_NetSerialTCPSOCKETCONNECTOR

#if 0
//#ifdef _FUNC_PULL_STREAMMING_SERVER
#include "../Function/PullStreammingServer/AgentPullStreammingServer.h"
#endif //_FUNC_PULL_STREAMMING_SERVER

#if defined(_FUNC_gMiniLicense)
#include "../Maintain/gMiniLicense.h"
#endif//_FUNC_gMiniLicense
#ifdef _FUNC_PandoraMatrix
#include "../Function/Pandora/Matrix/AgentPandoraMatrix.h"
#endif
#ifdef _FUNC_PandoraNode
#include "../Function/Pandora/Node/AgentPandoraNode.h"
#endif
// 推流- 天翼看店
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_TykdMobileService)
#include "../Function/StreamPusher/StreamPusherTykd.h"
#endif // _FUNC_PullTykdMobileService

#include "FunConsole.h"

// do not define outside
//#define DUMP_CONSOLE_MSG (pObj->*pCallBack)
#define DUMP_CONSOLE_MSG (m_pObj->*m_pCallBack)

#ifndef _DEBUG_THIS
//  #define _DEBUG_THIS
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

static  CONSOLE_FUNC_NODE_T m_tFuncTable_CFunConsole[] = {
            //About this
            { LVL_SPRTR, "------",              NULL,                                         "FunConsole  -----------------------",},
            { LVL_USR,   CMD_CONSOLE_HELP,      (CONSOLE_PROC)&CFunConsole::FunctionHelp,     "Dump the help message!",},
            /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
            { LVL_USR,   CMD_CONSOLE_VERSION,   (CONSOLE_PROC)&CFunConsole::FC_Ver,         "Dump version.",},

            { LVL_USR,   "reboot",              (CONSOLE_PROC)&CFunConsole::FC_SysReboot,   "reboot",},
            { LVL_USR,   "restart",              (CONSOLE_PROC)&CFunConsole::FC_SysRestart,   "restart",},
            { LVL_USR,   "suicide",                (CONSOLE_PROC)&CFunConsole::FC_Suicide,          "exit this process",},

            { LVL_USR,   "timer",                (CONSOLE_PROC)&CFunConsole::FC_Timer,          "show timer info",},
            { LVL_USR,   "thread",                (CONSOLE_PROC)&CFunConsole::FC_Thread,          "show thread info",},
            { LVL_USR,   "status",                (CONSOLE_PROC)&CFunConsole::FC_Status,          "show status info",},

#if defined(FUNC_GEMINILOCK)

            {
                LVL_USR, "glock",                 (CONSOLE_PROC)&CFunConsole::FC_gLock,       "Function Lock",
            },
#endif //FUNC_GEMINILOCK
#if defined(_FUN_FunSerial)

            {
                LVL_USR, "tty",                 (CONSOLE_PROC)&CFunConsole::FC_Serial,       "Function Serial",
            },
#endif //FUNC_GEMINILOCK

#if defined(USE_PUOVERHTTP)

            {
                LVL_USR, "spum",        (CONSOLE_PROC)&CFunConsole::FC_SendPuMsg,     "send pu mesage",
            },
#endif //USE_PUOVERHTTP
#if defined(USE_CUOVERHTTP)

            {
                LVL_USR, "scum",        (CONSOLE_PROC)&CFunConsole::FC_SendCuMsg,     "send cu mesage",
            },
#endif //USE_CUOVERHTTP
#if defined(USE_SC1ClientOverTcp)

            {
                LVL_USR, "scc",        (CONSOLE_PROC)&CFunConsole::FC_SendSC1Msg,     "send signal client mesage",
            },
#endif //USE_CUOVERHTTP
#ifdef USE_SMSOVERHTTP
            { LVL_USR, "smss",          (CONSOLE_PROC)&CFunConsole::FC_SmsSend,       "sms send msg to all cli",},
#endif //USE_SMSOVERHTTP
#ifdef _USE_DBAGENT_OCCI
            { LVL_USR, "dbagent",            (CONSOLE_PROC)&CFunConsole::FC_dbAgent,       "database test",},
#endif //USE_SMSOVERHTTP
#if 0
            //#ifdef _FUNC_PULL_STREAMMING_SERVER
            { LVL_USR, "pss",            (CONSOLE_PROC)&CFunConsole::FC_PullStreammingServer,       "PullStreammingServer",},
#endif //_FUNC_PULL_STREAMMING_SERVER
#ifdef _FUNC_gMiniLicense
            { LVL_HID, "lic",            (CONSOLE_PROC)&CFunConsole::FC_gMiniLicense,       "Gmini License",},
#endif //_FUNC_gMiniLicense

#if defined(_FUNC_PandoraMatrix)

            {
                LVL_USR, "pma",            (CONSOLE_PROC)&CFunConsole::FC_PandoraMatrix,       "PandoraMatrix",
            },
#endif //_FUNC_PandoraMatrix
#if defined(_FUNC_PandoraNode)

            {
                LVL_USR, "pno",            (CONSOLE_PROC)&CFunConsole::FC_PandoraNode,       "PandoraNode",
            },
#endif //_FUNC_PandoraNode
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_TykdMobileService)

            {
                LVL_USR, "pty",            (CONSOLE_PROC)&CFunConsole::FC_PushTykd,       "PushTykd",
            },
#endif //_FUNC_PullTykdMobileService

#if defined(_FUNC_LiveStatusServer)
            {
                LVL_USR, "pst",            (CONSOLE_PROC)&CFunConsole::FC_PublishStatus,       "publishstatus",
            },
#endif //_FUNC_LiveStatusServer

        };

#define CONSOLE_FUNC_NODE_ITEMS sizeof(m_tFuncTable_CFunConsole)/sizeof(CONSOLE_FUNC_NODE_T)
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int CFunConsole::OnCommand(int argc, char * argv[])
{
    if (/*pObj && pCallBack && */argc>0 && argv[0])
    {
        DBG(
            DUMP_CONSOLE_MSG("CFunConsole::OnCommand >>>>>>>>>>>>> \n");
        );
    }
    else
    {
        DUMP_CONSOLE_MSG("Invalid Argument.\n");

        return -1;
    }

    for (unsigned int i=0; i<CONSOLE_FUNC_NODE_ITEMS; i++)
    {
        if (strncmp(argv[0], m_tFuncTable_CFunConsole[i].cmdStr, strlen(argv[0])) == 0)
        {
            (this->*m_tFuncTable_CFunConsole[i].proc)(argc, argv/*, pObj, pCallBack*/);

            return 0;
        }
    }

    return -1;
}
CFunConsole::CFunConsole(CEZObject *pObj, CONSOLE_CALLBACK pCallBack)
{
    m_pObj = pObj;
    m_pCallBack = pCallBack;

    //Start();
    m_TimerAutoFunc.Start(this
                          , (EZ_TIMER_PROC)&CFunConsole::TimerProcLive
                          , 0
                          , 1 * 1000 /*每秒执行一次，请勿修改*/
                          , 0
                          , 10*1000
                         );


    m_bAuto_iDev_Status = false;
}

CFunConsole::~CFunConsole()
{
    m_TimerAutoFunc.Stop();
}
void CFunConsole::TimerProcLive()
{
    // 自动打印状态
    if (m_bAuto_iDev_Status)
    {
        FC_Status(INTERNAL_CALL_OF_ARGC, 0);
    }

}

int CFunConsole::Start()
{

    return 0;
}


const std::string & CFunConsole::HelperInfo(int iLevel)
{
    m_strHelper = "CFunConsole::HelperInfo";

    return m_strHelper;
}

void CFunConsole::FunctionHelp(int argc, char * argv[])
{
    for(unsigned int i=0; i<CONSOLE_FUNC_NODE_ITEMS; i++)
    {
        if (m_tFuncTable_CFunConsole[i].level == LVL_SPRTR)
        {
            DUMP_CONSOLE_MSG("------------------%s\n",m_tFuncTable_CFunConsole[i].hlpStr);
            continue;
        }
        else if (m_tFuncTable_CFunConsole[i].level == LVL_NONE)
        {
            break;
        }
        else if (m_tFuncTable_CFunConsole[i].level == LVL_HID)
        {
            continue;
        }
        else
        {
            DUMP_CONSOLE_MSG("%-9s         %s\n", m_tFuncTable_CFunConsole[i].cmdStr, m_tFuncTable_CFunConsole[i].hlpStr);
        }
    }

    DUMP_CONSOLE_MSG("-----------------------------------------------------\n");
    DUMP_CONSOLE_MSG("`cmd -h` for details.\n");
}


void CFunConsole::FC_Ver(int argc, char * argv[])
{
    time_t ttStart =  g_Solar.GetStartTime();
    unsigned long ttLast = g_Solar.GetRunPeriod();

    DUMP_CONSOLE_MSG("Sys Ver&Info:\n%s\n", TOKEN_Console_01);
    DUMP_CONSOLE_MSG("%s\n", g_Solar.GetCopyRights());
    DUMP_CONSOLE_MSG("%s\n", g_Solar.GetDesc());
    DUMP_CONSOLE_MSG("$ Solar: v %s %s %s %s Exp $\n\n", _VERSION, __DATE__, __TIME__, _CONNECT_INFO_STRING);

    DUMP_CONSOLE_MSG("\t%8s : %s\n", "Ver", g_Solar.GetVersion().c_str());
#ifdef _FUNC_gMiniLicense

    DUMP_CONSOLE_MSG("\t%8s : %s\n", "HUID", g_Solar.GetHuid().c_str());
#endif //_FUNC_gMiniLicense

    DUMP_CONSOLE_MSG("\t%8s : %s", "Start", ctime(&ttStart));
    //DUMP_CONSOLE_MSG("\t%-8s%lu(h) %lu(m) %lu(s) -- %lu(sec)\n", "Runed:", (unsigned long)(ttLast/3600), (unsigned long)(ttLast>3600?( (ttLast-( (ttLast/3600)*3600))/60):(ttLast/60)), (unsigned long)(ttLast%60), (unsigned long)ttLast);
    DUMP_CONSOLE_MSG("\t%8s : %lu(d) %lu(h) %lu(m) %lu(s) -- %lu(sec)\n", "Runed", (ttLast/(3600*24)), (ttLast/3600), (ttLast>3600?( (ttLast-( (ttLast/3600)*3600))/60):(ttLast/60)), (ttLast%60), ttLast);
#if 0//defined(_FUNC_gMiniLicense)

    DUMP_CONSOLE_MSG("\t%-8s%llu(s)", "Lic:", g_gMiniLicense.getLicenseUptime());
    DUMP_CONSOLE_MSG(" -- %s%llu(s)\n", "Remain:", g_gMiniLicense.getLicenseUptime()>ttLast ? g_gMiniLicense.getLicenseUptime()-ttLast : 0);
#endif//_FUNC_gMiniLicense

    DUMP_CONSOLE_MSG("\n\t%8s\n", "Function");
#ifdef _USE_MODULES_EZCONFIG

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_USE_MODULES_EZCONFIG");
#endif //_USE_MODULES_EZCONFIG
#if defined(_USE_LOG4CPP)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_USE_LOG4CPP");
#endif //_USE_LOG4CPP

#if defined(EXAMPLE_SIGNAL_ONDATA)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_SIGNAL_ONDATA");
#endif //EXAMPLE_SIGNAL_ONDATA
#if defined(EXAMPLE_SOCKET_STATUS_AGENT)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_SOCKET_STATUS_AGENT");
#endif //EXAMPLE_SOCKET_STATUS_AGENT
#if defined(_FUNC_CONSOLE_OVERTCP)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_CONSOLE_OVERTCP");
#endif //_FUNC_CONSOLE_OVERTCP
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETSERVER)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_GENERALAGENTTCPSOCKETSERVER");
#endif //EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR");
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#if defined(EXAMPLE_SOCKET_STATUS_AGENT)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_SOCKET_STATUS_AGENT");
#endif //EXAMPLE_SOCKET_STATUS_AGENT
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETSERVER)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_GENERALAGENTTCPSOCKETSERVER");
#endif //EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#if defined(EXAMPLE_GENERALAGENTHTTPSERVER)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_GENERALAGENTHTTPSERVER");
#endif //EXAMPLE_GENERALAGENTHTTPSERVER
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR");
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#if defined(EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR)

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR");
#endif //EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR

#ifdef EXAMPLE_THE_WEBSERVICE

    DUMP_CONSOLE_MSG("\t\t :%s\n", "EXAMPLE_THE_WEBSERVICE");
#endif
#ifdef USE_SMSOVERHTTP

    DUMP_CONSOLE_MSG("\t\t :%s\n", "USE_SMSOVERHTTP");
#endif
#ifdef USE_ASSOVERHTTP

    DUMP_CONSOLE_MSG("\t\t :%s\n", "USE_ASSOVERHTTP");
#endif
#ifdef USE_PUBSERVICE

    DUMP_CONSOLE_MSG("\t\t :%s\n", "USE_PUBSERVICE");
#endif
#ifdef USE_CUOVERHTTP

    DUMP_CONSOLE_MSG("\t\t :%s\n", "USE_CUOVERHTTP");
#endif
#ifdef USE_PUOVERHTTP

    DUMP_CONSOLE_MSG("\t\t :%s\n", "USE_PUOVERHTTP");
#endif
#ifdef FUNC_GEMINILOCK

    DUMP_CONSOLE_MSG("\t\t :%s\n", "FUNC_GEMINILOCK");
    // interface of glock
#ifdef _USE_InterThings_gLock

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_USE_InterThings_gLock");
#endif
#endif
#ifdef USE_InterUtcs_YiHeTech

    DUMP_CONSOLE_MSG("\t\t :%s\n", "USE_InterUtcs_YiHeTech");
#endif
#if 0
    //#ifdef _FUNC_PULL_STREAMMING_SERVER

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_PULL_STREAMMING_SERVER");
#endif //_FUNC_PULL_STREAMMING_SERVER

#ifdef _FUNC_PandoraMatrix

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_PandoraMatrix");
#endif //_FUNC_PandoraMatrix
#ifdef _FUNC_PusherTykdServer

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_PusherTykdServer");
#endif //_FUNC_PusherTykdServer
#ifdef _FUNC_PandoraNode

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_PandoraNode");
#endif //_FUNC_PandoraNode
#ifdef _FUNC_PullTykdMobileService

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_PullTykdMobileService");
#endif //_FUNC_PullTykdMobileService
    // 拉流相关
    //_USE_PssServerPlayUrlHXReq // 使用getPssServerPlayUrlHX 接口 否则使用getPlayUrlHX
#ifdef _USE_PssServerPlayUrlHXReq

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_USE_PssServerPlayUrlHXReq");
#endif //_USE_PssServerPlayUrlHXReq
    //_FUN_PssServerPlayUrlHXReq_NoLogin //_USE_PssServerPlayUrlHXReq接口不需要登录
#ifdef _FUN_PssServerPlayUrlHXReq_NoLogin

    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUN_PssServerPlayUrlHXReq_NoLogin");
#endif //_FUN_PssServerPlayUrlHXReq_NoLogin
    //_FUNC_PssServerPlayUrlHXReq_WithAuthPara 带有认证参数
#ifdef _FUNC_PssServerPlayUrlHXReq_WithAuthPara
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_PssServerPlayUrlHXReq_WithAuthPara");
#endif //_FUNC_PssServerPlayUrlHXReq_WithAuthPara

// 在流媒体系统中是不是支持man 用户配置
#ifdef _SUPPORT_CFG_PSSIXMAN
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_SUPPORT_CFG_PSSIXMAN");
#endif
// 天翼看店ws地址是否支持用户配置， 某些不需要天翼看店地址的流媒体系统去除
#ifdef _SUPPORT_CFG_TYKDWSURL
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_SUPPORT_CFG_TYKDWSURL");
#endif
// 推流状态服务
#ifdef _FUNC_LiveStatusServer
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_LiveStatusServer");
#endif

//
//2019-6-14 9:45:00
//主动上报状态，目前用在：
//09.02.Makefile.LiveStreamming.newMedia
#if defined(_Func_CCourier_StreamPusherTykd)
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_Func_CCourier_StreamPusherTykd");
#endif //_Func_CCourier_StreamPusherTykd
// 上报地址可配置 for _Func_CCourier_StreamPusherTykd
#if defined(_SUPPORT_CFG_StatusUploadWS)
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_SUPPORT_CFG_StatusUploadWS");
#endif //_SUPPORT_CFG_StatusUploadWS

// 返回录像空间
#if defined(_FUNC_GetUsedSpace)
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_FUNC_GetUsedSpace");
#endif //_FUNC_GetUsedSpace

// 两种查找方式
#if defined(_Func_queryRecord)
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_Func_queryRecord");
#endif //_Func_queryRecord

// 使用PID控制单例运行
#if defined(_USE_PID)
    DUMP_CONSOLE_MSG("\t\t :%s\n", "_USE_PID");
#endif //_Func_queryRecord

}

void CFunConsole::FC_SysReboot(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("iDev_SysReboot ...\n");
    g_Solar.Reboot();
}

void CFunConsole::FC_SysRestart(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("iDev_SysRestart ...\n");
    g_Solar.Restart();
}

void CFunConsole::FC_Suicide(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("iDev_Suicide ...\n");

    exit(0);

    return;
}

void CFunConsole::FC_Status(int argc, char * argv[])
{
    static int iDumpMode = enLogLevelShow_contes;
    ARG_USED(iDumpMode);

    if (INTERNAL_CALL_OF_ARGC != argc)
    {
        DUMP_CONSOLE_MSG("FC_Status auto ...\n");

        m_bAuto_iDev_Status = !m_bAuto_iDev_Status;
        // 自动处理， 这里直接返回先

        // 打印模式
        // 0 -- 默认模式，概要
        // 100 -- 详细模式，概要
        if (argc>1 && 0==strcmp(argv[1], "a"))
        {
            iDumpMode = enLogLevelShow_detail;
        }
        else
        {
            iDumpMode = enLogLevelShow_contes;
        }

        return ;
    }

    DUMP_CONSOLE_MSG("FC_Status ...\n");

    time_t ttStart =  g_Solar.GetStartTime();
    unsigned int ttLast = g_Solar.GetRunPeriod();

    DUMP_CONSOLE_MSG(" Sys Status:\n%s\n", TOKEN_Console_01);
    DUMP_CONSOLE_MSG("\t%-8s%s\n", "Ver:", g_Solar.GetVersion().c_str());
    DUMP_CONSOLE_MSG("\t%-8s%s", "Start:", ctime(&ttStart));
    DUMP_CONSOLE_MSG("\t%-8s%ld(h) %ld(m) %ld(s) -- %ld(sec)\n", "Runed:", ttLast/3600, (ttLast>3600?( (ttLast-( ((int)(ttLast/3600))*3600))/60):(ttLast/60)), ttLast%60, ttLast);
#if defined(_FUNC_gMiniLicense)

    DUMP_CONSOLE_MSG("\t%-8s%llu(s)", "Lic:", g_gMiniLicense.getLicenseUptime());
    DUMP_CONSOLE_MSG(" -- %s%llu(s)\n", "Remain:", g_gMiniLicense.getLicenseUptime()>ttLast ? g_gMiniLicense.getLicenseUptime()-ttLast : 0);
#endif//_FUNC_gMiniLicense

    DUMP_CONSOLE_MSG("\n Socket Status:\n%s\n", TOKEN_Console_01);
    //static_cast<CMaintainHandler&>(Handler()).List(this);

    std::string strDump;

    g_MaintainAgent.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());


#ifdef EXAMPLE_THE_WEBSERVICE

    g_AgentWebService.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
#ifdef USE_SMSOVERHTTP

    g_AgentSmsOverHttp.GetHandler()->List(strDump, iDumpMode);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
#ifdef USE_InterUtcs_YiHeTech

    g_AgentInterUtcs_YiHeTech.GetHandler()->List(strDump, iDumpMode);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
#ifdef USE_ASSOVERHTTP

    g_AgentAssOverHttp.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
#ifdef USE_PUBSERVICE

    g_AgentPubService.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif

#ifdef USE_CUOVERHTTP

    g_AgentCuOverHttp.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
#ifdef USE_PUOVERHTTP

    g_AgentPuOverHttp.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
#ifdef USE_SC1ClientOverTcp

    g_AgentSC1ClientOverTcp.GetHandler()->List(strDump);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif

#ifdef _USE_InterThings_gLock

    g_AgentInterThings_gLock.GetHandler()->List(strDump, iDumpMode);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif //_USE_InterThings_gLock

#ifdef _FUNC_PandoraMatrix

    g_AgentPandoraMatrix.GetHandler()->List(strDump, iDumpMode);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif

#ifdef _FUNC_PandoraNode

    g_AgentPandoraNode.GetHandler()->List(strDump, iDumpMode);
    DUMP_CONSOLE_MSG(strDump.c_str());
#endif
}

void CFunConsole::FC_Timer(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("iDev_Timer ...\n");

    std::string __str;
    g_TimerManager.DumpTimers(__str);
    DUMP_CONSOLE_MSG(__str.c_str());
    return;
}

void CFunConsole::FC_Thread(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("FC_Thread ...\n");

    std::string __str;
    // THREAD_PRIORITY_JWORKER 只统计不打印
    g_ThreadManager.DumpThreads(__str, THREAD_PRIORITY_JWORKER);

    DUMP_CONSOLE_MSG(__str.c_str());

    return;
}

#if defined(FUNC_GEMINILOCK)
void CFunConsole::FC_gLock(int argc, char * argv[])
{
    int iret;
    DUMP_CONSOLE_MSG("FC_gLock ...\n");

    if (argc>1 && 0==strcmp(argv[1], "s"))
    {
#if 0
        iret = g_FunGeminiLock.Start();
        DUMP_CONSOLE_MSG("g_FunGeminiLock.Start ret : %d\n", iret);
#else

        iret = 0;
        DUMP_CONSOLE_MSG("start it forever now: %d\n", iret);
#endif

    }
    else if (argc>1 && 0==strcmp(argv[1], "t"))
    {
#if 0
        iret = g_FunGeminiLock.Stop();
        DUMP_CONSOLE_MSG("g_FunGeminiLock.Stop ret : %d\n", iret);
#else

        iret = 0;
        DUMP_CONSOLE_MSG("start it forever now: %d\n", iret);
#endif

    }
    else if (argc>1 && 0==strcmp(argv[1], "w"))
    {
        int ch = argc>2?atoi(argv[2]):0;
        int onoff = argc>3?atoi(argv[3]):1;

        iret = g_FunGeminiLock.SetOut(ch, onoff);

        DUMP_CONSOLE_MSG("g_FunGeminiLock.SetOut(%d, %d) ret : %d\n", ch, onoff, iret);
    }
    else if (argc>1 && 0==strcmp(argv[1], "u"))
    {
        iret = g_FunGeminiLock.FreshStatus(1);
        DUMP_CONSOLE_MSG("g_FunGeminiLock.FreshStatus(1) ret : %d\n", iret);
    }
    else if (argc>1 && 0==strcmp(argv[1], "n"))
    {
        //全开：
        unsigned char buff_all_on[] = {0x55, 0x01, 0x13, 0x00, 0x00, 0xFF, 0xFF, 0x67};
        //全关：
        unsigned char buff_all_off[] = {0x55, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, 0x69};
        //unsigned int ulCmd = 0;
        unsigned char *send_buff = buff_all_on;
        int send_buff_len = sizeof(buff_all_off);
        static int iTimes = 0;
        if (iTimes%2==0)
        {
            send_buff = buff_all_on;
        }
        else
        {
            send_buff = buff_all_off;
        }

#ifdef _DEV_NetSerialTCPSOCKETCONNECTOR
        char strName[256];
        snprintf(strName, sizeof(strName)-1, "%s:%d", DEFAULT_NET_SERIAL_HOST, DEFAULT_NET_SERIAL_HOST_PORT);
        g_NetSerial.Send2Socket(strName, send_buff, send_buff_len);
#endif //_DEV_NetSerialTCPSOCKETCONNECTOR

        DUMP_CONSOLE_MSG("send_buff_len:%d\n", send_buff_len);
        iTimes++;
    }
    // key lock 的使用
    else if (argc>1 && 0==strcmp(argv[1], "k"))
    {
        GeminiKeys __KeysGen;
        const  char * p_GeminiKey = argc>2?argv[2]:"wZWKi60AyCDP1+96R+rccFwel58W2bjtoHVPjkeqmG9TVUXZUlcID0QKMvynLZZS+O7aLUKpy3G3kGbXmnMby+9TtGAAp3yhwyMhki/m368kQJUUBvjtBgmKaBlbie6UciW2xRnPn31UeGEdAJt5Yc1bGXrePzWFMLizVzVNtNASJrBDwFxxd6bg1Mp2RV+gNRz/aSCVJFjjqRJf5xh1eG/f7Z2o5jsL5Vbd9jB+z5bv8X2RwmJADHrhXM7peL6iYL4wAk1BmDMCLy/9l/gkmRBAdnoH4ZUQ2//KCSoQy32VGCyEI3ZQQIHRcd/PUYsBKxGwWRoalOJZJe/mu8B5HiZAjBtJKhQ979XHAwAfI2yS/ou66oGXxIsMc8jg8zutn88dybeiRoID0LBdmSQtv7bWaCmaRycG/umQB71YXPUInQK5tLY+4WtPCB9XWoR0iSWvLztIqIu1Mff4amE4MwaajMxY9uGVZgZaxLmfbLceat2Vf0opVzgAJ0fFYvDwV32l6/sTDz1phKxcxe6nHGBcryYSX6XCQl0Zwcw0mwpln1Dt+dWlWKIksDtdIjy4VctO+mD1XzyfcNzlrpbcytmmftZ9+DTKgyrrpi95Q1zk3Z620mGrTA==";

        iret = g_FunGeminiLock.OnKey_Chk(0, p_GeminiKey, strlen(p_GeminiKey), &__KeysGen);
        DUMP_CONSOLE_MSG("g_FunGeminiLock.OnKeyData(1) ret : %d\n", iret);

        if (iret >=0)
        {
            std::string strKeys;
            __KeysGen.dump(strKeys);

            DUMP_CONSOLE_MSG("g_FunGeminiLock.OnKeyData(1) strKeys : %s\n", strKeys.c_str());
        }

    }
    else if (argc>1 && 0==strcmp(argv[1], "g"))
    {
        const char * p_GeminiKey = argc>2?argv[2]:"wZWKi60AyCDP1+96R+rccFwel58W2bjtoHVPjkeqmG9TVUXZUlcID0QKMvynLZZS+O7aLUKpy3G3kGbXmnMby+9TtGAAp3yhwyMhki/m368kQJUUBvjtBgmKaBlbie6UciW2xRnPn31UeGEdAJt5Yc1bGXrePzWFMLizVzVNtNASJrBDwFxxd6bg1Mp2RV+gNRz/aSCVJFjjqRJf5xh1eG/f7Z2o5jsL5Vbd9jB+z5bv8X2RwmJADHrhXM7peL6iYL4wAk1BmDMCLy/9l/gkmRBAdnoH4ZUQ2//KCSoQy32VGCyEI3ZQQIHRcd/PUYsBKxGwWRoalOJZJe/mu8B5HiZAjBtJKhQ979XHAwAfI2yS/ou66oGXxIsMc8jg8zutn88dybeiRoID0LBdmSQtv7bWaCmaRycG/umQB71YXPUInQK5tLY+4WtPCB9XWoR0iSWvLztIqIu1Mff4amE4MwaajMxY9uGVZgZaxLmfbLceat2Vf0opVzgAJ0fFYvDwV32l6/sTDz1phKxcxe6nHGBcryYSX6XCQl0Zwcw0mwpln1Dt+dWlWKIksDtdIjy4VctO+mD1XzyfcNzlrpbcytmmftZ9+DTKgyrrpi95Q1zk3Z620mGrTA==";
        iret = g_FunGeminiLock.OnKey_Use(0, p_GeminiKey, strlen(p_GeminiKey), -1);

        DUMP_CONSOLE_MSG("g_FunGeminiLock.OnGeminiKey() ret : %d\n", iret);
    }
    else
    {
        DUMP_CONSOLE_MSG("Support CMD:\n");
        DUMP_CONSOLE_MSG("\t w -- g_FunGeminiLock.SetOut(ch, onoff)\n");
        DUMP_CONSOLE_MSG("\t u -- g_FunGeminiLock.FreshStatus(1);\n");
        DUMP_CONSOLE_MSG("\t n -- NetSerial;\n");

        DUMP_CONSOLE_MSG("\t k -- OnKeyData;\n");
        DUMP_CONSOLE_MSG("\t g -- OnGeminiKey;\n");

        DUMP_CONSOLE_MSG("Your input:\n");

        for (int ii=0; ii<argc; ii++)
        {
            DUMP_CONSOLE_MSG("\t %d ... %s\n", ii, argv[ii]);
        }
    }
}
#endif //FUNC_GEMINILOCK

#if defined(_FUN_FunSerial)
#include "../Function/GPorts/FunSerial.h"
void CFunConsole::FC_Serial(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("%s ...\n", __FUNCTION__);

    DUMP_CONSOLE_MSG("argc:%d \n", argc);

    if (argc>1 && 0==strcmp(argv[1], "s"))
    {
        // tty cmd chn data ...
        if (argc < 4)
        {
            DUMP_CONSOLE_MSG("tty s SerialCh, data ...\n");
            return;
        }

        //const char *pSerialChn = argv[2]; // 0 1 2
        int chn = atoi(argv[2]);

        int iLen = argc-3;
        char dataSend[64] = {0};
        int jj=0;

        for (int ii=3; ii<argc; ii++)
        {
            unsigned int nValude = 0;
            sscanf(argv[ii], "%x", &nValude);

            dataSend[jj] = (char)(nValude&0xff);
            DUMP_CONSOLE_MSG(" %0x", dataSend[jj]);

            jj++;
        }
        DUMP_CONSOLE_MSG(" ::%d data\n", iLen);

        int iSnded = g_FunSerial.Sendto(chn, dataSend, iLen);
        //int iSnded = g_FunSerial.Sendto_Modbus(atoi( pSerialChn ), dataSend, iLen);
        //int iSnded = g_FunSerial.Sendto_Modbus_TNP(atoi( pSerialChn ), dataSend, iLen);

        DUMP_CONSOLE_MSG("Serial.Sendto(CH(%d), 0x%0x ..., L(%d)):%d\n", chn, dataSend[0], iLen, iSnded);
    }
    else if (argc>1 && 0==strcmp(argv[1], "mb"))
    {
        // tty cmd chn data ...
        if (argc < 4)
        {
            DUMP_CONSOLE_MSG("tty ms SerialCh, data ...\n");
            return;
        }

        //const char *pSerialChn = argv[2]; // 0 1 2
        int chn = atoi(argv[2]);

        int iLen = argc-3;
        char dataSend[64] = {0};
        int jj=0;

        for (int ii=3; ii<argc; ii++)
        {
            unsigned int nValude = 0;
            sscanf(argv[ii], "%x", &nValude);

            dataSend[jj] = (char)(nValude&0xff);
            DUMP_CONSOLE_MSG(" %0x", dataSend[jj]);

            jj++;
        }
        DUMP_CONSOLE_MSG(" ::%d data\n", iLen);

        //int iSnded = g_FunSerial.Sendto(chn, dataSend, iLen);
        int iSnded = g_FunSerial.Sendto_Modbus(chn, dataSend, iLen);
        //int iSnded = g_FunSerial.Sendto_Modbus_TNP(chn, dataSend, iLen);

        DUMP_CONSOLE_MSG("Serial.Sendto_Modbus(CH(%d), 0x%0x ..., L(%d)):%d\n", chn, dataSend[0], iLen, iSnded);
    }
    else if (argc>1 && 0==strcmp(argv[1], "cio"))
    {
        if (argc < 5)
        {
            DUMP_CONSOLE_MSG("setIOOut, tty cio SerialCh, ch, onoff\n");
            return;
        }

        int serialId = atoi(argv[2]);
        int channel = atoi(argv[3]);
        int on_off = atoi(argv[4]);

        int iSnded = g_FunSerial.pubIOOut(serialId, channel, on_off);

        DUMP_CONSOLE_MSG("tty setIOOut(%d):%d, %d, %d\n", iSnded, serialId, channel, on_off);
    }
    // 键盘
    // 打印字符：a
    // tty kb 1 4 0
    // 打印字符串
    // tty kb 1 hello
    else if (argc > 1 && 0 == strcmp(argv[1], "kb"))
    {
        if (argc < 4)
        {
            DUMP_CONSOLE_MSG("kb SerialCh, usualKey, controlKey\n");
            return;
        }

        int serialId = atoi(argv[2]);
        int usualKey = atoi(argv[3]);

        int controlKey = 0;

        if (argc > 4)
        {
            controlKey = atoi(argv[4]);
        }

        if (usualKey == 0)
        {
            int iSnded = g_FunSerial.Sendto_CH9329_String(serialId, argv[3], strlen(argv[3]));

            DUMP_CONSOLE_MSG("tty onKb String(%d):%s(%d)\n", iSnded, argv[3], strlen(argv[3]));
        }
        else
        {

            int iSnded = g_FunSerial.Sendto_CH9329_keyVal(serialId, usualKey, controlKey);

            DUMP_CONSOLE_MSG("tty onKb(%d):%d, 0x%0x, 0x%0x\n", iSnded, serialId, usualKey, controlKey);
        }
    }
    // 鼠标
    // 向下移动 5 个像素点
    // tty ms 1 0 0 5 0
    else if (argc > 1 && 0 == strcmp(argv[1], "ms"))
    {
        if (argc < 6)
        {
            DUMP_CONSOLE_MSG("ms serialId, mouseKey, xMove, yMove, rollerMove\n");
            return;
        }

        int serialId = atoi(argv[2]);
        unsigned char mouseKey = atoi(argv[3]);
        unsigned char xMove = atoi(argv[4]);
        unsigned char yMove = atoi(argv[5]);
        unsigned char rollerMove = atoi(argv[6]);
        int iSnded = g_FunSerial.Sendto_CH9329_mouseKey(serialId,
                                                        mouseKey,
                                                        xMove,
                                                        yMove,
                                                        rollerMove);

        DUMP_CONSOLE_MSG("Sendto_CH9329_mouseKey Snd:%d\n", iSnded);
    }
    else
    {
        DUMP_CONSOLE_MSG("Support CMD:\n");
        DUMP_CONSOLE_MSG("\t s   -- Sendto(SerialCh, data ...)\n");
        DUMP_CONSOLE_MSG("\t mb  -- Sendto_Modbus(SerialCh, data ...)\n");
        DUMP_CONSOLE_MSG("\t cio -- ioout(SerialCh, ch, onoff)\n");
        DUMP_CONSOLE_MSG("\t kb  -- Sendto_CH9329_keyVal(SerialCh, usualKey, controlKey)\n");
        DUMP_CONSOLE_MSG("\t ms  -- Sendto_CH9329_mouseKey(SerialCh, mouseKey, xMove, yMove, rollerMove)\n");
    }

    return;
}
#endif //_FUN_FunSerial

#if defined(USE_PUOVERHTTP)

void CFunConsole::FC_SendPuMsg(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("CFunConsole::FC_SendPuMsg(%d, ...)\n", argc);

    if (argc == 3)
    {
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        int iMsgType = atoi(argv[1]);
        std::string strSID = argv[2];
        std::string strBody = "";

        switch (iMsgType)
        {
            // auth
            case 1:
            {
                CSmsStack __smsStack;
                CSmsMsg_requestAuth Msg;
                const char *pUri = "/Sms/requestAuth";


                Msg.m_strAccount  	    = "zhangsan";
                Msg.m_strPassword 		= "123456";
                Msg.m_strSessionID	    = "null";

                Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
                Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
                Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
                Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
                Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";


                CMsgService __Service;

                __Service.m_strServiceID   = "2011221100000000015501";
                __Service.m_strDescription = "GeminiLock service agent";
                __Service.m_strURI         = "ss1.faceaip.com";
                __Service.m_strPort        = "12345";
                __Service.m_strProtocol    = "ss_GeminiLock_http";
                Msg.m_vLocalService.push_back(__Service);

                __Service.m_strServiceID   = "2011221100000000015502";
                __Service.m_strDescription = "GeminiLock service agent";
                __Service.m_strURI         = "ss2.faceaip.com";
                __Service.m_strPort        = "12346";
                __Service.m_strProtocol    = "ss_GeminiLock_http";
                Msg.m_vLocalService.push_back(__Service);

                __smsStack.Encode(Msg);

                strBody = __smsStack.ToString();
                g_AgentPuOverHttp.SendSmsMsg(pUri , strSID, strBody, "");

                break;
            }
            default:
            {
                break;
            }
        }


    }
    else
    {
        //
        // USE_PUOVERHTTP USE_PUOVERHTTP 一般不同时在一个软件中出现
        //
        static time_t ttLastSend = 0;
        static time_t ttNow = 0;
        // 测试用，暂时写在这里
        ttNow  = time(NULL);

        DUMP_CONSOLE_MSG("USE_PUOVERHTTP:: press any key to POST ...\n");
        //getchar();
        //usleep(100*1000);
        //DUMP_CONSOLE_MSG("time:%ld\n", time(NULL));
        ttNow = 0;

        const char *pJson = "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }";
        const char *pUri = "/Sms/requestAuth";

        const char *http_req_head =
            "POST %s HTTP/1.1\r\n"
            "Accept: */*\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
            "Host: sms.ezlibs.com\r\n"
            "Connection: Keep-Alive\r\n"
            "Content-type: application/json\r\n"
            "Content-length: %d\r\n\r\n"
            "%s";

        char http_req[1024];
        sprintf(http_req, http_req_head, pUri, strlen(pJson), pJson);

        if (ttNow ==0
            ||ttNow-ttLastSend>1)
        {
            ttLastSend  = time(NULL);
            // 不得加1
            //SendBySocket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req)+1 );
            int iret = g_AgentPuOverHttp.SendBySocketName(CPuOverHttpKeepConnector_SOCKET_NAME, http_req, strlen(http_req));
            __fline;
            DUMP_CONSOLE_MSG("Send %d Bytes to buffer %s at:%ld\n", strlen(http_req), iret>=0?"Succeeded.":"Failed!", time(NULL));
        }

    }
}
#endif //USE_PUOVERHTTP
#if defined(USE_CUOVERHTTP)
void CFunConsole::FC_SendCuMsg(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("CFunConsole::FC_SendCuMsg(%d, ...)\n", argc);

    if (argc == 3)
    {
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        int iMsgType = atoi(argv[1]);
        std::string strSID = argv[2];
        std::string strBody = "";

        switch (iMsgType)
        {
            // auth
            case 1:
            {
                CSmsStack __smsStack;
                CSmsMsg_requestAuth Msg;
                const char *pUri = "/Sms/requestAuth";


                Msg.m_strAccount  	    = "zhangsan";
                Msg.m_strPassword 		= "123456";
                Msg.m_strSessionID	    = "null";

                Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
                Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
                Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
                Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
                Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";


                CMsgService __Service;

                __Service.m_strServiceID   = "2011221100000000015501";
                __Service.m_strDescription = "GeminiLock service agent";
                __Service.m_strURI         = "ss1.faceaip.com";
                __Service.m_strPort        = "12345";
                __Service.m_strProtocol    = "ss_GeminiLock_http";
                Msg.m_vLocalService.push_back(__Service);

                __Service.m_strServiceID   = "2011221100000000015502";
                __Service.m_strDescription = "GeminiLock service agent";
                __Service.m_strURI         = "ss2.faceaip.com";
                __Service.m_strPort        = "12346";
                __Service.m_strProtocol    = "ss_GeminiLock_http";
                Msg.m_vLocalService.push_back(__Service);

                __smsStack.Encode(Msg);

                strBody = __smsStack.ToString();
                g_AgentCuOverHttp.SendSmsMsg(pUri , strSID, strBody, "");

                break;
            }
            default:
            {
                break;
            }
        }


    }
    else
    {
        //
        // USE_CUOVERHTTP USE_PUOVERHTTP 一般不同时在一个软件中出现
        //
        static time_t ttLastSend = 0;
        static time_t ttNow = 0;
        // 测试用，暂时写在这里
        ttNow  = time(NULL);

        DUMP_CONSOLE_MSG("USE_CUOVERHTTP:: press any key to POST ...\n");
        //getchar();
        //usleep(100*1000);
        //DUMP_CONSOLE_MSG("time:%ld\n", time(NULL));
        ttNow = 0;

        const char *pJson = "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }";
        const char *pUri = "/Sms/requestAuth";

        const char *http_req_head =
            "POST %s HTTP/1.1\r\n"
            "Accept: */*\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
            "Host: sms.ezlibs.com\r\n"
            "Connection: Keep-Alive\r\n"
            "Content-type: application/json\r\n"
            "Content-length: %d\r\n\r\n"
            "%s";

        char http_req[1024];
        sprintf(http_req, http_req_head, pUri, strlen(pJson), pJson);

        if (ttNow ==0
            ||ttNow-ttLastSend>1)
        {
            ttLastSend  = time(NULL);
            // 不得加1
            //SendBySocket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req)+1 );
            int iret = g_AgentCuOverHttp.SendBySocketName(CCuOverHttpKeepConnector_SOCKET_NAME, http_req, strlen(http_req));
            __fline;
            DUMP_CONSOLE_MSG("Send %d Bytes to buffer %s at:%ld\n", strlen(http_req), iret>=0?"Succeeded.":"Failed!", time(NULL));
        }

    }
}
#endif //USE_CUOVERHTTP
#if defined(USE_SC1ClientOverTcp)
void CFunConsole::FC_SendSC1Msg(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("CFunConsole::FC_SendSC1Msg(%d, ...)\n", argc);

    if (argc == 2)
    {
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        int iCmd = atoi(argv[1]);

        g_AgentSC1ClientOverTcp.SendCmd(iCmd);

        DUMP_CONSOLE_MSG("g_AgentSC1ClientOverTcp.SendCmd(%d)\n", iCmd);

#if 0

        switch (iCmd)
        {
            // restart
            case 4:
            {
                g_AgentSC1ClientOverTcp.SendCmd(4);

                break;
            }
            default:
            {
                break;
            }
        }
#endif

    }
}
#endif //USE_CUOVERHTTP

#ifdef USE_SMSOVERHTTP
void CFunConsole::FC_SmsSend(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("CFunConsole::FC_SmsSend(%d, ...)\n", argc);

    if (argc == 3)
    {
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        int iMsgType = atoi(argv[1]);
        std::string strSID = argv[2];
        std::string strBody = "";

        //__fline;
        //cout << "strSID:[[[" << strSID << "]]]" << endl;

        switch (iMsgType)
        {
            // auth
            case 1:
            {
                // 鉴权返回消息
                CSmsStack CSmsStack_requestAuthACK;
                CSmsMsg_requestAuthACK Msg;

                Msg.Result         = "0";
                Msg.Reason      = "ok";
                Msg.m_strSessionID      = strSID;//"0bf273da-7b3b-45a5-bd1a-e176984aeb77";
                Msg.m_strSessionTimeout = "120";

                Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
                Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
                Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
                Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
                Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

                CMsgService __Service;

                __Service.m_strServiceID   = "2011221100000000015501";
                __Service.m_strDescription = "GeminiLock service agent";
                __Service.m_strURI         = "ss1.faceaip.com";
                __Service.m_strPort        = "12345";
                __Service.m_strProtocol    = "ss_GeminiLock_http";
                Msg.m_vLocalService.push_back(__Service);

                __Service.m_strServiceID   = "2011221100000000015502";
                __Service.m_strDescription = "GeminiLock service agent";
                __Service.m_strURI         = "ss2.faceaip.com";
                __Service.m_strPort        = "12346";
                __Service.m_strProtocol    = "ss_GeminiLock_http";
                Msg.m_vLocalService.push_back(__Service);

                //std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

                CSmsStack_requestAuthACK.Encode(Msg);

                //ReturnHttpMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, "testJson_AuthACK");
                strBody = CSmsStack_requestAuthACK.ToString();
                g_AgentSmsOverHttp.SendBySession(strSID, strBody, "testJson_AuthACK");

                break;
            }
            default:
            {
                break;
            }
        }


    }
    else
    {
        g_AgentSmsOverHttp.GetHandler()->BroadCast();
    }
}
#endif // USE_SMSOVERHTTP


#ifdef _USE_DBAGENT_OCCI
void CFunConsole::FC_dbAgent(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("CFunConsole::FC_dbAgent(%d, ...)\n", argc);

    std::string strCmd = "s";

    if (argc > 1)
    {
        std::string strCmd = argv[1];
        //cout << strCmd << endl;
    }

    if (strCmd =="s")
    {
        long runTimes = 1;
        if (argc > 2)
        {
            runTimes = atoi(argv[2]);
            //cout << strCmd << endl;
        }

        for (int ii=0; ii<runTimes; ii++)
        {
            cout << "runTimes:" << ii << endl;

            bool bret = g_ltUtcsDbAgent.execSQL("select EMPLOYEE_ID, FIRST_NAME, HIRE_DATE from hr.employees");
            if (!bret)
            {
                DUMP_CONSOLE_MSG("execSQL failed, dbAgent:%s, check config pls.\n", g_ltUtcsDbAgent.getReady()?"Ready":"Not Ready");
                return;
            }

            ResultSet *rset = g_ltUtcsDbAgent.getResultSet();

            int line_num=1;

            DUMP_CONSOLE_MSG("No\tEMPLOYEE_ID\tFIRST_NAME\n---------------------------------------\n");

            while (rset
                   &&rset->next ())
            {
                int roadid = rset->getInt (1);
                //std::string elem_name = rset->getString(2);
                std::string elem_name = (rset->isNull(2)) ? "n/a" : rset->getString(2);

                DUMP_CONSOLE_MSG("%d\t%d\t\t%s\n", line_num, roadid, elem_name.c_str());
                line_num++;
            }
            DUMP_CONSOLE_MSG("---------------------------------------\n");
        }
    }
}
#endif // _USE_DBAGENT_OCCI

#if defined(_FUNC_gMiniLicense)
void CFunConsole::FC_gMiniLicense(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("CFunConsole::FC_gMiniLicense(%d, ...)\n", argc);

    std::string strCmd = "d";
    unsigned int ttLast = g_Solar.GetRunPeriod();

    if (argc > 1)
    {
        strCmd = argv[1];
    }
    // dump
    if (strCmd == "d")
    {
        std::string strDump = "";
        g_gMiniLicense.Dump(strDump);
        DUMP_CONSOLE_MSG("%s\n", strDump.c_str());
    }
    // setting
    else if (strCmd == "s")
    {
        if (argc > 2)
        {
            int iret = g_gMiniLicense.setLicense(argv[2]);

            if (iret < 0)
            {
                DUMP_CONSOLE_MSG("setLicense:%s failed, iret:%d.\n", argv[2], iret);
            }
            else
            {
                DUMP_CONSOLE_MSG("setLicense:%s succeeded.\n", argv[2]);
            }
        }
        else
        {
            DUMP_CONSOLE_MSG("Set GminiLicense but no lic args.\n");
        }
    }
    else
    {
        DUMP_CONSOLE_MSG("\t%-8s%s\n", "Lic:", g_gMiniLicense.getLicense().c_str());
        DUMP_CONSOLE_MSG("\t%-8s%llu(s)\n", "LicUptime:", g_gMiniLicense.getLicenseUptime());
        DUMP_CONSOLE_MSG("\t%-8s%llu(s)\n", "Remain:", g_gMiniLicense.getLicenseUptime()>ttLast ? g_gMiniLicense.getLicenseUptime()-ttLast : 0);
    }
}
#endif //_FUNC_gMiniLicense
#define DBG_TYKD_DEVID "8128CD2002D" // 港区
#define DBG_TYKD_CHANNEL "0" // 港区
#define DBG_TYKD_STREAM "1" // 港区
#if defined(_FUNC_PandoraMatrix)
void CFunConsole::FC_PandoraMatrix(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("%s(%d, ...)\n", __FUNCTION__, argc);

    std::string strCmd = "d";
    //unsigned int ttLast = g_Solar.GetRunPeriod();

    std::string strUrl = "";
    std::string strReq = "";
    std::string strAck = "";

    if (argc > 1)
    {
        strCmd = argv[1];
    }

    do
    {
        // dump
        if (strCmd == "d")
        {
            std::string strDump;
            g_AgentPandoraMatrix.GetHandler()->List(strDump, enLogLevelShow_detail);
            DUMP_CONSOLE_MSG("\t%-8s%s\n", "PandoraMatrix:", strDump.c_str());

            break;
        }
        else if (strCmd == "h")
        {
            DUMP_CONSOLE_MSG("\t%-8s : %s\n", "hls", "hls start");

            break;
        }
#if 0
        // 兼容在用指令
        // hls start
        else if (strCmd == "hls")
        {
            CPssMsgHlsPullTykdStart __msg;
            __msg.Stream.DevID = DBG_TYKD_DEVID;
            __msg.Stream.Channel = DBG_TYKD_CHANNEL;
            __msg.Stream.Stream = DBG_TYKD_STREAM;

            strUrl = def_Pandora_MatchReq_hlsPullTykdStart;

            CPssStack __stack;
            __stack.Encode(__msg);
            strReq = __stack.ToString();
        }
        // hls stop
        else if (strCmd == "hlss")
        {
            CPssMsgHlsPullTykdStop __msg;
            __msg.Stream.DevID = DBG_TYKD_DEVID;
            __msg.Stream.Channel = DBG_TYKD_CHANNEL;
            __msg.Stream.Stream = DBG_TYKD_STREAM;

            strUrl = def_Pandora_MatchReq_hlsPullTykdStop;
            CPssStack __stack;
            __stack.Encode(__msg);
            strReq = __stack.ToString();
        }
        else if (strCmd == "rtsp")
        {
            CPssMsgRtspPullTykdStart __msg;
            __msg.Stream.DevID = DBG_TYKD_DEVID;
            __msg.Stream.Channel = "0";
            __msg.Stream.Stream = "2";

            strUrl = def_Pandora_MatchReq_rtspPullTykdStart;

            CPssStack __stack;
            __stack.Encode(__msg);
            strReq = __stack.ToString();
        }
        else if (strCmd == "rtsps")
        {
            CPssMsgRtspPullTykdStop __msg;
            __msg.Stream.DevID = DBG_TYKD_DEVID;
            __msg.Stream.Channel = "0";
            __msg.Stream.Stream = "2";

            strUrl = def_Pandora_MatchReq_rtspPullTykdStop;

            CPssStack __stack;
            __stack.Encode(__msg);
            strReq = __stack.ToString();
        }
#else
#endif
#if 0
        // dd cont
        else if (strCmd == "tty")
        {
            std::string strDevID = "G513785316";
            std::string strChn = "1";
            std::string strStream = "0";

            CPandoraMsg_TykdPullStart Msg;
            CPssStack Stack;

            Msg.m_strMsgSession = Utility::l2string(ez_sec_get_tick_count());//"6f61dfe3";
            Msg.Stream.DevID=strDevID;
            Msg.Stream.Channel=strChn;
            Msg.Stream.Stream=strStream;
            //Msg.Stream.Description="data2" + strDevID;
            Msg.Stream.Description = "aGVsbG9T"; // helloS
            Msg.Type = 0;

            Stack.Encode(Msg);

            strUrl = def_Pandora_Req_Hqtt1_Publish;
            strReq = Stack.ToString();
        }
#endif
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if 0
        else if (strCmd == "dd")
        {
            DUMP_CONSOLE_MSG("\t%-8s%lu\n", "Container getCount:"
                             , g_AgentPandoraMatrix.GetHandler()->GetPandoraContainer()->getCount());
            DUMP_CONSOLE_MSG("\t%-8s%lu\n", "numHls:"
                             , g_AgentPandoraMatrix.GetHandler()->GetPandoraContainer()->numHls());
            DUMP_CONSOLE_MSG("\t%-8s%lu\n", "numRtmp:"
                             , g_AgentPandoraMatrix.GetHandler()->GetPandoraContainer()->numRtmp());
            DUMP_CONSOLE_MSG("\t%-8s%lu\n", "numRtsp:"
                             , g_AgentPandoraMatrix.GetHandler()->GetPandoraContainer()->numRtsp());

            break;
        }
#endif
        else
        {
            std::string strDump;
            g_AgentPandoraMatrix.GetHandler()->List(strDump, enLogLevelShow_detail);
            DUMP_CONSOLE_MSG("\t%-8s%s\n", "PandoraMatrix:", strDump.c_str());

            break;
        }

        //
        DUMP_CONSOLE_MSG("\t%-8s%s\n", "strUrl:", strUrl.c_str());
        DUMP_CONSOLE_MSG("\t%-8s%s\n", "strReq:", strReq.c_str());
        //int iRet_MatchReq = g_AgentPandoraMatrix.MatchReq(strUrl, strReq, "streamid", "session");
        //DUMP_CONSOLE_MSG("\t%-8s:%d\n", "iRet_MatchReq", iRet_MatchReq);
    }
    while(0);
}
#endif //_FUNC_PandoraMatrix

#if defined(_FUNC_PandoraNode)
void CFunConsole::FC_PandoraNode(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("%s(%d, ...)\n", __FUNCTION__, argc);

    std::string strCmd = "d";
    unsigned int ttLast = g_Solar.GetRunPeriod();
    ARG_USED(ttLast);

    if (argc > 1)
    {
        strCmd = argv[1];
    }

    // dump
    if (strCmd == "d")
    {
        std::string strDump;
        g_AgentPandoraNode.GetHandler()->List(strDump, enLogLevelShow_detail);
        DUMP_CONSOLE_MSG("\t%-8s%s\n", "PandoraNode:", strDump.c_str());
    }
    // setting
    else if (strCmd == "hls")
    {}
    else
    {
        std::string strDump;
        g_AgentPandoraNode.GetHandler()->List(strDump, enLogLevelShow_detail);
        DUMP_CONSOLE_MSG("\t%-8s%s\n", "PandoraMatrix:", strDump.c_str());
    }
}
#endif //_FUNC_PandoraNode

#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_TykdMobileService)
// pty l h 0 30
void CFunConsole::FC_PushTykd(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("%s(%d, ...)\n", __FUNCTION__, argc);

    for (int ii=0; ii<argc; ii++)
        DUMP_CONSOLE_MSG("%d -- %s\n", ii, argv[ii]);

    std::string strCmd = "l";
    std::string strDump;
    int iDumpMode = 0;

    if (argc > 1)
    {
        strCmd = argv[1];
    }

    if (strCmd == "l")
    {
        if (argc > 4)
        {
            std::string strStream = argv[2];
            int __from = atoi(argv[3]);
            int __to = atoi(argv[4]);
            DUMP_CONSOLE_MSG("\t stream(%s), from(%d), to(%d)\n", strStream.c_str(), __from, __to);
            if (__to<=__from
                || __to-__from>40)
            {
                DUMP_CONSOLE_MSG("\t Invalid input: stream(%s), from(%d), to(%d)\n", strStream.c_str(), __from, __to);
            }
            else
            {
                g_StreamPusherTykd.ListPullerTables(strDump, iDumpMode);

#if defined(_FUNC_LiveStreammingTykd2Rtsp)

                if (strStream=="r")//rtsp
                {
                    g_StreamPusherTykd.ListPullerRtsp(strDump, __from, __to, iDumpMode);
                }
#else
                if (0)
                {}
#endif //#if defined(_FUNC_LiveStreammingTykd2Rtsp)
#ifdef _FUNC_LiveRecord
                else if (strStream=="c")//rec
                {
                    g_StreamPusherTykd.ListRecordTykdMediaUrl(strDump, __from, __to, iDumpMode);
                }
#endif //_FUNC_LiveRecord

                else
                {
#if defined(_FUNC_LiveStreammingStream2Hls)
                    g_StreamPusherTykd.ListPullerStream2Hls(strDump, __from, __to, iDumpMode);
#endif //_FUNC_LiveStreammingStream2Hls
                }
                DUMP_CONSOLE_MSG(strDump.c_str());
            }
        }
        else
        {
            strDump = "";

            int numOf = 0;
#ifdef _FUNC_LiveStreammingTykd2Rtsp

            numOf += g_StreamPusherTykd.getNumPullerRtsp();
#endif
#ifdef _FUNC_LiveRecord

            numOf += g_StreamPusherTykd.getNumRecordTykdMediaUrl();
#endif

            if (numOf < 40)
            {
                g_StreamPusherTykd.ListPuller(strDump, iDumpMode);
            }
            else
            {
                g_StreamPusherTykd.ListPullerContents(strDump, iDumpMode);
            }

            DUMP_CONSOLE_MSG(strDump.c_str());
        }
    }
    else if (strCmd == "d")
    {
        std::string strDevID = "812A930AC7B";
        if (argc > 2)
        {
            //runTimes = atoi(argv[2]);
            strDevID = argv[2];
        }

        g_StreamPusherTykd.dumpPuller(strDevID, strDump, iDumpMode);

        DUMP_CONSOLE_MSG(strDump.c_str());
    }

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    else if (strCmd == "ss")
    {
        std::string strTykdDevID = "812A930AC7B";
        std::string strChannel = "0";
        std::string strStream = "2";
        std::string strValidPeriod = "0";
        std::string strUrlRtsp = "";

        if (argc > 2)
        {
            strTykdDevID = argv[2];
        }

        int iCountReq = 0;
        int iret = g_StreamPusherTykd.startPullRtsp( \
                   strTykdDevID
                   , strChannel
                   , strStream
                   , strValidPeriod
                   , strUrlRtsp
                   , iCountReq
                                                   );

        DUMP_CONSOLE_MSG("startPull:%d\n", iret);
        DUMP_CONSOLE_MSG("strUrlRtsp:%s\n", strUrlRtsp.c_str());
    }
    else if (strCmd == "sp")
    {
        std::string strTykdDevID = "812A930AC7B";
        std::string strChannel = "0";
        std::string strStream = "2";

        if (argc > 2)
        {
            strTykdDevID = argv[2];
        }

        int iret = g_StreamPusherTykd.stopPullRtsp(
                       strTykdDevID
                       , strChannel
                       , strStream
                   );

        DUMP_CONSOLE_MSG("startPull:%d\n", iret);
    }
#endif //#if defined(_FUNC_LiveStreammingTykd2Rtsp)

#if defined(_FUNC_LiveRecord)
    else if (strCmd == "rf")
    {
        std::string strTykdDevID = "812A930AC7B";
        std::string strChannel = "0";
        std::string strStream = "1";
        time_t ttStart = time(NULL);
        time_t ttEnd = 0;
        int iItems = 9999;
        int iPages = 1;
        tdefList_CFileStruct listRecordFile;

        if (argc > 2)
        {
            strTykdDevID = argv[2];
        }

        //int iret = g_StreamPusherTykd.queryRecordTykdMediaUrl(
        int iret = g_StreamPusherTykd.queryRecord(
                       strTykdDevID
                       , strChannel
                       , strStream
                       , ttStart
                       , ttEnd
                       , iItems
                       , iPages
                       , listRecordFile
             );

        DUMP_CONSOLE_MSG("queryRecordTykdMediaUrl:%d, size:%ld\n", iret, listRecordFile.size());
    }
    else if (strCmd == "qrf")
    {
#if 0// cool pc
        std::string strTykdDevID = "812A930AC7B";
        int iChannel = 0;
        int iStream = 1;
        time_t ttStart = 1572683979;
#endif
#if 1// 111
        std::string strTykdDevID = "402880f66d665d69016daf52f36e00d4";
        std::string strChannel = "0";
        std::string strStream = "1";
        time_t ttStart = 1575125509;
#endif
        time_t ttEnd = time(NULL);
        int iItems = 9999;
        int iPages = 1;
        tdefList_CFileStruct listRecordFile;

        if (argc > 2)
        {
            strTykdDevID = argv[2];
        }

        int iret = g_StreamPusherTykd.queryRecord(
                       strTykdDevID
                       , strChannel
                       , strStream
                       , ttStart
                       , ttEnd
                       , iItems
                       , iPages
                       , listRecordFile
             );

        DUMP_CONSOLE_MSG("queryRecord:%d, size:%ld\n", iret, listRecordFile.size());
    }

#endif //#if defined(_FUNC_LiveRecord)

    else// if (strCmd == "h")
    {
        DUMP_CONSOLE_MSG("Support CMD:\n");
        DUMP_CONSOLE_MSG("\t h -- show help\n");
        DUMP_CONSOLE_MSG("\t l -- ListPuller\n");
        DUMP_CONSOLE_MSG("\t d -- d <streamid> dump a puller\n");
        DUMP_CONSOLE_MSG("\t ms -- start RTMP pusher\n");
        DUMP_CONSOLE_MSG("\t mp -- stop RTMP pusher\n");
        DUMP_CONSOLE_MSG("\t ss -- start RTSP pusher\n");
        DUMP_CONSOLE_MSG("\t sp -- stop RTSP pusher\n");
#if defined(_FUNC_LiveRecord)
        DUMP_CONSOLE_MSG("\t rf -- list record files\n");
#endif //#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    }

}
#endif //_FUNC_PandoraNode

#ifdef _FUNC_LiveStatusServer
#include "../Function/LiveStatusServer/AgentLiveStatusServer.h"
#endif// _FUNC_LiveStatusServer 

#ifdef _FUNC_LiveStatusServer
void CFunConsole::FC_PublishStatus(int argc, char * argv[])
{
    DUMP_CONSOLE_MSG("%s(%d, ...)\n", __FUNCTION__, argc);

    std::vector<std::string> vPublishedID;
    g_AgentLiveStatusServer.GetHandler()->listPublished(vPublishedID);
    std::vector<std::string>::const_iterator it = vPublishedID.begin();
    for ( ; it != vPublishedID.end() ; it++)
    {
            DUMP_CONSOLE_MSG("\t%s\n", it->c_str());
    }
}
#endif //_FUNC_LiveStatusServer

