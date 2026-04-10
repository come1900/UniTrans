/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * promQLStack.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: promQLStack.h 5884 2013-11-02 11:10:13Z WuJunjie $
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

#ifndef _promQLStack_H
#define _promQLStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "promQLMsg.h"

#include "../UniTransRestStack.h"

class CpromQLStack:public CUniTransRestStack
{
public :
    virtual void Encode(const CMmtMsgPlayUrlList &Msg);
    virtual bool Decode(CMmtMsgPlayUrlList       &Msg);

    virtual void Encode(const CMmtMsgValuesIpcamDialing &Msg);
    virtual bool Decode(CMmtMsgValuesIpcamDialing       &Msg);
    virtual void Encode(const CMmtMsgValuesIpcamDialingAck &Msg);
    virtual bool Decode(CMmtMsgValuesIpcamDialingAck       &Msg);

    virtual void Encode(const CMsgMmtReqValueCamDialingAck &Msg);
    virtual bool Decode(CMsgMmtReqValueCamDialingAck       &Msg);
    virtual void Encode(const CMsgMmtReqValueCamDialing &Msg);
    virtual bool Decode(CMsgMmtReqValueCamDialing       &Msg);

    virtual void Encode(const CMmtMsgCamDialing          &Msg);
    virtual bool Decode(CMmtMsgCamDialing                &Msg);

    virtual void Encode(const CMsgPromQlValue          &Msg, const std::string strValueType=PromQlMsg_Key_CpuUsage);
    virtual bool Decode(CMsgPromQlValue                &Msg);

    virtual void Encode(const CMsgMmt_requestMsg          &Msg);
    virtual bool Decode(CMsgMmt_requestMsg                &Msg);

    virtual void Encode(const CAckMsgSimple               &Msg);
    virtual bool Decode(CAckMsgSimple                     &Msg);

    CpromQLStack();
    virtual ~CpromQLStack();

protected:

};
#endif /* _promQLStack_H */
