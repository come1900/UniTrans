/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerInterThings_gLock.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerInterThings_gLock.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
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

#include "../../GeminiKey/GeminiKeyStack.h"
#include "../../GeminiLock/FunGeminiLock.h"

#include "../../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigInterThings_gLock.h"
#endif //_USE_MODULES_EZCONFIG
#include "str_opr.h"
#include "StringOpr.h"
#include "HandlerInterThings_gLock.h"
#include "HttpServerInterThings_gLock.h"

#ifdef _DEBUG
	#define DEB(x)
	#define DBG(x)
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

//5152f876-c738-4e96-9c2b-b314501c40f1
#define MY_SESSION_ID_LEN 36
// 30s
#define conn_TIMEOUT_DEFAULT_CHttpServerInterThings_gLock 30
#define conn_TIMEOUT_DEFAULT_CHttpServerInterThings_gLock_str "30"

CHttpServerInterThings_gLock::CHttpServerInterThings_gLock(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";
    m_sessionId = "";

    m_iTimeout = conn_TIMEOUT_DEFAULT_CHttpServerInterThings_gLock;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;
}

void CHttpServerInterThings_gLock::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerInterThings_gLock::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerInterThings_gLock::Exec()
{
    CreateHeader();
    //RetGeneralMsg();
    RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

CHttpServerInterThings_gLock::~CHttpServerInterThings_gLock()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerInterThings_gLock::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerInterThings_gLock::OnAccept()
{
    //printf("CHttpServerInterThings_gLock::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    EZ_LOGS(
        ez_log("%s(%d) -- OnAccept:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
    );

    m_ttOndata = time(NULL);
}
void CHttpServerInterThings_gLock::OnDisconnect()
{
    //printf("CHttpServerInterThings_gLock::OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    //printf("CHttpServerInterThings_gLock::OnDisconnect()\n");
    EZ_LOGS(
        ez_log("%s(%d) -- OnDisconnect:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
    );
}

void CHttpServerInterThings_gLock::OnHeaderComplete()
{
    m_strBody = "";
}

void CHttpServerInterThings_gLock::OnUnknowRequest()
{}

void CHttpServerInterThings_gLock::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");
    DBG(
        fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
    );

    AddResponseHeader("User-Agent", "Mozilla/5.0 (gMiniTech.com; geminiLock; Inter Things;)");
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
        m_sessionId = "joy-dbg-only";
        AddResponseHeader("Cookie", "JSESSIONID="+m_sessionId);
        m_sessionId="";
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

void CHttpServerInterThings_gLock::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
{
    std::stringstream __ss;
    //std::string strSendBuf ="";
    if (-1 == retCode)
    {
        __ss << "[{\"result\":\"100\",\"reason\":\"CHttpServerInterUtcs_YiHeTech not support url:" + GetUrl() + "\"}]";
    }
    else
    {
        //strSendBuf = "[{\"result\":\"100\",\"reason\":\"CHttpServerInterThings_gLock not support url:" + GetUrl() + "\"}]";
#if 0
        __ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?><Body>   <Code>" \
        << retCode \
        << "</Code><Message>\"" \
        << strMsg \
        << "\"</Message></Body>";
#else

        __ss << "[{\"Code\":\"" << retCode << "\",\"Message\":\"" + strMsg + "\"}]";
#endif // xml or json

    }

    SendSmsMsg(__ss.str(), matchRequest);
}

/** Chunk of http body data recevied. */
void CHttpServerInterThings_gLock::OnData(const char *p,size_t l)
{
    if (m_strBody.size()>4096 || l>4096)
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too big");
        std::string strBadboy = "Bad boy.";

        SendSmsMsg(strBadboy);

        //Reset(); // prepare for next request
        //SetCloseAndDelete();
        //m_strBody = "";

        return;
    }

    m_strBody += p;

    m_ttOndata = time(NULL);
}

void CHttpServerInterThings_gLock::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#ifdef _USE_MODULES_EZCONFIG
    CConfigInterThings_gLock __cfg;
    __cfg.update();
#endif//

    CInterThings_gLockStack __CezlibsWebServiceIF;

    //LOG4CPLUS_DEBUG(LOG_OF_SMS, "m_strBody:" << m_strBody);

    if (m_strBody.size()<strlen("[{}]"))
    {
        LOG4CPLUS_INFO(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ")" << " [" << m_strBody << "] not json");
    }
    else
    {
        // have []  of json string
#if 0
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
#endif //


        bool bRet = __CezlibsWebServiceIF.Parse(m_strBody);
        if (bRet)
        {
            m_iMsgCountGet ++;
        }
        else
        {
            LOG4CPLUS_INFO(LOG_OF_SMS, "__CezlibsWebServiceIF.Parse failed.");
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

    //exa:
    //	curl http://192.168.6.112:60089/InterThings_gLock/registerURL -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    //	curl http://192.168.6.112:60089/InterThings_gLock/registerURL -d ""
    //	curl http://192.168.6.112:60089/InterThings_gLock/registerURL
    // test interface
#ifdef _USE_MODULES_EZCONFIG

    if (GetUrl()=="/gLock/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    //钥匙数据分析接口
    //	http://localhost:60088/gLock/chkKey
    else if (GetUrl()==__cfg.getConfig().strURI_chkKey)
    {
        Do_chkKey(__CezlibsWebServiceIF);
    }
    //钥匙数据使用接口
    // gLock/useKey
    else if (GetUrl()==__cfg.getConfig().strURI_useKey)
    {
        Do_useKey(__CezlibsWebServiceIF);
    }
    //钥匙数据使用接口
    // gLock/queStatus
    else if (GetUrl()==__cfg.getConfig().strURI_queStatus)
    {
        Do_queryStatus(__CezlibsWebServiceIF);
    }
    //秘钥数据分析接口

#else // not define _USE_MODULES_EZCONFIG
    if (GetUrl()=="/gLock/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
#endif //_USE_MODULES_EZCONFIG

    else
    {
        RetGeneralMsg(100, "CHttpServerInterThings_gLock url not support:" + GetUrl(), GetUrl());
    }

    Reset(); // prepare for next request
    //SetCloseAndDelete();
    m_strBody = "";
}

void CHttpServerInterThings_gLock::update()
{
    time_t ttnow = time(NULL);
#if 1

    DBG(
        cout << ttnow-m_ttOndata << " tout:" << m_iTimeout << endl;
    );
    if (ttnow-m_ttOndata>m_iTimeout)
    {
        __fline;
        std::cout << "timeout :" <<  getMySessionId()  << std::endl << "SetCloseAndDelete ..."<< std::endl;

        SetCloseAndDelete();
    }
#endif
}

size_t CHttpServerInterThings_gLock::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerInterThings_gLock::getMsgNumSent()
{
    return m_iMsgCountSend;
}


// http + json
int CHttpServerInterThings_gLock::SendSmsMsg(const std::string &strMsg, const std::string &matchRequest)
{
    DBG(
        cout << "strMsg.size() :" <<  strMsg.size()  << endl;
        cout << "strMsg :" <<  strMsg  << endl;
    );
    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    EZ_LOGS(
        ez_log("%s(%d) -- SendSmsMsg:%s\n", __FILE__, __LINE__, GetUrl().c_str());
    );

    EZ_LOGS(
        ez_log("%s(%d) -- %s\n", __FILE__, __LINE__, strMsg.c_str());
    );

    Reset();
    //SetCloseAndDelete();
    m_strBody = "";

    m_iMsgCountSend++;

    return 0;
}

std::string CHttpServerInterThings_gLock::generateSessionId()
{
    char uuidBuff[MY_SESSION_ID_LEN+1] = {0};

    // single thread, do not add lock here
#ifdef HAVE_LIBUUID

    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuidBuff);
#else
    // 存在安全漏洞
    static unsigned short idNum = time(NULL);
    sprintf(uuidBuff, "6933c3d9-74f6-489e-a614-32de%08d", idNum);
    idNum ++;
#endif //HAVE_LIBUUID

    return uuidBuff;
}

std::string CHttpServerInterThings_gLock::getMySessionId()
{
    //__fline;
    //cout << "getMySessionId:" << m_sessionId << endl;

    return m_sessionId;
}

// curl http://localhost:60088/gLock/TestIF -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CHttpServerInterThings_gLock::Do_TestIF( CInterThings_gLockStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_TestIF");
    __fline;
    std::cout << "Do_TestIF, your json:" << JsonData.ToString()<< std::endl;

    std::string strSendBuf;

    strSendBuf = "[{\"result\":\"0\",\"reason\":\"Do_TestIF, Your URL:" + GetUrl() + "\"}]";

    SendSmsMsg(strSendBuf);

    return 0;
}

/*
curl http://192.168.5.1:60088/gLock/chkKey -d "{ \"Key\" : \"wZWKi60AyCDP1+96R+rccFwel58W2bjtoHVPjkeqmG9TVUXZUlcID0QKMvynLZZS+O7aLUKpy3G3kGbXmnMby+9TtGAAp3yhwyMhki/m368kQJUUBvjtBgmKaBlbie6UciW2xRnPn31UeGEdAJt5Yc1bGXrePzWFMLizVzVNtNASJrBDwFxxd6bg1Mp2RV+gNRz/aSCVJFjjqRJf5xh1eG/f7Z2o5jsL5Vbd9jB+z5bv8X2RwmJADHrhXM7peL6iYL4wAk1BmDMCLy/9l/gkmRBAdnoH4ZUQ2//KCSoQy32VGCyEI3ZQQIHRcd/PUYsBKxGwWRoalOJZJe/mu8B5HiZAjBtJKhQ979XHAwAfI2yS/ou66oGXxIsMc8jg8zutn88dybeiRoID0LBdmSQtv7bWaCmaRycG/umQB71YXPUInQK5tLY+4WtPCB9XWoR0iSWvLztIqIu1Mff4amE4MwaajMxY9uGVZgZaxLmfbLceat2Vf0opVzgAJ0fFYvDwV32l6/sTDz1phKxcxe6nHGBcryYSX6XCQl0Zwcw0mwpln1Dt+dWlWKIksDtdIjy4VctO+mD1XzyfcNzlrpbcytmmftZ9+DTKgyrrpi95Q1zk3Z620mGrTA==\", \"Ver\" : \"1.0\" }"
curl http://192.168.5.1:60088/gLock/chkKey -d "{ \"Key\" : \"20HY1G07gVgKU7WMl/lpkqsyVn6VkTwyE/My0GzeicKeG/dVFhNc3aR5DP/a0PFv4aqoKgZnVTehPqB4Gm4AtvTcoYhEgBRPe2i1YVNE5MIh2dPqKG8h4JSwYGVX2A6uKlMry85C3+RfP/2D7EA/WnmpaUeqqZpgj9jMInoFctg8qz1VwFTx+WES3VJI1Nwv8x/F9W082liU/ooPzBd5p8TB3e3ocLnmnImaXcTeULJ1l5ftLiACzauGeiioOszbTfy0ywdCbldkHh1JvpNwcQsMVPCnkSvOUUiDOMCvjVVaMJqtPcqc+94xwhjZ7QSVORkePhzxHN8Ja36gRoTpTrU1NXE0m/SQjhDUBu2NBdzGr965e2E55HRhZYTV9HcsidJvco1n3toyazH7NsoFUG4OhKfYJgHF3/MeiapBA228lbE9ubPNZq6KMuMfLYaC+IHfapwuhouAvjHbVf8HzxMX+tBCNNzrhQ3JAdjYUQXiwp3gaT1go4oBinCX89WcdwKX+qpHKNPV/fmS7PwQwvTicT93ytl1Hcmn1rEkpJ+UIBX/4Pg4MtubTtCZGlY9UX/rlpulEtgELXaeKqZInwe+gZsk7fWy0munLpcwbljwW0gYa1o9HfrCoaJr/KQNEJk6Y/ZpQt8FwwacMH1nwZuvDvDqumX7G9J41LV/IgHv2KQRdM0XR56BPXZ9ezaudUdkmKgpA27XUjbs6zNv5gz2/JQZhYeqLMMoypGxz9vXKlrgIa/TUA6r6pIlgQ0oFz7cA+KtPQsAulMxr/W/XgeYnf0lcFNCat1dUGY9KpqN3iaFfQNYzny08l1mHhupuyOC8BRPCTKGjr8j7B3Yz2MyJ/e1lTLLXGJLO2fgGKR5M3avYWL9qRTHDvBxLQZZ5YeNmKh3TkLrIKb9Apxj64n0wnLbpn7oUb75m+q5Qn8Ss5Klm/+5fUnvJ1NEWOcmSPwMHzUVG/6Po3dFltY+pD4CoxKel97A0jFoLFYeGkvWHLTkKO2zBLONSktYczWvXicHKln8agBnMGGDOyE/mHYk8P8RillPBrblMpbGpCR75CN9NIx8DpIOa5cz9gWoRKXCrGJ+ok9gnsCWv2BmY6opp6VxBjpVFdYzB/TowolIXtIpNymw28b0TwPsJg/RmoKNPjJ9oIwB0T25CAvpebpqqZ8rgT+LhX7CnGTIuwhRRD78sa5hvVlBmUxys4OZxffafsE8tBBmwRjVzMYT1lhh/mgZLDWhDPqcFjlGDppcqEwddjrwZs+GV0Dpsvoas+mJnTgpIzVG4jpYiuTX350UNwHxZ3L8lXP1U8ZR4hWiDb2R2gg7IDi3oK6chsOY9YxpnFEWCJSPYdLSAc8ah72aw1kaUCI06sO7gRAILkGGWMwlIJSCrwCAUrhGEMieok/Dh90tVID1UePuk+7Y9wiveSg7QOYckEd/1XXFNRfi967HFHrKtE9fSqG3h/3JmPJx2Ta2GRYAsVbBjXfW6Og8E8UI5eQKpiRCQJdLAcA+shC+ncvz0/WmHN0Ft97029srMhGNsstwhMHhbPS/EeRlwwowtPHqBj3uN/OshzLVL29DgkPwrlDGKNdovNWgIvDd6DSNInj1qHX/T5m9o7uaTVIrji9innFCiCCd1R09Z6YhHYV4mAveEJe7TU8u9ZvLNL3OHw264Nd4tVAM+unsl59vo4Iu/rTvwuTW6LPo4VrJh2fcwL0AFkR3TrwjarEbZ9ijEvvraZR4adc6sqp9UMyu9oAylupUzFN5A+ELkH3AtTum36dDQLqHGNshJZwbK9Y9F3UYMd5QOx8ywdysSwxGo976SASzenoKUfXdyUJ9rf21+oVHHYeurXGEWu0OMgFrxlGNNA3gAkcZZWdplyxheGuBvoCaFvz3ssR2jgiQyEdqvQ==\", \"Ver\" : \"1.0\" }"
*/
int CHttpServerInterThings_gLock::Do_chkKey( CInterThings_gLockStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_chkKey");
    EZ_LOGS(
        ez_log("%s(%d) -- Do_chkKey\n", __FILE__, __LINE__);
    );
    DBG(
        __fline;
        std::cout << "Do_chkKey, your json:" << JsonData.ToString()<< std::endl;
    );

    CMsgGeminiKeyString MsgKey;

    if (JsonData.Decode(MsgKey))
    {
        DBG(
            MsgKey.dump();
        );
        int iret = 0;

        EZ_LOGS(
            ez_log("%s(%d) -- MsgKey.m_strKeyVersion:%s\n", __FILE__, __LINE__, MsgKey.m_strKeyVersion.c_str());
        );

        if (1)//(MsgKey.m_strKeyVersion == KEY_STRING_Opr_parse)
        {
            GeminiKeys __KeysGenUnlock;
            iret = g_FunGeminiLock.OnKey_Chk(0, MsgKey.m_strKeyData.c_str(), MsgKey.m_strKeyData.size(), &__KeysGenUnlock);

            ez_printf_info("OnKeyData(%d) -- %s\n", iret, MsgKey.m_strKeyData.c_str());

            if (iret >=0)
            {
                std::string strKeys;
                __KeysGenUnlock.dump(strKeys);

                //strSendBuf = strKeys;
                EZ_LOGS(
                    ez_log("%s(%d) -- __KeysGenUnlock.dump: %s \n", __FILE__, __LINE__, strKeys.c_str());
                );

                RetGeneralMsg(iret, strKeys);
            }
            else
            {
                //std::stringstream __ss;

                //__ss << "keystring not valid, error no:" << iret;
                //strSendBuf = __ss.str();//"keystring not valid:"+MsgKey.m_strGeminiKey;

                EZ_LOGS(
                    ez_log("%s(%d) -- keystring not valid, error no:%d\n", __FILE__, __LINE__, iret);
                );
                RetGeneralMsg(iret, "keystring not valid");
            }
        }
        else
        {
            RetGeneralMsg(-404, "OprNotValid:" + MsgKey.m_strKeyVersion);
        }
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
        EZ_LOGS(
            ez_log("%s(%d) -- JsonData.Decode failed\n", __FILE__, __LINE__);
        );
        RetGeneralMsg(-1, "JsonData.Decode failed");
        return -1;
    }

    //SendSmsMsg(strSendBuf);

    return 0;
}

/*
curl http://192.168.5.1:60088/gLock/useKey -d "{ \"Key\" : \"wZWKi60AyCDP1+96R+rccFwel58W2bjtoHVPjkeqmG9TVUXZUlcID0QKMvynLZZS+O7aLUKpy3G3kGbXmnMby+9TtGAAp3yhwyMhki/m368kQJUUBvjtBgmKaBlbie6UciW2xRnPn31UeGEdAJt5Yc1bGXrePzWFMLizVzVNtNASJrBDwFxxd6bg1Mp2RV+gNRz/aSCVJFjjqRJf5xh1eG/f7Z2o5jsL5Vbd9jB+z5bv8X2RwmJADHrhXM7peL6iYL4wAk1BmDMCLy/9l/gkmRBAdnoH4ZUQ2//KCSoQy32VGCyEI3ZQQIHRcd/PUYsBKxGwWRoalOJZJe/mu8B5HiZAjBtJKhQ979XHAwAfI2yS/ou66oGXxIsMc8jg8zutn88dybeiRoID0LBdmSQtv7bWaCmaRycG/umQB71YXPUInQK5tLY+4WtPCB9XWoR0iSWvLztIqIu1Mff4amE4MwaajMxY9uGVZgZaxLmfbLceat2Vf0opVzgAJ0fFYvDwV32l6/sTDz1phKxcxe6nHGBcryYSX6XCQl0Zwcw0mwpln1Dt+dWlWKIksDtdIjy4VctO+mD1XzyfcNzlrpbcytmmftZ9+DTKgyrrpi95Q1zk3Z620mGrTA==\", \"Ver\" : \"1.0\" }"
curl http://192.168.5.1:60088/gLock/useKey -d "{ \"Key\" : \"20HY1G07gVgKU7WMl/lpkqsyVn6VkTwyE/My0GzeicKeG/dVFhNc3aR5DP/a0PFv4aqoKgZnVTehPqB4Gm4AtvTcoYhEgBRPe2i1YVNE5MIh2dPqKG8h4JSwYGVX2A6uKlMry85C3+RfP/2D7EA/WnmpaUeqqZpgj9jMInoFctg8qz1VwFTx+WES3VJI1Nwv8x/F9W082liU/ooPzBd5p8TB3e3ocLnmnImaXcTeULJ1l5ftLiACzauGeiioOszbTfy0ywdCbldkHh1JvpNwcQsMVPCnkSvOUUiDOMCvjVVaMJqtPcqc+94xwhjZ7QSVORkePhzxHN8Ja36gRoTpTrU1NXE0m/SQjhDUBu2NBdzGr965e2E55HRhZYTV9HcsidJvco1n3toyazH7NsoFUG4OhKfYJgHF3/MeiapBA228lbE9ubPNZq6KMuMfLYaC+IHfapwuhouAvjHbVf8HzxMX+tBCNNzrhQ3JAdjYUQXiwp3gaT1go4oBinCX89WcdwKX+qpHKNPV/fmS7PwQwvTicT93ytl1Hcmn1rEkpJ+UIBX/4Pg4MtubTtCZGlY9UX/rlpulEtgELXaeKqZInwe+gZsk7fWy0munLpcwbljwW0gYa1o9HfrCoaJr/KQNEJk6Y/ZpQt8FwwacMH1nwZuvDvDqumX7G9J41LV/IgHv2KQRdM0XR56BPXZ9ezaudUdkmKgpA27XUjbs6zNv5gz2/JQZhYeqLMMoypGxz9vXKlrgIa/TUA6r6pIlgQ0oFz7cA+KtPQsAulMxr/W/XgeYnf0lcFNCat1dUGY9KpqN3iaFfQNYzny08l1mHhupuyOC8BRPCTKGjr8j7B3Yz2MyJ/e1lTLLXGJLO2fgGKR5M3avYWL9qRTHDvBxLQZZ5YeNmKh3TkLrIKb9Apxj64n0wnLbpn7oUb75m+q5Qn8Ss5Klm/+5fUnvJ1NEWOcmSPwMHzUVG/6Po3dFltY+pD4CoxKel97A0jFoLFYeGkvWHLTkKO2zBLONSktYczWvXicHKln8agBnMGGDOyE/mHYk8P8RillPBrblMpbGpCR75CN9NIx8DpIOa5cz9gWoRKXCrGJ+ok9gnsCWv2BmY6opp6VxBjpVFdYzB/TowolIXtIpNymw28b0TwPsJg/RmoKNPjJ9oIwB0T25CAvpebpqqZ8rgT+LhX7CnGTIuwhRRD78sa5hvVlBmUxys4OZxffafsE8tBBmwRjVzMYT1lhh/mgZLDWhDPqcFjlGDppcqEwddjrwZs+GV0Dpsvoas+mJnTgpIzVG4jpYiuTX350UNwHxZ3L8lXP1U8ZR4hWiDb2R2gg7IDi3oK6chsOY9YxpnFEWCJSPYdLSAc8ah72aw1kaUCI06sO7gRAILkGGWMwlIJSCrwCAUrhGEMieok/Dh90tVID1UePuk+7Y9wiveSg7QOYckEd/1XXFNRfi967HFHrKtE9fSqG3h/3JmPJx2Ta2GRYAsVbBjXfW6Og8E8UI5eQKpiRCQJdLAcA+shC+ncvz0/WmHN0Ft97029srMhGNsstwhMHhbPS/EeRlwwowtPHqBj3uN/OshzLVL29DgkPwrlDGKNdovNWgIvDd6DSNInj1qHX/T5m9o7uaTVIrji9innFCiCCd1R09Z6YhHYV4mAveEJe7TU8u9ZvLNL3OHw264Nd4tVAM+unsl59vo4Iu/rTvwuTW6LPo4VrJh2fcwL0AFkR3TrwjarEbZ9ijEvvraZR4adc6sqp9UMyu9oAylupUzFN5A+ELkH3AtTum36dDQLqHGNshJZwbK9Y9F3UYMd5QOx8ywdysSwxGo976SASzenoKUfXdyUJ9rf21+oVHHYeurXGEWu0OMgFrxlGNNA3gAkcZZWdplyxheGuBvoCaFvz3ssR2jgiQyEdqvQ==\", \"Ver\" : \"1.0\" }"
*/
int CHttpServerInterThings_gLock::Do_useKey( CInterThings_gLockStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_chkKey");

    ez_printf_debug("Do_chkKey\n");

    DBG(
        __fline;
        std::cout << "Do_chkKey, your json:" << JsonData.ToString()<< std::endl;
    );

    CMsgGeminiKeyString MsgKey;

    if (JsonData.Decode(MsgKey))
    {
        DBG(
            MsgKey.dump();
        );
        int iret = 0;

        ez_printf_info("MsgKey.m_strKeyVersion:%s\n", MsgKey.m_strKeyVersion.c_str());
       
        if (1)//(MsgKey.m_strKeyVersion == KEY_STRING_Opr_Use)
        {
            iret = g_FunGeminiLock.OnKey_Use(0, MsgKey.m_strKeyData.c_str(), MsgKey.m_strKeyData.size(), -1);

            ez_printf_info("g_FunGeminiLock.OnGeminiKey(%d):%s\n", iret, MsgKey.m_strKeyData.c_str());

            RetGeneralMsg(iret, "Do_useKey");
        }
        else
        {
            RetGeneralMsg(-404, "OprNotValid:" + MsgKey.m_strKeyVersion);
        }
    }
    else
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "JsonData.Decode failed");
        EZ_LOGS(
            ez_log("%s(%d) -- JsonData.Decode failed\n", __FILE__, __LINE__);
        );
        RetGeneralMsg(-1, "JsonData.Decode failed");
        return -1;
    }

    //SendSmsMsg(strSendBuf);

    return 0;
}

/*
curl http://192.168.6.1:60088/gLock/useKey -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
*/
int CHttpServerInterThings_gLock::Do_queryStatus( CInterThings_gLockStack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_queryStatus");
    EZ_LOGS(
        ez_log("%s(%d) -- Do_queryStatus\n", __FILE__, __LINE__);
    );

    __fline;
    std::cout << "Do_queryStatus, your json:" << JsonData.ToString()<< std::endl;

    RetGeneralMsg(0, "Do_queryStatus, Your URL:" + GetUrl(), GetUrl());

    return 0;
}

