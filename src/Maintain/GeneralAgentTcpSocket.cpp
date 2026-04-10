/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentTcpSocket.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentTcpSocket.cpp 0001 2012-04-06 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -tcp 统一写，便于统一协议解析-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "GeneralAgentTcpSocket.h"

#include <string.h> /* memset */
#include <str_opr.h> /* dump */

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

CGeneralAgentTcpSocket::CGeneralAgentTcpSocket(ISocketHandler& h, std::string strSocketName) : TcpSocket(h)
{
    // 由Handle自己管理
    SetDeleteByHandler();
    // 数据不缓存
#ifdef EXAMPLE_DATAPARSE_EXAMPLE

    memset(&m_headLastOn, 0, sizeof(WIFI_AGENT_PROTOCOL_HEAD));
#else

    DisableInputBuffer();
#endif //EXAMPLE_DATAPARSE_EXAMPLE

    m_tOnData = 0;

    SetSockName(strSocketName);
}

#ifdef EXAMPLE_DATAPARSE_EXAMPLE
// read方法写在这个地方是为了统一协议解析，因为无论客户端还是服务器
// 一个系统的协议体系应该是一样的， wujunjie 2012-4-9 15:35:31
void CGeneralAgentTcpSocket::OnRead()
{
    size_t n = ibuf.GetLength();
    //__fline;
    //printf("data len:%d, time(NULL) %d -m_tOnData %d\n", n, time(NULL), m_tOnData);

    // 超过10s的数据视为 无效， 清理之， 防止客户端出错的时候发送错误数据
    if (n>0
        && time(NULL)-m_tOnData > WIFI_AGENT_PROTOCOL_DATA_TIMEOUT)
    {
        __trip;
        printf("data timeout len:%d\n", n);

        ibuf.Remove(n);
        memset(&m_headLastOn, 0, sizeof(WIFI_AGENT_PROTOCOL_HEAD));
    }


    TcpSocket::OnRead();
    m_tOnData = time(NULL);
    n = ibuf.GetLength();

    //  超长， 错误数据直接全部释放
    if (n>WIFI_AGENT_PROTOCOL_DATA_MAX_LEN)
    {
        __trip;
        printf("data too len:%d\n", n);

        ibuf.Remove(ibuf.GetLength());
        memset(&m_headLastOn, 0, sizeof(WIFI_AGENT_PROTOCOL_HEAD));
        return;
    }

    // 下面的根据实际定义最大的
    unsigned char wifiAgentData[16];

    // 先检查上次是否有未处理完的协议
    // 不可能有u8DataLen = 0的协议被缓存， 因为这样的协议直接处理掉了， 不用再等数据
    if (m_headLastOn.u8DataLen > 0)
    {
        if (n >= m_headLastOn.u8DataLen)
        {
            ibuf.Read((char *)wifiAgentData, m_headLastOn.u8DataLen);

            OnCommand_WifiAgent(&m_headLastOn, wifiAgentData);
            memset(&m_headLastOn, 0, sizeof(WIFI_AGENT_PROTOCOL_HEAD));
        }
    }

    n = ibuf.GetLength();

    while (n >= sizeof(WIFI_AGENT_PROTOCOL_HEAD))
    {

        ibuf.Read((char *)&m_headLastOn, sizeof(WIFI_AGENT_PROTOCOL_HEAD));

        n = ibuf.GetLength();

        // 长度不足， 下次处理
        if (n < m_headLastOn.u8DataLen)
        {
            break;
        }

        if(m_headLastOn.u8DataLen>0)
        {
            ibuf.Read((char *)wifiAgentData, m_headLastOn.u8DataLen);
        }
        OnCommand_WifiAgent(&m_headLastOn, wifiAgentData);
        memset(&m_headLastOn, 0, sizeof(WIFI_AGENT_PROTOCOL_HEAD));

        n = ibuf.GetLength();
    }
    return;
}
#endif // EXAMPLE_DATAPARSE_EXAMPLE

#include "GeneralAgentHandler.h"
#include <iostream>
void CGeneralAgentTcpSocket::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);
DBG(
    __fline;
    std::cout << "CGeneralAgentTcpSocket::OnRawData:" << len << std::endl;

    dumpBuffer(stdout
               , (unsigned char *)buf, len,
               SHOW_ASCII | SHOW_BINAR | SHOW_HEXAD | SHOW_LINER);
);
    //////////
    //有数据就调用， 另一种方式
    OnCommand((char *)buf, (unsigned int)len);
    ///////////

    // send to serial 0;
#ifdef EXAMPLE_SIGNAL_ONDATA

    static_cast<CMaintainHandler&>(Handler()).OnDataCall(this, buf, len);
#endif //EXAMPLE_SIGNAL_ONDATA
}

void CGeneralAgentTcpSocket::OnCommand(char *pCmdData, unsigned int ulCmdDataLen)
{
    DBG( printf("CGeneralAgentTcpSocket::OnCommand, len:%d\n", ulCmdDataLen); );
}

#ifdef EXAMPLE_DATAPARSE_EXAMPLE
void CGeneralAgentTcpSocket::OnCommand_WifiAgent(WIFI_AGENT_PROTOCOL_HEAD *pCmd, unsigned char *pData)
{
    DBG( printf("OnCommand_WifiAgent %d %d %d\n"
                     , pCmd->u8CmdOrStatus
                     , pCmd->u8AddOrCommand
                     , pCmd->u8DataLen); );
}
#endif
