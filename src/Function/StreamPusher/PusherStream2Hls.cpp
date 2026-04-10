/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PusherStream2Hls.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PusherStream2Hls.cpp 5884 2016-05-25 09:08:28Z WuJunjie $
 *
 *  Explain:
 *     -
 *       转码为hls/rtmp/rtsp
 *       统一转码
 *     -
 *
 *  Update:
 *     2016-05-25 09:08:28  Create
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
#include <str_opr.h>
#include <ez_socket.h>
#include <ez_url_parser.h>
#include "StringOpr.h"

#include "ez_util_platform_Exec.h"/*ez_Exec*/
#if defined(_USE_ezOnvif)
#include "ezOnvif.h"
#endif

#include "../../Logs.h"
#include <sha1.h>
#ifdef _FUNC_gMiniLicense
#include "../../Solar.h"
#endif // _FUNC_gMiniLicense

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif //_USE_MODULES_EZCONFIG
//#if defined (_FUNC_DO_RELAY)

#include "../Pandora/StreamUtility.h" /*工具*/

#include "dtPst.h" /* makePullCmdRtmp */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */
#include "uniFace.h"

//#endif //_FUNC_DO_RELAY
#include "PusherStream2Hls.h"
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
#ifndef DEFAULT_RTMP_URL
#define DEFAULT_RTMP_URL "rtmp://" "61.160.149.160" ":19090/live"
#endif// DEFAULT_RTMP_URL

#ifndef DEFAULT_HLS_URL_HEADER
#define DEFAULT_HLS_URL_HEADER "http://" "61.160.149.160" ":18080/hls"
#endif// DEFAULT_HLS_URL_HEADER
//https live stream
#ifndef DEFAULT_HLSs_URL_HEADER
#define DEFAULT_HLSs_URL_HEADER "http://" "61.160.149.160" ":18080/hls"
#endif// DEFAULT_HLSs_URL_HEADER

#ifndef DEFAULT_FLV_URL_HEADER
#define DEFAULT_FLV_URL_HEADER "http://" "61.160.149.160" ":18080/lflv?port=19090&app=live&stream="
#endif// DEFAULT_FLV_URL_HEADER
#ifndef DEFAULT_DASH_URL_HEADER
#define DEFAULT_DASH_URL_HEADER "http://" "61.160.149.160" ":18080/dash"
#endif// DEFAULT_DASH_URL_HEADER

#ifndef TYKD_ACCOUNT
#define TYKD_ACCOUNT "hx"
#endif// TYKD_ACCOUNT

#ifndef TYKD_PASSWD
#define TYKD_PASSWD "tykd@189.cn"
#endif// TYKD_PASSWD
// 当前只有一个url， 因此只使用relay ...
//#define _USE_LOCAL_URL 1

#define def_Mark_Hk8200_ss "fe5ea288647c2edaf39869ea7a18fb7402a0323e::"

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CPusherStream2Hls::CPusherStream2Hls()// :CEZThread(PREFIX_CPusherStream2Hls_THREAD_NAME, THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iSigUser = 0;

    m_strIdDst = "";//rtsp地址
    m_strTykdDevID = "";// 新的id

    // hls url
    //m_strUrlDst = "";

    DBG(
        ez_printf_debug("CPusherStream2Hls Enter--------\n");
    );

    m_iChildPid = -1;

    m_uiRound =0;

    m_strShaChkSum = "043979497";// dbg dev huid

    m_uPushFunc = 0;
    //m_ulStartPoint = 0;
    //m_iValidPeriod = 0;

    setDevStatus(em_terminal_status_offline);

    m_iPushedByTheFront = 0;
}

CPusherStream2Hls::~CPusherStream2Hls()
{
    ez_printf_debug("Leave:%s\n", __FUNCTION__);
}

// -2 - 数据不全
int CPusherStream2Hls::checkValid()
{
    int iNvsType = atoi(m_strType   .c_str());

    switch(iNvsType)
    {
        //case ezDef_StreamType_Streamming    : //1
        case ezDef_StreamType_IPC_hikvision : //2
        case ezDef_StreamType_IPC_dahua     : //3
        {
            if (m_strID.length() < 1
                ||m_strAccount.length() < 1
                || m_strPasswd.length() < 1
                || m_strUrl.length() < 1)
            {
                ez_printf_debug("%s:%s\n", "m_strID", m_strID.c_str());
                ez_printf_debug("%s:%s\n", "m_strAccount", m_strAccount.c_str());
                ez_printf_debug("%s:%s\n", "m_strPasswd", m_strPasswd.c_str());
                ez_printf_debug("%s:%s\n", "m_strUrl", m_strUrl.c_str());

                return -2;
            }
            break;
        }
        //    case ezDef_StreamType_megaeye       : //4
        //    case ezDef_StreamType_tykd          : //5
        //    case ezDef_StreamType_JSvCloud      : //10
        //    case ezDef_StreamType_hk8200        : //6
        //    case ezDef_StreamType_hk7600        : //7 //o￡?μ7600??ì¨
        //    case ezDef_StreamType_IPC_tdwy      : //8 // ììμ??°òμ
        //    case ezDef_StreamType_spj           : //9
        //    case ezDef_StreamType_onvif         : //16
        //    case ezDef_StreamType_ppp           : //17

        default : // 可选的
        {
            break;
        }

    }

    return 0;
}

EZTHREAD_BOOL CPusherStream2Hls::Start(CEZObject * pObj, PusherSignalProc_t pProc)
{
    CPusher::Start(pObj, pProc);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    // if StreamID not defined, we make it same as devid
    if (/*m_strIdDst.length()<1 ||*/getIdDst().length()<1)
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
        std::string strUrlPrefixHLS = DEFAULT_HLS_URL_HEADER;
        std::string strUrlPrefixHLSs = DEFAULT_HLSs_URL_HEADER;
        std::string strUrlPrefixRTMP = DEFAULT_RTMP_URL;
        std::string strUrlPrefixFlv = DEFAULT_FLV_URL_HEADER;
        std::string strUrlPrefixDash = DEFAULT_DASH_URL_HEADER;

        //m_strHostRelative = "";
        //
        m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

        m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense

        //#ifdef _USE_MODULES_EZCONFIG
#if defined(_FUNC_PullTykdMobileService)

        CConfigPullTykdMobileService __cfg;
        __cfg.update();
        strUrlPrefixHLS = __cfg.getConfig().strDefaultUrlPrefixHLS;
        strUrlPrefixHLSs = __cfg.getConfig().strDefaultUrlPrefixHLSs;
        strUrlPrefixRTMP = __cfg.getConfig().strDefaultUrlPrefixRTMP;
        strUrlPrefixFlv = __cfg.getConfig().strDefaultUrlPrefixFlv;
        strUrlPrefixDash = __cfg.getConfig().strDefaultUrlPrefixDash;

//#if defined(_SUPPORT_CFG_TYKDWSURL)
//        m_strTykdWsUrl = __cfg.getConfig().strTykdWsUrl;
//#endif //_SUPPORT_CFG_TYKDWSURL
        // have been setting at CStreamPusherTykd::startPull
        //setHostRelative(__cfg.getConfig().strHostRelative);
        m_iUsePlayUrl_Local = __cfg.getConfig().iUsePlayUrl_Local;
        m_iUsePlayUrl_Direct = __cfg.getConfig().iUsePlayUrl_Direct;
        m_iUsePlayUrl_Relay = __cfg.getConfig().iUsePlayUrl_Relay;
#else //_USE_MODULES_EZCONFIG
        // use default value
#endif //_USE_MODULES_EZCONFIG

        // 本类中不用
        //if (strUrlPrefixHLS.length() > 10) // 添加判断即防错， 也可不配不返回
        //    m_strUrlDst = strUrlPrefixHLS +"/" + getIdDst() + ".m3u8";

        // Create url of hls
        if (strUrlPrefixHLS.length() > 10) // 添加判断即防错， 也可不配不返回
            m_strUrlHls = strUrlPrefixHLS +"/" + getIdDst() + ".m3u8";
        if (strUrlPrefixHLSs.length() > 10) // 添加判断即防错， 也可不配不返回
            m_strUrlHlss = strUrlPrefixHLSs +"/" + getIdDst() + ".m3u8";

        if (strUrlPrefixRTMP.length() > 10) // 添加判断即防错， 也可不配不返回
            m_strUrlRtmp = strUrlPrefixRTMP +"/" + getIdDst();// + ".m3u8";

        if (strUrlPrefixFlv.length() > 10) // 添加判断即防错， 也可不配不返回
            m_strUrlFlv = strUrlPrefixFlv + getIdDst();
            //m_strUrlFlv = strUrlPrefixFlv+"?port=19090&app=live&stream=" + getIdDst();

        if (strUrlPrefixDash.length() > 10) // 添加判断即防错， 也可不配不返回
            m_strUrlDash = strUrlPrefixDash +"/" + getIdDst() + ".mpd";

        CreateThread();
        //
        CPusher::onStart();

        bRet = EZTHREAD_BOOL_TRUE;
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
        ez_printf_debug("m_iSigUser:%d\n", m_iSigUser);
    );
    return bRet;

}
EZTHREAD_BOOL CPusherStream2Hls::Stop(CEZObject * pObj, PusherSignalProc_t pProc)
{
    ez_printf_debug("CPusherStream2Hls::Stop\n");
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
            int  killReturn = kill( m_iChildPid, SIGKILL);  // Kill child process group
            //ez_printf_info("kill:%d, ret:%d\n", m_iChildPid, killReturn);

            if (killReturn == -1)
            {

                if (errno == ESRCH)      // pid does not exist
                {
                    //std::cout << "Group does not exist!" << std::endl;
                    ez_printf_error("kill:%d, ret:%d, Group does not exist\n", m_iChildPid, killReturn);
                }
                else if( errno == EPERM) // No permission to send signal
                {
                    //std::cout << "No permission to send signal!" << std::endl;
                    ez_printf_error("kill:%d, ret:%d, No permission to send signal\n", m_iChildPid, killReturn);
                }
                else
                {
                    //std::cout << "Signal sent. All Ok!" << std::endl;
                    ez_printf_debug("KChildPid:%d, ret:%d, Signal sent\n", m_iChildPid, killReturn);
                }
            }
            else
            {
                //usleep(5*1000);
            }
        }
        //bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
        bRet = DestroyThread();

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            ez_printf_error("DestroyThread failed.\n");
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
            printf("no need, DestroyThread, have %d users.\n", m_iSigUser);
        );
    }

    return bRet;
}

void CPusherStream2Hls::doPull()
{
    CPusher::doPull();

    // 前端推流模式
    if  (1 == m_iPushedByTheFront)
    {
        PushStart();

        while (m_bLoop)
        {

            sleep(1);
        };

        PushEnd();
        return;    	
    }

    //int link[2];
    pid_t pid;

    //#if defined (_FUNC_DO_RELAY)
#if 0

    std::string strAccount = TYKD_ACCOUNT;
    std::string strPasswd = TYKD_PASSWD;

    std::string strDevId= m_strTykdDevID;
    std::string strHlsStreamID = m_strIdDst;

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
#else
    std::string strHlsStreamID = m_strIdDst;

#endif

    std::string strLocalPlayUrl = "";
    std::string strDirectPlayUrl = "";
    std::string strRelayPlayUrl = "";
#if 0
    ez_printf_debug("Do(%s, %s, %s, %s, %s, %s, %s)\n"
                   , getTykdDevID().c_str()
                   , m_strType   .c_str()
                   , m_strUrl    .c_str()
                   , m_strAccount.c_str()
                   , m_strPasswd .c_str()
                   , m_strChnnel .c_str()
                   , m_strSubtype.c_str()
                  );
#endif
    int iret = 0;
    char        pPlayUrl[1024] = {'\0'};   /* 播放地址 */

    const int iNvsType = atoi(m_strType   .c_str());
    std::string __strChannel = m_strChnnel;

    // 天翼看店
    if (ezDef_StreamType_tykd== iNvsType
        ||ezDef_StreamType_JSvCloud == iNvsType)
    {
        std::string strAccount = "";
        std::string strPasswd = "";

#if defined(_FUNC_LiveStreammingTykd2Hls) || defined(_FUNC_LiveStreammingTykd2Rtsp)
#if 0
        iret = getMediaUrl_Use_getPssServerPlayUrlHXReq(strAccount
                , strPasswd
                , getTykdDevID()
                , atoi(getChanel().c_str())-1 //协议定义从1开始， 看店协议从0开始
                , atoi(getStream().c_str())
                , strLocalPlayUrl
                , strDirectPlayUrl
                , strRelayPlayUrl);
#else
        const char * p_endPoint = NULL;
        if (m_strTykdWsUrl.empty())
        {
#if defined (_VENDOR_hljmclz)
            p_endPoint = "http://42.101.46.84:9000/tykd/cxf/MobileService?wsdl";
#else
            p_endPoint = "http://www.189eyes.com:9000/cxf/MobileService?wsdl";
#endif
            ez_printf_error("m_strTykdWsUrl.empty\n");
        }
        else
        {
            p_endPoint = m_strTykdWsUrl.c_str();
        }

        const char * const p_devid=getTykdDevID().c_str();
#if defined(_FUNC_TykdChnFromZero)
        const int i_ch = atoi(getChanel().c_str());
#else
        const int i_ch = atoi(getChanel().c_str())-1;
#endif //_FUNC_TykdChnFromZero
        const int i_stream = atoi(getStream().c_str());
        t_hx_PlayUrl thxPlayUrl;
                
        int iret_get = uniFace_getPssServerPlayUrlHXReq(p_endPoint, p_devid, i_ch, i_stream, &thxPlayUrl);
        if (0==iret_get)
        {
            iret = 0;
            strLocalPlayUrl = thxPlayUrl.buffPlayUrl_Local;
            strDirectPlayUrl  = thxPlayUrl.buffPlayUrl_Direct;
            strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Relay;

            setDevStatus(em_terminal_status_online);
        }
        else
        {
            iret = -1;

            //ez_printf_error("uniFace_getPssServerPlayUrlHXReq failed:%s_%d_%d\n",  p_devid, i_ch, i_stream);
            //ez_printf_error("uniFace_getPssServerPlayUrlHXReq failed.\n p_endPoint:%s\n p_devid:%s\n i_ch:%d\n i_stream:%d\n", p_endPoint, p_devid, i_ch, i_stream);

            //目前全部设置为不在线
            //if (em_errorno_getMediaUrl_offline == iret_get)
            {
                setDevStatus(em_terminal_status_offline);
            }
        }
#endif
	
#else

        iret = -1;
#endif //#if defined(_FUNC_LiveStreammingTykd2Hls) || defined(_FUNC_LiveStreammingTykd2Rtsp)

        if (iret>=0)
        {
#ifdef _USE_LOCAL_URL
            ez_printf_info("%15s:%s\n", "LocalPlayUrl", (strLocalPlayUrl.c_str()));
            ez_printf_info("%15s:%s\n", "DirectPlayUrl", (strDirectPlayUrl.c_str()));
#endif // #ifdef _USE_LOCAL_URL
            ez_printf_info("%15s:%s\n", "RelayPlayUrl", (strRelayPlayUrl.c_str()));


            int i_url_valid_Local  = -1;
            int i_url_valid_Direct = -1;
            int i_url_valid_Relay  = -1;

            // 根据配置确定播放地址的使用与否
            if (1 != m_iUsePlayUrl_Local)
                strLocalPlayUrl.clear();
            else
                i_url_valid_Local  = CStreamUtility::ez_url_valid(strLocalPlayUrl);

            if (1 != m_iUsePlayUrl_Direct)
                strDirectPlayUrl.clear();
            else
                i_url_valid_Direct = CStreamUtility::ez_url_valid(strDirectPlayUrl);

            if (1 != m_iUsePlayUrl_Relay)
                strRelayPlayUrl.clear();
            else
                i_url_valid_Relay  = CStreamUtility::ez_url_valid(strRelayPlayUrl);

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
                        //ez_printf_info("%s:isPrivateIPAddress, set empty\n", parsed_url->host, (strLocalPlayUrl.c_str()));
                        strLocalPlayUrl.clear();// = "";
                    }
                    ez_parsed_url_free(parsed_url);
                }
                else
                {
                    ez_printf_error("failed ez_parse_url:%s\n", strLocalPlayUrl.c_str());
                    strLocalPlayUrl.clear();// = "";
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
                        strDirectPlayUrl.clear();// = "";
                    }
                    ez_parsed_url_free(parsed_url);
                }
                else
                {
                    ez_printf_error("failed ez_parse_url:%s\n", strDirectPlayUrl.c_str());
                    strDirectPlayUrl.clear();// = "";
                }
            }

            // 修正relay地址
            if (0 == i_url_valid_Relay)
            {
                std::string __strUrlFix;
                int i_fixRelayPlayUrl = fixRelayPlayUrl(__strUrlFix, strRelayPlayUrl);

                //ez_printf_debug("fixRelayPlayUrl:%d\n", i_fixRelayPlayUrl);

                if (0 == i_fixRelayPlayUrl)
                {
                    strRelayPlayUrl = __strUrlFix;
                }
            }//fix relay addr
        }
        else
        {
            ez_printf_info("getPssServerPlayUrlHXReq(%d):%s, %s, %s\n", iret, getTykdDevID().c_str(), getChanel().c_str(), getStream().c_str());

            return;
        }

    }
    else if (ezDef_StreamType_hk8200 == iNvsType)// 海康8200流媒体 针对镇江项目
    {
        //const char *cmd="runGetPlayUrlHk8200.sh";
        std::string strCmd = std::string("runGetPlayUrlHk8200.sh ") + m_strID;

        std::vector<std::string> resvec;

        iret = ez_Exec(strCmd.c_str(), resvec);

        char bufJson[1024] = {'\0'};
        std::string strRtspHK8200;

        if (iret > 0) // 返回的是输出条数
        {
            //std::string strJson;

            for (size_t ii=0; ii<resvec.size(); ii++)
            {
                DBG(
                    std::cout << ii << ":" << resvec[ii] << std::endl;
                );
                //std::size_t found = resvec[ii].find_first_of(def_Mark_Hk8200_ss);
                char *pMark = strstr((char *)resvec[ii].c_str(), def_Mark_Hk8200_ss);
                std::size_t lenMark = strlen(def_Mark_Hk8200_ss);
                if (pMark) // found
                {
                    // 取后面的字符串
                    //strJson = pMark+lenMark;
                    strcpy(bufJson, pMark+lenMark);
                    break;
                }

            }

            //ez_printf_info("bufJson--:%s\n", bufJson);

            char *pRtspPos = strstr(bufJson, "rtsp://");

            if (pRtspPos) // found
            {
                char *pEndPos = strstr(bufJson, "\"}");
                if (pEndPos)
                {
                    *pEndPos = '\0';
                }

                strRtspHK8200 = pRtspPos;
            }

            //std::cout << "strRtspHK8200:" << strRtspHK8200 << std::endl;

            ez_printf_info("strRtspHK8200--:%s\n", strRtspHK8200.c_str());

        }
        else
        {
            ez_printf_error("%s:%d\n", "iret", iret);

            return;
        }


        if (strRtspHK8200.length() > 4)
        {
            ez_printf_info("strRtspHK8200--:%s\n", strRtspHK8200.c_str());
            strRelayPlayUrl = strRtspHK8200;

            setDevStatus(em_terminal_status_online);
        }
        else
        {
            ez_printf_error("%s:%d\n", "iret", iret);
            setDevStatus(em_terminal_status_offline);

            return;
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

        ez_printf_debug("iret_ONVIF_getMediaProfiles:%d\n", iret_ONVIF_getMediaProfiles);

        if (iret_ONVIF_getMediaProfiles >= 0)
        {
            if (deviceInfo.m_vMedia.size() > 0)
            {
                ez_printf_debug("\tvMedia:%ld\n", deviceInfo.m_vMedia.size());

                for (size_t jj=0; jj<deviceInfo.m_vMedia.size(); jj++)
                {
#if 0 // 暂时不用
                    int iChn;
                    int iSub;
                    ezConvertFromString(iSub, m_strSubtype);
                    ezConvertFromString(iChn, m_strChnnel);
#endif
                    strRelayPlayUrl = deviceInfo.m_vMedia[0];

                    setDevStatus(em_terminal_status_online);

                    ez_printf_debug("\t\t%ld--%s\n", jj, deviceInfo.m_vMedia[jj].c_str());
                    break;
                }
            }
            else
            {
                ez_printf_error("Onvif::--Media NOT found!\n");
                setDevStatus(em_terminal_status_offline);
            }
        }//

    }
#endif //#if defined(_USE_ezOnvif)
    else// 其他
    {
        // 由于设计原因， 全球眼的id通过channel参数传入
        if (ezDef_StreamType_megaeye== iNvsType)
        {
            __strChannel = getTykdDevID();

            if (m_strUrl.length() < 14/*http://0.0.0.0*/)
                m_strUrl="http://58.223.251.12:9001";
        }
 
        //iret = ez_getStreamPlayUrl(pType, pUrl, pAccount , pPasswd  , pChannel , pSubtype , pPlayUrl);
        iret = ez_getStreamPlayUrl(
                   iNvsType
                   , m_strUrl    .c_str()
                   , m_strAccount.c_str()
                   , m_strPasswd .c_str()
                   , __strChannel .c_str()
                   , m_strSubtype.c_str()
                   , pPlayUrl, sizeof(pPlayUrl)-1);

        //ez_printf_debug("ez_getStreamPlayUrl:%d\n", iret);

        if (iret == 0)
        {
            ez_printf_debug("PlayUrl:%s\n", pPlayUrl);

            strRelayPlayUrl = pPlayUrl;

            setDevStatus(em_terminal_status_online);
        }
        else
        {
            ez_printf_error("getStreamPlayUrl:%d\n", iret);
            setDevStatus(em_terminal_status_offline);

            return;
        }
    }

    //int iType = atoi(m_strType.c_str());
#if 0

    int vcodec = 0;// default is copy

    // 指定是否重新编码
    if (m_strVideoEncode == "1")
    {
        vcodec = 1;
    }
    //int acodec =
    ez_printf_debug("vcodec:%d\n", vcodec);
#endif

    //目前全球眼URL 需要特殊处理
    if (ezDef_StreamType_megaeye == iNvsType)
    {
        //vcodec = 1; // 放在协议中指定

        replace_chars_with_chars(pPlayUrl, "&amp;", "&");
        strRelayPlayUrl = pPlayUrl;
    }

    // 在父进程中赋值
    m_strPlayUrl_Src = strRelayPlayUrl;
    //ez_printf_debug("m_strPlayUrl_Src:%s\n", m_strPlayUrl_Src.c_str());

    size_t ii;
#ifdef _USE_LOCAL_URL

    char *argvLocalPlayUrl[64] = {NULL};

    if (strLocalPlayUrl.length() > 3/*rtsp, rtmp, rtp ...*/)
    {
        ARG_USED(argvLocalPlayUrl);

        std::vector<std::string> vCmdAndArgs_Local;

        iret = makePullCmdRtmp(strLocalPlayUrl, strHlsStreamID, vCmdAndArgs_Local, m_iVideoEncode, m_iAudioEncode, m_iVideoFps/*默认0*/, m_iVideoWidth/*默认0，服务器自定*/);
        if (iret >= 0)
        {
            DBG(
                printf("strLocalPlayUrl\n");
            );
            for ( ii=0; ii<vCmdAndArgs_Local.size(); ii++)
            {
                argvLocalPlayUrl[ii] = (char *)vCmdAndArgs_Local[ii].c_str();
                //printf("\t%zu--[%s]\n", ii, argvLocalPlayUrl[ii]);
                DBG(
                    printf("%s ", argvLocalPlayUrl[ii]);
                );
            }
            argvLocalPlayUrl[ii] = NULL;
            DBG(
                printf("\n");
            );
#if 0

            std::string MyArgs;

            for ( ii=2; ii<vCmdAndArgs.size(); ii++)
            {
                if (MyArgs.length()>1)
                {
                    MyArgs += " " + vCmdAndArgs[ii];
                }
                else
                {
                    MyArgs += vCmdAndArgs[ii];
                }
            }
            argvLocalPlayUrl[2] = (char *)MyArgs.c_str();
            argvLocalPlayUrl[3] = NULL;

            printf("\t\t%zu--[%s]\n", 0, argvLocalPlayUrl[0]);
            printf("\t\t%zu--[%s]\n", 1, argvLocalPlayUrl[1]);
            printf("\t\t%zu--[%s]\n", 2, argvLocalPlayUrl[2]);
#endif

        }
        else
        {
            ez_printf_error("strLocalPlayUrl makePullCmdRtmp failed.\n");
        }
    }
    char *argvDirectPlayUrl[64] = {NULL};

    if (strDirectPlayUrl.length() > 3/*rtsp, rtmp, rtp ...*/)
    {
        ARG_USED(argvDirectPlayUrl);
        std::vector<std::string> vCmdAndArgs_Direct;
        iret = makePullCmdRtmp(strDirectPlayUrl, strHlsStreamID, vCmdAndArgs_Direct, m_iVideoEncode, m_iAudioEncode, m_iVideoFps/*默认0*/, m_iVideoWidth/*默认0，服务器自定*/);
        if (iret >= 0)
        {
            DBG(
                printf("strDirectPlayUrl\n");
            );
            for ( ii=0; ii<vCmdAndArgs_Direct.size(); ii++)
            {
                argvDirectPlayUrl[ii] = (char *)vCmdAndArgs_Direct[ii].c_str();
                //printf("\t%zu--[%s]\n", ii, argvDirectPlayUrl[ii]);
                DBG(
                    printf("%s ", argvDirectPlayUrl[ii]);
                );
            }
            argvDirectPlayUrl[ii] = NULL;
            DBG(
                printf("\n");
            );
        }
        else
        {
            ez_printf_error("strDirectPlayUrl makePullCmdRtmp failed.\n");
        }
    }
#endif//
    char *argvRelayPlayUrl[64] =
        {
            NULL
        };
    ARG_USED(argvRelayPlayUrl);
    std::vector<std::string> vCmdAndArgs_Relay;

    ez_printf_debug("AE:%d, VE:%d, VR:%d, VFps:%d\n", m_iAudioEncode, m_iVideoEncode, m_iVideoWidth, m_iVideoFps);

    iret = makePullCmdRtmp(strRelayPlayUrl, strHlsStreamID, vCmdAndArgs_Relay, m_iVideoEncode, m_iAudioEncode, m_iVideoFps/*默认0*/, m_iVideoWidth/*默认0，服务器自定*/);
    if (iret >= 0)
    {
        DBG(
            printf("strRelayPlayUrl\n");
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

        std::string __strPullCmd_Relay;
        int iret_CmdArray2HumanString = CmdArray2HumanString(vCmdAndArgs_Relay, __strPullCmd_Relay);
        if (iret_CmdArray2HumanString >= 0)
        {
            //ez_printf_debug("__strPullCmd_Relay:%s\n", __strPullCmd_Relay.c_str());
            //exit (0);
        }

    }
    else
    {
        ez_printf_error("strRelayPlayUrl makePullCmdRtmp failed::%s\n", strRelayPlayUrl.c_str());
    }

    //#else

    //#endif //_FUNC_DO_RELAY

    //    if (pipe(link)==-1)
    //    {
    //        ez_printf_error("pipe\n");
    //        sleep(1);
    //
    //        return ;
    //    }

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
        //ez_printf_debug("PushStart\n");
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

        //#else //_FUNC_DO_RELAY
        //
        //        execl("/usr/bin/ping", "ping", "localhost", (char *)0);
        //#endif //_FUNC_DO_RELAY

        die("execl");
    }
    else //(pid > 0)
    {
        //        close(link[1]);
        m_iChildPid = pid;
        signal(SIGCHLD, SIG_IGN);

        ez_printf_info("Begin, Pid(%d), Id:%s, IdDst:%s\n", m_iChildPid, getDevChnStreamID().c_str(), getIdDst().c_str());

        while (m_bLoop)
        {
#if 0
            int nbytes = read(link[0], foo, sizeof(foo));
            if (nbytes>0)
            {
                ez_printf_info("CPusherStream2Hls:[%.*s]\n", nbytes, foo);
            }
#endif
            //ez_printf_info("waitpid for child now...\n");

            int status;
            int pr = waitpid(pid, &status, WNOHANG);
            if(pr==0)
            {
                //printf("没有子进程退出,继续执行..\n");
                //ez_printf_info("Nochild exited, ok.\n");

                // 在父进程中赋值
                //m_strPlayUrl_Src = strRelayPlayUrl;
                //ez_printf_debug("m_strPlayUrl_Src:%s\n", m_strPlayUrl_Src.c_str());

                sleep(1);
            }
            else
            {
                // 防止先退出
                //sleep(1);

                //printf("子进程正常退出 PID=%d\n", pr);

                ez_printf_info("End, Pid(%d), Id:%s, IdDst:%s\n", pr, getDevChnStreamID().c_str(), getIdDst().c_str());

                PushEnd();

                break;
            }
        };

        m_iChildPid = -1;
        //wait(NULL);
    }

    m_SigBuffer(MSG_Pusher_Stream2Hls_OK, this);
}

const std::string &CPusherStream2Hls::getUrlHls()
{
    return m_strUrlHls;
}

const std::string &CPusherStream2Hls::getUrlHlsFix ()
{
    CStreamUtility::fixUrl(m_strUrlHlsFix, m_strUrlHls, m_strShaChkSum);

    return m_strUrlHlsFix;
}
const std::string &CPusherStream2Hls::getUrlHlss()
{
    return m_strUrlHlss;
}

const std::string &CPusherStream2Hls::getUrlHlssFix ()
{
    CStreamUtility::fixUrl(m_strUrlHlssFix, m_strUrlHlss, m_strShaChkSum);

    return m_strUrlHlssFix;
}

const std::string &CPusherStream2Hls::getUrlRtmp ()
{
    return m_strUrlRtmp ;
}

const std::string &CPusherStream2Hls::getUrlRtmpFix ()
{
    CStreamUtility::fixUrl(m_strUrlRtmpFix, m_strUrlRtmp, m_strShaChkSum);

    return m_strUrlRtmpFix;
}
const std::string &CPusherStream2Hls::getUrlRtsp ()
{
    return m_strUrlRtsp ;
}

const std::string &CPusherStream2Hls::getUrlRtspFix ()
{
    CStreamUtility::fixUrl(m_strUrlRtspFix, m_strUrlRtsp, m_strShaChkSum);

    return m_strUrlRtspFix;
}

const std::string &CPusherStream2Hls::getUrlFlv()
{
    return m_strUrlFlv;
}

// 特殊，自行计算
const std::string &CPusherStream2Hls::getUrlFlvFix ()
{
    m_strUrlFlvFix = "";

    if (m_strUrlFlv.length()<5)
    {
        return m_strUrlFlvFix;
    }

    std::string strShaChkSum = m_strShaChkSum;

    strShaChkSum += m_strUrlFlv;

    char dst[SHA1HashSize*2+1] = {0};
    int iret = ez_sha1_str(dst, sizeof(dst), strShaChkSum.c_str());
    if (iret>=0)
    {
        m_strUrlFlvFix = m_strUrlFlv+"&"; // 已经处于参数状态了
        m_strUrlFlvFix += fixUrl_URL_SIGN_PARA_MARK+std::string("=");
        m_strUrlFlvFix += dst;

    }
    else
    {
        m_strUrlFlvFix = m_strUrlFlv;
    }

    return m_strUrlFlvFix;
}

const std::string &CPusherStream2Hls::getUrlDash()
{
    return m_strUrlDash;
}

const std::string &CPusherStream2Hls::getUrlDashFix ()
{
    CStreamUtility::fixUrl(m_strUrlDashFix, m_strUrlDash, m_strShaChkSum);

    return m_strUrlDashFix;
}

const std::string &CPusherStream2Hls::getTitle()
{
    return m_strTitle;
}
const std::string &CPusherStream2Hls::getMemo ()
{
    return m_strMemo;
}
const std::string &CPusherStream2Hls::getAddr ()
{
    return m_strAddr;
}
const std::string &CPusherStream2Hls::getEndDt()
{
    return m_strEndDt;
}

void CPusherStream2Hls::onStoped()
{
    m_SigBuffer(MSG_Pusher_Stream2Hls_STOPED, this);
}
