/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerNvpRegisterServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerNvpRegisterServer.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
 *
 *  Explain:
 *     -
 *       ifuse onReq_NvpTRegister , "AuthType" : 0,
 *     -
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
#include <ez_socket.h>

#include "../../Solar.h"

#include "../Pandora/PandoraDef.h"

#include "../Pandora/StreamUtility.h" /*工具*/
#include "../Pandora/StreamContainer.h" /*工具*/

#include "HttpServerNvpRegisterServer.h"
#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveCast.h"
#include "../../Configs/ConfigNvpRegisterServer.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

#include "AgentNvpRegisterServer.h"
#include "HandlerNvpRegisterServer.h"

#ifdef _FUNC_NvpRegisterServer_TO_MYSQL_DB
                // 添加设备- 操作数据库
#include "mysql_addTerminal.h"
#endif // _FUNC_NvpRegisterServer_TO_MYSQL_DB
#ifdef _FUNC_InterVideo_TO_MYSQL_DB
                // 添加user- 操作数据库
#include "mysql_addUser.h"
#endif // _FUNC_InterVideo_TO_MYSQL_DB

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

const std::string PssMsg_Value_Result_InvalidDevID = "501"; // 数据错误
const std::string PssMsg_Value_ResultMsg_InvalidDevID = "Invalid DevID";

const std::string PssMsg_Value_Result_InvalidDevType = "502"; // 不支持的设备类型
const std::string PssMsg_Value_ResultMsg_InvalidDevType = "Invalid terminal type";

CHttpServerNvpRegisterServer::CHttpServerNvpRegisterServer(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    //m_iMsgCountGet = 0;
    //m_iMsgCountSend = 0;

    m_iIsAuthedHost = 0;

    m_pCfg = NULL;

    m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    m_strPssSid = "";
    m_strSetCookie = "";

    SetDeleteByHandler();
    //SetSockName(strSocketName);
    //DisableInputBuffer();

}

void CHttpServerNvpRegisterServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerNvpRegisterServer::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerNvpRegisterServer::~CHttpServerNvpRegisterServer()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerNvpRegisterServer::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerNvpRegisterServer::OnAccept()
{
    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());

    // 必须开放config
    //#if defined(_FUNC_NvpRegisterServer)
    //
    //    ConfigLiveCast *pCfgCast = g_AgentNvpRegisterServer.getConfigLiveCast();
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

    static CConfigNvpRegisterServer __cfg;
    __cfg.update();
    m_pCfg = &__cfg;

    m_iTimeout = m_pCfg->getConfig().iTimeout;

    //m_iCfgRandChannelID = __cfg.getConfig().iRandChannalID;
#else

    //SetTimeout(5);
    m_iTimeout = HTTPD_CONN_TIMEOUT;
    // use default value
    m_iCfgRandChannelID = 0
#endif//

    m_pContainerChannel = g_AgentNvpRegisterServer.GetHandler()->GetPandoraContainer();

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);

    m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

    m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense
}

void CHttpServerNvpRegisterServer::OnHeader(const std::string& key,const std::string& value)
{
    // 无认证相关信息
#if 1
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
#endif

    {
        HttpdSocket::OnHeader(key, value);
    }
}

void CHttpServerNvpRegisterServer::OnDisconnect()
{
    ez_printf_debug("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerNvpRegisterServer::Exec()
{
    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerNvpRegisterServer::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();

    m_strBody = "";

    // 这个服务不鉴权
#if 1 // 使用app鉴权

    do
    {

        if (m_pCfg
            && BitGet(m_pCfg->getConfig().iAuth, 0) == 1)
        {
            ez_printf_debug("-->> do secret key chk\n");

            if (m_strPssSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
            {
                ez_printf_error("PssSid not found, req:%s\n", +GetUrl().c_str());

                RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);

                break;
            }

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
                //appSecret = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
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

void CHttpServerNvpRegisterServer::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
    AddResponseHeader("Server", std::string(def_Pandora_NvpRegisterServer_ServerName)+g_Solar.GetVersion());
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

    if (m_strSetCookie.size()>2)
    {
        AddResponseHeader( "Set-Cookie", m_strSetCookie);
    }

    SendResponse();
}

void CHttpServerNvpRegisterServer::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
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

void CHttpServerNvpRegisterServer::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}


/** Chunk of http body data recevied. */
void CHttpServerNvpRegisterServer::OnData(const char *p,size_t l)
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

void CHttpServerNvpRegisterServer::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    ez_printf_debug("GetUrl:%s\n", GetUrl().c_str());
    ez_printf_debug("Body:%s\n", m_strBody.c_str());

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 终端注册消息
    if (GetUrl()== def_Pandora_Req_NvpTermRegister)
    {
        onReq_NvpTermRegister(m_strBody);
    }
    else if (GetUrl()== def_Pandora_Req_NvpTermInfo)
    {
        onReq_NvpTermInfo(m_strBody);
    }
    // 接管现有注册接口OK
    else if (strstr(GetUrl().c_str(), def_Pandora_Req_NvpTRegister))
    {
        onReq_NvpTRegister(m_strBody);
    }
    //
    // 账号
    //
    else if (GetUrl()==def_Pandora_Req_NvpAccountCreate)
    {
        onReq_NvpAccountCreate(m_strBody);
    }
    else if (GetUrl()==def_Pandora_Req_NvpAccountMod)
    {
        onReq_NvpAccountMod(m_strBody);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_TestIF(m_strBody);
    }
    else
    {
        RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
    }

    // 是否关闭等交由各个处理函数而定
    // SetCloseAndDelete();
    // m_strBody = "";
}

void CHttpServerNvpRegisterServer::Update()
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

void CHttpServerNvpRegisterServer::OnRegistered(int iCode, void *pData)
{
    return;
}

// http + json
int CHttpServerNvpRegisterServer::SendMsg(const std::string &strMsg, const std::string &matchRequest)
{
    DBG(
        std::cout << "strMsg.size() :" <<  strMsg.size()  << std::endl;
        std::cout << "strMsg :" <<  strMsg  << std::endl;
    );
    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    //Reset();
    //m_strBody = "";
    //m_iMsgCountSend++;

    // 短连接
    SetCloseAndDelete();

    return 0;
}

// curl http://localhost:18090/hello -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerNvpRegisterServer::Do_TestIF( const std::string &strReq)
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

int CHttpServerNvpRegisterServer::onReq_NvpTRegister(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    std::stringstream __ss;

    char strBody[4096];
    char strSession[4096];

    int iret = ez_nvp_device_reg_2t(strReq.c_str(), strSession, strBody);
    if (iret ==0 )
    {
        m_strSetCookie = strSession;
        __ss << strBody;
    }
    else
    {
        __ss \
        << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><REGISTER_RES><Result>0</Result><DevID>812A930E4BC</DevID><NatIP>202.102.108.42</NatIP><NatPort>8588</NatPort><BackupNatIP>202.102.108.48</BackupNatIP><BackupNatPort>8588</BackupNatPort><TurnIP>202.102.108.137</TurnIP><TurnPort>3478</TurnPort><BackupTurnIP>61.160.149.44</BackupTurnIP><BackupTurnPort>3478</BackupTurnPort><FtpIP>61.160.137.55</FtpIP><FtpPort>8321</FtpPort><FtpAccount>alarm</FtpAccount><FtpPassword>tykd!@#</FtpPassword><Time>20170507095652</Time><DMSUrl>202.102.108.30:9084</DMSUrl><DMSType>1</DMSType><DMSEnableFlag>1</DMSEnableFlag><CentralStorageURL>58.210.241.82:9500</CentralStorageURL><CentralStorageFlag>1</CentralStorageFlag></REGISTER_RES>" \
        ;
    }
    ez_printf_debug("SendMsg:%s.\n", __ss.str().c_str());

    SendMsg(__ss.str(), GetUrl());

    return 0;
}

int CHttpServerNvpRegisterServer::onReq_NvpTermRegister(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPssMsgNvpRegister __reqMsg;
    CPssMsgNvpRegisterAck __ackMsg;

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

        // 名字不匹配
        if (m_strPssSid != __reqMsg.DevID)
        {
            ez_printf_error("m_strPssSid(%s) != __reqMsg.DevID(%s)\n", m_strPssSid.c_str(), __reqMsg.DevID.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        int iTermType = atoi(__reqMsg.Type.c_str());

        if (1 == iTermType)
        {
            // 名字不匹配
            //if (__reqMsg.DevID.length()==11 && strncmp(__reqMsg.DevID.c_str(), "811", 3) !=0 )
            if (__reqMsg.DevID.length() != 11) // 不检查前置参数
            {

                iTermType = 1;
                ez_printf_error("__reqMsg.DevID(%s)-Len(%d), not valid\n", __reqMsg.DevID.c_str(), __reqMsg.DevID.length());

                __ackMsg.Result = PssMsg_Value_Result_InvalidDevID;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDevID;

                break;
            }

            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

            int iret = 0;
            // todo 此处记录

            // 这里没有记录，去别处添加
            if (iret == 0)
            {
#ifdef _FUNC_NvpRegisterServer_TO_WS
                // 添加设备 - 调用ws接口
                iret = ez_nvp_add_device(__reqMsg.DevID.c_str(), __reqMsg.DevKey.c_str());
                ez_printf_info("ez_nvp_add_device ret : %d.\n", iret);
#endif // _FUNC_NvpRegisterServer_TO_WS

#ifdef _FUNC_NvpRegisterServer_TO_MYSQL_DB
                // 添加设备- 操作数据库
                iret = pst1_mysql_addTerminal(__reqMsg.DevID.c_str(), __reqMsg.DevKey.c_str(), __reqMsg.Module.c_str());
                ez_printf_info("pst1_mysql_addTerminal ret : %d.\n", iret);
#endif // _FUNC_NvpRegisterServer_TO_MYSQL_DB
            }

            if (iret==0)
            {
                __ackMsg.Result = PssMsg_Value_Result_Success;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            }
            else if (iret==-4)
            {
                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;
                __ackMsg.UrlDataSrv = "turn://dts1.ezlibs.com:18080";
                __ackMsg.UrlCommSrv = "http://nvps2.ezlibs.com:18080";

            }
            else if (iret<0)
            {
                __ackMsg.Result = PssMsg_Value_Result_SysBusy;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
            }
#if 0 // 没有这种情况
            else if (iret>0)
            {
                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;
                __ackMsg.UrlDataSrv = "turn://dts1.ezlibs.com:18080";
                __ackMsg.UrlCommSrv = "http://nvps2.ezlibs.com:18080";

            }
#endif
            else
            {
                __ackMsg.Result = PssMsg_Value_Result_Unknow;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

                //__ackMsg.UrlDataSrv = "turn://dts1.ezlibs.com:18080";
                //__ackMsg.UrlCommSrv = "http://nvps2.ezlibs.com:18080";
            }

#if 0
            //#if defined(_USE_MODULES_LocalDataBase)
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
        // 手机
        else if (2==iTermType)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            // 返回广告
            // file 1
            {
                CAdsFileMsg __file;

                __file.No      = "1";
                __file.Url      = "http://61.160.149.164:18080/ads/guide0.png";
                __file.Type     = "1";
                __file.Sha1     = "f555888d62b2d6210733eea52c0eda90c5db5c7a";
                __file.Length   = "667901";
                __file.Delay    = "3";
                __file.StartDt  = "1495014080";
                __file.EndDt    = "2493971369";

                __ackMsg.AdsFiles.push_back(__file);
            }
            // file 2
            {
                CAdsFileMsg __file;

                __file.No      = "2";
                __file.Url      = "http://61.160.149.164:18080/ads/guide1.png";
                __file.Sha1     = "9bc6755c231a6ca2a5da4ffa56ade809812c917e";
                __file.Type     = "1";
                __file.Length   = "389899";
                __file.Delay    = "3";
                __file.StartDt  = "1495014080";
                __file.EndDt    = "2493971369";

                __ackMsg.AdsFiles.push_back(__file);
            }
            // file 3
            {
                CAdsFileMsg __file;

                __file.No      = "3";
                __file.Url      = "http://61.160.149.164:18080/ads/guide2.png";
                __file.Sha1     = "d536a68bd9fe7874ae63168225f70bd3d3a26450";
                __file.Type     = "1";
                __file.Length   = "480161";
                __file.Delay    = "3";
                __file.StartDt  = "1495014080";
                __file.EndDt    = "2493971369";

                __ackMsg.AdsFiles.push_back(__file);
            }

            break;
        }

        else // 暂不支持的设备
        {
            ez_printf_error("InvalidDevType.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDevType;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDevType;

            break;

        }// type
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), def_Pandora_Match_pssChannelCreate);

    return 0;
}

int CHttpServerNvpRegisterServer::onReq_NvpTermInfo(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPssMsgNvpTermInfo __reqMsg;
    CPssMsgNvpTermInfoAck __ackMsg;

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

        // 名字不匹配
        if (m_strPssSid != __reqMsg.DevID)
        {
            ez_printf_error("m_strPssSid(%s) != __reqMsg.DevID(%s)\n", m_strPssSid.c_str(), __reqMsg.DevID.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        ez_printf_info("DevID:%s, DevKey:%s\n", __reqMsg.DevID.c_str(), __reqMsg.DevKey.c_str());

        int iret;
        int TermOnline = -1;
        int TermBinded = -1;
        // 不存在
        iret = pst1_mysql_getTerminalInfo(__reqMsg.DevID.c_str(), __reqMsg.DevKey.c_str(), &TermOnline, &TermBinded);
        ez_printf_info("pst1_mysql_getTerminalInfo:%d, %d\n", iret, TermOnline);

        __ackMsg.Online = ezConvertToString(TermOnline);
        __ackMsg.Binded = ezConvertToString(TermBinded);
        
        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), def_Pandora_Match_pssChannelCreate);

    return 0;
}

// 
// 账号管理
//
int CHttpServerNvpRegisterServer::onReq_NvpAccountCreate(const std::string &strReq)
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, strReq.c_str());

    CPssStack __stackReq;
    CPssMsgNvpAccouontCreate __reqMsg;
    CPssMsgNvpAccouontCreateAck __ackMsg;

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
        if (m_strPssSid != __reqMsg.MP)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }


        std::string strMobilePhone = __reqMsg.MP;
        int iret=0;
#ifdef _FUNC_InterVideo_TO_WS
        iret = ez_nvp_add_account(strMobilePhone.c_str());
        ez_printf_info("ez_nvp_add_account ret : %d.\n", iret);
#endif //_FUNC_InterVideo_TO_WS

#ifdef _FUNC_InterVideo_TO_MYSQL_DB
                // 添加设备- 操作数据库
        iret = pst1_mysql_addUser(strMobilePhone.c_str(), "pwd", "suzhou");
        ez_printf_info("pst1_mysql_addUser ret : %d.\n", iret);
#endif // _FUNC_InterVideo_TO_MYSQL_DB

        if (iret>=0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            //__ackMsg.Result = PssMsg_Value_Result_SysBusy;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else if (iret==-4)
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;

        }
        else //
        {
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}


int CHttpServerNvpRegisterServer::onReq_NvpAccountMod( const std::string &strReq) 
{
    ez_printf_info("%s -- %s\n", __FUNCTION__, m_strBody.c_str());

    CPssStack __stackReq;
    CPssMsgNvpAccouontMod __reqMsg;
    CAckMsgSimple __ackMsg;

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
        }

        // 名字不匹配
        if (m_strPssSid != __reqMsg.LoginName)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        ez_printf_debug("LoginName:%s\n", __reqMsg.LoginName.c_str());
        ez_printf_debug("LoginName:%s\n", __reqMsg.Password.c_str());

        int iret =  pst1_mysql_modUser(__reqMsg.LoginName.c_str(), __reqMsg.Password.c_str());
        //int iret =  0;

        if (iret<0)
        {
            __ackMsg.Result = PssMsg_Value_Result_SysBusy;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_SysBusy;
        }
        else if (iret>0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;

        }
        else // ==0
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }

    }
    while(0);

    CPssStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("SendMsg:%s.\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}

