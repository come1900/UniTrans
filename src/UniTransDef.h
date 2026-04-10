/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UniTransDef.h - _explain_
 *
 * Copyright (C) 2012 ezlibs.com, All Rights Reserved.
 *
 * $Id: UniTransDef.h 0001 2012-4-11 9:32:40Z WuJunjie $
 *
 *  Explain:
 *     -Common define-
 *
 *  Update:
 *     2012-4-11 9:32:49 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _UniTransDef_H
#define _UniTransDef_H

// prompt

#define DEFAULT_NEXT_HOST "192.168.10.246"
#define DEFAULT_NEXT_HOST_PORT 60000

#define DEFAULT_NEXT_HTTP_HOST "192.168.6.101"
#define DEFAULT_NEXT_HTTP_PORT 60088
//#define DEFAULT_NEXT_HTTP_HOST "sms.ezlibs.com"
//#define DEFAULT_NEXT_HTTP_PORT 60088

#define THE_TCPSVR_AGENT_PORT 60000

#define THE_HTTP_SVR_AGENT_PORT 60080

#define THE_UDPSVR_AGENT_PORT 60000

// 调试端口
#define PORT_GENERAL_CONSOLE_OVER_TCP 19023
#define THE_STATUS_AGENT_PORT 19024

//// 用时修改
//#define _FUNC_CONSOLE_OVERTCP          // #远程调试接口
//#define EXAMPLE_SOCKET_STATUS_AGENT             // #tcp状态服务器
//#define EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR   // #http协议的客户端
//#define EXAMPLE_DATAPARSE_EXAMPLE               // #数据解析示例
//#define EXAMPLE_GENERALAGENTTCPSOCKETSERVER     // #TCP服务端
//#define EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR  // #tcp客户端
//#define EXAMPLE_GENERALAGENTUDPSOCKETSERVER     // #UDP服务端
//#define EXAMPLE_GENERALSOCKETPROCESSOR          // #使用单独的处理线程
//#define EXAMPLE_GENERALAGENTHTTPSERVER          // #HTTP服务端
//#define EXAMPLE_SIGNAL_ONDATA                   // #使用数据回调机制
//#define EXAMPLE_THE_WEBSERVICE                  // #webservice
//#define _USE_LOG4CPP                         // #
#ifndef _BUILD_VERSION
#define _BUILD_VERSION   "195:203"
#endif // $(shell svnversion -n)
#ifndef _VERSION
#define _VERSION "1.0.0"
#endif//_VERSION
#ifndef _COPY_RIGHT_STRING
#define _COPY_RIGHT_STRING "@(#) Copyright (c) 2022 ezlibs.com. All rights reserved"
#endif//_COPY_RIGHT_STRING
#ifndef _CONNECT_INFO_STRING
#define _CONNECT_INFO_STRING "info@ezlibs.com"
#endif //_CONNECT_INFO_STRING
#ifdef _VENDOR
#undef _COPY_RIGHT_STRING
#define _COPY_RIGHT_STRING "@(#) Copyright (c) 2022 "_VENDOR". All rights reserved"
#undef _CONNECT_INFO_STRING
#define _CONNECT_INFO_STRING "info@"_VENDOR
#endif //_VENDOR
#ifndef _CONFIG_FILENAME_MAIN
#define _CONFIG_FILENAME_MAIN "cfg-uniTrans.cfg"
#endif
#ifndef _CONFIG_FILENAME_MAIN_DUPLICATE
#define _CONFIG_FILENAME_MAIN_DUPLICATE "cfg-uniTrans.cfg.duplicate"
#endif
#ifndef _LOG_FILENAME
#define _LOG_FILENAME "log-uniTrans.log"
#endif

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//事务处理状态
typedef enum en_log_level_show
{
    enLogLevelShow_contes  =  0, //总体摘要
    enLogLevelShow_list  =  0,//列表
    enLogLevelShow_detail  =  100,
}enLogLevelShow;

typedef enum __en_ConfigYesOrNo
{
    ConfigYesOrNo_No = 0,
    ConfigYesOrNo_Yes = 1,
}enConfigYesOrNo;

#define TCP_CONN_TIMEOUT 10
#define TCP_MSGG_TIMEOUT 3

#define HTTPD_CONN_TIMEOUT 30
#define HTTPD_MSGG_TIMEOUT 5
#define def_HTTPD_MSGG_BodyMaxSize 1024*8

#define SERVER_MIN_PORT 1
#define HTTPD_MIN_PORT 80

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

// for src/Function/PullStreammingServer/
//_USE_PssServerPlayUrlHXReq
//
#define APP_AUTH_HTTP_HEAD_KEY_TransferEncoding "Transfer-Encoding"
#define APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null "Null" // 原始的， 不编码的
#define APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB "eZAB" // ezlibs's zip, aes, base64  消息用CheckSum 加密
#define APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_ZAB "zab" // ezlibs's zip, aes, base64  消息用sha(secret) 加密

#define APP_AUTH_HTTP_HEAD_KEY_Nonce "Nonce"
#define APP_AUTH_HTTP_HEAD_KEY_AppKey "AppKey"
#define APP_AUTH_HTTP_HEAD_KEY_CurTime "CurTime"
#define APP_AUTH_HTTP_HEAD_KEY_CheckSum "CheckSum"
#define APP_AUTH_HTTP_HEAD_KEY_PssSid "PssSid"
#define APP_AUTH_HTTP_HEAD_KEY_KmsSid "KmsSid"
#define APP_AUTH_HTTP_HEAD_KEY_MmtSid "MmtSid"
#define APP_AUTH_HTTP_HEAD_KEY_PssSid_MIN_LEN 3

#define def_MIN_TIME_T_VALUE_STR "252432000"/*1978-1-1 0:0:0*/
#define def_MIN_TIME_T_VALUE_INT 252432000 /*1978-1-1 0:0:0*/
#define def_MAX_TIME_T_VALUE_STR "2145888000"/*2038-1-1 0:0:0*/
#define def_MAX_TIME_T_VALUE_INT 2145888000 /*2038-1-1 0:0:0*/

#define def_Default_Alias "UniTrans"
#define def_Default_Code "G1"

#define def_Default_SerialNumber "147232519145067"
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_ProductID_Forensics_001      "Forensics_001"      // 取证仪 001
#define def_ProductID_Cleaner_001        "Cleaner_001"        // 除藻机 001
#define def_ProductID_QualityControl_001 "QualityControl_001" // 质控仪 001

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#endif // _UniTransDef_H
