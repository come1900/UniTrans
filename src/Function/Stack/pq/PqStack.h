/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PqStack.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PqStack.h 5884 2018-03-09 03:22:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-09 03:22:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _PqStack_H
#define _PqStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "PqMsg.h"
#include "../UniTransRestStack.h"

class CPqStack:public CUniTransRestStack
{
public :
    CPqStack();
    virtual ~CPqStack();

    virtual void Encode(const CAckMsgSimple             &Msg);
    virtual bool Decode(CAckMsgSimple                   &Msg);
    // 鉴权 | 注册 | 心跳 | 注销
    virtual void    Encode(const CSmsMsg_requestAuth    &Msg);
    virtual bool Decode(CSmsMsg_requestAuth             &Msg);
    // 鉴权 | 注册 | 心跳 | 注销 ACK
    virtual void    Encode(const CSmsMsg_requestAuthACK &Msg);
    virtual bool Decode(CSmsMsg_requestAuthACK          &Msg);

    virtual void Encode(const CUniMsg_reqCreateAppKey             &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKey                           &Msg);
    virtual void Encode(const CUniMsg_reqCreateAppKeyAck             &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKeyAck                           &Msg);

    // 节点能力
    virtual void Encode(const CStreamCapabilityMsg             &Msg);
    bool Decode(CStreamCapabilityMsg                           &Msg);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void Encode(const CPqMsg_requestMsg         &Msg);
    virtual bool Decode(CPqMsg_requestMsg               &Msg);
    virtual void Encode(const CPqMsg_requestMsgAck      &Msg);
    virtual bool Decode(CPqMsg_requestMsgAck            &Msg);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    virtual void Encode(const CPqMsg_requestMsgPub_IO_Out         &Msg);
    virtual bool Decode(CPqMsg_requestMsgPub_IO_Out               &Msg);

    virtual void Encode(const CPqMsg_requestMsgShareHID         &Msg);
    virtual bool Decode(CPqMsg_requestMsgShareHID               &Msg);

    virtual void Encode(const CPqMsg_requestMsgPubValue &Msg);
    virtual bool Decode(CPqMsg_requestMsgPubValue &Msg);
    virtual void Encode(const CPqMsg_requestMsgPubValueAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgPubValueAck &Msg);

    virtual void Encode(const CPqMsg_requestMsgPubValueList &Msg);
    virtual bool Decode(CPqMsg_requestMsgPubValueList &Msg);
    virtual void Encode(const CPqMsg_requestMsgPubValueListAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgPubValueListAck &Msg);

    virtual void Encode(const CPqMsg_requestMsgSetSchedule &Msg);
    virtual bool Decode(CPqMsg_requestMsgSetSchedule &Msg);
    virtual void Encode(const CPqMsg_requestMsgGetSchedule &Msg);
    virtual bool Decode(CPqMsg_requestMsgGetSchedule &Msg);
    virtual void Encode(const CPqMsg_requestMsgGetScheduleAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgGetScheduleAck &Msg);

    virtual void Encode(const CPqMsg_requestMsgTaskPub &Msg);
    virtual bool Decode(CPqMsg_requestMsgTaskPub &Msg);
    virtual void Encode(const CPqMsg_requestMsgTaskList &Msg);
    virtual bool Decode(CPqMsg_requestMsgTaskList &Msg);
    virtual void Encode(const CPqMsg_requestMsgTaskListAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgTaskListAck &Msg);

    virtual void Encode(const CPqMsg_requestMsgTaskDetails &Msg);
    virtual bool Decode(CPqMsg_requestMsgTaskDetails &Msg);
    virtual void Encode(const CPqMsg_requestMsgTaskDetailsAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgTaskDetailsAck &Msg);

    virtual void Encode(const CPqMsg_requestMsgDeviceList &Msg);
    virtual bool Decode(CPqMsg_requestMsgDeviceList &Msg);
    virtual void Encode(const CPqMsg_requestMsgDeviceListAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgDeviceListAck &Msg);

    virtual void Encode(const CPqMsg_requestMsgSetShpc &Msg);
    virtual bool Decode(CPqMsg_requestMsgSetShpc &Msg);
    virtual void Encode(const CPqMsg_requestMsgSetShpcAck &Msg);
    virtual bool Decode(CPqMsg_requestMsgSetShpcAck &Msg);

protected:
};
#endif /* _PqStack_H */
