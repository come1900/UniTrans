/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerMinerMtService.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: HttpServerMinerMtService.cpp 5884 2021-04-20 14:27:24Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-04-20 14:27:27  Create
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
#if defined(_FUNC_Crawler)
#include "../Crawler/ManagerCrawler.h"
#endif // _FUNC_Crawler

#include "AgentMinerMtService.h"
#include "HandlerMinerMtService.h"
#include "HttpServerMinerMtService.h"

// #define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

CHttpServerMinerMtService::CHttpServerMinerMtService(ISocketHandler& h) : CGRestHttpdSocket(h)
{
    m_pHandler = ((CHandlerMinerMtService*)(&(Handler())));

    m_pBodyBuf = new CezBuffer();

    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iIsAuthedHost = 0;

    m_pCfgMinerMtService = NULL;

    m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    m_strUniSid = "";
}

void CHttpServerMinerMtService::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerMinerMtService::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerMinerMtService::~CHttpServerMinerMtService()
{
    if (m_pBodyBuf != NULL)
    {
        delete m_pBodyBuf;
    }

}

void CHttpServerMinerMtService::OnAccept()
{
    ez_printf_info("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());


#ifdef _USE_MODULES_EZCONFIG

    static CConfigMinerMtService __cfg;

    __cfg.update();
    m_pCfgMinerMtService = &__cfg;

    m_iTimeout = m_pCfgMinerMtService->getConfig().iTimeout;

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
    m_strHttpHeader_Server = std::string(def_UniService_MinerMtService_ServerName) + " " + g_Solar.GetVersion() + " " + g_Solar.GetHostID();

    m_strShaChkSum = "043979497";// dbg dev huid
#ifdef _FUNC_gMiniLicense

    m_strShaChkSum = g_Solar.GetHuid();
#endif //_FUNC_gMiniLicense
}

void CHttpServerMinerMtService::OnHeader(const std::string& key,const std::string& value)
{
    if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_AppKey))
    {
        m_strAppKey = value;

        // ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_AppKey, m_strAppKey.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_Nonce))
    {
        m_strNonce = value;

        // ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_Nonce, m_strNonce.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CurTime))
    {
        m_strCurTime = value;

        // ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CurTime, m_strCurTime.c_str());
    }
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CheckSum))
    {
        m_strCheckSum = value;

        // ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CheckSum, m_strCheckSum.c_str());
    }
    else if (!strcasecmp(key.c_str(), APP_AUTH_HTTP_HEAD_KEY_MmtSid))
    {
        m_strUniSid = value;

        // ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_MmtSid, m_strUniSid.c_str());
    }
    else
    {
        HttpdSocket::OnHeader(key, value);
    }
}

void CHttpServerMinerMtService::OnDisconnect()
{
    ez_printf_debug("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerMinerMtService::Exec()
{
    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerMinerMtService::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();
    m_pBodyBuf->Reset();


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
    if (m_pCfgMinerMtService
        && 1 == m_pCfgMinerMtService->getConfig().iAccounts)
    {
        if (GetUrl() == def_Pandora_Req_manAppKeyCreate
            || GetUrl() == def_Pandora_Req_manAppKeyCancel)
        {
            ez_printf_debug("Noneed Auth:%s\n", GetUrl().c_str());

            return;
        }
    }

    do
    {
        if (m_strUniSid.length() < APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN)
        {
            ez_printf_info("%s not found, Req:%s\n", def_UniService_MinerMtService_Sid, +GetUrl().c_str());

            RetGeneralMsg(PssMsg_Value_Result_NotPermittedAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);

            break;
        }

        if (m_pCfgMinerMtService
            && BitGet(m_pCfgMinerMtService->getConfig().iAuth, 0) == 1)
        {
            //ez_printf_debug("-->> do secret key chk\n");

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

            //ez_printf_debug("ez_sha1_str:%s\n", src);
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

            //ez_printf_debug("appSecret Auth Src:%s\n", src);
            //ez_printf_debug("dst:%s\n", dst);
        }

        if (m_pCfgMinerMtService
            && BitGet(m_pCfgMinerMtService->getConfig().iAuth, 1) == 1)
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
                // strCode = PssMsg_Value_Result_IllegalAccess;
                // strMsg = PssMsg_Value_ResultMsg_NotPermittedAccess;
                m_strRetCode = "203";
                RetGeneralMsg(PssMsg_Value_Result_IllegalAccess, PssMsg_Value_ResultMsg_NotPermittedAccess);
                break;
            }
        }

        //ok
        return;
    }
    while(0);

    // RetGeneralMsg(strCode, strMsg, m_strRetCode);
    Reset(); // prepare for next request
    SetCloseAndDelete();
    // #endif
}

void CHttpServerMinerMtService::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );
    AddResponseHeader("Server", std::string(def_UniService_MinerMtService_ServerName) + " " + g_Solar.GetVersion());

    // we are json
    // http://www.ietf.org/rfc/rfc4627.txt
    AddResponseHeader("Content-type", "application/json;charset=utf-8");

    AddResponseHeader("Connection", "Keep-Alive");

    if (ContentLength>0)
    {
        AddResponseHeader( "Content-length", Utility::l2string((long)ContentLength) );
    }

    if (matchRequest.size()>0)
    {
        AddResponseHeader( "Match-request", matchRequest);
    }

    if (m_strUniSid.size()>0)
    {
        AddResponseHeader( def_UniService_MinerMtService_Sid, m_strUniSid);
    }

    SendResponse();
}

void CHttpServerMinerMtService::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CpromQLStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
}


/** Chunk of http body data recevied. */
void CHttpServerMinerMtService::OnData(const char *p,size_t l)
{
    if (p==NULL || l<1)
    {
        return;
    }

    //ez_printf_debug("%s:Size:%ld,Len:%ld\n", __FUNCTION__, m_pBodyBuf->Size(), l);

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

void CHttpServerMinerMtService::OnDataComplete()
{
    //验证失败不处理-
    if (CloseAndDelete())
    {
        return;
    }

    std::string __strBody ;
    CpromQLStack __uStack;

    unsigned char theTail[1] = {'\0'};
    m_pBodyBuf->Append(theTail, 1);
    __strBody = (char *)m_pBodyBuf->Buf();

    // 不需要解析为json数据
    if (GetUrl() == def_UniTrans_Req_hello
        ||GetUrl() == def_UniTrans_Req_Echo
        ||GetUrl() == def_UniTrans_Req_describe
        ||GetUrl() == def_UniTrans_Req_ip
       )
    {
        //__strBody = "{\"MmtSid\" : \"81229956A9B\"}";
    }
    else
    {
        if (__strBody.size() < 5 /*strlen("[{}]")*/)
        {
            ez_printf_info("Body(%s) is not json.\n", __strBody.c_str());

            RetGeneralMsg(PssMsg_Value_Result_InvalidDataFormat, PssMsg_Value_ResultMsg_InvalidDataFormat);

            return;
        }

        // have []  of json string
        //ez_Trim(__strBody, "[");
        //ez_Trim(__strBody, "]");

        bool bRet = __uStack.Parse(__strBody);
        if (bRet)
        {
            // m_iMsgCountGet ++;
        }
        else
        {
            ez_printf_error("__uStack.Parse failed.");

            RetGeneralMsg(PssMsg_Value_Result_InvalidDataFormat, PssMsg_Value_ResultMsg_InvalidDataFormat);

            return;
        }

    }

    ez_printf_debug("GetUrl:%s\n", GetUrl().c_str());
    ez_printf_debug("Body:%s\n", __strBody.c_str());

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_reqHello(__strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_Echo)
    {
        Do_reqEcho(__strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_describe)
    {
        Do_reqDescribe(__strBody);
    }
    else if (GetUrl() == def_UniTrans_Req_ip)
    {
        Do_reqIp(__strBody);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_up)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_HostUp);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_cpu_usage)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_CpuUsage);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_mem_usage)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_MemUsage);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_mem_total)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_MemTotal);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_mem_available)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_MemAvailable);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_disk_usage)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_DiskUsage);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_disk_total)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_DiskTotal);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_disk_available)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_DiskAvailable);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_network_upperrate)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_NetworkUpperrate);
    }
    else if (GetUrl()==def_Maintenance_Req_mt_query_host_network_downwardrate)
    {
        On_Req_MinerMtService_mt_query_PromQlValue(__uStack, PromQlMsg_Key_NetworkDownwardrate);
    }
    // 检测开启
    else if (GetUrl()==def_Maintenance_Req_mt_start_ipcam_dialing)
    {
        On_Req_MinerMtService_mt_dialing_ipcam_start(__uStack);
    }
    // 检测结果(按设备查)
    else if (GetUrl()==def_Maintenance_Req_mt_query_ipcam_dialing)
    {
        On_Req_MinerMtService_mt_query_ipcam_dialing(__uStack);
    }
    // 批量取走(Cut)检测结果
    else if (GetUrl()==def_Maintenance_Req_mt_cut_ipcam_dialing)
    {
        On_Req_MinerMtService_mt_cut_ipcam_dialing(__uStack);
    }
    else
    {
        m_strRetCode = PssMsg_Value_Result_MethodNotAllowed;
        RetGeneralMsg(PssMsg_Value_Result_MethodNotAllowed, PssMsg_Value_ResultMsg_MethodNotAllowed + std::string(" : ") + GetUrl());
    }

    // Reset(); // prepare for next request
    SetCloseAndDelete();
}

void CHttpServerMinerMtService::Update()
{
    unsigned long ttNow = ez_sec_get_tick_count(); //time(NULL);

    if (ttNow - m_ttOndata > m_iTimeout)
    {
        ez_printf_info("idle Timeout(%d), conn:%d\n", GetSocket(), m_iTimeout);

        SetCloseAndDelete();
        return;
    }
}

int CHttpServerMinerMtService::On_Req_MinerMtService_mt_query_PromQlValue( CpromQLStack &restStack, const std::string strValueType)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CMsgMmt_requestMsg __reqMsg;
    CMsgPromQlValue __ackMsg;

    __ackMsg.Result = PssMsg_Value_ResultMsg_Success;
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

        if (m_strUniSid != __reqMsg.m_strMmtSid)
        {
            ez_printf_error("Invalid %s, %s\n", m_strUniSid.c_str(), __reqMsg.m_strMmtSid.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        CpromQLStack __Stack;
        //std::string result = "[{\"status\":\"success\",\"data\":{\"resultType\":\"vector\",\"result\":[{\"metric\":{\"instance\":\"WXVM-08-VAS-WPC-99\"},\"value\":[1618469852.042,\"0.8691911254708251\"]},{\"metric\":{\"instance\":\"gts11AtHXYun\"},\"value\":[1618469852.042,\"14.300076331614353\"]}]}}]";

        std::string strValue;
        if (PromQlMsg_Key_HostUp == strValueType)
        {
            g_ManagerCrawler.getHost_up(strValue);
        }
        else if (PromQlMsg_Key_CpuUsage == strValueType)
        {
            g_ManagerCrawler.getHost_cpu_usage(strValue);
        }
        else if (PromQlMsg_Key_MemUsage == strValueType)
        {
            g_ManagerCrawler.getHost_mem_usage(strValue);
        }
        else if (PromQlMsg_Key_MemTotal == strValueType)
        {
            g_ManagerCrawler.getHost_mem_total(strValue);
        }
        else if (PromQlMsg_Key_MemAvailable == strValueType)
        {
            g_ManagerCrawler.getHost_mem_available(strValue);
        }
        else if (PromQlMsg_Key_DiskUsage == strValueType)
        {
            g_ManagerCrawler.getHost_disk_usage(strValue);
        }
        else if (PromQlMsg_Key_DiskTotal == strValueType)
        {
            g_ManagerCrawler.getHost_disk_total(strValue);

            ez_printf_debug("strValue:%s\n", strValue.c_str());
        }
        else if (PromQlMsg_Key_DiskAvailable == strValueType)
        {
            g_ManagerCrawler.getHost_disk_available(strValue);
        }
        else if (PromQlMsg_Key_NetworkUpperrate == strValueType)
        {
            g_ManagerCrawler.getHost_network_upperrate(strValue);
        }
        else if (PromQlMsg_Key_NetworkDownwardrate == strValueType)
        {
            g_ManagerCrawler.getHost_network_downwardrate(strValue);
        }
        else
        {
            ez_printf_error("Invalid %s\n", strValueType.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }


        ez_Trim(strValue, "[");
        ez_Trim(strValue, "]");

        __Stack.Parse(strValue);
        __Stack.Decode(__ackMsg);

    }
    while(0);

    CpromQLStack __stack;
    __stack.Encode(__ackMsg, strValueType);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    return 0;
}

int CHttpServerMinerMtService::On_Req_MinerMtService_mt_dialing_ipcam_start( CpromQLStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CMmtMsgCamDialing __reqMsg;
    CAckMsgSimple __ackMsg;

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

        if (m_strUniSid != __reqMsg.DevId)
        {
            ez_printf_error("Invalid %s, %s\n", m_strUniSid.c_str(), __reqMsg.DevId.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        int iret_startCrawlerMmtPlayUrl = g_ManagerCrawler.startCrawlerMmtPlayUrl(__reqMsg.DevId);

        ez_printf_debug("startCrawlerMmtPlayUrl(%d):%s\n", iret_startCrawlerMmtPlayUrl, __reqMsg.DevId.c_str());

        if (0 == iret_startCrawlerMmtPlayUrl)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else if (-2 == iret_startCrawlerMmtPlayUrl)
        {
            __ackMsg.Result = PssMsg_Value_Result_Redo;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Redo;
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;
        }
    }
    while(0);

    CpromQLStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);

    // ez_printf_debug("__stack.ToString():[%s]\n", __stack.ToString().c_str());

    return 0;
}

int CHttpServerMinerMtService::On_Req_MinerMtService_mt_query_ipcam_dialing( CpromQLStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CMsgMmtReqValueCamDialing __reqMsg;
    CMsgMmtReqValueCamDialingAck __ackMsg;

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

        if (m_strUniSid != __reqMsg.DevId)
        {
            ez_printf_error("Invalid %s, %s\n", m_strUniSid.c_str(), __reqMsg.DevId.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }

        __ackMsg.DevId = __reqMsg.DevId;

        static int ivideo_chk = 0;

        CDialingResult __result;
        __result.m_strResult = "play_ok";
        __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
        __result.m_strTime = 1619769995;
        __ackMsg.DialingResults.push_back(__result);

        ivideo_chk++;
        int times = ivideo_chk % 4;

        if (1 == times)
        {
            __result.m_strResult = "video_ok";
            __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
            __result.m_strTime = 1619769998;
            __ackMsg.DialingResults.push_back(__result);
        }
        else if (2 == times)
        {
            __result.m_strResult = "video_black";
            __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
            __result.m_strTime = 1619769998;
            __ackMsg.DialingResults.push_back(__result);
        }
        else if (3 == times)
        {
            __result.m_strResult = "video_green";
            __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
            __result.m_strTime = 1619769998;
            __ackMsg.DialingResults.push_back(__result);
        }

    } while (0);

    CpromQLStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    // ez_printf_debug("__stack.ToString():[%s]\n", __stack.ToString().c_str());

    return 0;
}

int CHttpServerMinerMtService::On_Req_MinerMtService_mt_cut_ipcam_dialing( CpromQLStack &restStack)
{
    ez_printf_debug("On:%s\n", __FUNCTION__);

    CMmtMsgValuesIpcamDialing __reqMsg;
    CMmtMsgValuesIpcamDialingAck __ackMsg;

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

        if (m_strUniSid != __reqMsg.m_strMmtSid)
        {
            ez_printf_error("Invalid %s, %s\n", m_strUniSid.c_str(), __reqMsg.m_strMmtSid.c_str());

            __ackMsg.Result = PssMsg_Value_Result_NotPermittedAccess;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotPermittedAccess;

            break;
        }
#if 0
        CMmtMsgValueCamDialing dialineMsg;
        dialineMsg.DevId = "0000132020000001310162294"; //__reqMsg.DevId;

        static int ivideo_chk = 0;

        CDialingResult __result;
        __result.m_strResult = "play_ok";
        __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
        __result.m_strTime = 1619769995;
        dialineMsg.DialingResults.push_back(__result);

        ivideo_chk++;
        int times = ivideo_chk % 4;

        if (1 == times)
        {
            __result.m_strResult = "video_ok";
            __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
            __result.m_strTime = 1619769998;
            dialineMsg.DialingResults.push_back(__result);
        }
        else if (2 == times)
        {
            __result.m_strResult = "video_black";
            __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
            __result.m_strTime = 1619769998;
            dialineMsg.DialingResults.push_back(__result);
        }
        else if (3 == times)
        {
            __result.m_strResult = "video_green";
            __result.m_strResultMsg = "http://1.2.3.4:18080/abc.jpg";
            __result.m_strTime = 1619769998;
            dialineMsg.DialingResults.push_back(__result);
        }

        __ackMsg.ValuesIpcamDialing.push_back(dialineMsg);
        __ackMsg.Items++;
#else
        int iret_cutMmtMsgValuesIpcamDialing = g_ManagerCrawler.cutMmtMsgValuesIpcamDialing(__ackMsg);

        ez_printf_debug("cutMmtMsgValuesIpcamDialing(%d):%d\n", iret_cutMmtMsgValuesIpcamDialing, __ackMsg.Items);

        if (0 == iret_cutMmtMsgValuesIpcamDialing && __ackMsg.Items>0)
        {
            __ackMsg.Result = PssMsg_Value_Result_Success;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
        }
        else
        {
            __ackMsg.Result = PssMsg_Value_Result_RecQueryFailed;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_RecQueryFailed;
        }
    
#endif
    } while (0);

    CpromQLStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl(), m_strRetCode);
    // ez_printf_debug("__stack.ToString():[%s]\n", __stack.ToString().c_str());

    return 0;
}
