/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerInterVideo.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerInterVideo.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#include "ez_util_platform.h"

#include "ez_socket.h"
#include "StringOpr.h"

#include "../../Solar.h"

#include "../UniService/DefUniService.h"

#include "../../Logs.h"

#include "AgentInterVideo.h"
#include "HandlerInterVideo.h"

#include "HttpServerInterVideo.h"

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



CHttpServerInterVideo::CHttpServerInterVideo(ISocketHandler& h) : HttpdSocket(h)
{
    m_pHandler = ((CHandlerInterVideo*)(&(Handler())));

    m_strBody = "";
    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iIsAuthedHost = 0;

    m_pCfg = NULL;

    m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    m_strPssSid = "";
}

void CHttpServerInterVideo::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerInterVideo::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerInterVideo::~CHttpServerInterVideo()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerInterVideo::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerInterVideo::OnAccept()
{
    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());

    // 必须开放config
    //#if defined(_FUNC_InterVideo)
    //
    //    ConfigLiveCast *pCfgCast = g_AgentInterVideo.getConfigLiveCast();
    //
    //    if (pCfgCast)
    //    {
    //        m_strPubPushRTMP = pCfgCast->strPubPushRTMP;
    //        m_strPubPushRTSP = pCfgCast->strPubPushRTSP;
    //
    //        m_strUrlPrefixRTMP = pCfgCast->strDefaultUrlPrefixRTMP;
    //        m_strUrlPrefixRTSP = pCfgCast->strDefaultUrlPrefixRTSP;
    //        m_strUrlPrefixHLS = pCfgCast->strDefaultUrlPrefixHLS;
    //    }
    //#else //_USE_MODULES_EZCONFIG
    //
    //#endif //_USE_MODULES_EZCONFIG

#ifdef _USE_MODULES_EZCONFIG

    static CConfigInterVideo __cfg;

    __cfg.update();
    m_pCfg = &__cfg;

    m_iTimeout = m_pCfg->getConfig().iTimeout;

#else

    m_iTimeout = HTTPD_CONN_TIMEOUT;
#endif//

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);

    m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

    m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense
}

void CHttpServerInterVideo::OnHeader(const std::string& key,const std::string& value)
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

void CHttpServerInterVideo::OnDisconnect()
{
    ez_printf_debug("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerInterVideo::Exec()
{
    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerInterVideo::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();

    m_strBody = "";


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
            std::string appSecret ;//= "c48faffaedf5d4cae01c6a21c11476fc118e32b0";

            if (m_strAppKey == "701818788")
            {
                appSecret = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
            }
            else if (m_strAppKey == "701918788")
            {
                appSecret = "9cba8e3fe970ddbca35424db80197a0167d68813";
            }
            else
            {
                ez_printf_error("unknow: AppKey.\n");
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                break;
            }

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

void CHttpServerInterVideo::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
    AddResponseHeader("Server", std::string(def_UniService_InterVideo_ServerName)+g_Solar.GetVersion());
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

void CHttpServerInterVideo::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
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


    CInterVideoStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    //SendMsg(__ss.str(), matchRequest);
}

void CHttpServerInterVideo::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CInterVideoStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}


/** Chunk of http body data recevied. */
void CHttpServerInterVideo::OnData(const char *p,size_t l)
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

        return;
    }

    m_strBody += pData;

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerInterVideo::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#if 0//def _USE_MODULES_EZCONFIG
    CConfigInterVideo __cfg;
    __cfg.update();
#endif//

    CInterVideoStack __CezlibsWebServiceIF;

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
            // m_iMsgCountGet ++;
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
    if (GetUrl()==def_InterVideo_Req_DeviceAdd)
    {
        On_Req_InterVideo_DeviceAdd(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_InterVideo_Req_DeviceCancel)
    {
        //onReq_NvpAccountMod(__CezlibsWebServiceIF);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    else
    {
        RetGeneralMsg(100, "Unknow URL:"+GetUrl(), GetUrl());
    }

    Reset(); // prepare for next request
    SetCloseAndDelete();
    m_strBody = "";
}

void CHttpServerInterVideo::Update()
{
    unsigned long ttNow = ez_sec_get_tick_count();//time(NULL);

#if 1

    DBG(
        std::cout << ttNow-m_ttOndata << " tout:" << m_iTimeout << std::endl;
    );
    if (ttNow-m_ttOndata>m_iTimeout)
    {
        ez_printf_info("idle Timeout(%d), conn:%d\n", GetSocket(), m_iTimeout)

        SetCloseAndDelete();
        return;
    }
#endif
}

// http + json
int CHttpServerInterVideo::SendMsg(const std::string &strMsg, const std::string &matchRequest)
{
    DBG(
        std::cout << "strMsg.size() :" <<  strMsg.size()  << std::endl;
        std::cout << "strMsg :" <<  strMsg  << std::endl;
    );

    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    // 短连接
    SetCloseAndDelete();

    return 0;
}

// curl http://localhost:18090/hello -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerInterVideo::Do_TestIF( CInterVideoStack &JsonData)
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

    RetGeneralMsg(0, "Hello, Your URL:[" + GetUrl() + "], Me:" + __cfg.getConfig().strCode);
#endif//

    return 0;
}

//#include "../../Probe/Probe.h"
int CHttpServerInterVideo::On_Req_InterVideo_DeviceAdd( CInterVideoStack &restStack)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CInterVideoMsg_addDevice __reqMsg;
    CInterVideoMsg_addDeviceAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {
        if (!restStack.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  InterVideoMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strDevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        std::string __strAccount    = __reqMsg.m_strAccount    ;
        std::string __strPasswd     = __reqMsg.m_strPasswd     ;
        std::string __strManufacture= __reqMsg.m_strManufacture;
        std::string __strModel      = __reqMsg.m_strModel      ;
        std::string __strChannelNum = __reqMsg.m_strChannelNum ;
        std::string __strHostIp     = __reqMsg.m_strHostIp     ;
        std::string __strHttpPort   = __reqMsg.m_strHttpPort   ;
        std::string __strRtspPort   = __reqMsg.m_strRtspPort   ;
        std::string __strTcpPort    = __reqMsg.m_strTcpPort    ;

        if (__strAccount    .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_Account.c_str()    );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_Account    ;
            break;
        }

        if (__strPasswd     .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_Passwd.c_str()     );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_Passwd     ;
            break;
        }

        if (__strManufacture.length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_Manufacture.c_str());
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_Manufacture;
            break;
        }

        if (__strModel      .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_Model.c_str()      );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_Model      ;
            break;
        }

        if (__strChannelNum.length() < 1)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_ChannelNum.c_str());
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_ChannelNum;
            break;
        }

        if (__strHostIp     .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_HostIp.c_str()     );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_HostIp     ;
            break;
        }

        if (__strHttpPort   .length() < 1)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_HttpPort.c_str()   );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_HttpPort   ;
            break;
        }

        if (__strRtspPort   .length() < 1)
        {
            ez_printf_error("InvalidParameter:%s.\n", InterVideoMsg_Key_RtspPort.c_str()   );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + InterVideoMsg_Key_RtspPort   ;
            break;
        }
#if 0
        if (__strTcpPort    .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_TcpPort    );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string("::") + def_Psso_ValueKey_TcpPort    ;
            break;
        }
#endif
        //int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        __ackMsg.m_strDevID = __reqMsg.m_strDevID;
#if 0

        if (__reqMsg.m_strDevID != "18115886801")
        {
            ez_printf_error("Err:%s.\n", PssMsg_Value_ResultMsg_DeviceOffline.c_str()   );

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }
#endif
        int iManufacture = 0;
        if (def_InterVideo_Value_Manufacture_HIKVISION == __strManufacture)
        {
            iManufacture = ezDef_StreamType_IPC_hikvision;
        }
        else if (def_InterVideo_Value_Manufacture_DAHUA == __strManufacture)
        {
            iManufacture = ezDef_StreamType_IPC_dahua;
        }
        else if (def_InterVideo_Value_Manufacture_TIANDY == __strManufacture)
        {
            iManufacture = ezDef_StreamType_IPC_tdwy;
        }
        else
        {
            ez_printf_error("%s.\n", PssMsg_Value_ResultMsg_UnSupportedRequest.c_str()   );

            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest + ":" + __strManufacture;

            break;
        }
        ARG_USED(iManufacture);

        std::string strUrl = "rtsp://"+__strHostIp+":"+__strRtspPort;

        int __iChannelNum;
        ezConvertFromString(__iChannelNum, __strChannelNum);

        // 在循环后面还使用它来检查是否在线
        char        pPlayUrl[1024] = {'\0'};

        for (int ii=1; ii<__iChannelNum+1; ii++)
        {
            const char *pSubtype = "1";

            std::string __strChnN;
            __strChnN = ezConvertToString(ii);

            int ret_ez_getStreamPlayUrl = ez_getStreamPlayUrl(iManufacture, strUrl.c_str(), __strAccount.c_str(), __strPasswd.c_str(), __strChnN.c_str(), pSubtype , pPlayUrl, sizeof(pPlayUrl)-1);

            if (0 == ret_ez_getStreamPlayUrl)
            {
            // url
            }
            else
            {
                pPlayUrl[0] = '\0';

                ez_printf_error("ez_getStreamPlayUrl:%d\n", ret_ez_getStreamPlayUrl);
            }
        }

        //g_Probe.addPeer(__reqMsg.m_strDevID, pPlayUrl);

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        //Value.m_strChannel = "";
        //Value.m_strType = def_Psso_ValueKey_DevStatus;
        //Value.m_strValue = def_Psso_Value_Status_Unknown;
        //__ackMsg.Values.push_back(Value);

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    }
    while(0);

    CInterVideoStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}

int CHttpServerInterVideo::On_Req_InterVideo_DeviceCancel( CInterVideoStack &restStack)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CInterVideoMsg_addDevice __reqMsg;
    CInterVideoMsg_addDeviceAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Success;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

    do
    {
        if (!restStack.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  InterVideoMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strDevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        int iret = 0;//g_Probe.delPeer(__reqMsg.m_strDevID);
        if (iret < 0)
        {
            ez_printf_error("%s not exists.\n", __reqMsg.m_strDevID.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;

            break;
        }

    }
    while(0);

    CInterVideoStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}
int CHttpServerInterVideo::On_Req_InterVideo_ValueGet( CInterVideoStack &restStack)
{
    return 0;
}

int CHttpServerInterVideo::On_Req_InterVideo_ValueSet( CInterVideoStack &restStack)
{
    return 0;
}

