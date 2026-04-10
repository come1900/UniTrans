/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PssStack.h - brief
 *
 * Copyright (C) 2016 ezlibs.com, All Rights Reserved.
 *
 * $Id: PssStack.h 5884 2016-07-02 11:10:13Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain -- Intelligent system communication protocol
 *     -
 *
 *  Update:
 *     2016-07-02 11:10:13 WuJunjie Create
 *
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _PssStack_H
#define _PssStack_H

#include <json.h>

#include <algorithm>
#include <stdio.h>

// must be: library/libjson, if in macos, use as:
// #include "../../../libs/include/json/json.h"
#include "PssMsg.h"
#include "../UniTransRestStack.h"

class CPssStack:public CUniTransRestStack
{
public :
    CPssStack();
    virtual ~CPssStack();

    virtual void Encode(const CAckMsgSimple               &Msg);
    virtual bool Decode(CAckMsgSimple                     &Msg);
    // 鉴权 | 注册 | 心跳 | 注销
    virtual void    Encode(const CSmsMsg_requestAuth      &Msg);
    virtual bool Decode(CSmsMsg_requestAuth               &Msg);
    // 鉴权 | 注册 | 心跳 | 注销 ACK
    virtual void    Encode(const CSmsMsg_requestAuthACK   &Msg);
    virtual bool Decode(CSmsMsg_requestAuthACK            &Msg);

    virtual void Encode(const CUniMsg_reqCreateAppKey     &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKey           &Msg);
    virtual void Encode(const CUniMsg_reqCreateAppKeyAck  &Msg);
    virtual bool Decode(CUniMsg_reqCreateAppKeyAck        &Msg);

    virtual void Encode(const CUniMsg_reqGetSetGmLic     &Msg);
    virtual bool Decode(CUniMsg_reqGetSetGmLic           &Msg);
    virtual void Encode(const CUniMsg_reqGetSetGmLicAck  &Msg);
    virtual bool Decode(CUniMsg_reqGetSetGmLicAck        &Msg);

    virtual void Encode(const CUniMsg_reqConfigModify     &Msg);
    virtual bool Decode(CUniMsg_reqConfigModify           &Msg);

    virtual void Encode(const CUniMsg_requestMsgOption    &Msg);
    virtual bool Decode(CUniMsg_requestMsgOption          &Msg);
    virtual void Encode(const CUniMsg_requestMsgOptionAck &Msg);
    virtual bool Decode(CUniMsg_requestMsgOptionAck       &Msg);
//    virtual void Encode(const CPssMsg_requestStart             &Msg);
//    bool Decode(CPssMsg_requestStart                           &Msg);
//    virtual void Encode(const CPssMsg_ackStart             &Msg);
//    bool Decode(CPssMsg_ackStart                           &Msg);
//    virtual void Encode(const CPssMsg_requestStop             &Msg);
//    bool Decode(CPssMsg_requestStop                           &Msg);
//    virtual void Encode(const CPssMsg_ackStop             &Msg);
//    bool Decode(CPssMsg_ackStop                           &Msg);

//    virtual void Encode(const CPssMsgHlsPullTykdStartS             &Msg);
//    bool Decode(CPssMsgHlsPullTykdStartS                           &Msg);
//    virtual void Encode(const CPssMsgHlsPullTykdStopS             &Msg);
//    bool Decode(CPssMsgHlsPullTykdStopS                           &Msg);
//    virtual void Encode(const CPssMsgHlsPullTykdStartSAck             &Msg);
//    bool Decode(CPssMsgHlsPullTykdStartSAck                           &Msg);
//    virtual void Encode(const CPssMsgHlsPullTykdStopSAck             &Msg);
//    bool Decode(CPssMsgHlsPullTykdStopSAck                           &Msg);


    virtual void Encode(const CPssMsgHlsPullTykdStart             &Msg);
    bool Decode(CPssMsgHlsPullTykdStart                           &Msg);
    virtual void Encode(const CPssMsgHlsPullTykdStop             &Msg);
    bool Decode(CPssMsgHlsPullTykdStop                           &Msg);
    virtual void Encode(const CPssMsgHlsPullTykdStartAck             &Msg);
    bool Decode(CPssMsgHlsPullTykdStartAck                           &Msg);
    virtual void Encode(const CPssMsgHlsPullTykdStopAck             &Msg);
    bool Decode(CPssMsgHlsPullTykdStopAck                           &Msg);

    virtual void Encode(const CPssMsgHlsPullStreamStart             &Msg);
    bool Decode(CPssMsgHlsPullStreamStart                           &Msg);
    //virtual void Encode(const CPssMsgPullStreamStart             &Msg);
    //bool Decode(CPssMsgPullStreamStart                           &Msg);
    virtual void Encode(const CPssMsgRecStreamStart             &Msg);
    bool Decode(CPssMsgRecStreamStart                           &Msg);
    virtual void Encode(const CPssMsgRecStreamStartAck             &Msg);
    bool Decode(CPssMsgRecStreamStartAck                           &Msg);

    virtual void Encode(const CPssMsgDeliveryStreamStartAck             &Msg);
    bool Decode(CPssMsgDeliveryStreamStartAck                           &Msg);

    virtual void Encode(const CPssMsgRtspPullTykdStart             &Msg);
    bool Decode(CPssMsgRtspPullTykdStart                           &Msg);
    virtual void Encode(const CPssMsgRtspPullTykdStop             &Msg);
    bool Decode(CPssMsgRtspPullTykdStop                           &Msg);
    virtual void Encode(const CPssMsgRtspPullTykdStartAck             &Msg);
    bool Decode(CPssMsgRtspPullTykdStartAck                           &Msg);
    virtual void Encode(const CPssMsgRtspPullTykdStopAck             &Msg);
    bool Decode(CPssMsgRtspPullTykdStopAck                           &Msg);

    virtual void Encode(const CPssMsgRtmpPullTykdStart             &Msg);
    bool Decode(CPssMsgRtmpPullTykdStart                           &Msg);
    virtual void Encode(const CPssMsgRtmpPullTykdStop             &Msg);
    bool Decode(CPssMsgRtmpPullTykdStop                           &Msg);
    virtual void Encode(const CPssMsgRtmpPullTykdStartAck             &Msg);
    bool Decode(CPssMsgRtmpPullTykdStartAck                           &Msg);
    virtual void Encode(const CPssMsgRtmpPullTykdStopAck             &Msg);
    bool Decode(CPssMsgRtmpPullTykdStopAck                           &Msg);
    
    //2016-10-26 16:15:50
    virtual void Encode(const CPandoraMsg_TykdPullStart             &Msg);
    bool Decode(CPandoraMsg_TykdPullStart                           &Msg);
    virtual void Encode(const CPandoraMsg_TykdPullStartAck             &Msg);
    bool Decode(CPandoraMsg_TykdPullStartAck                           &Msg);

#if 0
    virtual void Encode(const CPandoraMsg_TykdPullStop             &Msg);
    bool Decode(CPandoraMsg_TykdPullStop                           &Msg);
    virtual void Encode(const CPandoraMsg_TykdPullStopAck             &Msg);
    bool Decode(CPandoraMsg_TykdPullStopAck                           &Msg);
#endif
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 通道管理
    virtual void Encode(const CPandoraMsg_pssChannelCreate             &Msg);
    bool Decode(CPandoraMsg_pssChannelCreate                           &Msg);
    virtual void Encode(const CPandoraMsg_pssChannelCreateAck             &Msg);
    bool Decode(CPandoraMsg_pssChannelCreateAck                           &Msg);

    virtual void Encode(const CPandoraMsg_pssChannelCancel             &Msg);
    bool Decode(CPandoraMsg_pssChannelCancel                           &Msg);
    virtual void Encode(const CPandoraMsg_pssChannelCancelAck             &Msg);
    bool Decode(CPandoraMsg_pssChannelCancelAck                           &Msg);

    virtual void Encode(const CPandoraMsg_pssChannelQuery             &Msg);
    bool Decode(CPandoraMsg_pssChannelQuery                           &Msg);
    virtual void Encode(const CPandoraMsg_pssChannelQueryAck             &Msg);
    bool Decode(CPandoraMsg_pssChannelQueryAck                           &Msg);

    virtual void Encode(const CPandoraMsg_pssChannelList             &Msg);
    bool Decode(CPandoraMsg_pssChannelList                           &Msg);
    virtual void Encode(const CPandoraMsg_pssChannelListAck             &Msg);
    bool Decode(CPandoraMsg_pssChannelListAck                           &Msg);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 节点能力
    virtual void Encode(const CStreamCapabilityMsg             &Msg);
    bool Decode(CStreamCapabilityMsg                           &Msg);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 录像管理
    virtual void Encode(const CPssMsgRecTykdStart             &Msg);
    bool Decode(CPssMsgRecTykdStart                           &Msg);
    virtual void Encode(const CPssMsgRecTykdStartAck             &Msg);
    bool Decode(CPssMsgRecTykdStartAck                           &Msg);
    virtual void Encode(const CPssMsgRecTykdStop             &Msg);
    bool Decode(CPssMsgRecTykdStop                           &Msg);
    virtual void Encode(const CPssMsgRecTykdStopAck             &Msg);
    bool Decode(CPssMsgRecTykdStopAck                           &Msg);
    virtual void Encode(const CPssMsgRecTykdDel             &Msg);
    bool Decode(CPssMsgRecTykdDel                           &Msg);
    virtual void Encode(const CPssMsgRecTykdDelAck             &Msg);
    bool Decode(CPssMsgRecTykdDelAck                           &Msg);

    virtual void Encode(const CPssMsgRecTykdQuery             &Msg);
    bool Decode(CPssMsgRecTykdQuery                           &Msg);
    virtual void Encode(CPssMsgRecTykdQueryAck             &Msg);
    bool Decode(CPssMsgRecTykdQueryAck                           &Msg);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void Encode(const CPssMsgNvpAccouontCreate             &Msg);
    bool Decode(CPssMsgNvpAccouontCreate                           &Msg);
    virtual void Encode(const CPssMsgNvpAccouontCreateAck             &Msg);
    bool Decode(CPssMsgNvpAccouontCreateAck                           &Msg);
    virtual void Encode(const CPssMsgNvpAccouontMod             &Msg);
    bool Decode(CPssMsgNvpAccouontMod                           &Msg);

    virtual void Encode(const CPssMsgNvpShareCam2WeChat             &Msg);
    bool Decode(CPssMsgNvpShareCam2WeChat                           &Msg);
    virtual void Encode(const CPssMsgNvpShareCam2WeChatAck             &Msg);
    bool Decode(CPssMsgNvpShareCam2WeChatAck                           &Msg);

    virtual void Encode(const CPssMsgNvpShareCam2WeChatQueryAck             &Msg);
    bool Decode(CPssMsgNvpShareCam2WeChatQueryAck                           &Msg);
    
    virtual void Encode(const CPssMsgNvpRegister             &Msg);
    bool Decode(CPssMsgNvpRegister                           &Msg);
    virtual void Encode(CPssMsgNvpRegisterAck             &Msg);
    bool Decode(CPssMsgNvpRegisterAck                           &Msg);
	    
    virtual void Encode(const CPssMsgNvpTermInfo             &Msg);
    bool Decode(CPssMsgNvpTermInfo                           &Msg);
    virtual void Encode(const CPssMsgNvpTermInfoAck             &Msg);
    bool Decode(CPssMsgNvpTermInfoAck                           &Msg);

    virtual void Encode(CPssMsgEventsPub             &Msg);
    bool Decode(CPssMsgEventsPub                           &Msg);

    virtual void Encode(const CPssMsg_requestMsgDeviceList &Msg);
    virtual bool Decode(CPssMsg_requestMsgDeviceList &Msg);
    virtual void Encode(const CPssMsg_requestMsgDeviceListAck &Msg);
    virtual bool Decode(CPssMsg_requestMsgDeviceListAck &Msg);
protected:
};
#endif /* _PssStack_H */
