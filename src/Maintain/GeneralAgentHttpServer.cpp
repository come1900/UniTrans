/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHttpServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHttpServer.cpp 5884 2012-09-06 01:31:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 01:31:44  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include <string.h>
#include <stdio.h>

#include "GeneralAgentHttpServer.h"

CGeneralAgentHttpServer::CGeneralAgentHttpServer(ISocketHandler& h) : HttpdSocket(h)
{m_strBody = "";}

void CGeneralAgentHttpServer::Init()
{
    if (GetParent() -> GetPort() == 443 || GetParent() -> GetPort() == 8443)
    {
        fprintf(stderr, "SSL not available\n");
    }
}

void CGeneralAgentHttpServer::Exec()
{
    CreateHeader();
    GenerateDocument();
}

void CGeneralAgentHttpServer::CreateHeader()
{
    SetStatus("200");
    SetStatusText("OK");
    fprintf(stderr, "Uri: '%s'\n", GetUri().c_str());
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
    AddResponseHeader("Connection", "close");
    SendResponse();
}

void CGeneralAgentHttpServer::GenerateDocument()
{
    std::string strSendBuf = "CGeneralAgentHttpServer.\n\r";

    strSendBuf += GetHttpDate()+"\r\n";
    strSendBuf += "Your Body:["+m_strBody+"]\r\n";

    Send(strSendBuf);

    SetCloseAndDelete();
}



void CGeneralAgentHttpServer::OnData(const char *p,size_t l)
{
	m_strBody += p;
	printf("m_strBody:%s\n", m_strBody.c_str());
}

void CGeneralAgentHttpServer::OnDataComplete()
{
    //验证失败不处理- 
    if (CloseAndDelete())
    {
        return ;
    }

    Exec();
    Reset(); // prepare for next request
    m_strBody = "";
}

