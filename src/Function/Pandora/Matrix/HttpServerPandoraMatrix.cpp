/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerPandoraMatrix.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerPandoraMatrix.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#include "ez_util_platform.h"

#include "../../../Logs.h"
#include "../../../Solar.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigGeneral.h"
#include "../../../Configs/ConfigPandoraMatrix.h"
#endif //_USE_MODULES_EZCONFIG
#include "str_opr.h"
#include <ez_url_parser.h>
#include "StringOpr.h"
#include "ez_bit.h"
#include "md5.h"
#include "sha1.h"
#include "ez_socket.h"
#include "ez_appkey.h" // def_ez_APPKEY_BUFF_LEN ... // used by onReq_ManAppKeyCreate
#include "ezUtilCPP.h" // ez_ZAB_encode

#include "../PandoraDef.h"
#include "../../UniService/DefUniService.h"

#if defined(_FUNC_gMiniLicense)
#include "../../../Maintain/gMiniLicense.h"
#endif//_FUNC_gMiniLicense

#include "HandlerPandoraMatrix.h"

#include "HttpServerPandoraMatrix.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>
// #include <ezutil/ez_def_devel_debug.h>

//5152f876-c738-4e96-9c2b-b314501c40f1
#define MY_SESSION_ID_LEN 36

//取证持续时长 -- 秒
#define startSampling_Delay_Times (10*60)
//#define startSampling_Delay_Times (10)

// 前缀
#define def_shpKeyValueKeyPrefix               "ezlibs_Shp_"
// 最大在用端口
#define def_shpKeyValueKeyMaxPortHaveBeenUsed  "MaxPortHaveBeenUsed"
// 记录其使用的端口
#define def_shpKeyValueKeyRemotePort           "RemotPort"

CHttpServerPandoraMatrix::CHttpServerPandoraMatrix(ISocketHandler& h) : CGRestHttpdSocket(h)
{
    //m_strBody = "";
    m_pBodyBuf = new CezBuffer();
    m_requestPrefix = "";
    m_sessionId = "";

    m_iTimeout = def_Pandora_ConnTimeout;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;

    SetSockName(def_Default_Node_Name);

    //统一管理
    m_pHandlerPandoraMatrix = ((CHandlerPandoraMatrix *)(&(Handler())));

    m_pPssCfg = NULL;

    m_strKeyFrontOfValues = "";

    m_strGroupID = "";
    m_strNodeID = "";

    m_strTransferEncoding = APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null;
}

void CHttpServerPandoraMatrix::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    //LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerPandoraMatrix::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerPandoraMatrix::~CHttpServerPandoraMatrix()
{
    //LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerPandoraMatrix::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
    if (m_pBodyBuf != NULL)
    {
        delete m_pBodyBuf;
    }
}

void CHttpServerPandoraMatrix::OnAccept()
{
    ez_printf_info("%s:%s:%d\n", __FUNCTION__, GetRemoteAddress().c_str(), GetRemotePort());

    if (m_pBodyBuf == NULL)
    {
        ez_printf_emerg("Low memory\n");
        SetCloseAndDelete();
        return;
    }
    m_pBodyBuf->SetIncreaseSize(1024);

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
#ifdef _USE_MODULES_EZCONFIG

    static CConfigPandoraMatrix __cfg;
    __cfg.update();
    m_pPssCfg = &__cfg;

    m_iTimeout = m_pPssCfg->getConfig().ConnTimeout;
#else
    //SetTimeout(5);

    m_iTimeout = HTTPD_CONN_TIMEOUT;
#endif//
}
void CHttpServerPandoraMatrix::OnDisconnect()
{
    ez_printf_info("%s(%s):%s:%d\n", __FUNCTION__, m_strNodeID.c_str(), GetRemoteAddress().c_str(), GetRemotePort());

    int iClean = m_pPssCfg->getConfig().iCleanValuesWhenDisconnect;

    if (iClean == def_CleanValuesWhenDisconnect)
    {
        //ez_printf_debug("numPair:%d\n", m_pHandlerPandoraMatrix->m_pCacheKeyValue->numPair());
        m_pHandlerPandoraMatrix->m_pCacheKeyValue->clearPair(m_strKeyFrontOfValues);
        //ez_printf_debug("clearPair numPair:%d\n", m_pHandlerPandoraMatrix->m_pCacheKeyValue->numPair());
    }
}
//exec before: OnHeaderComplete
void CHttpServerPandoraMatrix::Exec()
{
    //ez_printf_debug("%s:%s:%d\n", __FUNCTION__, GetRemoteAddress().c_str(), GetRemotePort());
    ez_printf_debug("%s:%s:%d, ++++++++++++++++++++\n", __FUNCTION__, GetRemoteAddress().c_str(), GetRemotePort());
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());

    //SetCloseAndDelete();
}

void CHttpServerPandoraMatrix::OnHeader(const std::string& key,const std::string& value)
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
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_TransferEncoding))
    {
        m_strTransferEncoding = value;

DBG(
        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding, m_strTransferEncoding.c_str());
);
    }
    else
    {
        HttpdSocket::OnHeader(key, value);
    }
}

void CHttpServerPandoraMatrix::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();
    m_pBodyBuf->Reset();

#if 1 // 使用app鉴权
    // hello不鉴权
    if (GetUrl() == def_UniTrans_Req_hello)
    {
        //ez_printf_debug("on hello\n");
        //m_strTransferEncoding = APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null;
        return;
    }

    do
    {
        if (m_strPssSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
        {
            ez_printf_error("PssSid not found.\n");
            //RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);
            RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

            break;
        }

        if (m_pPssCfg
            && BitGet(m_pPssCfg->getConfig().iAuth, em_auth_type_appKey) == 1)
        {
            //ez_printf_debug("Do secret key chk\n");

            if (m_strAppKey.length()<1
                || m_strNonce.length()<1
                || m_strCurTime.length()<1
                || m_strCheckSum.length()<1
               )
            {
                ez_printf_error("Authdata not valid.\n");

                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);
                //RetGeneralMsg(102, "auth data not valid", GetUrl());
                break;
            }

            // 严格检查chk time
            if (BitGet(m_pPssCfg->getConfig().iAuth, em_auth_type_appKeyStrict) == 1)
            {

                time_t __ttTheCurTime;
                time_t __ttNow = time(NULL);
                ezConvertFromString(__ttTheCurTime, m_strCurTime);

                if (__ttTheCurTime != 1472451456 // 调试用
                    && abs(__ttTheCurTime-__ttNow)>300 )
                {
                    ez_printf_debug("__ttTheCurTime:%ld, __ttNow:%ld\n", __ttTheCurTime, __ttNow);
                    RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                    break;
                }
            }

            //encode("sha1", nonce + appSecret + curTime)
            // todo check appSecret from AppKey
            //std::string appSecret = "c48faffaedf5d4cae01c6a21c11476fc118e32b0";
            //std::string appSecret ;//= "c48faffaedf5d4cae01c6a21c11476fc118e32b0";

#if 1

            int iretGet = m_pHandlerPandoraMatrix->m_pCacheWithDb->getPair(m_strAppKey, m_strAppSecret);

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
            else if (m_strAppKey == "701819701")//node
            {
                appSecret = "ebd69a6ccd6f81ef4e6c52e37761750b42f0fe50";
            }
            else
#endif

            {
                ez_printf_warning("Unknow: AppKey:%s\n", m_strAppKey.c_str());
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);

                break;
            }
            else
            {
                //ez_printf_debug("AppKey:%s, appSecret:%s\n", m_strAppKey.c_str(), appSecret.c_str());
                ez_printf_debug("AppKey:%s\n", m_strAppKey.c_str());
            }

            m_strNonce+=m_strAppSecret;
            m_strNonce+=m_strCurTime;

            char dst[SHA1HashSize*2+1] = {0};
            const char *src = m_strNonce.c_str();
            ARG_USED(src);
            ARG_USED(dst);

            //ez_printf_debug("ez_sha1_str:%s\n", src);
            int iret = ez_sha1_str(dst, sizeof(dst), src );

            if (iret<0)
            {
                ez_printf_warning("auth failed src:[%s], dst:[%s]\n", src, dst);
                //RetGeneralMsg(103, "secret data not valid", GetUrl());
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);
                break;
            }

            if (m_strCheckSum != dst)
            {
                ez_printf_warning("auth failed src:[%s], dst:[%s]\n", src, dst);

                //RetGeneralMsg(104, "auth failed", GetUrl());
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_IllegalAccess);
                break;
            }

            //ez_printf_debug("appSecret Auth Src:%s\n", src);
            //ez_printf_debug("dst:%s\n", dst);
        }

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
                RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);
                break;
            }
        }

        // clean body buff
        //m_strBody = "";

        //ok
        return;
    }
    while(0);

    //Reset(); //
    SetCloseAndDelete();
#endif
}

void CHttpServerPandoraMatrix::OnUnknowRequest()
{}

bool CHttpServerPandoraMatrix::ReadyNode()
{
    if (!Ready())
    {
        return false;
    }

    if (GetSockName().length()<def_Min_Node_Name_Length)
    {
        return false;
    }

    return true;
}
#if 0
void CHttpServerPandoraMatrix::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
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
    AddResponseHeader("Content-type", "application/json");
#endif

    //AddResponseHeader("Connection", "close");
    AddResponseHeader("Connection", "Keep-Alive");

    if (m_sessionId.size()==MY_SESSION_ID_LEN)
    {
        AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
    }
    else
    {
        // debug only
        //m_sessionId = "joy-dbg-only";
        //AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
        //m_sessionId="";
    }

    if (ContentLength>0)
    {
        AddResponseHeader( "Content-length", Utility::l2string((long)ContentLength) );
    }

    if (matchRequest.size()>0)
    {
        AddResponseHeader( "Match-request", matchRequest);
    }
    SendResponse();
}
void CHttpServerPandoraMatrix::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
{

    CAckMsgSimple __ackMsg;

    __ackMsg.Result = ezConvertToString(retCode);
    __ackMsg.Reason = strMsg;

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), matchRequest);
}
#endif

void CHttpServerPandoraMatrix::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}

/** Chunk of http body data recevied. */
void CHttpServerPandoraMatrix::OnData(const char *p,size_t l)
{
#if 0
    ez_printf_debug("OnData:body(%lu), l(%lu)\n", m_strBody.length(), l);

    if (m_strBody.size()+l > def_Pandora_MaxMsgLen*9)
    {
        ez_printf_error("body(%lu), l(%lu) too long\n", m_strBody.length(), l);

        RetGeneralMsg(PssMsg_Value_Result_TooMuchData, PssMsg_Value_ResultMsg_TooMuchData);

        SetCloseAndDelete();

        return;
    }

    m_strBody += p;
#else

    ez_printf_debug("%s:Size:%ld,Len:%ld\n", __FUNCTION__, m_pBodyBuf->Size(), l);

    if (m_pBodyBuf->Size()+l > def_Pandora_MaxMsgLen)
    {
        ez_printf_error("body(%lu), l(%lu) too long\n", m_pBodyBuf->Size(), l);

        Reset();
        m_pBodyBuf->Reset();

        return;
    }

    m_pBodyBuf->Append((unsigned char *)p, l);
#endif
    //ez_printf_debug("OnData:body(%lu), l(%lu)\n", m_strBody.length(), l);

    m_ttOndata = ez_sec_get_tick_count();//time(NULL);
}
void CHttpServerPandoraMatrix::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    //def_MyServiceStack_Of_Pandora __CezlibsWebServiceIF;
    CPqStack __CezlibsWebServiceIF;

    //LOG4CPLUS_DEBUG(LOG_OF_SMS, "m_strBody:" << m_strBody);
    unsigned char theTail[1] = {'\0'};
    m_pBodyBuf->Append(theTail, 1);

    std::string m_strBody;

    ez_printf_debug("Url:%s\n", GetUrl().c_str());

    //ez_printf_debug("m_strTransferEncoding:[%s]\n", m_strTransferEncoding.c_str());

    do
    {
        // support ezab
        if (m_strTransferEncoding == APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB)
        {
            //ez_printf_debug("Body(%d):\n%s\n", m_pBodyBuf->Size(), m_pBodyBuf->Buf());
            ez_printf_debug("Body(%d):*\n", m_pBodyBuf->Size());

            //ez_printf_debug("on %s.\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB);

            //m_strBody不赋值，则在后面统一检查长度的时候返回PssMsg_Value_Result_InvalidData
            //m_strBody.clear();

            if (m_strCheckSum.length() < 30/*40*/)
            {
                ez_printf_error("eZAB need CheckSum.\n");

                break;
            }

            StlVecUnChar encBuff(m_pBodyBuf->Buf(), m_pBodyBuf->Buf()+m_pBodyBuf->Size());
            StlVecUnChar decBuff;
            int iret_dec = CezUtilCPP::ez_ZAB_decode(encBuff, decBuff, m_strCheckSum);
            if (0 != iret_dec)
            {
                //std::cout << "ez_ZAB_decode Failed:" << iret_dec << std::endl;
                ez_printf_error("ez_ZAB_decode Failed:\n", iret_dec);
                break;
            }

            if (decBuff.size()>def_HTTPD_MSGG_BodyMaxSize
                || strlen((char *)&decBuff[0]) > def_HTTPD_MSGG_BodyMaxSize)
            {
                ez_printf_error("decBuff.size()>def_HTTPD_MSGG_BodyMaxSize.\n");

                break;
            }

            m_strBody = (char *)(&decBuff[0]);
        }
        // support zab, // ezlibs's zip, aes, base64  消息用sha(m_strCheckSum + m_strAppSecret) 加密
        else if (APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_ZAB == m_strTransferEncoding)
        {
            //ez_printf_debug("Body(%d):\n%s\n", m_pBodyBuf->Size(), m_pBodyBuf->Buf());
            ez_printf_debug("Body(%d):*\n", m_pBodyBuf->Size());

            //ez_printf_debug("on %s.\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB);

            //m_strBody不赋值，则在后面统一检查长度的时候返回PssMsg_Value_Result_InvalidData
            //m_strBody.clear();

            if (m_strCheckSum.length() < 30/*40*/)
            {
                ez_printf_error("eZAB need CheckSum.\n");

                break;
            }

            StlVecUnChar encBuff(m_pBodyBuf->Buf(), m_pBodyBuf->Buf()+m_pBodyBuf->Size());
            StlVecUnChar decBuff;

            //for the AES_KEYLEN 16 
            std::string strSecret;
            CezUtilCPP::stringMergeAlternately(strSecret, m_strCheckSum, m_strAppSecret);
		
            int iret_dec = CezUtilCPP::ez_ZAB_decode(encBuff, decBuff, strSecret);
            if (0 != iret_dec)
            {
                //std::cout << "ez_ZAB_decode Failed:" << iret_dec << std::endl;
                ez_printf_error("ez_ZAB_decode Failed:%d\n", iret_dec);
                break;
            }

            if (decBuff.size()>def_HTTPD_MSGG_BodyMaxSize
                || strlen((char *)&decBuff[0]) > def_HTTPD_MSGG_BodyMaxSize)
            {
                ez_printf_error("decBuff.size()>def_HTTPD_MSGG_BodyMaxSize.\n");

                break;
            }

            m_strBody = (char *)(&decBuff[0]);
        }
        else //APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null
        {
            //ez_printf_debug("%s.\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null);

            m_strBody = (char *)m_pBodyBuf->Buf();//((const unsigned char*)m_pBodyBuf->Buf(), m_pBodyBuf->Size());
        }

        //ez_printf_debug("strBody(%d):\n%s\n", m_strBody.size(), m_strBody.c_str());
        ez_printf_debug("strBody(%d):*\n", m_strBody.size());
    }
    while(0);

    if (m_strBody.size()<strlen("[{}]"))
    {
        //LOG4CPLUS_INFO(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ")" << " [" << m_strBody << "] not json");
        ez_printf_error("No json string.\n");
        RetGeneralMsg(PssMsg_Value_Result_InvalidData, PssMsg_Value_ResultMsg_InvalidData);
        SetCloseAndDelete();

        return;
    }
    else
    {
        // have []  of json string
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

        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);
        if (bRet)
        {
            m_iMsgCountGet ++;
        }
        else
        {
            //LOG4CPLUS_INFO(LOG_OF_SMS, "__CezlibsWebServiceIF.Parse failed.");
            ez_printf_error("Json.Parse failed.\n");

            RetGeneralMsg(PssMsg_Value_Result_InvalidData, PssMsg_Value_ResultMsg_InvalidData);
            SetCloseAndDelete();

            return;
        }
    }

    //exa:
    //	curl http://localhost:60088/sms/register -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    //	curl http://localhost:60088/sms/register -d ""
    //	curl http://localhost:60088/sms/register
    // test interface
    if (GetUrl()==def_Pandora_requestHeartbeat)
    {
        On_requestHeartbeat(__CezlibsWebServiceIF);
    }//心跳接口
    //鉴权接口
    //	curl http://localhost:60088/sms/requestAuth -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
    else if (GetUrl()==def_Pandora_requestAuth)
    {
        On_requestAuth(__CezlibsWebServiceIF);
    }//鉴权接口
    //exa:
    //	curl http://localhost:18900/UniTrans/hello -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    // test interface
    else if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_reqHello(__CezlibsWebServiceIF);
    }
    else if (GetUrl() == def_UniTrans_Req_Echo)
    {
        //Do_reqEcho(__CezlibsWebServiceIF);
        Do_reqEcho(m_strBody);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 2018-08-01 8:49:59
#if defined(_FUNC_LiveInclusion)
    // 设备添加
    else if (GetUrl()== def_Psso_Req_DeviceAdd)
    {
        On_Req_Psso_DeviceAdd(__CezlibsWebServiceIF);
    }
    else if (GetUrl()== def_Psso_Req_DeviceCancel)
    {
        On_Req_Psso_DeviceCancel(__CezlibsWebServiceIF);
    }
    else if (GetUrl()== def_Psso_Req_ValueGet)
    {
        On_Req_Psso_ValueGet(__CezlibsWebServiceIF);
    }
    else if (GetUrl()== def_Psso_Req_ValueSet)
    {
        On_Req_Psso_ValueSet(__CezlibsWebServiceIF);
    }
#endif //#if defined(_FUNC_LiveInclusion)

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    else if (GetUrl()==def_PandoraQ_Req_Publish_IO_Out)
    {
        On_Req_Hqtt1_Pub_IO_Out();
    }//

    else if (GetUrl()==def_PandoraQ_Req_Publish_HID_Send)
    {
        On_Req_Hqtt1_Pub_ShareHID();
    }//
    else if (GetUrl()==def_PandoraQ_NodReq_Publish_HID_Send) // 返回
    {
        On_Req_Hqtt1_Pub_ShareHIDAck();
    }//
    
    else if (GetUrl()==def_PandoraQ_Req_GetValue)
    {
        On_Req_Hqtt1_Get_Value(__CezlibsWebServiceIF);
    }//
    //批量获取
    else if (GetUrl()==def_PandoraQ_Req_GetValue_List)
    {
        On_Req_PandoraQ_Req_GetValueList(__CezlibsWebServiceIF);
    }//
    // 批量设置、操作
    else if (GetUrl()==def_PandoraQ_Req_SetValue_List)
    {
        On_Req_PandoraQ_Req_SetValueList(__CezlibsWebServiceIF);
    }//

    else if (GetUrl()==def_PandoraQ_Req_SetPlanSchedule)
    {
        On_Req_PandoraQ_Req_SetPlanSchedule(__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_Req_GetPlanSchedule)
    {
        On_Req_PandoraQ_Req_GetPlanSchedule(__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_Req_PubTaskStart   )
    {
        On_Req_PandoraQ_Req_PubTaskStart   (__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_Req_PubTaskStop    )
    {
        On_Req_PandoraQ_Req_PubTaskStop    (__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_Req_GetTaskList  )
    {
        On_Req_PandoraQ_Req_GetTaskList  (__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_Req_GetTaskDetails  )
    {
        On_Req_PandoraQ_Req_GetTaskDetails  (__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_Req_GetDeviceList  )
    {
        On_Req_PandoraQ_Req_GetDeviceList  (__CezlibsWebServiceIF);
    }//
    //////// shp config
    else if (GetUrl() == def_PandoraQ_Req_ShpSet  )
    {
        On_Req_PandoraQ_Req_SetShpc (__CezlibsWebServiceIF);
    }//
    //ret from Node
    else if (GetUrl() == def_PandoraQ_NodReq_ShpSet  )
    {
        On_Req_PandoraQ_NodReq_SetShpc (__CezlibsWebServiceIF);
    }//
    else if (GetUrl() == def_PandoraQ_Req_ShpGet  )
    {
        On_Req_PandoraQ_Req_GetShpc (__CezlibsWebServiceIF);
    }//
    else if (GetUrl() == def_PandoraQ_NodReq_ShpGet  )
    {
        On_Req_PandoraQ_NodReq_GetShpc (__CezlibsWebServiceIF);
    }//
    ///////////////////////////////////////////////////////
    else if (GetUrl()==def_PandoraQ_Req_PublishValue)
    {
        On_Req_PandoraQ_Req_Pub_Value(__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_NodReq_PublishValue)
    {
        On_NodReq_Hqtt1_Pub_Value(__CezlibsWebServiceIF);
    }//
    else if (GetUrl()==def_PandoraQ_MasReq_PubTaskStart)
    {
        On_NodReq_Hqtt1_PubTaskStart(__CezlibsWebServiceIF);
    }//

    else
    {
        // do not ret
        //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
        //        RetGeneralMsg();
        ez_printf_error("unknow Request:%s, Body:%s\n", GetUrl().c_str(), m_strBody.c_str());
    }

    Reset(); // prepare for next request

    // at OnHeaderComplete
    //m_strBody = "";
}

void CHttpServerPandoraMatrix::Update()
{
    unsigned long ttnow = ez_sec_get_tick_count();//time(NULL);

    DBG(
        cout << ttnow-m_ttOndata << " tout:" << m_iTimeout << endl;
    );

    if (ttnow-m_ttOndata>m_iTimeout)
    {
        ez_printf_info("ttnow:%ld,m_ttOndata:%ld,Timeout:%ld Session:%s SetCloseAndDelete ...\n", ttnow, m_ttOndata, m_iTimeout, getMySessionId().c_str());
        SetCloseAndDelete();
    }
}

size_t CHttpServerPandoraMatrix::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerPandoraMatrix::getMsgNumSent()
{
    return m_iMsgCountSend;
}


// http + json
int CHttpServerPandoraMatrix::SendMsg(const std::string &strMsg, const std::string &matchRequest)
{
    CEZLock __lock(m_MutexSendMsg);

    std::string __strMsg;

    if (m_strTransferEncoding == APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB)
    {
        AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_TransferEncoding, APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB);
        StlVecUnChar oriBuff(strMsg.c_str(), strMsg.c_str()+strMsg.length()+1);
        StlVecUnChar encedBuff;
        int iret_enc = CezUtilCPP::ez_ZAB_encode(oriBuff, encedBuff, m_strCheckSum);

        if (0 != iret_enc)
        {
            //std::cout << "ez_ZAB_encode Failed:" << iret_enc << std::endl;
            ez_printf_error("ez_ZAB_encode Failed:%d\n", iret_enc);
            return -100;
        }

        __strMsg.assign((char *)(&(encedBuff[0])), encedBuff.size());
    }
    else if (m_strTransferEncoding == APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_ZAB)
    {
        char dst[SHA1HashSizeDestBuffer/*SHA1HashSize*2+1*/] = {0};
        std::string __newChkSumSrc = m_strCheckSum+m_strAppSecret; // 这里供客户端解密用， 不搞太复杂了
        const char *src = __newChkSumSrc.c_str();
        ARG_USED(src);
        ARG_USED(dst);

        //ez_printf_debug("ez_sha1_str:%s\n", src);
        int iret = ez_sha1_str(dst, sizeof(dst), src );
        ARG_USED(iret);

        std::string __strCheckSum = dst;

        AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_TransferEncoding, APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_ZAB);
        AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_CheckSum, __strCheckSum);

        StlVecUnChar oriBuff(strMsg.c_str(), strMsg.c_str()+strMsg.length()+1);
        StlVecUnChar encedBuff;

        std::string strSecret;
        CezUtilCPP::stringMergeAlternately(strSecret, __strCheckSum, m_strAppSecret);
        int iret_enc = CezUtilCPP::ez_ZAB_encode(oriBuff, encedBuff, strSecret);

        if (0 != iret_enc)
        {
            //std::cout << "ez_ZAB_encode Failed:" << iret_enc << std::endl;
            ez_printf_error("ez_ZAB_encode Failed:%d\n", iret_enc);
            return -100;
        }

        __strMsg.assign((char *)(&(encedBuff[0])), encedBuff.size());
    }
    else
    {
        __strMsg = strMsg;
    }

    //ez_printf_debug("%s(%ld):%s\n",  "SendMsg", strMsg.size(), strMsg.c_str());
    //ez_printf_debug("%s(%ld):%s\n",  "Send", __strMsg.size(), __strMsg.c_str());

    //ez_printf_info("%s(%ld):*\n",  "SendMsg", strMsg.size());
    //ez_printf_info("%s(%ld):*\n",  "Send", __strMsg.size());

    CreateHeader(__strMsg.size(), matchRequest, "");
    Send(__strMsg);

    //Reset();

    m_iMsgCountSend++;

    return 0;
}

std::string CHttpServerPandoraMatrix::generateSessionId()
{
    //char uuidBuff[MY_SESSION_ID_LEN+1] = {0};
    char uuidBuff[def_ez_uuid_len+1] = {0};

    // single thread, do not add lock here
#ifdef HAVE_LIBUUID

    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuidBuff);
#else

    ez_uuid(uuidBuff, g_Solar.GetRunPeriod()+g_Solar.GetCounter(), g_Solar.GetHuid().c_str());

#endif //HAVE_LIBUUID

    return uuidBuff;
}

std::string CHttpServerPandoraMatrix::getMySessionId()
{
    return m_sessionId;
}

const std::string &CHttpServerPandoraMatrix::getGroupID() const
{
    return m_strGroupID;
}

const std::string &CHttpServerPandoraMatrix::getNodeID() const
{
    return m_strNodeID;
}

// curl http://localhost:60088/Sms/TestIF -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerPandoraMatrix::Do_reqHello( CPqStack &JsonData)
{

    // 测试性能
    if (0)
    {
        // 结构体复用
        CPqMsg_requestMsgPubValue __reqMsg;
        CPqMsg_requestMsgPubValueAck __ackMsg;

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        __ackMsg.m_strDevID = __reqMsg.m_strDevID;

        do
        {
            if (!JsonData.Decode(__reqMsg))
            {
                ez_printf_error("Failed Decode.\n");

                __ackMsg.Result = PssMsg_Value_Result_InvalidData;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

                break;
            }

            CValueMsg Value;
            Value.m_strChannel = "1";
            Value.m_strType = "MsgLen";
            //Value.m_strValue = ezConvertToString(m_strBody.length());
            Value.m_strValue = ezConvertToString(m_pBodyBuf->Size());
            __ackMsg.Values.push_back(Value);

            //CValueMsg Value;
            Value.m_strChannel = "1";
            Value.m_strType = "Body";
            Value.m_strValue = "Body";//m_strBody;
            __ackMsg.Values.push_back(Value);
        }
        while(0);

        CPqStack __stack;
        __stack.Encode(__ackMsg);
        SendMsg(__stack.ToString(), GetUrl());
        //SetCloseAndDelete();

        return 0;
    }

    std::string strCode = PssMsg_Value_Result_Success;
    std::string strMsg = PssMsg_Value_ResultMsg_Success;

    ez_printf_info("PssSid:%s\n", m_strPssSid.c_str());

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
int CHttpServerPandoraMatrix::Do_reqEcho(const std::string &strReq)
{
    SendMsg(strReq, GetUrl());
    //    SetCloseAndDelete();

    return 0;
}

//
int CHttpServerPandoraMatrix::onReq_ManAppKeyCreate( CPqStack &JsonData)
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
            ez_printf_error("Error:%s\n", PssMsg_Value_ResultMsg_InvalidData.c_str());

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
            break;
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

        int iretGet = m_pHandlerPandoraMatrix->m_pCacheWithDb->getPair(bufAppKey, __ackMsg.AppSecret);

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
            int iretAdd = m_pHandlerPandoraMatrix->m_pCacheWithDb->addPair(bufAppKey, bufAppSecret);

            if (iretAdd < 0) // failed
            {
                ez_printf_error("Failed addPair:%d\n", iretAdd);

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

        ez_printf_debug("Body:%s\n", m_pBodyBuf->Buf());
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    //ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl());
    return 0;
}

//
int CHttpServerPandoraMatrix::onReq_ManAppKeyCancel( CPqStack &JsonData)
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

        std::string strAppSecret;
        int iretGet = m_pHandlerPandoraMatrix->m_pCacheWithDb->getPair(bufAppKey, strAppSecret);

        if (iretGet < 0) // not found
        {
            ez_printf_debug("AppSecret of %s not found.\n", bufAppKey);

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
            break;
        }
        else
        {
            int iretDel = m_pHandlerPandoraMatrix->m_pCacheWithDb->delPair(bufAppKey);
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

        ez_printf_debug("Body:%s\n", m_pBodyBuf->Buf());
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    ez_printf_debug("ack:%s\n", __stack.ToString().c_str());

    SendMsg(__stack.ToString(), GetUrl());
    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

// curl http://localhost:60088/sms/requestAuth -d "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12345\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Port\" : \"12346\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }"
//4.1	鉴权
int CHttpServerPandoraMatrix::On_requestAuth( CPqStack &JsonData)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CSmsMsg_requestAuth __reqMsg;
    CSmsMsg_requestAuthACK __ackMsg;
    bool bSetCloseAndDelete = false;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    do
    {

        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            bSetCloseAndDelete = true;

            break;
        }

        //    if (JsonData.Decode(__reqMsg))
        //    {
        //长度检查
        if (__reqMsg.m_LocalSystemInfo.m_strHostID.length() < def_Min_Node_Name_Length
            || __reqMsg.m_LocalSystemInfo.m_strHostID.length() > def_Max_Node_Name_Length)
        {
            ez_printf_error("Invalid HostName:%s\n", __reqMsg.m_LocalSystemInfo.m_strHostID.c_str());

            __ackMsg.Result = PssMsg_Result_PandoraMatrix_HostnameNotValid;
            __ackMsg.Reason = PssMsg_ResultMsg_PandoraMatrix_HostnameNotValid;

            //SetCloseAndDelete();
            bSetCloseAndDelete = true;

            //return -2;
            break;
        }
        // 检查是否重名
        if (NULL != m_pHandlerPandoraMatrix->GetPandoraNode(__reqMsg.m_LocalSystemInfo.m_strHostID))
        {
            ez_printf_error("Duplicated HostName:%s\n", __reqMsg.m_LocalSystemInfo.m_strHostID.c_str());

            __ackMsg.Result = PssMsg_Result_PandoraMatrix_DuplicateHostname;
            __ackMsg.Reason = PssMsg_ResultMsg_PandoraMatrix_DuplicateHostname;

            //SetCloseAndDelete();
            bSetCloseAndDelete = true;

            //return -3;
            break;
        }

        SetSockName(__reqMsg.m_LocalSystemInfo.m_strHostID);

        m_strGroupID = m_strAppKey;
        m_strNodeID = __reqMsg.m_LocalSystemInfo.m_strHostID;

        ez_printf_info("NewNode: %s -- %s:%d\n"
                       , __reqMsg.m_LocalSystemInfo.m_strHostID.c_str()
                       , GetRemoteAddress().c_str()
                       , GetRemotePort());

        // Name
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, PqMsg_Key_Name);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostAlias, modPair_If_Not_Exist_Then_Add);

                ez_printf_info("Node: %s -- %s:%s\n"
                               , __reqMsg.m_LocalSystemInfo.m_strHostID.c_str()
                               , __strDstIKey.c_str()
                               , __reqMsg.m_LocalSystemInfo.m_strHostAlias.c_str());
            }

        }
        // m_strDescription
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, UniMsg_Key_Description);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strDescription, modPair_If_Not_Exist_Then_Add);
            }

        }
        // Type
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, PqMsg_Key_Type);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strType, modPair_If_Not_Exist_Then_Add);
            }

        }
        // Version
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, UniMsg_Key_Version);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strVersion, modPair_If_Not_Exist_Then_Add);
            }

        }
        // Location
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, UniMsg_Key_Location);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strLocation, modPair_If_Not_Exist_Then_Add);
            }

        }
        // Vendor
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, UniMsg_Key_Vendor);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strVendor, modPair_If_Not_Exist_Then_Add);
            }

        }

        // Mac
        {
            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strHostID, UniMsg_Key_Mac);

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strMac, modPair_If_Not_Exist_Then_Add);
            }

        }

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        // 鉴权返回消息

        __ackMsg.Result         = PssMsg_Value_Result_Success;
        __ackMsg.Reason      = PssMsg_Value_ResultMsg_Success;
        // 重新认证, 返回新sid
        m_sessionId = generateSessionId();
        __ackMsg.m_strSessionID      = m_sessionId;//"6f61dfe3-1ec9-4123-826e-37862efd901d";

        __ackMsg.m_strSessionTimeout = "30";//__ostr.str();

        __ackMsg.m_LocalSystemInfo.m_strHostID    = "0";
        __ackMsg.m_LocalSystemInfo.m_strSerial      = "0";
        __ackMsg.m_LocalSystemInfo.m_strDescription = "theMatrix";
        __ackMsg.m_LocalSystemInfo.m_strType        = "0";

        //char p_buff[32];
        __ackMsg.m_LocalSystemInfo.m_strLocalDate   = ezConvertToString(time(NULL));//get_date_time_string(p_buff, NULL);//"2013-11-08 16:55:53 +0800";
#if 0

        CMsgService __Service;

        __Service.m_strServiceID   = "2011221100000000015501";
        __Service.m_strDescription = "GeminiLock service agent";
        __Service.m_strURI         = "ss1.ezlibs.com";
        //__Service.m_strPort        = "60600";
        __Service.m_strProtocol    = SERVICE_PROTOCOL_SERVER_Pandora;
        __ackMsg.m_vLocalService.push_back(__Service);
#endif

#if 0
        // todo: 端口起始
        unsigned short shpPort = 0;

        std::string __strDstIKeyShp="";

        {
            // 获取最大已用 shpport
            CCacheKeyValue::bldCacheKey(__strDstIKeyShp, def_shpKeyValueKeyPrefix, def_shpKeyValueKeyMaxPortHaveBeenUsed);

            std::string __strValue;
            int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheWithDb->getPair(__strDstIKeyShp, __strValue);

            if (0==iret_getPair)
            {
                ezConvertFromString(shpPort, __strValue);
                ez_printf_debug("Key:%s, Val:%s, shpPort:%d\n",  __strDstIKeyShp.c_str(), __strValue.c_str(), shpPort);
            }
            else
            {
                shpPort = 16690;
            }

            //m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKey, __reqMsg.m_LocalSystemInfo.m_strVendor, modPair_If_Not_Exist_Then_Add);
        }
        // 记录原始
        unsigned short shpPort_theMaxHaveBeenUsed = shpPort;

        for (size_t ii=0; ii<__reqMsg.m_vLocalService.size(); ii++)
        {
            if (def_Service_Protocol_Shp == __reqMsg.m_vLocalService[ii].m_strProtocol)
            {
                std::string strType = "";
                ez_parsed_url_t *parsed_url;
                parsed_url = ez_parse_url(__reqMsg.m_vLocalService[ii].m_strURI.c_str());
                if (parsed_url)
                {
                    strType = parsed_url->path;
                }

                ez_printf_info("Protocol:%s\n", parsed_url->path);

                CMsgService __Service;
                __Service.m_strServiceID   = __reqMsg.m_vLocalService[ii].m_strServiceID;
                __Service.m_strProtocol    = def_Service_Protocol_Shp;

                __Service.m_strDescription = __reqMsg.m_vLocalService[ii].m_strServiceID;
                __Service.m_strURI         = "shp://user:d9239799243904debc55fddb9107cc4fe58ee9bb@vsa.shp.ezlibs.com:18701/tcp/forensics?";//16309";

                unsigned short thisShpPort;
                std::string __strDstIKeyShpRemotPort;
                std::string __strValueRemotPort;
                CCacheKeyValue::bldCacheKey(__strDstIKeyShpRemotPort, def_shpKeyValueKeyPrefix, __reqMsg.m_vLocalService[ii].m_strServiceID, def_shpKeyValueKeyRemotePort);
                int iret_getPairRemotPort = m_pHandlerPandoraMatrix->m_pCacheWithDb->getPair(__strDstIKeyShpRemotPort, __strValueRemotPort);

                // 使用已有
                if (0==iret_getPairRemotPort)
                {
                    ezConvertFromString(thisShpPort, __strValueRemotPort);
                    ez_printf_debug("Key:%s, Val:%s, thisShpPort:%d\n",  __strDstIKeyShpRemotPort.c_str(), __strValueRemotPort.c_str(), thisShpPort);
                }
                //新分配
                else
                {
                    shpPort++;
                    thisShpPort = shpPort;
                    m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKeyShpRemotPort, ezConvertToString(thisShpPort), modPair_If_Not_Exist_Then_Add);
                    ez_printf_debug("use New thisShpPort:%d\n", thisShpPort);
                }

                __Service.m_strURI += (def_Shp_Tcp_Port_Query_Mark+ezConvertToString(thisShpPort));

                __ackMsg.m_vLocalService.push_back(__Service);
            }
        }

        // 最大值已修改
        if (shpPort_theMaxHaveBeenUsed != shpPort)
        {
            m_pHandlerPandoraMatrix->m_pCacheWithDb->modPair(__strDstIKeyShp, ezConvertToString(shpPort), modPair_If_Not_Exist_Then_Add);
        }
#endif

    }
    while(0);

    CPqStack CSmsStack_requestAuthACK;
    CSmsStack_requestAuthACK.Encode(__ackMsg);

    SendMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, GetUrl());

    if (bSetCloseAndDelete)
    {
        SetCloseAndDelete();
    }

    return 0;
}

int CHttpServerPandoraMatrix::On_requestHeartbeat( CPqStack &JsonData)
{
    ez_printf_debug("%s:%s\n", __FUNCTION__, m_strNodeID.c_str());

    CAckMsgSimple Msg;
    Msg.Result         = PssMsg_Value_Result_Success;
    Msg.Reason      = PssMsg_Value_ResultMsg_Success;
#if 0
    // Node 属性信息
    CStreamCapabilityMsg __onMsg;

    if (JsonData.Decode(__onMsg))
    {}
    else
    {
        ez_printf_error("JsonData.Decode failed.\n");

        Msg.Result         = PssMsg_Value_Result_InvalidData;
        Msg.Reason      = PssMsg_Value_ResultMsg_InvalidData;
    }
#endif
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 鉴权返回消息
    CPqStack CSmsStack_requestAuthACK;

    CSmsStack_requestAuthACK.Encode(Msg);
    SendMsg(CSmsStack_requestAuthACK.ToString()/*strEncoded*/, GetUrl());

    return 0;
}

int CHttpServerPandoraMatrix::Do_MatchReq(const std::string &strUrl, const std::string &strReq)
{
    SendMsg(strReq, strUrl);

    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// 2018-08-01 8:49:59
#if defined(_FUNC_LiveInclusion)
#include "../../Probe/Probe.h"
int CHttpServerPandoraMatrix::On_Req_Psso_DeviceAdd( CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValue __reqMsg;
    CPqMsg_requestMsgPubValueAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strDevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        // 查找既有信息

        if (0)
        {
            // UrlRtsp
            {
                std::string __strDstIKey;

                int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_strDevID, def_Psso_ValueKey_UrlRtsp, "1"/*it->m_strChannel*/);
                ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

                if (0==iret_ValueKey)
                {
                    std::string __strValue;
                    int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

                    if (0==iret_getPair)
                    {
                        ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __strValue.c_str());
                        CValueMsg Value;
                        Value.m_strChannel = "1";
                        Value.m_strType = def_Psso_ValueKey_UrlRtsp;
                        Value.m_strValue = __strValue;
                        __ackMsg.Values.push_back(Value);
                    }
                }
            }// UrlRtsp

        }
        std::string __strAccount    ;
        std::string __strPasswd     ;
        std::string __strManufacture;
        std::string __strModel      ;
        std::string __strChannelNum   ;
        std::string __strHostIp     ;
        std::string __strHttpPort   ;
        std::string __strRtspPort   ;
        std::string __strTcpPort    ;

        std::list<CValueMsg>::const_iterator it = __reqMsg.Values.begin();
        for ( ; it != __reqMsg.Values.end() ; it++)
        {
            if (def_Psso_ValueKey_Account     == it->m_strType)
            {
                __strAccount     = it->m_strValue;
            }
            else if (def_Psso_ValueKey_Passwd      == it->m_strType)
            {
                __strPasswd      = it->m_strValue;
            }
            else if (def_Psso_ValueKey_Manufacture == it->m_strType)
            {
                __strManufacture = it->m_strValue;
            }
            else if (def_Psso_ValueKey_Model       == it->m_strType)
            {
                __strModel       = it->m_strValue;
            }
            else if (def_Psso_ValueKey_ChannelNum== it->m_strType)
            {
                __strChannelNum       = it->m_strValue;
            }
            else if (def_Psso_ValueKey_HostIp      == it->m_strType)
            {
                __strHostIp      = it->m_strValue;
            }
            else if (def_Psso_ValueKey_HttpPort    == it->m_strType)
            {
                __strHttpPort    = it->m_strValue;
            }
            else if (def_Psso_ValueKey_RtspPort    == it->m_strType)
            {
                __strRtspPort    = it->m_strValue;
            }
            else if (def_Psso_ValueKey_TcpPort     == it->m_strType)
            {
                __strTcpPort     = it->m_strValue;
            }
            else
            {}
        }

        if (__strAccount    .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_Account    );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_Account    ;
            break;
        }

        if (__strPasswd     .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_Passwd     );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_Passwd     ;
            break;
        }

        if (__strManufacture.length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_Manufacture);
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_Manufacture;
            break;
        }

        if (__strModel      .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_Model      );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_Model      ;
            break;
        }

        if (__strChannelNum.length() < 1)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_ChannelNum);
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_ChannelNum;
            break;
        }

        if (__strHostIp     .length() < 2)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_HostIp     );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_HostIp     ;
            break;
        }

        if (__strHttpPort   .length() < 1)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_HttpPort   );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_HttpPort   ;
            break;
        }

        if (__strRtspPort   .length() < 1)
        {
            ez_printf_error("InvalidParameter:%s.\n", def_Psso_ValueKey_RtspPort   );
            __ackMsg.Result = PssMsg_Value_Result_InvalidParameter;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidParameter + std::string(":") + def_Psso_ValueKey_RtspPort   ;
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
        // Demo
        CValueMsg Value;

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
        if (def_Psso_Value_Manufacture_HIKVISION == __strManufacture)
        {
            iManufacture = ezDef_StreamType_IPC_hikvision;
        }
        else if (def_Psso_Value_Manufacture_DAHUA == __strManufacture)
        {
            iManufacture = ezDef_StreamType_IPC_dahua;
        }
        else if (def_Psso_Value_Manufacture_TIANDY == __strManufacture)
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
                Value.m_strChannel = __strChnN;
                Value.m_strType = def_Psso_ValueKey_UrlRtsp;
                Value.m_strValue = pPlayUrl;//"rtsp://61.160.148.78:59554/dev=JXJ-IPC-00000000/media=0/channel=0&level=0";
                __ackMsg.Values.push_back(Value);
            }
            else
            {
                pPlayUrl[0] = '\0';

                ez_printf_error("ez_getStreamPlayUrl:%d\n", ret_ez_getStreamPlayUrl);
            }
        }

        g_Probe.addPeer(__reqMsg.m_strDevID, pPlayUrl);

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;

        Value.m_strChannel = "";
        Value.m_strType = def_Psso_ValueKey_DevStatus;
        Value.m_strValue = def_Psso_Value_Status_Unknown;
        __ackMsg.Values.push_back(Value);

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}

int CHttpServerPandoraMatrix::On_Req_Psso_DeviceCancel( CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValue __reqMsg;
    CAckMsgSimple __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strDevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        int iret = g_Probe.delPeer(__reqMsg.m_strDevID);
        if (iret < 0)
        {
            ez_printf_error("%s not exists.\n", __reqMsg.m_strDevID.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;

            break;
        }

    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}
int CHttpServerPandoraMatrix::On_Req_Psso_ValueGet( CPqStack &JsonData)
{
    return 0;
}

int CHttpServerPandoraMatrix::On_Req_Psso_ValueSet( CPqStack &JsonData)
{
    return 0;
}
#endif //_FUNC_LiveInclusion
int CHttpServerPandoraMatrix::On_Req_Hqtt1_Pub_IO_Out()
{
    CAckMsgSimple __ackMsg;
    int iret = 0;
    std::string m_strBody = (char *)m_pBodyBuf->Buf();//((const unsigned char*)m_pBodyBuf->Buf(), m_pBodyBuf->Size());

    do
    {

        CPqStack __CezlibsWebServiceIF;

        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);

        if (!bRet)
        {
            ez_printf_error("CPqStack.Parse failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -1;
            break;
        }
        CPqMsg_requestMsgPub_IO_Out __reqMsg;
        bRet = __CezlibsWebServiceIF.Decode(__reqMsg);

        if (!bRet)
        {
            ez_printf_error("CPqStack.Decode failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -2;
            break;
        }

        m_iMsgCountGet ++;

        ez_printf_debug("DevID:%s\n", __reqMsg.m_strDevID.c_str());
        ez_printf_debug("Channel:%s\n", __reqMsg.m_strChannel.c_str());
        ez_printf_debug("Method:%s\n", __reqMsg.m_strMethod.c_str());

        CPqMsg_requestMsg __MatchReqMsg;
        __MatchReqMsg.m_strDevID = __reqMsg.m_strDevID;
        __MatchReqMsg.m_strSession = generateSessionId();
        __MatchReqMsg.m_strMsgBody = m_strBody;
        int iRet_MatchReq = m_pHandlerPandoraMatrix->Do_MatchReq(def_PandoraQ_MasReq_Publish_IO_Out, __MatchReqMsg);
        ARG_USED(iRet_MatchReq);

        if (iRet_MatchReq < 0)
        {
            ez_printf_info("Device Offline.\n");

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        iret = 0;

        // 保存设定的值
// #if 1

        std::string __strDstIKey;
        int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_strDevID, def_PqMsg_ValueKey_DigitalOutput, __reqMsg.m_strChannel);
        ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

        if (0==iret_ValueKey)
        {
            //std::string __strValue;
            int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__strDstIKey, __reqMsg.m_strMethod, modPair_If_Not_Exist_Then_Add);

            if (0==iret_getPair)
            {
                ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __reqMsg.m_strMethod.c_str());
            }
        }

// #endif

    }
    while (0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return iret;
}

int CHttpServerPandoraMatrix::On_Req_Hqtt1_Pub_ShareHID()
{
    CAckMsgSimple __ackMsg;
    int iret = 0;
    std::string m_strBody = (char *)m_pBodyBuf->Buf();//((const unsigned char*)m_pBodyBuf->Buf(), m_pBodyBuf->Size());

    do
    {

        CPqStack __CezlibsWebServiceIF;

        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);

        if (!bRet)
        {
            ez_printf_error("CPqStack.Parse failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -1;
            break;
        }
        // CPqMsg_requestMsgPub_ShareHID __reqMsg;
        CPqMsg_requestMsgShareHID __reqMsg;
        bRet = __CezlibsWebServiceIF.Decode(__reqMsg);

        if (!bRet)
        {
            ez_printf_error("CPqStack.Decode failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -2;
            break;
        }

        m_iMsgCountGet ++;

        ez_printf_debug("DevID:%s\n", __reqMsg.m_strDevID.c_str());
        // ez_printf_debug("Channel:%s\n", __reqMsg.m_strChannel.c_str());
        // ez_printf_debug("Method:%s\n", __reqMsg.m_strMethod.c_str());

        CPqMsg_requestMsg __MatchReqMsg;
        __MatchReqMsg.m_strDevID = __reqMsg.m_strDevID;
        __MatchReqMsg.m_strSession = generateSessionId();
        __MatchReqMsg.m_strMsgBody = m_strBody;
        int iRet_MatchReq = m_pHandlerPandoraMatrix->Do_MatchReq(def_PandoraQ_MasReq_Publish_HID_Send, __MatchReqMsg);
        ARG_USED(iRet_MatchReq);

        if (iRet_MatchReq < 0)
        {
            ez_printf_info("Device Offline.\n");

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        iret = 0;



    }
    while (0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return iret;
}
int CHttpServerPandoraMatrix::On_Req_Hqtt1_Pub_ShareHIDAck()
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    return 0;
}

// 接口请求， 断开
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_Pub_Value(CPqStack &JsonData)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    int iret = On_NodReq_Hqtt1_Pub_Value(JsonData);

    SetCloseAndDelete();

    return iret;
}

// 节点请求， 不断开
int CHttpServerPandoraMatrix::On_NodReq_Hqtt1_Pub_Value(CPqStack &JsonData)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValue __reqMsg;
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

        ez_printf_debug("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strDevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        int iret = -999; // unknow

        std::list<CValueMsg>::const_iterator it = __reqMsg.Values.begin();
        for ( ; it != __reqMsg.Values.end() ; it++)
        {
            iret = 0;

            std::string __strDstIKey="";
            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_strDevID, it->m_strType, it->m_strChannel);
#if 0

            ez_printf_debug("bldCacheKey(%s,%s,%s,%s)=%d\n"
                            , __strDstIKey.c_str()
                            , __reqMsg.m_strDevID.c_str()
                            , it->m_strType.c_str()
                            , it->m_strChannel.c_str()
                            , iret_ValueKey);
#endif

            if (0==iret_ValueKey)
            {
                m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__strDstIKey, it->m_strValue, modPair_If_Not_Exist_Then_Add);

                m_strKeyFrontOfValues = __reqMsg.m_strDevID;
            }
            else
            {
                iret = -999; // unknow
                ez_printf_error("Unknow Value(%s):%s=%s\n", it->m_strChannel.c_str(), it->m_strType.c_str(), it->m_strValue.c_str());
            }
        }// for

        DBG(
            m_pHandlerPandoraMatrix->m_pCacheKeyValue->dump(0);
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
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_Unknow;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), def_PandoraQ_MasAck_PublishValue);

    return 0;
}


// 前台请求
int CHttpServerPandoraMatrix::On_Req_Hqtt1_Get_Value(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValue __reqMsg;
    CPqMsg_requestMsgPubValueAck __ackMsg;

    __ackMsg.Result = PssMsg_Value_Result_Unknow;
    __ackMsg.Reason = PssMsg_Value_ResultMsg_Unknow;

    __ackMsg.m_strDevID = __reqMsg.m_strDevID;

    do
    {
        if (!JsonData.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());
        ez_printf_info("%s:%d\n",  "Req Values.Num", __reqMsg.Values.size());
        // 名字不匹配
        if (m_strPssSid != __reqMsg.m_strDevID)
        {
            ez_printf_error("Unknow PssSid.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }


        int igetNode = 0;

        // udp无连接
        if (strstr(__reqMsg.m_strDevID.c_str(), def_DevIdUdpServerPandoraMatrix))
        {}
        else // tcp 判断是否在线
        {
            igetNode = m_pHandlerPandoraMatrix->getNode(NULL, __reqMsg.m_strDevID);
        }

        if (0 == igetNode)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
            //ez_printf_debug("OK\n");
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;
            ez_printf_debug("Offline\n");
            break;
        }

        // which value?
        if (__reqMsg.Values.size() < 1)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnknowReq;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnknowReq;

            ez_printf_error("No requested.\n");
            break;
        }

        __ackMsg.m_strDevID = __reqMsg.m_strDevID;

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        std::list<CValueMsg>::const_iterator it = __reqMsg.Values.begin();
        for ( ; it != __reqMsg.Values.end() ; it++)
        {
            std::string __strDstIKey;

            int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_strDevID, it->m_strType, it->m_strChannel);
            ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKey.c_str());

            if (0==iret_ValueKey)
            {
                std::string __strValue;
                int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

                if (0==iret_getPair)
                {
                    ez_printf_debug("Key:%s, Val:%s\n",  __strDstIKey.c_str(), __strValue.c_str());
                    CValueMsg Value;
                    Value.m_strChannel = it->m_strChannel;
                    Value.m_strType = it->m_strType;
                    Value.m_strValue = __strValue;
                    __ackMsg.Values.push_back(Value);
                }
            }

        }
        //m_pHandlerPandoraMatrix->m_pCacheKeyValue->dump(0);

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    //std::cout << "++++++++++++" << __stack.ToString() << std::endl;

    SetCloseAndDelete();

    return 0;
}

int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_GetValueList(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValueList __reqMsg;
    CPqMsg_requestMsgPubValueListAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_GroupID.c_str(), __reqMsg.m_strGroupID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        // Demo
        __ackMsg.m_strGroupID = "701818788";

        {
            CPqMsg_requestMsgPubValue __DeviceValues;
            CValueMsg Value;

            __DeviceValues.m_strDevID = "G1751453539";

            Value.m_strChannel = "1";
            Value.m_strType = "Temperature";
            Value.m_strValue = "15";
            __DeviceValues.Values.push_back(Value);


            Value.m_strChannel = "2";
            Value.m_strType = "Humidity";
            Value.m_strValue = "55";
            __DeviceValues.Values.push_back(Value);

            __ackMsg.DeviceValues.push_back(__DeviceValues);
        }

        {
            CPqMsg_requestMsgPubValue __DeviceValues;
            CValueMsg Value;

            __DeviceValues.m_strDevID = "G1751453540";

            Value.m_strChannel = "1";
            Value.m_strType = "Temperature";
            Value.m_strValue = "15";
            __DeviceValues.Values.push_back(Value);


            Value.m_strChannel = "2";
            Value.m_strType = "Humidity";
            Value.m_strValue = "55";
            __DeviceValues.Values.push_back(Value);

            __ackMsg.DeviceValues.push_back(__DeviceValues);
        }


        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-999)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_SetValueList(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValueList __reqMsg;
    CPqMsg_requestMsgPubValueListAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_GroupID.c_str(), __reqMsg.m_strGroupID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        // Demo
        __ackMsg.m_strGroupID = "701818788";

        {
            CPqMsg_requestMsgPubValue __DeviceValues;
            CValueMsg Value;

            __DeviceValues.m_strDevID = "G1751453539";

            Value.m_strChannel = "1";
            Value.m_strType = "Temperature";
            Value.m_strValue = "15";
            __DeviceValues.Values.push_back(Value);


            Value.m_strChannel = "2";
            Value.m_strType = "Humidity";
            Value.m_strValue = "55";
            __DeviceValues.Values.push_back(Value);

            __ackMsg.DeviceValues.push_back(__DeviceValues);
        }

        {
            CPqMsg_requestMsgPubValue __DeviceValues;
            CValueMsg Value;

            __DeviceValues.m_strDevID = "G1751453540";

            Value.m_strChannel = "1";
            Value.m_strType = "Temperature";
            Value.m_strValue = "15";
            __DeviceValues.Values.push_back(Value);


            Value.m_strChannel = "2";
            Value.m_strType = "Humidity";
            Value.m_strValue = "55";
            __DeviceValues.Values.push_back(Value);

            __ackMsg.DeviceValues.push_back(__DeviceValues);
        }


        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-999)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}

/*
	curl http://localhost:18900/pq/set/plan/schedule \
	    -X POST -H "Content-Type: application/json;charset=utf-8"   \
	    -H "PssSid: G1751453539"                                      \
	    -H "AppKey: 701818788"                                      \
	    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
	    -H "CurTime: 1472451456"                                    \
	    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
	    -d "{ \"DevID\" : \"G1751453539\", \"Schedules\" : [ { \"Date\" : \"*\", \"Day\" : \"*\", \"Hour\" : \"*\", \"Minute\" : \"*\", \"Month\" : \"*\", \"ScheduleID\" : \"S001\", \"Task\" : \"TaskA\" }, { \"Date\" : \"*\", \"Day\" : \"*\", \"Hour\" : \"2\", \"Minute\" : \"1\", \"Month\" : \"*\", \"ScheduleID\" : \"S002\", \"Task\" : \"TaskB\" } ] }"
 
*/
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_SetPlanSchedule(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgSetSchedule __reqMsg;
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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-999)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}

/*
//	curl http://localhost:18900/pq/get/plan/schedule \
//	    -X POST -H "Content-Type: application/json;charset=utf-8"   \
//	    -H "PssSid: G1751453539"                                      \
//	    -H "AppKey: 701818788"                                      \
//	    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
//	    -H "CurTime: 1472451456"                                    \
//	    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
//	    -d "{ \"DevID\" : \"G1751453539\" }"
*/
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_GetPlanSchedule(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgGetSchedule __reqMsg;
    CPqMsg_requestMsgGetScheduleAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        CScheduleMsg Value;

        __ackMsg.Result = "0";
        __ackMsg.Reason = "OK";
        __ackMsg.m_strDevID = "G1751453539";

        Value.m_strScheduleID = "S001";
        Value.m_strMinute = "*";
        Value.m_strHour   = "*";
        Value.m_strDay    = "*";
        Value.m_strMonth  = "*";
        Value.m_strDate   = "*";
        Value.m_strTaskID   = "TaskA";

        __ackMsg.Schedules.push_back(Value);


        Value.m_strScheduleID = "S002";
        Value.m_strMinute = "1";
        Value.m_strHour   = "2";
        Value.m_strTaskID   = "TaskB";

        __ackMsg.Schedules.push_back(Value);


        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-999)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}


/*
//		curl http://localhost:18900/pq/pub/task/start \
//		    -X POST -H "Content-Type: application/json;charset=utf-8"   \
//		    -H "PssSid: G1751453539"                                    \
//		    -H "AppKey: 701818788"                                      \
//		    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
//		    -H "CurTime: 1472451456"                                    \
//		    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
//		    -d "{ \"DevID\" : \"G1751453539\", \"Tasks\" : [ { \"Memo\" : \"M\", \"Option\" : \"O\", \"TaskID\" : \"T001\" }, { \"Memo\" : \"M\", \"Option\" : \"O\", \"TaskID\" : \"T002\" } ] }"
*/
//逻辑控制在这里 比如不能重复开始等
//如果不定义， 则每次都会下发指令
//#define _Func_PubTask_WithLogic 1
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_PubTaskStart   (CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgTaskPub __reqMsg;
    CPqMsg_requestMsgTaskDetailsAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());

        unsigned long ttnow = time(NULL);//ez_sec_get_tick_count();//time(NULL);

        std::string __strDstIKeyTaskStartDt;
        int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKeyTaskStartDt, __reqMsg.m_strDevID, Pq_TaskID_QZ_startSampling, PqMsg_Key_StartDt);
        ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKeyTaskStartDt.c_str());

        std::string strDatetime;
        int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strDstIKeyTaskStartDt, strDatetime);

        if (0==iret_getPair)
        {
            unsigned long  ttTheStartTime;
            ezConvertFromString(ttTheStartTime, strDatetime);
            ez_printf_debug("ttTheStartTime:%ld, ttnow:%ld\n", ttTheStartTime, ttnow);

            // 小于10分钟， 还在做上次任务
            // 取证任务需要10分钟
            if (__reqMsg.m_strTaskID == Pq_TaskID_QZ_startSampling
                && ttnow < (ttTheStartTime+startSampling_Delay_Times))
            {
                ez_printf_debug("startSampling is doing.\n");

                __ackMsg.Result = PssMsg_Value_Result_Redo;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;
#ifdef _Func_PubTask_WithLogic

                break;
#endif

            }

            // 小于10分钟， 还在做上次任务
            // 取证任务需要10分钟
            if (__reqMsg.m_strTaskID == Pq_TaskID_QZ_stopSampling
                && ttnow >= (ttTheStartTime+startSampling_Delay_Times))
            {
                ez_printf_debug("startSampling is stoped now, do not need stop.\n");

                __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;

#ifdef _Func_PubTask_WithLogic

                break;
#endif

            }
        }
        // 还没有开始
        else
        {
            if (__reqMsg.m_strTaskID == Pq_TaskID_QZ_stopSampling)
            {
                ez_printf_debug("startSampling is NOT exist, do not need stop.\n");

                __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;

#ifdef _Func_PubTask_WithLogic

                break;
#endif

            }
        }

        //int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        std::string m_strBody = (char *)m_pBodyBuf->Buf();//((const unsigned char*)m_pBodyBuf->Buf(), m_pBodyBuf->Size());
        CPqMsg_requestMsg __MatchReqMsg;
        __MatchReqMsg.m_strDevID = __reqMsg.m_strDevID;
        __MatchReqMsg.m_strSession = generateSessionId();
        __MatchReqMsg.m_strMsgBody = m_strBody;
        int iRet_MatchReq = m_pHandlerPandoraMatrix->Do_MatchReq(def_PandoraQ_MasReq_PubTaskStart, __MatchReqMsg);
        ARG_USED(iRet_MatchReq);

        if (iRet_MatchReq < 0)
        {
            ez_printf_info("Device Offline.\n");

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }
        //iret = 0;

        if (__reqMsg.m_strTaskID == Pq_TaskID_QZ_startSampling)
        {
            // 记录任务开始时间
            m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__strDstIKeyTaskStartDt, ezConvertToString(ttnow), modPair_If_Not_Exist_Then_Add);
        }
        else if (__reqMsg.m_strTaskID == Pq_TaskID_QZ_stopSampling)
        {
            // 清理开始时间， 防止不能再次开始
            m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__strDstIKeyTaskStartDt, ezConvertToString(0), modPair_If_Not_Exist_Then_Add);

            std::string __strDstIKeyTaskStopDt;
            int iret_ValueKey_Stop = CCacheKeyValue::bldCacheKey(__strDstIKeyTaskStopDt, __reqMsg.m_strDevID, Pq_TaskID_QZ_startSampling, PqMsg_Key_EndDt);
            ARG_USED(iret_ValueKey_Stop);
            m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__strDstIKeyTaskStopDt, ezConvertToString(ttnow), modPair_If_Not_Exist_Then_Add);
        }
        // 返回数值
        //////////////
        CConfigGeneral __cfgGeneral;
        __cfgGeneral.update();
        //if (def_ProductID_Forensics_001 == __cfgGeneral.getConfig().strProductID)
        {
            __ackMsg.m_strDevID = __reqMsg.m_strDevID;
            CTaskValue __taskValue;
            //
            std::list<CValueMsg>  __valuesNeeded;
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_Temperature;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "2";
                __val.m_strType = def_PqMsg_ValueKey_Temperature;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_PH;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_DissolvedOxygen;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_Conductivity;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_Turbidity;
                __valuesNeeded.push_back(__val);
            }

            std::list<CValueMsg>::const_iterator it = __valuesNeeded.begin();
            for ( ; it != __valuesNeeded.end() ; it++)
            {
                std::string __strDstIKey;

                int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_strDevID, it->m_strType, it->m_strChannel);
                ez_printf_info("iret_ValueKey:%d, Key:%s.\n", iret_ValueKey, __strDstIKey.c_str());

                if (0==iret_ValueKey)
                {
                    std::string __strValue;
                    int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

                    if (0==iret_getPair)
                    {
                        ez_printf_info("Key:%s, Val:%s.\n",  __strDstIKey.c_str(), __strValue.c_str());

                        CValueMsg Value;
                        Value.m_strChannel = it->m_strChannel;
                        Value.m_strType = it->m_strType;
                        Value.m_strValue = __strValue;
                        __taskValue.Values.push_back(Value);
                    }
                }

            }

            __ackMsg.TaskValues.push_back(__taskValue);
        }
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}
//Nod 执行结果
int CHttpServerPandoraMatrix::On_NodReq_Hqtt1_PubTaskStart(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);
    return 0;
}


/*
//		curl http://localhost:18900/pq/pub/task/stop \
//		    -X POST -H "Content-Type: application/json;charset=utf-8"   \
//		    -H "PssSid: G1751453539"                                      \
//		    -H "AppKey: 701818788"                                      \
//		    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
//		    -H "CurTime: 1472451456"                                    \
//		    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
//		    -d "{ \"DevID\" : \"G1751453539\", \"Tasks\" : [ { \"Memo\" : \"M\", \"Option\" : \"O\", \"TaskID\" : \"T001\" }, { \"Memo\" : \"M\", \"Option\" : \"O\", \"TaskID\" : \"T002\" } ] }"
*/
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_PubTaskStop    (CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgPubValue __reqMsg;
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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-999)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}


/*
//		curl http://localhost:18900/pq/get/task/status \
//		    -X POST -H "Content-Type: application/json;charset=utf-8"   \
//		    -H "PssSid: G1751453539"                                      \
//		    -H "AppKey: 701818788"                                      \
//		    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
//		    -H "CurTime: 1472451456"                                    \
//		    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
//		    -d "{ \"DevID\" : \"G1751453539\", \"Tasks\" : [ { \"Memo\" : \"Wash\", \"Option\" : \"IoOut 1 On;IoOut 3 On;\", \"TaskID\" : \"T001\" }, { \"Memo\" : \"Working\", \"Option\" : \"IoOut 1 Off&&IoOut 2 On&&IoOut 3 Off\", \"TaskID\" : \"T002\" } ] }"
//			查询全部
//		    -d "{ \"DevID\" : \"G1751453539\", \"Tasks\" : [ { \"Memo\" : \"M\", \"Option\" : \"O\", \"TaskID\" : \"All\" } ] }"
*/
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_GetTaskList  (CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgTaskList __reqMsg;
    CPqMsg_requestMsgTaskListAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        CTaskMsg Value;

        __ackMsg.m_strDevID = "G1751453539";

        Value.m_strTaskID = "T001";
        Value.m_strOption= "IoOut 1 On;IoOut 3 On;";
        Value.m_strMemo= "Wash";

        __ackMsg.Tasks.push_back(Value);


        Value.m_strTaskID = "T002";
        Value.m_strOption= "IoOut 1 Off&&IoOut 2 On&&IoOut 3 Off";
        Value.m_strMemo= "Working";

        __ackMsg.Tasks.push_back(Value);


        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else if (iret==-999)
        {
            __ackMsg.Result = PssMsg_Value_Result_UnderConstruction;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnderConstruction;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}



/*
//		curl http://localhost:18900/pq/get/task/status \
//		    -X POST -H "Content-Type: application/json;charset=utf-8"   \
//		    -H "PssSid: G1751453539"                                      \
//		    -H "AppKey: 701818788"                                      \
//		    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
//		    -H "CurTime: 1472451456"                                    \
//		    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
//		    -d "{ \"DevID\" : \"G1751453539\", \"Tasks\" : [ { \"Memo\" : \"Wash\", \"Option\" : \"IoOut 1 On;IoOut 3 On;\", \"TaskID\" : \"T001\" }, { \"Memo\" : \"Working\", \"Option\" : \"IoOut 1 Off&&IoOut 2 On&&IoOut 3 Off\", \"TaskID\" : \"T002\" } ] }"
//			查询全部
//		    -d "{ \"DevID\" : \"G1751453539\", \"Tasks\" : [ { \"Memo\" : \"M\", \"Option\" : \"O\", \"TaskID\" : \"All\" } ] }"
*/
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_GetTaskDetails  (CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgTaskDetails __reqMsg;
    CPqMsg_requestMsgTaskDetailsAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_DevID.c_str(), __reqMsg.m_strDevID.c_str());

        // 是否在线
        if (NULL == m_pHandlerPandoraMatrix->GetPandoraNode(__reqMsg.m_strDevID))
        {
            ez_printf_info("Device Offline.\n");

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }

        unsigned long ttnow = time(NULL);//ez_sec_get_tick_count();//time(NULL);

        std::string __strDstIKeyTaskStartDt;
        int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKeyTaskStartDt, __reqMsg.m_strDevID, Pq_TaskID_QZ_startSampling, PqMsg_Key_StartDt);
        ez_printf_debug("iret_ValueKey:%d, Key:%s\n", iret_ValueKey, __strDstIKeyTaskStartDt.c_str());

        std::string strDatetime;
        unsigned long  ttTheStartTime = 0;
        int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strDstIKeyTaskStartDt, strDatetime);
        if (0==iret_getPair)
        {
            ezConvertFromString(ttTheStartTime, strDatetime);
            ez_printf_debug("ttTheStartTime:%ld, ttnow:%ld\n", ttTheStartTime, ttnow);

            // 手动停止的
            if (ttTheStartTime < 1)
            {
                ez_printf_debug("startSampling is interrupted.\n");

                __ackMsg.Result = PssMsg_Result_PandoraMatrix_Interrupted;
                __ackMsg.Reason = PssMsg_ResultMsg_PandoraMatrix_Interrupted;

#ifdef _Func_PubTask_WithLogic

                break;
#endif

            }
            // 小于10分钟， 还在做上次任务
            // 取证任务需要10分钟
            else if (__reqMsg.m_strTaskID == Pq_TaskID_QZ_startSampling  \
                     && (ttnow < (ttTheStartTime+startSampling_Delay_Times))   )
            {
                ez_printf_debug("startSampling is doing.\n");

                __ackMsg.Result = PssMsg_Result_PandoraMatrix_Progressing;
                __ackMsg.Reason = PssMsg_ResultMsg_PandoraMatrix_Progressing;

#ifdef _Func_PubTask_WithLogic

                break;
#endif

            }
            else
            {
                ez_printf_debug("startSampling is done.\n");

                __ackMsg.Result = PssMsg_Result_PandoraMatrix_Progressed;
                __ackMsg.Reason = PssMsg_ResultMsg_PandoraMatrix_Progressed;

                //break;
            }

        }
        else
        {
            ez_printf_debug("startSampling has NOT been started.\n");

            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;

#ifdef _Func_PubTask_WithLogic

            break;
#endif

        }

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        {
            __ackMsg.m_strDevID = __reqMsg.m_strDevID;
            CTaskValue __taskValue;
            __taskValue.m_strTaskID = __reqMsg.m_strTaskID;//Pq_TaskID_QZ_startSampling;
            __taskValue.m_strStartDt = ezConvertToString(ttTheStartTime);

            //
            std::list<CValueMsg>  __valuesNeeded;
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_Temperature;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "2";
                __val.m_strType = def_PqMsg_ValueKey_Temperature;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_PH;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_DissolvedOxygen;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_Conductivity;
                __valuesNeeded.push_back(__val);
            }
            {
                CValueMsg __val;
                __val.m_strChannel = "1";
                __val.m_strType = def_PqMsg_ValueKey_Turbidity;
                __valuesNeeded.push_back(__val);
            }

            std::list<CValueMsg>::const_iterator it = __valuesNeeded.begin();
            for ( ; it != __valuesNeeded.end() ; it++)
            {
                std::string __strDstIKey;

                int iret_ValueKey = CCacheKeyValue::bldCacheKey(__strDstIKey, __reqMsg.m_strDevID, it->m_strType, it->m_strChannel);
                ez_printf_info("iret_ValueKey:%d, Key:%s.\n", iret_ValueKey, __strDstIKey.c_str());

                if (0==iret_ValueKey)
                {
                    std::string __strValue;
                    int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strDstIKey, __strValue);

                    if (0==iret_getPair)
                    {
                        ez_printf_info("Key:%s, Val:%s.\n",  __strDstIKey.c_str(), __strValue.c_str());

                        CValueMsg Value;
                        Value.m_strChannel = it->m_strChannel;
                        Value.m_strType = it->m_strType;
                        Value.m_strValue = __strValue;
                        __taskValue.Values.push_back(Value);
                    }
                }

            }

            __ackMsg.TaskValues.push_back(__taskValue);
        }
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return 0;
}


/*
//		curl http://vsa.pq.ezlibs.com:18900/pq/get/device/list \
//		    -X POST -H "Content-Type: application/json;charset=utf-8"   \
//		    -H "PssSid: G1751453539"                                      \
//		    -H "AppKey: 701818788"                                      \
//		    -H "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c"            \
//		    -H "CurTime: 1472451456"                                    \
//		    -H "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b"     \
//		    -d "{ \"GroupID\" : \"G1751453539\", \"PageSize\" : \"9999\", \"Pages\" : \"1\" }"
*/
int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_GetDeviceList  (CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    // 结构体复用
    CPqMsg_requestMsgDeviceList __reqMsg;
    CPqMsg_requestMsgDeviceListAck __ackMsg;

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

        ez_printf_info("%s:%s\n",  PqMsg_Key_GroupID.c_str(), __reqMsg.m_strGroupID.c_str());

        int iret = -999;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        std::string strGroupID;

        if (__reqMsg.m_strGroupID == "c7d56fc0723a766824f554c4d00f28ea16c2c36b")
        {
            strGroupID.clear();// = ""; // 此时查所有
        }
        else if (__reqMsg.m_strGroupID == "701818788")
        {
            strGroupID = "SuoAo";
        }
        else
        {
            strGroupID = __reqMsg.m_strGroupID;
        }

        m_pHandlerPandoraMatrix->List( __ackMsg, strGroupID, 0, 0);

        __ackMsg.m_strGroupID = __reqMsg.m_strGroupID;
        __ackMsg.m_iTotalCount = __ackMsg.Devices.size();
        __ackMsg.m_iPages = 1;

        ez_printf_debug("%s:%d\n",  "TotalCount", __ackMsg.m_iTotalCount);

        iret = 0;
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        if (iret == 0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (iret==-1)
        {
            __ackMsg.Result = PssMsg_Value_Result_NoNeedThisOpr;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NoNeedThisOpr;
        }
        else
        {
            // default ret
            __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest;
        }
    }
    while(0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());

    SetCloseAndDelete();

    return 0;
}

int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_SetShpc(CPqStack &JsonData)
{
    CPqMsg_requestMsgSetShpcAck __ackMsg;

    int iret = 0;
    //std::string m_strBody = (char *)m_pBodyBuf->Buf();

    do
    {
        CPqMsg_requestMsgSetShpc __reqMsg;
#if 0

        CPqStack __CezlibsWebServiceIF;

        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);

        if (!bRet)
        {
            ez_printf_error("CPqStack.Parse failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -1;
            break;
        }
        bRet = __CezlibsWebServiceIF.Decode(__reqMsg);
#else

        bool bRet = JsonData.Decode(__reqMsg);

#endif

        if (!bRet)
        {
            ez_printf_error("CPqStack.Decode failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -2;
            break;
        }

        __ackMsg.m_strDevID = __reqMsg.m_strDevID;

        m_iMsgCountGet ++;

        ez_printf_debug("DevID:%s\n", __reqMsg.m_strDevID.c_str());
        ez_printf_debug("sHost:%s\n", __reqMsg.m_strsHost.c_str());
        ez_printf_debug("sPort:%d\n", __reqMsg.m_strsPort);
        ez_printf_debug("sTocken:%s\n", __reqMsg.m_strsTocken.c_str());

        //////////////////////////////////////////////////////////////////////////
        //数据检查
        if (__reqMsg.m_strDevID.length() < 2
            ||__reqMsg.m_strFunc.length() < 2
            ||__reqMsg.m_strsHost.length() < 2
            ||__reqMsg.m_strsPort < 1
            ||__reqMsg.m_strsPort > 65534
            ||__reqMsg.m_strsTocken.length() < 2
           )
        {
            ez_printf_error("%s\n", PssMsg_Value_ResultMsg_InvalidData.c_str());

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 基本信息
        __ackMsg.m_strDevID   = __reqMsg.m_strDevID  ;
        __ackMsg.m_strFunc    = __reqMsg.m_strFunc   ;
        __ackMsg.m_strsHost   = __reqMsg.m_strsHost  ;
        __ackMsg.m_strsPort   = __reqMsg.m_strsPort  ;
        __ackMsg.m_strsTocken = __reqMsg.m_strsTocken;

        __ackMsg.m_i_sTls = __reqMsg.m_i_sTls;

        //端口
        // 目前不自动分配， 外部指定

        // 返回 现原样返回
        std::copy(__reqMsg.Shpcs.begin(), __reqMsg.Shpcs.end(), std::back_inserter(__ackMsg.Shpcs));

        //////////////////////////////////////////////////////////////////////////
        CPqMsg_requestMsg __MatchReqMsg;
        __MatchReqMsg.m_strDevID = __reqMsg.m_strDevID;
        __MatchReqMsg.m_strSession = generateSessionId();
        __MatchReqMsg.m_strMsgBody = (char *)m_pBodyBuf->Buf();

        int iRet_MatchReq = m_pHandlerPandoraMatrix->Do_MatchReq(def_PandoraQ_MasReq_ShpSet, __MatchReqMsg);
        ARG_USED(iRet_MatchReq);

        if (iRet_MatchReq < 0)
        {
            ez_printf_info("Device Offline.\n");

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }

        std::string __strDstIKey="";
        CCacheKeyValue::bldCacheKey2(__strDstIKey, def_PandoraQ_Cfg_ShpcSet+__reqMsg.m_strDevID+__reqMsg.m_strFunc);

        //m_pHandlerPandoraMatrix->m_pCacheWithDb_NodeConfig->modPair(__strDstIKey, __MatchReqMsg.m_strMsgBody, modPair_If_Not_Exist_Then_Add);
        // 记录进零时缓存
        // 记录，返回的时候认定
        m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__MatchReqMsg.m_strSession, __strDstIKey, modPair_If_Not_Exist_Then_Add);
        m_pHandlerPandoraMatrix->m_pCacheKeyValue->modPair(__strDstIKey, __MatchReqMsg.m_strMsgBody, modPair_If_Not_Exist_Then_Add);

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        iret = 0;
    }
    while (0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return iret;
}

int CHttpServerPandoraMatrix::On_Req_PandoraQ_NodReq_SetShpc(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    int iret = 0;

    CPqStack __stackReqThis;

    // todo
    // 在此优化找到对应的session 返回给客户端
    do
    {
        // 对应Node 的Ack
        CPqMsg_requestMsgAck __ackMsgNode;
        CAckMsgSimple __ackMsg;

        if (!JsonData.Decode(__ackMsgNode))
        {
            ez_printf_error("Failed Decode.\n");

            //__ackMsg.Result = PssMsg_Value_Result_InvalidData;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
        //ez_printf_debug("__ackMsgNode.m_strSession:[%s], [%s]\n", __ackMsgNode.m_strSession.c_str(), __ackMsg.m_strDevID.c_str());
        ///////////////////////
        if (!__stackReqThis.Parse(__ackMsgNode.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            //__ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }
        if (!__stackReqThis.Decode(__ackMsg))
        {
            ez_printf_error("Failed Decode.\n");

            //__ackMsg.Result = PssMsg_Value_Result_InvalidData;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        //ez_printf_debug("__ackMsgNode.m_strSession : %s, DevID:%s\n", __ackMsgNode.m_strSession.c_str(), __ackMsgNode.m_strDevID.c_str());
        //ez_printf_debug("ackMsg.Result : %s, %s\n", __ackMsg.Result.c_str(), __ackMsg.Reason.c_str());

        std::string __strValue;
        int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__ackMsgNode.m_strSession, __strValue);
        ez_printf_debug("getPair(%d) : %s, %s\n", iret_getPair, __ackMsgNode.m_strSession.c_str(), __strValue.c_str());

        std::string __strValueDbCache;
        iret_getPair = m_pHandlerPandoraMatrix->m_pCacheKeyValue->getPair(__strValue, __strValueDbCache);
        ez_printf_debug("getPair(%d) : %s, %s\n", iret_getPair, __strValue.c_str(), __strValueDbCache.c_str());

        // 入库
        m_pHandlerPandoraMatrix->m_pCacheWithDb_NodeConfig->modPair(__strValue, __strValueDbCache, modPair_If_Not_Exist_Then_Add);
    }
    while (0);

    return iret;
}

int CHttpServerPandoraMatrix::On_Req_PandoraQ_Req_GetShpc(CPqStack &JsonData)
{
    CPqMsg_requestMsgSetShpcAck __ackMsg;

    int iret = 0;
    //std::string m_strBody = (char *)m_pBodyBuf->Buf();

    do
    {
        CPqMsg_requestMsgSetShpc __reqMsg;

        bool bRet = JsonData.Decode(__reqMsg);

        if (!bRet)
        {
            ez_printf_error("CPqStack.Decode failed.");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            iret = -2;
            break;
        }

        __ackMsg.m_strDevID = __reqMsg.m_strDevID;
        __ackMsg.m_strFunc = __reqMsg.m_strFunc;

        m_iMsgCountGet ++;


        //////////////////////////////////////////////////////////////////////////

        ez_printf_debug("DevID:%s\n", __reqMsg.m_strDevID.c_str());
        ez_printf_debug("Func:%s\n", __reqMsg.m_strFunc.c_str());

        // snd to node
        CPqMsg_requestMsg __MatchReqMsg;
        __MatchReqMsg.m_strDevID = __reqMsg.m_strDevID;
        __MatchReqMsg.m_strSession = generateSessionId();
        __MatchReqMsg.m_strMsgBody = (char *)m_pBodyBuf->Buf();

        ez_printf_debug("Do_MatchReq:%s\n", def_PandoraQ_MasReq_ShpGet);

        int iRet_MatchReq = m_pHandlerPandoraMatrix->Do_MatchReq(def_PandoraQ_MasReq_ShpGet, __MatchReqMsg);
        ARG_USED(iRet_MatchReq);

        if (iRet_MatchReq < 0)
        {
            ez_printf_info("Device Offline.\n");

            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;

            break;
        }

        ez_printf_debug("Do_MatchReq:%s -- OK\n", def_PandoraQ_MasReq_ShpGet);

        //////////////////////////////////////////////////////////////////////////
        //数据检查
        if (__reqMsg.m_strDevID.length() < 2
            ||__reqMsg.m_strFunc.length() < 2
           )
        {
            ez_printf_error("%s\n", PssMsg_Value_ResultMsg_InvalidData.c_str());

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            iret = -2;
            break;
        }

        std::string __strDstIKey = "";
        std::string __strTheKeyString = def_PandoraQ_Cfg_ShpcSet+__reqMsg.m_strDevID+__reqMsg.m_strFunc;
        std::string __strValueDbCache = "";
        CCacheKeyValue::bldCacheKey2(__strDstIKey, __strTheKeyString);
        int iret_getPair = m_pHandlerPandoraMatrix->m_pCacheWithDb_NodeConfig->getPair(__strDstIKey, __strValueDbCache);
        //ez_printf_debug("getPair(%d):[%s][%s]\n", iret_getPair, __strDstIKey.c_str());

        if (0 == iret_getPair)
        {
            //ez_printf_debug("getPair: %s, %s\n", __strDstIKey.c_str(), __strValueDbCache.c_str());

            CPqStack __stackShpcMsgThis;
            CPqMsg_requestMsgSetShpcAck __ShpcSetMsg;
            if (!__stackShpcMsgThis.Parse(__strValueDbCache))
            {
                ez_printf_error("Failed Parse.\n");

                __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

                iret = -2;
                break;
            }
            if (!__stackShpcMsgThis.Decode(__ShpcSetMsg))
            {
                ez_printf_error("Failed Decode.\n");

                __ackMsg.Result = PssMsg_Value_Result_InvalidData;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

                iret = -3;
                break;
            }

            // 返回 现原样返回
            __ackMsg.m_strsHost = __ShpcSetMsg.m_strsHost;
            __ackMsg.m_strsPort = __ShpcSetMsg.m_strsPort;
            __ackMsg.m_strsTocken = __ShpcSetMsg.m_strsTocken;
            __ackMsg.m_i_sTls = __ShpcSetMsg.m_i_sTls;
            std::copy(__ShpcSetMsg.Shpcs.begin(), __ShpcSetMsg.Shpcs.end(), std::back_inserter(__ackMsg.Shpcs));
        }
        else
        {
            ez_printf_error("Failed getPair(%d):%s\n", iret_getPair, __strTheKeyString.c_str());

            __ackMsg.Result = PssMsg_Result_PandoraNode_NotFoundCfgFile;
            __ackMsg.Reason = PssMsg_ResultMsg_PandoraNode_NotFoundCfgFile+":"+__strTheKeyString;
            iret = -3;
            break;
        }

        CHttpServerPandoraMatrix *pMatrixNode = NULL;
        pMatrixNode = m_pHandlerPandoraMatrix->GetPandoraNode(__reqMsg.m_strDevID);

        if (NULL == pMatrixNode)
        {
            __ackMsg.Result = PssMsg_Value_Result_DeviceOffline;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_DeviceOffline;
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }

        iret = 0;
    }
    while (0);

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(__stack.ToString(), GetUrl());
    SetCloseAndDelete();

    return iret;
}

//接收到的消息
int CHttpServerPandoraMatrix::On_Req_PandoraQ_NodReq_GetShpc(CPqStack &JsonData)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    int iret = 0;

    CPqStack __stackReqThis;

    do
    {
        // 对应Node 的Ack
        CPqMsg_requestMsgAck __ackMsgNode;
        CPqMsg_requestMsgSetShpcAck __ackMsg;

        if (!JsonData.Decode(__ackMsgNode))
        {
            ez_printf_error("Failed Decode.\n");

            //__ackMsg.Result = PssMsg_Value_Result_InvalidData;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
        //ez_printf_debug("__ackMsgNode.m_strSession:[%s], [%s]\n", __ackMsgNode.m_strSession.c_str(), __ackMsg.m_strDevID.c_str());
        ///////////////////////
        if (!__stackReqThis.Parse(__ackMsgNode.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            //__ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }
        if (!__stackReqThis.Decode(__ackMsg))
        {
            ez_printf_error("Failed Decode.\n");

            //__ackMsg.Result = PssMsg_Value_Result_InvalidData;
            //__ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_debug("__ackMsgNode.m_strSession : %s, DevID:%s\n", __ackMsgNode.m_strSession.c_str(), __ackMsgNode.m_strDevID.c_str());
        ez_printf_debug("ackMsg.Result : %s, %s\n", __ackMsg.Result.c_str(), __ackMsg.Reason.c_str());

        //保存
        std::string __strTheKeyString = def_PandoraQ_Cfg_ShpcSet+__ackMsg.m_strDevID+__ackMsg.m_strFunc;
        std::string __strDstIKey = "";
        CCacheKeyValue::bldCacheKey2(__strDstIKey, __strTheKeyString);
        std::string __strValueDbCache = "";

        CPqStack __stack;
        __stack.Encode(__ackMsg);

        //ez_printf_debug("ackMsg : %s\n", __stack.ToString().c_str());

        int iret_modPair = m_pHandlerPandoraMatrix->m_pCacheWithDb_NodeConfig->modPair(__strDstIKey, __stack.ToString(), modPair_If_Not_Exist_Then_Add);
        ez_printf_debug("iret_modPair : %d\n", iret_modPair);
    }
    while (0);

    return iret;
}

