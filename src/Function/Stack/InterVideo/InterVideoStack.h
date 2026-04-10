/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * InterVideoStack.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: InterVideoStack.h 5884 2018-03-09 03:22:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-09 03:22:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _InterVideoStack_H
#define _InterVideoStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "InterVideoMsg.h"
#include "../UniTransRestStack.h"

class CInterVideoStack:public CUniTransRestStack
{
public :
    CInterVideoStack();
    virtual ~CInterVideoStack();

    virtual void Encode(const CAckMsgSimple             &Msg);
    virtual bool Decode(CAckMsgSimple                   &Msg);

    virtual void Encode(const CInterVideoMsg_addDevice             &Msg);
    virtual bool Decode(CInterVideoMsg_addDevice                   &Msg);
    virtual void Encode(const CInterVideoMsg_addDeviceAck             &Msg);
    virtual bool Decode(CInterVideoMsg_addDeviceAck                   &Msg);

protected:
};
#endif /* _InterVideoStack_H */
