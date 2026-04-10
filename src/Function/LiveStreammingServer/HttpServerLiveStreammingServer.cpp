/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveStreammingServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveStreammingServer.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#include "str_opr.h"
#include "ez_bit.h"
#include "md5.h"
#include "sha1.h"
#include "StringOpr.h"
#include "ez_util_platform.h"
#include <ez_url_parser.h>
#include <ez_socket.h> /*ezDef_StreamType_Str_megaeye*/
#include "ez_appkey.h" // def_ez_APPKEY_BUFF_LEN ... // used by onReq_ManAppKeyCreate

#include "../../Solar.h"
#include "../../Logs.h"
#if defined(_FUNC_TykdMobileService)
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif

#if defined(_FUNC_gMiniLicense)
#include "../../Maintain/gMiniLicense.h"
#endif//_FUNC_gMiniLicense

#ifdef _FUNC_LiveStatusServer
#include "../LiveStatusServer/AgentLiveStatusServer.h"
#endif// _FUNC_LiveStatusServer 

// 推流
#ifdef _FUNC_PullTykdMobileService
#include "../StreamPusher/StreamPusherTykd.h"
#endif // _FUNC_PullTykdMobileService

#include "HandlerLiveStreammingServer.h"
#include "HttpServerLiveStreammingServer.h"

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

const std::string PssMsg_Value_Result_GliNotValid              = "211"; //
const std::string PssMsg_Value_ResultMsg_GliNotValid           = "GliNotValid";

CHttpServerLiveStreammingServer::CHttpServerLiveStreammingServer(ISocketHandler& h) : CGRestHttpdSocket(h)
{
    m_strBody = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;

    m_pPssCfg = NULL;
    m_pPullCfg = NULL;

    //统一管理
    m_pHandler = ((CHandlerLiveStreammingServer *)(&(Handler())));

    //m_strRetCode = "200";
}

void CHttpServerLiveStreammingServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        ez_printf_error("SSL not available\n");
    }

    //LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveStreammingServer::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerLiveStreammingServer::~CHttpServerLiveStreammingServer()
{
    //LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveStreammingServer::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerLiveStreammingServer::OnAccept()
{
    if (GetRemoteAddress()=="11.125.64.68"
        || GetRemoteAddress()=="11.125.64.69"
        )
    {
        SetCloseAndDelete();
        return;
    }

    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());
#ifdef _USE_MODULES_EZCONFIG

    //static CConfigLiveStreammingServer __cfg;
    //__cfg.update();
    //m_pPssCfg = &__cfg;
    m_pPssCfg = m_pHandler->m_pPssCfg;

    //static CConfigPullTykdMobileService __cfgPull;
    //__cfgPull.update();
    //m_pPullCfg = &__cfgPull;
    m_pPullCfg = m_pHandler->m_pPullCfg;

    //
    m_iTimeout = m_pPssCfg->getConfig().iTimeout;
#else

    m_iTimeout = HTTPD_CONN_TIMEOUT;
#endif//

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);

    m_strHttpHeader_Server = std::string(def_Pandora_StreammingServer_ServerName) + " " + g_Solar.GetVersion() + " " + g_Solar.GetHostID();
}

void CHttpServerLiveStreammingServer::OnHeader(const std::string& key,const std::string& value)
{
    //ez_printf_debug("%s:%s\n", key.c_str(), value.c_str());

    if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_AppKey))
    {
        m_strAppKey = value;

DBG(
        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_AppKey, m_strAppKey.c_str());
);
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_Nonce))
    {
        m_strNonce = value;

DBG(
        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_Nonce, m_strNonce.c_str());
);
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CurTime))
    {
        m_strCurTime = value;

DBG(
        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CurTime, m_strCurTime.c_str());
);
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CheckSum))
    {
        m_strCheckSum = value;

DBG(
        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CheckSum, m_strCheckSum.c_str());
);
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_PssSid))
    {
        m_strPssSid = value;

DBG(
        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_PssSid, m_strPssSid.c_str());
);
    }
    else
    {
        HttpdSocket::OnHeader(key, value);
    }
}

void CHttpServerLiveStreammingServer::OnDisconnect()
{
    ez_printf_debug("%s:%s:%d\n", __FUNCTION__, GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerLiveStreammingServer::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();

    m_strBody = "";


//#if 1 // 使用app鉴权

    // hello不鉴权
    if (GetUrl() == def_UniTrans_Req_hello
        // appkey 的不鉴权
        || GetUrl() == def_UniTrans_Req_ip
        //|| GetUrl() == def_Pandora_Req_manAppKeyCancel
       )
    {
        ez_printf_debug("Noneed Auth:%s\n", GetUrl().c_str());

        return;
    }

    //使用账号
    if (1 == m_pPssCfg->getConfig().iAccounts)
    {
        if (GetUrl() == def_Pandora_Req_manAppKeyCreate
            || GetUrl() == def_Pandora_Req_manAppKeyCancel
           )
        {
            ez_printf_debug("Noneed Auth:%s\n", GetUrl().c_str());

            return;
        }
    }

    std::string strCode = PssMsg_Value_Result_Unknow;
    std::string strMsg = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (GetUrl().length() < 2)
        {
            strCode = PssMsg_Value_Result_MethodNotAllowed; 
            strMsg = PssMsg_Value_ResultMsg_MethodNotAllowed;
            m_strRetCode = "405"; // https://www.restapitutorial.com/httpstatuscodes.html
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_MethodNotAllowed.c_str());

            break;
        }

#ifndef _SOAP_SVR_SUZHOUSIFA_23
        if (m_strPssSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
        {
            ez_printf_error("IllegalAccess PssSid not found\n");

            strCode = PssMsg_Value_Result_IllegalAccess; 
            strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
            m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

            //RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess, m_strRetCode);

            break;
        }
#endif //

        if (m_pPssCfg
            &&( (BitGet(m_pPssCfg->getConfig().iAuth, em_auth_type_appKey) == 1) || (BitGet(m_pPssCfg->getConfig().iAuth, em_auth_type_appKeyStrict) == 1))
           )
        {
DBG(
            ez_printf_debug("-->> do secret key chk\n");
);
            if (m_strAppKey.length()<1
                || m_strNonce.length()<1
                || m_strCurTime.length()<1
                || m_strCheckSum.length()<1
               )
            {
                //RetGeneralMsg(102, "auth data not valid", GetUrl());
                ez_printf_error("IllegalAccess, para not valid\n");
                ez_printf_debug("%s:%s\n", "m_strAppKey", m_strAppKey.c_str());
                ez_printf_debug("%s:%s\n", "m_strNonce", m_strNonce.c_str());
                ez_printf_debug("%s:%s\n", "m_strCurTime", m_strCurTime.c_str());
                ez_printf_debug("%s:%s\n", "m_strCheckSum", m_strCheckSum.c_str());

                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess, m_strRetCode);
                break;
            }

            // 严格检查
            if (m_pPssCfg
                && BitGet(m_pPssCfg->getConfig().iAuth, em_auth_type_appKeyStrict) == 1)
            {

                // 检查时间
                time_t ttCurTimeOfMsg;
                time_t ttCurTime=time(NULL);

                ezConvertFromString(ttCurTimeOfMsg, m_strCurTime);

                if (abs(ttCurTimeOfMsg-ttCurTime)>3600)
                {
                    ez_printf_error("IllegalAccess ttCurTimeOfMsg%ld, ttCurTime:%ld\n", ttCurTimeOfMsg, ttCurTime);

                    strCode = PssMsg_Value_Result_IllegalAccess; 
                    strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                    m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html
                    //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess, m_strRetCode);

                    break;
                }
                else
                {
                    ez_printf_debug("chk m_strCurTime.\n");
                }
            }

            // todo check appSecret from AppKey
            //std::string appSecret = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
            std::string appSecret ;//= "c48faffaedf5d4cae01c6a21c11476fc118e32b0";

#if 1

            int iretGet = m_pHandler->getAppKey(m_strAppKey, appSecret);// ->m_pCacheWithDb->getPair(m_strAppKey, appSecret);

            if (iretGet < 0)
#else

            if (m_strAppKey == "701818788")
            {
                appSecret = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
            }
            else if (m_strAppKey == "701918788")
            {
                appSecret = "9cba8e3fe970ddbca35424db80197a0167d68813";
            }
            else
#endif

            {
                ez_printf_error("Unknow: AppKey:%s\n", m_strAppKey.c_str());
                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess, m_strRetCode);

                break;
            }
            else
            {
                //ez_printf_debug("AppKey:%s, appSecret:%s\n", m_strAppKey.c_str(), appSecret.c_str());
            }

            m_strNonce+=appSecret;
            m_strNonce+=m_strCurTime;

            char dst[SHA1HashSizeDestBuffer] = {0};
            const char *src = m_strNonce.c_str();
            //ARG_USED(src);
            //ARG_USED(dst);
DBG(
            ez_printf_debug("ez_sha1_str:%s\n", src);
);
            int iret = ez_sha1_str(dst, sizeof(dst), src );

            if (iret<0)
            {
                ez_printf_error("ez_sha1_str failed src:[%s], dst:[%s]\n", src, dst);

                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess, m_strRetCode);

                break;
            }

            if (m_strCheckSum != dst)
            {
                ez_printf_warning("auth failed src:[%s], dst:[%s]\n", src, dst);

                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess, m_strRetCode);

                break;
            }
DBG(
            ez_printf_debug("authed src:[%s], dst:[%s]\n", src, dst);
);
        }
        // 去掉这个功能，利用防火墙
#if 0
        if (m_pPssCfg
            && BitGet(m_pPssCfg->getConfig().iAuth, em_auth_type_allowedIP) == 1)
        {
            ez_printf_debug("-->> do host chk\n");

            // tykd platform
            if (GetRemoteAddress()=="202.102.108.6"
                || GetRemoteAddress()=="202.102.108.29"
                || GetRemoteAddress()=="202.102.108.29"
               )
            {
                //m_iIsAuthedHost = 1;
                //ez_printf_debug("by tykd, ok\n");
            }
            else if (GetRemoteAddress()=="58.213.224.244"
                     || GetRemoteAddress()=="202.102.101.133"
                     || GetRemoteAddress()=="202.102.101.134"
                     || GetRemoteAddress()=="127.0.0.1"
                    )
            {
                //ez_printf_debug("by developer, ok\n");
            }
            else
            {
                ez_printf_warning("by unknow, close now!\n");
                //RetGeneralMsg(103, "host not allow", GetUrl());
                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                m_strRetCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess, m_strRetCode);
                break;
            }
        }
#endif
        //ok
        return;
    }
    while(0);

    RetGeneralMsg(strCode, strMsg, m_strRetCode);
    //Reset(); // prepare for next request
    //SetCloseAndDelete();
//#endif
}

// after OnHeaderComplete
void CHttpServerLiveStreammingServer::Exec()
{
    //ez_printf_debug("%s:%s:%d\n", __FUNCTION__, GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerLiveStreammingServer::OnUnknowRequest()
{}
void CHttpServerLiveStreammingServer::RetGeneralMsg(const std::string &strCode, const std::string &strMsg, const std::string &retCode)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl(), retCode);
}


/** Chunk of http body data recevied. */
void CHttpServerLiveStreammingServer::OnData(const char *p,size_t l)
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

    if (m_strBody.size()+l > def_Pandora_MaxMsgLen)
    {
        ez_printf_error("body(%lu), l(%lu) too long\n", m_strBody.length(), l);

        RetGeneralMsg(PssMsg_Value_Result_TooMuchData, PssMsg_Value_ResultMsg_TooMuchData, m_strRetCode);

        //SetCloseAndDelete();

        return;
    }

    m_strBody += pData;

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerLiveStreammingServer::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    if (GetUrl() == def_UniTrans_Req_ip)
    {
        Do_reqIp(m_strBody);
        return;
    }

    CPssStack __CezlibsWebServiceIF;

    if (m_strBody.size()<strlen("[{}]"))
    {
        ez_printf_error("BodyLen:%d\n", m_strBody.size());

        RetGeneralMsg(PssMsg_Value_Result_InvalidData, PssMsg_Value_ResultMsg_InvalidData, m_strRetCode);
        return;
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
            //LOG4CPLUS_INFO(LOG_OF_SMS, "__CezlibsWebServiceIF.Parse failed.");
            ez_printf_error(":%s\n", PssMsg_Value_ResultMsg_InvalidData.c_str());

            RetGeneralMsg(PssMsg_Value_Result_InvalidData, PssMsg_Value_ResultMsg_InvalidData, m_strRetCode);
            return;
        }
    }

    ez_printf_info("GetUrl:%s\n", GetUrl().c_str());
    //ez_printf_debug("Body:%s\n", m_strBody.c_str());

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_reqHello(m_strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_Echo)
    {
        Do_reqEcho(m_strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_describe)
    {
        Do_reqDescribe(m_strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_ip)
    {
        Do_reqIp(m_strBody);
    }
#if defined(_FUNC_LiveStreammingTykd2Hls)
    else if (GetUrl() == def_Pandora_Req_hlsPullTykdStart)
    {
        Do_reqPullTykd2HlsStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_hlsPullTykdStop)
    {
        Do_reqPullTykd2HlsStop(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtmpPullTykdStart)
    {
        Do_reqPullTykd2RtmpStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtmpPullTykdStop)
    {
        Do_reqPullTykd2RtmpStop(__CezlibsWebServiceIF);
    }
#endif //_FUNC_LiveStreammingTykd2Hls
#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    else if (GetUrl()==def_Pandora_Req_rtspPullTykdStart)
    {
        Do_reqPullTykd2RtspStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtspPullTykdStop)
    {
        Do_reqPullTykd2RtspStop(__CezlibsWebServiceIF);
    }
#endif //_FUNC_LiveStreammingTykd2Rtsp
    else if (GetUrl()==def_Pandora_Req_hlsPullStreamStart)
    {
        Do_reqPullStream2HlsStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_hlsPullStreamStop)
    {
        Do_reqPullStream2HlsStop(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtmpPullStreamList)
    {
        Do_reqPullStream2RtmpList(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtmpPullStreamStart)
    {
        Do_reqPullStream2RtmpStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtmpPullStreamStop)
    {
        Do_reqPullStream2RtmpStop(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtmpPullStreamList)
    {
        Do_reqPullStream2RtmpList(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_flvPullStreamStart)
    {
        Do_reqPullStream2FlvStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_flvPullStreamStop)
    {
        Do_reqPullStream2FlvStop(__CezlibsWebServiceIF);
    }
    // --2019-05-23 //暂未实现
    else if (GetUrl()==def_Pandora_Req_rtspPullStreamStart)
    {
        Do_reqPullStream2RtspStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_Pandora_Req_rtspPullStreamStop)
    {
        Do_reqPullStream2RtspStop(__CezlibsWebServiceIF);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()== def_Pandora_Req_pssOption)
    {
        onReq_Option(__CezlibsWebServiceIF);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()== def_Pandora_Req_manAppKeyCreate)
    {
        onReq_ManAppKeyCreate(__CezlibsWebServiceIF);
    }
    else if (GetUrl()== def_Pandora_Req_manAppKeyCancel)
    {
        onReq_ManAppKeyCancel(__CezlibsWebServiceIF);
    }
    else if (GetUrl()== def_Pandora_Req_manConfigMod)
    {
        onReq_ManConfigMod(__CezlibsWebServiceIF);
    }
#if defined(_FUNC_gMiniLicense)
    //
    else if (GetUrl()== def_Pandora_Req_getGminiLicense)
    {
        onReq_getGminiLicense(__CezlibsWebServiceIF);
    }
    else if (GetUrl()== def_Pandora_Req_setGminiLicense)
    {
        onReq_setGminiLicense(__CezlibsWebServiceIF);
    }
#endif //_FUNC_gMiniLicense
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else
    {
        ez_printf_error("%s:%s\n", PssMsg_Value_ResultMsg_MethodNotAllowed.c_str(), GetUrl().c_str());

        RetGeneralMsg(PssMsg_Value_Result_MethodNotAllowed
			, PssMsg_Value_ResultMsg_MethodNotAllowed+std::string(" : ")+GetUrl()
			, PssMsg_Value_Result_MethodNotAllowed);
    }

    //Reset(); // prepare for next request
    //SetCloseAndDelete();
    //m_strBody = "";
}

void CHttpServerLiveStreammingServer::Update()
{
    // 检查连接超时
    unsigned long ttNow = ez_sec_get_tick_count();//time(NULL);

    DBG(
        cout << ttNow-m_ttOndata << " tout:" << m_iTimeout << endl;
    );

    if (ttNow-m_ttOndata > m_iTimeout)
    {
        ez_printf_info("Conn:%d Timeout(%d), \n", GetSocket(), m_iTimeout);

        SetCloseAndDelete();

        return;
    }
}

size_t CHttpServerLiveStreammingServer::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerLiveStreammingServer::getMsgNumSent()
{
    return m_iMsgCountSend;
}

// 0 - valid
// others not
int CHttpServerLiveStreammingServer::chkValidDevChStreamPara(const std::string &strDevID, const std::string &strChannel, const std::string &strStream)
{
    if (strDevID.size()<3)
    {
        return -1;
    }

    if (strChannel.size()<1)
    {
        return -2;
    }

    if (strStream.size()<1)
    {
        return -3;
    }

    int iChn = atoi(strChannel.c_str());

    // now
    if (iChn != 0)
    {
        return -4;
    }

    int iStream = atoi(strStream.c_str());

    // 主辅码流
    if (iStream != 1
        && iStream != 2)
    {
        return -5;
    }

    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

// curl http://localhost:18088/UniTrans/hello -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerLiveStreammingServer::onReq_Option( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CUniMsg_requestMsgOption __reqMsg;
    CUniMsg_requestMsgOptionAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Success;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

    __ackMsg.m_strServer      = g_Solar.GetVersion();//"liveStreamming-uniTrans.195:203";
    __ackMsg.m_strHuid = g_Solar.GetHuid();//"liveStreamming-uniTrans.195:203";

    CFirmMsg Value;

    Value.m_strDesc = "Streamming";
    Value.m_strCode = "1";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "hikvision.com";
    Value.m_strCode = "2";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "dahuatech.com";
    Value.m_strCode = "3";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "megaeye";
    Value.m_strCode = "4";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "tykd";
    Value.m_strCode = "5";
    __ackMsg.Firms.push_back(Value);
#if 0
    Value.m_strDesc = "hk8200";
    Value.m_strCode = "6";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "tiandy.com";
    Value.m_strCode = "8";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "ryht.net";
    Value.m_strCode = "11";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "xiongmaitech.com";
    Value.m_strCode = "12";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "Tp-Link";
    Value.m_strCode = "13";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "JiGuang";
    Value.m_strCode = "14";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "onvif";
    Value.m_strCode = "16";
    __ackMsg.Firms.push_back(Value);

    Value.m_strDesc = "TUYA";
    Value.m_strCode = "22";
    __ackMsg.Firms.push_back(Value);
#endif
    CPssStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    return 0;
}
//
int CHttpServerLiveStreammingServer::onReq_ManAppKeyCreate( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CUniMsg_reqCreateAppKey __reqMsg;
    CUniMsg_reqCreateAppKeyAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 名字不匹配
#if 0
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
#endif

        if (__reqMsg.Account.length()<1)
        {
            ez_printf_error("%s:%s\n", PssMsg_Value_ResultMsg_InvalidData.c_str(), UniMsg_Key_Account.c_str());

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData+":"+UniMsg_Key_Account;
            break;
        }

        // 检查账号
        if (m_pPssCfg
            && m_pPssCfg->getConfig().iAccounts == 1)
        {
            if (__reqMsg.Password.length() < 1)
            {
                ez_printf_error("%s:%s\n", PssMsg_Value_ResultMsg_InvalidData.c_str(), UniMsg_Key_Password.c_str());

                __ackMsg.Result = PssMsg_Value_Result_InvalidData;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData+":"+UniMsg_Key_Password;
                break;
            }

            std::string __Values ;
            int iretGet = m_pHandler->getAccount(__reqMsg.Account, __Values);

            if (iretGet < 0)
            {
                ez_printf_error("%s:%s\n", PssMsg_Value_Result_AccountNotExist.c_str(), __reqMsg.Account.c_str());

                __ackMsg.Result = PssMsg_Value_Result_AccountNotExist;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_AccountNotExist+":"+__reqMsg.Account;
                break;
            }

            if (__Values != __reqMsg.Password)
            {
                ez_printf_error("%s:%s\n", PssMsg_Value_Result_PasswdNotValid.c_str(), __reqMsg.Account.c_str());

                __ackMsg.Result = PssMsg_Value_Result_PasswdNotValid;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_PasswdNotValid+":"+__reqMsg.Account;
                break;
            }

        }

        char bufAppKey[def_ez_APPKEY_BUFF_LEN] = {'\0'};
        char bufAppSecret[def_ez_APPSECRET_BUFF_LEN] = {'\0'};

        std::string strSrcAppKey = __reqMsg.Account + __reqMsg.Service + def_ez_APPKEY_SRC_ATTATCH;
        unsigned char *pRet = ez_md5_calc_ascii_digit((unsigned char *)bufAppKey, def_ez_APPKEY_BUFF_LEN, (unsigned char *)strSrcAppKey.c_str(), strSrcAppKey.length());

        if (NULL==pRet)
        {
            ez_printf_error("Error:ez_md5_calc_ascii_digit %s\n", strSrcAppKey.c_str());

            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
            break;
        }
        else
        {
            ez_printf_debug("ez_md5_calc_ascii_digit %s -> %s\n", strSrcAppKey.c_str(), bufAppKey);
        }

        //int iretGet = m_pHandler->m_pCacheWithDb->getPair(bufAppKey, __ackMsg.AppSecret);
        int iretGet = m_pHandler->getAppKey(bufAppKey, __ackMsg.AppSecret);

        if (iretGet < 0) // not found
        {
            std::string strSrcAppSecret = std::string(bufAppKey) + std::string(def_ez_APPSECRET_SRC_ATTATCH);
            int iret = ez_sha1_str(bufAppSecret, sizeof(bufAppSecret), strSrcAppSecret.c_str());

            if (iret >= 0)
            {
                ez_printf_debug("ez_sha1_str %s -> %s\n", strSrcAppSecret.c_str(), bufAppSecret);
            }
            else
            {
                ez_printf_error("ez_sha1_str %d\n", iret);

                __ackMsg.Result = PssMsg_Value_Result_internalError;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
                break;
            }
            //int iretAdd = m_pHandler->m_pCacheWithDb->addPair(bufAppKey, bufAppSecret);
            int iretAdd = m_pHandler->setAppKey(bufAppKey, bufAppSecret);

            if (iretAdd < 0) // failed
            {
                ez_printf_error("Failed addPair(%d):%s, %s\n", iretAdd, bufAppKey, bufAppSecret);

                __ackMsg.Result = PssMsg_Value_Result_internalError;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
                break;
            }

            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            __ackMsg.AppSecret = bufAppSecret;
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;
            // 直接使用获取的
            // __ackMsg.AppSecret = bufAppSecret;
        }

        __ackMsg.AppKey    = bufAppKey;
        __ackMsg.Validity  = def_AppKeyValidity_LongTerm;

        ez_printf_debug("req:%s\n", m_strBody.c_str());
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    //ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    return 0;
}

//
int CHttpServerLiveStreammingServer::onReq_ManAppKeyCancel( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CUniMsg_reqCreateAppKey __reqMsg;
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        if (__reqMsg.Account.length()<1)
        {
            ez_printf_error("Account not valid.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
            break;
        }

        char bufAppKey[def_ez_APPKEY_BUFF_LEN] = {'\0'};

        std::string strSrcAppKey = __reqMsg.Account + __reqMsg.Service + def_ez_APPKEY_SRC_ATTATCH;
        unsigned char *pRet = ez_md5_calc_ascii_digit((unsigned char *)bufAppKey, def_ez_APPKEY_BUFF_LEN, (unsigned char *)strSrcAppKey.c_str(), strSrcAppKey.length());

        if (NULL==pRet)
        {
            ez_printf_error("Failed ez_md5_calc_ascii_digit %s\n", strSrcAppKey.c_str());

            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
            break;
        }
        else
        {
            ez_printf_debug("ez_md5_calc_ascii_digit %s -> %s\n", strSrcAppKey.c_str(), bufAppKey);
        }

        if (m_strAppKey != bufAppKey)
        {
            ez_printf_error("NotPermitted Key:%s cancel:%s\n", m_strAppKey.c_str(), bufAppKey);

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedOperation;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedOperation;
            break;
        }

        std::string strAppSecret;
        //int iretGet = m_pHandler->m_pCacheWithDb->getPair(bufAppKey, strAppSecret);
        int iretGet = m_pHandler->getAppKey(bufAppKey, strAppSecret);

        if (iretGet < 0) // not found
        {
            ez_printf_debug("AppSecret of %s not found.\n", bufAppKey);

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
            break;
        }
        else
        {
            //int iretDel = m_pHandler->m_pCacheWithDb->delPair(bufAppKey);
            int iretDel = m_pHandler->cancelAppKey(bufAppKey);

            if (iretDel < 0)
            {
                ez_printf_debug("delPair %s failed.\n", bufAppKey);

                __ackMsg.Result = PssMsg_Value_Result_internalError;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
                break;
            }

            ez_printf_info("Cancel appkey for:%s, %s succeeded.\n", __reqMsg.Account.c_str(), __reqMsg.Service.c_str());

            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }

        ez_printf_debug("req:%s\n", m_strBody.c_str());
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    return 0;
}

//
int CHttpServerLiveStreammingServer::onReq_ManConfigMod( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CUniMsg_reqConfigModify __reqMsg;
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        //ez_printf_debug("req:\n%s\n", m_strBody.c_str());

        if (
            (UniMsg_Key_manConfig_TykdMobileService == __reqMsg.Section)
            && (UniMsg_Key_manConfig_HostRelative == __reqMsg.Name)
            )
        {
            CConfigPullTykdMobileService __cfgTykdMobileService;
            __cfgTykdMobileService.update();

            ConfigPullTykdMobileService &__tCfg = __cfgTykdMobileService.getConfig();
            if (__tCfg.strHostRelative!=__reqMsg.Value)
            {
                ez_printf_debug("Modify strHostRelative:%s\n", __reqMsg.Value.c_str());
                __tCfg.strHostRelative = __reqMsg.Value;

                __cfgTykdMobileService.commit();
            }
            else
            {
                ez_printf_debug("Sames strHostRelative:%s\n", __reqMsg.Value.c_str());
            }
        }
        else if (
            (UniMsg_Key_manConfig_TykdMobileService == __reqMsg.Section)
            && (UniMsg_Key_manConfig_DefaultAudioCodec == __reqMsg.Name)
            )
        {
            int iDefaultAudioCodec = atoi(__reqMsg.Value.c_str());
		
            CConfigPullTykdMobileService __cfgTykdMobileService;
            __cfgTykdMobileService.update();

            ConfigPullTykdMobileService &__tCfg = __cfgTykdMobileService.getConfig();

            if (__tCfg.iDefaultAudioCodec != iDefaultAudioCodec)
            {
                ez_printf_debug("Modify iDefaultAudioCodec:%d\n", iDefaultAudioCodec);
                __tCfg.iDefaultAudioCodec = iDefaultAudioCodec;

                __cfgTykdMobileService.commit();
            }
            else
            {
                ez_printf_debug("Sames iDefaultAudioCodec:%d\n", iDefaultAudioCodec);
            }
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedOperation;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedOperation;
            break;
        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    return 0;
}
#ifdef _FUNC_gMiniLicense
int CHttpServerLiveStreammingServer::onReq_getGminiLicense( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);
    CUniMsg_reqGetSetGmLic __reqMsg;
    CUniMsg_reqGetSetGmLicAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Success;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        __ackMsg.strGLic = g_gMiniLicense.getLicense();

        //ez_printf_debug("req:%s\n", m_strBody.c_str());
        if (g_gMiniLicense.getLicVers() == GMINI_LIC_V1)
        {
            __ackMsg.m_strllValidSeconds = ezConvertToString( g_gMiniLicense.getLicenseUptime() );
            __ackMsg.m_iGLicVer = 1;
        }
        else if (g_gMiniLicense.getLicVers() == GMINI_LIC_V2)
        {
            __ackMsg.m_strttValidDate = g_gMiniLicense.getValidDateStr();
            __ackMsg.m_iGLicVer = 2;
        }
        else
        {
            ez_printf_error("g_gMiniLicense Ver(%d) not fit Lic:%s\n", g_gMiniLicense.getLicVers(), __reqMsg.strGLic.c_str());
            __ackMsg.m_iGLicVer = 0;

            break;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    //ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveStreammingServer::onReq_setGminiLicense( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);
    CUniMsg_reqGetSetGmLic __reqMsg;
    CUniMsg_reqGetSetGmLicAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        int iret = g_gMiniLicense.setLicense(__reqMsg.strGLic.c_str());
        ez_printf_debug("setLicense:%d, Lic:%s\n", iret, __reqMsg.strGLic.c_str());

        if (-2 == iret)
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;

            ez_printf_info("setLicense failed:same license\n", iret, __reqMsg.strGLic.c_str());

            //break;
        }
        else if (iret < 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_GliNotValid;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_GliNotValid;

            ez_printf_error("setLicense failed:%d, Lic:%s\n", iret, __reqMsg.strGLic.c_str());

            break;
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }

        {
            //__ackMsg.m_iGLicVer = g_gMiniLicense.getLicVers();

            if (g_gMiniLicense.getLicVers() == GMINI_LIC_V1)
            {
                __ackMsg.m_strllValidSeconds = ezConvertToString( g_gMiniLicense.getLicenseUptime() );
                __ackMsg.m_iGLicVer = 1;
            }
            else if (g_gMiniLicense.getLicVers() == GMINI_LIC_V2)
            {
                __ackMsg.m_strttValidDate = g_gMiniLicense.getValidDateStr();
                __ackMsg.m_iGLicVer = 2;
            }
            else
            {
                ez_printf_error("g_gMiniLicense Ver(%d) not fit Lic:%s\n", g_gMiniLicense.getLicVers(), __reqMsg.strGLic.c_str());
                __ackMsg.m_iGLicVer = 0;

                break;
            }

        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    //ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

#endif //_FUNC_gMiniLicense

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if defined(_FUNC_LiveStreammingTykd2Hls)
// WEB子系统向连辅子系统注册接口地址的接口
// curl http://localhost:18088/hls/pull/tykd/startS -d "{ \"DevID\" : \"812A930AC7B\", \"Session\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"StreamID\" : \"812A930A78D\" }"
// curl http://localhost:18088/pss/start -d "{ \"DstID\" : \"812A930E4BC\", \"RTSPUrl\" : \"rtsp:\/\/202.102.108.137:50059\/realplay\" }"
int CHttpServerLiveStreammingServer::Do_reqPullTykd2HlsStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgHlsPullTykdStart __reqMsg;
    CPssMsgHlsPullTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
    __ackMsg.Hls.PlayUrl = "";

    do
    {
        //CConfigPullTykdMobileService __cfg;
        //__cfg.update();
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixHLS.length() < MIN_LENGTH_OF_HLS_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("%s\n", PssMsg_Value_ResultMsg_PssSidMismatched.c_str());

            __ackMsg.Result = PssMsg_Value_Result_PssSidMismatched;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_PssSidMismatched;

            break;
        }

        //ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.Stream.DevID.c_str());

        // for tykd only
        if (__reqMsg.Stream.DevID.length() != 11/*strlen(812A930AC7B)*/)
        {
            ez_printf_error("DevID:%s NOT valid.\n", __reqMsg.Stream.DevID.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_DevIDNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_DevIDNotValid;

            break;
        }

        std::string strUrlRtmp;
        std::string strUrlRtsp;
        EM_TERMINAL_STATUS_T iPushStatus = em_terminal_status_online; // no ret now
        std::string strDstStreamID;
        int iCountReq = 0;

        // 新协议定义channel统一从1开始，原有协议从0开始， 做兼容
        int __iChannel;
        std::string __strChannel;
        ezConvertFromString(__iChannel, __reqMsg.Stream.Channel);

        if (__iChannel < 0 || __iChannel>255)
        {
            //__iChannel = 0;
            ez_printf_error("invalid channel:%d\n", __iChannel);

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        __iChannel++;
        __strChannel = ezConvertToString(__iChannel);

        ez_printf_info("startPullStream2Hls - %s_%s_%s, ValidP:%s\n", __reqMsg.Stream.DevID.c_str(), __strChannel.c_str(), __reqMsg.Stream.Stream.c_str(), __reqMsg.Hls.ValidPeriod.c_str());

        // 转换完毕
        std::string strMediaSource;
        //std::string strDstStreamID;
        //Pusher_PushStatus_T iPushStatus = Pusher_PushStatus_T_Ini;

        int iret = g_StreamPusherTykd.startPullStream2Hls( \
                   __reqMsg.Stream.DevID
                   , "Alias"
                   , ezDef_StreamType_Str_tykd/* __reqMsg.Type */
                   , __reqMsg.Hls.ValidPeriod
                   , ""/*__reqMsg.StandStream.Url*/
                   , ""/*__reqMsg.StandStream.Account*/
                   , ""/*__reqMsg.StandStream.Passwd*/
                   , __strChannel/*__reqMsg.Stream.Channel*/
                   , __reqMsg.Stream.Stream
                   , __reqMsg.Stream.AudioEncode//"2" //strAudioEncode #0 copy, 1 aac, 2 no, 3 libfdk_aac
                   , "0"/*__reqMsg.StandStream.VideoEncode*//*0-copy, 1-libx264*/
                   , "0" //strVideoResize
                   , "0" //strVideoRefps

                   , __ackMsg.Hls.PlayUrl
                   , __ackMsg.Hls.PlayUrlHlss
                   , __ackMsg.Hls.PlayUrlRtmp
                   , __ackMsg.Hls.PlayUrlFlv
                   , __ackMsg.Hls.PlayUrlDash
                   , strMediaSource
                   , strDstStreamID
                   , iPushStatus
                   , iCountReq);

        //ez_printf_debug("PlayUrl:%s\n", __ackMsg.Hls.PlayUrl.c_str());
        //ez_printf_debug("PlayUrlRtmp:%s\n", __ackMsg.Hls.PlayUrlRtmp.c_str());
        //ez_printf_debug("PlayUrlFlv:%s\n", __ackMsg.Hls.PlayUrlFlv.c_str());
        //ez_printf_debug("PlayUrlDash:%s\n", __ackMsg.Hls.PlayUrlDash.c_str());

#ifdef _FUNC_LiveStatusServer
        time_t __ttLastUpdate = g_AgentLiveStatusServer.GetHandler()->getLastUpdate(strDstStreamID);
        ARG_USED(__ttLastUpdate);

        if (__ttLastUpdate > 0)
        {
            __ackMsg.Hls.Status = "1"; //ezConvertToString(__ttLastUpdate>0);
        }
        else
        {
            __ackMsg.Hls.Status = ezConvertToString(iPushStatus==em_terminal_status_online);
        }

        ez_printf_debug("%s::ttLastUpdate:%d\n", strDstStreamID.c_str(), __ttLastUpdate);
#else
        __ackMsg.Hls.Status = ezConvertToString(iPushStatus==em_terminal_status_online);
#endif // _FUNC_LiveStatusServer

        ez_printf_debug("startPullStream2Hls:%d, __reqMsg.m_iDetails:%d\n", iret, __reqMsg.m_iDetails);

        if (iret==0)
        {
            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails
                || em_PssMsg_Key_Details_Undefined == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            }
            else
            {
                //__ackMsg.Hls.PlayUrl = "";
                __ackMsg.Hls.PlayUrl.clear();
                __ackMsg.Hls.PlayUrlHlss.clear();
                __ackMsg.Hls.PlayUrlRtmp.clear();
                __ackMsg.Hls.PlayUrlFlv.clear();
                __ackMsg.Hls.PlayUrlDash.clear();
                __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
            }//#endif
        }
        else if (iret==-1) // DevID数据不对
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else if (iret==-2)
        {
            //__ackMsg.Result = PssMsg_Value_Result_Redo;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";

            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails
                || em_PssMsg_Key_Details_Undefined == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";
            }
            else
            {
                if (__ackMsg.Hls.Status != "1") // 还没有成功
                {
                    __ackMsg.Hls.PlayUrl.clear();
                    __ackMsg.Hls.PlayUrlHlss.clear();
                    __ackMsg.Hls.PlayUrlRtmp.clear();
                    __ackMsg.Hls.PlayUrlFlv.clear();
                    __ackMsg.Hls.PlayUrlDash.clear();
                    __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
                }
                else
                {
                    __ackMsg.Result = PssMsg_Value_Result_Success;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
                }
            }//#endif
        }
        else if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
        }
        else //if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveStreammingServer::Do_reqPullTykd2HlsStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgHlsPullTykdStop __reqMsg;
    CPssMsgHlsPullTykdStopAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
            //return -1;
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        // 新协议定义channel统一从1开始，原有协议从0开始， 做兼容
        int __iChannel;
        std::string __strChannel;
        ezConvertFromString(__iChannel, __reqMsg.Stream.Channel);

        if (__iChannel < 0 || __iChannel>255)
        {
            __iChannel = 0;
        }
        __iChannel++;
        __strChannel = ezConvertToString(__iChannel);
        // 转换完毕

        int iret = g_StreamPusherTykd.stopPullStream2Hls( \
                   __reqMsg.Stream.DevID
                   , ezDef_StreamType_Str_tykd/* __reqMsg.Type */
                   , ""/*__reqMsg.StandStream.Url*/
                   , ""/*__reqMsg.StandStream.Account*/
                   , ""/*__reqMsg.StandStream.Passwd*/
                   , __strChannel
                   , __reqMsg.Stream.Stream
                                                        );

        ez_printf_debug("stopPullStream2Hls:%d\n", iret);

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, stop not permited.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveStreammingServer::Do_reqPullTykd2RtmpStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRtmpPullTykdStart __reqMsg;
    CPssMsgRtmpPullTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
    __ackMsg.Rtmp.PlayUrl = "";

    do
    {
        //CConfigPullTykdMobileService __cfg;
        //__cfg.update();
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixRTMP.length() < MIN_LENGTH_OF_RTMP_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
            //return -1;
        }

#ifndef _SOAP_SVR_SUZHOUSIFA_23
        // 名字不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
#endif //_SOAP_SVR_SUZHOUSIFA_23
        ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.Stream.DevID.c_str());
        // for tykd only
        if (__reqMsg.Stream.DevID.length() != 11/*strlen(812A930AC7B)*/)
        {
            ez_printf_error("DevID:%s NOT valid.\n", __reqMsg.Stream.DevID.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_DevIDNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_DevIDNotValid;

            break;
        }

        std::string strUrlHls;
        std::string strUrlHlss;
        std::string strUrlFlv;
        std::string strUrlDash;
        //std::string strUrlRtsp;
        EM_TERMINAL_STATUS_T iPushStatus = em_terminal_status_online; // no ret now
        std::string strDstStreamID = "";
        int iCountReq = 0;

        // 新协议定义channel统一从1开始，原有协议从0开始， 做兼容
        int __iChannel;
        std::string __strChannel;
        ezConvertFromString(__iChannel, __reqMsg.Stream.Channel);

        if (__iChannel < 0 || __iChannel>255)
        {
            //__iChannel = 0;
            ez_printf_error("invalid channel:%d\n", __iChannel);

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        __iChannel++;
        __strChannel = ezConvertToString(__iChannel);

        ez_printf_debug("__strChannel:%s\n", __strChannel.c_str());
        // 转换完毕
        std::string strMediaSource;

        int iret = g_StreamPusherTykd.startPullStream2Hls( \
                   __reqMsg.Stream.DevID
                   , "Alias"
                   , ezDef_StreamType_Str_tykd/* __reqMsg.Type */
                   , __reqMsg.Rtmp.ValidPeriod
                   , ""/*__reqMsg.StandStream.Url*/
                   , ""/*__reqMsg.StandStream.Account*/
                   , ""/*__reqMsg.StandStream.Passwd*/
                   , __strChannel/*__reqMsg.Stream.Channel*/
                   , __reqMsg.Stream.Stream
                   , "2" //#0 copy, 1 aac, 2 no
                   , "0"/*__reqMsg.StandStream.VideoEncode*//*0-copy, 1-libx264*/
                   , "0"
                   , "0"

                   , strUrlHls
                   , strUrlHlss
                   , __ackMsg.Rtmp.PlayUrl/*__ackMsg.Delivery.UrlRtmp*/
                   , strUrlFlv
                   , strUrlDash
                   , strMediaSource
                   , strDstStreamID
                   , iPushStatus
                   , iCountReq);

        ez_printf_debug("startPullStream2Hls:%d\n", iret);
#ifdef _FUNC_LiveStatusServer
        time_t __ttLastUpdate = g_AgentLiveStatusServer.GetHandler()->getLastUpdate(strDstStreamID);
        ARG_USED(__ttLastUpdate);

        if (__ttLastUpdate > 0)
        {
            __ackMsg.Rtmp.Status = "1"; //ezConvertToString(__ttLastUpdate>0);
        }
        else
        {
            __ackMsg.Rtmp.Status = ezConvertToString(iPushStatus==em_terminal_status_online);
        }

        ez_printf_debug("%s::ttLastUpdate:%d\n", strDstStreamID.c_str(), __ttLastUpdate);
#else
        __ackMsg.Rtmp.Status = ezConvertToString(iPushStatus==em_terminal_status_online);
#endif // _FUNC_LiveStatusServer

        if (iret==0)
        {
            // 默认参数或者传递了1
            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails
                || em_PssMsg_Key_Details_Undefined == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            }
            else
            {
                __ackMsg.Rtmp.PlayUrl = "";
                __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
            }//#endif
        }
        else if (iret==-1) // DevID数据不对
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else if (iret==-2)
        {
            //__ackMsg.Result = PssMsg_Value_Result_Redo;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";

            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails
                || em_PssMsg_Key_Details_Undefined == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";
            }
            else
            {
                if (__ackMsg.Rtmp.Status != "1") // 还没有成功
                {
                    __ackMsg.Rtmp.PlayUrl = "";
                    __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
                }
                else
                {
                    __ackMsg.Result = PssMsg_Value_Result_Success;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
                }
            }//#endif
        }
        else if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
        }
        else //if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveStreammingServer::Do_reqPullTykd2RtmpStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRtmpPullTykdStop __reqMsg;
    CPssMsgRtmpPullTykdStopAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
            //return -1;
        }

        // 名字不匹配
#ifndef _SOAP_SVR_SUZHOUSIFA_23
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
#endif

        // 新协议定义channel统一从1开始，原有协议从0开始， 做兼容
        int __iChannel;
        std::string __strChannel;
        ezConvertFromString(__iChannel, __reqMsg.Stream.Channel);

        if (__iChannel < 0 || __iChannel>255)
        {
            __iChannel = 0;
        }
        __iChannel++;
        __strChannel = ezConvertToString(__iChannel);
        // 转换完毕

        int iret = g_StreamPusherTykd.stopPullStream2Hls( \
                   __reqMsg.Stream.DevID
                   , ezDef_StreamType_Str_tykd/* __reqMsg.Type */
                   , ""/*__reqMsg.StandStream.Url*/
                   , ""/*__reqMsg.StandStream.Account*/
                   , ""/*__reqMsg.StandStream.Passwd*/
                   , __strChannel
                   , __reqMsg.Stream.Stream
                                                        );

        ez_printf_debug("stopPullStream2Hls:%d\n", iret);

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, stop not permited.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}
#endif //_FUNC_LiveStreammingTykd2Hls

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
// 开始rtsp 分发
// curl http://localhost:18088/hls/pull/tykd/start -d "{ \"DevID\" : \"812A930AC7B\", \"Session\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"StreamID\" : \"812A930A78D\" }"
int CHttpServerLiveStreammingServer::Do_reqPullTykd2RtspStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRtspPullTykdStart __reqMsg;
    CPssMsgRtspPullTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
    __ackMsg.Rtsp.PlayUrl = "";

    do
    {
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixRTSP.length() < MIN_LENGTH_OF_RTSP_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
        ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.Stream.DevID.c_str());
        // for tykd only
        if (__reqMsg.Stream.DevID.length() != 11/*strlen(812A930AC7B)*/)
        {
            ez_printf_error("DevID:%s NOT valid.\n", __reqMsg.Stream.DevID.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_DevIDNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_DevIDNotValid;

            break;
        }

        std::string strUrlHls;
        std::string strUrlRtmp;
        int iCountReq = 0;

        int iret = g_StreamPusherTykd.startPullRtsp( \
                   __reqMsg.Stream.DevID
                   , __reqMsg.Stream.Channel
                   , __reqMsg.Stream.Stream
                   , __reqMsg.Rtsp.ValidPeriod
                   , __ackMsg.Rtsp.PlayUrl
                   , iCountReq);

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1) // DevID数据不对
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else if (iret==-2)
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";
        }
        else if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
        }
        else //if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

// 停止rtsp 分发
int CHttpServerLiveStreammingServer::Do_reqPullTykd2RtspStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRtspPullTykdStop __reqMsg;
    CPssMsgRtspPullTykdStopAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
            //return -1;
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        int iret = g_StreamPusherTykd.stopPullRtsp(__reqMsg.Stream.DevID, __reqMsg.Stream.Channel, __reqMsg.Stream.Stream);
        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, stop not permited.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            //__ackMsg.result = "400";
            //__ackMsg.reason = "failed";
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}
#endif //_FUNC_LiveStreammingTykd2Rtsp

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// WEB子系统向连辅子系统注册接口地址的接口
// curl http://localhost:18088/pss/start -d "{ \"DstID\" : \"812A930E4BC\", \"RTSPUrl\" : \"rtsp:\/\/202.102.108.137:50059\/realplay\" }"
int CHttpServerLiveStreammingServer::Do_reqPullStream2HlsStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgHlsPullStreamStart __reqMsg;
    CPssMsgHlsPullTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
    //__ackMsg.Hls.PlayUrl = "";

    do
    {
        //CConfigPullTykdMobileService __cfg;
        //__cfg.update();
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixHLS.length() < MIN_LENGTH_OF_HLS_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        if (__reqMsg.Type==ezDef_StreamType_Str_megaeye)
        {
            //ez_printf_error("ParaNotValidChannel.\n");

            //__ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            //__ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            //break;
            __reqMsg.StandStream.Channel = "1";
            __reqMsg.StandStream.Subtype = "1";
        }

        int iTheChnnel = 0;
        int iTheSubtype = 0;
        ezConvertFromString(iTheChnnel, __reqMsg.StandStream.Channel);
        ezConvertFromString(iTheSubtype, __reqMsg.StandStream.Subtype);

        if (iTheChnnel < 1)
        {
            ez_printf_error("Channel:%s NOT valid.\n", __reqMsg.StandStream.Channel.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            break;
        }

        if (iTheSubtype < 1)
        {
            ez_printf_error("Subtype:%s NOT valid.\n", __reqMsg.StandStream.Subtype.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_SubtypeNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid;

            break;
        }

        std::string strUrlRtmp;
        std::string strMediaSource;
        std::string strUrlRtsp;
        std::string strDstStreamID;

        EM_TERMINAL_STATUS_T iPushStatus = em_terminal_status_unknown;
        int iCountReq = 0;

        int iret = g_StreamPusherTykd.startPullStream2Hls( \
                   __reqMsg.ID
                   , __reqMsg.Alias
                   , __reqMsg.Type
                   , __reqMsg.ValidPeriod
                   , __reqMsg.StandStream.Url
                   , __reqMsg.StandStream.Account
                   , __reqMsg.StandStream.Passwd
                   , __reqMsg.StandStream.Channel
                   , __reqMsg.StandStream.Subtype
                   , __reqMsg.StandStream.AudioEncode
                   , __reqMsg.StandStream.VideoEncode
                   , __reqMsg.StandStream.VideoResize
                   , __reqMsg.StandStream.VideoRefps

                   , __ackMsg.Hls.PlayUrl
                   , __ackMsg.Hls.PlayUrlHlss
                   , __ackMsg.Hls.PlayUrlRtmp
                   , __ackMsg.Hls.PlayUrlFlv
                   , __ackMsg.Hls.PlayUrlDash
                   , __ackMsg.Hls.MediaSource
                   , strDstStreamID
                   , iPushStatus
                   , iCountReq);

        //ez_printf_debug("PlayUrl:%s\n", __ackMsg.Hls.PlayUrl.c_str());
        //ez_printf_debug("PlayUrlRtmp:%s\n", __ackMsg.Hls.PlayUrlRtmp.c_str());
        //ez_printf_debug("PlayUrlFlv:%s\n", __ackMsg.Hls.PlayUrlFlv.c_str());
        //ez_printf_debug("PlayUrlDash:%s\n", __ackMsg.Hls.PlayUrlDash.c_str());

#ifdef _FUNC_LiveStatusServer
        time_t __ttLastUpdate = g_AgentLiveStatusServer.GetHandler()->getLastUpdate(strDstStreamID);
        ARG_USED(__ttLastUpdate);

        if (__ttLastUpdate > 0)
        {
            __ackMsg.Hls.Status = "1"; //ezConvertToString(__ttLastUpdate>0);
        }
        else
        {
            __ackMsg.Hls.Status = ezConvertToString(iPushStatus==em_terminal_status_unknown);
        }

        ez_printf_debug("%s::ttLastUpdate:%ld, Lasted:%ld\n", strDstStreamID.c_str(), __ttLastUpdate, time(NULL)-__ttLastUpdate);
#else
        __ackMsg.Hls.Status = ezConvertToString(iPushStatus==em_terminal_status_unknown);
#endif // _FUNC_LiveStatusServer

        if (iret==0)
        {
            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            }
            else
            {
                // 不返回不可播放的地址
                __ackMsg.Hls.PlayUrl.clear();
                __ackMsg.Hls.PlayUrlHlss.clear();
                __ackMsg.Hls.PlayUrlRtmp.clear();
                __ackMsg.Hls.PlayUrlFlv.clear();
                __ackMsg.Hls.PlayUrlDash.clear();

                __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
            }//#endif
        }
        else if (iret==-1) // DevID数据不对
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else if (iret==-2)
        {
            //__ackMsg.Result = PssMsg_Value_Result_Redo;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";

            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";
            }
            else
            {
                if (__ackMsg.Hls.Status != "1") // 还没有成功
                {
                    // 不返回不可播放的地址
                    __ackMsg.Hls.PlayUrl.clear();
                    __ackMsg.Hls.PlayUrlHlss.clear();
                    __ackMsg.Hls.PlayUrlRtmp.clear();
                    __ackMsg.Hls.PlayUrlFlv.clear();
                    __ackMsg.Hls.PlayUrlDash.clear();

                    __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
                }
                else
                {
                    __ackMsg.Result = PssMsg_Value_Result_Success;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
                }
            }//#endif
        }
        else if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
        }
        else //if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

//
int CHttpServerLiveStreammingServer::Do_reqPullStream2HlsStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPssMsgHlsPullStreamStart __reqMsg;
    CPssMsgHlsPullTykdStopAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixHLS.length() < MIN_LENGTH_OF_HLS_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        if (__reqMsg.Type==ezDef_StreamType_Str_megaeye)
        {
            //ez_printf_error("ParaNotValidChannel.\n");

            //__ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            //__ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            //break;
            __reqMsg.StandStream.Channel = "1";
            __reqMsg.StandStream.Subtype = "1";
        }

        int iTheChnnel = 0;
        int iTheSubtype = 0;
        ezConvertFromString(iTheChnnel, __reqMsg.StandStream.Channel);
        ezConvertFromString(iTheSubtype, __reqMsg.StandStream.Subtype);

        if (iTheChnnel < 1)
        {
            ez_printf_error("Channel:%s NOT valid.\n", __reqMsg.StandStream.Channel.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            break;
        }

        if (iTheSubtype < 1)
        {
            ez_printf_error("Subtype:%s NOT valid.\n", __reqMsg.StandStream.Subtype.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_SubtypeNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid;

            break;
        }

        int iret = g_StreamPusherTykd.stopPullStream2Hls( \
                   __reqMsg.ID
                   , __reqMsg.Type
                   , __reqMsg.StandStream.Url
                   , __reqMsg.StandStream.Account
                   , __reqMsg.StandStream.Passwd
                   , __reqMsg.StandStream.Channel
                   , __reqMsg.StandStream.Subtype
                                                        );
        ez_printf_debug("%s:%d\n", "stopPullStream2Hls", iret);

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, stop not permited.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//
int CHttpServerLiveStreammingServer::Do_reqPullStream2RtmpStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgHlsPullStreamStart __reqMsg; // 消息体同hls
    CPssMsgRtmpPullTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
    __ackMsg.Rtmp.PlayUrl = "";

    do
    {
        //CConfigPullTykdMobileService __cfg;
        //__cfg.update();
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixRTMP.length() < MIN_LENGTH_OF_RTMP_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("PssSid mismatching.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
        if (__reqMsg.Type==ezDef_StreamType_Str_megaeye)
        {
            //ez_printf_error("ParaNotValidChannel.\n");

            //__ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            //__ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            //break;
            __reqMsg.StandStream.Channel = "1";
            __reqMsg.StandStream.Subtype = "1";
        }

        int iTheChnnel = 0;
        int iTheSubtype = 0;
        ezConvertFromString(iTheChnnel, __reqMsg.StandStream.Channel);
        ezConvertFromString(iTheSubtype, __reqMsg.StandStream.Subtype);

        if (iTheChnnel < 1)
        {
            ez_printf_error("Channel:%s NOT valid.\n", __reqMsg.StandStream.Channel.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            break;
        }

        if (iTheSubtype < 1)
        {
            ez_printf_error("Subtype:%s NOT valid.\n", __reqMsg.StandStream.Subtype.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_SubtypeNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid;

            break;
        }
	
        std::string strUrlHls;
        std::string strUrlHlss;
        std::string strUrlFlv;
        std::string strUrlDash;
        //std::string strUrlRtmp;
        std::string strUrlRtsp;
        std::string strDstStreamID;
        int iCountReq = 0;
        EM_TERMINAL_STATUS_T iPushStatus = em_terminal_status_unknown;
        int iret = g_StreamPusherTykd.startPullStream2Hls( \
                   __reqMsg.ID
                   , __reqMsg.Alias
                   , __reqMsg.Type
                   , __reqMsg.ValidPeriod
                   , __reqMsg.StandStream.Url
                   , __reqMsg.StandStream.Account
                   , __reqMsg.StandStream.Passwd
                   , __reqMsg.StandStream.Channel
                   , __reqMsg.StandStream.Subtype
                   , __reqMsg.StandStream.AudioEncode
                   , __reqMsg.StandStream.VideoEncode
                   , __reqMsg.StandStream.VideoResize
                   , __reqMsg.StandStream.VideoRefps

                   , strUrlHls
                   , strUrlHlss
                   , __ackMsg.Rtmp.PlayUrl
                   , strUrlFlv
                   , strUrlDash
                   , __ackMsg.Rtmp.MediaSource
                   , strDstStreamID
                   , iPushStatus
                   , iCountReq);

        //ez_printf_debug("MediaSource:%s\n", __ackMsg.Rtmp.MediaSource.c_str());

#ifdef _FUNC_LiveStatusServer
        time_t __ttLastUpdate = g_AgentLiveStatusServer.GetHandler()->getLastUpdate(strDstStreamID);
        ARG_USED(__ttLastUpdate);

        if (__ttLastUpdate > 0)
        {
            __ackMsg.Rtmp.Status = "1"; //ezConvertToString(__ttLastUpdate>0);
        }
        else
        {
            __ackMsg.Rtmp.Status = ezConvertToString(iPushStatus==em_terminal_status_unknown);
        }

        ez_printf_debug("%s::ttLastUpdate:%d\n", strDstStreamID.c_str(), __ttLastUpdate);
#else
        __ackMsg.Rtmp.Status = ezConvertToString(iPushStatus==em_terminal_status_unknown);
#endif // _FUNC_LiveStatusServer

        if (iret==0)
        {
            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            }
            else
            {
                __ackMsg.Rtmp.PlayUrl = "";
                __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
            }//#endif
        }
        else if (iret==-1) // DevID数据不对
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else if (iret==-2)
        {
            //__ackMsg.Result = PssMsg_Value_Result_Redo;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";

            if (em_PssMsg_Key_Details_QuickPlayurl == __reqMsg.m_iDetails)// 立即返回播放地址
            {
                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";
            }
            else
            {
                if (__ackMsg.Rtmp.Status != "1") // 还没有成功
                {
                    __ackMsg.Rtmp.PlayUrl = "";
                    __ackMsg.Result = PssMsg_Value_Result_Prepareing;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Prepareing;
                }
                else
                {
                    __ackMsg.Result = PssMsg_Value_Result_Success;
                    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
                }
            }//#endif
        }
        else if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
        }
        else if (iret==-5) 
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else //if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

        if (iret!=0 && iret!=-2)
        {
            ez_printf_error("Failed startPullStream2Hls:%d\n", iret);
        }
        else
        {
            // 存储信息
            // 调试用，功能开发完毕后留意修改
#if 0            // 让storage失效
            __ackMsg.Storage.Type        = "3";
            __ackMsg.Storage.Name        = "nvr://admin:admin123@192.168.5.117:37777/sreq?http=80&rtsp=554&onvif=no";
            __ackMsg.Storage.Channel     = "2";
            __ackMsg.Storage.Description = "DH";
            __ackMsg.Storage.Status      = 1;
            __ackMsg.Storage.Capacity    = 1024*1024;
            __ackMsg.Storage.FreeSpace   = 1024;
#endif //            // 让storage失效
        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

//
int CHttpServerLiveStreammingServer::Do_reqPullStream2RtmpStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPssMsgHlsPullStreamStart __reqMsg;
    CPssMsgRtmpPullTykdStopAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixRTMP.length() < MIN_LENGTH_OF_RTMP_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        if (__reqMsg.Type==ezDef_StreamType_Str_megaeye)
        {
            //ez_printf_error("ParaNotValidChannel.\n");

            //__ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            //__ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            //break;
            __reqMsg.StandStream.Channel = "1";
            __reqMsg.StandStream.Subtype = "1";
        }

        int iTheChnnel = 0;
        int iTheSubtype = 0;
        ezConvertFromString(iTheChnnel, __reqMsg.StandStream.Channel);
        ezConvertFromString(iTheSubtype, __reqMsg.StandStream.Subtype);

        if (iTheChnnel < 1)
        {
            ez_printf_error("Channel:%s NOT valid.\n", __reqMsg.StandStream.Channel.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            break;
        }

        if (iTheSubtype < 1)
        {
            ez_printf_error("Subtype:%s NOT valid.\n", __reqMsg.StandStream.Subtype.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_SubtypeNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid;

            break;
        }

        int iret = g_StreamPusherTykd.stopPullStream2Hls( \
                   __reqMsg.ID
                   , __reqMsg.Type
                   , __reqMsg.StandStream.Url
                   , __reqMsg.StandStream.Account
                   , __reqMsg.StandStream.Passwd
                   , __reqMsg.StandStream.Channel
                   , __reqMsg.StandStream.Subtype
                                                        );
        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, stop not permited.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

// hls and rtmp公用
int CHttpServerLiveStreammingServer::Do_reqPullStream2RtmpList( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsg_requestMsgDeviceList __reqMsg;
    CPssMsg_requestMsgDeviceListAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Success;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

    do
    {
        //CConfigPullTykdMobileService __cfg;
        //__cfg.update();
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixHLS.length() < MIN_LENGTH_OF_HLS_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strGroupID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.m_strGroupID.c_str());

        if (__reqMsg.m_strGroupID.length()  < 1)
        {
            ez_printf_error("DevID:%s NOT valid.\n", __reqMsg.m_strGroupID.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_DevIDNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_DevIDNotValid;

            break;
        }

        // 返回数量
        int iret = g_StreamPusherTykd.listPullerStream2Hls(__ackMsg, __reqMsg.m_iPageSize, __reqMsg.m_iPages);

        if (iret < 1)
        {
            ez_printf_error("listPullerStream2Hls(%d):%s.\n", iret, __reqMsg.m_strGroupID.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_NoDevice;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_NoDevice;

            break;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}
int CHttpServerLiveStreammingServer::Do_reqPullStream2FlvStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    return Do_reqPullStream2HlsStart(JsonData);
}
int CHttpServerLiveStreammingServer::Do_reqPullStream2FlvStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    return Do_reqPullStream2HlsStop(JsonData);
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//
int CHttpServerLiveStreammingServer::Do_reqPullStream2RtspStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgHlsPullStreamStart __reqMsg; // 消息体同hls
    CPssMsgRtmpPullTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
    __ackMsg.Rtmp.PlayUrl = "";

    do
    {
#if defined(_FUNC_LiveStreammingTykd2Rtsp) || defined(_FUNC_LiveStreammingStream2Rtsp)
        if (m_pPullCfg->getConfig().strDefaultUrlPrefixRTSP.length() < MIN_LENGTH_OF_RTMP_URL)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }
#else
        if (1)
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str());

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;

            break;
        }
#endif//_FUNC_LiveStreammingTykd2Rtsp

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
        if (__reqMsg.Type==ezDef_StreamType_Str_megaeye)
        {
            //ez_printf_error("ParaNotValidChannel.\n");

            //__ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            //__ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            //break;
            __reqMsg.StandStream.Channel = "1";
            __reqMsg.StandStream.Subtype = "1";
        }

        int iTheChnnel = 0;
        int iTheSubtype = 0;
        ezConvertFromString(iTheChnnel, __reqMsg.StandStream.Channel);
        ezConvertFromString(iTheSubtype, __reqMsg.StandStream.Subtype);

        if (iTheChnnel < 1)
        {
            ez_printf_error("Channel:%s NOT valid.\n", __reqMsg.StandStream.Channel.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            break;
        }

        if (iTheSubtype < 1)
        {
            ez_printf_error("Subtype:%s NOT valid.\n", __reqMsg.StandStream.Subtype.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_SubtypeNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid;

            break;
        }

        std::string strUrlHls;
        //std::string strUrlRtmp;
        std::string strUrlRtsp;
        //int iCountReq = 0;
#if 0
        int iret = g_StreamPusherTykd.startPullStream2Hls( \
                   __reqMsg.ID
                   , __reqMsg.Type
                   , __reqMsg.ValidPeriod
                   , __reqMsg.StandStream.Url
                   , __reqMsg.StandStream.Account
                   , __reqMsg.StandStream.Passwd
                   , __reqMsg.StandStream.Channel
                   , __reqMsg.StandStream.Subtype
                   , __reqMsg.StandStream.VideoEncode
                   , __reqMsg.StandStream.VideoResize
                   , __reqMsg.StandStream.VideoRefps

                   , strUrlHls
                   , __ackMsg.Rtmp.PlayUrl
                   //, strUrlRtsp
                   , iCountReq);

#else
        int iret = -4;
#endif

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1) // DevID数据不对
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
        else if (iret==-2)
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;//"repeat for this url or id are not permitted";
        }
        else if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_internalError;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_internalError;
        }
        else //if (iret==-3)
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

//
int CHttpServerLiveStreammingServer::Do_reqPullStream2RtspStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPssMsgHlsPullStreamStart __reqMsg;
    CPssMsgRtmpPullTykdStopAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        if (__reqMsg.Type==ezDef_StreamType_Str_megaeye)
        {
            //ez_printf_error("ParaNotValidChannel.\n");

            //__ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            //__ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            //break;
            __reqMsg.StandStream.Channel = "1";
            __reqMsg.StandStream.Subtype = "1";
        }

        int iTheChnnel = 0;
        int iTheSubtype = 0;
        ezConvertFromString(iTheChnnel, __reqMsg.StandStream.Channel);
        ezConvertFromString(iTheSubtype, __reqMsg.StandStream.Subtype);

        if (iTheChnnel < 1)
        {
            ez_printf_error("Channel:%s NOT valid.\n", __reqMsg.StandStream.Channel.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_ChannelNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_ChannelNotValid;

            break;
        }

        if (iTheSubtype < 1)
        {
            ez_printf_error("Subtype:%s NOT valid.\n", __reqMsg.StandStream.Subtype.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_SubtypeNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid;

            break;
        }
#if 0
        int iret = g_StreamPusherTykd.stopPullStream2Hls( \
                   __reqMsg.ID
                   , __reqMsg.Type
                   , __reqMsg.StandStream.Url
                   , __reqMsg.StandStream.Account
                   , __reqMsg.StandStream.Passwd
                   , __reqMsg.StandStream.Channel
                   , __reqMsg.StandStream.Subtype
                                                        );
#else 
    int iret = -1;
#endif
        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, stop not permited.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

