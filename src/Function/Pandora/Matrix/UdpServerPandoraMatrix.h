/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UdpServerPandoraMatrix.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: UdpServerPandoraMatrix.h 5884 2018-12-03 02:19:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-12-03 02:19:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _UdpServerPandoraMatrix_H
#define _UdpServerPandoraMatrix_H

#include <UdpSocket.h>
#include <vector>

class IPCPEER_T
{
public:
    std::string strName;// 暂时无用
    struct sockaddr PeerAddr;
    socklen_t PeerAddrLen;
    time_t ttLastOnData;
    IPCPEER_T()
    {
        strName = "";
        memset(&PeerAddr, 0, sizeof(struct sockaddr));
        PeerAddrLen = sizeof(struct sockaddr);

        ttLastOnData = 0;
    }
    ~IPCPEER_T()
    {
        ;
    }

};

class CUdpServerPandoraMatrix : public UdpSocket
{
public:
    CUdpServerPandoraMatrix(ISocketHandler&, std::string strSocketName="CUdpServerPandoraMatrix");

    void OnRawData(const char *,size_t,struct sockaddr *,socklen_t);

    /** Outgoing traffic counter. */
    virtual uint64_t GetBytesSent(bool clear = false);

    /** Incoming traffic counter. */
    virtual uint64_t GetBytesReceived(bool clear = false);
private:

    int OnData_RTO(const unsigned char *buf, int len, unsigned short &devid);

    // 存放同ipc 通讯的所有客户端
    std::vector<IPCPEER_T> m_IpcPeer;
    std::vector<IPCPEER_T>::iterator it;

    uint64_t m_BytesSent;
    uint64_t m_BytesReceived;
};

#endif // _UdpServerPandoraMatrix_H
