/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PusherTykd2Rtsp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PusherTykd2Rtsp.cpp 5884 2016-06-14 11:08:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-06-14 11:08:04  Create
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
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif //_USE_MODULES_EZCONFIG
//#if defined (_FUNC_DO_RELAY)
//#include "gPushMan/tykdMobileServiceHX_Cli.h"
//#include "../Interface/tykdWebService/tykdMobileServiceClient.h"
#include "../Pandora/StreamUtility.h" /*工具*/

#include "dtPst.h" /* makePullCmdRtmp */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */
#include "uniFace.h"

//#endif //_FUNC_DO_RELAY
#include "PusherTykd2Rtsp.h"
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
#ifndef DEFAULT_RTSP_URL
#define DEFAULT_RTSP_URL "rtsp://" "61.160.149.160" ":18554"
#endif// DEFAULT_RTSP_URL

#ifndef TYKD_ACCOUNT
#define TYKD_ACCOUNT "hx"
#endif// TYKD_ACCOUNT

#ifndef TYKD_PASSWD
#define TYKD_PASSWD "tykd@189.cn"
#endif// TYKD_PASSWD

#define _USE_LOCAL_URL 1// 三个url轮番使用， 否则只用relay url
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CPusherTykd2Rtsp::CPusherTykd2Rtsp() //:CEZThread(PREFIX_CPusherTykd2Rtsp_THREAD_NAME, THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iSigUser = 0;

    m_strIdDst = "";//rtsp地址
    m_strTykdDevID = "";// 新的id

    // hls url
    m_strUrlDst = "";

    ez_printf_debug("CPusherTykd2Rtsp Enter--------\n");

    m_iChildPid = -1;
    m_uiRound =0;

    // 0-copy
    m_iVideoEncode = 0;
    m_iAudioEncode = 0;

    setDevStatus(em_terminal_status_offline);
}

CPusherTykd2Rtsp::~CPusherTykd2Rtsp()
{
    ez_printf_debug("CPusherTykd2Rtsp Leave--------\n");
}

EZTHREAD_BOOL CPusherTykd2Rtsp::Start(CEZObject * pObj, PusherSignalProc_t pProc)
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
        //std::string strUrlPrefixRTMP = DEFAULT_RTMP_URL;
        std::string strUrlPrefixRTSP = DEFAULT_RTSP_URL;
        //std::string strUrlPrefixHLS = DEFAULT_HLS_URL_HEADER;

        //m_strHostRelative = "";
        //#ifdef _USE_MODULES_EZCONFIG
#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_PusherTykdServer_SingleServer)

        CConfigPullTykdMobileService __cfg;
        __cfg.update();
        //strUrlPrefixRTMP = __cfg.getConfig().strDefaultUrlPrefixRTMP;
        strUrlPrefixRTSP = __cfg.getConfig().strDefaultUrlPrefixRTSP;
        //strUrlPrefixHLS = __cfg.getConfig().strDefaultUrlPrefixHLS;

        m_iVideoEncode = __cfg.getConfig().iDefaultVideoCodec;
        m_iAudioEncode = __cfg.getConfig().iDefaultAudioCodec;

        //m_strHostRelative = __cfg.getConfig().strFixTurnIP;
        // have been setting at CStreamPusherTykd::startPull
        //setHostRelative(__cfg.getConfig().strHostRelative);

        m_strTykdWsUrl = __cfg.getConfig().strTykdWsUrl;

        m_iUsePlayUrl_Local = __cfg.getConfig().iUsePlayUrl_Local;
        m_iUsePlayUrl_Direct = __cfg.getConfig().iUsePlayUrl_Direct;
        m_iUsePlayUrl_Relay = __cfg.getConfig().iUsePlayUrl_Relay;
#else //_USE_MODULES_EZCONFIG
        // use the default
#endif //_USE_MODULES_EZCONFIG

        // Create url of return
        m_strUrlDst = strUrlPrefixRTSP +"/" + getIdDst() + ".sdp";
        ez_printf_info("m_strUrlDst:%s\n", m_strUrlDst.c_str());

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
        __fline;
        printf("m_iSigUser:%d\n", m_iSigUser);
    );

    return bRet;
}
EZTHREAD_BOOL CPusherTykd2Rtsp::Stop(CEZObject * pObj, PusherSignalProc_t pProc)
{
    ez_printf_info("CPusherTykd2Rtsp::Stop\n");
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

            ez_printf_info("DevID:%s, StreamID:%s, PullerPid:%d, killReturn:%d\n"
                           , getTykdDevID().c_str()
                           , getIdDst().c_str()
                           , m_iChildPid
                           , killReturn);

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

void CPusherTykd2Rtsp::doPull()
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

    setDevStatus(  em_terminal_status_unknown );
#if 0
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

            setDevStatus(em_terminal_status_online);
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
    if (iret>=0)
    {
        setDevStatus(  em_terminal_status_online );

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
                    ez_printf_debug("%s:isPrivateIPAddress, set empty\n", parsed_url->host);
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
        ez_printf_error("getPssServerPlayUrlHXReq(%d):%s, %s, %s\n", iret, getTykdDevID().c_str(), getChanel().c_str(), getStream().c_str());

        if (-107 == iret) //"device not exist"
        {
            setDevStatus(  em_terminal_status_unknown );
        }
        else if (-108 == iret) //"device not exist"
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

    char *argvLocalPlayUrl[def_Max_DoPush_Arg_Num] = {NULL};
    ARG_USED(argvLocalPlayUrl);
    if (strLocalPlayUrl.length() > 3/*rtsp, rtmp, rtp ...*/)
    {

        std::vector<std::string> vCmdAndArgs_Local;

        iret = makePullCmdRtsp(strLocalPlayUrl, strHlsStreamID, vCmdAndArgs_Local, m_iVideoEncode, m_iAudioEncode);
        if (iret >= 0)
        {
            DBG(
                printf("strLocalPlayUrl\n");
            );
            // 防止异常
            if (vCmdAndArgs_Local.size() >= def_Max_DoPush_Arg_Num)
            {
                argvLocalPlayUrl[0] = NULL;

                ez_printf_emerg("%lld >= %d, check code!\n", vCmdAndArgs_Local.size(), def_Max_DoPush_Arg_Num);
            }
            else
            {
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
            }
        }
        else
        {
                ez_printf_error("strLocalPlayUrl makePullCmdRtsp failed.\n");
        }
    }

    char *argvDirectPlayUrl[def_Max_DoPush_Arg_Num] = {NULL};
    ARG_USED(argvDirectPlayUrl);
    if (strLocalPlayUrl.length() > 3/*rtsp, rtmp, rtp ...*/)
    {
        std::vector<std::string> vCmdAndArgs_Direct;
        iret = makePullCmdRtsp(strDirectPlayUrl, strHlsStreamID, vCmdAndArgs_Direct, m_iVideoEncode, m_iAudioEncode);
        if (iret >= 0)
        {
            DBG(
                printf("strDirectPlayUrl\n");
            );
            // 防止异常
            if (vCmdAndArgs_Direct.size() >= def_Max_DoPush_Arg_Num)
            {
                argvDirectPlayUrl[0] = NULL;

                ez_printf_emerg("%lld >= %d, check code!\n", vCmdAndArgs_Direct.size(), def_Max_DoPush_Arg_Num);
            }
            else
            {
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
            }//(vCmdAndArgs_Local.size() >= def_Max_DoPush_Arg_Num)
        }
        else
        {
                ez_printf_error("strDirectPlayUrl makePullCmdRtsp failed.\n");
        }
    }
#endif//_USE_LOCAL_URL

    char *argvRelayPlayUrl[def_Max_DoPush_Arg_Num] =
        {
            NULL
        };
    ARG_USED(argvRelayPlayUrl);
    if (strRelayPlayUrl.length() > 3/*rtsp, rtmp, rtp ...*/)
    {
        std::vector<std::string> vCmdAndArgs_Relay;

        iret = makePullCmdRtsp(strRelayPlayUrl, strHlsStreamID, vCmdAndArgs_Relay, m_iVideoEncode, m_iAudioEncode);
        if (iret >= 0)
        {
            DBG(
                printf("strRelayPlayUrl\n");
            );
            // 防止异常
            if (vCmdAndArgs_Relay.size() >= def_Max_DoPush_Arg_Num)
            {
                argvRelayPlayUrl[0] = NULL;

                ez_printf_emerg("%lld >= %d, check code!\n", vCmdAndArgs_Relay.size(), def_Max_DoPush_Arg_Num);
            }
            else
            {
                for ( ii=0; ii<vCmdAndArgs_Relay.size(); ii++)
                {
                    argvRelayPlayUrl[ii] = (char *)vCmdAndArgs_Relay[ii].c_str();
                    DBG(
                        printf("%s ", argvRelayPlayUrl[ii]);
                    );
                }
                argvRelayPlayUrl[ii] = NULL;
                DBG(
                    printf("\n");
                );
DBG(
                std::string __strPullCmd_Relay;
                int iret_CmdArray2HumanString = CmdArray2HumanString(vCmdAndArgs_Relay, __strPullCmd_Relay);
                if (iret_CmdArray2HumanString >= 0)
                {
                    ez_printf_debug("__strPullCmd_Relay:%s\n", __strPullCmd_Relay.c_str());
                }
);
            } //if (vCmdAndArgs_Relay.size() >= def_Max_DoPush_Arg_Num)
        }
        else
        {
                ez_printf_error("strRelayPlayUrl makePullCmdRtsp failed.\n");
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

    if(pid == 0) // 子进程
    {
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

        die("execl");
    }
    else //(pid > 0) 父进程
    {
        m_iChildPid = pid;
        signal(SIGCHLD, SIG_IGN);
        ez_printf_info("Begin, Pid(%d), Id:%s, IdDst:%s\n", m_iChildPid, getDevChnStreamID().c_str(), getIdDst().c_str());

        while (m_bLoop)
        {
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
    }

    m_SigBuffer(MSG_Pusher_Tykd2Rtsp_OK, this);
}

void CPusherTykd2Rtsp::onStoped()
{
    m_SigBuffer(MSG_Pusher_Tykd2Rtsp_STOPED, this);
}
