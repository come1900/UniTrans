/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DefUniService.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DefUniService.h 5884 2018-03-09 05:57:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-09 05:57:47  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _DefUniService_H
#define _DefUniService_H

#define SERVICE_PROTOCOL_SERVER_Pandora "ss_Pandora_http" //
#define SERVICE_PROTOCOL_SERVER_Shp "vsa.shp.ezlibs.com" //

// 账号管理
#define def_UniService_InterVideo_Port 18099 
#define def_UniService_InterVideo_ServerName "com.ezlibs.InterVideo"
// 运维采集服务
#define def_UniService_MinerMtService_Port 18100 
#define def_UniService_MinerMtService_ServerName "com.ezlibs.MmtService"
#define def_UniService_MinerMtService_Sid "MmtSid"
// 
#define def_UniService_KeyManagementService_Port 18189 
#define def_UniService_KeyManagementService_ServerName "com.ezlibs.KeyManagementService"
#define def_UniService_KeyManagementService_Sid "KmsSid"

// 媒体播放状态服务
#define def_UniService_LiveStatus_Port 60186
#define def_Pandora_LiveStatusServer_ServerName "com.ezlibs.LiveStatus "
#define def_UniService_LiveStatus_Sid "StaSid"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* UniTrans */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//调试
#define def_UniTrans_Req_hello "/UniTrans/hello"
#define def_UniTrans_Req_Echo "/UniTrans/echo"
#define def_UniTrans_Req_describe "/UniTrans/describe"
#define def_UniTrans_Req_ip "/UniTrans/ip"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* PandoraQ */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#define def_PandoraQ_Req_Publish_IO_Out       "/pq/pub/io/out"            // io输出 控制
#define def_PandoraQ_MasReq_Publish_IO_Out    "/Ma2Nod/pq/pub/io/out"     // Ma请求
#define def_PandoraQ_NodReq_Publish_IO_Out    "/Nod2Ma/pq/pub/io/out"     //Node请求

// 鼠标键盘控制
#define def_PandoraQ_Req_Publish_HID_Send       "/pq/pub/hid/send"            // HID 输出
#define def_PandoraQ_MasReq_Publish_HID_Send    "/Ma2Nod/pq/pub/hid/send"     // Ma请求
#define def_PandoraQ_NodReq_Publish_HID_Send    "/Nod2Ma/pq/pub/hid/send"     //Node请求

// 目前用于设备上报
#define def_PandoraQ_Req_PublishValue         "/pq/pub/val"
#define def_PandoraQ_MasAck_PublishValue      "/Ma2Nod/pq/pub/val"        // Ma请求
#define def_PandoraQ_NodReq_PublishValue      "/Nod2Ma/pq/pub/val"        //Node请求
#define def_PandoraQ_Req_GetValue                   "/pq/get/val"

//批量 todo
#define def_PandoraQ_Req_GetValue_List         "/pq/get/val/list"
#define def_PandoraQ_MasAck_GetValue_List      "/Ma2Nod/pq/get/val/list"        // Ma请求
#define def_PandoraQ_NodReq_GetValue_List      "/Nod2Ma/pq/get/val/list"        //Node请求

#define def_PandoraQ_Req_SetValue_List         "/pq/set/val/list"
#define def_PandoraQ_MasAck_SetValue_List      "/Ma2Nod/pq/set/val/list"        // Ma请求
#define def_PandoraQ_NodReq_SetValue_List      "/Nod2Ma/pq/set/val/list"        //Node请求

// todo
#define def_PandoraQ_Req_SetPlanSchedule            "/pq/set/plan/schedule"     // 设置计划表
#define def_PandoraQ_Req_GetPlanSchedule            "/pq/get/plan/schedule"     //查询计划表
#define def_PandoraQ_MasReq_PubTaskStart    "/Ma2Nod/pq/pub/task/start"     // Ma请求
#define def_PandoraQ_Req_PubTaskStart               "/pq/pub/task/start"        //手动开启任务
#define def_PandoraQ_Req_PubTaskStop                "/pq/pub/task/stop"         // 手动停止任务
#define def_PandoraQ_Req_GetTaskList              "/pq/get/task/list"       // 任务执行状况查询
#define def_PandoraQ_Req_GetTaskDetails              "/pq/get/task/details"       // 任务执行状况查询

#define def_PandoraQ_Req_GetDeviceList              "/pq/get/device/list"       // 任务执行状况查询

//设置shp
#define def_PandoraQ_Cfg_ShpcSet       "config_shpc"            // 
#define def_PandoraQ_Req_ShpSet       "/pq/shp/set"            // 设置
#define def_PandoraQ_MasReq_ShpSet    "/Ma2Nod/pq/shp/set"     // Ma请求
#define def_PandoraQ_NodReq_ShpSet    "/Nod2Ma/pq/shp/set"     //Node请求
#define def_PandoraQ_Req_ShpGet       "/pq/shp/get"            // 查看当前设置
#define def_PandoraQ_MasReq_ShpGet    "/Ma2Nod/pq/shp/get"     // Ma请求
#define def_PandoraQ_NodReq_ShpGet    "/Nod2Ma/pq/shp/get"     //Node请求
//
#define def_PandoraQ_Cfg_ShpcPortMin       "config_shpc_port_min"             // 
#define def_PandoraQ_Cfg_ShpcPortMax       "config_shpc_port_max"            // 
//#define def_PandoraQ_Cfg_ShpcPortNext       "config_shpc_port_next"            // 

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* Psso -- Pandora Streamming Service Onvif*/
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_Psso_Req_DeviceAdd        "/psso/dev/add"
#define def_Psso_Req_DeviceCancel     "/psso/dev/cancel"
#define def_Psso_Req_ValueGet         "/psso/val/get"
#define def_Psso_Req_ValueSet         "/psso/val/set"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* iv -- InterVideo */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_InterVideo_Req_DeviceAdd        "/intervideo/dev/add"
#define def_InterVideo_Req_DeviceCancel     "/intervideo/dev/cancel"
#define def_InterVideo_Req_ValueGet         "/intervideo/val/get"
#define def_InterVideo_Req_ValueSet         "/intervideo/val/set"
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* kms -- KeyManagementService */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_KeyManagementService_Req_generateKeypair        "/kms/generate/keypair"
#define def_KeyManagementService_Req_encryptAsymmetric      "/kms/encrypt/asymmetric"
#define def_KeyManagementService_Req_decryptAsymmetric      "/kms/decrypt/asymmetric"
#define def_KeyManagementService_Req_generateKey            "/kms/generate/key"
#define def_KeyManagementService_Req_encryptSymmetric       "/kms/encrypt/symmetric "
#define def_KeyManagementService_Req_decryptSymmetric       "/kms/decrypt/symmetric"

#define def_KeyManagementService_Req_AppKeyCreate  "/kms/appkey/create"
#define def_KeyManagementService_Req_AppKeyCancel  "/kms/appkey/cancel"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* pss_Ntf : pss notify */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//
#define def_PssNotify_Req_Rtmp_OnPlay "/pss_Ntf/rtmp/on_play"
#define def_PssNotify_Req_Rtmp_OnPlayDone "/pss_Ntf/rtmp/on_play_done"
#define def_PssNotify_Req_Rtmp_OnPublish "/pss_Ntf/rtmp/on_publish"
#define def_PssNotify_Req_Rtmp_OnPublishDone "/pss_Ntf/rtmp/on_publish_done"
#define def_PssNotify_Req_Rtmp_OnUpdate "/pss_Ntf/rtmp/on_update"
#define def_PssNotify_Req_Rtmp_OnDone "/pss_Ntf/rtmp/on_done"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* 运维- mt */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_Maintenance_Req_mt_query_host_up                   "/mt/query/host/up"                     // 主机类-在线
#define def_Maintenance_Req_mt_query_host_cpu_usage            "/mt/query/host/cpu/usage"              // 主机类-CPU使用率

#define def_Maintenance_Req_mt_query_host_mem_usage            "/mt/query/host/mem/usage"              // 主机类-内存使用率
#define def_Maintenance_Req_mt_query_host_mem_total            "/mt/query/host/mem/total"              // 主机类-内存总量
#define def_Maintenance_Req_mt_query_host_mem_available        "/mt/query/host/mem/available"          // 主机类-内存未使用量

#define def_Maintenance_Req_mt_query_host_disk_usage           "/mt/query/host/disk/usage"             // 主机类-磁盘使用率
#define def_Maintenance_Req_mt_query_host_disk_total           "/mt/query/host/disk/total"             // 主机类-磁盘使用率
#define def_Maintenance_Req_mt_query_host_disk_available       "/mt/query/host/disk/available"         // 主机类-磁盘使用率

#define def_Maintenance_Req_mt_query_host_network_upperrate    "/mt/query/host/network/upperrate"      // 主机类-上行速率
#define def_Maintenance_Req_mt_query_host_network_downwardrate "/mt/query/host/network/downwardrate"   // 主机类-下行速率

#define def_Maintenance_Req_mt_start_ipcam_dialing             "/mt/start/ipcam/dialing"               // ipcam-摄像机检测-启动
#define def_Maintenance_Req_mt_query_ipcam_dialing             "/mt/query/ipcam/dialing"               // ipcam-检测结果(按设备查)
#define def_Maintenance_Req_mt_cut_ipcam_dialing               "/mt/cut/ipcam/dialing"                 // ipcam-批量取走(Cut)检测结果

#endif // _DefUniService_H
