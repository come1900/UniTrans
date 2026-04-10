/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveRecordServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveRecordServer.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#endif //HAVE_LIBUUID
#include "str_opr.h"
#include "ez_bit.h"
#include "sha1.h"
#include "StringOpr.h"
#include "ez_util_platform.h"
#include "str_opr.h"
#include <ez_url_parser.h>

#include "str_opr.h"
#include "ez_bit.h"
#include "md5.h"
#include "sha1.h"
#include "ez_appkey.h" // def_ez_APPKEY_BUFF_LEN ... // used by onReq_ManAppKeyCreate

#include "../../Logs.h"
#include "../../Solar.h"

#include "../UniService/DefUniService.h"

#include "../Pandora/StreamUtility.h" /*工具*/

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveRecordServer.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG
//#ifdef _FUNC_PullTykdMobileService
#include "../StreamPusher/StreamPusherTykd.h"
//#include "../Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
//#endif // _FUNC_PullTykdMobileService

#if defined(_FUNC_TykdMobileService)
#include "../../Configs/ConfigTykdMobileService.h"
#endif

#if defined(_FUNC_gMiniLicense)
#include "../../Maintain/gMiniLicense.h"
#endif//_FUNC_gMiniLicense

#include "HandlerLiveRecordServer.h"
#include "HttpServerLiveRecordServer.h"

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

CHttpServerLiveRecordServer::CHttpServerLiveRecordServer(ISocketHandler& h) : CGRestHttpdSocket(h)//: HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;

    m_iIsAuthedHost = 0;

    //m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    //统一管理
    m_pHandler = ((CHandlerLiveRecordServer *)(&(Handler())));
    assert(m_pHandler);
    m_pCfg = m_pHandler->m_pCfgLiveRecordServer;
    assert(m_pCfg);

    m_strPssSid = "";

#if defined(_USE_MODULES_LocalDataBase)
    m_dbaLiveRecord =  m_pHandler->m_dbaLiveRecord;
    assert(m_dbaLiveRecord);
#endif //_USE_MODULES_LocalDataBase

    m_strRetCode = "200";
}

void CHttpServerLiveRecordServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveRecordServer::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerLiveRecordServer::~CHttpServerLiveRecordServer()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveRecordServer::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerLiveRecordServer::OnAccept()
{
#if defined(_Vendor_TaizhouVC)
    if (GetRemoteAddress()=="11.125.64.68"
        || GetRemoteAddress()=="11.125.64.69"
        )
    {
        SetCloseAndDelete();
        return;
    }
#endif //_Vendor_TaizhouVC

    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());
#ifdef _USE_MODULES_EZCONFIG

    //static CConfigLiveRecordServer __cfg;
    //__cfg.update();
    //m_pCfg = &__cfg;

    m_iTimeout = m_pCfg->getConfig().iTimeout;
#else

    //SetTimeout(5);
    m_iTimeout = HTTPD_CONN_TIMEOUT;
#endif//

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);

    //    m_pStreamContainer =  g_AgentPandoraMatrix.GetHandler()->GetPandoraContainer();
    //    assert(m_pStreamContainer);

    m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

    m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense

    m_strHttpHeader_Server = std::string(def_Pandora_LiveRecordServer_ServerName) + " " + g_Solar.GetVersion() + " " + g_Solar.GetHostID();
}

void CHttpServerLiveRecordServer::OnHeader(const std::string& key,const std::string& value)
{
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

void CHttpServerLiveRecordServer::OnDisconnect()
{
    ez_printf_info("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerLiveRecordServer::Exec()
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    if (CloseAndDelete())
    {
        return ;
    }

    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerLiveRecordServer::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();

    // 短链接，不需要
    m_strBody.clear();// = "";

    // hello不鉴权
    if (GetUrl() == def_UniTrans_Req_hello        
            || GetUrl() == def_UniTrans_Req_ip)
    {
        //ez_printf_debug("on hello\n");

        return;
    }

    std::string strCode = PssMsg_Value_Result_Unknow;
    std::string strMsg = PssMsg_Value_ResultMsg_Unknow;
    std::string retCode = "200";
    do
    {
        if (GetUrl().length() < 2)
        {
            strCode = PssMsg_Value_Result_MethodNotAllowed; 
            strMsg = PssMsg_Value_ResultMsg_MethodNotAllowed;
            retCode = "405"; // https://www.restapitutorial.com/httpstatuscodes.html
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_MethodNotAllowed.c_str());

            break;
        }

        if (m_strPssSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
        {
            ez_printf_error("PssSid not found.\n");
            strCode = PssMsg_Value_Result_NotPermittedAccess; 
            strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
            retCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

            break;
        }

        if (m_pCfg
            && BitGet(m_pCfg->getConfig().iAuth, 0) == 1)
        {
            //ez_printf_debug("-->> do secret key chk\n");

            if (m_strAppKey.length()<1
                || m_strNonce.length()<1
                || m_strCurTime.length()<1
                || m_strCheckSum.length()<1
               )
            {
                ez_printf_error("IllegalAccess:auth data not valid.\n");
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_IllegalAccess;
                retCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

                break;
            }

            //encode("sha1", nonce + appSecret + curTime)
            // todo check appSecret from AppKey
            std::string appSecret ;//= "c48faffaedf5d4cae01c6a21c11476fc118e32b0";

            //int iretGet = m_pHandler->m_pCacheWithDb->getPair(m_strAppKey, appSecret);
            int iretGet = -1;
            if (m_pHandler)
            {
                //iretGet = m_pHandler->m_pCacheWithDb->getPair(m_strAppKey, appSecret);
                iretGet = m_pHandler->getAppKey(m_strAppKey, appSecret);// ->m_pCacheWithDb->getPair(m_strAppKey, appSecret);
            }
            else
            {
                ez_printf_error("m_pHandler null\n");
            }
			
            if (iretGet < 0)
            {
                ez_printf_error("Failed getAppKey(%d):%s\n", iretGet, m_strAppKey.c_str());

                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_IllegalAccess;
                retCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

                break;
            }

            m_strNonce+=appSecret;
            m_strNonce+=m_strCurTime;

            char dst[SHA1HashSize*2+1] = {0};
            const char *src = m_strNonce.c_str();
            ARG_USED(src);
            ARG_USED(dst);

            //ez_printf_debug("ez_sha1_str:%s\n", src);
            int iret = ez_sha1_str(dst, sizeof(dst), src );

            if (iret<0)
            {
                ez_printf_error("IllegalAccess:ez_sha1_str failed.\n");
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);
                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_IllegalAccess;
                retCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

                //RetGeneralMsg(103, "secret data not valid", GetUrl());
                break;
            }

            if (m_strCheckSum != dst)
            {
                ez_printf_warning("auth failed src:[%s], OK_dst:[%s]\n", src, dst);

                //RetGeneralMsg(104, "auth failed", GetUrl());
                //RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);
                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_IllegalAccess;
                retCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html

                break;
            }

            //ez_printf_debug("appSecret Auth Src:%s\n", src);
            //ez_printf_debug("dst:%s\n", dst);
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
                //RetGeneralMsg(103, "host not allow", GetUrl());
                strCode = PssMsg_Value_Result_IllegalAccess; 
                strMsg = PssMsg_Value_ResultMsg_IllegalAccess;
                retCode = "203"; // https://www.restapitutorial.com/httpstatuscodes.html
                break;
            }
        }

        //ok
        return;
    }
    while(0);

    RetGeneralMsg(strCode, strMsg, retCode);
}


/** Chunk of http body data recevied. */
void CHttpServerLiveRecordServer::OnData(const char *p,size_t l)
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
        //LOG4CPLUS_ERROR(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too long");
        //std::string strBadboy = "Bad boy.";

        //SendMsg(strBadboy);

        //m_strBody = "";
        //Reset(); // prepare for next request
        //SetCloseAndDelete();

        ez_printf_error("InvalidData len:%d\n", m_strBody.size()+l);

        RetGeneralMsg(PssMsg_Value_Result_TooMuchData, PssMsg_Value_ResultMsg_TooMuchData, m_strRetCode);

        return;
    }

    m_strBody += pData;

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerLiveRecordServer::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    ez_printf_debug("GetUrl:%s\n", GetUrl().c_str());

    if (GetUrl() == def_UniTrans_Req_ip)
    {
        Do_reqIp(m_strBody);
        return;
    }

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

    //ez_printf_debug("Body:%s\n", m_strBody.c_str());

    if (GetUrl()== def_UniTrans_Req_hello)
    {
        //Do_reqHello(__CezlibsWebServiceIF);
        Do_reqHello(m_strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_Echo)
    {
        //Do_reqEcho(__CezlibsWebServiceIF);
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
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if defined(_FUNC_liveRecordTykd)
    else if (GetUrl() == def_Pandora_Req_LiveRecordTykdStart)
    {
        Do_reqLiveRecordTykdStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_Pandora_Req_LiveRecordTykdStop)
    {
        Do_reqLiveRecordTykdStop(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_Pandora_Req_LiveRecordTykdQuery)
    {
        Do_reqLiveRecordTykdQuery(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_Pandora_Req_LiveRecordTykdDel)
    {
        Do_reqLiveRecordTykdDel(__CezlibsWebServiceIF);
    }
#endif //#if defined(_FUNC_liveRecordTykd)

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl() == def_Pandora_Req_LiveRecordStreamStart)
    {
        Do_reqLiveRecordStreamStart(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_Pandora_Req_LiveRecordStreamStop)
    {
        Do_reqLiveRecordStreamStop(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_Pandora_Req_LiveRecordStreamQuery)
    {
        Do_reqLiveRecordStreamQuery(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_Pandora_Req_LiveRecordStreamDel)
    {
        Do_reqLiveRecordStreamDel(__CezlibsWebServiceIF);
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
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()== def_Pandora_Req_manConfigMod)
    {
        onReq_ManConfigMod(__CezlibsWebServiceIF);
    }
    else
    {
        ez_printf_error("%s:%s\n", PssMsg_Value_ResultMsg_MethodNotAllowed.c_str(), GetUrl().c_str());

        RetGeneralMsg(PssMsg_Value_Result_MethodNotAllowed
			, PssMsg_Value_ResultMsg_MethodNotAllowed+std::string(" : ")+GetUrl()
			, PssMsg_Value_Result_MethodNotAllowed);
    }
}

void CHttpServerLiveRecordServer::Update()
{
    unsigned long ttNow = ez_sec_get_tick_count();//time(NULL);

    if (ttNow-m_ttOndata>m_iTimeout)
    {
        ez_printf_info("Timeout(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());

        SetCloseAndDelete();
        return;
    }
}

size_t CHttpServerLiveRecordServer::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerLiveRecordServer::getMsgNumSent()
{
    return m_iMsgCountSend;
}

void CHttpServerLiveRecordServer::RetGeneralMsg(const std::string &strCode, const std::string &strMsg, const std::string &retCode)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), retCode);
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//#if defined(_FUNC_liveRecordTykd)
int CHttpServerLiveRecordServer::Do_reqLiveRecordTykdStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRecTykdStart __reqMsg;
    CPssMsgRecTykdStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {
        int iret = 0;

        ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.Stream.DevID.c_str());
        ez_printf_info("%s:%s\n",  StreamMsg_Key_Channel.c_str(), __reqMsg.Stream.Channel.c_str());
        ez_printf_info("%s:%s\n", StreamMsg_Key_Stream.c_str(), __reqMsg.Stream.Stream.c_str());
        ez_printf_info("%s:%s\n", StreamMsg_Key_AudioEncode.c_str(), __reqMsg.Stream.AudioEncode.c_str());
        ez_printf_info("%s:%s\n", RecMsg_Key_ValidPeriod.c_str(), __reqMsg.Rec.ValidPeriod.c_str());

        // ID不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl(), m_strRetCode);

            return -1;
        }

        EM_TERMINAL_STATUS_T  Status     ;
        EM_TERMINAL_STATUS_T  UpStatus   ;
        unsigned int UsedSpace  = 0;
        unsigned int UsedUpSpace = 0;

        iret = g_StreamPusherTykd.startRecordTykdMediaUrl( \
                __reqMsg.Stream.DevID
                , __reqMsg.Stream.Channel
                , __reqMsg.Stream.Stream
                , __reqMsg.Rec.FilePeriod
                , __reqMsg.Rec.ValidPeriod
                , __reqMsg.Rec.RecPeriod
                , ezDef_StreamType_tykd, "", "", "", ""
                , __reqMsg.Stream.AudioEncode

                , Status     
                , UpStatus   
                , UsedSpace  
                , UsedUpSpace
                                                         );

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

            // 记录数据
#if defined(_USE_MODULES_LocalDataBase)

            m_dbaLiveRecord->addRecRecord(\
                                          __reqMsg.Stream.DevID+"_"+__reqMsg.Stream.Channel+"_"+__reqMsg.Stream.Stream \
                                          , atol(__reqMsg.Rec.FilePeriod.c_str()) \
                                          , atol(__reqMsg.Rec.ValidPeriod.c_str()) \
                                         );
#endif //_USE_MODULES_LocalDataBase

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
        else //
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveRecordServer::Do_reqLiveRecordTykdStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRecTykdStop __reqMsg;
    CPssMsgRecTykdStopAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {

        ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.Stream.DevID.c_str());
        ez_printf_info("%s:%s\n",  StreamMsg_Key_Channel.c_str(), __reqMsg.Stream.Channel.c_str());
        ez_printf_info("%s:%s\n", StreamMsg_Key_Stream.c_str(), __reqMsg.Stream.Stream.c_str());

        // ID不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl(), m_strRetCode);

            return -1;
        }

        int iret = g_StreamPusherTykd.stopRecordTykdMediaUrl( \
                   __reqMsg.Stream.DevID
                   , __reqMsg.Stream.Channel
                   , __reqMsg.Stream.Stream);

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            ez_printf_error("iret:%d\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-2)
        {
            ez_printf_error("Have been start in 45 sec, not permited to stop.\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else
        {
            // default ret
            ez_printf_error("PssMsg_Value_Result_Unknow\n");

            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveRecordServer::Do_reqLiveRecordTykdQuery( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRecTykdQuery __reqMsg;
    CPssMsgRecTykdQueryAck __ackMsg;
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

        ez_printf_info("%s:%s\n",  PssMsg_Key_ID.c_str(), __reqMsg.Stream.DevID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("%s\n", PssMsg_Value_ResultMsg_PssSidMismatched.c_str());

            __ackMsg.Result = PssMsg_Value_Result_PssSidMismatched;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_PssSidMismatched;

            break;
        }

        //if (iret==0)
        //{

        bool bStarted = g_StreamPusherTykd.isStartedRecordTykdMediaUrl( \
                        __reqMsg.Stream.DevID
                        , __reqMsg.Stream.Channel
                        , __reqMsg.Stream.Stream);

        //是否开始录像
        if (!bStarted)
        {
        }
        else
        {}

        const time_t __ttNow = time(NULL);

        time_t __ttReqStart = 0;
        time_t __ttReqEnd = 0;
        ezConvertFromString(__ttReqStart, __reqMsg.StartDt);
        ezConvertFromString(__ttReqEnd, __reqMsg.EndDt);

        // 3小时前
        if (0 == __ttReqStart)
        {
            __ttReqStart = __ttNow-3*3600;
        }

        if (0 == __ttReqEnd
                ||__ttReqEnd >__ttNow)
        {
            __ttReqEnd = __ttNow;
        }

        if (__ttReqStart < 1378102740/*2013/9/2 14:19:0*/
            ||__ttReqStart > __ttReqEnd
            )
        {
            ez_printf_error("%s:[%ld, %ld]\n", PssMsg_Value_ResultMsg_InvalidData.c_str(), __ttReqStart, __ttReqEnd);

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData + std::string("::") + std::string("__ttReqStart>__ttReqEnd>1378102740");

            break;
        }

        CFileMsg __file;

        int iItems = atoi(__reqMsg.Items.c_str());
        int iPages = atoi(__reqMsg.Pages.c_str());
//#if 1
//#if defined (_FUNC_LiveRecord_Attr_BigMsg) //common 2019-09-20 10:35:59
        if (iItems<1 || iItems>800)
//#else
//        if (iItems<1 || iItems>50)
//#endif
        {
            ez_printf_error("%s:[%d]\n", PssMsg_Value_ResultMsg_InvalidData.c_str(), iItems);

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData + std::string("::") + std::string("iItems(1-800)");

            break;
            //iItems = 1;
        }
        if (iPages<1 || iPages>9999999)
        {
            // 视频上云传递有问题， 暂时这样
#if 1
            ez_printf_error("%s:[%d]\n", PssMsg_Value_ResultMsg_InvalidData.c_str(), iPages);

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData + std::string("::") + std::string("iPages(1-9999999)");

            break;
#endif
            //iPages = 1;
        }
//#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        //文件查找方式
        tdefList_CFileStruct __listRecordFile;

        int iret = g_StreamPusherTykd.queryRecord( \
                   __reqMsg.Stream.DevID
                   , (__reqMsg.Stream.Channel.c_str())
                   , (__reqMsg.Stream.Stream.c_str())
                   , __ttReqStart//atol(__reqMsg.StartDt.c_str())
                   , __ttReqEnd//atol(__reqMsg.EndDt.c_str())
                   , iItems
                   , iPages
                   , __listRecordFile);

        ez_printf_info("queryRecordTykdMediaUrl:%d, %d\n", iret, __listRecordFile.size());
        ez_printf_info("iItems:%d, iPages:%d\n", iItems, iPages);

        if (iret < 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_RecQueryFailed;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_RecQueryFailed;

            ez_printf_error("queryRecord failed:%d\n", iret);

            break;
        }

            int iCurrentItem = 0;
            for (tdefList_CFileStruct::iterator ci = __listRecordFile.begin(); ci != __listRecordFile.end(); ++ci)
            {
                iCurrentItem++;

                // 前
                if (iCurrentItem <= iItems*(iPages-1))
                {
                    ez_printf_info("continue; when iItems:%d, iPages:%d, iCurrentItem:%d\n"
                                   , iItems, iPages, iCurrentItem);
                    continue;
                }

                if (iCurrentItem > iItems*(iPages))
                {
                    ez_printf_info("break; when iItems:%d, iPages:%d, iCurrentItem:%d\n"
                                   , iItems, iPages, iCurrentItem);
                    break;
                }

                __file.Size    = Utility::l2string(ci->iSize);
                __file.StartDt = Utility::l2string(ci->startDt) ;
                __file.EndDt   = Utility::l2string(ci->endDt);

                // g_StreamPusherTykd.makeFileUrl(*ci, __file.Url, __file.SnapUrl);
                __file.Url = ci->strUri;
                __file.SnapUrl = ci->strUriSnap;
                __file.MotionUrl = ci->strUriMotionFile;

                __ackMsg.Files.push_back(__file);
            }

            __ackMsg.TotalItems=__listRecordFile.size();

            __ackMsg.Items=__reqMsg.Items;
            __ackMsg.Pages=__reqMsg.Pages;

            if (!bStarted)
            {
                __ackMsg.Result = PssMsg_Value_Result_RecNotStarted;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_RecNotStarted;
                //break;
            }
            else
            {
                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            }

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    }
    while (0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveRecordServer::Do_reqLiveRecordTykdDel( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRecTykdDel __reqMsg;
    CPssMsgRecTykdDelAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {
        int iret = 0;
        // ID不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl(), m_strRetCode);

            return -1;
        }

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}
//#endif //#if defined(_FUNC_liveRecordTykd)

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

int CHttpServerLiveRecordServer::Do_reqLiveRecordStreamStart( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    CPssMsgRecStreamStart __reqMsg;
    CPssMsgRecStreamStartAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {
        int iret = 0;

        ez_printf_info("%s:%s, %s:%s, %s:%s\n"
                ,  PssMsg_Key_ID.c_str(), __reqMsg.ID.c_str()
                ,  StandStreamMsg_Key_Channel.c_str(), __reqMsg.StandStream.Channel.c_str()
                , StandStreamMsg_Key_Subtype.c_str(), __reqMsg.StandStream.Subtype.c_str());

        // ID不匹配
        if (m_strPssSid != __reqMsg.ID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl(), m_strRetCode);

            return -1;
        }
#if defined(_Vendor_TaizhouVC)
    if (__reqMsg.ID !="012307010505297314"    //电梯A
        && __reqMsg.ID != "012307010505297312" //电信大楼西大门 
        && __reqMsg.ID != "012307010505297310" //电信大楼南大门 
        && __reqMsg.ID != "012307010505297309" //电信大楼北大门 
        && __reqMsg.ID != "012307010505297307" //电信大楼东大门 
        )
    {
        ez_printf_error("_Vendor_TaizhouVC, Unsupported id:%s\n",  __reqMsg.ID.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_DevIDNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_DevIDNotValid;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl(), m_strRetCode);
        return -2;
    }
#endif //_Vendor_TaizhouVC

        int iret_chkIDValid = CStreamUtility::ez_devid_valid(__reqMsg.ID.c_str());
        if (iret_chkIDValid <= 0)
        {
            ez_printf_error("%s\n", PssMsg_ResultMsg_LiveStreamming_DevIDNotValid.c_str());

            __ackMsg.Result = PssMsg_Result_LiveStreamming_DevIDNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_LiveStreamming_DevIDNotValid;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl(), m_strRetCode);

            return -1;
        }

        int iType = atoi(__reqMsg.Type.c_str());
        ARG_USED(iType);

        EM_TERMINAL_STATUS_T  Status     = em_terminal_status_unknown;
        EM_TERMINAL_STATUS_T  UpStatus = em_terminal_status_unknown;
        unsigned int UsedSpace    = 0;
        unsigned int UsedUpSpace = 0;

        iret = g_StreamPusherTykd.startRecordTykdMediaUrl( \
                __reqMsg.ID
                , __reqMsg.StandStream.Channel
                , __reqMsg.StandStream.Subtype
                , __reqMsg.Rec.FilePeriod
                , __reqMsg.Rec.ValidPeriod
                , __reqMsg.Rec.RecPeriod

                , atoi(__reqMsg.Type.c_str())
                , __reqMsg.StandStream.Url
                , __reqMsg.StandStream.Account
                , __reqMsg.StandStream.Passwd
                , __reqMsg.StandStream.VideoEncode
                , __reqMsg.StandStream.AudioEncode

                , Status
                , UpStatus
                , UsedSpace
                , UsedUpSpace
                                                         );
        __ackMsg.Status = ezConvertToString(Status==em_terminal_status_online) ;
        __ackMsg.UpStatus = ezConvertToString(UpStatus==em_terminal_status_online) ;
        __ackMsg.UsedSpace = UsedSpace ;
        __ackMsg.UsedUpSpace = UsedUpSpace ;

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

            // 记录数据
#if defined(_USE_MODULES_LocalDataBase__)

            m_dbaLiveRecord->addRecRecord(\
                                          __reqMsg.ID+"_"+__reqMsg.StandStream.Channel+"_"+__reqMsg.StandStream.Subtype \
                                          , atol(__reqMsg.Rec.FilePeriod.c_str()) \
                                          , atol(__reqMsg.Rec.ValidPeriod.c_str()) \
                                         );
#endif //_USE_MODULES_LocalDataBase

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
        else //
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerLiveRecordServer::Do_reqLiveRecordStreamStop( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

#if 1

    return this->Do_reqLiveRecordTykdStop(JsonData);
#else

    CPssMsgRecTykdStop __reqMsg;
    CPssMsgRecTykdStopAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {

        ez_printf_info("%s:%s\n",  StreamMsg_Key_DevID.c_str(), __reqMsg.Stream.DevID.c_str());
        ez_printf_info("%s:%s\n",  StreamMsg_Key_Channel.c_str(), __reqMsg.Stream.Channel.c_str());
        ez_printf_info("%s:%s\n", StreamMsg_Key_Stream.c_str(), __reqMsg.Stream.Stream.c_str());

        // ID不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl());

            return -1;
        }

        int iret = g_StreamPusherTykd.stopRecordTykdMediaUrl( \
                   __reqMsg.Stream.DevID
                   , __reqMsg.Stream.Channel
                   , __reqMsg.Stream.Stream);

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            ez_printf_error("iret:%d\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else
        {
            // default ret
            ez_printf_error("iret:%d\n", iret);

            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
#endif //

    return 0;
}

int CHttpServerLiveRecordServer::Do_reqLiveRecordStreamQuery( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    return this->Do_reqLiveRecordTykdQuery(JsonData);
#if 0

    CPssMsgRecTykdQuery __reqMsg;
    CPssMsgRecTykdQueryAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {
        int iret = 0;

        // ID不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl());

            return -1;
        }

        if (iret==0)
        {

            bool bStarted = g_StreamPusherTykd.isStartedRecordTykdMediaUrl( \
                            __reqMsg.Stream.DevID
                            , __reqMsg.Stream.Channel
                            , __reqMsg.Stream.Stream);
            if (!bStarted)
            {
                __ackMsg.Result = 204;
                __ackMsg.Reason = "Record not started.";
            }

            if (__reqMsg.StartDt.length() < 9)
            {
                __reqMsg.StartDt = def_MIN_TIME_T_VALUE_STR/*1978-1-1 0:0:0*/;
            }

            if (__reqMsg.EndDt.length() < 9)
            {
                __reqMsg.EndDt = def_MAX_TIME_T_VALUE_STR;
            }

#if 0
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
#else
            // 消息调试用
            CFileMsg __file;
            tdef_RecordList __listRecordFile;

            int iItems = atoi(__reqMsg.Items.c_str());
            int iPages = atoi(__reqMsg.Pages.c_str());
            if (iItems<1 || iItems>50)
            {
                iItems = 1;
            }
            if (iPages<1 || iPages>9999999)
            {
                iPages = 1;
            }

            int iret = g_StreamPusherTykd.queryRecordTykdMediaUrl( \
                       __reqMsg.Stream.DevID
                       , __reqMsg.Stream.Channel
                       , __reqMsg.Stream.Stream
                       , atol(__reqMsg.StartDt.c_str())
                       , atol(__reqMsg.EndDt.c_str())
                       , iItems
                       , iPages
                       , __listRecordFile);

            ez_printf_info("queryRecordTykdMediaUrl:%d, %d\n", iret, __listRecordFile.size());
            ez_printf_info("iItems:%d, iPages:%d\n", iItems, iPages);

            if (iret >= 0)
            {
                int iCurrentItem = 0;
                for (tdef_RecordList::iterator ci = __listRecordFile.begin(); ci != __listRecordFile.end(); ++ci)
                {
                    iCurrentItem++;

                    // 前
                    if (iCurrentItem <= iItems*(iPages-1))
                    {
                        ez_printf_info("continue; when iItems:%d, iPages:%d, iCurrentItem:%d\n"
                                       , iItems, iPages, iCurrentItem);
                        continue;
                    }

                    if (iCurrentItem > iItems*(iPages))
                    {
                        ez_printf_info("break; when iItems:%d, iPages:%d, iCurrentItem:%d\n"
                                       , iItems, iPages, iCurrentItem);
                        break;
                    }

                    __file.Size    = Utility::l2string(ci->iSize);
                    __file.StartDt = Utility::l2string(ci->startDt) ;
                    __file.EndDt   = Utility::l2string(ci->endDt);
                    //__file.SnapUrl = Utility::l2string(ci->endDt);

                    //__file.Url = ci->strPath+"/"+ci->strName;
                    g_StreamPusherTykd.makeFileUrl(*ci, __file.Url, __file.SnapUrl);

                    //__ackMsg.Files.push_front(__file);
                    __ackMsg.Files.push_back(__file);
                }

                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

                __ackMsg.Items=__reqMsg.Items;
                __ackMsg.Pages=__reqMsg.Pages;
                __ackMsg.TotalItems=__listRecordFile.size();
            }
            else
            {
                __ackMsg.Result = 205;
                __ackMsg.Reason = "File not found.";
            }
#endif

        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
#endif

    return 0;
}

int CHttpServerLiveRecordServer::Do_reqLiveRecordStreamDel( CPssStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    return this->Do_reqLiveRecordTykdDel(JsonData);
#if 0

    CPssMsgRecTykdDel __reqMsg;
    CPssMsgRecTykdDelAck __ackMsg;
    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    if (JsonData.Decode(__reqMsg))
    {
        int iret = 0;
        // ID不匹配
        if (m_strPssSid != __reqMsg.Stream.DevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            CPssStack __err_stack;
            __err_stack.Encode(__ackMsg);

            //
            SendMsg(__err_stack.ToString(), GetUrl());

            return -1;
        }

        if (iret==0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
    }
    else
    {
        ez_printf_error("%s\n", "JsonData.Decode");

        __ackMsg.Result = PssMsg_Value_Result_InvalidData;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
    }

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
#endif

    return 0;
}

//
int CHttpServerLiveRecordServer::onReq_ManAppKeyCreate( CPssStack &JsonData)
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
        if (m_pCfg
            && m_pCfg->getConfig().iAccounts == 1)
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
int CHttpServerLiveRecordServer::onReq_ManAppKeyCancel( CPssStack &JsonData)
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
int CHttpServerLiveRecordServer::onReq_ManConfigMod( CPssStack &JsonData)
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
            CConfigTykdMobileService __cfgTykdMobileService;
            __cfgTykdMobileService.update();

            ConfigTykdMobileService &__tCfg = __cfgTykdMobileService.getConfig();
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
		
            CConfigTykdMobileService __cfgTykdMobileService;
            __cfgTykdMobileService.update();

            ConfigTykdMobileService &__tCfg = __cfgTykdMobileService.getConfig();

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

//example
#if 0
//
int CHttpServerLiveRecordServer::onReq_ManConfigMod( CPssStack &JsonData)
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

        ez_printf_debug("req:\n%s\n", m_strBody.c_str());
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    //ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    return 0;
}

#endif
