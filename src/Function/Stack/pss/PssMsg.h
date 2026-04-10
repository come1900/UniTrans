/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PssMsg.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PssMsg.h 5884 2016-05-24 04:24:59Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-05-24 04:24:59  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _PssMsg_H
#define _PssMsg_H

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "../UniTransRestMsg.h"

//#define _DEV_THIS_WITH_COUT 1

// id最小长度
const int PssMsg_Common_Devid_Len_Min = 1;

typedef enum __em_PssMsg_Key_Details_t {
    em_PssMsg_Key_Details_None =  0,	//
    em_PssMsg_Key_Details_QuickPlayurl =  1,	//
    em_PssMsg_Key_Details_Undefined = 44,	//
}EM_PssMsg_Key_Details_T;

// 返回详细信息 0-默认， 1-返回其他详细信息
const std::string PssMsg_Key_Details = "Details";
//
//const std::string PssMsg_Key_Session = "Session";
class CPssMsg_requestMsg
{
public:
    int m_iDetails; // 0-默认， 1-返回其他详细信息
    //std::string m_strSession;

    CPssMsg_requestMsg()
    {
        m_iDetails = em_PssMsg_Key_Details_Undefined;

        //m_strSession = "";
    }

    void dump()
    {
        std::cout << PssMsg_Key_Details << ":" << m_iDetails << std::endl;
        //std::cout << PssMsg_Key_Session << ":" << m_strSession << std::endl;
    }
};
#if 0
//
// tykd消息
//
const std::string TykdAuthMsg_Key_Piece = "Tykd";
const std::string TykdAuthMsg_Key_Account = "Account";
const std::string TykdAuthMsg_Key_Password = "Password";
const std::string TykdAuthMsg_Key_Session = "Session";
class CTykdAuthMsg
{
public:
    std::string Account;
    std::string Password;
    std::string Session;

    CTykdAuthMsg()
    {
        Account = "";
        Password = "";
        Session = "";
    }

    void dump()
    {
        std::cout << TykdAuthMsg_Key_Account << ":" << Account << std::endl;
        std::cout << TykdAuthMsg_Key_Password << ":" << Password << std::endl;
        std::cout << TykdAuthMsg_Key_Session << ":" << Session << std::endl;
    }
};
#endif
//
// Stream message
//
const std::string StreamMsg_Key_Piece       = "Stream";
const std::string StreamMsg_Key_DevID       = "DevID";
const std::string StreamMsg_Key_Channel     = "Channel";
const std::string StreamMsg_Key_Stream      = "Stream";
const std::string StreamMsg_Key_VideoEncode      = "VideoEncode";
const std::string StreamMsg_Key_AudioEncode      = "AudioEncode";
const std::string StreamMsg_Key_Description = "Description";
class CStreamMsg
{
public:
    std::string DevID        ;
    std::string Channel      ;
    std::string Stream       ;
    std::string VideoEncode   ;//视频编码
    std::string AudioEncode   ;//
    std::string Description  ;

    CStreamMsg()
    {
        DevID       = "" ;
        Channel     = "" ;
        Stream      = "" ;
        VideoEncode      = "" ;
        AudioEncode      = "" ;
        Description = "" ;
    }

    CStreamMsg & operator=(const CStreamMsg &t1)
    {
        DevID    = t1.DevID   ;
        Channel  = t1.Channel ;
        Stream   = t1.Stream  ;
        VideoEncode   = t1.VideoEncode  ;
        AudioEncode   = t1.AudioEncode  ;
        Description   = t1.Description  ;

        return *this;

    }


    void dump()
    {
        std::cout << StreamMsg_Key_DevID       << ":" << DevID << std::endl;
        std::cout << StreamMsg_Key_Channel     << ":" << Channel << std::endl;
        std::cout << StreamMsg_Key_Stream      << ":" << Stream << std::endl;
        std::cout << StreamMsg_Key_VideoEncode << ":" << VideoEncode << std::endl;
        std::cout << StreamMsg_Key_AudioEncode << ":" << AudioEncode << std::endl;
        std::cout << StreamMsg_Key_Description << ":" << Description << std::endl;

    }
};

//
// Hls message
//
const std::string HlsMsg_Key_Piece = "Hls";
const std::string HlsMsg_Key_PlayUrl     = "PlayUrl";
const std::string HlsMsg_Key_PlayUrlHlss     = "PlayUrlHlss";
const std::string HlsMsg_Key_PlayUrlRtmp     = "PlayUrlRtmp";
const std::string HlsMsg_Key_PlayUrlFlv     = "PlayUrlFlv";
const std::string HlsMsg_Key_PlayUrlDash     = "PlayUrlDash";
const std::string HlsMsg_Key_MediaSource     = "MediaSource";
const std::string HlsMsg_Key_Snapshot    = "Snapshot";
const std::string HlsMsg_Key_ValidPeriod = "ValidPeriod";
const std::string HlsMsg_Key_StreamID = "StreamID";
const std::string HlsMsg_Key_Status = "Status";
const std::string HlsMsg_Key_Description = "Description";
class CHlsMsg
{
public:
    std::string PlayUrl        ;
    std::string PlayUrlHlss        ;
    std::string PlayUrlRtmp;
    std::string PlayUrlFlv;
    std::string PlayUrlDash;
    std::string MediaSource        ;
    std::string Snapshot      ;
    std::string ValidPeriod;
    std::string StreamID;
    std::string Status;
    std::string Description       ;

    CHlsMsg()
    {
        PlayUrl     = "" ;
        PlayUrlHlss     = "" ;
        PlayUrlRtmp = "" ;
        PlayUrlFlv = "" ;
        PlayUrlDash = "" ;
        MediaSource     = "" ;
        Snapshot    = "" ;
        ValidPeriod = "" ;
        StreamID = "" ;
        Status = "" ;
        Description = "" ;
    }

    void dump()
    {
        std::cout << HlsMsg_Key_PlayUrl     << ":" << PlayUrl     << std::endl;
        std::cout << HlsMsg_Key_PlayUrlHlss     << ":" << PlayUrlHlss     << std::endl;
        std::cout << HlsMsg_Key_PlayUrlRtmp<< ":" << PlayUrlRtmp<< std::endl;
        std::cout << HlsMsg_Key_PlayUrlFlv<< ":" << PlayUrlFlv << std::endl;
        std::cout << HlsMsg_Key_PlayUrlDash<< ":" << PlayUrlDash << std::endl;
        std::cout << HlsMsg_Key_MediaSource     << ":" << MediaSource     << std::endl;
        std::cout << HlsMsg_Key_Snapshot    << ":" << Snapshot    << std::endl;
        std::cout << HlsMsg_Key_ValidPeriod << ":" << ValidPeriod<< std::endl;
        std::cout << HlsMsg_Key_StreamID << ":" << StreamID<< std::endl;
        std::cout << HlsMsg_Key_Status << ":" << Status << std::endl;
        std::cout << HlsMsg_Key_Description << ":" << Description << std::endl;

    }
};

const std::string PssMsg_Key_devId = "devId";

#if 0

//
// 转码开启请求消息
//
const std::string PssMsg_Key_urlRtsp = "urlRtsp";
class CPssMsg_requestStart:public CPssMsg_requestMsg
{
public:
    // 需要拉流的RTSP 地址
    std::string m_strRTSPUrl;
    // 分发ID， 作为HLS链接的标识
    std::string m_strDevId;

    CPssMsg_requestStart()
    {
        m_strRTSPUrl = "";
        m_strDevId = "";
    }

    void dump()
    {
        std::cout << PssMsg_Key_urlRtsp << ":" << m_strRTSPUrl << std::endl;
        std::cout << PssMsg_Key_devId << ":" << m_strDevId << std::endl;
    }
};

//
// 转码开启返回消息
//
const std::string PssMsg_Key_urlHls = "urlHls";
class CPssMsg_ackStart:public CAckMsgSimple
{
public:
    // 需要拉流的RTSP 地址
    std::string m_strHLSUrl;

    CPssMsg_ackStart()
    {
        m_strHLSUrl = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();
        std::cout << PssMsg_Key_urlHls << ":" << m_strHLSUrl << std::endl;
    }
};

//
// 转码关闭请求消息
//
//const std::string PssMsg_Key_urlRtsp = "urlRtsp";
//const std::string PssMsg_Key_devId = "devId";
class CPssMsg_requestStop:public CPssMsg_requestMsg
{
public:
    // 需要拉流的RTSP 地址
    std::string m_strRTSPUrl;
    // 分发ID， 作为HLS链接的标识
    std::string m_strDevId;

    CPssMsg_requestStop()
    {
        m_strRTSPUrl = "";
        m_strDevId = "";
    }

    void dump()
    {
        std::cout << PssMsg_Key_urlRtsp << ":" << m_strRTSPUrl << std::endl;
        std::cout << PssMsg_Key_devId << ":" << m_strDevId << std::endl;
    }
};

//
// 转码关闭返回消息
//
//const std::string PssMsg_Key_urlHls = "urlHls";
class CPssMsg_ackStop:public CAckMsgSimple
{
public:
    // 需要拉流的RTSP 地址
    //std::string m_strHLSUrl;

    CPssMsg_ackStop()
    {
        //m_strHLSUrl = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();
        //std::cout << PssMsg_Key_urlHls << ":" << m_strHLSUrl << std::endl;
    }
};

//
// 天翼看店rtsp2hls开启请求消息
//
class CPssMsgHlsPullTykdStartS:public CPssMsg_requestMsg
{
public:
    // 需要拉流的RTSP 地址
    std::string HlsStreamID;
    // 分发ID， 作为HLS链接的标识
    std::string TykdDevID;

    CPssMsgHlsPullTykdStartS()
    {
        HlsStreamID = "";
        TykdDevID = "";
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();
        std::cout << HlsMsg_Key_StreamID << ":" << HlsStreamID << std::endl;
        std::cout << StreamMsg_Key_DevID << ":" << TykdDevID << std::endl;
    }
};

//
// 天翼看店rtsp2hls转码开启返回消息
//
class CPssMsgHlsPullTykdStartSAck:public CAckMsgSimple
{
public:
    // 需要拉流的RTSP 地址
    std::string m_strHLSUrl;

    CPssMsgHlsPullTykdStartSAck()
    {
        m_strHLSUrl = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();
        std::cout << HlsMsg_Key_PlayUrl<< ":" << m_strHLSUrl << std::endl;
    }
};
//
// 天翼看店rtsp2hls转码关闭请求消息
//
//const std::string PssMsg_Key_urlRtsp = "urlRtsp";
//const std::string PssMsg_Key_devId = "devId";
class CPssMsgHlsPullTykdStopS:public CPssMsg_requestMsg
{
public:
    // 需要拉流的RTSP 地址
    std::string HlsStreamID;
    // 分发ID， 作为HLS链接的标识
    std::string TykdDevID;

    CPssMsgHlsPullTykdStopS()
    {
        HlsStreamID = "";
        TykdDevID = "";
    }

    void dump()
    {
        std::cout << PssMsg_Key_urlRtsp << ":" << HlsStreamID << std::endl;
        std::cout << PssMsg_Key_devId << ":" << TykdDevID << std::endl;
    }
};

//
// 天翼看店rtsp2hls转码关闭返回消息
//
//const std::string PssMsg_Key_urlHls = "urlHls";
class CPssMsgHlsPullTykdStopSAck:public CAckMsgSimple
{
public:
    // 需要拉流的RTSP 地址
    //std::string m_strHLSUrl;

    CPssMsgHlsPullTykdStopSAck()
    {
        //m_strHLSUrl = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();
        //std::cout << PssMsg_Key_urlHls << ":" << m_strHLSUrl << std::endl;
    }
};
#endif

//
// 天翼看店系统码流的 转码开启请求消息
//
class CPssMsgHlsPullTykdStart:public CPssMsg_requestMsg
{
public:
    //CTykdAuthMsg Tykd;
    CStreamMsg Stream;
    CHlsMsg Hls;

    CPssMsgHlsPullTykdStart()
    {
        //CPssMsg_requestMsg::CPssMsg_requestMsg();
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();

        //Tykd.dump();
        Stream.dump();
        Hls.dump();
    }
};

// 天翼看店系统码流的 转码开启反馈消息
class CPssMsgHlsPullTykdStartAck:public CAckMsgSimple
{
public:
    CHlsMsg Hls;

    CPssMsgHlsPullTykdStartAck()
    {
        //CAckMsgSimple::CAckMsgSimple();
    }

    void dump()
    {
        CAckMsgSimple::dump();
        Hls.dump();
    }
};
//
// Delivery message
//
const std::string DeliveryMsg_Key_Piece       = "Delivery";
const std::string DeliveryMsg_Key_UrlHls      = "UrlHls";
const std::string DeliveryMsg_Key_UrlRtmp     = "UrlRtmp";
const std::string DeliveryMsg_Key_MediaSource     = "MediaSource";
const std::string DeliveryMsg_Key_UrlRtsp     = "UrlRtsp";
const std::string DeliveryMsg_Key_Snapshot    = "Snapshot";
const std::string DeliveryMsg_Key_Description = "Description";
class CDeliveryMsg
{
public:
    std::string UrlHls        ;
    std::string UrlRtmp        ;
    std::string MediaSource        ;
    std::string UrlRtsp        ;
    std::string Snapshot      ;
    //std::string ValidPeriod;
    //std::string StreamID;
    std::string Description       ;

    CDeliveryMsg()
    {
        UrlHls     = "" ;
        UrlRtmp     = "" ;
        UrlRtsp     = "" ;
        Snapshot    = "" ;
        //ValidPeriod = "" ;
        //StreamID = "" ;
        Description = "" ;
    }

    void dump()
    {
        std::cout << DeliveryMsg_Key_UrlHls      << ":" << UrlHls      << std::endl;
        std::cout << DeliveryMsg_Key_UrlRtmp     << ":" << UrlRtmp     << std::endl;
        std::cout << DeliveryMsg_Key_MediaSource     << ":" << MediaSource     << std::endl;
        std::cout << DeliveryMsg_Key_UrlRtsp     << ":" << UrlRtsp     << std::endl;
        std::cout << DeliveryMsg_Key_Snapshot    << ":" << Snapshot    << std::endl;
        //std::cout << DeliveryMsg_Key_ValidPeriod << ":" << ValidPeriod << std::endl;
        //std::cout << DeliveryMsg_Key_StreamID    << ":" << StreamID    << std::endl;
        std::cout << DeliveryMsg_Key_Description << ":" << Description << std::endl;
    }
};

// 天翼看店系统码流的 转码开启反馈消息
class CPssMsgDeliveryStreamStartAck:public CAckMsgSimple
{
public:
    CDeliveryMsg Delivery;

    CPssMsgDeliveryStreamStartAck()
    {
    }

    void dump()
    {
        CAckMsgSimple::dump();
        Delivery.dump();
    }
};

//
// 天翼看店系统码流的 转码关闭请求消息
//
class CPssMsgHlsPullTykdStop:public CPssMsg_requestMsg
{
public:
    // CTykdAuthMsg Tykd;
    CStreamMsg Stream;
    CHlsMsg Hls;

    CPssMsgHlsPullTykdStop()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

        //Tykd.dump();
        Stream.dump();
        Hls.dump();
    }
};

// 天翼看店系统码流的 转码关闭回复消息
class CPssMsgHlsPullTykdStopAck:public CAckMsgSimple
{
public:

    CPssMsgHlsPullTykdStopAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
    }
};

//
// Rtsp message
//
const std::string RtspMsg_Key_Piece = "Rtsp";
const std::string RtspMsg_Key_PlayUrl     = "PlayUrl";
const std::string RtspMsg_Key_Snapshot    = "Snapshot";
const std::string RtspMsg_Key_ValidPeriod = "ValidPeriod";
const std::string RtspMsg_Key_StreamID = "StreamID";
const std::string RtspMsg_Key_Status = "Status";
const std::string RtspMsg_Key_Description = "Description";
class CRtspMsg
{
public:
    std::string PlayUrl        ;
    std::string Snapshot      ;
    std::string ValidPeriod;
    std::string StreamID;
    std::string Status;
    std::string Description       ;

    CRtspMsg()
    {
        PlayUrl     = "" ;
        Snapshot    = "" ;
        ValidPeriod = "" ;
        StreamID = "" ;
        Status = "" ;
        Description = "" ;
    }

    void dump()
    {
        std::cout << RtspMsg_Key_PlayUrl     << ":" << PlayUrl     << std::endl;
        std::cout << RtspMsg_Key_Snapshot    << ":" << Snapshot    << std::endl;
        std::cout << RtspMsg_Key_ValidPeriod << ":" << ValidPeriod<< std::endl;
        std::cout << RtspMsg_Key_StreamID << ":" << StreamID<< std::endl;
        std::cout << RtspMsg_Key_Status << ":" << Status<< std::endl;
        std::cout << RtspMsg_Key_Description << ":" << Description << std::endl;

    }
};
//
// 天翼看店系统码流的 转码开启请求消息
//
class CPssMsgRtspPullTykdStart:public CPssMsg_requestMsg
{
public:
    //CTykdAuthMsg Tykd;
    CStreamMsg Stream;
    CRtspMsg Rtsp;

    CPssMsgRtspPullTykdStart()
    {
        //CPssMsg_requestMsg::CPssMsg_requestMsg();
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();

        //Tykd.dump();
        Stream.dump();
        Rtsp.dump();
    }
};

// 天翼看店系统码流的 转码开启反馈消息

class CPssMsgRtspPullTykdStartAck:public CPandoraMsg_Common, public CAckMsgSimple
{
public:
    CRtspMsg Rtsp;

    CPssMsgRtspPullTykdStartAck()
    {
        //CAckMsgSimple::CAckMsgSimple();
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        CAckMsgSimple::dump();
        Rtsp.dump();
    }
};

//
// 天翼看店系统码流的 转码关闭请求消息
//
class CPssMsgRtspPullTykdStop:public CPssMsg_requestMsg
{
public:
    //CTykdAuthMsg Tykd;
    CStreamMsg Stream;
    CRtspMsg Rtsp;

    CPssMsgRtspPullTykdStop()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

        //Tykd.dump();
        Stream.dump();
        Rtsp.dump();
    }
};

// 天翼看店系统码流的 转码关闭回复消息
class CPssMsgRtspPullTykdStopAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:

    CPssMsgRtspPullTykdStopAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
    }
};

//
// Rtmp message
//
const std::string RtmpMsg_Key_Piece = "Rtmp";
const std::string RtmpMsg_Key_PlayUrl     = "PlayUrl";
const std::string RtmpMsg_Key_MediaSource     = "MediaSource";
const std::string RtmpMsg_Key_Snapshot    = "Snapshot";
const std::string RtmpMsg_Key_ValidPeriod = "ValidPeriod";
const std::string RtmpMsg_Key_StreamID = "StreamID";
const std::string RtmpMsg_Key_Status = "Status";
const std::string RtmpMsg_Key_Description = "Description";
class CRtmpMsg
{
public:
    std::string PlayUrl        ;
    std::string MediaSource        ;
    std::string Snapshot      ;
    std::string ValidPeriod;
    std::string StreamID;
    std::string Status        ;
    std::string Description       ;

    CRtmpMsg()
    {
        PlayUrl     = "" ;
        Snapshot    = "" ;
        ValidPeriod = "" ;
        StreamID = "" ;
        Status = "" ;
        Description = "" ;
    }

    void dump()
    {
        std::cout << RtmpMsg_Key_PlayUrl     << ":" << PlayUrl     << std::endl;
        std::cout << RtmpMsg_Key_MediaSource     << ":" << MediaSource     << std::endl;
        std::cout << RtmpMsg_Key_Snapshot    << ":" << Snapshot    << std::endl;
        std::cout << RtmpMsg_Key_ValidPeriod << ":" << ValidPeriod<< std::endl;
        std::cout << RtmpMsg_Key_StreamID << ":" << StreamID << std::endl;
        std::cout << RtmpMsg_Key_Status << ":" << Status << std::endl;
        std::cout << RtmpMsg_Key_Description << ":" << Description << std::endl;

    }
};

//
// 天翼看店系统码流的 转码开启请求消息
//
class CPssMsgRtmpPullTykdStart:public CPssMsg_requestMsg
{
public:
    //CTykdAuthMsg Tykd;
    CStreamMsg Stream;
    CRtmpMsg Rtmp;

    CPssMsgRtmpPullTykdStart()
    {
        //CPssMsg_requestMsg::CPssMsg_requestMsg();
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();

        //Tykd.dump();
        Stream.dump();
        Rtmp.dump();
    }
};

const std::string StorageInfo_Key_Piece       = "StorageInfo";
const std::string StorageInfo_Key_Type        = "Type"; // 类型： 1-本机存储，2-海康NVR，3-大华NVR，
const std::string StorageInfo_Key_Name        = "Name"; // Url / Filename etc ...
const std::string StorageInfo_Key_Channel     = "Channel";// 在NVR中是第？路
const std::string StorageInfo_Key_Description = "Description";

const std::string StorageInfo_Key_Status      = "Status";
const std::string StorageInfo_Key_Capacity    = "Capacity";
const std::string StorageInfo_Key_FreeSpace   = "FreeSpace";

//存储信息--返回用
class CStorageInfo
{
public:
    std::string Type          ;
    std::string Name          ;
    std::string Channel       ;
    std::string Description   ;

    unsigned int Status       ; //0-未知, 1-正常, 11-介质异常
    unsigned int Capacity     ; // MB
    unsigned int FreeSpace    ; // MB

    CStorageInfo()
    {
        Type          = "" ;
        Name          = "" ;
        Channel       = "" ;
        Description   = "" ;

        Status        = 0 ;
        Capacity      = 0 ;
        FreeSpace     = 0 ;
    }

    CStorageInfo & operator=(const CStorageInfo &t1)
    {
        Type          = t1.Type        ;
        Name          = t1.Name        ;
        Channel       = t1.Channel     ;
        Description   = t1.Description ;

        Status        = t1.Status      ;
        Capacity      = t1.Capacity    ;
        FreeSpace     = t1.FreeSpace   ;

        return *this;
    }

    bool isValid() const
    {
        return !Name.empty();
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << StorageInfo_Key_Type        << ":" << Type        << std::endl ;
        std::cout << StorageInfo_Key_Name        << ":" << Name        << std::endl ;
        std::cout << StorageInfo_Key_Channel     << ":" << Channel     << std::endl ;
        std::cout << StorageInfo_Key_Description << ":" << Description << std::endl ;

        std::cout << StorageInfo_Key_Status      << ":" << Status      << std::endl ;
        std::cout << StorageInfo_Key_Capacity    << ":" << Capacity    << std::endl ;
        std::cout << StorageInfo_Key_FreeSpace   << ":" << FreeSpace   << std::endl ;
#endif //_DEV_THIS_WITH_COUT
    }
};

// 天翼看店系统码流的 转码开启反馈消息
class CPssMsgRtmpPullTykdStartAck:public CAckMsgSimple
{
public:
    CRtmpMsg Rtmp;
    CStorageInfo Storage;

    CPssMsgRtmpPullTykdStartAck()
    {
        //CAckMsgSimple::CAckMsgSimple();
    }

    void dump()
    {
        CAckMsgSimple::dump();
        Rtmp.dump();
        Storage.dump();
    }
};

//
// 天翼看店系统码流的 转码关闭请求消息
//
class CPssMsgRtmpPullTykdStop:public CPssMsg_requestMsg
{
public:
    // CTykdAuthMsg Tykd;
    CStreamMsg Stream;
    CRtmpMsg Rtmp;

    CPssMsgRtmpPullTykdStop()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

        //Tykd.dump();
        Stream.dump();
        Rtmp.dump();
    }
};

// 天翼看店系统码流的 转码关闭回复消息
class CPssMsgRtmpPullTykdStopAck:public CAckMsgSimple
{
public:

    CPssMsgRtmpPullTykdStopAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
    }
};

//
// Pandora -- 视频云消息
//
const std::string PandoraMsg_Key_Type = "Type";// 某消息的标识
//开启和关闭均用此消息体 2016-10-26 17:10:28
class CPandoraMsg_TykdPullStart:public CPandoraMsg_Common
{
public:
    CStreamMsg Stream;
    // 0   -- hls rtmp
    // 1 -- rtsp
    int Type;

    CPandoraMsg_TykdPullStart()
    {
        Type = 0;
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        Stream.dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << PandoraMsg_Key_Type << ":" << Type << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string PandoraMsg_Key_hlsPlayUrl = "hlsPlayUrl";// 播放地址
const std::string PandoraMsg_Key_rtmpPlayUrl = "rtmpPlayUrl";// 播放地址
const std::string PandoraMsg_Key_rtspPlayUrl = "rtspPlayUrl";// 播放地址

class CPandoraMsg_TykdPullStartAck:public CPandoraMsg_Common, public CAckMsgSimple
{
public:
    std::string hlsPlayUrl;
    std::string rtmpPlayUrl;
    std::string rtspPlayUrl;

    CPandoraMsg_TykdPullStartAck()
    {
        hlsPlayUrl = "";
        rtmpPlayUrl = "";
        rtspPlayUrl = "";
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        CAckMsgSimple::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << PandoraMsg_Key_hlsPlayUrl << ":" << hlsPlayUrl << std::endl;
        std::cout << PandoraMsg_Key_rtmpPlayUrl << ":" << rtmpPlayUrl << std::endl;
        std::cout << PandoraMsg_Key_rtspPlayUrl << ":" << rtspPlayUrl << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};
#if 0
class CPandoraMsg_TykdPullStop:public CPandoraMsg_Common
{
public:
    CStreamMsg Stream;
    // 0   -- hls rtmp rtsp
    // 1 -- rtsp
    int Type;

    CPandoraMsg_TykdPullStop()
    {
        Type = 0;
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        Stream.dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << PandoraMsg_Key_Type << ":" << Type << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};
class CPandoraMsg_TykdPullStopAck:public CPandoraMsg_Common, public CAckMsgSimple
{
public:
    std::string hlsPlayUrl;
    std::string rtmpPlayUrl;
    std::string rtspPlayUrl;

    CPandoraMsg_TykdPullStopAck()
    {
        hlsPlayUrl = "";
        rtmpPlayUrl = "";
        rtspPlayUrl = "";
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        CAckMsgSimple::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << PandoraMsg_Key_hlsPlayUrl << ":" << hlsPlayUrl << std::endl;
        std::cout << PandoraMsg_Key_rtmpPlayUrl << ":" << rtmpPlayUrl << std::endl;
        std::cout << PandoraMsg_Key_rtspPlayUrl << ":" << rtspPlayUrl << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};
#endif

const std::string pssChannelCreate_Key_Name = "Name";
const std::string pssChannelCreate_Key_Desc = "Desc";
const std::string pssChannelCreate_Key_Type = "Type";
class CPandoraMsg_pssChannelCreate:public CPandoraMsg_Common
{
public:
    std::string Name;
    std::string Desc;
    std::string Type;

    CPandoraMsg_pssChannelCreate()
    {
        Name       = "" ;
        Desc = "" ;
        Type = "" ;
    }

    CPandoraMsg_pssChannelCreate & operator=(const CPandoraMsg_pssChannelCreate &t1)
    {
        Name    = t1.Name;
        Desc    = t1.Desc;
        Type   = t1.Type;

        return *this;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << pssChannelCreate_Key_Name       << ":" << Name<< std::endl;
        std::cout << pssChannelCreate_Key_Desc       << ":" << Desc<< std::endl;
        std::cout << pssChannelCreate_Key_Type<< ":" << Type << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp = "PushUrlRtsp";
const std::string PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp = "PushUrlRtmp";
const std::string PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls  = "PlayUrlHls";
const std::string PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp = "PlayUrlRtmp";
const std::string PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp = "PlayUrlRtsp";
class CPandoraMsg_pssChannelCreateAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:
    std::string PushUrlRtsp ;
    std::string PushUrlRtmp ;
    std::string PlayUrlHls  ;
    std::string PlayUrlRtmp ;
    std::string PlayUrlRtsp ;

    CPandoraMsg_pssChannelCreateAck()
    {
        PushUrlRtsp = "";
        PushUrlRtmp = "";
        PlayUrlHls  = "";
        PlayUrlRtmp = "";
        PlayUrlRtsp = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp << ":" << PushUrlRtsp << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp << ":" << PushUrlRtmp << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls  << ":" << PlayUrlHls  << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp << ":" << PlayUrlRtmp << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp << ":" << PlayUrlRtsp << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string pssChannelCancel_Key_Name = "Name";
class CPandoraMsg_pssChannelCancel:public CPandoraMsg_Common
{
public:
    std::string Name;

    CPandoraMsg_pssChannelCancel()
    {
        Name       = "" ;
        m_strMsgSession = "";
    }

    CPandoraMsg_pssChannelCancel & operator=(const CPandoraMsg_pssChannelCancel &t1)
    {
        Name    = t1.Name;
        m_strMsgSession    = t1.m_strMsgSession;

        return *this;
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << pssChannelCancel_Key_Name       << ":" << Name<< std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

class CPandoraMsg_pssChannelCancelAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:

    CPandoraMsg_pssChannelCancelAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
    }
};

const std::string pssChannelQuery_Key_Name = "Name";
class CPandoraMsg_pssChannelQuery:public CPandoraMsg_Common
{
public:
    std::string Name;

    CPandoraMsg_pssChannelQuery()
    {
        Name       = "" ;
    }

    CPandoraMsg_pssChannelQuery & operator=(const CPandoraMsg_pssChannelQuery &t1)
    {
        Name    = t1.Name;
        m_strMsgSession    = t1.m_strMsgSession;

        return *this;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << pssChannelQuery_Key_Name       << ":" << Name<< std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string PandoraMsg_pssChannelQueryAck_Key_PlayUrlHlsStatus = "PlayUrlHlsStatus";
const std::string PandoraMsg_pssChannelQueryAck_Key_PlayUrlRtspStatus = "PlayUrlRtspStatus";
const std::string PandoraMsg_pssChannelQueryAck_Key_AcceptedTimes = "AcceptedTimes";
class CPandoraMsg_pssChannelQueryAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:
    std::string PlayUrlHlsStatus ;
    std::string PlayUrlRtspStatus ;

    int AcceptedTimes;

    std::string PushUrlRtsp ;
    std::string PushUrlRtmp ;
    std::string PlayUrlHls  ;
    std::string PlayUrlRtmp ;
    std::string PlayUrlRtsp ;

    CPandoraMsg_pssChannelQueryAck()
    {
        PlayUrlHlsStatus = "";
        PlayUrlRtspStatus = "";
        AcceptedTimes = 0;

        PushUrlRtsp = "";
        PushUrlRtmp = "";
        PlayUrlHls  = "";
        PlayUrlRtmp = "";
        PlayUrlRtsp = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << PandoraMsg_pssChannelQueryAck_Key_PlayUrlHlsStatus << ":" << PlayUrlHlsStatus << std::endl;
        std::cout << PandoraMsg_pssChannelQueryAck_Key_PlayUrlRtspStatus << ":" << PlayUrlRtspStatus << std::endl;
        std::cout << PandoraMsg_pssChannelQueryAck_Key_AcceptedTimes << ":" << AcceptedTimes << std::endl;


        std::cout << PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp << ":" << PushUrlRtsp << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp << ":" << PushUrlRtmp << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls  << ":" << PlayUrlHls  << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp << ":" << PlayUrlRtmp << std::endl;
        std::cout << PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp << ":" << PlayUrlRtsp << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string pssChannelList_Key_Type = "Type";
class CPandoraMsg_pssChannelList:public CPandoraMsg_Common
{
public:
    std::string Type;

    CPandoraMsg_pssChannelList()
    {
        Type = "" ;
    }

    CPandoraMsg_pssChannelList & operator=(const CPandoraMsg_pssChannelList &t1)
    {
        Type   = t1.Type;

        return *this;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << pssChannelList_Key_Type<< ":" << Type << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string pssChannelListAck_Key_Type = "Type";
const std::string pssChannelListAck_Key_Items = "Items";
const std::string pssChannelListAck_Key_Pages = "Pages";
const std::string pssChannelListAck_Key_TotalItems = "TotalItems";
const std::string pssChannelListAck_Key_Channel = "Channel";
const std::string pssChannelListAck_Key_Name = "Name";
class CPandoraMsg_pssChannelListAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:
    std::string Items;
    std::string Pages;
    int TotalItems;

    std::vector<std::string> m_vChannel;

    CPandoraMsg_pssChannelListAck()
    {
        Items = "";
        Pages = "";
        TotalItems = 0;
    }

    void dump()
    {
        CAckMsgSimple::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << pssChannelListAck_Key_Items << ":" << Items << std::endl;
        std::cout << pssChannelListAck_Key_Pages << ":" << Pages << std::endl;
        std::cout << pssChannelListAck_Key_TotalItems << ":" << TotalItems << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string StreamCapabilityMsg_Key_Piece          = "StreamCapability";
const std::string StreamCapabilityMsg_Key_BandwidthIn    = "BandwidthIn";
const std::string StreamCapabilityMsg_Key_BandwidthOut   = "BandwidthOut";
const std::string StreamCapabilityMsg_Key_CpuMark        = "CpuMark";
const std::string StreamCapabilityMsg_Key_Memory         = "Memory";
const std::string StreamCapabilityMsg_Key_StreamChannelTotal  = "StreamChannelTotal";
const std::string StreamCapabilityMsg_Key_StreamChannelUsed  = "StreamChannelUsed";
const std::string StreamCapabilityMsg_Key_StreamChannelUsing  = "StreamChannelUsing";
class CStreamCapabilityMsg
{
public:
    int BandwidthIn        ; // 入口带宽 bps
    int BandwidthOut       ; // 出口带宽 bps
    int CpuMark            ; // cpu评分，
    int Memory             ; // 内存数量 MByte
    int StreamChannelTotal      ; // 分发能力
    int StreamChannelUsed      ; // 已经分配的
    int StreamChannelUsing      ; // 正在使用的

    CStreamCapabilityMsg()
    {
        BandwidthIn        = 0; // 入口带宽 bps
        BandwidthOut       = 0 ; // 出口带宽 bps
        CpuMark            = 0; // cpu评分，
        Memory             = 0; // 内存数量 MByte
        StreamChannelTotal = 0; // 分发能力
        StreamChannelUsed  = 0; // 已经分配的
        StreamChannelUsing = 0; // 正在使用的
    }

    CStreamCapabilityMsg & operator=(const CStreamCapabilityMsg &t1)
    {
        BandwidthIn        = t1.BandwidthIn   ;
        BandwidthOut       = t1.BandwidthOut  ;
        CpuMark            = t1.CpuMark     ;
        Memory             = t1.Memory      ;
        StreamChannelTotal = t1.StreamChannelTotal  ;
        StreamChannelUsed  = t1.StreamChannelUsed  ;
        StreamChannelUsing = t1.StreamChannelUsing  ;

        return *this;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << StreamCapabilityMsg_Key_BandwidthIn        << ":" << BandwidthIn        << std::endl;
        std::cout << StreamCapabilityMsg_Key_BandwidthOut       << ":" << BandwidthOut       << std::endl;
        std::cout << StreamCapabilityMsg_Key_CpuMark            << ":" << CpuMark            << std::endl;
        std::cout << StreamCapabilityMsg_Key_Memory             << ":" << Memory             << std::endl;
        std::cout << StreamCapabilityMsg_Key_StreamChannelTotal << ":" << StreamChannelTotal << std::endl;
        std::cout << StreamCapabilityMsg_Key_StreamChannelUsed  << ":" << StreamChannelUsed  << std::endl;
        std::cout << StreamCapabilityMsg_Key_StreamChannelUsing << ":" << StreamChannelUsing << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//
// Rec message
//
const std::string RecMsg_Key_Piece = "Rec";
const std::string RecMsg_Key_ValidPeriod = "ValidPeriod";
const std::string RecMsg_Key_FilePeriod = "FilePeriod";
const std::string RecMsg_Key_RecPeriod = "RecPeriod";
class CRecMsg
{
public:
    std::string ValidPeriod        ;//文件有效期
    std::string FilePeriod      ;//文件长度
    std::string RecPeriod      ;//录像时长(录像最长多长停止)

    CRecMsg()
    {
        ValidPeriod = "" ;
        FilePeriod = "" ;
        FilePeriod = "" ;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << RecMsg_Key_ValidPeriod << ":" << ValidPeriod<< std::endl;
        std::cout << RecMsg_Key_FilePeriod << ":" << FilePeriod<< std::endl;
        std::cout << RecMsg_Key_RecPeriod << ":" << RecPeriod<< std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};
//
// 天翼看店系统码流的 录像开启请求消息
//
class CPssMsgRecTykdStart:public CPssMsg_requestMsg
{
public:
    CStreamMsg Stream;
    CRecMsg Rec;

    CPssMsgRecTykdStart()
    {
        //CPssMsg_requestMsg::CPssMsg_requestMsg();
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();

        Stream.dump();
        Rec.dump();
    }
};

// 天翼看店系统码流的 转码开启反馈消息

class CPssMsgRecTykdStartAck:public CPandoraMsg_Common, public CAckMsgSimple
{
public:

    CPssMsgRecTykdStartAck()
    {
        //CAckMsgSimple::CAckMsgSimple();
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        CAckMsgSimple::dump();
    }
};

//
// 天翼看店系统码流的 转码关闭请求消息
//
class CPssMsgRecTykdStop:public CPssMsg_requestMsg
{
public:
    CStreamMsg Stream;

    CPssMsgRecTykdStop()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

        Stream.dump();
    }
};

// 天翼看店系统码流的 录像停止回复消息
class CPssMsgRecTykdStopAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:

    CPssMsgRecTykdStopAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
    }
};

//
// 天翼看店系统码流的 录像删除请求消息
//
const std::string RecMsg_Key_StartDt = "StartDt";
const std::string RecMsg_Key_EndDt = "EndDt";
class CPssMsgRecTykdDel:public CPssMsg_requestMsg
{
public:
    CStreamMsg Stream;
    std::string StartDt;
    std::string EndDt;

    CPssMsgRecTykdDel()
    {
        StartDt = "";
        EndDt = "";
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();

        Stream.dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << RecMsg_Key_StartDt << ":" << StartDt<< std::endl;
        std::cout << RecMsg_Key_EndDt << ":" << EndDt<< std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

// 天翼看店系统码流的 转码开启反馈消息

class CPssMsgRecTykdDelAck:public CPandoraMsg_Common, public CAckMsgSimple
{
public:

    CPssMsgRecTykdDelAck()
    {
        //CAckMsgSimple::CAckMsgSimple();
    }

    void dump()
    {
        CPandoraMsg_Common::dump();
        CAckMsgSimple::dump();
    }
};

//
// 天翼看店系统码流的 录像查询请求消息
//
const std::string RecMsg_Key_Items = "Items";
const std::string RecMsg_Key_Pages = "Pages";
class CPssMsgRecTykdQuery:public CPssMsg_requestMsg
{
public:
    CStreamMsg Stream;
    std::string StartDt;
    std::string EndDt;
    std::string Items;
    std::string Pages;

    CPssMsgRecTykdQuery()
    {
        StartDt = "";
        EndDt = "";

        Items = "";
        Pages = "";
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();

        Stream.dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << RecMsg_Key_StartDt << ":" << StartDt<< std::endl;
        std::cout << RecMsg_Key_EndDt << ":" << EndDt<< std::endl;
        std::cout << RecMsg_Key_Items << ":" << Items<< std::endl;
        std::cout << RecMsg_Key_Pages << ":" << Pages<< std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

//
// RecFile message
//
const std::string FileMsg_Key_Piece   = "Files";
const std::string FileMsg_Key_Url     = "Url";
const std::string FileMsg_Key_SnapUrl     = "SnapUrl";
const std::string FileMsg_Key_MotionUrl     = "MotionUrl";
const std::string FileMsg_Key_Size    = "Size";
const std::string FileMsg_Key_StartDt = "StartDt";
const std::string FileMsg_Key_EndDt   = "EndDt";
class CFileMsg
{
public:
    std::string Url     ;
    std::string SnapUrl     ;
    std::string MotionUrl     ;
    std::string Size    ;
    std::string StartDt ;
    std::string EndDt   ;

    CFileMsg()
    {
        Url     = "";
        SnapUrl = "";
        MotionUrl = "";
        Size    = "";
        StartDt = "";
        EndDt   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << FileMsg_Key_Url     << ":" << Url << std::endl;
        std::cout << FileMsg_Key_SnapUrl << ":" << SnapUrl << std::endl;
        std::cout << FileMsg_Key_MotionUrl << ":" << MotionUrl << std::endl;
        std::cout << FileMsg_Key_Size    << ":" << Size << std::endl;
        std::cout << FileMsg_Key_StartDt << ":" << StartDt << std::endl;
        std::cout << FileMsg_Key_EndDt   << ":" << EndDt << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

// 天翼看店系统码流的 查询录像反馈消息
const std::string RecMsg_Key_TotalItems = "TotalItems";
class CPssMsgRecTykdQueryAck:public CPandoraMsg_Common, public CAckMsgSimple
{
public:
    std::string Items;
    std::string Pages;
    int TotalItems;
    std::list<CFileMsg> Files;

    CPssMsgRecTykdQueryAck()
    {
        Items = "";
        Pages = "";
        TotalItems = 0;
        //CAckMsgSimple::CAckMsgSimple();
    }

    void dump()
    {
        //CPandoraMsg_Common::dump();
        CAckMsgSimple::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << RecMsg_Key_Items << ":" << Items<< std::endl;
        std::cout << RecMsg_Key_Pages << ":" << Pages<< std::endl;
        std::cout << RecMsg_Key_TotalItems << ":" << TotalItems<< std::endl;

        //for (unsigned int i = 0; i < Files.size(); i++)
        for (std::list<CFileMsg>::ci = Files.begin() ; ci != Files.end() ; ++ci)
        {
            //Files[i].dump();
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//2017-04-18 18:45:22
//
// 添加用户
//
const std::string NvpMsg_Key_MP        = "MP";
const std::string NvpMsg_Key_Name      = "Name";
const std::string NvpMsg_Key_Nick      = "Nick";
const std::string NvpMsg_Key_Sex       = "Sex";
const std::string NvpMsg_Key_Birthday  = "Birthday";
const std::string NvpMsg_Key_Add       = "Add";

class CPssMsgNvpAccouontCreate:public CPssMsg_requestMsg
{
public:

    std::string MP        ;
    std::string Name      ;
    std::string Nick      ;
    std::string Sex       ;
    std::string Birthday  ;
    std::string Add       ;

    CPssMsgNvpAccouontCreate()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << NvpMsg_Key_MP        << ":" << MP       << std::endl;
        std::cout << NvpMsg_Key_Name      << ":" << Name     << std::endl;
        std::cout << NvpMsg_Key_Nick      << ":" << Nick     << std::endl;
        std::cout << NvpMsg_Key_Sex       << ":" << Sex      << std::endl;
        std::cout << NvpMsg_Key_Birthday  << ":" << Birthday << std::endl;
        std::cout << NvpMsg_Key_Add       << ":" << Add      << std::endl;

#endif //_DEV_THIS_WITH_COUT

    }
};

// 天翼看店系统码流的 录像停止回复消息
class CPssMsgNvpAccouontCreateAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:

    CPssMsgNvpAccouontCreateAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
    }
};

const std::string NvpMsg_Key_LoginName       = "LoginName";
const std::string NvpMsg_Key_Password        = "Password";
class CPssMsgNvpAccouontMod:public CPssMsg_requestMsg
{
public:

    std::string LoginName      ;
    std::string Password       ;

    CPssMsgNvpAccouontMod()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

#ifdef _DEV_THIS_WITH_COUT
        std::cout << NvpMsg_Key_LoginName        << ":" << LoginName       << std::endl;
        std::cout << NvpMsg_Key_Password      << ":" << Password     << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
const std::string NvpMsg_Key_DevID    = "DevID"    ;
const std::string NvpMsg_Key_Channel  = "Channel"  ;
const std::string NvpMsg_Key_Stream   = "Stream"   ;
const std::string NvpMsg_Key_Title    = "Title"    ;
const std::string NvpMsg_Key_Memo     = "Memo"     ;
const std::string NvpMsg_Key_Addr     = "Addr"     ;
const std::string NvpMsg_Key_StartDt     = "StartDt"     ;
const std::string NvpMsg_Key_EndDt     = "EndDt"     ;
class CPssMsgNvpShareCam2WeChat:public CPssMsg_requestMsg
{
public:
    std::string DevID      ;
    std::string Channel    ;
    std::string Stream     ;
    std::string Title      ;
    std::string Memo       ;
    std::string Addr       ;
    std::string EndDt       ;

    CPssMsgNvpShareCam2WeChat()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << NvpMsg_Key_DevID    << ":" << DevID   << std::endl;
        std::cout << NvpMsg_Key_Channel  << ":" << Channel << std::endl;
        std::cout << NvpMsg_Key_Stream   << ":" << Stream  << std::endl;
        std::cout << NvpMsg_Key_Title    << ":" << Title   << std::endl;
        std::cout << NvpMsg_Key_Memo     << ":" << Memo    << std::endl;
        std::cout << NvpMsg_Key_Addr     << ":" << Addr    << std::endl;
        std::cout << NvpMsg_Key_EndDt     << ":" << EndDt    << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string NvpMsg_Key_UrlHlsPage     = "UrlHlsPage"     ;
// 天翼看店系统码流的 录像停止回复消息
class CPssMsgNvpShareCam2WeChatAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:
    std::string UrlHlsPage       ;

    CPssMsgNvpShareCam2WeChatAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << NvpMsg_Key_UrlHlsPage    << ":" << UrlHlsPage   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

//const std::string NvpMsg_Key_UrlHlsPage     = "UrlHlsPage"     ;
class CPssMsgNvpShareCam2WeChatQueryAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:
    std::string UrlHlsPage       ;

    std::string Title      ;
    std::string Memo       ;
    std::string Addr       ;
    std::string EndDt       ;
    std::string StartDt       ;

    CPssMsgNvpShareCam2WeChatQueryAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << NvpMsg_Key_UrlHlsPage    << ":" << UrlHlsPage   << std::endl;
        std::cout << NvpMsg_Key_Title    << ":" << Title   << std::endl;
        std::cout << NvpMsg_Key_Memo     << ":" << Memo    << std::endl;
        std::cout << NvpMsg_Key_Addr     << ":" << Addr    << std::endl;
        std::cout << NvpMsg_Key_EndDt     << ":" << EndDt    << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//2017-04-18 18:45:22
//
// 设备注册 / 心跳
//
const std::string NvpMsg_Key_Sequence      = "Sequence"; // 消息序号 ， 上电后第一次从1开始，其后累加 1-n
const std::string NvpMsg_Key_Type          = "Type"; // 终端类型，1-ipc；2-手机

const std::string NvpMsg_Key_Vendor        = "Vendor";// 厂商、作者
const std::string NvpMsg_Key_Module        = "Mode";// 型号、类型
//const std::string NvpMsg_Key_DevID         = "DevID";//设备id、终端id
const std::string NvpMsg_Key_DevKey        = "DevKey"; //Key
const std::string NvpMsg_Key_AppVer        = "AppVer"; //AppVer
//const std::string NvpMsg_Key_Addr          = "Addr";

const std::string NvpMsg_Key_LanMac        = "LanMac";//
const std::string NvpMsg_Key_LanIP         = "LanIP";//
const std::string NvpMsg_Key_WLanMac       = "WLanMac";//
const std::string NvpMsg_Key_WLanIP        = "WLanIP";//

const std::string NvpMsg_Key_DataUrl       = "DataUrl";//数据地址， 如： rtsp://192.168.5.66:1554
const std::string NvpMsg_Key_CommUrl       = "CommUrl";//通讯地址， 如： tcp://192.168.5.66:36666
const std::string NvpMsg_Key_WebUrl        = "WebUrl"; //Web界面地址， 如： http://192.168.5.66:8080

const std::string NvpMsg_Key_DataUrlTu     = "DataUrlTu";//远端数据地址， 如： rtsp://192.168.5.66:1554
const std::string NvpMsg_Key_CommUrlTu     = "CommUrlTu";//远端通讯地址， 如： tcp://192.168.5.66:36666
const std::string NvpMsg_Key_WebUrlTu      = "WebUrlTu"; //远端Web界面地址， 如： http://192.168.5.66:8080

class CPssMsgNvpRegister:public CPssMsg_requestMsg
{
public:
    std::string Sequence  ;
    std::string Type      ;
    std::string Vendor    ;
    std::string Module    ;
    std::string DevID     ;
    std::string DevKey    ;
    std::string AppVer    ;
    std::string Addr      ;

    std::string LanMac    ;
    std::string LanIP     ;
    std::string WLanMac   ;
    std::string WLanIP    ;

    std::string DataUrl   ;
    std::string CommUrl   ;
    std::string WebUrl    ;

    std::string DataUrlTu ;
    std::string CommUrlTu ;
    std::string WebUrlTu  ;

    CPssMsgNvpRegister()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();

#ifdef _DEV_THIS_WITH_COUT

        std::cout << NvpMsg_Key_Sequence  << ":" << Sequence  << std::endl;
        std::cout << NvpMsg_Key_Type      << ":" << Type      << std::endl;
        std::cout << NvpMsg_Key_Vendor    << ":" << Vendor    << std::endl;
        std::cout << NvpMsg_Key_Module    << ":" << Module    << std::endl;
        std::cout << NvpMsg_Key_DevID     << ":" << DevID     << std::endl;
        std::cout << NvpMsg_Key_DevKey    << ":" << DevKey    << std::endl;
        std::cout << NvpMsg_Key_AppVer    << ":" << AppVer    << std::endl;
        std::cout << NvpMsg_Key_Addr      << ":" << Addr      << std::endl;

        std::cout << NvpMsg_Key_LanMac    << ":" << LanMac    << std::endl;
        std::cout << NvpMsg_Key_LanIP     << ":" << LanIP     << std::endl;
        std::cout << NvpMsg_Key_WLanMac   << ":" << WLanMac   << std::endl;
        std::cout << NvpMsg_Key_WLanIP    << ":" << WLanIP    << std::endl;

        std::cout << NvpMsg_Key_DataUrl   << ":" << DataUrl   << std::endl;
        std::cout << NvpMsg_Key_CommUrl   << ":" << CommUrl   << std::endl;
        std::cout << NvpMsg_Key_WebUrl    << ":" << WebUrl    << std::endl;

        std::cout << NvpMsg_Key_DataUrlTu << ":" << DataUrlTu << std::endl;
        std::cout << NvpMsg_Key_CommUrlTu << ":" << CommUrlTu << std::endl;
        std::cout << NvpMsg_Key_WebUrlTu  << ":" << WebUrlTu  << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

//
// RecFile message
//
const std::string AdsFileMsg_Key_Ads     = "Ads";

const std::string AdsFileMsg_Key_No      = "No";
const std::string AdsFileMsg_Key_Url     = "Url";
const std::string AdsFileMsg_Key_Sha1    = "Sha1";
const std::string AdsFileMsg_Key_Type    = "Type";
const std::string AdsFileMsg_Key_Length  = "Length";
const std::string AdsFileMsg_Key_Delay   = "Delay";
const std::string AdsFileMsg_Key_StartDt = "StartDt";
const std::string AdsFileMsg_Key_EndDt   = "EndDt";

class CAdsFileMsg
{
public:
    std::string No      ;// 序号
    std::string Url     ;
    std::string Sha1    ;
    std::string Type    ;
    std::string Length  ;
    std::string Delay   ;
    std::string StartDt ;
    std::string EndDt   ;

    CAdsFileMsg()
    {
        No      = "";
        Url     = "";
        Sha1    = "";
        Type    = "";
        Length  = "";
        Delay   = "";
        StartDt = "";
        EndDt   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << AdsFileMsg_Key_No      << ":" << No  << std::endl;
        std::cout << AdsFileMsg_Key_Url     << ":" << Url << std::endl;
        std::cout << AdsFileMsg_Key_Sha1    << ":" << Sha1 << std::endl;
        std::cout << AdsFileMsg_Key_Type    << ":" << Type << std::endl;
        std::cout << AdsFileMsg_Key_Length  << ":" << Length << std::endl;
        std::cout << AdsFileMsg_Key_Delay   << ":" << Delay << std::endl;
        std::cout << AdsFileMsg_Key_StartDt << ":" << StartDt << std::endl;
        std::cout << AdsFileMsg_Key_EndDt   << ":" << EndDt << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string NvpMsg_Key_UrlDataSrv     = "UrlDataSrv"     ;
const std::string NvpMsg_Key_UrlCommSrv     = "UrlCommSrv"     ;

const std::string NvpMsg_Key_UrlStorSrv     = "UrlStorSrv"     ;
const std::string NvpMsg_Key_UrlEventsSrv     = "UrlEventsSrv"     ;
const std::string NvpMsg_Key_UrlAppSrv     = "UrlAppSrv"     ;
// 注册返回
class CPssMsgNvpRegisterAck:public CAckMsgSimple,public CPandoraMsg_Common
{
public:
    std::string UrlDataSrv       ;
    std::string UrlCommSrv       ;

    std::string UrlStorSrv       ; // 存储服务器
    std::string UrlEventsSrv       ;// 事件服务器
    std::string UrlAppSrv       ;// 版本服务器

    std::list<CAdsFileMsg> AdsFiles;

    CPssMsgNvpRegisterAck()
    {}

    void dump()
    {
        CAckMsgSimple::dump();
        CPandoraMsg_Common::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << NvpMsg_Key_UrlDataSrv    << ":" << UrlDataSrv   << std::endl;
        std::cout << NvpMsg_Key_UrlCommSrv    << ":" << UrlCommSrv   << std::endl;

        std::cout << NvpMsg_Key_UrlStorSrv    << ":" << UrlStorSrv   << std::endl;
        std::cout << NvpMsg_Key_UrlEventsSrv    << ":" << UrlEventsSrv   << std::endl;
        std::cout << NvpMsg_Key_UrlAppSrv    << ":" << UrlAppSrv   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }
};

class CPssMsgNvpTermInfo
{
public:
    std::string DevID     ;
    std::string DevKey    ;

    CPssMsgNvpTermInfo()
    {}

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << NvpMsg_Key_DevID     << ":" << DevID     << std::endl;
        std::cout << NvpMsg_Key_DevKey    << ":" << DevKey    << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string NvpMsg_Key_DevOnline        = "Online"; //0- offline, 1-online, 3-unknow
const std::string NvpMsg_Key_DevBinded        = "Binded"; //0- not, 1-binded, 3-unknow
class CPssMsgNvpTermInfoAck:public CAckMsgSimple
{
public:
    std::string Online     ;
    std::string Binded     ;

    CPssMsgNvpTermInfoAck()
    {}

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << NvpMsg_Key_DevOnline     << ":" << Online     << std::endl;
        std::cout << NvpMsg_Key_DevBinded     << ":" << Binded     << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string StandStreamMsg_Key_Piece = "Stream";
const std::string StandStreamMsg_Key_Url         = "Url";
const std::string StandStreamMsg_Key_Account     = "Account";
const std::string StandStreamMsg_Key_Passwd      = "Passwd";
const std::string StandStreamMsg_Key_Channel     = "Channel";
const std::string StandStreamMsg_Key_Subtype     = "Subtype";
const std::string StandStreamMsg_Key_Location    = "Location";
const std::string StandStreamMsg_Key_VideoEncode = "VideoEncode";
const std::string StandStreamMsg_Key_VideoResize = "VideoResize";
const std::string StandStreamMsg_Key_VideoRefps = "VideoRefps";
const std::string StandStreamMsg_Key_AudioEncode = "AudioEncode";
const std::string StandStreamMsg_Key_Description = "Description";

//独立描述信息的流信息
class CStandStreamMsg
{
public:
    std::string Url           ;
    std::string Account       ;
    std::string Passwd        ;
    std::string Channel       ;
    std::string Subtype       ;
    std::string Location      ;
    std::string VideoEncode   ;//视频编码
    std::string VideoResize   ;//视频新尺寸
    std::string VideoRefps   ;//视频新帧率
    std::string AudioEncode   ;//
    std::string Description   ;

    CStandStreamMsg()
    {
        Url           = "" ;
        Account       = "" ;
        Passwd        = "" ;
        Channel       = "" ;
        Subtype       = "" ;
        Location      = "" ;
        VideoEncode   = "0" ;
        VideoResize   = "0" ;
        VideoRefps   = "0" ;
        AudioEncode   = "1" ;// 不传递则用aac
        Description   = "" ;
    }

    CStandStreamMsg & operator=(const CStandStreamMsg &t1)
    {

        Url           = t1.Url         ;
        Account       = t1.Account     ;
        Passwd        = t1.Passwd      ;
        Channel       = t1.Channel     ;
        Subtype       = t1.Subtype     ;
        Location      = t1.Location ;
        VideoEncode   = t1.VideoEncode ;
        VideoResize   = t1.VideoResize ;
        VideoRefps   = t1.VideoRefps ;
        AudioEncode   = t1.AudioEncode ;
        Description   = t1.Description ;

        return *this;

    }


    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << StreamMsg_Key_Url         << ":" << Url         << std::endl ;
        std::cout << StreamMsg_Key_Account     << ":" << Account     << std::endl ;
        std::cout << StreamMsg_Key_Passwd      << ":" << Passwd      << std::endl ;
        std::cout << StreamMsg_Key_Channel     << ":" << Channel     << std::endl ;
        std::cout << StreamMsg_Key_Subtype     << ":" << Subtype     << std::endl ;
        std::cout << StreamMsg_Key_Location    << ":" << Location    << std::endl ;
        std::cout << StreamMsg_Key_VideoEncode << ":" << VideoEncode << std::endl ;
        std::cout << StreamMsg_Key_VideoResize << ":" << VideoResize << std::endl ;
        std::cout << StreamMsg_Key_VideoRefps  << ":" << VideoRefps << std::endl ;
        std::cout << StreamMsg_Key_AudioEncode << ":" << AudioEncode << std::endl ;
        std::cout << StreamMsg_Key_Description << ":" << Description << std::endl ;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string StandStorageMsg_Key_Piece       = "Storage";
const std::string StandStorageMsg_Key_Type        = "Type"; // 类型： 1-本机存储，2-海康NVR，3-大华NVR，
const std::string StandStorageMsg_Key_Name        = "Name"; // Url / Filename etc ...
//const std::string StandStorageMsg_Key_Account     = "Account";
//const std::string StandStorageMsg_Key_Passwd      = "Passwd";
const std::string StandStorageMsg_Key_Channel     = "Channel";// 在NVR中是第？路
const std::string StandStorageMsg_Key_Description = "Description";

const std::string StandStorageMsg_Key_Status = "Status";

//存储信息
class CStandStorageMsg
{
public:
    std::string Type          ;
    std::string Name          ;
    //std::string Account       ;
    //std::string Passwd        ;
    std::string Channel       ;
    std::string Description   ;

    unsigned int Status   ;//0-未知, 1-正常, 11-硬盘异常
    unsigned int Capacity   ;//MB
    unsigned int FreeSpace   ;//MB

    CStandStorageMsg()
    {
        Type          = "" ;
        Name          = "" ;
        //Account       = "" ;
        //Passwd        = "" ;
        Channel       = "" ;
        Description   = "" ;

        Status   = 0 ;
        Capacity   = 0 ;
        FreeSpace   = 0 ;
    }

    CStandStorageMsg & operator=(const CStandStorageMsg &t1)
    {

        Type          = t1.Type        ;
        Name          = t1.Name        ;
        //Account       = t1.Account     ;
        //Passwd        = t1.Passwd      ;
        Channel       = t1.Channel     ;
        Description   = t1.Description ;

        Status   = t1.Status ;
        Capacity   = t1.Capacity ;
        FreeSpace   = t1.FreeSpace ;

        return *this;

    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << StandStorageMsg_Key_Type        << ":" << Type        << std::endl ;
        std::cout << StandStorageMsg_Key_Name        << ":" << Name        << std::endl ;
        //std::cout << StandStorageMsg_Key_Account     << ":" << Account     << std::endl ;
        //std::cout << StandStorageMsg_Key_Passwd      << ":" << Passwd      << std::endl ;
        std::cout << StandStorageMsg_Key_Channel     << ":" << Channel     << std::endl ;
        std::cout << StandStorageMsg_Key_Description << ":" << Description << std::endl ;
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string PssMsg_Key_ID = "ID";
const std::string PssMsg_Key_Alias = "Alias";
const std::string PssMsg_Key_Type = "Type";
const std::string PssMsg_Key_ValidPeriod = "ValidPeriod";
// /hls/pull/stream/start
// /delivery/stream/start
class CPssMsgHlsPullStreamStart:public CPssMsg_requestMsg
{
public:
    std::string ID       ;
    std::string Alias       ;//名称
    std::string Type       ;
    std::string ValidPeriod        ;//有效期

    CStandStreamMsg StandStream;

    // 存储信息 2019-05-13 19:45:08
    CStandStorageMsg Storage;

    CPssMsgHlsPullStreamStart()
    {
        ID = "";
        Type = "";
        ValidPeriod = "";
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << PssMsg_Key_ID    << ":" << ID   << std::endl;
        std::cout << PssMsg_Key_Type    << ":" << Type   << std::endl;
        std::cout << PssMsg_Key_ValidPeriod    << ":" << ValidPeriod   << std::endl;
#endif //_DEV_THIS_WITH_COUT
        StandStream.dump();
        // 存储信息 2019-05-13 19:45:08
        Storage.dump();
    }
};
#if 0
const std::string StreamSrcMsg_Key_Piece = "StreamSrc";
const std::string StreamDstMsg_Key_Piece = "StreamDst";
// 取流并推流
class CPssMsgPullStreamStart:public CPssMsg_requestMsg
{
public:
    std::string ID       ;
    std::string Type       ;
    std::string ValidPeriod        ;//有效期

    CStandStreamMsg StreamSrc;
    CStandStreamMsg StreamDst;

    CPssMsgPullStreamStart()
    {
        ID = "";
        Type = "";
        ValidPeriod = "";
    }

    void dump()
    {
        CPssMsg_requestMsg::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << PssMsg_Key_ID    << ":" << ID   << std::endl;
        std::cout << PssMsg_Key_Type    << ":" << Type   << std::endl;
        std::cout << PssMsg_Key_ValidPeriod    << ":" << ValidPeriod   << std::endl;
#endif //_DEV_THIS_WITH_COUT
        StreamSrc.dump();
        StreamDst.dump();
    }
};
#endif
//
// XX系统码流的 录像开启请求消息
//
// "/rec/stream/start"
class CPssMsgRecStreamStart:public CPssMsg_requestMsg
{
public:
    std::string ID       ;
    std::string Type       ;

    CStandStreamMsg StandStream;
    CRecMsg Rec;

    CPssMsgRecStreamStart()
    {}

    void dump()
    {
        CPssMsg_requestMsg::dump();
#ifdef _DEV_THIS_WITH_COUT

        std::cout << PssMsg_Key_ID    << ":" << ID   << std::endl;
        std::cout << PssMsg_Key_Type    << ":" << Type   << std::endl;
#endif //_DEV_THIS_WITH_COUT
        StandStream.dump();
        Rec.dump();
    }
};

const std::string CPssMsgRecStreamStartAck_Key_Status      = "Status";
const std::string CPssMsgRecStreamStartAck_Key_UpStatus    = "UpStatus";
const std::string CPssMsgRecStreamStartAck_Key_UsedSpace   = "UsedSpace";
const std::string CPssMsgRecStreamStartAck_Key_UsedUpSpace = "UsedUpSpace";

class CPssMsgRecStreamStartAck : public CAckMsgSimple
{
public:
    std::string  Status      ;
    std::string  UpStatus    ;
    unsigned int UsedSpace   ;
    unsigned int UsedUpSpace ;

    CPssMsgRecStreamStartAck()
    {
        Status       = "";
        UpStatus     = "";
        UsedSpace    = 0;
        UsedUpSpace  = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << CPssMsgRecStreamStartAck_Key_Status      << ":" << Status      << std::endl;
        std::cout << CPssMsgRecStreamStartAck_Key_UpStatus    << ":" << UpStatus    << std::endl;
        std::cout << CPssMsgRecStreamStartAck_Key_UsedSpace   << ":" << UsedSpace   << std::endl;
        std::cout << CPssMsgRecStreamStartAck_Key_UsedUpSpace << ":" << UsedUpSpace << std::endl;
#endif //_DEV_THIS_WITH_COUT

        CAckMsgSimple::dump();
    }
};
//2017-05-24 11:00:25
//
// Event message
//
const std::string EventMsg_Key_Piece        = "Events";
const std::string EventMsg_Key_Type         = "Type";
const std::string EventMsg_Key_Data         = "Data";
const std::string EventMsg_Key_StartDt      = "StartDt";
const std::string EventMsg_Key_EndDt        = "EndDt";
const std::string EventMsg_Key_Description  = "Description";
class CEventMsg
{
public:
    std::string Type        ;
    std::string Data        ;
    std::string StartDt     ;
    std::string EndDt       ;
    std::string Description ;

    CEventMsg()
    {
        Type    = "";
        Data    = "";
        StartDt = "";
        EndDt   = "";
        Description    = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << EventMsg_Key_Type        << ":" << Type    << std::endl;
        std::cout << EventMsg_Key_Data        << ":" << Data    << std::endl;
        std::cout << EventMsg_Key_StartDt     << ":" << StartDt << std::endl;
        std::cout << EventMsg_Key_EndDt       << ":" << EndDt   << std::endl;
        std::cout << EventMsg_Key_Description << ":" << Desc    << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

// 事件发布消息
const std::string EventsMsg_Key_DevID = "DevID";
class CPssMsgEventsPub:public CPssMsg_requestMsg
{
public:
    std::string DevID        ;
    std::list<CEventMsg> Event;
    CPssMsgEventsPub()
    {
        DevID = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << EventsMsg_Key_DevID << ":" << Items<< std::endl;

        //for (unsigned int i = 0; i < Files.size(); i++)
        for (std::list<CEventMsg>::ci = Files.begin() ; ci != Files.end() ; ++ci)
        {
            //Files[i].dump();
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }
};

const std::string PssMsg_Key_GroupID      = "GroupID";
const std::string PssMsg_Key_DeviceValues   = "DeviceValues";
const std::string PssMsg_Key_PageSize     = "PageSize";
const std::string PssMsg_Key_Pages        = "Pages";

class CPssMsg_requestMsgDeviceList
{
public:
    std::string m_strGroupID;     // 组id，appkey
    int m_iPageSize;    // 每页多少个 1-n
    int m_iPages;       // 当前页 1-n

    CPssMsg_requestMsgDeviceList()
    {
        m_strGroupID      = "";
        m_iPageSize     = 0;
        m_iPages        = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PssMsg_Key_GroupID       << ":" << m_strGroupID      << std::endl;
        std::cout << PssMsg_Key_PageSize      << ":" << m_iPageSize     << std::endl;
        std::cout << PssMsg_Key_Pages         << ":" << m_iPages        << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PssMsg_Key_Status = "Status";
const std::string PssMsg_Key_Name   = "Name";
const std::string PssMsg_Key_Vendor   = "Vendor";
const std::string PssMsg_Key_Location   = "Location";
const std::string PssMsg_Key_LastOnTime   = "LastOnTime";

const std::string PssMsg_Key_ChannelNum  = "ChannelNum"       ; //通道数量
const std::string PssMsg_Key_HostIp      = "HostIp"           ; // ip
const std::string PssMsg_Key_HttpPort    = "HttpPort"         ; // Http端口
const std::string PssMsg_Key_RtspPort    = "RtspPort"         ; //
const std::string PssMsg_Key_TcpPort     = "TcpPort"          ; //

typedef enum __em_device_status_t {
    em_device_status_Offline =  0,	//
    em_device_status_Online =  1,	//
}EM_DEVICE_STATUS_T;

class CDeviceMsg
{
public:
    std::string m_strDevID;   // 设备ID
    std::string m_strName;    // 名称
    std::string m_strCode;    // 型号
    std::string m_strVersion; // 版本
    std::string m_strVendor;       //
    std::string m_strStatus;       // 状态 0/1 - EM_DEVICE_STATUS_T, 401~499-error code
    std::string m_strLastOnTime;   // 位置
    std::string m_strLocation;     // 位置
    std::string m_strDescription;  // 描述

    std::string m_strChannelNum   ;
    std::string m_strHostIp       ;
    std::string m_strHttpPort     ;
    std::string m_strRtspPort     ;
    std::string m_strTcpPort      ;

    CDeviceMsg()
    {
        m_strDevID = "";
        m_strCode = "";
        m_strVersion= "";
        m_strStatus = "";
        m_strLastOnTime = "";
        m_strName = "";   //
        m_strVendor = "";   //
        m_strLocation = "";   //
        m_strDescription = "";   //

        m_strChannelNum   = "";
        m_strHostIp       = "";
        m_strHttpPort     = "";
        m_strRtspPort     = "";
        m_strTcpPort      = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PssMsg_Key_devId<< ":" << m_strDevID << std::endl;
        std::cout << PssMsg_Key_Type << ":" << m_strCode << std::endl;
        std::cout << UniMsg_Key_Version << ":" << m_strVersion << std::endl;
        std::cout << PssMsg_Key_Status << ":" << m_strStatus << std::endl;
        std::cout << PssMsg_Key_LastOnTime << ":" << m_strLastOnTime << std::endl;
        std::cout << PssMsg_Key_Name<< ":" << m_strName<< std::endl;
        std::cout << PssMsg_Key_Vendor << ":" << m_strVendor<< std::endl;
        std::cout << PssMsg_Key_Location<< ":" << m_strLocation<< std::endl;
        std::cout << UniMsg_Key_Description << ":" << m_strDescription<< std::endl;

        std::cout << PssMsg_Key_ChannelNum     << ":" << m_strChannelNum    << std::endl;
        std::cout << PssMsg_Key_HostIp         << ":" << m_strHostIp        << std::endl;
        std::cout << PssMsg_Key_HttpPort       << ":" << m_strHttpPort      << std::endl;
        std::cout << PssMsg_Key_RtspPort       << ":" << m_strRtspPort      << std::endl;
        std::cout << PssMsg_Key_TcpPort        << ":" << m_strTcpPort       << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string PssMsg_Key_TotalCount   = "TotalCount";
const std::string PssMsg_Key_Devices      = "Devices";

class CPssMsg_requestMsgDeviceListAck : public CAckMsgSimple
{
public:
    std::string m_strGroupID;     // appkey
    int m_iTotalCount;  // 设备总数 0-n
    int m_iPages;       // 当前页 1-n

    std::list<CDeviceMsg> Devices;

    CPssMsg_requestMsgDeviceListAck()
    {
        m_strGroupID      = "";
        m_iTotalCount   = 0;
        m_iPages        = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PssMsg_Key_GroupID       << ":" << m_strGroupID      << std::endl;
        std::cout << PssMsg_Key_TotalCount    << ":" << m_iTotalCount   << std::endl;
        std::cout << PssMsg_Key_Pages         << ":" << m_iPages        << std::endl;

        for (std::list<CDeviceMsg>::iterator ci = Devices.begin() ; ci != Devices.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//2018-11-22 14:58:18
const std::string UniMsg_Key_Agent      = "Agent";

class CUniMsg_requestMsgOption
{
public:
    std::string m_strAgent;     // 服务描述信息， 版本等， StreamSystem4.3.2

    CUniMsg_requestMsgOption()
    {
        m_strAgent      = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_Agent       << ":" << m_strAgent      << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }// dump
};

const std::string UniMsg_Key_Firm     = "Firm";
const std::string UniMsg_Key_FirmDesc = "Desc";
const std::string UniMsg_Key_FirmCode = "Code";
class CFirmMsg
{
public:
    std::string m_strDesc;  // 厂商名
    std::string m_strCode;  // 代号

    CFirmMsg()
    {
        m_strDesc = "";
        m_strCode = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_FirmDesc << ":" << m_strDesc << std::endl;
        std::cout << UniMsg_Key_FirmCode << ":" << m_strCode << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string UniMsg_Key_Server      = "Server";
const std::string UniMsg_Key_Huid      = "Huid";

class CUniMsg_requestMsgOptionAck : public CAckMsgSimple
{
public:
    std::string m_strServer;     // 服务描述信息， 版本等， StreamSystem4.3.2
    std::string m_strHuid;     // id

    std::list<CFirmMsg> Firms;

    CUniMsg_requestMsgOptionAck()
    {
        m_strServer      = "";
        m_strHuid      = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_Server       << ":" << m_strServer      << std::endl;
        std::cout << UniMsg_Key_Huid    << ":" << m_strHuid << std::endl;

        for (std::list<CFirmMsg>::iterator ci = Firms.begin() ; ci != Firms.end() ; ++ci)
        {
            ci->dump();
        }
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
const std::string PssNotifyMsg_Key_Notify    = "Notify"       ; //
const std::string PssNotifyMsg_Key_StreamID  = "StreamID"       ; //
const std::string PssNotifyMsg_Key_ReqAddr   = "ReqAddr"       ; //

const std::string PssNotifyMsg_Key_ReqFlashver   = "Flashver"       ; //
const std::string PssNotifyMsg_Key_ReqPsstk   = "Psstk"       ; //

const std::string PssNotifyMsg_Key_Notify_PlayCount    = "PNM_PlayCount"       ; //处理play和play done消息来维持计数
const std::string PssNotifyMsg_Key_Notify_PlayTTime    = "PNM_PlayTTime"       ; //处理hls的刷新时间
const std::string PssNotifyMsg_Key_Notify_PublishTTime   = "PNM_PublishTTime"       ; //处理publish和update消息publish done消息

class CPssNotifyMsg
{
public:
    std::string m_strNotify;     // 通知消息 call 传入
    std::string m_strStreamID;   // 码流ID name 传入
    std::string m_strReqAddr;    // 请求者地址 addr 传入

    std::string m_strFlashver;    // 用以区分是推流还是播放
    std::string m_strPsstk;    // 自定义参数s=

    CPssNotifyMsg()
    {
        m_strNotify   = "";  
        m_strStreamID = "";
        m_strReqAddr  = ""; 

        m_strFlashver = ""; 
        m_strPsstk = ""; 
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PssNotifyMsg_Key_Notify<< ":"   << m_strNotify << std::endl;
        std::cout << PssNotifyMsg_Key_StreamID<< ":" << m_strStreamID << std::endl;
        std::cout << PssNotifyMsg_Key_ReqAddr<< ":"  << m_strReqAddr << std::endl;

        std::cout << PssNotifyMsg_Key_ReqFlashver << ":"  << m_strFlashver << std::endl;
        std::cout << PssNotifyMsg_Key_ReqPsstk << ":"  << m_strPsstk << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif /* _PssMsg_H */
