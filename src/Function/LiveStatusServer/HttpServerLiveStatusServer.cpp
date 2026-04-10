/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveStatusServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveStatusServer.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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
#include <iostream>
#include <ez_http_query_parser.h>

#include "../../Logs.h"
#include "../../Solar.h"

#include "../UniService/DefUniService.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveStatusServer.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

#include "../UniService/DefUniService.h"
#include "../Pandora/StreamUtility.h" /*工具*/

#include "HandlerLiveStatusServer.h"
#include "HttpServerLiveStatusServer.h"

#define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

typedef enum __em_body_data_style_t {
    em_body_data_style_string = 0, //do not know
    em_body_data_style_json,    	//
    em_body_data_style_httpQuery,	//split by &
}EM_BODY_DATA_STYLE_T;

CHttpServerLiveStatusServer::CHttpServerLiveStatusServer(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";

    m_iTimeout = HTTPD_CONN_TIMEOUT;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;

    //m_iIsAuthedHost = 0;

    //m_pCfg = NULL;

    //m_ttOnProcMsg = 0;

    // 请求时间  为判断节点是否已返回
    m_llTheReqTime = 0;

    //统一管理
    m_pHandler = ((CHandlerLiveStatusServer *)(&(Handler())));

    m_strPssSid = "";
}

void CHttpServerLiveStatusServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveStatusServer::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

CHttpServerLiveStatusServer::~CHttpServerLiveStatusServer()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerLiveStatusServer::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerLiveStatusServer::OnAccept()
{
    //ez_printf_debug("OnAccept(%d):%s:%d\n", GetSocket(), GetRemoteAddress().c_str(), GetRemotePort());
#ifdef _USE_MODULES_EZCONFIG

    /*static */
    CConfigLiveStatusServer __cfg;
    __cfg.update();
    //m_pCfg = &__cfg;

    m_iTimeout = __cfg.getConfig().iTimeout;
#else

    //SetTimeout(5);
    m_iTimeout = HTTPD_CONN_TIMEOUT;
#endif//

    m_ttOndata = g_Solar.GetRunPeriod();//ez_sec_get_tick_count();//time(NULL);

    //    m_pStreamContainer =  g_AgentPandoraMatrix.GetHandler()->GetPandoraContainer();
    //    assert(m_pStreamContainer);

}

void CHttpServerLiveStatusServer::OnHeader(const std::string& key,const std::string& value)
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

void CHttpServerLiveStatusServer::OnDisconnect()
{
    //ez_printf_debug("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CHttpServerLiveStatusServer::Exec()
{
    //CreateHeader();
    //RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

void CHttpServerLiveStatusServer::OnHeaderComplete()
{
    HttpdSocket::OnHeaderComplete();
}

void CHttpServerLiveStatusServer::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    AddResponseHeader("Server", std::string(def_Pandora_LiveStatusServer_ServerName)+g_Solar.GetVersion());
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
    SendResponse();
}

void CHttpServerLiveStatusServer::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
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

void CHttpServerLiveStatusServer::RetGeneralMsg(const std::string &strCode, const std::string &strMsg)
{
    CAckMsgSimple __ackMsg;

    __ackMsg.Result = strCode;
    __ackMsg.Reason = strMsg;

    CPssStack __stack;
    __stack.Encode(__ackMsg);
    SendMsg(__stack.ToString(), GetUrl());
}


/** Chunk of http body data recevied. */
void CHttpServerLiveStatusServer::OnData(const char *p,size_t l)
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
        RetGeneralMsg(PssMsg_Value_Result_TooMuchData, PssMsg_Value_ResultMsg_TooMuchData);

        return;
    }

    m_strBody += pData;

    m_ttOndata = g_Solar.GetRunPeriod();//ez_sec_get_tick_count();//time(NULL);
}

void CHttpServerLiveStatusServer::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    EM_BODY_DATA_STYLE_T iBodyDataStyle = em_body_data_style_string;
    CPssStack __CezlibsWebServiceIF;
    CPssNotifyMsg __CPssNotifyMsg;

    if (GetUrl().find("pss_Ntf")!= std::string::npos)
    {
        iBodyDataStyle = em_body_data_style_httpQuery;
    }
    else if (GetUrl()== def_UniTrans_Req_hello)
    {
        iBodyDataStyle = em_body_data_style_json;
    }
    else //if (GetUrl()== def_UniTrans_Req_hello)
    {
        iBodyDataStyle = em_body_data_style_string;
    }
DBG(
    ez_printf_debug("GetUrl:%s\n", GetUrl().c_str());
    ez_printf_debug("Body:%s\n", m_strBody.c_str());
);

    if (m_strBody.size()<strlen("[{}]"))
    {
        ez_printf_error("Too short m_strBody(%d).\n", m_strBody.length() );

        //RetGeneralMsg(PssMsg_Value_Result_InvalidDataFormat
        //              , PssMsg_Value_ResultMsg_InvalidDataFormat);

        //return ;
    }
    // should httpQuery
    else if (em_body_data_style_httpQuery == iBodyDataStyle)
    {
        char __msgBuf[128];

        ez_getParam_HttpQuery(m_strBody.c_str(), "call", __msgBuf);
        __CPssNotifyMsg.m_strNotify = __msgBuf;
DBG(
        ez_printf_debug("%s:[%s]\n", "call", __msgBuf);
);
        ez_getParam_HttpQuery(m_strBody.c_str(), "name", __msgBuf);
        __CPssNotifyMsg.m_strStreamID = __msgBuf;
DBG(
        ez_printf_debug("%s:[%s]\n", "name", __msgBuf);
);

        ez_getParam_HttpQuery(m_strBody.c_str(), "addr", __msgBuf);
        __CPssNotifyMsg.m_strReqAddr = __msgBuf;
DBG(
        ez_printf_debug("%s:[%s]\n", "addr", __msgBuf);
);

        ez_getParam_HttpQuery(m_strBody.c_str(), "flashver", __msgBuf);
        __CPssNotifyMsg.m_strFlashver = __msgBuf;
DBG(
        ez_printf_debug("%s:[%s]\n", "flashver", __msgBuf);
);

        ez_getParam_HttpQuery(m_strBody.c_str(), fixUrl_URL_SIGN_PARA_MARK, __msgBuf);
        __CPssNotifyMsg.m_strPsstk = __msgBuf;
DBG(
        ez_printf_debug("%s:[%s]\n", "Psstk", __msgBuf);
);
    }
    // should json
    else if (em_body_data_style_json == iBodyDataStyle)
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
            ez_printf_error("Not JSON.\n");

            //RetGeneralMsg(PssMsg_Value_Result_InvalidDataFormat
            //              , PssMsg_Value_ResultMsg_InvalidDataFormat);

            //return;
        }
    }
    else
    {
        //ez_printf_debug("GetUrl:%s\n", GetUrl().c_str());
    }

    if (GetUrl()== def_UniTrans_Req_hello)
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    // 统一处理
    else if (GetUrl().find("pss_Ntf")!= std::string::npos)
    {
        Do_reqPssNotify(__CPssNotifyMsg);
    }
    else
    {
        //SendMsg( PssMsg_Value_ResultMsg_Success, GetUrl());
        RetGeneralMsg(PssMsg_Value_Result_Success, g_Solar.GetHostID());
    }

}

void CHttpServerLiveStatusServer::Update()
{
    unsigned long ttNow = g_Solar.GetRunPeriod();//ez_sec_get_tick_count();//time(NULL);

    if (ttNow-m_ttOndata>m_iTimeout)
    {
        ez_printf_info("idle Timeout(%d), conn:%d\n", GetSocket(), m_iTimeout)

        RetGeneralMsg(PssMsg_Value_Result_ConnectionTimedout, PssMsg_Value_ResultMsg_ConnectionTimedout);

        return;
    }
}

size_t CHttpServerLiveStatusServer::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerLiveStatusServer::getMsgNumSent()
{
    return m_iMsgCountSend;
}


// http + json
int CHttpServerLiveStatusServer::SendMsg(const std::string &strMsg, const std::string &matchRequest)
{
    //DBG(
        //std::cout << "strMsg.size() :" <<  strMsg.size()  << std::endl;
        //std::cout << "strMsg :" <<  strMsg  << std::endl;
    //);
    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    // 短连接
    SetCloseAndDelete();

    return 0;
}

// curl http://localhost:18090/UniTrans/hello -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerLiveStatusServer::Do_TestIF( CPssStack &JsonData)
{
    RetGeneralMsg(PssMsg_Value_Result_Success, g_Solar.GetHostID());

    return 0;
}

// pss notify msg
int CHttpServerLiveStatusServer::Do_reqPssNotify( CPssNotifyMsg &PssNotifyMsg)
{
    do
    {
        if (PssNotifyMsg.m_strStreamID.length() < 1)
        {
            ez_printf_error("m_strStreamID not valid.\n");

            //RetGeneralMsg(PssMsg_Value_Result_Success, g_Solar.GetHostID());

            break;
        }

        std::string __strDstIKey;

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        // 维护播放计数
        if (GetUrl()== def_PssNotify_Req_Rtmp_OnPlay)
        {
            CCacheKeyValue::bldCacheKey(__strDstIKey, PssNotifyMsg.m_strStreamID, PssNotifyMsg_Key_Notify_PlayCount);
            int iPlayCount = 0;
            m_pHandler->m_pCacheKeyValue->getPair(__strDstIKey, iPlayCount);
            if (iPlayCount<0)
            {
                ez_printf_error("iPlayCount not valid.\n");

                break;
            }

            iPlayCount++;
            m_pHandler->m_pCacheKeyValue->modPair(__strDstIKey, iPlayCount, modPair_If_Not_Exist_Then_Add);

            ez_printf_debug("%s:%d\n", __strDstIKey.c_str(), iPlayCount);
        }
        else if (GetUrl()== def_PssNotify_Req_Rtmp_OnPlayDone)
        {
            CCacheKeyValue::bldCacheKey(__strDstIKey, PssNotifyMsg.m_strStreamID, PssNotifyMsg_Key_Notify_PlayCount);
            int iPlayCount = 0;
            m_pHandler->m_pCacheKeyValue->getPair(__strDstIKey, iPlayCount);
            if (iPlayCount<1)
            {
                ez_printf_error("iPlayCount <1 do not mod now\n");

                break;
            }

            iPlayCount--;
            m_pHandler->m_pCacheKeyValue->modPair(__strDstIKey, iPlayCount, modPair_If_Not_Exist_Then_Add);

            ez_printf_debug("%s:%d\n", __strDstIKey.c_str(), iPlayCount);
        }
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        // 维护publish time
        else if ( \
			GetUrl()== def_PssNotify_Req_Rtmp_OnPublish
			//|| (GetUrl()== def_PssNotify_Req_Rtmp_OnUpdate && PssNotifyMsg.m_strFlashver.find("FMLE")!=std::string::npos/*用此条件过滤播放器消息*/ )// 中途接收的不处理
			)
        {
            CCacheKeyValue::bldCacheKey(__strDstIKey, PssNotifyMsg.m_strStreamID, PssNotifyMsg_Key_Notify_PublishTTime);

            time_t __ttValue = time(NULL);//g_Solar.GetRunPeriod();
            m_pHandler->m_pCacheKeyValue->modPair(__strDstIKey, __ttValue, modPair_If_Not_Exist_Then_Add);
DBG(
            ez_printf_info("Fresh:[%s:%ul]\n", __strDstIKey.c_str(), __ttValue);
);
        }
        else if ( \
			(GetUrl()== def_PssNotify_Req_Rtmp_OnPublishDone)
			//|| ((GetUrl()== def_PssNotify_Req_Rtmp_OnDone && PssNotifyMsg.m_strFlashver.find("FMLE")!=std::string::npos)) // 暂不需要
			)
        {
            CCacheKeyValue::bldCacheKey(__strDstIKey, PssNotifyMsg.m_strStreamID, PssNotifyMsg_Key_Notify_PublishTTime);

            time_t __ttValue = time(NULL);//g_Solar.GetRunPeriod();
            m_pHandler->m_pCacheKeyValue->delPair(__strDstIKey);

            //_printf_info("Del: %s at %ul\n", __strDstIKey.c_str(), __ttValue);
            ez_printf_info("Del:[%s:%ul]\n", __strDstIKey.c_str(), __ttValue);
        }
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        else if (GetUrl()== def_PssNotify_Req_Rtmp_OnUpdate)//
        {
#if 0
            if (PssNotifyMsg.m_strFlashver.find("FMLE")!=std::string::npos)
            {
                ez_printf_debug("pusher msg\n");
            }
            else
            {
                ez_printf_debug("player msg\n");
            }
#endif
        }
        else
        {
            ez_printf_debug("Skip:%s\n", GetUrl().c_str());
        }
    }
    while(0);

    RetGeneralMsg(PssMsg_Value_Result_Success, g_Solar.GetHostID());
    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
