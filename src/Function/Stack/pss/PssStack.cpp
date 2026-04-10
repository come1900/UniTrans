/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PssStack.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: PssStack.cpp 0001 2016-07-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     PssMsg ---- Intelligent system communication protocol
 *
 *  Update:
 *     2016-07-02 14:38:33 WuJunjie 549 Create
 *     2016-08-02 10:21:25 WuJunjie 549 all decode interface ready
 *
 *  todo:
 *     异常数据检查
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "PssStack.h"

#include <assert.h>

CPssStack::CPssStack()
{}

CPssStack::~CPssStack()
{
    //    if (m_pJsonValue)
    //    {
    //        delete m_pJsonValue;
    //    }
}

void CPssStack::Encode(const CAckMsgSimple &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPssStack::Decode(CAckMsgSimple &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CPssStack::Encode(const CSmsMsg_requestAuth &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPssStack::Decode(CSmsMsg_requestAuth &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}
void CPssStack::Encode(const CSmsMsg_requestAuthACK &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPssStack::Decode(CSmsMsg_requestAuthACK &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPssStack::Encode(const CUniMsg_reqCreateAppKey &Msg)
{
    CUniTransRestStack::Encode(Msg);
}

bool CPssStack::Decode(CUniMsg_reqCreateAppKey &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPssStack::Encode(const CUniMsg_reqCreateAppKeyAck &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPssStack::Decode(CUniMsg_reqCreateAppKeyAck &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPssStack::Encode(const CUniMsg_reqGetSetGmLic &Msg)
{
    CUniTransRestStack::Encode(Msg);
}

bool CPssStack::Decode(CUniMsg_reqGetSetGmLic &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPssStack::Encode(const CUniMsg_reqGetSetGmLicAck &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPssStack::Decode(CUniMsg_reqGetSetGmLicAck &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPssStack::Encode(const CUniMsg_reqConfigModify &Msg)
{
    CUniTransRestStack::Encode(Msg);
}
bool CPssStack::Decode(CUniMsg_reqConfigModify &Msg)
{
    return CUniTransRestStack::Decode(Msg);
}

void CPssStack::Encode(const CPssMsgHlsPullTykdStart &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Details] = Msg.m_iDetails;
    //var[PssMsg_Key_Session] = Msg.m_strSession;
#if 0
    Json::Value Tykd;
    Tykd[TykdAuthMsg_Key_Account]   = Msg.Tykd.Account   ;
    Tykd[TykdAuthMsg_Key_Password]  = Msg.Tykd.Password   ;
    Tykd[TykdAuthMsg_Key_Session]   = Msg.Tykd.Session   ;
    var[TykdAuthMsg_Key_Piece]     = Tykd;
#endif

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Hls;
    //Hls[HlsMsg_Key_PlayUrl]   = Msg.Hls.PlayUrl;
    //Hls[HlsMsg_Key_Snapshot]   = Msg.Hls.Snapshot;
    Hls[HlsMsg_Key_ValidPeriod]   = Msg.Hls.ValidPeriod;
    Hls[HlsMsg_Key_StreamID]   = Msg.Hls.StreamID;
    //Hls[HlsMsg_Key_Description]   = Msg.Hls.Description;
    var[HlsMsg_Key_Piece]     = Hls;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgHlsPullTykdStart &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    if(m_JsonValue[PssMsg_Key_Details].isInt())
        Msg.m_iDetails = m_JsonValue[PssMsg_Key_Details].asInt();
    //Msg.m_strSession = m_JsonValue[PssMsg_Key_Session].asString();

#if 0
    const Json::Value TykdPieces = m_JsonValue[TykdAuthMsg_Key_Piece];
    Msg.Tykd.Account    = TykdPieces[TykdAuthMsg_Key_Account]   .asString();
    Msg.Tykd.Password    = TykdPieces[TykdAuthMsg_Key_Password]   .asString();
    Msg.Tykd.Session    = TykdPieces[TykdAuthMsg_Key_Session]   .asString();
#endif

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Hls = m_JsonValue[HlsMsg_Key_Piece];
    Msg.Hls.PlayUrl    = Hls[HlsMsg_Key_PlayUrl]   .asString();
    Msg.Hls.Snapshot = Hls[HlsMsg_Key_Snapshot]   .asString();
    Msg.Hls.ValidPeriod = Hls[HlsMsg_Key_ValidPeriod]   .asString();
    Msg.Hls.StreamID= Hls[HlsMsg_Key_StreamID]   .asString();
    Msg.Hls.Description = Hls[HlsMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgHlsPullTykdStop &Msg)
{
    Json::Value var;

    //var[PssMsg_Key_Session] = Msg.m_strSession;
#if 0

    Json::Value Tykd;
    Tykd[TykdAuthMsg_Key_Account]   = Msg.Tykd.Account   ;
    Tykd[TykdAuthMsg_Key_Password]  = Msg.Tykd.Password   ;
    Tykd[TykdAuthMsg_Key_Session]   = Msg.Tykd.Session   ;
    var[TykdAuthMsg_Key_Piece]     = Tykd;
#endif

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    //Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Hls;
    //Hls[HlsMsg_Key_PlayUrl]   = Msg.Hls.PlayUrl;
    //Hls[HlsMsg_Key_Snapshot]   = Msg.Hls.Snapshot;
    //Hls[HlsMsg_Key_ValidPeriod]   = Msg.Hls.ValidPeriod;
    Hls[HlsMsg_Key_StreamID]   = Msg.Hls.StreamID;
    //Hls[HlsMsg_Key_Description]   = Msg.Hls.Description;
    var[HlsMsg_Key_Piece]     = Hls;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgHlsPullTykdStop &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    //Msg.m_strSession = m_JsonValue[PssMsg_Key_Session].asString();

#if 0

    const Json::Value TykdPieces = m_JsonValue[TykdAuthMsg_Key_Piece];
    Msg.Tykd.Account    = TykdPieces[TykdAuthMsg_Key_Account]   .asString();
    Msg.Tykd.Password    = TykdPieces[TykdAuthMsg_Key_Password]   .asString();
    Msg.Tykd.Session    = TykdPieces[TykdAuthMsg_Key_Session]   .asString();
#endif

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    //Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Hls = m_JsonValue[HlsMsg_Key_Piece];
    //Msg.Hls.PlayUrl    = Hls[HlsMsg_Key_PlayUrl]   .asString();
    //Msg.Hls.Snapshot = Hls[HlsMsg_Key_Snapshot]   .asString();
    //Msg.Hls.ValidPeriod = Hls[HlsMsg_Key_ValidPeriod]   .asString();
    Msg.Hls.StreamID= Hls[HlsMsg_Key_StreamID]   .asString();
    //Msg.Hls.Description = Hls[HlsMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgHlsPullTykdStartAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    Json::Value Hls;
    if (!Msg.Hls.PlayUrl.empty())
    {
        Hls[HlsMsg_Key_PlayUrl]   = Msg.Hls.PlayUrl;
    }
    if (!Msg.Hls.PlayUrlHlss.empty())
    {
        Hls[HlsMsg_Key_PlayUrlHlss]   = Msg.Hls.PlayUrlHlss;
    }
    if (!Msg.Hls.PlayUrlRtmp.empty())
    {
        Hls[HlsMsg_Key_PlayUrlRtmp]   = Msg.Hls.PlayUrlRtmp;
    }
    if (!Msg.Hls.PlayUrlFlv.empty())
    {
        Hls[HlsMsg_Key_PlayUrlFlv]   = Msg.Hls.PlayUrlFlv;
    }
    if (!Msg.Hls.PlayUrlDash.empty())
    {
        Hls[HlsMsg_Key_PlayUrlDash]   = Msg.Hls.PlayUrlDash;
    }

#if defined (_FUNC_LiveStreamming_BackMediaSource)
    Hls[HlsMsg_Key_MediaSource]   = Msg.Hls.MediaSource;
#endif // _FUNC_LiveStreamming_BackMediaSource
    if (!Msg.Hls.Snapshot.empty())
    {
        Hls[HlsMsg_Key_Snapshot]   = Msg.Hls.Snapshot;
    }

    //Hls[HlsMsg_Key_ValidPeriod]   = Msg.Hls.ValidPeriod;
    Hls[HlsMsg_Key_Status]   = Msg.Hls.Status;

    if (!Msg.Hls.Description.empty())
        Hls[HlsMsg_Key_Description]   = Msg.Hls.Description;

    var[HlsMsg_Key_Piece]     = Hls;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgHlsPullTykdStartAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    const Json::Value Hls = m_JsonValue[HlsMsg_Key_Piece];

    Msg.Hls.PlayUrl    = Hls[HlsMsg_Key_PlayUrl]   .asString();
    Msg.Hls.PlayUrlHlss = Hls[HlsMsg_Key_PlayUrlHlss]   .asString();
    Msg.Hls.PlayUrlRtmp = Hls[HlsMsg_Key_PlayUrlRtmp]   .asString();
    Msg.Hls.PlayUrlFlv = Hls[HlsMsg_Key_PlayUrlFlv]   .asString();
    Msg.Hls.PlayUrlDash = Hls[HlsMsg_Key_PlayUrlDash]   .asString();

#if defined (_FUNC_LiveStreamming_BackMediaSource)
    Msg.Hls.MediaSource= Hls[HlsMsg_Key_MediaSource]   .asString();
#endif // _FUNC_LiveStreamming_BackMediaSource
    Msg.Hls.Snapshot = Hls[HlsMsg_Key_Snapshot]   .asString();
    Msg.Hls.ValidPeriod = Hls[HlsMsg_Key_ValidPeriod]   .asString();
    Msg.Hls.StreamID= Hls[HlsMsg_Key_StreamID]   .asString();
    Msg.Hls.Status = Hls[HlsMsg_Key_Status]   .asString();
    Msg.Hls.Description = Hls[HlsMsg_Key_Description]   .asString();

    return true;
}
void CPssStack::Encode(const CPssMsgDeliveryStreamStartAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    Json::Value Delivery;
    Delivery[DeliveryMsg_Key_UrlHls]   = Msg.Delivery.UrlHls;
    Delivery[DeliveryMsg_Key_UrlRtmp]   = Msg.Delivery.UrlRtmp;
#if defined (_FUNC_LiveStreamming_BackMediaSource)
    Delivery[DeliveryMsg_Key_MediaSource]   = Msg.Delivery.MediaSource;
#endif // _FUNC_LiveStreamming_BackMediaSource
    if (!Msg.Delivery.UrlRtsp.empty())
        Delivery[DeliveryMsg_Key_UrlRtsp]   = Msg.Delivery.UrlRtsp;

    if (!Msg.Delivery.Snapshot.empty())
        Delivery[DeliveryMsg_Key_Snapshot]   = Msg.Delivery.Snapshot;

    if (!Msg.Delivery.Description.empty())
        Delivery[DeliveryMsg_Key_Description]   = Msg.Delivery.Description;

    var[DeliveryMsg_Key_Piece]     = Delivery;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgDeliveryStreamStartAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    const Json::Value Delivery = m_JsonValue[DeliveryMsg_Key_Piece];
    Msg.Delivery.UrlHls    = Delivery[DeliveryMsg_Key_UrlHls]   .asString();
    Msg.Delivery.UrlRtmp    = Delivery[DeliveryMsg_Key_UrlRtmp]   .asString();
#if defined (_FUNC_LiveStreamming_BackMediaSource)
    Msg.Delivery.MediaSource= Delivery[DeliveryMsg_Key_MediaSource]   .asString();
#endif // _FUNC_LiveStreamming_BackMediaSource
    Msg.Delivery.UrlRtsp    = Delivery[DeliveryMsg_Key_UrlRtsp]   .asString();
    Msg.Delivery.Snapshot = Delivery[DeliveryMsg_Key_Snapshot]   .asString();
    //Msg.Delivery.ValidPeriod = Delivery[DeliveryMsg_Key_ValidPeriod]   .asString();
    //Msg.Delivery.StreamID= Delivery[DeliveryMsg_Key_StreamID]   .asString();
    Msg.Delivery.Description = Delivery[DeliveryMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgHlsPullTykdStopAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgHlsPullTykdStopAck &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgHlsPullStreamStart             &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Details] = Msg.m_iDetails;

    var[PssMsg_Key_ID] = Msg.ID;
    var[PssMsg_Key_Alias] = Msg.Alias;
    var[PssMsg_Key_Type] = Msg.Type;
    var[PssMsg_Key_ValidPeriod] = Msg.ValidPeriod;

    Json::Value __StandStream;
    __StandStream[StandStreamMsg_Key_Url         ]= Msg.StandStream.Url;
    __StandStream[StandStreamMsg_Key_Account     ]= Msg.StandStream.Account;
    __StandStream[StandStreamMsg_Key_Passwd      ]= Msg.StandStream.Passwd;
    __StandStream[StandStreamMsg_Key_Channel     ]= Msg.StandStream.Channel;
    __StandStream[StandStreamMsg_Key_Subtype     ]= Msg.StandStream.Subtype;
    __StandStream[StandStreamMsg_Key_Location    ]= Msg.StandStream.Location;
    __StandStream[StandStreamMsg_Key_VideoEncode    ]= Msg.StandStream.VideoEncode;
    __StandStream[StandStreamMsg_Key_VideoResize    ]= Msg.StandStream.VideoResize;
    __StandStream[StandStreamMsg_Key_VideoRefps]= Msg.StandStream.VideoRefps;
    __StandStream[StandStreamMsg_Key_AudioEncode    ]= Msg.StandStream.AudioEncode;
    __StandStream[StandStreamMsg_Key_Description ]= Msg.StandStream.Description;
    var[StandStreamMsg_Key_Piece]     = __StandStream;

    Json::Value __Storage;
    __Storage[StandStorageMsg_Key_Type        ]= Msg.Storage.Type;
    __Storage[StandStorageMsg_Key_Name        ]= Msg.Storage.Name;
    //__Storage[StandStorageMsg_Key_Account     ]= Msg.Storage.Account;
    //__Storage[StandStorageMsg_Key_Passwd      ]= Msg.Storage.Passwd;
    __Storage[StandStorageMsg_Key_Channel     ]= Msg.Storage.Channel;
    __Storage[StandStorageMsg_Key_Description ]= Msg.Storage.Description;

    //__Storage[StandStorageMsg_Key_Status ]= Msg.Storage.Status;

    var[StandStorageMsg_Key_Piece]     = __Storage;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgHlsPullStreamStart                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    if (m_JsonValue[PssMsg_Key_Details].isInt())
        Msg.m_iDetails = m_JsonValue[PssMsg_Key_Details].asInt();

    Msg.ID = m_JsonValue[PssMsg_Key_ID].asString();
    Msg.Alias = m_JsonValue[PssMsg_Key_Alias].asString();
    Msg.Type = m_JsonValue[PssMsg_Key_Type].asString();
    Msg.ValidPeriod = m_JsonValue[PssMsg_Key_ValidPeriod].asString();

    const Json::Value __StandStreamPieces = m_JsonValue[StandStreamMsg_Key_Piece];

    Msg.StandStream.Url         = __StandStreamPieces[StandStreamMsg_Key_Url         ].asString();
    Msg.StandStream.Account     = __StandStreamPieces[StandStreamMsg_Key_Account     ].asString();
    Msg.StandStream.Passwd      = __StandStreamPieces[StandStreamMsg_Key_Passwd      ].asString();
    Msg.StandStream.Channel     = __StandStreamPieces[StandStreamMsg_Key_Channel     ].asString();
    Msg.StandStream.Subtype     = __StandStreamPieces[StandStreamMsg_Key_Subtype     ].asString();
    Msg.StandStream.Location    = __StandStreamPieces[StandStreamMsg_Key_Location    ].asString();
    Msg.StandStream.VideoEncode = __StandStreamPieces[StandStreamMsg_Key_VideoEncode ].asString();
    Msg.StandStream.VideoResize = __StandStreamPieces[StandStreamMsg_Key_VideoResize ].asString();
    Msg.StandStream.VideoRefps  = __StandStreamPieces[StandStreamMsg_Key_VideoRefps  ].asString();
    Msg.StandStream.AudioEncode = __StandStreamPieces[StandStreamMsg_Key_AudioEncode ].asString();
    Msg.StandStream.Description = __StandStreamPieces[StandStreamMsg_Key_Description ].asString();

    const Json::Value __StoragePieces = m_JsonValue[StandStorageMsg_Key_Piece];
    Msg.Storage.Type        = __StoragePieces[StandStorageMsg_Key_Type        ].asString();
    Msg.Storage.Name        = __StoragePieces[StandStorageMsg_Key_Name        ].asString();
    //Msg.Storage.Account     = __StoragePieces[StandStorageMsg_Key_Account     ].asString();
    //Msg.Storage.Passwd      = __StoragePieces[StandStorageMsg_Key_Passwd      ].asString();
    Msg.Storage.Channel     = __StoragePieces[StandStorageMsg_Key_Channel     ].asString();
    Msg.Storage.Description = __StoragePieces[StandStorageMsg_Key_Description ].asString();

    //Msg.Storage.Status = __StoragePieces[StandStorageMsg_Key_Status ].asUInt();

    return true;
}
#if 0
// 2018-10-31 17:29:28
void CPssStack::Encode(const CPssMsgPullStreamStart             &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Details] = Msg.m_iDetails;

    var[PssMsg_Key_ID] = Msg.ID;
    var[PssMsg_Key_Type] = Msg.Type;
    var[PssMsg_Key_ValidPeriod] = Msg.ValidPeriod;

    Json::Value __StreamSrc;
    __StreamSrc[StandStreamMsg_Key_Url            ]= Msg.StreamSrc.Url;
    __StreamSrc[StandStreamMsg_Key_Account        ]= Msg.StreamSrc.Account;
    __StreamSrc[StandStreamMsg_Key_Passwd         ]= Msg.StreamSrc.Passwd;
    __StreamSrc[StandStreamMsg_Key_Channel        ]= Msg.StreamSrc.Channel;
    __StreamSrc[StandStreamMsg_Key_Subtype        ]= Msg.StreamSrc.Subtype;
    __StreamSrc[StandStreamMsg_Key_Location       ]= Msg.StreamSrc.Location;
    __StreamSrc[StandStreamMsg_Key_VideoEncode    ]= Msg.StreamSrc.VideoEncode;
    __StreamSrc[StandStreamMsg_Key_VideoResize    ]= Msg.StreamSrc.VideoResize;
    __StreamSrc[StandStreamMsg_Key_VideoRefps     ]= Msg.StreamSrc.VideoRefps;
    __StreamSrc[StandStreamMsg_Key_AudioEncode    ]= Msg.StreamSrc.AudioEncode;
    __StreamSrc[StandStreamMsg_Key_Description    ]= Msg.StreamSrc.Description;
    var[StreamSrcMsg_Key_Piece]     =   __StreamSrc;

    Json::Value __StreamDst;
    __StreamDst[StandStreamMsg_Key_Url            ]= Msg.StreamDst.Url;
    __StreamDst[StandStreamMsg_Key_Account        ]= Msg.StreamDst.Account;
    __StreamDst[StandStreamMsg_Key_Passwd         ]= Msg.StreamDst.Passwd;
    __StreamDst[StandStreamMsg_Key_Channel        ]= Msg.StreamDst.Channel;
    __StreamDst[StandStreamMsg_Key_Subtype        ]= Msg.StreamDst.Subtype;
    __StreamDst[StandStreamMsg_Key_Location       ]= Msg.StreamDst.Location;
    __StreamDst[StandStreamMsg_Key_VideoEncode    ]= Msg.StreamDst.VideoEncode;
    __StreamDst[StandStreamMsg_Key_VideoResize    ]= Msg.StreamDst.VideoResize;
    __StreamDst[StandStreamMsg_Key_VideoRefps     ]= Msg.StreamDst.VideoRefps;
    __StreamDst[StandStreamMsg_Key_AudioEncode    ]= Msg.StreamDst.AudioEncode;
    __StreamDst[StandStreamMsg_Key_Description    ]= Msg.StreamDst.Description;
    var[StreamDstMsg_Key_Piece]     =   __StreamDst;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgPullStreamStart                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_iDetails = m_JsonValue[PssMsg_Key_Details].asInt();

    Msg.ID = m_JsonValue[PssMsg_Key_ID].asString();
    Msg.Type = m_JsonValue[PssMsg_Key_Type].asString();
    Msg.ValidPeriod = m_JsonValue[PssMsg_Key_ValidPeriod].asString();

    const Json::Value __StreamSrcPieces = m_JsonValue[StreamSrcMsg_Key_Piece];
    Msg.StreamSrc.Url         = __StreamSrcPieces[StandStreamMsg_Key_Url         ].asString();
    Msg.StreamSrc.Account     = __StreamSrcPieces[StandStreamMsg_Key_Account     ].asString();
    Msg.StreamSrc.Passwd      = __StreamSrcPieces[StandStreamMsg_Key_Passwd      ].asString();
    Msg.StreamSrc.Channel     = __StreamSrcPieces[StandStreamMsg_Key_Channel     ].asString();
    Msg.StreamSrc.Subtype     = __StreamSrcPieces[StandStreamMsg_Key_Subtype     ].asString();
    Msg.StreamSrc.Location    = __StreamSrcPieces[StandStreamMsg_Key_Location    ].asString();
    Msg.StreamSrc.VideoEncode = __StreamSrcPieces[StandStreamMsg_Key_VideoEncode ].asString();
    Msg.StreamSrc.VideoResize = __StreamSrcPieces[StandStreamMsg_Key_VideoResize ].asString();
    Msg.StreamSrc.VideoRefps  = __StreamSrcPieces[StandStreamMsg_Key_VideoRefps  ].asString();
    Msg.StreamSrc.AudioEncode = __StreamSrcPieces[StandStreamMsg_Key_AudioEncode ].asString();
    Msg.StreamSrc.Description = __StreamSrcPieces[StandStreamMsg_Key_Description ].asString();

    const Json::Value __StreamDstPieces = m_JsonValue[StreamDstMsg_Key_Piece];
    Msg.StreamDst.Url         = __StreamDstPieces[StandStreamMsg_Key_Url         ].asString();
    Msg.StreamDst.Account     = __StreamDstPieces[StandStreamMsg_Key_Account     ].asString();
    Msg.StreamDst.Passwd      = __StreamDstPieces[StandStreamMsg_Key_Passwd      ].asString();
    Msg.StreamDst.Channel     = __StreamDstPieces[StandStreamMsg_Key_Channel     ].asString();
    Msg.StreamDst.Subtype     = __StreamDstPieces[StandStreamMsg_Key_Subtype     ].asString();
    Msg.StreamDst.Location    = __StreamDstPieces[StandStreamMsg_Key_Location    ].asString();
    Msg.StreamDst.VideoEncode = __StreamDstPieces[StandStreamMsg_Key_VideoEncode ].asString();
    Msg.StreamDst.VideoResize = __StreamDstPieces[StandStreamMsg_Key_VideoResize ].asString();
    Msg.StreamDst.VideoRefps  = __StreamDstPieces[StandStreamMsg_Key_VideoRefps  ].asString();
    Msg.StreamDst.AudioEncode = __StreamDstPieces[StandStreamMsg_Key_AudioEncode ].asString();
    Msg.StreamDst.Description = __StreamDstPieces[StandStreamMsg_Key_Description ].asString();

    return true;
}
#endif
void CPssStack::Encode(const CPssMsgRecStreamStart             &Msg)
{
    Json::Value var;

    var[PssMsg_Key_ID] = Msg.ID;
    var[PssMsg_Key_Type] = Msg.Type;

    Json::Value __StandStream;

    __StandStream[StandStreamMsg_Key_Url         ]= Msg.StandStream.Url;
    __StandStream[StandStreamMsg_Key_Account     ]= Msg.StandStream.Account;
    __StandStream[StandStreamMsg_Key_Passwd      ]= Msg.StandStream.Passwd;
    __StandStream[StandStreamMsg_Key_Channel     ]= Msg.StandStream.Channel;
    __StandStream[StandStreamMsg_Key_Subtype     ]= Msg.StandStream.Subtype;
    __StandStream[StandStreamMsg_Key_Location    ]= Msg.StandStream.Location;
    __StandStream[StandStreamMsg_Key_VideoEncode ]= Msg.StandStream.VideoEncode;
    __StandStream[StandStreamMsg_Key_AudioEncode ]= Msg.StandStream.AudioEncode;
    __StandStream[StandStreamMsg_Key_Description ]= Msg.StandStream.Description;

    var[StandStreamMsg_Key_Piece]     = __StandStream;

    Json::Value Rec;
    Rec[RecMsg_Key_ValidPeriod]   = Msg.Rec.ValidPeriod;
    Rec[RecMsg_Key_FilePeriod]   = Msg.Rec.FilePeriod;
    Rec[RecMsg_Key_RecPeriod]   = Msg.Rec.RecPeriod;
    var[RecMsg_Key_Piece]     = Rec;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecStreamStart                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.ID = m_JsonValue[PssMsg_Key_ID].asString();
    Msg.Type = m_JsonValue[PssMsg_Key_Type].asString();

    const Json::Value __StandStreamPieces = m_JsonValue[StandStreamMsg_Key_Piece];

    Msg.StandStream.Url         = __StandStreamPieces[StandStreamMsg_Key_Url         ].asString();
    Msg.StandStream.Account     = __StandStreamPieces[StandStreamMsg_Key_Account     ].asString();
    Msg.StandStream.Passwd      = __StandStreamPieces[StandStreamMsg_Key_Passwd      ].asString();
    Msg.StandStream.Channel     = __StandStreamPieces[StandStreamMsg_Key_Channel     ].asString();
    Msg.StandStream.Subtype     = __StandStreamPieces[StandStreamMsg_Key_Subtype     ].asString();
    Msg.StandStream.Location    = __StandStreamPieces[StandStreamMsg_Key_Location    ].asString();
    Msg.StandStream.VideoEncode = __StandStreamPieces[StandStreamMsg_Key_VideoEncode ].asString();
    Msg.StandStream.AudioEncode = __StandStreamPieces[StandStreamMsg_Key_AudioEncode ].asString();
    Msg.StandStream.Description = __StandStreamPieces[StandStreamMsg_Key_Description ].asString();

    const Json::Value Rec = m_JsonValue[RecMsg_Key_Piece];
    Msg.Rec.ValidPeriod = Rec[RecMsg_Key_ValidPeriod]   .asString();
    Msg.Rec.FilePeriod= Rec[RecMsg_Key_FilePeriod]   .asString();
    Msg.Rec.RecPeriod= Rec[RecMsg_Key_RecPeriod]   .asString();

    return true;
}
void CPssStack::Encode(const CPssMsgRecStreamStartAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[CPssMsgRecStreamStartAck_Key_Status     ] = Msg.Status     ;
    var[CPssMsgRecStreamStartAck_Key_UpStatus   ] = Msg.UpStatus   ;
    var[CPssMsgRecStreamStartAck_Key_UsedSpace  ] = Msg.UsedSpace  ;
    var[CPssMsgRecStreamStartAck_Key_UsedUpSpace] = Msg.UsedUpSpace;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecStreamStartAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.Status      = m_JsonValue[CPssMsgRecStreamStartAck_Key_Status     ].asString();
    Msg.UpStatus    = m_JsonValue[CPssMsgRecStreamStartAck_Key_UpStatus   ].asString();
    Msg.UsedSpace   = m_JsonValue[CPssMsgRecStreamStartAck_Key_UsedSpace  ].asUInt();
    Msg.UsedUpSpace = m_JsonValue[CPssMsgRecStreamStartAck_Key_UsedUpSpace].asUInt();

    return true;
}


void CPssStack::Encode(const CPssMsgRtspPullTykdStart &Msg)
{
    Json::Value var;

    //var[PssMsg_Key_Session] = Msg.m_strSession;
    var[PssMsg_Key_Details] = Msg.m_iDetails;
#if 0
    Json::Value Tykd;
    Tykd[TykdAuthMsg_Key_Account]   = Msg.Tykd.Account   ;
    Tykd[TykdAuthMsg_Key_Password]  = Msg.Tykd.Password   ;
    Tykd[TykdAuthMsg_Key_Session]   = Msg.Tykd.Session   ;
    var[TykdAuthMsg_Key_Piece]     = Tykd;
#endif

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Rtsp;
    //Rtsp[RtspMsg_Key_PlayUrl]   = Msg.Rtsp.PlayUrl;
    //Rtsp[RtspMsg_Key_Snapshot]   = Msg.Rtsp.Snapshot;
    Rtsp[RtspMsg_Key_ValidPeriod]   = Msg.Rtsp.ValidPeriod;
    Rtsp[RtspMsg_Key_StreamID]   = Msg.Rtsp.StreamID;
    //Rtsp[RtspMsg_Key_Description]   = Msg.Rtsp.Description;
    var[RtspMsg_Key_Piece]     = Rtsp;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtspPullTykdStart &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    if (m_JsonValue[PssMsg_Key_Details].isInt())
        Msg.m_iDetails = m_JsonValue[PssMsg_Key_Details].asInt();

#if 0
    const Json::Value TykdPieces = m_JsonValue[TykdAuthMsg_Key_Piece];
    Msg.Tykd.Account    = TykdPieces[TykdAuthMsg_Key_Account]   .asString();
    Msg.Tykd.Password    = TykdPieces[TykdAuthMsg_Key_Password]   .asString();
    Msg.Tykd.Session    = TykdPieces[TykdAuthMsg_Key_Session]   .asString();
#endif

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Rtsp = m_JsonValue[RtspMsg_Key_Piece];
    Msg.Rtsp.PlayUrl    = Rtsp[RtspMsg_Key_PlayUrl]   .asString();
    Msg.Rtsp.Snapshot = Rtsp[RtspMsg_Key_Snapshot]   .asString();
    Msg.Rtsp.ValidPeriod = Rtsp[RtspMsg_Key_ValidPeriod]   .asString();
    Msg.Rtsp.StreamID= Rtsp[RtspMsg_Key_StreamID]   .asString();
    Msg.Rtsp.Description = Rtsp[RtspMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtspPullTykdStop &Msg)
{
    Json::Value var;

    //var[PssMsg_Key_Session] = Msg.m_strSession;
#if 0

    Json::Value Tykd;
    Tykd[TykdAuthMsg_Key_Account]   = Msg.Tykd.Account   ;
    Tykd[TykdAuthMsg_Key_Password]  = Msg.Tykd.Password   ;
    Tykd[TykdAuthMsg_Key_Session]   = Msg.Tykd.Session   ;
    var[TykdAuthMsg_Key_Piece]     = Tykd;
#endif

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    //Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Rtsp;
    //Rtsp[RtspMsg_Key_PlayUrl]   = Msg.Rtsp.PlayUrl;
    //Rtsp[RtspMsg_Key_Snapshot]   = Msg.Rtsp.Snapshot;
    //Rtsp[RtspMsg_Key_ValidPeriod]   = Msg.Rtsp.ValidPeriod;
    Rtsp[RtspMsg_Key_StreamID]   = Msg.Rtsp.StreamID;
    //Rtsp[RtspMsg_Key_Description]   = Msg.Rtsp.Description;
    var[RtspMsg_Key_Piece]     = Rtsp;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtspPullTykdStop &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    //Msg.m_strSession = m_JsonValue[PssMsg_Key_Session].asString();

#if 0

    const Json::Value TykdPieces = m_JsonValue[TykdAuthMsg_Key_Piece];
    Msg.Tykd.Account    = TykdPieces[TykdAuthMsg_Key_Account]   .asString();
    Msg.Tykd.Password    = TykdPieces[TykdAuthMsg_Key_Password]   .asString();
    Msg.Tykd.Session    = TykdPieces[TykdAuthMsg_Key_Session]   .asString();
#endif

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    //Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Rtsp = m_JsonValue[RtspMsg_Key_Piece];
    //Msg.Rtsp.PlayUrl    = Rtsp[RtspMsg_Key_PlayUrl]   .asString();
    //Msg.Rtsp.Snapshot = Rtsp[RtspMsg_Key_Snapshot]   .asString();
    //Msg.Rtsp.ValidPeriod = Rtsp[RtspMsg_Key_ValidPeriod]   .asString();
    Msg.Rtsp.StreamID= Rtsp[RtspMsg_Key_StreamID]   .asString();
    //Msg.Rtsp.Description = Rtsp[RtspMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtspPullTykdStartAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    Json::Value Rtsp;
    if (!Msg.Rtsp.PlayUrl.empty())
        Rtsp[RtspMsg_Key_PlayUrl]   = Msg.Rtsp.PlayUrl;

    if (!Msg.Rtsp.Snapshot.empty())
        Rtsp[RtspMsg_Key_Snapshot]   = Msg.Rtsp.Snapshot;

    //Rtsp[RtspMsg_Key_ValidPeriod]   = Msg.Rtsp.ValidPeriod;
    Rtsp[RtspMsg_Key_Status]   = Msg.Rtsp.Status;
    Rtsp[RtspMsg_Key_Description]   = Msg.Rtsp.Description;
    var[RtspMsg_Key_Piece]     = Rtsp;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtspPullTykdStartAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    const Json::Value Rtsp = m_JsonValue[RtspMsg_Key_Piece];
    Msg.Rtsp.PlayUrl    = Rtsp[RtspMsg_Key_PlayUrl]   .asString();
    Msg.Rtsp.Snapshot = Rtsp[RtspMsg_Key_Snapshot]   .asString();
    Msg.Rtsp.ValidPeriod = Rtsp[RtspMsg_Key_ValidPeriod]   .asString();
    Msg.Rtsp.StreamID= Rtsp[RtspMsg_Key_StreamID]   .asString();
    Msg.Rtsp.Status = Rtsp[RtspMsg_Key_Status]   .asString();
    Msg.Rtsp.Description = Rtsp[RtspMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtspPullTykdStopAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtspPullTykdStopAck &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtmpPullTykdStart &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Details] = Msg.m_iDetails;

#if 0
    Json::Value Tykd;
    Tykd[TykdAuthMsg_Key_Account]   = Msg.Tykd.Account   ;
    Tykd[TykdAuthMsg_Key_Password]  = Msg.Tykd.Password   ;
    Tykd[TykdAuthMsg_Key_Session]   = Msg.Tykd.Session   ;
    var[TykdAuthMsg_Key_Piece]     = Tykd;
#endif

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Rtmp;
    //Rtmp[RtmpMsg_Key_PlayUrl]   = Msg.Rtmp.PlayUrl;
    //Rtmp[RtmpMsg_Key_Snapshot]   = Msg.Rtmp.Snapshot;
    Rtmp[RtmpMsg_Key_ValidPeriod]   = Msg.Rtmp.ValidPeriod;
    Rtmp[RtmpMsg_Key_StreamID]   = Msg.Rtmp.StreamID;
    //Rtmp[RtmpMsg_Key_Description]   = Msg.Rtmp.Description;
    var[RtmpMsg_Key_Piece]     = Rtmp;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtmpPullTykdStart &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    if (m_JsonValue[PssMsg_Key_Details].isInt())
        Msg.m_iDetails = m_JsonValue[PssMsg_Key_Details].asInt();
    //Msg.m_strSession = m_JsonValue[PssMsg_Key_Session].asString();
#if 0
    const Json::Value TykdPieces = m_JsonValue[TykdAuthMsg_Key_Piece];
    Msg.Tykd.Account    = TykdPieces[TykdAuthMsg_Key_Account]   .asString();
    Msg.Tykd.Password    = TykdPieces[TykdAuthMsg_Key_Password]   .asString();
    Msg.Tykd.Session    = TykdPieces[TykdAuthMsg_Key_Session]   .asString();
#endif

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Rtmp = m_JsonValue[RtmpMsg_Key_Piece];
    Msg.Rtmp.PlayUrl    = Rtmp[RtmpMsg_Key_PlayUrl]   .asString();
    Msg.Rtmp.Snapshot = Rtmp[RtmpMsg_Key_Snapshot]   .asString();
    Msg.Rtmp.ValidPeriod = Rtmp[RtmpMsg_Key_ValidPeriod]   .asString();
    Msg.Rtmp.StreamID= Rtmp[RtmpMsg_Key_StreamID]   .asString();
    Msg.Rtmp.Description = Rtmp[RtmpMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtmpPullTykdStop &Msg)
{
    Json::Value var;

    //var[PssMsg_Key_Session] = Msg.m_strSession;
#if 0

    Json::Value Tykd;
    Tykd[TykdAuthMsg_Key_Account]   = Msg.Tykd.Account   ;
    Tykd[TykdAuthMsg_Key_Password]  = Msg.Tykd.Password   ;
    Tykd[TykdAuthMsg_Key_Session]   = Msg.Tykd.Session   ;
    var[TykdAuthMsg_Key_Piece]     = Tykd;
#endif

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    //Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Rtmp;
    //Rtmp[RtmpMsg_Key_PlayUrl]   = Msg.Rtmp.PlayUrl;
    //Rtmp[RtmpMsg_Key_Snapshot]   = Msg.Rtmp.Snapshot;
    //Rtmp[RtmpMsg_Key_ValidPeriod]   = Msg.Rtmp.ValidPeriod;
    Rtmp[RtmpMsg_Key_StreamID]   = Msg.Rtmp.StreamID;
    //Rtmp[RtmpMsg_Key_Description]   = Msg.Rtmp.Description;
    var[RtmpMsg_Key_Piece]     = Rtmp;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtmpPullTykdStop &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    //Msg.m_strSession = m_JsonValue[PssMsg_Key_Session].asString();

#if 0

    const Json::Value TykdPieces = m_JsonValue[TykdAuthMsg_Key_Piece];
    Msg.Tykd.Account    = TykdPieces[TykdAuthMsg_Key_Account]   .asString();
    Msg.Tykd.Password    = TykdPieces[TykdAuthMsg_Key_Password]   .asString();
    Msg.Tykd.Session    = TykdPieces[TykdAuthMsg_Key_Session]   .asString();
#endif

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    //Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Rtmp = m_JsonValue[RtmpMsg_Key_Piece];
    //Msg.Rtmp.PlayUrl    = Rtmp[RtmpMsg_Key_PlayUrl]   .asString();
    //Msg.Rtmp.Snapshot = Rtmp[RtmpMsg_Key_Snapshot]   .asString();
    //Msg.Rtmp.ValidPeriod = Rtmp[RtmpMsg_Key_ValidPeriod]   .asString();
    Msg.Rtmp.StreamID= Rtmp[RtmpMsg_Key_StreamID]   .asString();
    //Msg.Rtmp.Description = Rtmp[RtmpMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtmpPullTykdStartAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    Json::Value Rtmp;

    if (!Msg.Rtmp.PlayUrl.empty())
        Rtmp[RtmpMsg_Key_PlayUrl]   = Msg.Rtmp.PlayUrl;

#if defined (_FUNC_LiveStreamming_BackMediaSource)
    Rtmp[RtmpMsg_Key_MediaSource]   = Msg.Rtmp.MediaSource;
#endif // _FUNC_LiveStreamming_BackMediaSource
    if (!Msg.Rtmp.Snapshot.empty())
    {
        Rtmp[RtmpMsg_Key_Snapshot]   = Msg.Rtmp.Snapshot;
    }

    //Rtmp[RtmpMsg_Key_ValidPeriod]   = Msg.Rtmp.ValidPeriod;
    Rtmp[RtmpMsg_Key_Status]   = Msg.Rtmp.Status;
    Rtmp[RtmpMsg_Key_Description]   = Msg.Rtmp.Description;
    var[RtmpMsg_Key_Piece]     = Rtmp;

    if (Msg.Storage.isValid())
    {
        Json::Value __Storage;
        __Storage[RtmpMsg_Key_PlayUrl]   = Msg.Rtmp.PlayUrl;

        __Storage[StorageInfo_Key_Type        ] = Msg.Storage.Type        ;
        __Storage[StorageInfo_Key_Name        ] = Msg.Storage.Name        ;
        __Storage[StorageInfo_Key_Channel     ] = Msg.Storage.Channel     ;
        __Storage[StorageInfo_Key_Description ] = Msg.Storage.Description ;

        __Storage[StorageInfo_Key_Status      ] = Msg.Storage.Status      ;
        __Storage[StorageInfo_Key_Capacity    ] = Msg.Storage.Capacity    ;
        __Storage[StorageInfo_Key_FreeSpace   ] = Msg.Storage.FreeSpace   ;

        var[StorageInfo_Key_Piece]     = __Storage;
    }

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtmpPullTykdStartAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    const Json::Value Rtmp = m_JsonValue[RtmpMsg_Key_Piece];
    Msg.Rtmp.PlayUrl    = Rtmp[RtmpMsg_Key_PlayUrl]   .asString();
#if defined (_FUNC_LiveStreamming_BackMediaSource)
    Msg.Rtmp.MediaSource    = Rtmp[RtmpMsg_Key_MediaSource]   .asString();
#endif // _FUNC_LiveStreamming_BackMediaSource
    Msg.Rtmp.Snapshot = Rtmp[RtmpMsg_Key_Snapshot]   .asString();
    Msg.Rtmp.ValidPeriod = Rtmp[RtmpMsg_Key_ValidPeriod]   .asString();
    Msg.Rtmp.StreamID= Rtmp[RtmpMsg_Key_StreamID]   .asString();
    Msg.Rtmp.Status= Rtmp[RtmpMsg_Key_Status]   .asString();
    Msg.Rtmp.Description = Rtmp[RtmpMsg_Key_Description]   .asString();

    return true;
}

void CPssStack::Encode(const CPssMsgRtmpPullTykdStopAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRtmpPullTykdStopAck &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}

void CPssStack::Encode(const CPandoraMsg_TykdPullStart             &Msg)
{
    Json::Value var;
    // session
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    //stream
    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    // type
    var[PandoraMsg_Key_Type] = Msg.Type;

    m_JsonValue = var;
    setValid();
}
bool CPssStack::Decode(CPandoraMsg_TykdPullStart                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    Msg.Type = m_JsonValue[PandoraMsg_Key_Type].asInt();

    return true;
}
void CPssStack::Encode(const CPandoraMsg_TykdPullStartAck             &Msg)
{
    Json::Value var;

    // session
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    // result
    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    // playurl
    var[PandoraMsg_Key_hlsPlayUrl] = Msg.hlsPlayUrl;
    var[PandoraMsg_Key_rtmpPlayUrl] = Msg.rtmpPlayUrl;
    var[PandoraMsg_Key_rtspPlayUrl] = Msg.rtspPlayUrl;

    m_JsonValue = var;
    setValid();
}
bool CPssStack::Decode(CPandoraMsg_TykdPullStartAck                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    // result
    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.hlsPlayUrl = m_JsonValue[PandoraMsg_Key_hlsPlayUrl]  .asString();
    Msg.rtmpPlayUrl = m_JsonValue[PandoraMsg_Key_rtmpPlayUrl].asString();
    Msg.rtspPlayUrl = m_JsonValue[PandoraMsg_Key_rtspPlayUrl].asString();

    return true;
}

#if 0
void CPssStack::Encode(const CPandoraMsg_TykdPullStop             &Msg)
{
    Json::Value var;
    // session
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    //stream
    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    // type
    var[PandoraMsg_Key_Type] = Msg.Type;

    m_JsonValue = var;
    setValid();
}
bool CPssStack::Decode(CPandoraMsg_TykdPullStop                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    Msg.Type = m_JsonValue[PandoraMsg_Key_Type].asInt();

    return true;
}
void CPssStack::Encode(const CPandoraMsg_TykdPullStopAck             &Msg)
{
    Json::Value var;

    // session
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    // result
    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    // playurl
    var[PandoraMsg_Key_hlsPlayUrl] = Msg.hlsPlayUrl;
    var[PandoraMsg_Key_rtmpPlayUrl] = Msg.rtmpPlayUrl;
    var[PandoraMsg_Key_rtspPlayUrl] = Msg.rtspPlayUrl;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_TykdPullStopAck                           &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    // result
    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.hlsPlayUrl = m_JsonValue[PandoraMsg_Key_hlsPlayUrl]  .asString();
    Msg.rtmpPlayUrl = m_JsonValue[PandoraMsg_Key_rtmpPlayUrl].asString();
    Msg.rtspPlayUrl = m_JsonValue[PandoraMsg_Key_rtspPlayUrl].asString();

    return true;
}
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CPssStack::Encode(const CPandoraMsg_pssChannelCreate &Msg)
{
    Json::Value var;

    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[pssChannelCreate_Key_Name] = Msg.Name;
    var[pssChannelCreate_Key_Desc] = Msg.Desc;
    var[pssChannelCreate_Key_Type] = Msg.Type;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_pssChannelCreate &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Name = m_JsonValue[pssChannelCreate_Key_Name].asString();
    Msg.Desc = m_JsonValue[pssChannelCreate_Key_Desc].asString();
    Msg.Type   = m_JsonValue[pssChannelCreate_Key_Type].asString();

    return true;
}

void CPssStack::Encode(const CPandoraMsg_pssChannelCreateAck &Msg)
{
    Json::Value var;

    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp] = Msg.PushUrlRtsp;
    var[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp] = Msg.PushUrlRtmp;
    var[PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls ] = Msg.PlayUrlHls ;
    var[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp] = Msg.PlayUrlRtmp;
    var[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp] = Msg.PlayUrlRtsp;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_pssChannelCreateAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }
    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.PushUrlRtsp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp].asString();
    Msg.PushUrlRtmp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp].asString();
    Msg.PlayUrlHls  = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls ].asString();
    Msg.PlayUrlRtmp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp].asString();
    Msg.PlayUrlRtsp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp].asString();

    return true;
}

void CPssStack::Encode(const CPandoraMsg_pssChannelCancel &Msg)
{
    Json::Value var;

    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[pssChannelCreate_Key_Name] = Msg.Name;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_pssChannelCancel &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Name = m_JsonValue[pssChannelCreate_Key_Name].asString();

    return true;
}
void CPssStack::Encode(const CPandoraMsg_pssChannelCancelAck &Msg)
{
    Json::Value var;

    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_pssChannelCancelAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }
    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}


void CPssStack::Encode(const CPandoraMsg_pssChannelQuery &Msg)
{
    Json::Value var;
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[pssChannelQuery_Key_Name] = Msg.Name;

    m_JsonValue = var;
    setValid();
}

void CPssStack::Encode(const CPandoraMsg_pssChannelQueryAck &Msg)
{
    Json::Value var;
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[PandoraMsg_pssChannelQueryAck_Key_PlayUrlHlsStatus] = Msg.PlayUrlHlsStatus;
    var[PandoraMsg_pssChannelQueryAck_Key_PlayUrlRtspStatus] = Msg.PlayUrlRtspStatus;

    var[PandoraMsg_pssChannelQueryAck_Key_AcceptedTimes] = Msg.AcceptedTimes;

    var[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp] = Msg.PushUrlRtsp;
    var[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp] = Msg.PushUrlRtmp;
    var[PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls ] = Msg.PlayUrlHls ;
    var[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp] = Msg.PlayUrlRtmp;
    var[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp] = Msg.PlayUrlRtsp;

    m_JsonValue = var;
    setValid();
}
bool CPssStack::Decode(CPandoraMsg_pssChannelQueryAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.PlayUrlHlsStatus = m_JsonValue[PandoraMsg_pssChannelQueryAck_Key_PlayUrlHlsStatus].asString();
    Msg.PlayUrlRtspStatus = m_JsonValue[PandoraMsg_pssChannelQueryAck_Key_PlayUrlRtspStatus].asString();

    Msg.AcceptedTimes = m_JsonValue[PandoraMsg_pssChannelQueryAck_Key_AcceptedTimes].asInt();

    Msg.PushUrlRtsp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtsp].asString();
    Msg.PushUrlRtmp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PushUrlRtmp].asString();
    Msg.PlayUrlHls  = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PlayUrlHls ].asString();
    Msg.PlayUrlRtmp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtmp].asString();
    Msg.PlayUrlRtsp = m_JsonValue[PandoraMsg_pssChannelCreateAck_Key_PlayUrlRtsp].asString();

    return true;
}

bool CPssStack::Decode(CPandoraMsg_pssChannelQuery &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Name = m_JsonValue[pssChannelQuery_Key_Name].asString();

    return true;
}

void CPssStack::Encode(const CPandoraMsg_pssChannelList &Msg)
{
    Json::Value var;
    var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[pssChannelList_Key_Type] = Msg.Type;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_pssChannelList &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Type   = m_JsonValue[pssChannelList_Key_Type].asString();


    return true;
}

void CPssStack::Encode(const CPandoraMsg_pssChannelListAck &Msg)
{
    Json::Value var;

    //var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[pssChannelListAck_Key_Items] = Msg.Items;
    var[pssChannelListAck_Key_Pages] = Msg.Pages;
    var[pssChannelListAck_Key_TotalItems] = Msg.TotalItems;

    Json::Value pieces_all;//store all pieces
    for (unsigned int ii=0; ii<Msg.m_vChannel.size(); ii++)
    {
        Json::Value pieces;//store all pieces

        pieces[pssChannelListAck_Key_Name]    = Msg.m_vChannel[ii];//.m_strServiceID     ;

        pieces_all.append(pieces);
    }
    var[pssChannelListAck_Key_Channel] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPandoraMsg_pssChannelListAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    //Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.Items = m_JsonValue[pssChannelListAck_Key_Items].asString();
    Msg.Pages = m_JsonValue[pssChannelListAck_Key_Pages].asString();
    Msg.TotalItems = m_JsonValue[pssChannelListAck_Key_TotalItems].asInt();

    const Json::Value arrayOperate = m_JsonValue[pssChannelListAck_Key_Channel];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        std::string Name = arrayOperate[i][pssChannelListAck_Key_Channel]     .asString();

        Msg.m_vChannel.push_back(Name);
    }

    return true;
}

void CPssStack::Encode(const CStreamCapabilityMsg &Msg)
{
    Json::Value var;

    var[StreamCapabilityMsg_Key_BandwidthIn       ] = Msg.BandwidthIn  ;
    var[StreamCapabilityMsg_Key_BandwidthOut      ] = Msg.BandwidthOut ;
    var[StreamCapabilityMsg_Key_CpuMark           ] = Msg.CpuMark      ;
    var[StreamCapabilityMsg_Key_Memory            ] = Msg.Memory       ;
    var[StreamCapabilityMsg_Key_StreamChannelTotal] = Msg.StreamChannelTotal;
    var[StreamCapabilityMsg_Key_StreamChannelUsed ] = Msg.StreamChannelUsed;
    var[StreamCapabilityMsg_Key_StreamChannelUsing] = Msg.StreamChannelUsing;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CStreamCapabilityMsg &Msg)
{
    if(!(m_JsonValue.type() == Json::objectValue))
    {
        return false;
    }

    Msg.BandwidthIn        = m_JsonValue[StreamCapabilityMsg_Key_BandwidthIn       ].asInt();
    Msg.BandwidthOut       = m_JsonValue[StreamCapabilityMsg_Key_BandwidthOut      ].asInt();
    Msg.CpuMark            = m_JsonValue[StreamCapabilityMsg_Key_CpuMark           ].asInt();
    Msg.Memory             = m_JsonValue[StreamCapabilityMsg_Key_Memory            ].asInt();
    Msg.StreamChannelTotal = m_JsonValue[StreamCapabilityMsg_Key_StreamChannelTotal].asInt();
    Msg.StreamChannelUsed  = m_JsonValue[StreamCapabilityMsg_Key_StreamChannelUsed ].asInt();
    Msg.StreamChannelUsing = m_JsonValue[StreamCapabilityMsg_Key_StreamChannelUsing].asInt();

    return true;
}

void CPssStack::Encode(const CPssMsgRecTykdStart &Msg)
{
    Json::Value var;

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_VideoEncode]   = Msg.Stream.VideoEncode;
    Stream[StreamMsg_Key_AudioEncode]   = Msg.Stream.AudioEncode;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    Json::Value Rec;
    Rec[RecMsg_Key_ValidPeriod]   = Msg.Rec.ValidPeriod;
    Rec[RecMsg_Key_FilePeriod]   = Msg.Rec.FilePeriod;
    var[RecMsg_Key_Piece]     = Rec;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdStart &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.VideoEncode = Stream[StreamMsg_Key_VideoEncode ].asString();
    Msg.Stream.AudioEncode = Stream[StreamMsg_Key_AudioEncode ].asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    const Json::Value Rec = m_JsonValue[RecMsg_Key_Piece];
    Msg.Rec.ValidPeriod = Rec[RecMsg_Key_ValidPeriod]   .asString();
    Msg.Rec.FilePeriod= Rec[RecMsg_Key_FilePeriod]   .asString();

    return true;
}
void CPssStack::Encode(const CPssMsgRecTykdStartAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    //var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdStartAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    //Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();


    return true;
}

void CPssStack::Encode(const CPssMsgRecTykdStop &Msg)
{
    Json::Value var;

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdStop &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    return true;
}
void CPssStack::Encode(const CPssMsgRecTykdStopAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    //var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdStopAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    //Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();


    return true;
}

void CPssStack::Encode(const CPssMsgRecTykdDel &Msg)
{
    Json::Value var;

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    //Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    var[RecMsg_Key_StartDt] = Msg.StartDt;
    var[RecMsg_Key_EndDt] = Msg.EndDt;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdDel &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    //Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    Msg.StartDt = m_JsonValue[RecMsg_Key_StartDt].asString();
    Msg.EndDt = m_JsonValue[RecMsg_Key_EndDt].asString();

    return true;
}
void CPssStack::Encode(const CPssMsgRecTykdDelAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    //var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdDelAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    //Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();


    return true;
}

void CPssStack::Encode(const CPssMsgRecTykdQuery &Msg)
{
    Json::Value var;

    Json::Value Stream;
    Stream[StreamMsg_Key_DevID]   = Msg.Stream.DevID;
    Stream[StreamMsg_Key_Channel]   = Msg.Stream.Channel;
    Stream[StreamMsg_Key_Stream]   = Msg.Stream.Stream;
    //Stream[StreamMsg_Key_Description]   = Msg.Stream.Description;
    var[StreamMsg_Key_Piece]     = Stream;

    var[RecMsg_Key_StartDt] = Msg.StartDt;
    var[RecMsg_Key_EndDt] = Msg.EndDt;
    var[RecMsg_Key_Items] = Msg.Items;
    var[RecMsg_Key_Pages] = Msg.Pages;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdQuery &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    const Json::Value Stream = m_JsonValue[StreamMsg_Key_Piece];
    Msg.Stream.DevID    = Stream[StreamMsg_Key_DevID]   .asString();
    Msg.Stream.Channel= Stream[StreamMsg_Key_Channel]   .asString();
    Msg.Stream.Stream= Stream[StreamMsg_Key_Stream]   .asString();
    //Msg.Stream.Description= Stream[StreamMsg_Key_Description]   .asString();

    Msg.StartDt = m_JsonValue[RecMsg_Key_StartDt].asString();
    Msg.EndDt = m_JsonValue[RecMsg_Key_EndDt].asString();
    Msg.Items = m_JsonValue[RecMsg_Key_Items].asString();
    Msg.Pages = m_JsonValue[RecMsg_Key_Pages].asString();

    return true;
}
void CPssStack::Encode(CPssMsgRecTykdQueryAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[RecMsg_Key_Items] = Msg.Items;
    var[RecMsg_Key_Pages] = Msg.Pages;
    var[RecMsg_Key_TotalItems] = Msg.TotalItems;
    //var[PandoraMsg_Key_MsgSession] = Msg.m_strMsgSession;

    Json::Value pieces_all;//store all pieces
    std::list<CFileMsg>::iterator it = Msg.Files.begin();
    for ( ; it != Msg.Files.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces[FileMsg_Key_Url]    = it->Url;
        pieces[FileMsg_Key_SnapUrl]    = it->SnapUrl;
        if (!it->MotionUrl.empty())
            pieces[FileMsg_Key_MotionUrl]    = it->MotionUrl;
        pieces[FileMsg_Key_Size]    = it->Size;
        pieces[FileMsg_Key_StartDt]    = it->StartDt;
        pieces[FileMsg_Key_EndDt]    = it->EndDt;

        pieces_all.append(pieces);
    }
    var[FileMsg_Key_Piece] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgRecTykdQueryAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.Items = m_JsonValue[RecMsg_Key_Items].asString();
    Msg.Pages = m_JsonValue[RecMsg_Key_Pages].asString();
    Msg.TotalItems = m_JsonValue[RecMsg_Key_TotalItems].asInt();

    //Msg.m_strMsgSession = m_JsonValue[PandoraMsg_Key_MsgSession].asString();

    const Json::Value arrayOperate = m_JsonValue[FileMsg_Key_Piece];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CFileMsg __file;
        __file.Url     = arrayOperate[i][FileMsg_Key_Url]     .asString();
        __file.SnapUrl     = arrayOperate[i][FileMsg_Key_SnapUrl]     .asString();
        __file.MotionUrl     = arrayOperate[i][FileMsg_Key_MotionUrl]     .asString();
        __file.Size    = arrayOperate[i][FileMsg_Key_Size]     .asString();
        __file.StartDt = arrayOperate[i][FileMsg_Key_StartDt]     .asString();
        __file.EndDt   = arrayOperate[i][FileMsg_Key_EndDt]     .asString();

        Msg.Files.push_back(__file);
    }

    return true;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CPssStack::Encode(const CPssMsgNvpAccouontCreate &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_MP      ]  = Msg.MP;
    var[NvpMsg_Key_Name    ]  = Msg.Name;
    var[NvpMsg_Key_Nick    ]  = Msg.Nick;
    var[NvpMsg_Key_Sex     ]  = Msg.Sex;
    var[NvpMsg_Key_Birthday]  = Msg.Birthday;
    var[NvpMsg_Key_Add     ]  = Msg.Add;


    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpAccouontCreate &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.MP       = m_JsonValue[NvpMsg_Key_MP      ].asString();
    Msg.Name     = m_JsonValue[NvpMsg_Key_Name    ].asString();
    Msg.Nick     = m_JsonValue[NvpMsg_Key_Nick    ].asString();
    Msg.Sex      = m_JsonValue[NvpMsg_Key_Sex     ].asString();
    Msg.Birthday = m_JsonValue[NvpMsg_Key_Birthday].asString();
    Msg.Add      = m_JsonValue[NvpMsg_Key_Add     ].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgNvpAccouontCreateAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpAccouontCreateAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}
void CPssStack::Encode(const CPssMsgNvpAccouontMod &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_LoginName      ]  = Msg.LoginName;
    var[NvpMsg_Key_Password    ]  = Msg.Password;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpAccouontMod &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.LoginName       = m_JsonValue[NvpMsg_Key_LoginName      ].asString();
    Msg.Password     = m_JsonValue[NvpMsg_Key_Password    ].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgNvpShareCam2WeChat &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_DevID  ]  = Msg.DevID  ;
    var[NvpMsg_Key_Channel]  = Msg.Channel;
    var[NvpMsg_Key_Stream ]  = Msg.Stream ;
    var[NvpMsg_Key_Title  ]  = Msg.Title  ;
    var[NvpMsg_Key_Memo   ]  = Msg.Memo   ;
    var[NvpMsg_Key_Addr   ]  = Msg.Addr   ;
    var[NvpMsg_Key_EndDt   ]  = Msg.EndDt   ;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpShareCam2WeChat &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.DevID   = m_JsonValue[NvpMsg_Key_DevID  ].asString();
    Msg.Channel = m_JsonValue[NvpMsg_Key_Channel].asString();
    Msg.Stream  = m_JsonValue[NvpMsg_Key_Stream ].asString();
    Msg.Title   = m_JsonValue[NvpMsg_Key_Title  ].asString();
    Msg.Memo    = m_JsonValue[NvpMsg_Key_Memo   ].asString();
    Msg.Addr    = m_JsonValue[NvpMsg_Key_Addr   ].asString();
    Msg.EndDt    = m_JsonValue[NvpMsg_Key_EndDt   ].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgNvpShareCam2WeChatAck &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_UrlHlsPage] = Msg.UrlHlsPage;
    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpShareCam2WeChatAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.UrlHlsPage = m_JsonValue[NvpMsg_Key_UrlHlsPage].asString();
    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgNvpShareCam2WeChatQueryAck &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_Title  ]  = Msg.Title  ;
    var[NvpMsg_Key_Memo   ]  = Msg.Memo   ;
    var[NvpMsg_Key_Addr   ]  = Msg.Addr   ;
    var[NvpMsg_Key_StartDt   ]  = Msg.StartDt   ;
    var[NvpMsg_Key_EndDt   ]  = Msg.EndDt   ;

    var[NvpMsg_Key_UrlHlsPage] = Msg.UrlHlsPage;
    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpShareCam2WeChatQueryAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Title   = m_JsonValue[NvpMsg_Key_Title  ].asString();
    Msg.Memo    = m_JsonValue[NvpMsg_Key_Memo   ].asString();
    Msg.Addr    = m_JsonValue[NvpMsg_Key_Addr   ].asString();
    Msg.StartDt    = m_JsonValue[NvpMsg_Key_StartDt   ].asString();
    Msg.EndDt    = m_JsonValue[NvpMsg_Key_EndDt   ].asString();

    Msg.UrlHlsPage = m_JsonValue[NvpMsg_Key_UrlHlsPage].asString();
    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    return true;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CPssStack::Encode(const CPssMsgNvpRegister &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_Sequence  ] = Msg.Sequence  ;
    var[NvpMsg_Key_Type      ] = Msg.Type      ;
    var[NvpMsg_Key_Vendor    ] = Msg.Vendor    ;
    var[NvpMsg_Key_Module    ] = Msg.Module    ;
    var[NvpMsg_Key_DevID     ] = Msg.DevID     ;
    var[NvpMsg_Key_DevKey    ] = Msg.DevKey    ;
    var[NvpMsg_Key_AppVer] = Msg.AppVer;
    var[NvpMsg_Key_Addr      ] = Msg.Addr      ;
    var[NvpMsg_Key_LanMac    ] = Msg.LanMac    ;
    var[NvpMsg_Key_LanIP     ] = Msg.LanIP     ;
    var[NvpMsg_Key_WLanMac   ] = Msg.WLanMac   ;
    var[NvpMsg_Key_WLanIP    ] = Msg.WLanIP    ;
    var[NvpMsg_Key_DataUrl   ] = Msg.DataUrl   ;
    var[NvpMsg_Key_CommUrl   ] = Msg.CommUrl   ;
    var[NvpMsg_Key_WebUrl    ] = Msg.WebUrl    ;
    var[NvpMsg_Key_DataUrlTu ] = Msg.DataUrlTu ;
    var[NvpMsg_Key_CommUrlTu ] = Msg.CommUrlTu ;
    var[NvpMsg_Key_WebUrlTu  ] = Msg.WebUrlTu  ;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpRegister &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Sequence  = m_JsonValue[NvpMsg_Key_Sequence  ].asString();
    Msg.Type      = m_JsonValue[NvpMsg_Key_Type      ].asString();
    Msg.Vendor    = m_JsonValue[NvpMsg_Key_Vendor    ].asString();
    Msg.Module    = m_JsonValue[NvpMsg_Key_Module    ].asString();
    Msg.DevID     = m_JsonValue[NvpMsg_Key_DevID     ].asString();
    Msg.DevKey    = m_JsonValue[NvpMsg_Key_DevKey    ].asString();
    Msg.AppVer    = m_JsonValue[NvpMsg_Key_AppVer].asString();
    Msg.Addr      = m_JsonValue[NvpMsg_Key_Addr      ].asString();
    Msg.LanMac    = m_JsonValue[NvpMsg_Key_LanMac    ].asString();
    Msg.LanIP     = m_JsonValue[NvpMsg_Key_LanIP     ].asString();
    Msg.WLanMac   = m_JsonValue[NvpMsg_Key_WLanMac   ].asString();
    Msg.WLanIP    = m_JsonValue[NvpMsg_Key_WLanIP    ].asString();
    Msg.DataUrl   = m_JsonValue[NvpMsg_Key_DataUrl   ].asString();
    Msg.CommUrl   = m_JsonValue[NvpMsg_Key_CommUrl   ].asString();
    Msg.WebUrl    = m_JsonValue[NvpMsg_Key_WebUrl    ].asString();
    Msg.DataUrlTu = m_JsonValue[NvpMsg_Key_DataUrlTu ].asString();
    Msg.CommUrlTu = m_JsonValue[NvpMsg_Key_CommUrlTu ].asString();
    Msg.WebUrlTu  = m_JsonValue[NvpMsg_Key_WebUrlTu  ].asString();

    return true;
}

void CPssStack::Encode(CPssMsgNvpRegisterAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    if(Msg.UrlDataSrv.length()>0)
        var[NvpMsg_Key_UrlDataSrv] = Msg.UrlDataSrv;

    if(Msg.UrlCommSrv.length()>0)
        var[NvpMsg_Key_UrlCommSrv] = Msg.UrlCommSrv;

    if(Msg.UrlStorSrv.length()>0)
        var[NvpMsg_Key_UrlStorSrv] = Msg.UrlStorSrv;
    if(Msg.UrlEventsSrv.length()>0)
        var[NvpMsg_Key_UrlEventsSrv] = Msg.UrlEventsSrv;
    if(Msg.UrlAppSrv.length()>0)
        var[NvpMsg_Key_UrlAppSrv] = Msg.UrlAppSrv;

    if (Msg.AdsFiles.size()>0)
    {
        Json::Value pieces_all;//store all pieces
        std::list<CAdsFileMsg>::iterator it = Msg.AdsFiles.begin();
        for ( ; it != Msg.AdsFiles.end() ; it++)
        {
            Json::Value pieces;//store all pieces

            pieces[AdsFileMsg_Key_No     ] = it->No      ;
            pieces[AdsFileMsg_Key_Url    ] = it->Url     ;
            pieces[AdsFileMsg_Key_Sha1   ] = it->Sha1    ;
            pieces[AdsFileMsg_Key_Type   ] = it->Type    ;
            pieces[AdsFileMsg_Key_Length ] = it->Length  ;
            pieces[AdsFileMsg_Key_Delay  ] = it->Delay   ;
            pieces[AdsFileMsg_Key_StartDt] = it->StartDt ;
            pieces[AdsFileMsg_Key_EndDt  ] = it->EndDt   ;

            pieces_all.append(pieces);
        }
        var[AdsFileMsg_Key_Ads] = pieces_all;
    }

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpRegisterAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.UrlDataSrv = m_JsonValue[NvpMsg_Key_UrlDataSrv].asString();
    Msg.UrlCommSrv = m_JsonValue[NvpMsg_Key_UrlCommSrv].asString();

    Msg.UrlStorSrv = m_JsonValue[NvpMsg_Key_UrlStorSrv].asString();
    Msg.UrlEventsSrv = m_JsonValue[NvpMsg_Key_UrlEventsSrv].asString();
    Msg.UrlAppSrv = m_JsonValue[NvpMsg_Key_UrlAppSrv].asString();

    const Json::Value arrayOperate = m_JsonValue[AdsFileMsg_Key_Ads];
    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CAdsFileMsg __file;

        __file.No      = arrayOperate[i][AdsFileMsg_Key_No     ].asString();
        __file.Url     = arrayOperate[i][AdsFileMsg_Key_Url    ].asString();
        __file.Sha1    = arrayOperate[i][AdsFileMsg_Key_Sha1   ].asString();
        __file.Type    = arrayOperate[i][AdsFileMsg_Key_Type   ].asString();
        __file.Length  = arrayOperate[i][AdsFileMsg_Key_Length ].asString();
        __file.Delay   = arrayOperate[i][AdsFileMsg_Key_Delay  ].asString();
        __file.StartDt = arrayOperate[i][AdsFileMsg_Key_StartDt].asString();
        __file.EndDt   = arrayOperate[i][AdsFileMsg_Key_EndDt  ].asString();

        Msg.AdsFiles.push_back(__file);
    }

    return true;
}
void CPssStack::Encode(const CPssMsgNvpTermInfo &Msg)
{
    Json::Value var;

    var[NvpMsg_Key_DevID      ]  = Msg.DevID;
    var[NvpMsg_Key_DevKey ]  = Msg.DevKey;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpTermInfo &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.DevID = m_JsonValue[NvpMsg_Key_DevID].asString();
    Msg.DevKey = m_JsonValue[NvpMsg_Key_DevKey].asString();

    return true;
}

void CPssStack::Encode(const CPssMsgNvpTermInfoAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;

    var[NvpMsg_Key_DevOnline] = Msg.Online;
    var[NvpMsg_Key_DevBinded] = Msg.Binded;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgNvpTermInfoAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result = m_JsonValue[PssMsg_Key_Result].asString();
    Msg.Reason = m_JsonValue[PssMsg_Key_ResultMsg].asString();

    Msg.Online= m_JsonValue[NvpMsg_Key_DevOnline].asString();
    Msg.Binded = m_JsonValue[NvpMsg_Key_DevBinded].asString();

    return true;
}

void CPssStack::Encode(CPssMsgEventsPub &Msg)
{
    Json::Value var;

    var[EventsMsg_Key_DevID] = Msg.DevID;

    Json::Value pieces_all;//store all pieces
    std::list<CEventMsg>::iterator it = Msg.Event.begin();
    for ( ; it != Msg.Event.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [EventMsg_Key_Type       ] = it->Type        ;
        pieces [EventMsg_Key_Data       ] = it->Data        ;
        pieces [EventMsg_Key_StartDt    ] = it->StartDt     ;
        pieces [EventMsg_Key_EndDt      ] = it->EndDt       ;
        pieces [EventMsg_Key_Description] = it->Description ;

        pieces_all.append(pieces);
    }
    var[EventMsg_Key_Piece] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsgEventsPub &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.DevID = m_JsonValue[EventsMsg_Key_DevID].asString();

    const Json::Value arrayOperate = m_JsonValue[FileMsg_Key_Piece];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CEventMsg __event;

        __event.Type        = arrayOperate[i] [EventMsg_Key_Type       ].asString();
        __event.Data        = arrayOperate[i] [EventMsg_Key_Data       ].asString();
        __event.StartDt     = arrayOperate[i] [EventMsg_Key_StartDt    ].asString();
        __event.EndDt       = arrayOperate[i] [EventMsg_Key_EndDt      ].asString();
        __event.Description = arrayOperate[i] [EventMsg_Key_Description].asString();

        Msg.Event.push_back(__event);
    }

    return true;
}

void CPssStack::Encode(const CPssMsg_requestMsgDeviceList &Msg)
{
    Json::Value var;

    var[PssMsg_Key_GroupID ] = Msg.m_strGroupID ;
    var[PssMsg_Key_PageSize] = Msg.m_iPageSize;
    var[PssMsg_Key_Pages   ] = Msg.m_iPages   ;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsg_requestMsgDeviceList &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strGroupID  = m_JsonValue[PssMsg_Key_GroupID ].asString();
    Msg.m_iPageSize = m_JsonValue[PssMsg_Key_PageSize].asInt();
    Msg.m_iPages    = m_JsonValue[PssMsg_Key_Pages   ].asInt();

    return true;
}

void CPssStack::Encode(const CPssMsg_requestMsgDeviceListAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result] = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var[PssMsg_Key_GroupID   ] = Msg.m_strGroupID   ;
    var[PssMsg_Key_TotalCount] = Msg.m_iTotalCount;
    var[PssMsg_Key_Pages     ] = Msg.m_iPages     ;

    Json::Value pieces_all;//store all pieces
    std::list<CDeviceMsg>::const_iterator it = Msg.Devices.begin();
    for ( ; it != Msg.Devices.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [PssMsg_Key_devId] = it->m_strDevID ;
        pieces [PssMsg_Key_Type    ] = it->m_strCode  ;
        pieces [UniMsg_Key_Version ] = it->m_strVersion;
        pieces [PssMsg_Key_Status  ] = it->m_strStatus;
        pieces [PssMsg_Key_LastOnTime] = it->m_strLastOnTime;
        pieces [PssMsg_Key_Name] = it->m_strName;
        pieces [PssMsg_Key_Vendor] = it->m_strVendor;
        pieces [PssMsg_Key_Location] = it->m_strLocation;
        pieces [UniMsg_Key_Description] = it->m_strDescription;

        pieces [PssMsg_Key_ChannelNum] = it->m_strChannelNum;
        pieces [PssMsg_Key_HostIp    ] = it->m_strHostIp    ;
        pieces [PssMsg_Key_HttpPort  ] = it->m_strHttpPort  ;
        pieces [PssMsg_Key_RtspPort  ] = it->m_strRtspPort  ;
        pieces [PssMsg_Key_TcpPort   ] = it->m_strTcpPort   ;

        pieces_all.append(pieces);
    }
    var[PssMsg_Key_Devices] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CPssMsg_requestMsgDeviceListAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result        = m_JsonValue[PssMsg_Key_Result   ].asString();
    Msg.Reason        = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.m_strGroupID  = m_JsonValue[PssMsg_Key_GroupID   ].asString();
    Msg.m_iTotalCount = m_JsonValue[PssMsg_Key_TotalCount].asInt();
    Msg.m_iPages      = m_JsonValue[PssMsg_Key_Pages     ].asInt();

    const Json::Value arrayOperate = m_JsonValue[PssMsg_Key_Devices];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CDeviceMsg __Devices;

        __Devices.m_strDevID        = arrayOperate[i] [PssMsg_Key_devId       ].asString();
        __Devices.m_strCode         = arrayOperate[i] [PssMsg_Key_Type        ].asString();
        __Devices.m_strVersion = arrayOperate[i] [UniMsg_Key_Version        ].asString();
        __Devices.m_strStatus       = arrayOperate[i] [PssMsg_Key_Status      ].asString();
        __Devices.m_strLastOnTime   = arrayOperate[i] [PssMsg_Key_LastOnTime  ].asString();
        __Devices.m_strName         = arrayOperate[i] [PssMsg_Key_Name        ].asString();
        __Devices.m_strVendor       = arrayOperate[i] [PssMsg_Key_Vendor      ].asString();
        __Devices.m_strLocation     = arrayOperate[i] [PssMsg_Key_Location    ].asString();
        __Devices.m_strDescription  = arrayOperate[i] [UniMsg_Key_Description].asString();

        __Devices.m_strChannelNum  = arrayOperate[i] [PssMsg_Key_ChannelNum].asString();
        __Devices.m_strHostIp      = arrayOperate[i] [PssMsg_Key_HostIp    ].asString();
        __Devices.m_strHttpPort    = arrayOperate[i] [PssMsg_Key_HttpPort  ].asString();
        __Devices.m_strRtspPort    = arrayOperate[i] [PssMsg_Key_RtspPort  ].asString();
        __Devices.m_strTcpPort     = arrayOperate[i] [PssMsg_Key_TcpPort   ].asString();

        Msg.Devices.push_back(__Devices);
    }

    return true;
}

void CPssStack::Encode(const CUniMsg_requestMsgOption &Msg)
{
    Json::Value var;

    var[UniMsg_Key_Agent ] = Msg.m_strAgent ;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CUniMsg_requestMsgOption &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.m_strAgent  = m_JsonValue[UniMsg_Key_Agent ].asString();

    return true;
}

void CPssStack::Encode(const CUniMsg_requestMsgOptionAck &Msg)
{
    Json::Value var;

    var[PssMsg_Key_Result]    = Msg.Result;
    var[PssMsg_Key_ResultMsg] = Msg.Reason;
    var[UniMsg_Key_Server   ] = Msg.m_strServer   ;
    var[UniMsg_Key_Huid   ] = Msg.m_strHuid;

    Json::Value pieces_all;//store all pieces
    std::list<CFirmMsg>::const_iterator it = Msg.Firms.begin();

    for ( ; it != Msg.Firms.end() ; it++)
    {
        Json::Value pieces;//store all pieces

        pieces [UniMsg_Key_FirmDesc  ] = it->m_strDesc ;
        pieces [UniMsg_Key_FirmCode  ] = it->m_strCode  ;

        pieces_all.append(pieces);
    }
    var[UniMsg_Key_Firm] = pieces_all;

    m_JsonValue = var;
    setValid();
}

bool CPssStack::Decode(CUniMsg_requestMsgOptionAck &Msg)
{
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return false;
    }

    Msg.Result        = m_JsonValue[PssMsg_Key_Result   ].asString();
    Msg.Reason        = m_JsonValue[PssMsg_Key_ResultMsg].asString();
    Msg.m_strServer   = m_JsonValue[UniMsg_Key_Server   ].asString();
    Msg.m_strHuid = m_JsonValue[UniMsg_Key_Huid ].asString();

    const Json::Value arrayOperate = m_JsonValue[UniMsg_Key_Firm];

    for (size_t i = 0; i < arrayOperate.size(); i++)
    {
        CFirmMsg __Firm;

        __Firm.m_strDesc        = arrayOperate[i] [UniMsg_Key_FirmDesc       ].asString();
        __Firm.m_strCode        = arrayOperate[i] [UniMsg_Key_FirmCode       ].asString();

        Msg.Firms.push_back(__Firm);
    }

    return true;
}
