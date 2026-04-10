/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Solar.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Solar.cpp 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      一般主调类
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <iostream>

#include <ez_fs.h>
#include <ez_c_printf.h>
#include <net_work_interface.h>
#include <ez_util_platform.h>/*ez_sys_reboot*/
#include <ez_util_platform_Exec.h>

#include "Logs.h"

#include <EZThread.h>
#include <EZTimer.h>

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "./UniTransDef.h"

#ifdef _USE_MODULES_EZCONFIG
#include "Configs/ConfigManager.h"
#endif //_USE_MODULES_EZCONFIG

#include <gmini_host_unique_identifier.h>
#include "gmini_license.h"
#ifdef _FUNC_gMiniLicense
    #include "Maintain/gMiniLicense.h"
#endif //_FUNC_gMiniLicense

#ifdef _USE_DBAGENT_OCCI
//g_ltUtcsDbAgent.open();
#include "DBAgent/ltUtcsDbAgent.h"
#endif //_USE_DBAGENT_OCCI

#include "./Maintain/GeneralAgent.h"

#ifdef EXAMPLE_THE_WEBSERVICE
#include "WebService/AgentWebService.h"
#endif

#ifdef USE_PUBSERVICE
#include "Function/PubService/AgentPubService.h"
#endif
#ifdef USE_SMSOVERHTTP
#include "Function/SmsOverHttp/AgentSmsOverHttp.h"
#endif
#ifdef USE_InterUtcs_YiHeTech
#include "Function/InterUtcs/YiHeTech/AgentInterUtcs_YiHeTech.h"
#endif
#ifdef USE_ASSOVERHTTP
#include "Function/AssOverHttp/AgentAssOverHttp.h"
#endif
#ifdef USE_CUOVERHTTP
#include "Function/CuOverHttp/AgentCuOverHttp.h"
#endif
#ifdef USE_SC1ClientOverTcp
#include "Function/SC1ClientOverTcp/AgentSC1ClientOverTcp.h"
#endif
#ifdef USE_PUOVERHTTP
#include "Function/PuOverHttp/AgentPuOverHttp.h"
#endif
#if defined(XP_FaceWebs)
#include "FaceWebs/FaceWebs.h"
#endif //XP_FaceWebs

#ifdef FUNC_GEMINILOCK
#include "Function/GeminiLock/FunGeminiLock.h"
#endif

#ifdef FUNC_LOCAL_CONSOLE
#include "./Console/LocalConsole.h"
#endif

#ifdef _FUNC_PandoraMatrix
#include "Function/Pandora/Matrix/AgentPandoraMatrix.h"
#endif
#ifdef _FUNC_LiveStreammingServer
#include "Function/LiveStreammingServer/AgentLiveStreammingServer.h"
#endif
#ifdef _FUNC_PandoraNode
#include "Function/Pandora/Node/AgentPandoraNode.h"
#endif
#ifdef _FUNC_TouchIngress
#include "Function/Touch/Ingress/AgentTouchIngress.h"
#endif
#ifdef _FUNC_TouchEdge
#include "Function/Touch/Edge/AgentTouchEdge.h"
#endif
// pst sys
//defined(_FUNC_PullTykdMobileService) 定义了ws接口地址和推流地址等
//defined(_FUNC_TykdMobileService) usedby LiveRecord， 因此可以去掉
//defined(_FUNC_LiveRecord)
//#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_TykdMobileService) || defined(_FUNC_LiveRecord)
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_LiveRecord)
#include "Function/StreamPusher/StreamPusherTykd.h"
#endif // _FUNC_PullTykdMobileService
#if defined(_FUNC_Crawler)
#include "Function/Crawler/ManagerCrawler.h"
#endif // _FUNC_Crawler
#ifdef _FUN_FunSerial
#include "Function/GPorts/FunSerial.h"
#endif // _FUN_FunSerial
#ifdef _FUNC_LiveRecordServer
#include "Function/LiveRecordServer/AgentLiveRecordServer.h"
#endif// _FUNC_LiveRecordServer
#ifdef _FUNC_LiveCastServer
#include "Function/LiveCastServer/AgentLiveCastServer.h"
#endif// _FUNC_LiveCastServer
#ifdef _FUNC_InterVideo
#include "Function/InterVideo/AgentInterVideo.h"
#endif// _FUNC_InterVideo
#ifdef _FUNC_KeyManagementService
#include "Function/KeyManagementService/AgentKeyManagementService.h"
#endif// _FUNC_KeyManagementService
#ifdef _FUNC_NvpRegisterServer
#include "Function/NvpRegisterServer/AgentNvpRegisterServer.h"
#endif// _FUNC_NvpRegisterServer
#ifdef _FUNC_MinerMtService
#include "Function/MinerMtService/AgentMinerMtService.h"
#endif// _FUNC_MinerMtService

#ifdef _FUNC_NetVideo
#include "Function/NetVideo/PuManager.h"
#endif// _FUNC_NetVideo
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "Solar.h"

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


#ifdef _USE_MODULES_EZCONFIG
// config fname
const char * const ConfigFile_1= _CONFIG_FILENAME_MAIN;
const char * const ConfigFile_2 = _CONFIG_FILENAME_MAIN_DUPLICATE;
#endif //_USE_MODULES_EZCONFIG

const char * const copy_right = _COPY_RIGHT_STRING;
const char * const version_string = _VERSION;
const char * const desc="P_2013-5-20." _PROJECT_NAME "." _BUILD_VERSION;

//const char * const INSTANCE_PID_FILE = "/tmp/" _PROJECT_NAME ".pid";

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

PATTERN_SINGLETON_IMPLEMENT(CSolar);

CSolar::CSolar()//:m_TimerLive("CSolar-Live")
{
    m_strVer = version_string;
    m_strVer += " ";
    m_strVer += __DATE__;
    m_strVer += " ";
    m_strVer +=__TIME__;

    m_iRunPeriod = 0;
    m_ttStartTime = time(NULL);

#ifdef _USE_MODULES_EZCONFIG

    m_strConfigFile = ConfigFile_1;
    m_strConfigFileDuplicate = ConfigFile_2;
#endif //_USE_MODULES_EZCONFIG

    m_iRunDaemon = 0;
    m_log_ctrl = 999;
    m_iServiceRunning = 0;

    m_iCounter = 0;
}

CSolar::~CSolar()
{
#ifdef _USE_MODULES_EZCONFIG
    g_Config.saveFile();
#endif //_USE_MODULES_EZCONFIG

    //#if defined(_USE_EZLOG)
    EZ_LOGS(
        ez_log_destructor();
    );
    //#endif

}

void CSolar::ShowVersion()
{
    printf("---------------------------\n\n");
    printf("%s\n", copy_right);
    //printf("%s\n", desc);
    ez_c_printf("[g]%s\n", desc);
    //printf("$ Solar: %s,v %s %s %s Exp $\n\n", __FILE__, _VERSION, __DATE__, __TIME__);
    printf("$ Solar: v %s %s %s %s Exp $\n\n", _VERSION, __DATE__, __TIME__, _CONNECT_INFO_STRING);
    printf("---------------------------\n");
}

int CSolar::Initialize(int argc, char * argv[])
{
    int iret = 0;
    ARG_USED(iret);

    ShowVersion();

    iret = parseArg(argc, argv);
    if (iret != 0)
    {
        return iret;
    }

    // 检查环境备用
    chkRuntimeEnvironment();

    // 初始化系统参数
    iniRuntimeEnvironment();

    g_ThreadManager.RegisterMainThread(ThreadGetID());
    g_TimerManager.Start();

    std::string strPathName_PID;

#ifdef _USE_MODULES_EZCONFIG

    g_Config.initialize(m_strConfigFile.c_str(), m_strConfigFileDuplicate.c_str());

    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    if (__cfgGeneral.getConfig().strHostID.length() < 3)
    {
        __cfgGeneral.commit();
    }

    strPathName_PID=__cfgGeneral.getConfig().strDataFilePath;

    // 配置的是全路径
    if (NULL != strstr(__cfgGeneral.getConfig().strPid.c_str(), "/"))
    {
        strPathName_PID = __cfgGeneral.getConfig().strPid;
    }
    // 配置为空， 不使用此功能
    else if (__cfgGeneral.getConfig().strPid.length() < 3)
    {
        strPathName_PID.clear();
    }
    else
    {
        strPathName_PID += "/";
        strPathName_PID += __cfgGeneral.getConfig().strPid;
    }
#else//_USE_MODULES_EZCONFIG
    strPathName_PID = getcwd(NULL, 0);
    strPathName_PID += "/"_PROJECT_NAME".pid";
#endif //_USE_MODULES_EZCONFIG

#if defined(_USE_PID)
    // 配置为非空， 使用此功能
    if (!strPathName_PID.empty())
    {	
        printf("Pid:%s\n", strPathName_PID.c_str());

        int iret_alone_runnind = alone_runnind(strPathName_PID.c_str());
	
        if (-1 == iret_alone_runnind)
        {
            printf("Failed open:%s, Exit ...\n", strPathName_PID.c_str());
            exit (-1);
        }
        else if (-2 == iret_alone_runnind)
        {
            printf("Alone runnind, Bye ...\n");
            exit (-2);
        }
        else if (iret_alone_runnind < 0)
        {
            printf("failed alone_runnind:%d\n", iret_alone_runnind);
            exit (-3);
        }
        else 
        {}
	
    }
    else
    {	
        printf("Pid empty, skip this function.\n");
    }
#endif

    InitializeLogs(m_log_ctrl);

    //LOG4CPLUS_INFO(LOG_SOLAR, "Starting(" << desc << ") ...");
    ez_printf_info_content("Starting(%s) ...\n", desc);

#if defined(_FUNC_gMiniLicense)
    g_gMiniLicense.Start(this, (CgMiniLicense::gMiniLicenseSignalProc_t)&CSolar::OnGminiLicenseMsg);
#else

    char d_verType;
    char d_p_id[GMINI_HUID_LEN_BUFFER];
    char d_data[64];
    int iret_gmini_license_decode = gmini_license_decode(def_Default_SerialNumber, &d_verType, d_p_id, d_data, sizeof(d_data));
    ARG_USED(iret_gmini_license_decode);
#endif // _FUNC_gMiniLicense

#ifdef _USE_DBAGENT_OCCI

    g_ltUtcsDbAgent.Start();
#endif //_USE_DBAGENT_OCCI

    //begin -- start server
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // cu pu 不再启动这个服务 20140328
    //#if !defined(USE_CUOVERHTTP) && !defined(USE_PUOVERHTTP)
    g_MaintainAgent.Initialize(argc, argv);
    // 开启
    g_MaintainAgent.Start();
    //#endif

    // 开启服务
    Start();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //end -- start server
    //g_Maintenance.Start(/*bUseCfgAtStartForce*/false, /*bMaintenanceAuto*/true);

    // 启动完毕， 保存一次配置文件
    //g_Config.saveFile();
    //LOG4CPLUS_INFO(LOG_SOLAR, argv[0] << " Initialize finished");
    ez_printf_info_content("%s Initialize end, running ...\n", argv[0]);

#ifdef FUNC_LOCAL_CONSOLE

    if (m_iRunDaemon != 1)
    {
        iret = g_LocalConsole.Start();
    }
#endif

    // 设置参数
    setRuntimeEnvironment();

    return 0;
}


int CSolar::InitializeLogs(int log_ctrl)
{
#if defined(_USE_LOG4CPP) || defined(_USE_EZLOG) || defined(_USE_EZLOG_CLASSIC) || defined(_USE_EZLOG_FILENAME)
#ifdef _USE_MODULES_EZCONFIG
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    std::string strFilePath = __cfgGeneral.getConfig().strLogFilePath;
    std::string strFileName = __cfgGeneral.getConfig().strLogFileName;
    int maxFileSize = __cfgGeneral.getConfig().maxFileSize;
    int maxFiles = __cfgGeneral.getConfig().maxBackupIndex;
    int logCtl= __cfgGeneral.getConfig().levelLog;

#else

    std::string strFilePath = "./logs/";
    std::string strFileName = _LOG_FILENAME;
    int maxFileSize = 1024*1024;
    int maxFiles = 2;
    int logCtl = 27;
#endif

    std::string strFilePathName = strFilePath +"/"+  strFileName;

    printf("Initialize Logs:%s, MaxSize:%d Files:%d - "
           , strFilePathName.c_str()
           , maxFileSize
           , maxFiles);

    int iret = -1;
    if (strFilePath.size()>0 && strFileName.size()>0)
    {
        iret = ez_creat_dir(strFilePath.c_str());

        //printf("\n\nez_creat_dir:%s , ret:%d.\n", strFilePath.c_str(), iret);
    }
#endif //defined(_USE_LOG4CPP) || defined(_USE_EZLOG) || defined(_USE_EZLOG_CLASSIC) || defined(_USE_EZLOG_FILENAME)

#if defined(_USE_LOG4CPP)
    //	初始化日志系统

    log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D %-5p %-5c - %m [%l]%n"); // c , model
    SharedAppenderPtr append_1;
    if (iret<0)
    {
        printf("failed, use stdout now.\n");
        append_1=new ConsoleAppender();
    }
    else
    {
        printf("success\n");

        append_1 = new RollingFileAppender(LOG4CPLUS_TEXT(strFilePathName)
                                           , __cfgGeneral.getConfig().maxFileSize
                                           , __cfgGeneral.getConfig().maxBackupIndex);
    }

    if (append_1)
    {
        append_1->setName(LOG4CPLUS_TEXT("UniTrans"));
        append_1->setLayout( std::auto_ptr<Layout>(new PatternLayout(pattern)) );
        Logger::getRoot().addAppender(append_1);
        NDCContextCreator _context(LOG4CPLUS_TEXT("uni"));
    }

    //#elif defined(_USE_EZLOG)
#endif //_USE_LOG4CPP
    EZ_LOGS(
        if (iret<0)
    {
        printf("failed, use stdout now.\n")
            ;
            // ez_log 无法使用文件的时候， 自动使用stdout
        }
        else
        {
            printf("success\n");

            ez_log_structure(strFilePathName.c_str(), maxFiles, maxFileSize);

            //int __llevel = EZ_LOG_TO_FILE*10+EZ_LOG_LEVEL_DEBUG;
            int __llevel = EZ_LOG_TO_FILE_AND_STDOUT*10+EZ_LOG_LEVEL_DEBUG;
            //ez_log_set_level(iLevel);

            // 优先使用输入参数 在使用配置数据 最后使用默认数据
            m_log_ctrl = (m_log_ctrl<999)? m_log_ctrl : logCtl;
            if (ez_log_set_level(m_log_ctrl) <0 )
            {
                ez_log_set_level(__llevel) ;
            }
        }
    );

    return 0;
}
// 开启服务
int CSolar::Start()
{
    int iret;
    ARG_USED(iret);

    CEZLock __lock(m_mutexMark);

    // 功能放在前面
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_LiveStreammingServer) || defined(_FUNC_TykdMobileService) || defined(_FUNC_LiveRecord)

    iret = g_StreamPusherTykd.Start();
#endif // _FUNC_PullTykdMobileService

#if defined(_FUNC_Crawler)
    iret = g_ManagerCrawler.Start();
#endif // _FUNC_PullTykdMobileService

#ifdef _FUN_FunSerial

    iret = g_FunSerial.Start();
#endif

#ifdef EXAMPLE_THE_WEBSERVICE

    g_AgentWebService.Start();
#endif
#ifdef USE_SMSOVERHTTP

    g_AgentSmsOverHttp.Start();
#endif
#ifdef USE_InterUtcs_YiHeTech

    g_AgentInterUtcs_YiHeTech.Start();
#endif
#ifdef USE_ASSOVERHTTP

    g_AgentAssOverHttp.Start();
#endif
#ifdef USE_PUBSERVICE

    g_AgentPubService.Start();
#endif

#ifdef USE_CUOVERHTTP

    g_AgentCuOverHttp.Start();
#endif


#ifdef USE_SC1ClientOverTcp

    g_AgentSC1ClientOverTcp.Start();
#endif

#ifdef USE_PUOVERHTTP

    g_AgentPuOverHttp.Start();
#endif

#ifdef FUNC_GEMINILOCK

    iret = g_FunGeminiLock.Start();
#endif

#ifdef _FUNC_LiveStreammingServer

    iret = g_AgentLiveStreammingServer.Start();
#endif
#ifdef _FUNC_PandoraMatrix

    iret = g_AgentPandoraMatrix.Start();
#endif
#ifdef _FUNC_PandoraNode

    iret = g_AgentPandoraNode.Start(this, (CAgentPandoraNode::PandoraNodeSignalProc_t)&CSolar::OnMsgCallback);
#endif

#ifdef _FUNC_TouchIngress
    iret = g_AgentTouchIngress.Start(this, (CAgentTouchIngress::TouchIngressSignalProc_t)&CSolar::OnMsgCallback);
#endif
#ifdef _FUNC_TouchEdge
    iret = g_AgentTouchEdge.Start(this, (CAgentTouchEdge::TouchEdgeSignalProc_t)&CSolar::OnMsgCallback);
#endif


#ifdef _FUNC_LiveRecordServer

    iret = g_AgentLiveRecordServer.Start();
#endif // _FUNC_LiveRecordServer
#ifdef _FUNC_LiveCastServer

    iret = g_AgentLiveCastServer.Start();
#endif //#ifdef _FUNC_LiveCastServer
#ifdef _FUNC_InterVideo

    iret = g_AgentInterVideo.Start();
#endif //#ifdef _FUNC_InterVideo
#ifdef _FUNC_KeyManagementService

    iret = g_AgentKeyManagementService.Start();
#endif //#ifdef _FUNC_KeyManagementService
#ifdef _FUNC_NvpRegisterServer

    iret = g_AgentNvpRegisterServer.Start();
#endif //#ifdef _FUNC_NvpRegisterServer

#ifdef _FUNC_MinerMtService
    iret = g_AgentMinerMtService.Start();
#endif //#ifdef _FUNC_MinerMtService

#ifdef _FUNC_NetVideo

    iret = g_NetVideo_PuManager.Start();
#endif //#ifdef _FUNC_NetVideo

#ifdef XP_FaceWebs

    iret = g_FaceWebs.Start();
#endif//XP_FaceWebs

    m_iServiceRunning = 1;
    return 0;
}

//停止服务
int CSolar::Stop(int mod)
{
    int iret;
    ARG_USED(iret);

    CEZLock __lock(m_mutexMark);
#ifdef EXAMPLE_THE_WEBSERVICE

    g_AgentWebService.Stop();
#endif
#ifdef USE_SMSOVERHTTP

    g_AgentSmsOverHttp.Stop();
#endif
#ifdef USE_InterUtcs_YiHeTech

    g_AgentInterUtcs_YiHeTech.Stop();
#endif
#ifdef USE_ASSOVERHTTP

    g_AgentAssOverHttp.Stop();
#endif
#ifdef USE_PUBSERVICE

    g_AgentPubService.Stop();
#endif

#ifdef USE_CUOVERHTTP

    g_AgentCuOverHttp.Stop();
#endif
#ifdef USE_SC1ClientOverTcp

    g_AgentSC1ClientOverTcp.Stop();
#endif

#ifdef USE_PUOVERHTTP

    g_AgentPuOverHttp.Stop();
#endif

#ifdef FUNC_GEMINILOCK

    iret = g_FunGeminiLock.Stop();
#endif

#ifdef _FUNC_LiveStreammingServer

    iret = g_AgentLiveStreammingServer.Stop();
#endif
#ifdef _FUNC_PandoraMatrix

    iret = g_AgentPandoraMatrix.Stop();
#endif
#ifdef _FUNC_PandoraNode

    iret = g_AgentPandoraNode.Stop(this, (CAgentPandoraNode::PandoraNodeSignalProc_t)&CSolar::OnMsgCallback);
#endif

#ifdef _FUNC_TouchIngress
    iret = g_AgentTouchIngress.Stop(this, (CAgentTouchIngress::TouchIngressSignalProc_t)&CSolar::OnMsgCallback);
#endif
#ifdef _FUNC_TouchEdge
    iret = g_AgentTouchEdge.Stop(this, (CAgentTouchEdge::TouchEdgeSignalProc_t)&CSolar::OnMsgCallback);
#endif

#ifdef _FUNC_PullTykdMobileService

    iret = g_StreamPusherTykd.Stop();
#endif // _FUNC_PullTykdMobileService

#ifdef XP_FaceWebs

    iret = g_FaceWebs.Stop();
#endif//XP_FaceWebs

#ifdef _FUNC_LiveRecordServer

    iret = g_AgentLiveRecordServer.Stop();
#endif // _FUNC_LiveRecordServer

#ifdef _FUNC_LiveCastServer

    iret = g_AgentLiveCastServer.Stop();
#endif //#ifdef _FUNC_LiveCastServer
#ifdef _FUNC_InterVideo

    iret = g_AgentInterVideo.Stop();
#endif //#ifdef _FUNC_InterVideo
#ifdef _FUNC_KeyManagementService

    iret = g_AgentKeyManagementService.Stop();
#endif //#ifdef _FUNC_KeyManagementService
#ifdef _FUNC_NvpRegisterServer

    iret = g_AgentNvpRegisterServer.Stop();
#endif //#ifdef _FUNC_NvpRegisterServer
#ifdef _FUNC_MinerMtService
    iret = g_AgentMinerMtService.Stop();
#endif //#ifdef _FUNC_MinerMtService

#ifdef FUNC_LOCAL_CONSOLE

    if (m_iRunDaemon != 1)
    {
        iret = g_LocalConsole.Stop();
    }
#endif

    m_iServiceRunning = 0;
    return 0;
}

int CSolar::Done()
{
    while (1)
    {
        CEZSemaphore sem;
        sem.Wait();
    }

    return 0;
}

int CSolar::Term()
{
    LOG4CPLUS_INFO(LOG_SOLAR, "CSolar::Term");
    ez_printf_notice("-- ::Term\n");

    ez_printf_notice("-- ::Stop ...\n");
    Stop();
    ez_printf_notice("-- ::Stoped\n");

    return 0;
}

int CSolar::Reboot()
{
    ez_printf_notice("%s\n", __FUNCTION__);

    Stop();

    return ez_sys_reboot();
}
int CSolar::Shutdown()
{
    return 0;
}

int CSolar::Restart()
{
    return 0;
}

int CSolar::Restore()
{
    DBG(__fline;printf("g_Config.SetDefaultConfig(CFG_IDX_ALL);\n"););
    // 全部恢复

    return 0;
}
void CSolar::haveException(int ExcNo, char *pMsg)
{
    //LOG4CPLUS_INFO(LOG_SOLAR, "CSolar::haveException:" << ExcNo << " Msg:" << pMsg);

    //std::cout <<  "CSolar::haveException:" << ExcNo << " Msg:" << pMsg << std::endl;

    if (SIGCHLD == ExcNo)
    {
        //ez_printf_debug("-- CSolar::haveException::%d, Msg:%s\n", ExcNo, pMsg);
    }
    else
    {
        ez_printf_notice("-- CSolar::haveException::%d, Msg:%s\n", ExcNo, pMsg);
    }

    return;
}

const std::string & CSolar::GetVersion()
{
    return m_strVer;
}
//#ifdef _FUNC_gMiniLicense
const std::string & CSolar::GetHuid()
{
    return m_strHuid;
}
const std::string & CSolar::GetHostID()
{
    return m_strHostID;
}
//#endif //_FUNC_gMiniLicense
const char *CSolar::GetCopyRights()
{
    return copy_right;
}
const char *CSolar::GetDesc()
{
    return desc;
}

unsigned long CSolar::GetRunPeriod()
{
    //return m_iRunPeriod;
    return /*(unsigned int)*/(process_counter_sec());
}

unsigned long CSolar::GetRunPeriodMs()
{
    //return m_iRunPeriod;
    return /*(unsigned int)*/(process_counter_ms());
}

time_t CSolar::GetStartTime()
{
    return m_ttStartTime;
}

const std::string & CSolar::GetHostIP()
{
    return m_strHostIp;
}

const std::string & CSolar::GetHostMac()
{
    return m_strHostMac;
}

long CSolar::GetCounter()
{
    CEZLock __lock(m_mutexCounter);

    m_iCounter++;
    return m_iCounter;
}

//#ifdef _USE_MODULES_EZCONFIG
#if 0 // not good , use config instead
const std::string &CSolar::GetConfigFilePath()
{
    size_t found;

    found = m_strConfigFile.find_last_of("/\\");

    DBG(
        std::cout << " file: " << m_strConfigFile << std::endl;
        std::cout << " folder: " << m_strConfigFile.substr(0,found) << std::endl;
        std::cout << " file: " << m_strConfigFile.substr(found+1) << std::endl;
    );

    if (std::string::npos == found)
    {

        m_strConfigFilePath = ".";

        DBG(
            std::cout << " no path valid." << std::endl;
        );
    }
    else
    {
        m_strConfigFilePath = m_strConfigFile.substr(0, found);

        DBG(
            std::cout << " path:" << m_strConfigFilePath << std::endl;
        );
    }

    return m_strConfigFilePath;
}
#endif //_USE_MODULES_EZCONFIG

static const char* month[] =
    {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

void CSolar::GetBuildDate(int &year, int &month_, int &day)
{
    int i;//, year, day;
    char s1[] = __DATE__;
    char *p;

    for (i = 0; i < 12; i++)
    {
        if( strncmp(month[i], s1, 3) == 0 )
            break;
    }
    month_ = i+1;

    p = s1;
    p += 4;
    if ( *p == ' ')
    {
        *p = '0';
    }
    sscanf(p, "%d %d", &day, &year);
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if !defined(MACOSX)
void signal_handler(int sig, sighandler_t handler)
{
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(sig, &action, NULL);
}

void run_daemon()
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "daemon can't fork\n");
        exit(1);
    }
    else if (pid != 0)
    {
        exit(0);
    }
    setsid();

    signal_handler(SIGHUP, SIG_IGN);
    signal_handler(SIGTERM, SIG_IGN);
    signal_handler(SIGPIPE, SIG_IGN);
}
#endif //!defined(MACOSX)
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <getopt.h>

// 解析输入参数
int CSolar::parseArg(int argc, char *argv[])
{
    int c;

    while (1)
    {
        static struct option long_options[] =
            {
                {"help",          no_argument,         0, 'h'
                },
                {"version",     no_argument,         0, 'v'},
                {"daemon",    no_argument,          0, 'd'},
                {"config",       required_argument, 0, 'c'},
                {"logs",          required_argument, 0, 'g'},
                {0, 0, 0, 0}
            };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hvdc:g:",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:
            {
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
            }
            case 'h':
            {
                printf ("\n");
                printf ("%-32s : %s \n", "-h", "--help");
                printf ("%-32s : %s \n", "-v", "--version, show the version");
                printf ("%-32s : %s \n", "-d", "--daemon ");
                printf ("%-32s : %s \n", "-c <config file pathname>", "--config set the config file");
                printf ("%-32s : %s \n", "-g <logCtl>", "ctl log");
                //break;
                return 2;
            }
            case 'v':
            {
                // default showed
                //ShowVersion();
                //break;
                return 2;
            }
            case 'd':
            {
                puts ("option -d, run_daemon\n");
#if !defined(MACOSX)

                run_daemon();
                m_iRunDaemon = 1;
#endif

                break;
            }
            case 'c':
            {
                printf ("option -c with value `%s'\n", optarg);

                if (optarg && strlen(optarg)>3 && strlen(optarg)<256)
                {
#ifdef _USE_MODULES_EZCONFIG
                    m_strConfigFile = optarg;
                    m_strConfigFileDuplicate = m_strConfigFile+".duplicate";
                    printf ("use your config file `%s'\n", optarg);
#else

                    printf ("set config file '%s', but _USE_MODULES_EZCONFIG not defined.\n", optarg);
#endif //_USE_MODULES_EZCONFIG

                }
                break;
            }
            case 'g':
            {
                if (optarg)
                {
                    m_log_ctrl= atoi(optarg);
                    fprintf(stdout, "m_log_ctrl=%d\n", m_log_ctrl);
                }
            }
            case '?':
            {
                /* getopt_long already printed an error message. */
                break;
            }
            default:
            {
                __trip;
                return -1;
            }
        }
    }

    return 0;
}

int CSolar::chkRuntimeEnvironment(int mode/*All*/)
{
    // 开始的时候调用一次， 进程计数开始
    process_counter_sec();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //#ifdef _FUNC_gMiniLicense
    char huid[GMINI_HUID_LEN_BUFFER] = {'\0'};
    int huid_len = GMINI_HUID_LEN_BUFFER;

    int iret = gmini_host_unique_identifier_get(GMINI_HUID_V2, huid, &huid_len);

    if (0 == iret)
    {
        //printf("huid:%s\n", huid);
        m_strHuid = huid;
    }
    else
    {
        m_strHuid = _PROJECT_NAME;
    }

#if defined(_FUNC_PandoraNode)
    m_strHostID = "G1" + m_strHuid;
#else

    m_strHostID = "GZ" + m_strHuid;
#endif

    //std::cout <<  "Starting(" << desc << ", " << m_strHuid << ") ..." << std::endl;
    //std::cout <<  "HUID(" << m_strHuid << ") ..." << std::endl;
    //#endif //_FUNC_gMiniLicense
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    EZ_NET_DEVICE_T net_dev[8];

    iret = ez_get_eth_device(net_dev, 8);
    DBG(
        printf("num of eth_device:%d\n", iret);
        printf("%2s %8s %15s %15s %15s %17s\n", "No", "Name", "Ip", "Bcast", "Mask", "Mac");
    );
    for (int ii=0; ii<iret; ii++)
    {
        if (net_dev[ii].trans_media==NM_WIRELESS)
        {
            continue;
        }

        char ip_get[IP4_STR_SIZ];
        char bcast_get[IP4_STR_SIZ];
        char mask_get[IP4_STR_SIZ];
        char mac_get[MAC_STR_SIZ];

        int iret_ez_ip_get = ez_ip_get(net_dev[ii].name, ip_get, bcast_get, mask_get, mac_get, def_Mac_Format_Upper_Compact/*NULL*/);

        if (iret_ez_ip_get==0)
        {
            DBG(
                printf("%2d %8s %15s %15s %15s %17s\n"
                       , ii
                       , net_dev[ii].name
                       , ip_get
                       , bcast_get
                       , mask_get
                       , mac_get);
            );

            if (strcmp(mac_get, "000000000000") == 0)
            {
                continue;
            }

            // mac不必等
            m_strHostMac = mac_get;

            struct in_addr in_addr_ip;
            int result = inet_pton(AF_INET, ip_get, &(/*sa.sin_addr*/in_addr_ip));
            if (result == 1 && in_addr_ip.s_addr!=0/*0.0.0.0*/)
            {
                m_strHostIp = ip_get;
                //m_strHostMac = mac_get;
                break;
            }

        }
        else
        {
            DBG(
                printf("%2d %8s\n"
                       , ii
                       , net_dev[ii].name
                      );
            );
        }
    }

    return 0;
}

int CSolar::iniRuntimeEnvironment(int mode/*All*/)
{

    return 0;
}

int CSolar::setRuntimeEnvironment(int mode/*All*/)
{
#if defined(_Fun_iniRuntimeEnvironment_OpenWrt)
    DBG(ez_printf_debug("%s::%s\n", __FUNCTION__, "_Fun_iniRuntimeEnvironment_OpenWrt"););

    const char *pFilePathName = "/tmp/sysinfo/model";
    char *pBuffer=NULL;

    const char *pMyModule=NULL;

    do
    {
        int iret_load = ez_load_file_to_memory(pFilePathName, &pBuffer);

        if (iret_load<=0)
        {
            ez_printf_debug("ez_load_file_to_memory failed:%d\n", iret_load);
            break;
        }

        DBG(ez_printf_debug("ez_load_file_to_memory(%d):\n[%s]\n", iret_load, pBuffer);)

        if (strstr(pBuffer, "YOUKU YK1"))
        {
            pMyModule = "A1.PQ ezlibs.com";
        }
        else if (strstr(pBuffer, "ZBT-WR8305RT"))
        {
            pMyModule = "A2.PQ ezlibs.com";
        }
        // 2019-10-18
        else if (strstr(pBuffer, "UniElec U7628-01 (128M RAM/16M flash)"))
        {
            //pMyModule = "A3.PQ ezlibs.com";
            pMyModule = "A3.PQ gminitech.com";
        }
        else if (strstr(pBuffer, "ZBT-WG3526 (16M)"))
        {
            pMyModule = "C6.PQ ezlibs.com";
        }
        else
        {
            //pMyModule = pBuffer; //"ezlibs.PQ ezlibs.com";
        }

        if (pMyModule != NULL)
        {
            iret_load = ez_save_file_from_memory(pFilePathName, pMyModule, strlen(pMyModule));
            if (iret_load>0)
            {
                ez_printf_debug("ez_save_file_from_memory(%d):\n[%s]\n", iret_load, pBuffer);
            }
            else
            {
                ez_printf_debug("ez_save_file_from_memory failed:%d\n", iret_load);
            }
        }
    }
    while (0);

    if (pBuffer)
    {
        free(pBuffer);
        pBuffer = NULL;
    }

#endif //_Fun_iniRuntimeEnvironment_OpenWrt

#if defined(_Fun_iniRuntimeEnvironment_uci)
    DBG(ez_printf_debug("%s::%s\n", __FUNCTION__, "_Fun_iniRuntimeEnvironment_uci"););

    std::vector<std::string> resvec;
    const char *cmd="uci get system.@system[0].hostname";
    int iret = ez_Exec(cmd, resvec);
    if (1==iret
        && resvec.size()==1 // return is valid
        && (resvec[0].find(m_strHostID) == std::string::npos) // 没有带id
       )
    {
        std::string __strCmd ;
        __strCmd += "uci set system.@system[0].hostname='"+ m_strHostID + "' ;";
        __strCmd += "uci set system.@system[0].zonename='Asia/Shanghai' ;";
        __strCmd += "uci set system.@system[0].timezone='CST-8' ;";
        //__strCmd += "uci commit system ;";

        // dns
        __strCmd += "uci set network.lan.dns='8.8.8.8 114.114.114.114';";
        __strCmd += "uci set network.wan.dns='8.8.8.8 114.114.114.114';";
        //__strCmd += "uci commit network ;";

        __strCmd += "uci commit ;";
        //__strCmd += "reboot ;";
        __strCmd += "/etc/init.d/network reload ;";
        __strCmd += "/etc/init.d/system reload ;";

        ez_printf_info("Set system info\n");

        system(__strCmd.c_str());
    }
#endif

    return 0;
}

#if defined(_FUNC_gMiniLicense)
void CSolar::OnGminiLicenseMsg(int iMsg, char *pPacket, int iCh, time_t msg_time)
{
    switch ( iMsg)
    {
        case MSG_GMINI_LICENSE_TIMEOUT:
        {
            if (1==m_iServiceRunning)
            {
                //ez_printf_notice("OnGminiLicenseMsg MSG_GMINI_LICENSE_TIMEOUT, Stop ...\n");
                ez_printf_notice("dc80f26694f1251c5b5a6b4b1867d25603476e2d\n");
                Stop();
            }

            break;
        }
        case MSG_GMINI_LICENSE_OBJECT_ERROR:
        {
            if (1==m_iServiceRunning)
            {
                // 只要序列号对即可， 不停机
#ifdef _FUNC_gMiniLicenseDoNotStop
                //ez_printf_debug("8fcc363f09c521bfa3cd38efe4ce7c017c069858-%s\n", GetHuid().c_str());
#else
                ez_printf_notice("8fcc363f09c521bfa3cd38efe4ce7c017c069858-%s\n", GetHuid().c_str());
                Stop();
#endif

            }

            break;
        }
        default:
        {
            // Code
            break;
        }
    }
}
#endif // _FUNC_gMiniLicense

void CSolar::OnMsgCallback(int iMsg, void *pData)
{
    switch ( iMsg)
    {
        case 0:
        {
            break;
        }
#ifdef _FUNC_PandoraNode
        case MSG_PANDORANODE_T_CONNECT_SUCCEEDED:
        {

            break;
        }
        case MSG_PANDORANODE_T_CONNECT_TRYING:
        {
            if (pData)
            {
                MSG_PandoraNodeData_T *__pMsg = (MSG_PandoraNodeData_T *)pData;

                ez_printf_debug("onMsg:CONNECT_TRYING, iConnTimes:%d\n", __pMsg->iConnTimes);
            }
            break;
        }
        case MSG_PANDORANODE_T_CONNECT_TRY_FAILED:
        {
            if (pData)
            {
                MSG_PandoraNodeData_T *__pMsg = (MSG_PandoraNodeData_T *)pData;

                ez_printf_info("onMsg:MSG_PANDORANODE_T_CONNECT_TRY_FAILED, iConnTimes:%d\n", __pMsg->iConnTimes);
            }

            Reboot();

            break;
        }
        case MSG_PANDORANODE_T_CONNECT_FAILED:
        {

            break;
        }
#endif //_FUNC_PandoraNode
        default:
        {
            ez_printf_debug("Unknow iMsg:%d\n",  iMsg);
            break;
        }
    }// switch ( iMsg)
}

