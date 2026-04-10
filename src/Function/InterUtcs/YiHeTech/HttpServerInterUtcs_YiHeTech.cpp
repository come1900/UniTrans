/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerInterUtcs_YiHeTech.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerInterUtcs_YiHeTech.cpp 5884 2013-01-29 03:21:34Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 16:35:56  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#include <stdio.h>
#ifdef HAVE_LIBUUID
#include <uuid/uuid.h>
#endif

#include "HttpServerInterUtcs_YiHeTech.h"
#include "../../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigInterUtcs_YiHeTech.h"
#endif //_USE_MODULES_EZCONFIG
#ifdef _USE_DBAGENT_OCCI
#include "../../../DBAgent/ltUtcsDbAgent.h"
#endif //_USE_DBAGENT_OCCI

#include "str_opr.h"
#include "StringOpr.h"
#include "HandlerInterUtcs_YiHeTech.h"
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
#define conn_TIMEOUT_DEFAULT_CHttpServerInterUtcs_YiHeTech 30
#define conn_TIMEOUT_DEFAULT_CHttpServerInterUtcs_YiHeTech_str "30"

CHttpServerInterUtcs_YiHeTech::CHttpServerInterUtcs_YiHeTech(ISocketHandler& h) : HttpdSocket(h)
{
    m_strBody = "";
    m_requestPrefix = "";
    m_sessionId = "";

    m_iTimeout = conn_TIMEOUT_DEFAULT_CHttpServerInterUtcs_YiHeTech;

    m_iMsgCountGet = 0;
    m_iMsgCountSend = 0;
}

void CHttpServerInterUtcs_YiHeTech::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }

    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerInterUtcs_YiHeTech::On:" << GetRemoteAddress() << ":" << GetRemotePort());
}
void CHttpServerInterUtcs_YiHeTech::Exec()
{
    CreateHeader();
    RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
}

CHttpServerInterUtcs_YiHeTech::~CHttpServerInterUtcs_YiHeTech()
{
    LOG4CPLUS_INFO(LOG_OF_SMS, "CHttpServerInterUtcs_YiHeTech::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CHttpServerInterUtcs_YiHeTech::OnAccept()
{
    //printf("CHttpServerInterUtcs_YiHeTech::OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    EZ_LOGS(
        ez_log("%s(%d) -- OnAccept:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
    );

    m_ttOndata = time(NULL);
}
void CHttpServerInterUtcs_YiHeTech::OnDisconnect()
{
    //printf("CHttpServerInterUtcs_YiHeTech::OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
    //printf("CHttpServerInterUtcs_YiHeTech::OnDisconnect()\n");
    EZ_LOGS(
        ez_log("%s(%d) -- OnDisconnect:%s:%d\n", __FILE__, __LINE__, GetRemoteAddress().c_str(), GetRemotePort());
    );
}

void CHttpServerInterUtcs_YiHeTech::OnHeaderComplete()
{
    m_strBody = "";
}

void CHttpServerInterUtcs_YiHeTech::OnUnknowRequest()
{}

void CHttpServerInterUtcs_YiHeTech::CreateHeader(size_t ContentLength, const std::string &matchRequest)
{
    SetStatus("200");
    SetStatusText("OK");

    AddResponseHeader("User-Agent", "Mozilla/5.0 (gMiniTech.com; Utcs; Inter YiHe1;)");

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
    //AddResponseHeader("Content-type", "application/json");

    // xml
    AddResponseHeader("Content-type", "text/xml");
#endif

    //AddResponseHeader("Connection", "close");
    AddResponseHeader("Connection", "Keep-Alive");
#if 0

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
#endif
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

void CHttpServerInterUtcs_YiHeTech::RetGeneralMsg(const int retCode, const std::string &strMsg, const std::string &matchRequest)
{
    std::stringstream __ss;
    //std::string strSendBuf ="";
    if (-1 == retCode)
    {
        __ss << "[{\"result\":\"100\",\"reason\":\"CHttpServerInterUtcs_YiHeTech not support url:" + GetUrl() + "\"}]";
    }
    else
    {
        //strSendBuf = "[{\"result\":\"100\",\"reason\":\"CHttpServerInterUtcs_YiHeTech not support url:" + GetUrl() + "\"}]";

        __ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?><Body>   <Code>" \
        << retCode \
        << "</Code><Message>\"" \
        << strMsg \
        << "\"</Message></Body>";
    }

    SendYiHe1Msg(__ss.str(), matchRequest);
}

/** Chunk of http body data recevied. */
void CHttpServerInterUtcs_YiHeTech::OnData(const char *p,size_t l)
{
    if (m_strBody.size()>4096 || l>4096)
    {
        LOG4CPLUS_ERROR(LOG_OF_SMS, "m_strBody(" << m_strBody.size() << ") or l("<<l<<")"<<"too big");
        std::string strBadboy = "Bad boy.";

        SendYiHe1Msg(strBadboy);

        //Reset(); // prepare for next request
        //SetCloseAndDelete();
        //m_strBody = "";

        return;
    }

    m_strBody += p;

    m_ttOndata = time(NULL);
}

void CHttpServerInterUtcs_YiHeTech::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

#ifdef _USE_MODULES_EZCONFIG
    CConfigInterUtcs_YiHeTech __cfg;
    __cfg.update();
#endif//

    CYiHe1Stack __CezlibsWebServiceIF;

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
    //	curl http://192.168.6.112:60089/InterUtcs_YiHeTech/registerURL -d "[{\"reason\":\"OK\",\"result\":\"0\"}]"
    //	curl http://192.168.6.112:60089/InterUtcs_YiHeTech/registerURL -d ""
    //	curl http://192.168.6.112:60089/InterUtcs_YiHeTech/registerURL
    // test interface
#ifdef _USE_MODULES_EZCONFIG
    //exa:
    //	curl http://localhost:60088/yh1/TestIF -d "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
    // test interface
    if (GetUrl()=="/yh1/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    //请求信号机参数
    /*
    curl http://localhost:60088/yh1/getSignalControlerPara -d "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><Message>  <Version>1.0</Version>  <Token>d279449b631fd968068732f3d83e9ea1</Token>  <From>ltgs</From>  <To>tz</To>  <Type>signalM</Type>  <Seq>1</Seq>  <Body>    <Operation order=\"1\" name=\"getSignalControlerPara\">      <Object name=\"TSC\" sumCount=\"1\" />    </Operation>  </Body></Message>"
    */
    else if (GetUrl()==__cfg.getConfig().strURIgetSignalControlerPara)
    {
        Do_getSignalControlerPara(__CezlibsWebServiceIF);
    }//请求信号机参数
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


    //请求路口参数
    //	http://localhost:60088/Sms/getCrossPara
    else if (GetUrl()==__cfg.getConfig().strURIgetCrossPara)
    {
        Do_getCrossPara(__CezlibsWebServiceIF);
    }//请求路口参数
    //请求路口状态
    //	http://localhost:60088/Sms/requestHeartbeat
    else if (GetUrl()==__cfg.getConfig().strURIgetCrossStatus)
    {
        Do_getCrossStatus(__CezlibsWebServiceIF);
    }//请求路口状态


#else // not define _USE_MODULES_EZCONFIG
    if (GetUrl()=="/yh1/TestIF")
    {
        Do_TestIF(__CezlibsWebServiceIF);
    }
#endif //_USE_MODULES_EZCONFIG

    else
    {
        RetGeneralMsg(100, "Not Support URL:"+GetUrl(), GetUrl());
    }

    Reset(); // prepare for next request
    //SetCloseAndDelete();
    m_strBody = "";
}

void CHttpServerInterUtcs_YiHeTech::update()
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

size_t CHttpServerInterUtcs_YiHeTech::getMsgNumReceived()
{
    return m_iMsgCountGet;
}

size_t CHttpServerInterUtcs_YiHeTech::getMsgNumSent()
{
    return m_iMsgCountSend;
}


// http + json
int CHttpServerInterUtcs_YiHeTech::SendYiHe1Msg(const std::string &strMsg, const std::string &matchRequest)
{
    DBG(
        cout << "strMsg.size() :" <<  strMsg.size()  << endl;
        cout << "strMsg :" <<  strMsg  << endl;
    );
    CreateHeader(strMsg.size(), matchRequest);
    Send(strMsg);

    EZ_LOGS(
        ez_log("%s(%d) -- SendYiHe1Msg:%s\n", __FILE__, __LINE__, GetUrl().c_str());
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

std::string CHttpServerInterUtcs_YiHeTech::generateSessionId()
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

std::string CHttpServerInterUtcs_YiHeTech::getMySessionId()
{
    //__fline;
    //cout << "getMySessionId:" << m_sessionId << endl;

    return m_sessionId;
}

// curl http://localhost:60088/yh1/TestIF -d "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
int CHttpServerInterUtcs_YiHeTech::Do_TestIF( CYiHe1Stack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_TestIF");
    __fline;
    cout << "Do_TestIF, your input:" << JsonData.ToString()<< endl;

    std::string strSendBuf;

    strSendBuf = "[[" + JsonData.ToString() + "]]";

    SendYiHe1Msg(strSendBuf);

    return 0;
}

/*
 
curl http://localhost:60088/yh1/getSignalControlerPara -d "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><Message>  <Version>1.0</Version>  <Token>d279449b631fd968068732f3d83e9ea1</Token>  <From>ltgs</From>  <To>tz</To>  <Type>signalM</Type>  <Seq>1</Seq>  <Body>    <Operation order=\"1\" name=\"getSignalControlerPara\">      <Object name=\"TSC\" sumCount=\"1\" />    </Operation>  </Body></Message>"
 
*/
int CHttpServerInterUtcs_YiHeTech::Do_getSignalControlerPara( CYiHe1Stack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_getSignalControlerPara");

    bool bret;
    CMsgGetSignalControlerParaRequest __ReqMsg;
    bret = JsonData.Decode(__ReqMsg);
    if (!bret)
    {
        RetGeneralMsg(600, "getSignalControlerPara bad req xml", GetUrl());
        return -1;
    }

    CMsgGetSignalControlerParaResponse __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = __ReqMsg.m_yhHeader.m_strToken;// "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "getSignalControlerPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";

    __msg.m_yhObjAttr.m_strSumCount = "1";

    //    __msg.m_yhSignalControlerPara.m_strSignalControlerID    = "98776";
    //    __msg.m_yhSignalControlerPara.m_strSupplier             = "lt";
    //    __msg.m_yhSignalControlerPara.m_strType                 = "LT-XHJXT-10";
    //    __msg.m_yhSignalControlerPara.m_strCrossIDList          = "11";

    CYH1MsgSignalControlerPara __para;
    // 使用oracle数据库
#ifdef _USE_DBAGENT_OCCI

    bret = g_ltUtcsDbAgent.execSQL("select ROADID,SIGNALLERID  from TROADPARA");

    if (!bret)
    {
        RetGeneralMsg(404, "database not ready", GetUrl());
        return -2;
    }

    ResultSet *rset = g_ltUtcsDbAgent.getResultSet();

    int line_num=0;

    while (rset && rset->next ())
    {
        int roadid = rset->getInt (1);
        std::string elem_name = (rset->isNull(2)) ? "n/a" : rset->getString(2);

        std::ostringstream __stringstream;
        __stringstream << roadid;

        __para.m_strSignalControlerID    = elem_name;
        __para.m_strSupplier             = "lantai";// 固定参数， 写死
        __para.m_strType                 = "LT-XHJXT-10";// 固定参数， 写死
        __para.m_strCrossIDList          = __stringstream.str();

        __msg.m_v_yhSignalControlerPara.push_back(__para);

        line_num++;
    }
    //cout << "---------------------------------------" << endl;

    std::ostringstream __stringstream;
    __stringstream << line_num;
    __msg.m_yhObjAttr.m_strSumCount = __stringstream.str();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#else
    // 测试用
    __para.m_strSignalControlerID    = "98776";
    __para.m_strSupplier             = "lantai";// 固定参数， 写死
    __para.m_strType                 = "LT-XHJXT-10";// 固定参数， 写死
    __para.m_strCrossIDList          = "11";

    __msg.m_v_yhSignalControlerPara.push_back(__para);
    __msg.m_v_yhSignalControlerPara.push_back(__para);
#endif

    __stack.Encode(__msg);

    SendYiHe1Msg(__stack.ToString()/*strEncoded*/, GetUrl());

    return 0;
}

/*
 
curl http://localhost:60088/yh1/getCrossPara -d "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><Message>  <Version>1.0</Version>  <Token>d279449b631fd9GetCrossPara</Token>  <From>ltgs</From>  <To>tz</To>  <Type>signalM</Type>  <Seq>1</Seq>  <Body>    <Operation order=\"1\" name=\"GetCrossPara\">      <Object name=\"TSC\" sumCount=\"1\">        <SignalControlerID>9DC383E555C50C2E</SignalControlerID>      </Object>    </Operation>  </Body></Message>"
 
*/
int CHttpServerInterUtcs_YiHeTech::Do_getCrossPara( CYiHe1Stack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_getSignalControlerPara");
    EZ_LOGS(
        ez_log("%s(%d) -- Do_getSignalControlerPara\n", __FILE__, __LINE__);
    );

    bool bret;
    CMsgGetCrossParaRequest __ReqMsg;
    bret = JsonData.Decode(__ReqMsg);
    if (!bret)
    {
        RetGeneralMsg(600, "GetCrossPara bad req xml", GetUrl());
        return -1;
    }

    CMsgGetCrossParaResponse __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = __ReqMsg.m_yhHeader.m_strToken;// "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "getCrossPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";

    __msg.m_yhObjAttr.m_strSumCount = "1";

    __msg.m_strSupplier = "lantai";
    __msg.m_strSignalControlerID = __ReqMsg.m_strSignalControlerID;
    
    CYH1MsgCrossPara __para;

    // 使用oracle数据库
#ifdef _USE_DBAGENT_OCCI

    std::ostringstream __streamSql;
    __streamSql << "select TROADPARA.ROADID,troad.roadname,TROADPARA.SIGNALLERID  from TROADPARA, troad where troad.roadid=TROADPARA.roadid and TROADPARA.SIGNALLERID='"  \
    <</*"9DC383E555C50C2E"*/__ReqMsg.m_strSignalControlerID<<"'";

    bret = g_ltUtcsDbAgent.execSQL(__streamSql.str());
    EZ_LOGS(
        ez_log("%s(%d) -- execSQL(%d):%s\n", __FILE__, __LINE__, bret, __streamSql.str().c_str());
    );


    if (!bret)
    {
        RetGeneralMsg(404, "database not ready", GetUrl());
        return -2;
    }

    ResultSet *rset = g_ltUtcsDbAgent.getResultSet();

    int line_num=0;

    while (rset && rset->next())
    {
        int roadid = rset->getInt (1);
        std::string elem_name = (rset->isNull(2)) ? "n/a" : rset->getString(2);

        std::ostringstream __stringstream;
        __stringstream << roadid;

        __para.m_strCrossID    = __stringstream.str();
        __para.m_strCrossName             = elem_name;
        __para.m_strFeature                 = "41"; // 表A.1 路口特征取值表     41	 十字形路口
        __para.m_strIsKey          = "1";

        __msg.m_v_yhCrossPara.push_back(__para);

        line_num++;
    }

    // no record
#if 0
    if (line_num<1)
    {
        // 2014-10-24 16:20:00
        // 这种情况上面的while不执行， 对方也可知道查不到的情况， 目前来看也可不加
        RetGeneralMsg(404, "No Record.", GetUrl());
        return -3;
    }
#endif

    std::ostringstream __stringstream;
    __stringstream << line_num;
    __msg.m_yhObjAttr.m_strSumCount = __stringstream.str();
#else
    // 测试用
    __para.m_strCrossID    = "98776";
    __para.m_strCrossName             = "lt";
    __para.m_strFeature                 = "LT-XHJXT-10";
    __para.m_strIsKey          = "1";

    __msg.m_v_yhCrossPara.push_back(__para);
#endif

    __stack.Encode(__msg);

    SendYiHe1Msg(__stack.ToString()/*strEncoded*/, GetUrl());

    return 0;
}

/*
 
curl http://localhost:60088/yh1/getCrossStatus -d "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><Message>  <Version>1.0</Version>  <Token>d279449b631fd968068732f3d83e9ea1</Token>  <From>ltgs</From>  <To>tz</To>  <Type>signalM</Type>  <Seq>1</Seq>  <Body>    <Operation order=\"1\" name=\"GetCrossPara\">      <Object name=\"TSC\" sumCount=\"1\">        <CrossId>2</CrossId>      </Object>    </Operation>  </Body></Message>"
 
*/
int CHttpServerInterUtcs_YiHeTech::Do_getCrossStatus( CYiHe1Stack &JsonData)
{
    LOG4CPLUS_DEBUG(LOG_OF_SMS, "Do_getCrossStatus");

    bool bret;
    CMsgGetCrossStatusRequest __ReqMsg;
    bret = JsonData.Decode(__ReqMsg);
    if (!bret)
    {
        RetGeneralMsg(600, "GetCrossStatus bad req xml", GetUrl());
        return -1;
    }

    CMsgGetCrossStatusResponse __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = __ReqMsg.m_yhHeader.m_strToken;// "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "getCrossStatus";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";

    __msg.m_yhObjAttr.m_strSumCount = "1";

    // 使用oracle数据库
#ifdef _USE_DBAGENT_OCCI
    std::ostringstream __streamSql;
    __streamSql << "select TSIGNALLERSTATUS.roadid,TSIGNALLERSTATUS.SIGNALLERSTATUS from tsignallerstatus where roadid="  \
    <</*"1"*/__ReqMsg.m_strCrossId;

    bret = g_ltUtcsDbAgent.execSQL(__streamSql.str());
    EZ_LOGS(
        ez_log("%s(%d) -- execSQL(%d):%s\n", __FILE__, __LINE__, bret, __streamSql.str().c_str());
    );


    if (!bret)
    {
        RetGeneralMsg(404, "database not ready", GetUrl());
        return -2;
    }

    ResultSet *rset = g_ltUtcsDbAgent.getResultSet();

    int line_num=0;
	    __msg.m_v_yhCrossStatus.m_strCrossID    = __ReqMsg.m_strCrossId;


    while (rset && rset->next())
    {
        //int roadid = rset->getInt (1);
        int sig_status = rset->getInt (2);
// 无需赋值
#if 0
        std::ostringstream __stringstream;
        __stringstream << roadid;

    __msg.m_v_yhCrossStatus.m_strCrossID    = __stringstream.str();
#endif 
    __msg.m_v_yhCrossStatus.m_strValue             = sig_status==0?"Offline":"Online";;

        line_num++;
    }

    // no record
    // 未查到
#if 1
    if (line_num<1)
    {
    //__msg.m_v_yhCrossStatus.m_strCrossID    = __ReqMsg.m_strCrossId;
    __msg.m_v_yhCrossStatus.m_strValue             = "Not-Exists";

        // 2014-10-24 16:20:00
        // 这种情况上面的while不执行， 对方也可知道查不到的情况， 目前来看也可不加
//        RetGeneralMsg(404, "No Record.", GetUrl());
//        return -3;
    }
#endif

    std::ostringstream __stringstream;
    __stringstream << line_num;
    __msg.m_yhObjAttr.m_strSumCount = __stringstream.str();
#else
    // 测试用
    __msg.m_v_yhCrossStatus.m_strCrossID    = "98776";
    __msg.m_v_yhCrossStatus.m_strValue             = "statusvalue";
#endif

    __stack.Encode(__msg);

    SendYiHe1Msg(__stack.ToString()/*strEncoded*/, GetUrl());

    return 0;
}

