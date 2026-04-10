/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerKeyManagementService.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: HttpServerKeyManagementService.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:34  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "base64.h"
#include "str_opr.h"
#include "ez_bit.h"
#include "sha1.h"
#include "ez_util_platform.h"

#include "Compression.h"

#include "ez_socket.h"
#include "StringOpr.h"

#include "../UniService/DefUniService.h"
#include "../../Logs.h"

#include "../../Solar.h"
#if defined(_FUNC_gMiniLicense)
#include "../../Maintain/gMiniLicense.h"
#endif//_FUNC_gMiniLicense

#include "../../Capability/ezRsa.h"

#include "AgentKeyManagementService.h"
#include "HandlerKeyManagementService.h"

#include "HttpServerKeyManagementService.h"

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



CHttpServerKeyManagementService::CHttpServerKeyManagementService(ISocketHandler& h) : HttpdSocket(h)
{
    m_pHandler = ((CHandlerKeyManagementService*)(&(Handler())));

    //m_strBody = "";
    m_pBodyBuf = new CezBuffer();

    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iIsAuthedHost = 0;

    m_pCfgKeyManagementService = NULL;

    m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    m_strKmsSid = "";
}

void CHttpServerKeyManagementService::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerKeyManagementService::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerKeyManagementService::~CHttpServerKeyManagementService()
{
    if (m_pBodyBuf != NULL)
    {
        delete m_pBodyBuf;
    }

}

void CHttpServerKeyManagementService::OnAccept()
{
    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());


#ifdef _USE_MODULES_EZCONFIG

    static CConfigKeyManagementService __cfg;

    __cfg.update();
    m_pCfgKeyManagementService = &__cfg;

    m_iTimeout = m_pCfgKeyManagementService->getConfig().iTimeout;

#else

    m_iTimeout = HTTPD_CONN_TIMEOUT;
#endif//

    if (m_pBodyBuf == NULL)
    {
        ez_printf_emerg("Low memory\n");
        SetCloseAndDelete();
        return;
    }
    m_pBodyBuf->SetIncreaseSize(1024);

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);

    m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

    m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense
}

void CHttpServerKeyManagementService::OnHeader(const std::string& key,const std::string& value)
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
    else if (!strcasecmp(key.c_str(), APP_AUTH_HTTP_HEAD_KEY_KmsSid))
    {
        m_strKmsSid = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_KmsSid, m_strKmsSid.c_str());
    }
    else
    {
        HttpdSocket::OnHeader(key, value);
    }
}

void CHttpServerKeyManagementService::OnDisconnect()
{
    ez_printf_debug("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerKeyManagementService::Exec()
{
    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerKeyManagementService::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();
    m_pBodyBuf->Reset();

#if 1 // 使用app鉴权
    // 特殊指令不鉴权
    if (GetUrl() == def_UniTrans_Req_hello
        // appkey 的不鉴权
        //|| GetUrl() == def_Pandora_Req_manAppKeyCreate
        //|| GetUrl() == def_Pandora_Req_manAppKeyCancel
       )
    {
        //ez_printf_debug("Noneed Auth:%s\n", GetUrl().c_str());

        return;
    }

    do
    {
        if (m_strKmsSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
        {
            ez_printf_info("%s not found, Req:%s\n", def_UniService_KeyManagementService_Sid, +GetUrl().c_str());

            RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);

            break;
        }

        if (m_pCfgKeyManagementService
            && BitGet(m_pCfgKeyManagementService->getConfig().iAuth, 0) == 1)
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

            int iretGet = m_pHandler->m_pCacheWithDb->getPair(m_strAppKey, appSecret);

            if (iretGet < 0)
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

        if (m_pCfgKeyManagementService
            && BitGet(m_pCfgKeyManagementService->getConfig().iAuth, 1) == 1)
        {
            ez_printf_debug("-->> do host chk\n");

            // tykd platform
            if (GetRemoteAddress()=="202.102.108.6"
                || GetRemoteAddress()=="202.102.108.29"
                || GetRemoteAddress()=="202.102.108.29"
               )
            {
                m_iIsAuthedHost = 1;
                ez_printf_debug("by , tykd ok\n");
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

void CHttpServerKeyManagementService::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
    AddResponseHeader("Server", std::string(def_UniService_KeyManagementService_ServerName) + " " + g_Solar.GetVersion());
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

    if (m_strKmsSid.size()>0)
    {
        AddResponseHeader( def_UniService_KeyManagementService_Sid, m_strKmsSid);
    }

    SendResponse();
}

void CHttpServerKeyManagementService::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
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


    CKmsStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}

void CHttpServerKeyManagementService::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CKmsStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}


/** Chunk of http body data recevied. */
void CHttpServerKeyManagementService::OnData(const char *p,size_t l)
{
    if (p==NULL || l<1)
    {
        return;
    }

    ez_printf_debug("%s:Size:%ld,Len:%ld\n", __FUNCTION__, m_pBodyBuf->Size(), l);

    if (m_pBodyBuf->Size()+l > def_HTTPD_MSGG_BodyMaxSize)
    {
        ez_printf_error("body(%lu), l(%lu) too long\n", m_pBodyBuf->Size(), l);

        Reset();
        m_pBodyBuf->Reset();

        return;
    }

    m_pBodyBuf->Append((unsigned char *)p, l);

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerKeyManagementService::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    CKmsStack __CezlibsWebServiceIF;

    unsigned char theTail[1] = {'\0'};
    m_pBodyBuf->Append(theTail, 1);
    std::string m_strBody = (char *)m_pBodyBuf->Buf();//((const unsigned char*)m_pBodyBuf->Buf(), m_pBodyBuf->Size());

    // 特殊指令
    if (GetUrl() == def_UniTrans_Req_hello
       )
    {
        Do_TestIF(__CezlibsWebServiceIF);

        return;
    }

    if (m_strBody.size()<strlen("[{}]"))
    {
        ez_printf_info("Body(%s) is not json.\n", m_strBody.c_str() );

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
    if (GetUrl()==def_KeyManagementService_Req_generateKeypair)
    {
        On_Req_KeyManagementService_generateKeypair(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_KeyManagementService_Req_encryptAsymmetric)
    {
        On_Req_KeyManagementService_encryptAsymmetric(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_KeyManagementService_Req_decryptAsymmetric)
    {
        On_Req_KeyManagementService_decryptAsymmetric(__CezlibsWebServiceIF);
    }
    else if (GetUrl()==def_KeyManagementService_Req_generateKey)
    {
        On_Req_KeyManagementService_generateKey(__CezlibsWebServiceIF);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if 0
    else if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
#endif
    else
    {
        RetGeneralMsg(100, "Unknow URL:"+GetUrl(), GetUrl());
    }

    Reset(); // prepare for next request
    SetCloseAndDelete();
    m_strBody = "";
}

void CHttpServerKeyManagementService::Update()
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
int CHttpServerKeyManagementService::SendMsg(const std::string &strMsg, const std::string &matchRequest)
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
int CHttpServerKeyManagementService::Do_TestIF( CKmsStack &JsonData)
{
    std::string strCode = PssMsg_Value_Result_Success;
    std::string strMsg = PssMsg_Value_ResultMsg_Success;

    ez_printf_info("%s:%s\n", def_UniService_KeyManagementService_Sid, m_strKmsSid.c_str());

#if defined(_FUNC_gMiniLicense)

    if (g_gMiniLicense.getLicVers() == GMINI_LIC_V1)
    {
        strMsg = g_gMiniLicense.getLicense();
    }
    else if (g_gMiniLicense.getLicVers() == GMINI_LIC_V2)
    {
        strMsg = g_gMiniLicense.getLicense();
    }
    else
    {
        std::string strCode = PssMsg_Value_Result_UnauthorizedAccess;
        std::string strMsg = "";
    }
#else
    CConfigGeneral __cfg;
    __cfg.update();

    strMsg = "Hello, Your URL:[" + GetUrl() + "], Me:" + __cfg.getConfig().strCode;
#endif

    RetGeneralMsg(strCode, strMsg);
    SetCloseAndDelete();

    return 0;
}

int CHttpServerKeyManagementService::On_Req_KeyManagementService_generateKeypair( CKmsStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CKmsMsg_requestMsgGenerateKeypair __reqMsg;
    CKmsMsg_requestMsgGenerateKeypairAck __ackMsg;

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

        ez_printf_debug("%s:%s\n",  KmsMsg_Key_KmsID.c_str(), __reqMsg.m_strKmsID.c_str());
        __ackMsg.m_strKmsID = __reqMsg.m_strKmsID;

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        //int iret = genKeyPair(__ackMsg.m_strPrivateKey, __ackMsg.m_strPublicKey);
        ezRSA __rs;
        std::string strPrivateKey;
        std::string strPublicKey;
        bool bRet = __rs.generateRSAKey(strPrivateKey, strPublicKey);
        if (!bRet)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
            break;
        }

        //to base64
        // strPrivateKey
        {
            int bufBase64Len = ez_base64encode_len(strPrivateKey.length()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear

            int ibase64Len = ez_base64encode((char *)&vecBase64[0], strPrivateKey.c_str(), strPrivateKey.length());
            ARG_USED(ibase64Len);
            DBG(
                __fline;
                printf("ez_base64encode:%d: buflen:%d bufBase64Len, %d, ==vecBase64.size():%d\n", ibase64Len, strlen((char *)&vecBase64[0]), bufBase64Len, vecBase64.size());
            );
            __ackMsg.m_strPrivateKey.assign(vecBase64.begin(), vecBase64.end());//
        }
        // strPublicKey
        {
            int bufBase64Len = ez_base64encode_len(strPublicKey.length()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear

            int ibase64Len = ez_base64encode((char *)&vecBase64[0], strPublicKey.c_str(), strPublicKey.length());
            ARG_USED(ibase64Len);
            DBG(
                __fline;
                printf("ez_base64encode:%d: buflen:%d bufBase64Len, %d, ==vecBase64.size():%d\n", ibase64Len, strlen((char *)&vecBase64[0]), bufBase64Len, vecBase64.size());
            );
            __ackMsg.m_strPublicKey.assign(vecBase64.begin(), vecBase64.end());//
        }
    }
    while(0);

    CKmsStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}


int CHttpServerKeyManagementService::On_Req_KeyManagementService_encryptAsymmetric( CKmsStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CKmsMsg_requestMsgEncryptAsymmetric __reqMsg;
    CKmsMsg_requestMsgEncryptAsymmetricAck __ackMsg;

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

        ez_printf_debug("%s:%s\n",  KmsMsg_Key_KmsID.c_str(), __reqMsg.m_strKmsID.c_str());
        __ackMsg.m_strKmsID = __reqMsg.m_strKmsID;

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        std::string strPublicKey;

        // base64decode
        {
            //
            int bufBase64Len = ez_base64decode_len(__reqMsg.m_strPublicKey.c_str()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear

            ///////////////////
            // restore b64 data
            int restored_encrypted_len=0;

            restored_encrypted_len = ez_base64decode((char *)&vecBase64[0], __reqMsg.m_strPublicKey.c_str());
            ARG_USED(restored_encrypted_len);
            DBG(
                __fline;
                printf("resultEncrypt datalen:%d\n", restored_encrypted_len);
            );

            strPublicKey.assign(vecBase64.begin(), vecBase64.end());//
        }

        ezRSA __rs;
        std::string strCipherText;
        bool bRet = __rs.RsaPublicEncrypt(__reqMsg.m_strPlainText, strPublicKey, strCipherText);
        if (!bRet)
        {
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
            break;
        }

        ez_printf_debug("strCipherText.length:%d\n", strCipherText.length());

        // strCipherText
        {
            int bufBase64Len = ez_base64encode_len(strCipherText.length()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear

            int ibase64Len = ez_base64encode((char *)&vecBase64[0], strCipherText.c_str(), strCipherText.length());
            ARG_USED(ibase64Len);
            DBG(
                __fline;
                printf("ez_base64encode:%d: buflen:%d bufBase64Len, %d, ==vecBase64.size():%d\n", ibase64Len, strlen((char *)&vecBase64[0]), bufBase64Len, vecBase64.size());
            );
            __ackMsg.m_strCipherText.assign(vecBase64.begin(), vecBase64.end());//
        }

    }
    while(0);

    CKmsStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}


int CHttpServerKeyManagementService::On_Req_KeyManagementService_decryptAsymmetric( CKmsStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CKmsMsg_requestMsgDecryptAsymmetric __reqMsg;
    CKmsMsg_requestMsgDecryptAsymmetricAck __ackMsg;

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

        ez_printf_debug("%s:%s\n",  KmsMsg_Key_KmsID.c_str(), __reqMsg.m_strKmsID.c_str());
        __ackMsg.m_strKmsID = __reqMsg.m_strKmsID;

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        std::string strPrivateKey;

        // base64decode
        // pri key
        {
            //
            int bufBase64Len = ez_base64decode_len(__reqMsg.m_strPrivateKey.c_str()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear

            ///////////////////
            // restore b64 data
            int restored_encrypted_len=0;

            restored_encrypted_len = ez_base64decode((char *)&vecBase64[0], __reqMsg.m_strPrivateKey.c_str());
            ARG_USED(restored_encrypted_len);

            strPrivateKey.assign(vecBase64.begin(), vecBase64.end());//

            ez_printf_debug("resultEncrypt datalen:%d\n%s\n", restored_encrypted_len, strPrivateKey.c_str());
        }
        // CipherText
        std::string strCipherText;
        {
            //
            int bufBase64Len = ez_base64decode_len(__reqMsg.m_strCipherText.c_str()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear
            ez_printf_debug("bufBase64Len:%d\n", bufBase64Len);

            ///////////////////
            // restore b64 data
            int restored_encrypted_len=0;

            restored_encrypted_len = ez_base64decode((char *)&vecBase64[0], __reqMsg.m_strCipherText.c_str());
            ARG_USED(restored_encrypted_len);

            //strCipherText.assign(vecBase64.begin(), vecBase64.end());//
            strCipherText.assign((char *)&vecBase64[0], restored_encrypted_len);//

            ez_printf_debug("restored_encrypted_len:%d, strCipherText.length:%d\n", restored_encrypted_len, strCipherText.length());
        }

        ezRSA __rs;
        std::string strPlainText;
        bool bRet = __rs.RsaPrivateDecrypt(strCipherText, strPrivateKey, strPlainText);
        if (!bRet)
        {
            ez_printf_debug("RsaPrivateDecrypt failed.\n");
            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;
            break;
        }
#if 0
        // strPlainText
        {
            int bufBase64Len = ez_base64encode_len(strPlainText.length()); // 返回的长度含结束符
            StlVecUnChar vecBase64;
            vecBase64.reserve(bufBase64Len);
            vecBase64.assign(bufBase64Len, '\0');// clear

            int ibase64Len = ez_base64encode((char *)&vecBase64[0], strPlainText.c_str(), strPlainText.length());
            ARG_USED(ibase64Len);
            DBG(
                __fline;
                printf("ez_base64encode:%d: buflen:%d bufBase64Len, %d, ==vecBase64.size():%d\n", ibase64Len, strlen((char *)&vecBase64[0]), bufBase64Len, vecBase64.size());
            );
            __ackMsg.m_strPlainText.assign(vecBase64.begin(), vecBase64.end());//
        }
#else
        __ackMsg.m_strPlainText = strPlainText;
#endif

    }
    while(0);

    CKmsStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}

int CHttpServerKeyManagementService::On_Req_KeyManagementService_generateKey( CKmsStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CKmsMsg_requestMsgGenerateKey __reqMsg;
    CKmsMsg_requestMsgGenerateKeyAck __ackMsg;

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

        ez_printf_debug("%s:%s\n",  KmsMsg_Key_KmsID.c_str(), __reqMsg.m_strKmsID.c_str());

        __ackMsg.m_strKmsID = __reqMsg.m_strKmsID;
        generateKey(__ackMsg.m_strKey);

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while(0);

    CKmsStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    return 0;
}

int CHttpServerKeyManagementService::generateKey(std::string &strKey)
{
    char uuidBuff[def_ez_uuid_len+1] = {0};

    // single thread, do not add lock here
#ifdef HAVE_LIBUUID

    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuidBuff);
#else

    ez_uuid(uuidBuff, g_Solar.GetRunPeriod()+g_Solar.GetCounter(), g_Solar.GetHuid().c_str());

#endif //HAVE_LIBUUID

    strKey = uuidBuff;

    return 0;
}

