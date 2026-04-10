/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecordTykdMediaUrl.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecordTykdMediaUrl.cpp 5884 2017-02-17 05:56:32Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-17 05:56:32  Create
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
#include <signal.h>/*killpg*/
#include <sys/wait.h> /*waitpid*/
#include <iostream>
//
#include <ez_fs.h>
#include <str_opr.h>
#include <ez_socket.h> /*ezDef_StreamType_Str_megaeye*/
#include <ez_url_parser.h>
#include "StringOpr.h"

#if defined(_USE_ezOnvif)
#include "ezOnvif.h"
#endif

//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */
#include "uniFace.h"

#include "../Pandora/StreamUtility.h" /*工具*/

#include "../../Logs.h"
//#ifdef _USE_MODULES_EZCONFIG
//#include "../../Configs/ConfigPullTykdMobileService.h"
//#endif //_USE_MODULES_EZCONFIG
#if defined(_FUNC_LiveRecord)
//#include "../../Configs/ConfigLiveRecord.h"
#endif //_FUNC_LiveRecord

#if defined(_FUNC_TykdMobileService)
//#include "../../Configs/ConfigTykdMobileService.h"
#endif

//#if defined (_FUNC_DO_RELAY)
//#include "gPushMan/tykdMobileServiceHX_Cli.h"
//#include "../Interface/tykdWebService/tykdMobileServiceClient.h"

//#endif //_FUNC_DO_RELAY
#include "RecordTykdMediaUrl.h"
//#include "gPushMan/TypedefPssOfTykd.h"
//#include "../Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */

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
#ifndef TYKD_ACCOUNT
#define TYKD_ACCOUNT "hx"
#endif// TYKD_ACCOUNT

#ifndef TYKD_PASSWD
#define TYKD_PASSWD "tykd@189.cn"
#endif// TYKD_PASSWD

//#define _USE_LOCAL_URL 1// 三个url轮番使用， 否则只用relay url
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CRecordTykdMediaUrl::CRecordTykdMediaUrl() //:CEZThread(PREFIX_CRecordTykdMediaUrl_THREAD_NAME, THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iSigUser = 0;

    m_strIdDst = "";//rtsp地址
    m_strTykdDevID = "";// 新的id

    // hls url
    m_strUrlDst = "";

    DBG(
        ez_trace("CRecordTykdMediaUrl Enter--------\n");
    );

    m_iChildPid = -1;
    m_uiRound =0;

    m_iFileSegTime = def_MinFileSegTime;//单个录像文件时长， s
    m_iFileValidPeriod = def_MinFileValidPeriod;
    m_iRecValidPeriod = 0;
    //m_ttRecStartAt = 0;
    m_iFileListSize = 5;//索引内文件个数
    m_iFileHelperPeriod = 0;

    m_iType=ezDef_StreamType_tykd;          // 默认
    //m_strUrl;           // Atatch
    //m_strAccount;       // Atatch
    //m_strPasswd;        // Atatch
    //m_strVideoEncode;   // Atatch
    m_iGetPlayUrlmethod = em_GetPlayUrlmethod_getPssServerPlayUrlHXReq;

#if defined(_Func_useRecFileHelper)

    m_pRecFileHelper = new CRecFileHelper(this);
    assert(m_pRecFileHelper);
#endif //_Func_useRecFileHelper
    //m_strRecPath = "";

    //setDevStatus(em_terminal_status_offline);
}

CRecordTykdMediaUrl::~CRecordTykdMediaUrl()
{
    DBG(
        ez_trace("CRecordTykdMediaUrl Leave--------\n");
    );

#if defined(_Func_useRecFileHelper)

    if (NULL != m_pRecFileHelper)
    {
        delete m_pRecFileHelper;
    }
#endif //_Func_useRecFileHelper
}

int CRecordTykdMediaUrl::setFileSegTime(const int iFileSegTime)
{
    if (iFileSegTime < def_MinFileSegTime)
    {
        return -1;
    }

    if (iFileSegTime > def_MaxFileSegTime)
    {
        return -2;
    }

    m_iFileSegTime = iFileSegTime;

    return 0;
}
int CRecordTykdMediaUrl::setFileSegTime(const std::string &strFileSegTime)
{
    return setFileSegTime(atoi(strFileSegTime.c_str()));
}

int CRecordTykdMediaUrl::setFileValidPeriod(const int iFileValidPeriod)
{
    if (iFileValidPeriod < def_MinFileValidPeriod)
    {
        return -1;
    }

    if (iFileValidPeriod > def_MaxFileValidPeriod)
    {
        return -2;
    }

    m_iFileValidPeriod = iFileValidPeriod;

    return 0;
}
int CRecordTykdMediaUrl::setFileValidPeriod(const std::string &strFileValidPeriod)
{
    return setFileValidPeriod(atoi(strFileValidPeriod.c_str()));
}

int CRecordTykdMediaUrl::setFileListSize(const std::string &strFileListSize)
{
    int iFileListSize = atoi(strFileListSize.c_str());

    if (iFileListSize>100 || iFileListSize<1)
    {
        return -1;
    }

    m_iFileListSize = iFileListSize;

    return 0;
}

int CRecordTykdMediaUrl::setRecValidPeriod(const int iRecValidPeriod)
{
    if (iRecValidPeriod < def_MinRecValidPeriod)
    {
        return -1;
    }

    m_iRecValidPeriod = iRecValidPeriod;

    return 0;
}
int CRecordTykdMediaUrl::setRecValidPeriod(const std::string &strRecValidPeriod)
{
    return setRecValidPeriod(atoi(strRecValidPeriod.c_str()));
}

int CRecordTykdMediaUrl::getFileSegTime()
{
    return m_iFileSegTime;
}
int CRecordTykdMediaUrl::getFileValidPeriod()
{
    return m_iFileValidPeriod;
}
int CRecordTykdMediaUrl::getFileListSize()
{
    return m_iFileListSize;
}
int CRecordTykdMediaUrl::getRecValidPeriod()
{
    return m_iRecValidPeriod;
}

EZTHREAD_BOOL CRecordTykdMediaUrl::Start(CEZObject * pObj, PusherSignalProc_t pProc)
{
    CPusher::Start(pObj, pProc);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    // if StreamID not defined, we make it same as devid
    if (/*m_strIdDst.length()<1 ||*/m_strTykdDevID.length()<1)
    {
        return bRet;
    }

    CEZLock __lock(m_MutexSigBuffer);

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        //__fline;
        ez_printf_error("attach error\n");

        return bRet;
    }

    if (m_iSigUser==0)
    {

        //std::string strIndexPath;//索引路径
        //std::string strMediaPath;//录像文件路径

        ez_printf_info("FileSegTime:%d, FileValidPeriod:%d\n", m_iFileSegTime, m_iFileValidPeriod);

        CreateThread();
        //m_ttRecStartAt = time(NULL);
        CPusher::onStart();

        bRet = EZTHREAD_BOOL_TRUE;
#if 1
        std::string __strID = getDevChnStreamID();

        if (__strID.length() > def_max_ThreadNameLen)
        {
            __strID = __strID.substr(__strID.length()-def_max_ThreadNameLen);
        }

        std::string strThrName = PREFIX_CRecFileHelper_THREAD_NAME + __strID;
#else
        std::string strThrName = PREFIX_CRecFileHelper_THREAD_NAME + pRecorder->getDevChnStreamID();
#endif
        m_pRecFileHelper->SetThreadName(strThrName);
        // 启动文件管理
        //m_pRecFileHelper->Start();
    }
    else
    {
        bRet = EZTHREAD_BOOL_TRUE;

        DBG(
            __fline;
            printf("no need, CreateThread, have %d users.\n", m_iSigUser);
        );

    }
    m_iSigUser++;
    DBG(
        __fline;
        printf("m_iSigUser:%d\n", m_iSigUser);
    );

    return bRet;
}
EZTHREAD_BOOL CRecordTykdMediaUrl::Stop(CEZObject * pObj, PusherSignalProc_t pProc)
{
    ez_printf_info("CRecordTykdMediaUrl::Stop\n");
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iSigUser>0)
    {
        m_iSigUser--;
    }

    if (m_iSigUser==0)
    {

        if (m_iChildPid>0)
        {
            //int  killReturn = kill( m_iChildPid, SIGKILL);  // 不管最后一个文件
            int  killReturn = kill( m_iChildPid, SIGTERM);  // 这种方式，Kill 最后一个文件处理正常

            ez_printf_info("DevID:%s, StreamID:%s, PullerPid:%d, killReturn:%d\n"
                           , getTykdDevID().c_str()
                           , getIdDst().c_str()
                           , m_iChildPid
                           , killReturn);

            if(killReturn == -1)
            {
                if( errno == ESRCH)      // pid does not exist
                {
                    //std::cout << "Group does not exist!" << std::endl;
                    ez_printf_error("Group does not exist.\n");
                }
                else if( errno == EPERM) // No permission to send signal
                {
                    //std::cout << "No permission to send signal!" << std::endl;
                    ez_printf_error("No permission to send signal.\n");
                }
                else
                {
                    ez_printf_debug("Signal sent. Ok!.\n");
                    //std::cout << "Signal sent. All Ok!" << std::endl;
                }
            }
        }
        // 停止文件管理
        m_pRecFileHelper->Stop();

        //bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
        bRet = DestroyThread();

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            ez_printf_error("DestroyThread failed.\n");
        }
        else
        {
            ez_printf_debug("DestroyThread ok.\n");
        }
    }
    else
    {
        DBG(
            __fline;
            printf("no need, DestroyThread, have %d users.\n", m_iSigUser);
        );
    }

    return bRet;
}

void CRecordTykdMediaUrl::doPull()
{
    CPusher::doPull();

    //int link[2];
    pid_t pid;

    //#if defined (_FUNC_DO_RELAY)

    std::string strAccount = TYKD_ACCOUNT;
    std::string strPasswd = TYKD_PASSWD;

    std::string strDevId= m_strTykdDevID;
    std::string strHlsStreamID = m_strIdDst;
    int iVideoEncode = 0;

    if (strHlsStreamID.length()<1)
    {
        strHlsStreamID = strDevId;
    }

    if (getTykdAccount().length()>0)
    {
        strAccount = getTykdAccount();
    }

    if (getTykdPasswd().length()>0)
    {
        strPasswd = getTykdPasswd();
    }

    std::string strLocalPlayUrl;
    std::string strDirectPlayUrl;
    std::string strRelayPlayUrl;

    int iret = 0;
    int iNvsType = m_iType;

    //m_emDevStatus = em_terminal_status_unknown;
    //setDevStatus(em_terminal_status_unknown);

    //iret = getMediaUrl_Use_getPlayUrlHX(strAccount
    if (ezDef_StreamType_tykd== iNvsType
        ||ezDef_StreamType_JSvCloud == iNvsType)
    {
        // 1-n, but tykd is 0-n
        int iChannel = atoi(getChanel().c_str()) - 1;

        if (iChannel < 0 || iChannel>1024)
            iChannel = 0;

#if defined(_FUNC_liveRecordTykd)

        if (em_GetPlayUrlmethod_getPlayUrlHX == m_iGetPlayUrlmethod)
        {
        // 不再支持
#if 0
            ez_printf_debug("getMediaUrl_Use_getPlayUrlHX:%d\n", m_iGetPlayUrlmethod);
            iret = getMediaUrl_Use_getPlayUrlHX(strAccount
                                                , strPasswd
                                                , strDevId
                                                , iChannel
                                                , atoi(getStream().c_str())
                                                , strLocalPlayUrl
                                                , strDirectPlayUrl
                                                , strRelayPlayUrl);
#else
            iret = -1;
#endif
        }
        else if (em_GetPlayUrlmethod_getPssStoragePlayUrlHXReq == m_iGetPlayUrlmethod)
        {
#if 0
            ez_printf_debug("getMediaUrl_Use_getPssStoragePlayUrlHXReq:%d\n", m_iGetPlayUrlmethod);

            iret = getMediaUrl_Use_getPssStoragePlayUrlHXReq(strAccount
                    , strPasswd
                    , strDevId
                    , iChannel
                    , atoi(getStream().c_str())
                    , strLocalPlayUrl
                    , strDirectPlayUrl
                    , strRelayPlayUrl);
#else
        const char * p_endPoint = NULL;
        if (m_strTykdWsUrl.empty())
            p_endPoint = "http://www.189eyes.com:9000/cxf/MobileService?wsdl";
        else
            p_endPoint = m_strTykdWsUrl.c_str();

        const char * const p_devid=getTykdDevID().c_str();
        //const int i_ch = atoi(getChanel().c_str());
        const int i_stream = atoi(getStream().c_str());
        t_hx_PlayUrl thxPlayUrl;
                
        int iret_get = uniFace_getPssStoragePlayUrlHXReq(p_endPoint, p_devid, iChannel, i_stream, &thxPlayUrl);
        if (0==iret_get)
        {
            iret = 0;
            strLocalPlayUrl = thxPlayUrl.buffPlayUrl_Local;
            strDirectPlayUrl  = thxPlayUrl.buffPlayUrl_Direct;
            strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Relay;

            setDevStatus(em_terminal_status_online);

            ez_printf_debug("uniFace_getPssStoragePlayUrlHXReq:%s_%d_%d\n", p_devid, iChannel, i_stream);
        }
        else
        {
            iret = -1;
            //目前全部设置为不在线
            //if (em_errorno_getMediaUrl_offline == iret_get)
            {
                setDevStatus(em_terminal_status_offline);
            }

            ez_printf_error("failed uniFace_getPssStoragePlayUrlHXReq:%d\n", iret_get);
        }
#endif

        }
        else
        {
#if 0
            ez_printf_debug("getMediaUrl_Use_getPssServerPlayUrlHXReq:%d\n", m_iGetPlayUrlmethod);

            iret = getMediaUrl_Use_getPssServerPlayUrlHXReq(strAccount
                    , strPasswd
                    , strDevId
                    , iChannel
                    , atoi(getStream().c_str())
                    , strLocalPlayUrl
                    , strDirectPlayUrl
                    , strRelayPlayUrl);
#else
        const char * p_endPoint = NULL;
        if (m_strTykdWsUrl.empty())
            p_endPoint = "http://www.189eyes.com:9000/cxf/MobileService?wsdl";
        else
            p_endPoint = m_strTykdWsUrl.c_str();

        const char * const p_devid=getTykdDevID().c_str();
        //const int i_ch = atoi(getChanel().c_str());
        const int i_stream = atoi(getStream().c_str());
        t_hx_PlayUrl thxPlayUrl;
                
        int iret_get = uniFace_getPssServerPlayUrlHXReq(p_endPoint, p_devid, iChannel, i_stream, &thxPlayUrl);
        if (0==iret_get)
        {
            iret = 0;
            strLocalPlayUrl = thxPlayUrl.buffPlayUrl_Local;
            strDirectPlayUrl  = thxPlayUrl.buffPlayUrl_Direct;
            strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Relay;

            setDevStatus(em_terminal_status_online);

            ez_printf_debug("uniFace_getPssServerPlayUrlHXReq:%s_%d_%d\n", p_devid, iChannel, i_stream);
        }
        else
        {
            iret = -1;
            ez_printf_error("Failed uniFace_getPssServerPlayUrlHXReq:%d, End:%s, ID:%s, ch:%d, stream:%d\n", iret_get, p_endPoint, p_devid, iChannel, i_stream);
            //目前全部设置为不在线
            //if (em_errorno_getMediaUrl_offline == iret_get)
            {
                setDevStatus(em_terminal_status_offline);
            }
        }
#endif

        }

#else
        iret = -1;
#endif //#if defined(_FUNC_liveRecordTykd)

        if (iret>=0)
        {
            //m_emDevStatus = em_terminal_status_online;
            setDevStatus(em_terminal_status_online);

#ifdef _USE_LOCAL_URL
            ez_printf_info("%20s:%s\n", "LocalPlayUrl", (strLocalPlayUrl.c_str()));
            ez_printf_info("%20s:%s\n", "DirectPlayUrl", (strDirectPlayUrl.c_str()));
            int i_url_valid_Local = CStreamUtility::ez_url_valid(strLocalPlayUrl);
            int i_url_valid_Direct = CStreamUtility::ez_url_valid(strDirectPlayUrl);
#endif
            ez_printf_info("%20s:%s\n", "RelayPlayUrl", (strRelayPlayUrl.c_str()));

            int i_url_valid_Relay = CStreamUtility::ez_url_valid(strRelayPlayUrl);
            ARG_USED(i_url_valid_Relay);

#ifdef _USE_LOCAL_URL
            //局域网地址检查
            if (0 == i_url_valid_Local)
            {
                ez_parsed_url_t *parsed_url;
                parsed_url = ez_parse_url(strLocalPlayUrl.c_str());
                if (parsed_url)
                {

                    //私有地址不用
                    if ( parsed_url->host != NULL
                         && 1 == isPrivateIPAddress(parsed_url->host) )
                    {
                        ez_printf_info("%s:isPrivateIPAddress, set empty\n", parsed_url->host, (strLocalPlayUrl.c_str()));
                        strLocalPlayUrl = "";
                    }
                    ez_parsed_url_free(parsed_url);
                }
                else
                {
                    ez_printf_error("failed ez_parse_url:%s\n", strLocalPlayUrl.c_str());
                    strLocalPlayUrl = "";
                }
            }
            //直连地址检查
            if (0 == i_url_valid_Direct)
            {
                ez_parsed_url_t *parsed_url;
                parsed_url = ez_parse_url(strDirectPlayUrl.c_str());
                if (parsed_url)
                {

                    //私有地址不用
                    if ( parsed_url->host != NULL
                         && 1 == isPrivateIPAddress(parsed_url->host) )
                    {
                        ez_printf_info("%s:isPrivateIPAddress, set empty\n", parsed_url->host, (strDirectPlayUrl.c_str()));
                        strDirectPlayUrl = "";
                    }
                    ez_parsed_url_free(parsed_url);
                }
                else
                {
                    ez_printf_error("failed ez_parse_url:%s\n", strDirectPlayUrl.c_str());
                    strDirectPlayUrl = "";
                }
            }
#endif//#ifdef _USE_LOCAL_URL

            // 修正relay地址
            //if (0 == i_url_valid_Relay)
            // 在最后统一修正
            if (0)
            {
                std::string __strUrlFix;
                int i_fixRelayPlayUrl = fixRelayPlayUrl(__strUrlFix, strRelayPlayUrl);

                ez_printf_debug("fixRelayPlayUrl:%d\n", i_fixRelayPlayUrl);

                if (0 == i_fixRelayPlayUrl)
                {
                    ez_printf_debug("fixRelayPlayUrl:[%s] -> [%s]\n", strRelayPlayUrl.c_str(), __strUrlFix.c_str());

                    strRelayPlayUrl = __strUrlFix;
                }
            }//fix relay addr

        }
        else
        {
            ez_printf_info("%20s:%d\n", "iret", iret);
            if (-107 == iret) //"device not exist"
            {
                //m_emDevStatus = em_terminal_status_DevNotExist;
                setDevStatus(em_terminal_status_DevNotExist);
            }
            else if (-108 == iret) //"device offline"
            {
                //m_emDevStatus = em_terminal_status_offline;
                setDevStatus(em_terminal_status_offline);
            }
            else
            {
                //m_emDevStatus = em_terminal_status_unknown;
                setDevStatus(em_terminal_status_unknown);
            }

            return;
        }
    }
    else if (ezDef_StreamType_Streamming == m_iType)
    {
        strRelayPlayUrl = m_strUrl;
    }
    else if (ezDef_StreamType_GB_HX == m_iType)
    {

        const char * p_endPoint=m_strEndPoint4VideoRs.empty()?"http://122.227.101.27:60000/gbManager/cxf/rest/client/getPssServerPlayUrlHXReq" : m_strEndPoint4VideoRs.c_str();
        const char * const p_devid=getTykdDevID().c_str();
        const int i_ch = 0;
        const int i_stream = 1;

        //p_endPoint="http://10.27.0.147:60000/gbManager/cxf/rest/client/getPssServerPlayUrlHXReq";

        t_hx_PlayUrl thxPlayUrl;
        memset(&thxPlayUrl, 0, sizeof(t_hx_PlayUrl));

        //ez_printf_debug("EndPoint4VideoRs:%s\n", m_strEndPoint4VideoRs.c_str());
        ez_printf_debug("endPoint:%s\n", p_endPoint);
        ez_printf_debug("devid:%s\n", p_devid);

        int iret_get = uniFace_getPssServerPlayUrlHXReq_GBPLA(p_endPoint, p_devid, i_ch, i_stream, &thxPlayUrl);

        if (0==iret_get)
        {
            const char * const p_show_format="%13s:%-3ld:%s\n";
            ez_printf_debug(p_show_format, "LocalPlayUrl", strlen(thxPlayUrl.buffPlayUrl_Local), thxPlayUrl.buffPlayUrl_Local);
            ez_printf_debug(p_show_format, "DirectPlayUrl", strlen(thxPlayUrl.buffPlayUrl_Direct), thxPlayUrl.buffPlayUrl_Direct);
            ez_printf_debug(p_show_format, "RelayPlayUrl", strlen(thxPlayUrl.buffPlayUrl_Relay), thxPlayUrl.buffPlayUrl_Relay);
            // hls
            //strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Local;
            // rtsp
            //strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Relay;
            // rtmp, use it  at 2020-11-24 9:32:26
            strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Direct;
        }
        else
        {
            ez_printf_error("uniFace_getPssServerPlayUrlHXReq_GBPLA failed:%d\n", iret);
            strRelayPlayUrl.clear();
            //目前全部设置为不在线
            //if (em_errorno_getMediaUrl_offline == iret_get)
            {
                setDevStatus(em_terminal_status_offline);
            }
        }
    }
#if defined(_USE_ezOnvif)
    else if (ezDef_StreamType_onvif == iNvsType)// Onvif协议
    {
        char *__phost = NULL;
        int __port_http = 0;
        int __port_rtsp = 0;

        ez_parsed_url_t *parsed_url = NULL;

        ARG_USED(__port_http);

        if (m_strUrl.length()>sizeof("pss://") )
        {
            //
            parsed_url = ez_parse_url(m_strUrl.c_str());

            do
            {
                if (NULL == parsed_url)
                {
                    break;
                }

                __phost = parsed_url->host;

                // use the port
                if (!strcasecmp(parsed_url->scheme, "rtsp"))
                {
                    __port_rtsp = atoi(parsed_url->port);
                }
                else if (!strcasecmp(parsed_url->scheme, "http"))
                {
                    __port_http = atoi(parsed_url->port);
                }
                else if (!strcasecmp(parsed_url->scheme, "pss"))
                {
                    // 防止：pss://192.168.5.66:18088?http=80&rtsp=554
                    if (NULL == parsed_url->path
                        || NULL == parsed_url->query)
                    {
                        break;
                    }

                    char *pFindPort = NULL;

                    pFindPort = strstr(parsed_url->query, "http=");
                    if (pFindPort)
                    {
                        __port_http = atoi(pFindPort+strlen("http="));
                    }

                    pFindPort = strstr(parsed_url->query, "rtsp=");
                    if (pFindPort)
                    {
                        __port_rtsp = atoi(pFindPort+strlen("rtsp="));
                    }
                }
                else // unknow
                {
                    __phost = NULL;
                    __port_http = 0;
                    __port_rtsp = 0;
                }
            }
            while(0);
        }

        ARG_USED(__port_rtsp);

        std::string strUri;

        if (__phost && __port_http!=0)
        {
            strUri = __phost + std::string(":") + ezConvertToString(__port_http);
        }

        if (parsed_url)
        {
            ez_parsed_url_free(parsed_url);
        }

        t_onvif_device_info deviceInfo;

        ez_printf_debug("ez_ONVIF_getDeviceInfo:%s, %s, %s\n", strUri.c_str(), m_strAccount.c_str(), m_strPasswd.c_str());

        int iret_ONVIF_getMediaProfiles = ez_ONVIF_getDeviceInfo(strUri.c_str(), m_strAccount.c_str(), m_strPasswd.c_str(), deviceInfo);
        ez_printf_debug(" :%d\n", iret_ONVIF_getMediaProfiles);
        if (iret_ONVIF_getMediaProfiles>=0)
        {
            if (deviceInfo.m_vMedia.size() > 0)
            {
                ez_printf_debug("\tvMedia:%ld\n", deviceInfo.m_vMedia.size());
                for (size_t jj=0; jj<deviceInfo.m_vMedia.size(); jj++)
                {
                    strRelayPlayUrl = deviceInfo.m_vMedia[0];
                    ez_printf_debug("\t\t%ld--%s\n", jj, deviceInfo.m_vMedia[jj].c_str());
                    break;
                }
            }
            else
            {
                ez_printf_debug("Media NOT found!\n");
            }
        }//

    }
#endif //_USE_ezOnvif
    else// 其他情况用新接口
    {
        //全球眼是否
        if (ezDef_StreamType_megaeye == m_iType)
        {
            iVideoEncode = (m_iVideoEncode==1) ? 1 : 0;
        }

        char        bufPlayUrl[1024] = {'\0'};   /* 播放地址 */

        // 如果全球眼， 则通过chn字段将id传入
        std::string strTheChannel;
        if (ezDef_StreamType_megaeye == m_iType)
        {
            strTheChannel = m_strTykdDevID;
        }
        else
        {
            strTheChannel = getChanel();
        }

        iret = ez_getStreamPlayUrl(
                   m_iType
                   , m_strUrl    .c_str()
                   , m_strAccount.c_str()
                   , m_strPasswd .c_str()
                   , strTheChannel .c_str()
                   , getStream().c_str()
                   , bufPlayUrl
                   , sizeof(bufPlayUrl)-1);

        //ez_printf_info("ez_getStreamPlayUrl:%d\n", iret);

        if (iret == 0)
        {
            ez_printf_info("PlayUrl:%s\n", bufPlayUrl);

            strLocalPlayUrl = "";
            strDirectPlayUrl = "";

            //目前全球眼URL 需要特殊处理
            if (ezDef_StreamType_megaeye == m_iType)
            {
                //vcodec = 1; // 放在协议中指定

                replace_chars_with_chars(bufPlayUrl, "&amp;", "&");
                //strRelayPlayUrl = bufPlayUrl;
            }
            strRelayPlayUrl = bufPlayUrl;
        }
        else
        {
            ez_printf_error("%s:%d\n", "ez_getStreamPlayUrl", iret);

            return;
        }
    }

    // 修正relay地址
DBG(
    ez_printf_debug("+++++++++++++++ ->%s\n", strRelayPlayUrl.c_str());
);
    {
        int i_url_valid_Relay  = CStreamUtility::ez_url_valid(strRelayPlayUrl);
        if (0 == i_url_valid_Relay)
        {
            std::string __strUrlFix;
            int i_fixRelayPlayUrl = fixRelayPlayUrl(__strUrlFix, strRelayPlayUrl);

            //ez_printf_debug("fixRelayPlayUrl:%d\n", i_fixRelayPlayUrl);

            if (0 == i_fixRelayPlayUrl)
            {
                strRelayPlayUrl = __strUrlFix;

                //ez_printf_debug(" ->%s\n", strRelayPlayUrl.c_str());
            }
        }//fix relay addr
    }
DBG(
    ez_printf_debug("+++++++++++++++ ->%s\n", strRelayPlayUrl.c_str());
);
    size_t ii;
    // 已经注释
#ifdef _USE_LOCAL_URL

    char *argvLocalPlayUrl[64] = {NULL};
    ARG_USED(argvLocalPlayUrl);

    std::vector<std::string> vCmdAndArgs_Local;

    //iret = makeRecCmdFromRtsp(strLocalPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Local, iVideoEncode);
    //iret = makeRecCmdFromStream(strLocalPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Local, iVideoEncode, m_iAudioEncode);//此方法已经取消20201102
    iret = makeRecCmdFromUrl(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, __strRecPath, iVideoEncode, m_iAudioEncode);
    if (iret >= 0)
    {
        printf("strLocalPlayUrl\n");
        for ( ii=0; ii<vCmdAndArgs_Local.size(); ii++)
        {
            argvLocalPlayUrl[ii] = (char *)vCmdAndArgs_Local[ii].c_str();
            //printf("\t%zu--[%s]\n", ii, argvLocalPlayUrl[ii]);
            printf("%s ", argvLocalPlayUrl[ii]);
        }
        argvLocalPlayUrl[ii] = NULL;
        printf("\n");

    }
    else
    {
        printf("strLocalPlayUrl makeRecCmdFromRtsp failed.\n");
    }

    char *argvDirectPlayUrl[64] = {NULL};
    ARG_USED(argvDirectPlayUrl);
    std::vector<std::string> vCmdAndArgs_Direct;
    //iret = makeRecCmdFromRtsp(strDirectPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Direct, iVideoEncode, m_iAudioEncode);
    //iret = makeRecCmdFromStream(strDirectPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Direct, iVideoEncode, m_iAudioEncode);//此方法已经取消20201102
    iret = makeRecCmdFromUrl(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, __strRecPath, iVideoEncode, m_iAudioEncode);

    if (iret >= 0)
    {
        printf("strDirectPlayUrl\n");
        for ( ii=0; ii<vCmdAndArgs_Direct.size(); ii++)
        {
            argvDirectPlayUrl[ii] = (char *)vCmdAndArgs_Direct[ii].c_str();
            //printf("\t%zu--[%s]\n", ii, argvDirectPlayUrl[ii]);
            printf("%s ", argvDirectPlayUrl[ii]);
        }
        argvDirectPlayUrl[ii] = NULL;
        printf("\n");
    }
    else
    {
        printf("strDirectPlayUrl makeRecCmdFromRtsp failed.\n");
    }
#endif//_USE_LOCAL_URL

    char *argvRelayPlayUrl[64] = { NULL };
    ARG_USED(argvRelayPlayUrl);
    std::vector<std::string> vCmdAndArgs_Relay;

    //if (ezDef_StreamType_Streamming == m_iType)
    //{
//#if defined(_FUNC_LiveRecord_FileNoTimeMark)
//    iret = makeRecCmdFromStream_FileNoTimeMark(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, iVideoEncode);
//#else
    //iret = makeRecCmdFromStream(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, iVideoEncode, m_iAudioEncode);
    //m_strRecPath.clear();
    std::string __strRecPath;
    iret = makeRecCmdFromUrl(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, __strRecPath, iVideoEncode, m_iAudioEncode);

    //ez_printf_debug("makeRecCmdFromUrl(%d):%s\n", iret, strRelayPlayUrl.c_str());
//#endif //_FUNC_LiveRecord_FileNoTimeMark

    //}
    //else
    //{
        //iret = makeRecCmdFromRtsp(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, iVideoEncode, m_iAudioEncode);
        //iret = makeRecCmdFromStream(strRelayPlayUrl, strHlsStreamID, m_iFileSegTime, m_iFileListSize, vCmdAndArgs_Relay, iVideoEncode, m_iAudioEncode);
    //}

    if (iret >= 0)
    {
DBG(
        printf("+++++++++++++strRelayPlayUrl\n");
);
        for ( ii=0; ii<vCmdAndArgs_Relay.size(); ii++)
        {
            argvRelayPlayUrl[ii] = (char *)vCmdAndArgs_Relay[ii].c_str();
            //printf("\t%zu--[%s]\n", ii, argvRelayPlayUrl[ii]);
DBG(
            printf("%s ", argvRelayPlayUrl[ii]);
);
        }
        argvRelayPlayUrl[ii] = NULL;
DBG(
        printf("\n");
);

        // 调试用
#if 0
        std::string __strPullCmd_Relay;
        int iret_CmdArray2HumanString = CmdArray2HumanString(vCmdAndArgs_Relay, __strPullCmd_Relay);
        if (iret_CmdArray2HumanString >= 0)
        {
            ez_printf_debug("__strPullCmd_Relay:%s\n", __strPullCmd_Relay.c_str());
            //exit (0);
        }
#endif
        // 维护 m_strRecPath 一次即可， 防止冲突
        // 第一次启动， 其后不必
        if (m_strRecPath.empty())
        {
            m_strRecPath = __strRecPath;
            // 启动文件管理
            m_pRecFileHelper->Start();
        }
    }
    else
    {
        ez_printf_error("strRelayPlayUrl makeRecCmdFromRtsp failed.\n");
    }

    // 如果接口返回了这个数据， 则需要维护录像
    if (m_strRecPath.length()>2
        && m_strRecPathCreatDired != m_strRecPath)
    {
        int iretCreatDir = ez_creat_dir(m_strRecPath.c_str());

        if (iretCreatDir<0)
        {
            ez_printf_error("Failed ez_creat_dir(%d):%s\n", iretCreatDir, m_strRecPath.c_str());
            return;
        }
        else
        {
            m_strRecPathCreatDired = m_strRecPath;
            //ez_printf_debug("ez_creat_dir(%d):%s\n", iretCreatDir, m_strRecPath.c_str());
        }
    }

    pid = fork();
    if ( pid == -1)
    {
        PushEnd();

        ez_printf_error("fork\n");
        sleep(1);

        return ;
    }
    else
    {
        PushStart();
    }

    if(pid == 0)
    {

        //        dup2 (link[1], STDOUT_FILENO);
        //        close(link[0]);
        //        close(link[1]);
        //#if defined (_FUNC_DO_RELAY)

#ifdef _USE_LOCAL_URL
        if (m_uiRound%3 == 1)
        {
            if (NULL != argvRelayPlayUrl[0])
            {
                execv(argvRelayPlayUrl[0], argvRelayPlayUrl+1);
            }
        }
        else if (m_uiRound%3 == 2)
        {
            if (NULL != argvDirectPlayUrl[0])
            {
                execv(argvDirectPlayUrl[0], argvDirectPlayUrl+1);
            }
        }
        else if (m_uiRound%3 == 0)
        {
            if (NULL != argvLocalPlayUrl[0])
            {
                execv(argvLocalPlayUrl[0], argvLocalPlayUrl+1);
            }
        }
        else
        {
            if (NULL != argvRelayPlayUrl[0])
            {
                execv(argvRelayPlayUrl[0], argvRelayPlayUrl+1);
            }
        }
#else //_USE_LOCAL_URL
        if (NULL != argvRelayPlayUrl[0])
        {
            execv(argvRelayPlayUrl[0], argvRelayPlayUrl+1);
        }

#endif //_USE_LOCAL_URL

        //#else
        //
        //        execl("/usr/bin/ping", "ping", "localhost", (char *)0);
        //#endif //_FUNC_DO_RELAY

        die("execl");
    }
    else //(pid > 0)
    {
        //        close(link[1]);

        m_iChildPid = pid;
        //ez_printf_info("Begin, Pid(%d), Id:%s, IdDst:%s\n", m_iChildPid, getDevChnStreamID().c_str(), getIdDst().c_str());

        while (m_bLoop)
        {
#if 0
            int nbytes = read(link[0], foo, sizeof(foo));
            if (nbytes>0)
            {
                ez_printf_info("CPullerTykdJsict:[%.*s]\n", nbytes, foo);
            }
#endif
            //ez_printf_info("waitpid for child now...\n");

            int status;
            int pr = waitpid(pid, &status, WNOHANG);
            if(pr==0)
            {
                //printf("没有子进程退出,继续执行..\n");
                //ez_printf_info("Nochild exited, ok.\n");

                sleep(1);
            }
            else
            {
                //printf("子进程正常退出 PID=%d\n", pr);
                ez_printf_info("End, Pid(%d), Id:%s, IdDst:%s\n", pr, getDevChnStreamID().c_str(), getIdDst().c_str());

                PushEnd();

                break;
            }
        };

        m_iChildPid = -1;
        //wait(NULL);
    }

    m_SigBuffer(MSG_Pusher_Tykd2Record_OK, this);
}

void CRecordTykdMediaUrl::onStoped()
{
    ez_printf_info("m_SigBuffer(MSG_Pusher_Tykd2Record_STOPED, this)\n");

    m_SigBuffer(MSG_Pusher_Tykd2Record_STOPED, this);
}

