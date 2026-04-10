/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SmsStack.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SmsStack.h 5884 2013-11-02 11:10:13Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain -- Intelligent system communication protocol
 *     -
 *
 *  Update:
 *     2013-11-02 11:10:13 WuJunjie Create
 *
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _SmsStack_H
#define _SmsStack_H


#include "SmsMsg.h"

#include "UniTransRestStack.h"

class CSmsStack:public CUniTransRestStack
{
public :
    CSmsStack();
    virtual ~CSmsStack();

    virtual void Encode(const CAckMsgSimple             &Msg);
    virtual bool Decode(CAckMsgSimple                           &Msg);

    virtual void Encode(const CAckMsgSession            &Msg);
    virtual void Encode(const CSmsMsg_RegisterReq       &Msg);
    virtual void Encode(const CSmsMsg_RegisterAck       &Msg);

    bool Decode(CAckMsgSession                          &Msg);
    bool Decode(CSmsMsg_RegisterReq                     &Msg);
    bool Decode(CSmsMsg_RegisterAck                     &Msg);


    // 鉴权 | 注册 | 心跳 | 注销
    virtual void    Encode(const CSmsMsg_requestAuth    &Msg);
    bool            Decode(CSmsMsg_requestAuth          &Msg);
    // 鉴权 | 注册 | 心跳 | 注销 ACK
    virtual void    Encode(const CSmsMsg_requestAuthACK &Msg);
    bool            Decode(CSmsMsg_requestAuthACK       &Msg);


    // 转发
    virtual void    Encode(const CSmsMsg_requestSend    &Msg);
    bool            Decode(CSmsMsg_requestSend          &Msg);
    // 转发 ACK
    virtual void    Encode(const CSmsMsg_requestSendACK &Msg);
    bool            Decode(CSmsMsg_requestSendACK       &Msg);


};
#endif /* _SmsStack_H */
