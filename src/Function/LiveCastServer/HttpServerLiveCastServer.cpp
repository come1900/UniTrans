/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveCastServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveCastServer.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:34  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#include <stdio.h>
#ifdef HAVE_LIBUUID
#include <uuid/uuid.h>
#endif
#include "str_opr.h"
#include "ez_bit.h"
#include "sha1.h"
#include "StringOpr.h"
#include "ez_util_platform.h"
#include "str_opr.h"
#include <ez_url_parser.h>

#include "../../Solar.h"

#include "../Pandora/PandoraDef.h"

#include "../Pandora/StreamUtility.h" /*工具*/
#include "../Pandora/StreamContainer.h" /*工具*/

#include "HttpServerLiveCastServer.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveCast.h"
#include "../../Configs/ConfigLiveCastServer.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

#include "AgentLiveCastServer.h"
#include "HandlerLiveCastServer.h"

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



CHttpServerLiveCastServer::CHttpServerLiveCastServer(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;

    m_iIsAuthedHost = 0;

    m_pCfg = NULL;

    m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    m_strPssSid = "";
#if defined(_USE_MODULES_LocalDataBase)
    CHandlerLiveCastServer *_pHandler = ((CHandlerLiveCastServer *)(&(Handler())));
    m_dbaLiveRecord =  _pHandler->m_dbaLiveRecord;
#endif //_USE_MODULES_LocalDataBase
}

void CHttpServerLiveCastServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveCastServer::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerLiveCastServer::~CHttpServerLiveCastServer()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveCastServer::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerLiveCastServer::OnAccept()
{
    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());

// 必须开放config
#if defined(_FUNC_LiveCastServer)

    ConfigLiveCast *pCfgCast = g_AgentLiveCastServer.getConfigLiveCast();

    if (pCfgCast)
    {
        m_strPubPushRTMP = pCfgCast->strPubPushRTMP;
        m_strPubPushRTSP = pCfgCast->strPubPushRTSP;

        m_strUrlPrefixRTMP = pCfgCast->strDefaultUrlPrefixRTMP;
        m_strUrlPrefixRTSP = pCfgCast->strDefaultUrlPrefixRTSP;
        m_strUrlPrefixHLS = pCfgCast->strDefaultUrlPrefixHLS;
    }
#else //_USE_MODULES_EZCONFIG

#endif //_USE_MODULES_EZCONFIG

#ifdef _USE_MODULES_EZCONFIG

    static CConfigLiveCastServer __cfg;
    __cfg.update();
    m_pCfg = &__cfg;

    m_iTimeout = m_pCfg->getConfig().iTimeout;

    m_iCfgRandChannelID = __cfg.getConfig().iRandChannalID;
#else

    //SetTimeout(5);
    m_iTimeout = HTTPD_CONN_TIMEOUT;
    // use default value
    m_iCfgRandChannelID = 0
#endif//

    m_pContainerChannel = g_AgentLiveCastServer.GetHandler()->GetPandoraContainer();

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);

    m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

    m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense
}

void CHttpServerLiveCastServer::OnHeader(const std::string& key,const std::string& value)
{
    if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_AppKey))
    {
        m_strAppKey = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_AppKey, m_strAppKey.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_Nonce))
    {
        m_strNonce = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_Nonce, m_strNonce.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CurTime))
    {
        m_strCurTime = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CurTime, m_strCurTime.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CheckSum))
    {
        m_strCheckSum = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CheckSum, m_strCheckSum.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_PssSid))
    {
        m_strPssSid = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_PssSid, m_strPssSid.c_str());
    }
    else
    {
        HttpdSocket::OnHeader(key, value);
    }
}

void CHttpServerLiveCastServer::OnDisconnect()
{
    ez_printf_debug("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerLiveCastServer::Exec()
{
    ez_printf_debug("%s:%s:%d\n", __FUNCTION__, GetRemoteAddress().c_str(), GetRemotePort());
    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerLiveCastServer::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();

    //m_strBody = "";
    std::string().swap(m_strBody);

#if 1 // 使用app鉴权

    do
    {
        if (m_strPssSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
        {
            ez_printf_error("PssSid not found, req:%s\n", +GetUrl().c_str());

            RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);

            break;
        }

        if (m_pCfg
            && BitGet(m_pCfg->getConfig().iAuth, 0) == 1)
        {
            ez_printf_debug("-->> do secret key chk\n");

            if (m_strAppKey.length()<1
                || m_strNonce.length()<1
                || m_strCurTime.length()<1
                || m_strCheckSum.length()<1
               )
            {
                ez_printf_error("IllegalAccess:auth data not valid.\n");
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                break;
            }

            //encode("sha1", nonce + appSecret + curTime)
            // todo check appSecret from AppKey
            std::string appSecret = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
            m_strNonce+=appSecret;
            m_strNonce+=m_strCurTime;

            char dst[SHA1HashSize*2+1] = {0};
            const char *src = m_strNonce.c_str();
            ARG_USED(src);
            ARG_USED(dst);

            ez_printf_debug("ez_sha1_str:%s\n", src);
            int iret = ez_sha1_str(dst, sizeof(dst), src );

            if (iret<0)
            {
                ez_printf_error("IllegalAccess:auth data not valid.\n");
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                //RetGeneralMsg(103, "secret data not valid", GetUrl());
                break;
            }

            if (m_strCheckSum != dst)
            {
                ez_printf_warning("auth failed src:[%s], dst:[%s]\n", src, dst);

                //RetGeneralMsg(104, "auth failed", GetUrl());
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                break;
            }

            ez_printf_debug("appSecret Auth Src:%s\n", src);
            ez_printf_debug("dst:%s\n", dst);
        }

        if (m_pCfg
            && BitGet(m_pCfg->getConfig().iAuth, 1) == 1)
        {
            ez_printf_debug("-->> do host chk\n");

            // tykd platform
            if (GetRemoteAddress()=="202.102.108.6"
                || GetRemoteAddress()=="202.102.108.29"
                || GetRemoteAddress()=="202.102.108.29"
               )
            {
                m_iIsAuthedHost = 1;
                ez_printf_debug("by tykd, ok\n");
            }
            else if (GetRemoteAddress()=="58.213.224.244"
                     || GetRemoteAddress()=="202.102.101.133"
                     || GetRemoteAddress()=="202.102.101.134"
                     || GetRemoteAddress()=="127.0.0.1"
                    )
            {
                ez_printf_debug("by developer, ok\n");
            }
            else
            {
                ez_printf_warning("by unknow, close now!\n");
                RetGeneralMsg(103, "host not allow", GetUrl());
                break;
            }
        }

        //ok
        return;
    }
    while(0);

    Reset(); // prepare for next request
    SetCloseAndDelete();
#endif
}

void CHttpServerLiveCastServer::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
    AddResponseHeader("Server", std::string(def_Pandora_LiveCastServer_ServerName)+g_Solar.GetVersion());
#if 0
    // 下方代码当此服务器主动发消息，此时uri为空串，会出问题
    {
        size_t x = 0;
        for (size_t i = 0; i < GetUri().size(); i++)
            if (GetUri()[i] == '.')
                x = i;
        std::string ext = GetUri().substr(x + 1);
        if (ext == "gif" || ext == "jpg" || ext == "png")
            AddResponseHeader("Content-type", "image/" + ext);
        else
            AddResponseHeader("Content-type", "text/" + ext);
    }
#else
    // we are json
    // http://www.ietf.org/rfc/rfc4627.txt
    AddResponseHeader("Content-type", "application/json;charset=utf-8");
#endif

    AddResponseHeader("Connection", "Keep-Alive");

    if (ContentLength>0)
    {
        AddResponseHeader( "Content-length", Utility::l2string((long)ContentLength) );
    }

    if (matchRequest.size()>0)
    {
        AddResponseHeader( "Match-request", matchRequest);
    }

    if (m_strPssSid.size()>0)
    {
        AddResponseHeader( "PssSid", m_strPssSid);
    }

    SendResponse();
}

void CHttpServerLiveCastServer::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
{
    CAckMsgSimple __ackMsg;
    char strCode[16] = {'\0'};

    snprintf(strCode, sizeof(strCode)-1, "%d", retCode);

    __ackMsg.Result = "100";
    __ackMsg.Reason = "not support url:" + GetUrl() ;

    if (-1 == retCode)
    {
        //__ss << "[{\"result\":\"100\",\"reason\":\"not support url:" + GetUrl() + "\"}]";
    }
    else
    {
        __ackMsg.Result = strCode;
        __ackMsg.Reason = strMsg;
    }


    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    //SendMsg(__ss.str(), matchRequest);
}

void CHttpServerLiveCastServer::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}


/** Chunk of http body data recevied. */
void CHttpServerLiveCastServer::OnData(const char *p,size_t l)
{
    if (p==NULL || l<1)
    {
        return;
    }

    size_t ii=0;
    const char *pData = p;

    // FirstData, switch blank
    if (m_strBody.size() < 1)
    {
        for (ii=0; ii<l;ii++)
        {
            if (p[ii]=='\0')
            {
                break;
            }

            if (p[ii]!=' '
                && p[ii]!='\t'
                && p[ii]!='\r'
                && p[ii]!='\n' )
            {
                break;
            }

            pData++;
        }
    }

    if (m_strBody.size()+l > def_HTTPD_MSGG_BodyMaxSize)
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too long");
        std::string strBadboy = "Bad boy.";

        SendMsg(strBadboy);

        m_strBody = "";
        Reset(); // prepare for next request
        SetCloseAndDelete();

        return;
    }

    m_strBody += pData;

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerLiveCastServer::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#if 0//def _USE_MODULES_EZCONFIG
    CConfigLiveCastServer __cfg;
    __cfg.update();
#endif//

    CPssStack __CezlibsWebServiceIF;

    if (m_strBody.size()<strlen("[{}]"))
    {
        ez_printf_error("m_strBody(%s) not json.\n", m_strBody.c_str() );

        RetGeneralMsg(PssMsg_Value_Result_InvalidDataFormat
                      , PssMsg_Value_ResultMsg_InvalidDataFormat);

        return ;
    }
    else
    {
        // have []  of json string
#if 1
        if (m_strBody.at(0)=='[')
        {
            size_t ii;
            for(ii=m_strBody.length()-1;ii>0; ii--)
            {
                if (m_strBody.at(ii)==']')
                {
                    break;
                }
            }
            //if (ii !=m_strBody.length()-1)
            if (ii > 3)
            {
                //m_strBody.erase(ii+1,m_strBody.length()-1);
                m_strBody.erase(ii,m_strBody.length()-1);
            }

            for(ii=0;ii<m_strBody.length()-1; ii++)
            {
                if (m_strBody.at(ii)=='[')
                {
                    m_strBody.erase(0,ii+1);
                    break;
                }
            }
        }
#endif //

        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);
        if (bRet)
        {
            m_iMsgCountGet ++;
        }
        else
        {
            ez_printf_error("__CezlibsWebServiceIF.Parse failed.");

            RetGeneralMsg(PssMsg_Value_Result_InvalidDataFormat
                          , PssMsg_Value_ResultMsg_InvalidDataFormat);

            return;
        }
    }

    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Request URL:" << GetUrl());
    EZ_LOGS(
        ez_log("%s(%d) -- OnMsg:%s\n", __FILE__, __LINE__, GetUrl().c_str());
    );
    LOG4CPLUS_INFO(LOG_OF_SMS, "Html request:"
                   << "\n\t\t Method: "<<GetMethod()
                   << "\n\t\t URL: "<<GetUrl()
                   << "\n\t\t Http version: "<<GetHttpVersion()
                   << "\n\t\t Body: "<<m_strBody << "\n\t\t\t");

    EZ_LOGS(
        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, m_strBody.c_str());
        //ez_log("------------------------------------------\n");
        //ez_log("%s\n", m_strBody.c_str());
        //ez_log("\n------------------------------------------\n");
    );

    ez_printf_debug("GetUrl:%s\n", GetUrl().c_str());
    ez_printf_debug("Body:%s\n", m_strBody.c_str());

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    if (GetUrl()==def_Pandora_Match_pssChannelCreate)
    {
        onReq_pssChannelCreate_Single(m_strBody);
    }
    else if (GetUrl()==def_Pandora_Match_pssChannelCancel)
    {
        onReq_pssChannelCancel_Single(m_strBody);
    }
    else if (GetUrl()==def_Pandora_Match_pssChannelQuery)
    {
        onReq_pssChannelQuery_Single(m_strBody);
    }
    else if (GetUrl()==def_Pandora_Match_pssChannelList)
    {
        onReq_pssChannelList_Single(m_strBody);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    else
    {
        RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
    }

    Reset(); // prepare for next request
    SetCloseAndDelete();
    m_strBody = "";
}

void CHttpServerLiveCastServer::Update()
{
    unsigned long ttNow = ez_sec_get_tick_count();//time(NULL);

#if 1

    DBG(
        std::cout << ttNow-m_ttOndata << " tout:" << m_iTimeout << std::endl;
    );
    if (ttNow-m_ttOndata>m_iTimeout)
    {
        //__fline;
        //std::cout << "timeout :" <<  getMySessionId()  << std::endl << "SetCloseAndDelete ..."<< std::endl;
        ez_printf_info("idle Timeout(%d), conn:%d\n", GetSocket(), m_iTimeout)

        SetCloseAndDelete();
        return;
    }
#endif
}

size_t CHttpServerLiveCastServer::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerLiveCastServer::getMsgNumSent()
{
    return m_iMsgCountSend;
}


// http + json
int CHttpServerLiveCastServer::SendMsg(const std::string &strMsg, const std::string &matchRequest)
{
    DBG(
        std::cout << "strMsg.size() :" <<  strMsg.size()  << std::endl;
        std::cout << "strMsg :" <<  strMsg  << std::endl;
    );
    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    EZ_LOGS(
        ez_log("%s(%d) -- SendMsg:%s\n", __FILE__, __LINE__, GetUrl().c_str());
    );

    EZ_LOGS(
        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, strMsg.c_str());
    );

    Reset();
    m_strBody = "";
    m_iMsgCountSend++;

    // 短连接
    SetCloseAndDelete();

    return 0;
}

// curl http://localhost:18090/hello -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerLiveCastServer::Do_TestIF( CPssStack &JsonData)
{
#if 0
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_TestIF");
    __fline;
    std::cout << "Do_TestIF, your json:" << JsonData.ToString()<< std::endl;

    std::string strSendBuf;

    strSendBuf = "[{\"result\":\"0\",\"reason\":\"Do_TestIF, Your URL:" + GetUrl() + "\"}]";

    SendMsg(strSendBuf, GetUrl());
#else //

    CConfigGeneral __cfg;
    __cfg.update();
    //CAckMsgSimple __ackMsg;
    //CPssStack __stack;

    RetGeneralMsg(0, "Hello, Your URL:[" + GetUrl() + "], Me:" + __cfg.getConfig().strCode);
#endif//

    return 0;
}

#define def_UrlPrefixMinLen 15
int CHttpServerLiveCastServer::CreateStreamUrl( \
        const std::string &strName
        , std::string &strPushRtmp
        , std::string &strPushRtsp
        , std::string &strPlayHls
        , std::string &strPlayRtmp
        , std::string &strPlayRtsp)
{

    // create stream id
    std::string strShaChkSum = "043979497";// dbg dev huid
    std::string strPssStreamID = strName;
#ifdef _FUNC_gMiniLicense

    strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense

    strShaChkSum+=strName;

    //保证 每次不一样
    if (1 == m_iCfgRandChannelID)
    {
        strShaChkSum += Utility::l2string(g_Solar.GetCounter());
    }

    char dst[SHA1HashSize*2+1] = {0};
    int iret = ez_sha1_str(dst, sizeof(dst), strShaChkSum.c_str());
    if (iret>=0)
    {

        strPssStreamID = dst;
    }

    strPlayHls = "";
    strPlayRtmp = "";
    strPlayRtsp = "";
    strPushRtmp = "";
    strPushRtsp = "";

    if (m_strUrlPrefixHLS.length() > def_UrlPrefixMinLen)
        strPlayHls = m_strUrlPrefixHLS +"/" + strPssStreamID + ".m3u8";
    if (m_strUrlPrefixRTMP.length() > def_UrlPrefixMinLen)
        strPlayRtmp = m_strUrlPrefixRTMP +"/" + strPssStreamID;// + ".m3u8";
    if (m_strUrlPrefixRTSP.length() > def_UrlPrefixMinLen)
        strPlayRtsp = m_strUrlPrefixRTSP +"/" + strPssStreamID+ ".sdp";

    if (m_strPubPushRTMP.length() > def_UrlPrefixMinLen)
        strPushRtmp = m_strPubPushRTMP +"/" + strPssStreamID;// + ".m3u8";
    if (m_strPubPushRTSP.length() > def_UrlPrefixMinLen)
        strPushRtsp = m_strPubPushRTSP +"/" + strPssStreamID+ ".sdp";

    return 0;
}

int CHttpServerLiveCastServer::onReq_pssChannelCreate_Single(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPandoraMsg_pssChannelCreate __reqMsg;
    CPandoraMsg_pssChannelCreateAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!__stackReq.Parse(strReq))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }

        if (!__stackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
            //return -1;
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.Name)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        CStreamNode __node;
        int idx = m_pContainerChannel->getNodeStream(__reqMsg.Name, __node);

        // 已有，直接取用
        if (idx >= 0)
        {
            // do nothing

            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;

            // 刷新url
            if (1==m_iCfgRandChannelID)
            {
                CreateStreamUrl( \
                                 __reqMsg.Name
                                 , __node.PushUrlRtmp
                                 , __node.PushUrlRtsp
                                 , __node.UrlHls
                                 , __node.UrlRtmp
                                 , __node.UrlRtsp
                               );
            }
            __node.iAskedTimes++;
            m_pContainerChannel->modNodeStream(idx, __node);

#if defined(_USE_MODULES_LocalDataBase)
            m_dbaLiveRecord->modCast(\
                                          __reqMsg.Name \
                                          , time(NULL)
                                          , __node.iAskedTimes
                                         );
#endif //_USE_MODULES_LocalDataBase
        }
        else
        {
            __node.Initialize();
            __node.StreamID = __reqMsg.Name;

            CreateStreamUrl( \
                             __reqMsg.Name
                             , __node.PushUrlRtmp
                             , __node.PushUrlRtsp
                             , __node.UrlHls
                             , __node.UrlRtmp
                             , __node.UrlRtsp
                           );


            m_pContainerChannel->addNodeStream(__node);
        }

        CStreamUtility::fixUrl( __ackMsg.PushUrlRtsp, __node.PushUrlRtsp, m_strShaChkSum);
        CStreamUtility::fixUrl( __ackMsg.PushUrlRtmp, __node.PushUrlRtmp, m_strShaChkSum);
        CStreamUtility::fixUrl( __ackMsg.PlayUrlHls , __node.UrlHls     , m_strShaChkSum);
        CStreamUtility::fixUrl( __ackMsg.PlayUrlRtmp, __node.UrlRtmp    , m_strShaChkSum);
        CStreamUtility::fixUrl( __ackMsg.PlayUrlRtsp, __node.UrlRtsp    , m_strShaChkSum);

#if defined(_USE_MODULES_LocalDataBase)
            m_dbaLiveRecord->addCast(\
                                          __reqMsg.Name \
                                          , __ackMsg.PushUrlRtsp \
                                          , __ackMsg.PushUrlRtmp \
                                          , __ackMsg.PlayUrlHls \
                                          , __ackMsg.PlayUrlRtmp \
                                          , __ackMsg.PlayUrlRtsp \
                                         );
#endif //_USE_MODULES_LocalDataBase

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), def_Pandora_Match_pssChannelCreate);

    return 0;
}
int CHttpServerLiveCastServer::onReq_pssChannelCancel_Single(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPandoraMsg_pssChannelCancel __reqMsg;
    CPandoraMsg_pssChannelCancelAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!__stackReq.Parse(strReq))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }

        if (!__stackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 名字错误
        if (m_strPssSid != __reqMsg.Name)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        CStreamNode __node;
        int idx = m_pContainerChannel->getNodeStream(__reqMsg.Name, __node);

        // 已有删除
        if (idx >= 0)
        {
            ez_printf_info("delNodeStream:%d.\n", idx);

            m_pContainerChannel->delNodeStream(idx);

            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

#if defined(_USE_MODULES_LocalDataBase)
            m_dbaLiveRecord->delCast(\
                                          __reqMsg.Name \
                                         );
#endif //_USE_MODULES_LocalDataBase

        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_info("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), def_Pandora_Match_pssChannelCancel);

    return 0;
}

int CHttpServerLiveCastServer::onReq_pssChannelQuery_Single(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPandoraMsg_pssChannelQuery __reqMsg;
    CPandoraMsg_pssChannelQueryAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!__stackReq.Parse(strReq))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }

        if (!__stackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 名字错误
        if (m_strPssSid != __reqMsg.Name)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        CStreamNode __node;
        int idx = m_pContainerChannel->getNodeStream(__reqMsg.Name, __node);

        // Found
        if (idx >= 0)
        {
            ez_printf_info("delNodeStream:%d.\n", idx);

            __ackMsg.PlayUrlHlsStatus = "0";
            __ackMsg.PlayUrlRtspStatus = "0";

            __ackMsg.AcceptedTimes = __node.iAskedTimes;

            CStreamUtility::fixUrl( __ackMsg.PushUrlRtsp, __node.PushUrlRtsp, m_strShaChkSum);
            CStreamUtility::fixUrl( __ackMsg.PushUrlRtmp, __node.PushUrlRtmp, m_strShaChkSum);
            CStreamUtility::fixUrl( __ackMsg.PlayUrlHls , __node.UrlHls     , m_strShaChkSum);
            CStreamUtility::fixUrl( __ackMsg.PlayUrlRtmp, __node.UrlRtmp    , m_strShaChkSum);
            CStreamUtility::fixUrl( __ackMsg.PlayUrlRtsp, __node.UrlRtsp    , m_strShaChkSum);

            ez_parsed_url_t *parsed_url_hls = ez_parse_url(__node.UrlHls.c_str());
            ez_parsed_url_t *parsed_url_rtsp = ez_parse_url(__node.UrlRtsp.c_str());

            if (NULL != parsed_url_hls)
            {
                std::string strHlsFPN = std::string("/tmp/") + parsed_url_hls->path;
                ez_printf_info("strHlsFPN:%s.\n", strHlsFPN.c_str());
                if ( !access(strHlsFPN.c_str(), 0) )
                {
                    __ackMsg.PlayUrlHlsStatus = "1";
                }

                ez_parsed_url_free(parsed_url_hls);
            }

            if (NULL != parsed_url_rtsp)
            {
                std::string strRtspFPN = std::string("/usr/local/movies/") + parsed_url_rtsp->path;
                ez_printf_info("strRtspFPN:%s.\n", strRtspFPN.c_str());
                if ( !access(strRtspFPN.c_str(), 0) )
                {
                    __ackMsg.PlayUrlRtspStatus = "1";
                }

                ez_parsed_url_free(parsed_url_rtsp);
            }

            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        }
        // Not Found
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_info("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), def_Pandora_Match_pssChannelQuery);

    return 0;
}

// 多机版中需注意
int CHttpServerLiveCastServer::onReq_pssChannelList_Single(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPandoraMsg_pssChannelList __reqMsg;
    CPandoraMsg_pssChannelListAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!__stackReq.Parse(strReq))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }

        if (!__stackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }


        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        __ackMsg.TotalItems = m_pContainerChannel->numNode();
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_info("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), def_Pandora_Match_pssChannelQuery);

    return 0;
}

