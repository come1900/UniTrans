/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Pusher.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Pusher.cpp 5884 2016-05-25 09:08:28Z WuJunjie $
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
#include <sys/wait.h> /*waitpid*/
#include <arpa/inet.h>

#include <sha1.h>
#include <str_opr.h>
#include <ez_url_parser.h>
#include <StringOpr.h>/*ezConvertToString*/
#include <ezParse.h>
#include <net_work_interface.h>

#include "../../Logs.h"

//#ifdef _FUNC_gMiniLicense
#include "../../Solar.h"
//#endif // _FUNC_gMiniLicense

//#include "gPushMan/TypedefPssOfTykd.h"
//#include "../Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
#include "StreamPusherTykd.h"
#include "Pusher.h"

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

#define def_MIN_PLAYURL_LEN (sizeof("rtsp://0.0.0.0"))

#define def_sleep_seconds_when_devoffline 60
#define def_sleep_seconds_when_devunknow 3*60

// 分隔符
#define def_SplitorRelative "&&"
#define def_SplitorHost    "||"
//天津服务器
//219.150.56.138||10.10.101.6&&219.150.56.139||10.10.101.7&&219.150.56.140||10.10.101.8&&219.150.56.141||10.10.101.9
//noc流媒体服务器
//61.160.149.159||192.168.64.193&&61.160.149.160||192.168.64.194&&61.160.149.164||192.168.64.195&&61.160.149.165||192.168.64.196

std::string CPusher::m_strHostRelative = "";
std::string CPusher::m_strFixedhost = "";
std::map<std::string, std::string> *CPusher::m_pMapHostRelative = NULL;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CPusher::CPusher() :CEZThread(PREFIX_CPusher_THREAD_NAME, THREAD_PRIORITY_JWORKER), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iSigUser = 0;

    m_iChildPid = 0; //子进程pid
    m_uiRound = 0;   //循环次数
    m_iChanel = "0"; // 通道号
    m_iStream = "2"; // 码流 1主码流  2子码流

    memset(m_ullRoundTimePoints, sizeof(m_ullRoundTimePoints), 0);
    //m_emDevStatus = em_terminal_status_unknown;
    setDevStatus(em_terminal_status_offline);

    m_iCountReq = 0;
    m_ulReqPoint = 0;
    m_ulStartPoint = 0;
    m_ulStartPushPoint = 0;

    m_iAutoLiveSecond = ValidPeriod_DEF_Auto_LiveSecond;

    if (NULL == m_pMapHostRelative)
    {
        m_pMapHostRelative = new std::map<std::string, std::string>;
        assert(m_pMapHostRelative);
    }

    m_iVideoEncode = 0;
    // 默认重新编码， 有些相机拉流失败
    // 原因是摄像机实际没有音频，但是SDP中有音频描述(8128B9AE295)
    m_iAudioEncode = EnAudioCodec_Type_NoAudio; /*0-audo(copy), 1-aac, 2-noaudio*/

    m_iVideoFps = 0; /* 编码选项 帧率>=0 0--自动*/
    m_iVideoWidth = 0; /* 编码选项 宽度>=0, 0--系统自定, 1-640, 2-704, 3-800 */ //目前对应协议中的VideoResize
    m_iVideoHeight = 0; /* 编码选项 高度， 暂时不用 同width等比例变换*/

    // 功能配置
    // 播放地址使用
    m_iUsePlayUrl_Local = 1;
    m_iUsePlayUrl_Direct = 0;
    m_iUsePlayUrl_Relay = 1;

#if defined (_FUNC_LiveStreamming_BackMediaSource)

    m_strPlayUrl_Src = "";
#endif // _FUNC_LiveStreamming_BackMediaSource

    m_iPushTimeout_SECOND_Max = PushTimeout_SECOND_Max;

    m_strEndPoint4VideoRs = "";

    m_pPusherManager = NULL;
}

CPusher::~CPusher()
{
    if (m_iChildPid>0)
    {
        int  killReturn = kill( m_iChildPid, SIGKILL);  // Kill child process group
        ARG_USED(killReturn);
    }
}

EZTHREAD_BOOL CPusher::Start(CEZObject * pObj, PusherSignalProc_t pProc)
{
    //printf("CPusher::Start\n");
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CStreamPusherTykd *pPusherManager = dynamic_cast<CStreamPusherTykd *>(pObj);

    if (pPusherManager)
        m_pPusherManager = pPusherManager;

    return bRet;
}

EZTHREAD_BOOL CPusher::Stop(CEZObject * pObj, PusherSignalProc_t pProc)
{
    //printf("CPusher::Stop\n");
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    return bRet;
}

void CPusher::ThreadProc()
{
    while (m_bLoop)
    {
        sleep(1);
        doPull();

        int iPushInterval = 3;// 重试间隔， 0 默认， >10 用这个参数， 1-10 备用
#if 1

        if (m_pPusherManager != NULL
            && m_pPusherManager->getCfg_PushInterval() > 10)
        {
            iPushInterval = m_pPusherManager->getCfg_PushInterval();

            ez_printf_debug("::%s use Cfg_PushInterval %d sec\n"
                            , m_strTykdDevID.length()>0?m_strTykdDevID.c_str():"--"
                            , iPushInterval);
        }
        else if (getDevStatus()==em_terminal_status_offline)
        {
            ez_printf_debug("::%s offline sleep %d sec\n"
                            , m_strTykdDevID.length()>0?m_strTykdDevID.c_str():"--"
                            , def_sleep_seconds_when_devoffline);

            iPushInterval = def_sleep_seconds_when_devoffline;
        }
        else if (getDevStatus()==em_terminal_status_unknown)
        {
            ez_printf_debug("::%s not exists sleep %d sec\n"
                            , m_strTykdDevID.length()>0?m_strTykdDevID.c_str():"--"
                            , def_sleep_seconds_when_devunknow);

            iPushInterval = def_sleep_seconds_when_devunknow;
        }
        else
#endif//
        {
            iPushInterval = (m_uiRound%9)*2+2;

            ez_printf_debug("::%s use else %d sec\n"
                            , m_strTykdDevID.length()>0?m_strTykdDevID.c_str():"--"
                            , iPushInterval);
        }// 在线否

        sleep(iPushInterval);
    }

    onStoped();
}

void CPusher::doPull()
{
    // 记录运行时间点
    m_uiRound++;
    m_ullRoundTimePoints[m_uiRound%REC_ROUND_TIME_NUM] = process_counter_sec();
}

void CPusher::onStoped()
{
    m_SigBuffer(MSG_Pusher_STOPED, this);
}

int CPusher::setIdDst(const std::string &strHlsStreamID, const int iEncryptID)
{
    if (EncryptID_Encrypt==iEncryptID)
    {
        std::string strShaChkSum = ezConvertToString(g_Solar.GetCounter());
        strShaChkSum+= strHlsStreamID;

        char dst[SHA1HashSize*2+1] = {0};
        int iret = ez_sha1_str(dst, sizeof(dst), strShaChkSum.c_str());

        if (iret>=0)
        {
            m_strIdDst = dst;
            m_strIdDstOri = strShaChkSum;
        }
        else
        {
            m_strIdDst = strHlsStreamID;
            m_strIdDstOri = "";
        }
    }
    else
    {
        m_strIdDst = strHlsStreamID;
    }

    return 0;
}

const std::string & CPusher::getIdDst()
{
    if (m_strIdDst.length() > 2)
    {
        return m_strIdDst ;
    }
    else
    {
        return getTykdDevID ();
    }
}
int CPusher::setAutoLiveSecond(int iAutoLiveSecond)
{
    //chk var
    if (iAutoLiveSecond < 0)
    {
        return -1;
    }
    //chk var

    // 记录开始时间
    m_iAutoLiveSecond  = iAutoLiveSecond;

    return 0;
}
int CPusher::getAutoLiveSecond()
{
    return m_iAutoLiveSecond;
}

int CPusher::setValidPeriod(int iValidPeriod)
{
    //chk var
    if (iValidPeriod < 0)
    {
        return -1;
    }
    //chk var

    // 记录开始时间
    m_iValidPeriod  = iValidPeriod;

    return 0;
}
int CPusher::setValidPeriod(const std::string &strValidPeriod)
{
    int __iValidPeriod = 0;
    ezConvertFromString(__iValidPeriod, strValidPeriod);

    return setValidPeriod(__iValidPeriod);
}
int CPusher::getValidPeriod()
{
    return m_iValidPeriod;
}

// 开始
int CPusher::onStart()
{
    // 记录开始时间
    m_ulStartPoint = g_Solar.GetRunPeriod();

    m_ulReqPoint = m_ulStartPoint;

    return 0;
}
// 请求调用
// 返回请求次数
int CPusher::onReq()
{
    m_ulReqPoint = g_Solar.GetRunPeriod();
    m_iCountReq++;

    return m_iCountReq;
}

int CPusher::PushStart()
{
    m_ulStartPushPoint = g_Solar.GetRunPeriod();

    return 0;
}

int CPusher::PushEnd()
{
    m_ulStartPushPoint = 0;

#if defined(_VEND_LiveStreammingNewMedia_ZSDC)
    //m_strPlayUrl_Src.clear();
#else//_VEND_LiveStreammingNewMedia_ZSDC

    m_strPlayUrl_Src.clear();
#endif //_VEND_LiveStreammingNewMedia_ZSDC

    return 0;
}

unsigned long CPusher::getPushLast()
{
    if (0 == m_ulStartPushPoint)
        return 0;

    return g_Solar.GetRunPeriod()-m_ulStartPushPoint;
}
#if 0
Pusher_PushStatus_T CPusher::PushStatus()
{
    unsigned long __ulNow = g_Solar.GetRunPeriod();
    //ez_printf_info("now:%lld, Start:%lld, StartPush:%lld\n", __ulNow, m_ulStartPoint, m_ulStartPushPoint);

    if (0 == m_ulStartPushPoint)
    {
        //ez_printf_info("now:%lld, Start:%lld\n", __ulNow, m_ulStartPoint);
        return Pusher_PushStatus_T_Ini;
    }
    //else if (__ulNow>=m_ulStartPoint+PushTimeout_SECOND_Max)
    else if (__ulNow>=m_ulStartPushPoint+m_iPushTimeout_SECOND_Max)
    {
        //ez_printf_info("now:%lld, Start:%lld\n", __ulNow, m_ulStartPoint);
        return Pusher_PushStatus_T_Pushing;
    }
    else
    {
        //ez_printf_info("now:%lld, Start:%lld\n", __ulNow, m_ulStartPoint);
        return Pusher_PushStatus_T_Starting;
    }

    // others
    //return Pusher_PushStatus_T_Starting;
}
#endif
// 检查生存期
// ret:
//        0 - 正常
//        1 - 超期
//        -1 - 未开始

int CPusher::chkLifeCycle()
{
    int iret = Pusher_LifeCycle_T_OK;

    unsigned long ulChkPoint = g_Solar.GetRunPeriod();

    do
    {
        if (ValidPeriod_DEF_LongLife == m_iValidPeriod)
        {
            iret = Pusher_LifeCycle_T_OK;
            break;
        }
        else if (ValidPeriod_DEF_Auto == m_iValidPeriod)
        {
            if (m_iAutoLiveSecond >= ValidPeriod_SECOND_MIN
                && ulChkPoint > (m_ulReqPoint+m_iAutoLiveSecond))
            {
                iret = Pusher_LifeCycle_T_TimeOut;
                break;
            }

            iret = Pusher_LifeCycle_T_OK;
            break;
        }
        // 使用传入的时间
        else if (m_iValidPeriod >= ValidPeriod_SECOND_MIN)
        {
            if (ulChkPoint > (m_ulReqPoint+m_iValidPeriod))
            {
                iret = Pusher_LifeCycle_T_TimeOut;
                break;
            }

            iret = Pusher_LifeCycle_T_OK;
            break;
        }
        else
        {
            iret = Pusher_LifeCycle_T_OK;
            break;
        }
    }
    while (0);

    return iret;
}

unsigned long CPusher::secOfStarted()//已经启动了?秒
{
    return g_Solar.GetRunPeriod() - m_ulStartPoint;
}

unsigned long CPusher::secOfStart()//启动的时间点
{
    return m_ulStartPoint;
}

//应该一起设置， 故而统一接口
int CPusher::setDevChnStreamID(const std::string &strTykdDevID, const std::string strChannel, const std::string strStream)
{
    m_strTykdDevID = strTykdDevID;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int iChanel = atoi(strChannel.c_str());

    if (iChanel<0)
    {
        return -1;
    }

    m_iChanel = strChannel;
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int iStream = atoi(strStream.c_str());

    if (iStream<1 || iStream>4)
    {
        return -2;
    }

    m_iStream = strStream;

    m_strDevChnStreamID = m_strTykdDevID+"_"+m_iChanel+"_"+m_iStream;

    return 0;
}

const std::string & CPusher::getTykdDevID ()
{
    return m_strTykdDevID ;
}

const std::string &CPusher::getChanel()
{
    return m_iChanel;
}

const std::string &CPusher::getStream()
{
    return m_iStream;
}

const std::string &CPusher::getDevChnStreamID()
{
    return m_strDevChnStreamID;
}

int CPusher::setTykdAccount (const std::string &strTykdAccount)
{
    m_strTykdAccount = strTykdAccount;

    return 0;
}

const std::string & CPusher::getTykdAccount ()
{
    return m_strTykdAccount ;
}

int CPusher::setTykdPasswd (const std::string &strTykdPasswd)
{
    m_strTykdPasswd = strTykdPasswd;

    return 0;
}

const std::string & CPusher::getTykdPasswd ()
{
    return m_strTykdPasswd;
}

const std::string & CPusher::getUrlDst ()
{
    return m_strUrlDst ;
}

const std::string & CPusher::getUrlDstFix ()
{
    std::string strShaChkSum = "043979497";// dbg huid

#ifdef _FUNC_gMiniLicense

    strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense

    strShaChkSum+=m_strUrlDst;

    char dst[SHA1HashSize*2+1] = {0};
    int iret = ez_sha1_str(dst, sizeof(dst), strShaChkSum.c_str());
    if (iret>=0)
    {
        m_strUrlDstFix = m_strUrlDst+"?";
        m_strUrlDstFix += URL_SIGN_PARA_MARK;
        m_strUrlDstFix += dst;

        return m_strUrlDstFix;
    }

    return m_strUrlDst ;
}

int CPusher::getChildPid() //子进程pid
{
    return m_iChildPid;
}

unsigned int CPusher::getRound() //循环次数
{
    return m_uiRound;
}
EM_TERMINAL_STATUS_T CPusher::getDevStatus()
{
    return m_emDevStatus;
}

int CPusher::setDevStatus(EM_TERMINAL_STATUS_T st)
{
    m_emDevStatus = st;
    return 0;
}

unsigned long long CPusher::getRoundTimePoints()//运行时间点
{
    return m_ullRoundTimePoints[m_uiRound%REC_ROUND_TIME_NUM] ;
}

void CPusher::setHostRelative(const std::string &strHostRelative)
{
    //ez_printf_debug("Relative:%s\n", strHostRelative.c_str());

    // 只更新一次
    if (m_strHostRelative.length() > IP4_ADD_STR_LEN_MIN
        || m_strFixedhost.length() > IP4_ADD_STR_LEN_MIN)
    {
        ez_printf_info("HostRelative have been setted.\n");

        return;
    }

    int iret_validHostRelative = ip4_valid(strHostRelative.c_str());
    // 只有一个ip地址，方式1
    if (0 == iret_validHostRelative)
    {
        ez_printf_info("setHostRelative with a ip.\n");

        m_strFixedhost = strHostRelative;

        return;
    }

    std::vector<std::string> vRelative;
    Split(strHostRelative, def_SplitorRelative, vRelative);
#if 0
    std::vector<std::string>::iterator iter;
    for (iter=vRelative.begin(); iter!=vRelative.end(); iter++)
    {

        std:: string Insta;
        Insta=*iter;
        ez_printf_debug("Insta:%s\n", Insta.c_str());
    }
#endif

    // 解析ok
    if (vRelative.size() < 1)
    {
        ez_printf_error("failed paRelative:%s\n", strHostRelative.c_str());
        return;
    }//if (paRelative.Size()>0)

    for (std::string::size_type ii=0; ii<vRelative.size(); ii++)
    {
        std::vector<std::string> vHost;
        Split(vRelative[ii], def_SplitorHost, vHost);

        if (2 != vHost.size()) // ok
        {
            ez_printf_error("failed paHost:%s\n", vRelative[ii].c_str());
            continue;
        }

        int iret_validHost = ip4_valid(vHost[0].c_str());
        if (iret_validHost<0)
        {
            ez_printf_error("iret_validHost(%d):%s\n", iret_validHost, vHost[0].c_str());
            continue;
        }

        int iret_validRela = ip4_valid(vHost[1].c_str());
        if (iret_validRela<0)
        {
            ez_printf_error("iret_validRela(%d):%s\n", iret_validRela, vHost[1].c_str());
            continue;
        }

        if (m_pMapHostRelative)
        {
            (*m_pMapHostRelative)[vHost[0]] = vHost[1];

            //ez_printf_debug("%2d ::%-16s -->> %-16s\n", ii, vHost[0].c_str(), vHost[1].c_str());
        }
    }// for

    // 设置了
    if (m_pMapHostRelative
        && !m_pMapHostRelative->empty())
    {
        m_strHostRelative = strHostRelative;

#if 0
        std::map<std::string, std::string>::iterator it;
        for (it=m_pMapHostRelative->begin(); it!=m_pMapHostRelative->end(); ++it)
        {
            //std::cout << it->first << " => " << it->second << '\n';
            ez_printf_debug("%s-->>%s\n", it->first.c_str(), it->second.c_str());
        }
#endif

    }

}

int CPusher::fixRelayPlayUrl(std::string &strDst, const std::string strUrl)
{
    // 没有配置
    if (m_strFixedhost.length()<IP4_ADD_STR_LEN_MIN
        &&  (NULL==m_pMapHostRelative ||m_pMapHostRelative->empty()) )
    {
        //ez_printf_debug("no need fixRelayPlayUrl\n");
        return 1;
    }

    int iret = 0;
    ez_parsed_url_t *parsed_url = NULL;

    do
    {
        parsed_url = ez_parse_url(strUrl.c_str());
        std::string strFixedhost;

        if (NULL == parsed_url)
        {
            ez_printf_error("failed ez_parse_url:%s\n", strUrl.c_str());
            iret = -1;
            break;
        }

        // 用第一种方式修正relay地址
        if (m_strFixedhost.length()>=IP4_ADD_STR_LEN_MIN)// 0.0.0.0
        {
            strFixedhost = m_strFixedhost;
        }
        // 第二种配置方式
        else if (m_pMapHostRelative
                 && !m_pMapHostRelative->empty())
        {
            std::map<std::string, std::string>::iterator it = m_pMapHostRelative->find(parsed_url->host);

            if (it != m_pMapHostRelative->end())// find
            {
                ez_printf_debug("HostRelative:%s-->>%s\n", it->first.c_str(), it->second.c_str());
                strFixedhost = it->second;
            }
            else // 没有找到
            {
                iret = 2;
                break;
            }

        }
        else // 这种情况在入口处已经检查
        {
            ez_printf_error("T\n");

            iret = 1;
            break;
        }

        if (strFixedhost.length() < IP4_ADD_STR_LEN_MIN)
        {
            ez_printf_error("invalid host:%d\n", strFixedhost.length());
            iret = 3;
            break;
        }

        if (NULL == parsed_url->scheme)
        {
            ez_printf_error("scheme NULL\n");
            iret = 4;
            break;
        }

        std::string __strUrlFix = std::string(parsed_url->scheme) + std::string("://") ;

        if (parsed_url->username!=NULL  && parsed_url->password!=NULL )
            if (strlen(parsed_url->username)>0
                &&strlen(parsed_url->password)>0)
            {
                __strUrlFix += (std::string(parsed_url->username)  + std::string(":") + std::string(parsed_url->password)   + std::string("@"));
            }

        __strUrlFix += std::string(strFixedhost);
        //ez_printf_debug("__strUrlFix:%s\n", __strUrlFix.c_str());

        if (parsed_url->port != NULL)
            if (strlen(parsed_url->port)>0)
            {
                __strUrlFix += ( std::string(":") + std::string(parsed_url->port) );
            }

        if (parsed_url->path != NULL)
            if (strlen(parsed_url->path)>0)
            {
                __strUrlFix += ( std::string("/") + std::string(parsed_url->path) );
            }

        if (parsed_url->query != NULL)
            if (strlen(parsed_url->query)>0)
            {
                __strUrlFix += ( std::string("?") + std::string(parsed_url->query) );
            }

        strDst = __strUrlFix;

        ez_printf_debug("%s:%s\n", "UrlRelative", (__strUrlFix.c_str()));
    }
    while (0);

    if (parsed_url)
    {
        ez_parsed_url_free(parsed_url);
    }

    return iret;
}

void CPusher::setTykdWsUrl(const std::string &strTykdWsUrl)
{
    m_strTykdWsUrl = strTykdWsUrl;

    //return 0;
}

#if 0
int CPusher::chkPlayUrlHX(std::string &strLocalPlayUrl, std::string &strDirectPlayUrl)
{
    int iret = 0;

    ez_parsed_url_t *parsed_urlLocal = NULL;
    ez_parsed_url_t *parsed_urlDirect = NULL;

    do
    {
        if (strDirectPlayUrl.length() > def_MIN_PLAYURL_LEN)
            parsed_urlDirect = ez_parse_url(strDirectPlayUrl.c_str());

        if (NULL == parsed_urlDirect)
        {
            iret = -11;
            strDirectPlayUrl = "";
        }
        else
        {
            int result = 0;
            struct in_addr in_addr_ip;
            memset(&in_addr_ip,0,sizeof(struct in_addr));

            if (NULL == parsed_urlDirect->host)
            {
                iret = -10;
                break;
            }

            result = inet_pton(AF_INET, parsed_urlDirect->host, &(/*sa.sin_addr*/in_addr_ip));

            if (result == 0 || in_addr_ip.s_addr==0/*0.0.0.0*/)
            {
                iret = -10;
                strDirectPlayUrl = "";
            }

            if (NULL == parsed_urlDirect->port)
            {
                iret = -10;
                break;
            }

            if (atoi(parsed_urlDirect->port)==0)
            {
                iret = -11;
                strDirectPlayUrl = "";
            }
        }

        if (strLocalPlayUrl.length() > def_MIN_PLAYURL_LEN)
            parsed_urlLocal = ez_parse_url(strLocalPlayUrl.c_str());

        if (NULL == parsed_urlLocal)
        {
            iret = -11;
            strLocalPlayUrl = "";
        }
        else
        {
            struct in_addr in_addr_ip;

            if (NULL == parsed_urlLocal->host)
            {
                iret = -10;
                break;
            }
            int result = inet_pton(AF_INET, parsed_urlLocal->host, &(/*sa.sin_addr*/in_addr_ip));

            if (result == 0 || in_addr_ip.s_addr==0/*0.0.0.0*/)
            {
                iret = -10;
                strLocalPlayUrl = "";
            }

            if (atoi(parsed_urlLocal->port)==0)
            {
                iret = -11;
                strLocalPlayUrl = "";
            }

            //
            if (NULL != parsed_urlDirect
                && 0!=strcmp(parsed_urlDirect->host, parsed_urlLocal->host))
            {
                iret = -11;
                strDirectPlayUrl = "";
            }

        }
    }
    while (0);

    if (parsed_urlDirect)
        ez_parsed_url_free(parsed_urlDirect);

    if (parsed_urlLocal)
        ez_parsed_url_free(parsed_urlLocal);

    return iret;
}
#endif
