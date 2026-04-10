/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * KmsStack.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: KmsStack.h 5884 2019-02-27 05:26:39Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-02-27 05:26:39  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _KmsStack_H
#define _KmsStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "KmsMsg.h"
#include "../UniTransRestStack.h"

class CKmsStack:public CUniTransRestStack
{
public :
    CKmsStack();
    virtual ~CKmsStack();

    virtual void Encode(const CAckMsgSimple              &Msg);
    virtual bool Decode(CAckMsgSimple                    &Msg);
    virtual void Encode(const CSmsMsg_requestAuth        &Msg);
    virtual bool Decode(CSmsMsg_requestAuth              &Msg);
    virtual void Encode(const CSmsMsg_requestAuthACK     &Msg);
    virtual bool Decode(CSmsMsg_requestAuthACK           &Msg);

    virtual void Encode(const CUniMsg_reqCreateAppKey    &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKey          &Msg);
    virtual void Encode(const CUniMsg_reqCreateAppKeyAck &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKeyAck       &Msg);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void Encode(const CKmsMsg_requestMsgGenerateKeypair         &Msg);
    virtual bool Decode(CKmsMsg_requestMsgGenerateKeypair               &Msg);
    virtual void Encode(const CKmsMsg_requestMsgGenerateKeypairAck      &Msg);
    virtual bool Decode(CKmsMsg_requestMsgGenerateKeypairAck            &Msg);

    virtual void Encode(const CKmsMsg_requestMsgEncryptAsymmetric         &Msg);
    virtual bool Decode(CKmsMsg_requestMsgEncryptAsymmetric               &Msg);
    virtual void Encode(const CKmsMsg_requestMsgEncryptAsymmetricAck      &Msg);
    virtual bool Decode(CKmsMsg_requestMsgEncryptAsymmetricAck            &Msg);

    virtual void Encode(const CKmsMsg_requestMsgDecryptAsymmetric         &Msg);
    virtual bool Decode(CKmsMsg_requestMsgDecryptAsymmetric               &Msg);
    virtual void Encode(const CKmsMsg_requestMsgDecryptAsymmetricAck      &Msg);
    virtual bool Decode(CKmsMsg_requestMsgDecryptAsymmetricAck            &Msg);

    virtual void Encode(const CKmsMsg_requestMsgGenerateKey         &Msg);
    virtual bool Decode(CKmsMsg_requestMsgGenerateKey               &Msg);
    virtual void Encode(const CKmsMsg_requestMsgGenerateKeyAck      &Msg);
    virtual bool Decode(CKmsMsg_requestMsgGenerateKeyAck            &Msg);
protected:
};
#endif /* _KmsStack_H */
