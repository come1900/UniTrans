/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocket.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocket.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -tcp 统一写，便于统一协议解析-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _GENERALAGENTTCPSOCKET_H
#define _GENERALAGENTTCPSOCKET_H

#include <stdio.h>

#include <TcpSocket.h>
#include <ISocketHandler.h>

#ifdef EXAMPLE_DATAPARSE_EXAMPLE
//!wifi 网关协议， 通用部分
typedef struct __wifi_agent_protocol_head
{
    unsigned char u8CmdOrStatus;
    unsigned char u8AddOrCommand;
    unsigned char u8DataLen;
}
WIFI_AGENT_PROTOCOL_HEAD;
#define WIFI_AGENT_PROTOCOL_DATA_TIMEOUT 10 // SECOND
#define WIFI_AGENT_PROTOCOL_DATA_MAX_LEN 64 // 最大缓存
#endif //EXAMPLE_DATAPARSE_EXAMPLE

class CGeneralAgentTcpSocket : public TcpSocket
{
public:
    CGeneralAgentTcpSocket(ISocketHandler& , std::string strSocketName="GeneralAgentTcpSocket");
    
    // 从 TcpSocket::OnRead(); 中调用
    virtual void OnRawData(const char *buf,size_t len);
    // 统一command
    // 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);

#ifdef EXAMPLE_DATAPARSE_EXAMPLE
    // 统一read
    // 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
    virtual void OnRead();
    // 通用头 pCmd
    // 数据 pData， 长度见 pCmd->u8DataLen
    virtual void OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData);
private:
    //  保存上次未处理的头，
    WIFI_AGENT_PROTOCOL_HEAD m_headLastOn;
#endif

private:
    time_t m_tOnData; ///< last Time in seconds when this socket was on data
};
#endif // _GENERALAGENTTCPSOCKET_H
