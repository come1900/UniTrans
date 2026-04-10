/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UniTransRestMsg.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: UniTransRestMsg.h 5884 2016-09-25 09:29:45Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-09-25 09:29:45  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#if !defined(_UniTransRestMsg_H)
#define _UniTransRestMsg_H

#include <string>
#include <vector>
#include <iostream>

//#define _DEV_THIS_WITH_COUT 1

const std::string UniMsg_Key_HostID      = "HostID"      ;
const std::string UniMsg_Key_HostAlias   = "HostAlias"   ;
const std::string UniMsg_Key_Serial      = "Serial"      ;
const std::string UniMsg_Key_Description = "Description" ;
const std::string UniMsg_Key_Type        = "Type"        ;
const std::string UniMsg_Key_Version     = "Version"        ;
const std::string UniMsg_Key_Location    = "Location"        ;
const std::string UniMsg_Key_Vendor      = "Vendor"        ;
const std::string UniMsg_Key_Mac         = "Mac"        ;
const std::string UniMsg_Key_LocalDate   = "LocalDate"   ;

const std::string UniMsg_Key_System      = "System"      ;

//
// 消息组成元素——本身系统信息
//
class CMsgSystem
{
public:
    //System.HostName  String  50  系统名， System中的内容均填充本消息
    //发送者的信息；下同
    //System.Serial  String  256  产品序列号;全网唯一
    //System.Description  String  256  产品名或者型号等的说明
    //System.Type  String  256  类型，系统预定义如下：
    //ss_GeminiLock : 根服务器
    //sg_GeminiLock : 智能网关
    //ci_GeminiLock_ipad : ipad终端软件
    //ci_GeminiLock_iphone : iphone终端软件
    //ci_GeminiLock_android : android版终端软件
    //非上述类型的消息可能会被系统忽略
    //System.LocalDate  String  50

    std::string m_strHostID       ;
    std::string m_strHostAlias    ;
    std::string m_strSerial       ;
    std::string m_strDescription  ;
    std::string m_strType         ;
    std::string m_strVersion      ;
    std::string m_strLocation     ;
    std::string m_strVendor       ;
    std::string m_strMac          ;
    std::string m_strLocalDate    ;

    CMsgSystem()
    {
        m_strHostID      = "";
        m_strHostAlias   = "";
        m_strSerial      = "";
        m_strDescription = "";
        m_strType        = "";
        m_strVersion     = "";
        m_strLocation    = "";
        m_strVendor      = "";
        m_strMac         = "";
        m_strLocalDate   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_HostID       << ":" << m_strHostID      << std::endl;
        std::cout << UniMsg_Key_HostAlias    << ":" << m_strHostAlias   << std::endl;
        std::cout << UniMsg_Key_Serial       << ":" << m_strSerial      << std::endl;
        std::cout << UniMsg_Key_Description  << ":" << m_strDescription << std::endl;
        std::cout << UniMsg_Key_Type         << ":" << m_strType        << std::endl;
        std::cout << UniMsg_Key_Version      << ":" << m_strVersion     << std::endl;
        std::cout << UniMsg_Key_Location     << ":" << m_strLocation    << std::endl;
        std::cout << UniMsg_Key_Vendor       << ":" << m_strVendor      << std::endl;
        std::cout << UniMsg_Key_Mac          << ":" << m_strMac         << std::endl;
        std::cout << UniMsg_Key_LocalDate    << ":" << m_strLocalDate   << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string UniMsg_Key_ServiceID      = "ServiceID"      ;
const std::string UniMsg_Key_URI      = "URI"      ;
const std::string UniMsg_Key_Protocol      = "Protocol"      ;

const std::string UniMsg_Key_Service      = "Service"      ;

//
// 消息组成元素——本身系统信息
//
class CMsgService
{
public:
    //Service.ServiceID	String	50	服务ID号，为serial+1…2 保证唯一，如注册者只提供一个服务，可以为serial即可。即自行保证自己的服务可唯一识别
    //Service.Description	String	50	服务描述
    //Service.URI	String	50	服务地址，可提供服务的设备专有，其他设备可省
    //Service.Port	String	50	服务端口，可提供服务的设备专有，其他设备可省
    //Service.Protocol	String	50	服务协议：
    //sg_GeminiLock_tcp ： 网关协议over tcp
    //ss_GeminiLock_http ： 智能服务器协议over http
    //pubs_GeminiLock_http ： 公共服务协议over http
    std::string m_strServiceID   ;
    std::string m_strDescription ;
    std::string m_strURI         ;
    //std::string m_strPort        ;
    std::string m_strProtocol    ;

    CMsgService()
    {
        m_strServiceID   = "";
        m_strDescription = "";
        m_strURI         = "";
        //m_strPort        = "";
        m_strProtocol    = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        //std::cout << "CMsgService::dump:" << std::endl;

        std::cout << "ServiceID   :" << m_strServiceID    << std::endl;
        std::cout << "Description :" << m_strDescription  << std::endl;
        std::cout << "URI         :" << m_strURI          << std::endl;
        //std::cout << "Port        :" << m_strPort         << std::endl;
        std::cout << "Protocol    :" << m_strProtocol     << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

// 统一的返回消息
const std::string PssMsg_Value_Result_Success                       = "0";
const std::string PssMsg_Value_ResultMsg_Success                    = "Ok";
const std::string PssMsg_Value_Result_Prepareing                    = "1";
const std::string PssMsg_Value_ResultMsg_Prepareing                 = "Request accepted, Prepareing";
const std::string PssMsg_Value_Result_InvalidRequest                = "100";               // 数据格式错误
const std::string PssMsg_Value_ResultMsg_InvalidRequest             = "Invalid Request";
const std::string PssMsg_Value_Result_InvalidDataFormat             = "101";               // 数据格式错误
const std::string PssMsg_Value_ResultMsg_InvalidDataFormat          = "InvalidDataFormat";
const std::string PssMsg_Value_Result_InvalidData                   = "102"; // 数据错误
const std::string PssMsg_Value_ResultMsg_InvalidData                = "InvalidData";
const std::string PssMsg_Value_Result_IllegalAccess                 = "104"; // 非法访问
const std::string PssMsg_Value_ResultMsg_IllegalAccess              = "Illegal access";
const std::string PssMsg_Value_Result_UnauthorizedAccess            = "105"; // 未授权
const std::string PssMsg_Value_ResultMsg_UnauthorizedAccess         = "Unauthorized access";
const std::string PssMsg_Value_Result_NotPermittedAccess            = "106"; // 未容许， 如黑名单， 非白名单用户等
const std::string PssMsg_Value_ResultMsg_NotPermittedAccess         = "Not permitted access";
const std::string PssMsg_Value_Result_TooMuchData                   = "107"; // 未授权
const std::string PssMsg_Value_ResultMsg_TooMuchData                = "Message is too long";
const std::string PssMsg_Value_Result_InvalidParameter              = "108"; //
const std::string PssMsg_Value_ResultMsg_InvalidParameter           = "Invalid parameter";
const std::string PssMsg_Value_Result_ConnectionTimedout            = "109"; //
const std::string PssMsg_Value_ResultMsg_ConnectionTimedout         = "Connection timed out";

const std::string PssMsg_Value_Result_Unknow                        = "400"; // 未知错误
const std::string PssMsg_Value_ResultMsg_Unknow                     = "Unknow error";
const std::string PssMsg_Value_Result_SysBusy                       = "401"; // 系统忙
const std::string PssMsg_Value_ResultMsg_SysBusy                    = "System busy";
const std::string PssMsg_Value_Result_Redo                          = "402"; // 重复创建
const std::string PssMsg_Value_ResultMsg_Redo                       = "Duplicate request";
const std::string PssMsg_Value_Result_NotEnoughMem                  = "403"; // 内存不足
const std::string PssMsg_Value_ResultMsg_NotEnoughMem               = "Insufficient memoryt";
const std::string PssMsg_Value_Result_UnknowReq                     = "404"; // 重复创建
const std::string PssMsg_Value_ResultMsg_UnknowReq                  = "Unknow request";
const std::string PssMsg_Value_Result_MethodNotAllowed              = "405"; // 请求不支持
const std::string PssMsg_Value_ResultMsg_MethodNotAllowed           = "Method not allowed";
const std::string PssMsg_Value_Result_UnSupportedRequest            = "406"; // 不支持
const std::string PssMsg_Value_ResultMsg_UnSupportedRequest         = "Unsupported request";
const std::string PssMsg_Value_Result_UnderConstruction             = "407"; // 不支持
const std::string PssMsg_Value_ResultMsg_UnderConstruction          = "Under construction";
const std::string PssMsg_Value_Result_NotPermittedOperation         = "408"; // 操作不容许
const std::string PssMsg_Value_ResultMsg_NotPermittedOperation      = "Not permitted operation";
const std::string PssMsg_Value_Result_UnSupportedOperation          = "409"; // 操作不支持
const std::string PssMsg_Value_ResultMsg_UnSupportedOperation       = "Unsupported operation";
const std::string PssMsg_Value_Result_PasswdNotValid                = "440"; 
const std::string PssMsg_Value_ResultMsg_PasswdNotValid             = "Passwd not valid";
const std::string PssMsg_Value_Result_AccountNotExist               = "441"; 
const std::string PssMsg_Value_ResultMsg_AccountNotExist            = "Account not exist";
const std::string PssMsg_Value_Result_internalError                 = "442"; // 内存不足
const std::string PssMsg_Value_ResultMsg_internalError              = "Internal error";

const std::string PssMsg_Value_Result_DeviceOffline                 = "202"; // 
const std::string PssMsg_Value_ResultMsg_DeviceOffline              = "Device Offline";
const std::string PssMsg_Value_Result_NoNeedThisOpr                 = "204"; 
const std::string PssMsg_Value_ResultMsg_NoNeedThisOpr              = "Invalid operation(Not found)";
const std::string PssMsg_Value_Result_PssSidMismatched              = "206"; //
const std::string PssMsg_Value_ResultMsg_PssSidMismatched           = "PssSid Mismatched.";

// 自定义错误号 801-899
const std::string PssMsg_Result_LiveStreamming_ChannelNotValid      = "801"; 
const std::string PssMsg_ResultMsg_LiveStreamming_ChannelNotValid   = "Channel not valid";
const std::string PssMsg_Result_LiveStreamming_SubtypeNotValid      = "8012"; 
const std::string PssMsg_ResultMsg_LiveStreamming_SubtypeNotValid   = "Subtype not valid";
const std::string PssMsg_Result_LiveStreamming_DevIDNotValid        = "803"; 
const std::string PssMsg_ResultMsg_LiveStreamming_DevIDNotValid     = "DevID not valid";
const std::string PssMsg_Result_LiveStreamming_NoDevice             = "804"; // 没有一个在转码任务中
const std::string PssMsg_ResultMsg_LiveStreamming_NoDevice          = "No device";

const std::string PssMsg_Result_PandoraMatrix_HostnameNotValid      = "812"; 
const std::string PssMsg_ResultMsg_PandoraMatrix_HostnameNotValid   = "Hostname not valid";
const std::string PssMsg_Result_PandoraMatrix_DuplicateHostname     = "813"; 
const std::string PssMsg_ResultMsg_PandoraMatrix_DuplicateHostname  = "Duplicate Hostname";

const std::string PssMsg_Result_PandoraMatrix_FailedTodo            = "824"; // 执行失败
const std::string PssMsg_ResultMsg_PandoraMatrix_FailedTodo         = "Failed to do";
const std::string PssMsg_Result_PandoraMatrix_Progressing           = "825";  // 正在执行
const std::string PssMsg_ResultMsg_PandoraMatrix_Progressing        = "Progressing";
const std::string PssMsg_Result_PandoraMatrix_Progressed            = "826"; // 执行完毕
const std::string PssMsg_ResultMsg_PandoraMatrix_Progressed         = "Progressed";
const std::string PssMsg_Result_PandoraMatrix_Interrupted           = "827"; //中断执行
const std::string PssMsg_ResultMsg_PandoraMatrix_Interrupted        = "Was interrupted";
const std::string PssMsg_Result_PandoraNode_FailedToSaveCfgFile     = "828"; //保存文件失败
const std::string PssMsg_ResultMsg_PandoraNode_FailedToSaveCfgFile  = "Failed to save config file";
const std::string PssMsg_Result_PandoraNode_NotFoundCfgFile         = "829"; //
const std::string PssMsg_ResultMsg_PandoraNode_NotFoundCfgFile      = "Config Not Found";

const std::string PssMsg_Value_Result_RecNotStarted                 = "830"; // 当前不在录像
const std::string PssMsg_Value_ResultMsg_RecNotStarted              = "Record not started.";
const std::string PssMsg_Value_Result_RecQueryFailed                = "831"; // 查询失败
const std::string PssMsg_Value_ResultMsg_RecQueryFailed             = "Failed query.";

//
// 简单返回消息
//
const std::string PssMsg_Key_Result                                 = "Result";
const std::string PssMsg_Key_ResultMsg                              = "ResultMsg";

class CAckMsgSimple
{
public:
    std::string Result;
    std::string Reason;

    CAckMsgSimple()
    {
        Result = "";
        Reason = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PssMsg_Key_Result << ":" << Result << std::endl;
        std::cout << PssMsg_Key_ResultMsg << ":" << Reason << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string UniMsg_Key_Account = "Account";
const std::string UniMsg_Key_Password = "Password";
const std::string UniMsg_Key_SessionID = "SessionID";
//
// 鉴权 | 注册 | 心跳 | 注销 消息
//
class CSmsMsg_requestAuth
{
public:
    std::string m_strAccount  	    ;// 设备编号
    std::string m_strPassword 		;// 访问授权码 通过编号+授权码直接访问
    std::string m_strSessionID	    ;// 授权用户名

    CMsgSystem m_LocalSystemInfo;

    std::vector<CMsgService> m_vLocalService;

    CSmsMsg_requestAuth()
    {
        m_strAccount  	    = "" ;
        m_strPassword 		= "" ;
        m_strSessionID	    = "" ;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << "+ CSmsMsg_requestAuth::dump" << std::endl;
        std::cout << "Account   :" << m_strAccount   << std::endl;
        std::cout << "Password  :" << m_strPassword  << std::endl;
        std::cout << "SessionID :" << m_strSessionID << std::endl;

        std::cout << "++ LocalSystemInfo" << std::endl;
        m_LocalSystemInfo.dump();

        std::cout << "++ ServiceInfo" << std::endl;
        for (std::vector<CMsgService>::iterator iter = m_vLocalService.begin(); iter != m_vLocalService.end(); ++iter)
        {
            iter->dump();
        }
#endif //_DEV_THIS_WITH_COUT
    }
};

//
// 鉴权 | 注册 | 心跳 | 注销 消息 —— ACK
//
class CSmsMsg_requestAuthACK:public CAckMsgSimple
{
public:
    std::string m_strSessionID	       ;// 授权用户名
    std::string m_strSessionTimeout    ;// 设备编号
    //std::string m_strResult 		   ;// 结果
    //std::string m_strResultMsg 		   ;// Result的参考文字描述

    CMsgSystem m_LocalSystemInfo;

    std::vector<CMsgService> m_vLocalService;

    CSmsMsg_requestAuthACK()
    {
        m_strSessionID	       = "";
        m_strSessionTimeout    = "";
        //m_strResult 		   = "";
        //m_strResultMsg 		   = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();
#ifdef _DEV_THIS_WITH_COUT
        std::cout << "+ CSmsMsg_requestAuthACK::dump" << std::endl;
        std::cout << "SessionID	     :" << m_strSessionID	   << std::endl;
        std::cout << "SessionTimeout :" << m_strSessionTimeout << std::endl;
        //std::cout << "Result 		 :" << m_strResult 		   << std::endl;
        //std::cout << "ResultMsg  	 :" << m_strResultMsg 		<< std::endl;
        std::cout << "++ LocalSystemInfo" << std::endl;
        m_LocalSystemInfo.dump();

        std::cout << "++ ServiceInfo" << std::endl;
        for (std::vector<CMsgService>::iterator iter = m_vLocalService.begin(); iter != m_vLocalService.end(); ++iter)
        {
            iter->dump();
        }
#endif //_DEV_THIS_WITH_COUT
    }
};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//Pandora
//
const std::string PandoraMsg_Key_MsgSession = "pToSe";// 某消息的标识
class CPandoraMsg_Common
{
public:
    std::string m_strMsgSession;

    CPandoraMsg_Common()
    {
        m_strMsgSession = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PandoraMsg_Key_MsgSession << ":" << m_strMsgSession << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

//
// 创建AppKey - 请求消息
//
//const std::string UniMsg_Key_Account  = "Account";
//const std::string UniMsg_Key_Password = "Password";
//const std::string UniMsg_Key_Service  = "Service";//Service's ID

class CUniMsg_reqCreateAppKey
{
public:
    std::string Account;
    std::string Password;
    std::string Service;

    CUniMsg_reqCreateAppKey()
    {
        Account  = "";
        Password = "";
        Service  = ""; 
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_Account  << ":" << Account  << std::endl;
        std::cout << UniMsg_Key_Password << ":" << Password << std::endl;
        std::cout << UniMsg_Key_Service  << ":" << Service  << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

//
// 创建AppKey - 返回消息
//
const std::string UniMsg_Key_AppKey    = "AppKey";
const std::string UniMsg_Key_AppSecret = "AppSecret";
const std::string UniMsg_Key_Validity  = "Validity";//有效期，UTC， 0-长期

const std::string  def_AppKeyValidity_Canceled = "0"; //已取消
const std::string  def_AppKeyValidity_LongTerm = "1"; // 长期有效

class CUniMsg_reqCreateAppKeyAck : public CAckMsgSimple
{
public:
    std::string AppKey;
    std::string AppSecret;
    std::string Validity; //0-100 错误码 特殊含义，大于1000表示UTC

    CUniMsg_reqCreateAppKeyAck()
    {
        AppKey  = "";
        AppSecret = "";
        Validity  = ""; 
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << UniMsg_Key_AppKey  << ":" << AppKey << std::endl;
        std::cout << UniMsg_Key_AppSecret << ":" << AppSecret << std::endl;
        std::cout << UniMsg_Key_Validity  << ":" << Validity << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

//
const std::string UniMsg_Key_GLic    = "GLic";
class CUniMsg_reqGetSetGmLic
{
public:
    std::string strGLic;

    CUniMsg_reqGetSetGmLic()
    {
        strGLic  = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_GLic  << ":" << strGLic  << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string UniMsg_Key_GLicVer    = "GLicVer";
const std::string UniMsg_Key_ValidSeconds    = "ValidSeconds";
const std::string UniMsg_Key_ValidDate    = "ValidDate";
class CUniMsg_reqGetSetGmLicAck : public CAckMsgSimple
{
public:
    int m_iGLicVer;
    std::string m_strllValidSeconds;//for GMINI_LIC_V1
    std::string m_strttValidDate;//for GMINI_LIC_V2
    //char m_strValidDate[20];//for GMINI_LIC_V2
    std::string strGLic;

    CUniMsg_reqGetSetGmLicAck()
    {
        m_iGLicVer = 0;
        m_strllValidSeconds = "";//for GMINI_LIC_V1
        m_strttValidDate = "";//for GMINI_LIC_V2
        strGLic = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << UniMsg_Key_GLicVer<< ":" << m_iGLicVer << std::endl;
        std::cout << UniMsg_Key_ValidSeconds  << ":" << m_strllValidSeconds  << std::endl;
        std::cout << UniMsg_Key_ValidDate << ":" << m_strttValidDate  << std::endl;
        std::cout << UniMsg_Key_GLic  << ":" << strGLic  << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

//  配置管理
const std::string UniMsg_Key_User    = "User";
const std::string UniMsg_Key_Section = "Section";
const std::string UniMsg_Key_Name    = "Name";
const std::string UniMsg_Key_Value   = "Value";
class CUniMsg_reqConfigModify
{
public:
    std::string User   ;
    std::string Section;
    std::string Name   ;
    std::string Value  ;

    CUniMsg_reqConfigModify()
    {
        User    = "";
        Section = "";
        Name    = ""; 
        Value   = ""; 
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << UniMsg_Key_User    << ":" << User    << std::endl;
        std::cout << UniMsg_Key_Section << ":" << Section << std::endl;
        std::cout << UniMsg_Key_Name    << ":" << Name    << std::endl;
        std::cout << UniMsg_Key_Value   << ":" << Value   << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string UniMsg_Key_manConfig_TykdMobileService    = "TykdMobileService";
const std::string UniMsg_Key_manConfig_HostRelative    = "HostRelative";
const std::string UniMsg_Key_manConfig_DefaultAudioCodec    = "DefaultAudioCodec";
#endif // !defined(_UniTransRestMsg_H)
