/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GRestHttpdSocket.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GRestHttpdSocket.cpp 5884 2019-09-08 09:05:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-09-08 09:05:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "../../Solar.h"

#include "GRestHttpdSocket.h"
#include "defGRest.h"

CGRestHttpdSocket::CGRestHttpdSocket(ISocketHandler& h) : HttpdSocket(h)
{
    m_strRetCode = "200";
}

CGRestHttpdSocket::~CGRestHttpdSocket()
{}


void CGRestHttpdSocket::CreateHeader(size_t ContentLength, const std::string &matchRequest, const std::string &retCode)
{
    if (retCode.empty() || "200"==retCode)
    {
        SetStatus("200");
        SetStatusText("OK");
    }
    else
    {
        SetStatus(retCode);

        if (retCode == "405")
            SetStatusText("Method not allowed");
        else
            SetStatusText("NOK");
    }

    if (m_strHttpHeader_Server.empty())
        AddResponseHeader("Server", std::string(def_GRestServer_ServerName));
    else
        AddResponseHeader("Server", m_strHttpHeader_Server);

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
    AddResponseHeader("Content-type", "application/json; charset=utf-8");
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
// http + json
int CGRestHttpdSocket::SendMsg(const std::string &strMsg, const std::string &matchRequest, const std::string &retCode)
{
    CreateHeader(strMsg.size(), matchRequest, retCode);
    Send(strMsg);

    // 短连接
    SetCloseAndDelete();

    return 0;
}
#include <sstream>
// curl http://localhost:18090/UniTrans/hello -d "{\"reason\":\"I Love You.\",\"result\":\"0\"}"
int CGRestHttpdSocket::Do_reqHello(const std::string &strReq)
{
    std::stringstream ssRet;

    ssRet << "{ \"Result\" : \"0\", \"ResultMsg\" : \"" \
       << g_Solar.GetVersion() \
       << " || " << g_Solar.GetHuid() \
       << " || " << g_Solar.GetHostIP() \
       << " || " << g_Solar.GetRunPeriod() \
       << "\" }";

    return SendMsg(ssRet.str(), GetUrl(), m_strRetCode);
}

int CGRestHttpdSocket::Do_reqEcho(const std::string &strReq)
{
    SendMsg(strReq, GetUrl(), m_strRetCode);

    return 0;
}

int CGRestHttpdSocket::Do_reqDescribe(const std::string &strReq)
{
    std::stringstream ssRet;

    ssRet \
       << "{ \"Result\" : \"0\", \"ResultMsg\" : \"" << "Ok" << "\""
       << ", \"Version\" : \"" << g_Solar.GetVersion() << "\""
       << ", \"Huid\" : \"" << g_Solar.GetHuid() << "\""
       << ", \"HostIP\" : \"" << g_Solar.GetHostIP() << "\""
       << ", \"RunPeriod\" : \"" << g_Solar.GetRunPeriod() << "\""
       << " }";

    return SendMsg(ssRet.str(), GetUrl(), m_strRetCode);
}

int CGRestHttpdSocket::Do_reqIp(const std::string &strReq)
{
    SendMsg(GetRemoteAddress(), GetUrl(), m_strRetCode);

    return 0;
}

