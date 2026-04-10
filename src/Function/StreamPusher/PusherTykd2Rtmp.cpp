/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PusherTykd2Rtmp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PusherTykd2Rtmp.cpp 5884 2016-05-25 09:08:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
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
#include <ez_url_parser.h>

#include "../../Logs.h"
#include <sha1.h>
#ifdef _FUNC_gMiniLicense
#include "../../Solar.h"
#endif // _FUNC_gMiniLicense

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif //_USE_MODULES_EZCONFIG
//#if defined (_FUNC_DO_RELAY)
//#include "gPushMan/tykdMobileServiceHX_Cli.h"

#include "../Pandora/StreamUtility.h" /*工具*/

//#include "../Interface/tykdWebService/tykdMobileServiceClient.h"
#include "dtPst.h" /* makePullCmdRtmp */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */
#include "uniFace.h"

//#endif //_FUNC_DO_RELAY
#include "PusherTykd2Rtmp.h"
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

#ifndef DEFAULT_RTMP_URL
#define DEFAULT_RTMP_URL "rtmp://" "61.160.149.160" ":19090/live"
#endif// DEFAULT_RTMP_URL

#ifndef DEFAULT_HLS_URL_HEADER
#define DEFAULT_HLS_URL_HEADER "http://" "61.160.149.160" ":18080/hls"
#endif// DEFAULT_HLS_URL_HEADER


#define _USE_LOCAL_URL 1
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CPusherTykd2Rtmp::CPusherTykd2Rtmp()// :CEZThread(PREFIX_CPusherTykd2Rtmp_THREAD_NAME, THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iSigUser = 0;

    m_strIdDst = "";//rtsp地址
    m_strTykdDevID = "";// 新的id

    // hls url
    m_strUrlDst = "";

    DBG(
        ez_printf_debug("CPusherTykd2Rtmp Enter--------\n");
    );

    m_iChildPid = -1;

    m_uiRound =0;

    m_strShaChkSum = "043979497";// dbg dev huid

    m_iGetPlayUrlmethod = em_GetPlayUrlmethod_getPssServerPlayUrlHXReq;

    m_iVideoEncode = 0;/*0-copy, 1-libx264*/
    m_iAudioEncode = 2;/*0-copy, 1-aac, 2-no*/

    m_iVideoFps = 0; /* 编码选项 帧率>=0 0--自动*/
    m_iVideoWidth = 0; /* 编码选项 宽度>=0, 0--系统自定, 1-640, 2-704, 3-800 */
    m_iVideoHeight = 0; /* 编码选项 高度， 暂时不用 */
}

CPusherTykd2Rtmp::~CPusherTykd2Rtmp()
{
    ez_printf_debug("CPusherTykd2Rtmp Leave--------\n");
}


EZTHREAD_BOOL CPusherTykd2Rtmp::Start(CEZObject * pObj, PusherSignalProc_t pProc)
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
        std::string strUrlPrefixHLS = DEFAULT_HLS_URL_HEADER;
        std::string strUrlPrefixRTMP = DEFAULT_RTMP_URL;
        //std::string strUrlPrefixRTSP = DEFAULT_RTSP_URL;

        //m_strHostRelative = "";

        //
        m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

        m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense

        //#ifdef _USE_MODULES_EZCONFIG
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_PusherTykdServer_SingleServer)

        CConfigPullTykdMobileService __cfg;
        __cfg.update();
        strUrlPrefixRTMP = __cfg.getConfig().strDefaultUrlPrefixRTMP;
        //strUrlPrefixRTSP = __cfg.getConfig().strDefaultUrlPrefixRTSP;
        strUrlPrefixHLS = __cfg.getConfig().strDefaultUrlPrefixHLS;

        m_strTykdWsUrl = __cfg.getConfig().strTykdWsUrl;

        //m_strHostRelative = __cfg.getConfig().strFixTurnIP;
        // have been setting at CStreamPusherTykd::startPull
        //setHostRelative(__cfg.getConfig().strHostRelative);
        m_iUsePlayUrl_Local = __cfg.getConfig().iUsePlayUrl_Local;
        m_iUsePlayUrl_Direct = __cfg.getConfig().iUsePlayUrl_Direct;
        m_iUsePlayUrl_Relay = __cfg.getConfig().iUsePlayUrl_Relay;
#else //_USE_MODULES_EZCONFIG
        // use default value
#endif //_USE_MODULES_EZCONFIG

        // Create url of hls
        m_strUrlDst = strUrlPrefixHLS +"/" + getIdDst() + ".m3u8";

        m_strUrlHls = strUrlPrefixHLS +"/" + getIdDst() + ".m3u8";
        m_strUrlRtmp = strUrlPrefixRTMP +"/" + getIdDst();// + ".m3u8";
        //m_strUrlRtsp = strUrlPrefixRTSP +"/" + getIdDst()+ ".sdp";

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
EZTHREAD_BOOL CPusherTykd2Rtmp::Stop(CEZObject * pObj, PusherSignalProc_t pProc)
{
    ez_printf_debug("CPusherTykd2Rtmp::Stop\n");
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


void CPusherTykd2Rtmp::doPull()
{
    CPusher::doPull();

    //int link[2];
    pid_t pid;

    //#if defined (_FUNC_DO_RELAY)

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

    std::string strLocalPlayUrl;
    std::string strDirectPlayUrl;
    std::string strRelayPlayUrl;

    int iret = 0;

    setDevStatus( em_terminal_status_unknown );

    if (em_GetPlayUrlmethod_getPlayUrlHX == m_iGetPlayUrlmethod)
    {
#if 0
        ez_printf_debug("getMediaUrl_Use_getPlayUrlHX:%d\n", m_iGetPlayUrlmethod);

        iret = getMediaUrl_Use_getPlayUrlHX(strAccount
                                            , strPasswd
                                            , strDevId
                                            , atoi(getChanel().c_str())
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
                , atoi(getChanel().c_str())
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
        const int i_ch = atoi(getChanel().c_str());
        const int i_stream = atoi(getStream().c_str());
        t_hx_PlayUrl thxPlayUrl;
                
        int iret_get = uniFace_getPssStoragePlayUrlHXReq(p_endPoint, p_devid, i_ch, i_stream, &thxPlayUrl);
        if (0==iret_get)
        {
            iret = 0;
            strLocalPlayUrl = thxPlayUrl.buffPlayUrl_Local;
            strDirectPlayUrl  = thxPlayUrl.buffPlayUrl_Direct;
            strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Relay;
        }
        else
        {
            iret = -1;

            //目前全部设置为不在线
            //if (em_errorno_getMediaUrl_offline == iret_get)
            {
                setDevStatus(em_terminal_status_offline);
            }
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
                , atoi(getChanel().c_str())
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
        const int i_ch = atoi(getChanel().c_str());
        const int i_stream = atoi(getStream().c_str());
        t_hx_PlayUrl thxPlayUrl;
                
        int iret_get = uniFace_getPssServerPlayUrlHXReq(p_endPoint, p_devid, i_ch, i_stream, &thxPlayUrl);
        if (0==iret_get)
        {
            iret = 0;
            strLocalPlayUrl = thxPlayUrl.buffPlayUrl_Local;
            strDirectPlayUrl  = thxPlayUrl.buffPlayUrl_Direct;
            strRelayPlayUrl = thxPlayUrl.buffPlayUrl_Relay;
        }
        else
        {
            iret = -1;
            //目前全部设置为不在线
            //if (em_errorno_getMediaUrl_offline == iret_get)
            {
                setDevStatus(em_terminal_status_offline);
            }
        }
#endif
    }

    if (iret>=0)
    {
        setDevStatus( em_terminal_status_online );

        ez_printf_info("%15s:%s\n", "LocalPlayUrl", (strLocalPlayUrl.c_str()));
        ez_printf_info("%15s:%s\n", "DirectPlayUrl", (strDirectPlayUrl.c_str()));
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
                    //ez_printf_debug("%s:isPrivateIPAddress, set empty.\n", parsed_url->host);
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
                    //ez_printf_info("%s:isPrivateIPAddress, set empty\n", parsed_url->host);
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

            ez_printf_debug("fixRelayPlayUrl:%d\n", i_fixRelayPlayUrl);

            if (0 == i_fixRelayPlayUrl)
            {
                strRelayPlayUrl = __strUrlFix;
            }
        }//fix relay addr

    }
    else
    {
        ez_printf_error("getPssServerPlayUrlHXReq(%d):%s, %s, %s\n", iret, getTykdDevID().c_str(), getChanel().c_str(), getStream().c_str());

        if (-107 == iret) //"device not exist"
        {
            setDevStatus( em_terminal_status_unknown );
        }
        else if (-108 == iret) //"device offline"
        {
            setDevStatus(  em_terminal_status_offline );
        }
        else
        {
            setDevStatus(  em_terminal_status_unknown );
        }

        return;
    }

    size_t ii;
#ifdef _USE_LOCAL_URL

    char *argvLocalPlayUrl[64] = {NULL};

    if (strLocalPlayUrl.length() > 3/*rtsp, rtmp, rtp ...*/)
    {
        ARG_USED(argvLocalPlayUrl);

        std::vector<std::string> vCmdAndArgs_Local;

        iret = makePullCmdRtmp(strLocalPlayUrl, strHlsStreamID, vCmdAndArgs_Local, 0, m_iAudioEncode, 0/*fps=25*/, 0/*width=640*/);
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
        iret = makePullCmdRtmp(strDirectPlayUrl, strHlsStreamID, vCmdAndArgs_Direct, 0, m_iAudioEncode, 0/*fps=25*/, 0/*width=640*/);
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
#endif//_USE_LOCAL_URL

    char *argvRelayPlayUrl[64] =
        {
            NULL
        };
    ARG_USED(argvRelayPlayUrl);
    std::vector<std::string> vCmdAndArgs_Relay;
    iret = makePullCmdRtmp(strRelayPlayUrl, strHlsStreamID, vCmdAndArgs_Relay, 0, m_iAudioEncode, 0/*fps=25*/, 0/*width=640*/);
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
        }
    }
    else
    {
        ez_printf_error("strRelayPlayUrl makePullCmdRtmp failed.\n");
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
                ez_printf_info("CPusherTykd2Rtmp:[%.*s]\n", nbytes, foo);
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

    m_SigBuffer(MSG_Pusher_OK, this);
}

const std::string &CPusherTykd2Rtmp::getUrlHls()
{
    return m_strUrlHls;
}

const std::string &CPusherTykd2Rtmp::getUrlHlsFix ()
{
    //fixUrl(m_strUrlHlsFix, m_strUrlHls);
    CStreamUtility::fixUrl(m_strUrlHlsFix, m_strUrlHls, m_strShaChkSum);

    return m_strUrlHlsFix;
}

const std::string &CPusherTykd2Rtmp::getUrlRtmp ()
{
    return m_strUrlRtmp ;
}

const std::string &CPusherTykd2Rtmp::getUrlRtmpFix ()
{
    //fixUrl(m_strUrlRtmpFix, m_strUrlRtmp);
    CStreamUtility::fixUrl(m_strUrlRtmpFix, m_strUrlRtmp, m_strShaChkSum);

    return m_strUrlRtmpFix;
}
const std::string &CPusherTykd2Rtmp::getUrlRtsp ()
{
    return m_strUrlRtsp ;
}

const std::string &CPusherTykd2Rtmp::getUrlRtspFix ()
{
    //fixUrl(m_strUrlRtspFix, m_strUrlRtsp);
    CStreamUtility::fixUrl(m_strUrlRtspFix, m_strUrlRtsp, m_strShaChkSum);

    return m_strUrlRtspFix;
}

const std::string &CPusherTykd2Rtmp::getTitle()
{
    return m_strTitle;
}
const std::string &CPusherTykd2Rtmp::getMemo ()
{
    return m_strMemo;
}
const std::string &CPusherTykd2Rtmp::getAddr ()
{
    return m_strAddr;
}
const std::string &CPusherTykd2Rtmp::getEndDt()
{
    return m_strEndDt;
}

void CPusherTykd2Rtmp::onStoped()
{
    m_SigBuffer(MSG_Pusher_STOPED, this);
}
