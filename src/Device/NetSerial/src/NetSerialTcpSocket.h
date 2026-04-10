/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * NetSerialTcpSocket.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: NetSerialTcpSocket.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -tcp 统一写，便于统一协议解析-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _NetSerialTCPSOCKET_H
#define _NetSerialTCPSOCKET_H

#include <stdio.h>

#include <TcpSocket.h>
#include <ISocketHandler.h>


class CNetSerialTcpSocket : public TcpSocket
{
public:
    CNetSerialTcpSocket(ISocketHandler& , std::string strSocketName="NetSerialTcpSocket");
    
    // 从 TcpSocket::OnRead(); 中调用
    virtual void OnRawData(const char *buf,size_t len);
    // 统一command
    // 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);

private:
    time_t m_tOnData; ///< last Time in seconds when this socket was on data
};
#endif // _NetSerialTCPSOCKET_H
