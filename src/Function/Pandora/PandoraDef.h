/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PandoraDef.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PandoraDef.h 5884 2012-09-06 03:34:17Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-06 03:34:17  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _PandoraDef_H
#define _PandoraDef_H

#define def_Pandora_MatrixHost "vsa.pq.ezlibs.com"
#define def_Pandora_MatrixPort 18900 // 节点接入接口
#define def_Pandora_MatrixService 18089 // 云服务接口

// 转发
#define def_Pandora_StreammingServer_Port 18088 
#define def_Pandora_StreammingServer_ServerName "com.ezlibs.Pss.LiveStreamming"
// 直播
#define def_Pandora_LiveCastServer_Port 18089 
#define def_Pandora_LiveCastServer_ServerName "com.ezlibs.Pss.LiveCast"
// 录像服务接口
#define def_Pandora_LiveRecordServer_Port 18090 
#define def_Pandora_LiveRecordServer_ServerName "com.ezlibs.Pss.LiveRecord "
// 可视通话
#define def_Pandora_LiveDialogServer_Port 18091 
#define def_Pandora_LiveDialogServer_ServerName "com.ezlibs.Pss.LiveDialog"
// 设备注册
#define def_Pandora_NvpRegisterServer_Port 18098 
#define def_Pandora_NvpRegisterServer_ServerName "com.ezlibs.nvp.RegisterServer"

#define def_Pandora_ConnTimeout 60

#define def_Pandora_MaxMsgLen (1024*10)

#define def_MyServiceStack_Of_Pandora CPssStack

#define MIN_LENGTH_OF_JSON_STRING 4//// strlen("[{}]")
#define MIN_LENGTH_OF_HLS_URL 14//// strlen("http:///a.m3u8")
#define MIN_LENGTH_OF_RTMP_URL 10//// strlen("rtmp://A/1")
#define MIN_LENGTH_OF_RTSP_URL 10//// strlen("rtsp://A/1")

#define def_Default_Node_Name  "PN"//// strlen("[{}]")
#define def_Min_Node_Name_Length strlen(def_Default_Node_Name)+1 // 让默认的失效
#define def_Max_Node_Name_Length 128 

// 加密id
#define EncryptID_Encrypt 1
#define EncryptID_Do_Not_Encrypt 0

// 默认自动停止时长
// 目前服务器不是特别紧张的情况下， 定为1.5天
// 实时视频
#define ValidPeriod_DEF_Auto_LiveSecond 3600*24*1.5

#define ValidPeriod_SECOND_MIN 30 // 生存期,单位秒（>=10）
#define ValidPeriod_DEF_Auto 0 // 0 - 自动，自行判断停止时刻
#define ValidPeriod_DEF_str_Auto "0" // 0 - 自动，自行判断停止时刻
#define ValidPeriod_DEF_LongLife 1 // 1 - 不失效
#define ValidPeriod_DEF_str_LongLife "1" // 1 - 不失效

// 历史视频
#define def_MinFileValidPeriod 600 // second
#define def_MaxFileValidPeriod 157680000 // second, 94608000 = 5 years, =3600*24*365*5
#define def_MinFileSegTime 300 // second
#define def_MaxFileSegTime 2*3600 // second
#define def_MinRecValidPeriod 300 // second

// shp over tcp
#define def_Service_Protocol_Shp "shp"
#define def_Shp_Tcp_Port_Query_Mark "p="

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//req by node
//base
#define def_Pandora_requestAuth      "/pandora/auth"
#define def_Pandora_requestHeartbeat "/pandora/heartbeat"

//
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_Pandora_Req_pssOption "/pss/option"
#define def_Pandora_Match_pssChannelCreate "/pss/channel/create"
#define def_Pandora_Match_pssChannelCancel "/pss/channel/cancel"
#define def_Pandora_Match_pssChannelQuery "/pss/channel/query"
#define def_Pandora_Match_pssChannelList "/pss/channel/list"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// for tykd, same as, req by other sys
#define def_Pandora_Req_hlsPullTykdStart "/hls/pull/tykd/start"
#define def_Pandora_Req_hlsPullTykdStop "/hls/pull/tykd/stop"

#define def_Pandora_Req_rtmpPullTykdStart "/rtmp/pull/tykd/start"
#define def_Pandora_Req_rtmpPullTykdStop "/rtmp/pull/tykd/stop"

#define def_Pandora_Req_rtspPullTykdStart "/rtsp/pull/tykd/start"
#define def_Pandora_Req_rtspPullTykdStop "/rtsp/pull/tykd/stop"
#if 0
#define def_Pandora_MatchReq_hlsPullTykdStart def_Pandora_Match_hlsPullTykdStart
#define def_Pandora_MatchReq_hlsPullTykdStop def_Pandora_Match_hlsPullTykdStop
#define def_Pandora_MatchReq_rtmpPullTykdStart def_Pandora_Match_rtmpPullTykdStart
#define def_Pandora_MatchReq_rtmpPullTykdStop def_Pandora_Match_rtmpPullTykdStop
#define def_Pandora_MatchReq_rtspPullTykdStart def_Pandora_Match_rtspPullTykdStart
#define def_Pandora_MatchReq_rtspPullTykdStop def_Pandora_Match_rtspPullTykdStop
#endif
//#define def_Pandora_Req_PullTykdStart "/pandora/pull/tykd/start"
//#define def_Pandora_Req_PullTykdStop "/pandora/pull/tykd/stop"
//9 媒体转码通信接口
#define def_Pandora_Req_hlsPullStreamStart "/hls/pull/stream/start"
#define def_Pandora_Req_hlsPullStreamStop "/hls/pull/stream/stop"
#define def_Pandora_Req_hlsPullStreamList "/hls/pull/stream/list"
#define def_Pandora_Req_rtmpPullStreamStart "/rtmp/pull/stream/start"
#define def_Pandora_Req_rtmpPullStreamStop "/rtmp/pull/stream/stop"
#define def_Pandora_Req_rtmpPullStreamList "/rtmp/pull/stream/list"
#define def_Pandora_Req_flvPullStreamStart "/flv/pull/stream/start"
#define def_Pandora_Req_flvPullStreamStop "/flv/pull/stream/stop"
#define def_Pandora_Req_flvPullStreamList "/flv/pull/stream/list"
#define def_Pandora_Req_dashPullStreamStart "/dash/pull/stream/start"
#define def_Pandora_Req_dashPullStreamStop "/dash/pull/stream/stop"
#define def_Pandora_Req_dashPullStreamList "/dash/pull/stream/list"
#define def_Pandora_Req_rtspPullStreamStart "/rtsp/pull/stream/start"
#define def_Pandora_Req_rtspPullStreamStop "/rtsp/pull/stream/stop"
//#define def_Pandora_Req_rtspPullStreamList "/rtsp/pull/stream/list"
//#define def_Pandora_Req_rtpPullStreamStart "/rtp/pull/stream/start"
//#define def_Pandora_Req_rtpPullStreamStop "/rtp/pull/stream/stop"

//#define def_Pandora_Req_deliveryStreamStart "/delivery/stream/start"
//#define def_Pandora_Req_deliveryStreamStop "/delivery/stream/stop"
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_Pandora_Req_LiveRecordTykdStart "/rec/tykd/start"
#define def_Pandora_Req_LiveRecordTykdStop "/rec/tykd/stop"
#define def_Pandora_Req_LiveRecordTykdDel "/rec/tykd/del"
#define def_Pandora_Req_LiveRecordTykdQuery "/rec/tykd/query"

#define def_Pandora_Req_LiveRecordStreamStart "/rec/stream/start"
#define def_Pandora_Req_LiveRecordStreamStop "/rec/stream/stop"
#define def_Pandora_Req_LiveRecordStreamDel "/rec/stream/del"
#define def_Pandora_Req_LiveRecordStreamQuery "/rec/stream/query"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// by 
#define def_Pandora_Req_NvpAccountCreate "/nvp/account/create"
#define def_Pandora_Req_NvpAccountMod "/nvp/account/mod"
// by CHttpServerPusherTykdServer
//#define def_Pandora_Req_NvpShareCam2WeChat "/nvp/cam/share"
//#define def_Pandora_Req_NvpShareCam2WeChatStop "/nvp/cam/share/stop"
//#define def_Pandora_Req_NvpShareCam2WeChatQuery "/nvp/cam/share/query"
// by 
#define def_Pandora_Req_NvpTRegister "/DeviceRegisterServlet"
#define def_Pandora_Req_NvpTermRegister "/nvp/term/register"
#define def_Pandora_Req_NvpTermInfo "/nvp/term/info"

//#define def_Pandora_Req_NvpTermAdResource "/nvp/term/adresource"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// by 
#define def_Pandora_Req_Hqtt1_Publish "/pq/pub"// 第一版本

// Req, Matrix 发往node
#define def_Pandora_MaReq_Hqtt1_Publish "/Ma2Node/pq/pub"// 第一版本

#endif // _PandoraDef_H
