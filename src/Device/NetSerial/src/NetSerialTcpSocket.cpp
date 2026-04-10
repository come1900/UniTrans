/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * NetSerialTcpSocket.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: NetSerialTcpSocket.cpp 0001 2012-04-06 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -tcp 统一写，便于统一协议解析-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "NetSerialTcpSocket.h"

#include <string.h> /* memset */
//#include <str_opr.h> /* dump */
#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
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

CNetSerialTcpSocket::CNetSerialTcpSocket(ISocketHandler& h, std::string strSocketName) : TcpSocket(h)
{
    // 由Handle自己管理
    SetDeleteByHandler();
    // 数据不缓存

    DisableInputBuffer();

    m_tOnData = 0;

    SetSockName(strSocketName);
}

#include "NetSerialHandler.h"
#include <iostream>
void CNetSerialTcpSocket::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);
DBG(
    __fline;
    std::cout << "CNetSerialTcpSocket::OnRawData:" << len << std::endl;

    dumpBuffer(stdout
               , (unsigned char *)buf, len,
               SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
);
    //////////
    //有数据就调用， 另一种方式
    OnCommand((char *)buf, (unsigned int)len);
    ///////////


    static_cast<CNetSerialHandler&>(Handler()).OnDataCall(this, buf, len);
}

void CNetSerialTcpSocket::OnCommand(char *pCmdData, unsigned int ulCmdDataLen)
{
    DBG( printf("CNetSerialTcpSocket::OnCommand, len:%d\n", ulCmdDataLen); );
}

