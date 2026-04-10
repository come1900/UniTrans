/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * NetSerialTcpSocketConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: NetSerialTcpSocketConnector.h 0001 2012-04-06 09:58:28Z WuJunjie $
 *
 *  Explain:
 *     -Tcp客户端实例-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _NetSerialTCPSOCKETCONNECTOR_H
#define _NetSerialTCPSOCKETCONNECTOR_H

#include "NetSerialTcpSocket.h"
#include <ISocketHandler.h>


class CNetSerialTcpSocketConnector : public CNetSerialTcpSocket
{
public:
    CNetSerialTcpSocketConnector(ISocketHandler&
                                    , std::string strSocketName="NetSerialTcpSocketConnector"
                                                                , time_t Timeout= 5
                                                                                  , bool bReconn = false);
    ~CNetSerialTcpSocketConnector();

    void SetCloseAndDelete();
    void OnConnectFailed();
    void OnDelete();

    /////////////////////////////
    // 加入命令解析
    virtual void OnConnect();
    virtual void OnReconnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    virtual void OnCommand(char *pCmdData, unsigned int ulCmdDataLen);
    /////////////////////////////

private:
    CNetSerialTcpSocketConnector(const CNetSerialTcpSocketConnector& s) : CNetSerialTcpSocket(s)
    {} // copy constructor
    CNetSerialTcpSocketConnector& operator=(const CNetSerialTcpSocketConnector& )
    {
        return *this;
    } // assignment operator
    /** Create a new instance and reconnect */
    CNetSerialTcpSocketConnector *Reconnect();

    ///////////
    std::string m_strSocketName;
    bool m_b_connected;
    time_t m_ttTimeout;
    bool m_bReconn;
    ///////////
};

#endif // _NetSerialTCPSOCKETCONNECTOR_H
