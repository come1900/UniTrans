/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * SmsMsg.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: SmsMsg.h 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     SmsMsg -- Intelligent system communication protocol
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _SmsMsg_H
#define _SmsMsg_H

#include <string>
#include <vector>
#include <iostream>

#include "UniTransRestMsg.h"

using namespace std;

//
// 消息组成元素——数据转发
//
class CDataCapsules
{
public:
    std::string m_strType     ;
    std::string m_strEncode   ;
    std::string m_strLen      ;
    std::string m_strValue    ;

    CDataCapsules()
    {
        m_strType     = "";
        m_strEncode   = "";
        m_strLen      = "";
        m_strValue    = "";
    }

    void dump()
    {
        std::cout << "CDataCapsules::dump:"   << std::endl;
        std::cout << "Type   :" << m_strType   << std::endl;
        std::cout << "Encode :" << m_strEncode << std::endl;
        std::cout << "Len    :" << m_strLen    << std::endl;
        std::cout << "Value  :" << m_strValue  << std::endl;
    }
};

//
// 返回带有session的简单消息
//
class CAckMsgSession : public CAckMsgSimple
{
public:
    std::string SessionId;
    CAckMsgSession()
    {
        SessionId = "";
    }
    void dump()
    {
        std::cout << "SessionId:" << SessionId << std::endl;
        CAckMsgSimple::dump();
    }
};

//
// 本机状态
//
class CSmsMsg_LocalStatus
{
public:
    std::string LocalIp 		    ;// socket local address GetSockAddress
    std::string LocalDateTime		;// 2013-07-08 11:31:11

    CSmsMsg_LocalStatus()
    {
        LocalIp 	 = "";
        LocalDateTime  = "2013-07-08 11:31:11";
    }

    void dump()
    {
        std::cout << "LocalIp   :" << LocalIp  		<< std::endl;
        std::cout << "LocalDateTime :" 	 << LocalDateTime  	<< std::endl;
    }
};

//
// 数据转发 消息
//
class CSmsMsg_requestSend
{
public:
    std::string m_strSessionID	    ;// 授权用户名

    CMsgSystem m_LocalSystemInfo;
    CMsgService m_From;
    CMsgService m_Dest;
    CDataCapsules m_Data;

    CSmsMsg_requestSend()
    {
        m_strSessionID  	    = "" ;
    }

    void dump()
    {
        std::cout << "+ CSmsMsg_requestSend::dump" << std::endl;
        std::cout << "SessionID :" << m_strSessionID << std::endl;

        std::cout << "++ LocalSystemInfo" << std::endl;
        m_LocalSystemInfo.dump();
        
        std::cout << "++ From" << std::endl;
        m_From.dump();
        
        std::cout << "++ Dest" << std::endl;
        m_Dest.dump();

        std::cout << "++ Data" << std::endl;
        m_Data.dump();
    }
};

//
// 数据转发 消息 —— ACK
//
class CSmsMsg_requestSendACK:public CAckMsgSimple
{
public:
    std::string m_strSessionID	       ;// 授权用户名
    //std::string m_strResult 		   ;// 结果
    //std::string m_strResultMsg 		   ;// Result的参考文字描述

    CMsgSystem m_LocalSystemInfo;
    CMsgService m_From;
    CMsgService m_Dest;

    CSmsMsg_requestSendACK()
    {
        m_strSessionID	       = "";
        //m_strResult 		   = "";
        //m_strResultMsg 		   = "";
    }

    void dump()
    {
        CAckMsgSimple::dump();
        std::cout << "+ CSmsMsg_requestAuthACK::dump" << std::endl;
        std::cout << "SessionID	     :" << m_strSessionID	   << std::endl;
        //std::cout << "Result 		 :" << m_strResult 		   << std::endl;
        //std::cout << "ResultMsg  	 :" << m_strResultMsg 		<< std::endl;
        std::cout << "++ LocalSystemInfo" << std::endl;
        m_LocalSystemInfo.dump();

        std::cout << "++ From" << std::endl;
        m_From.dump();
        
        std::cout << "++ Dest" << std::endl;
        m_Dest.dump();
    }
};

//
// 注册消息，
//
class CSmsMsg_RegisterReq
{
public:
    std::string ProductID 		    ;// 设备编号
    std::string AuthCode 			;// 访问授权码 通过编号+授权码直接访问
    std::string AuthName 		    ;// 授权用户名
    std::string Password			;// AuthName + Password 的方式建立“信任圈”， 可以互相呈现，圈内客户端可以通过编号访问

    CSmsMsg_LocalStatus m_LocalStatus;

    CSmsMsg_RegisterReq()
    {
        ProductID 	 = "";
        AuthCode  = "";
        AuthName 	 = "";
        Password = "";
    }

    void dump()
    {
        std::cout << "ProductID   :" << ProductID  		<< std::endl;
        std::cout << "AuthCode :" 	 << AuthCode  	<< std::endl;
        std::cout << "AuthName    :" << AuthName   		<< std::endl;
        std::cout << "Password:" 	 << Password 	<< std::endl;

        std::cout << "m_LocalStatus:" 	<< std::endl;
        m_LocalStatus.dump();
    }
};

class CSmsMsg_ServiceAgent
{
public:
    std::string ServiceName 		;//
    std::string ServiceLocation		;// ip:port(192.168.6.112:10000) or http://pubservice.ezlibs.com:60089/PubService/getPubIP4

    CSmsMsg_ServiceAgent()
    {
        ServiceName 	 = "";
        ServiceLocation  = "http://pubservice.ezlibs.com:60089/PubService/getPubIP4";
    }

    void dump()
    {
        std::cout << "ServiceName:" << ServiceName  		<< std::endl;
        std::cout << "ServiceLocation:" 	 << ServiceLocation  	<< std::endl;
    }
};

//
// 注册返回消息，
//
class CSmsMsg_RegisterAck
{
public:
    CAckMsgSimple m_ackInfo		;
    std::string m_Session		; // 2013-07-09 11:23:14
    std::vector<CSmsMsg_ServiceAgent> m_Service;

    CSmsMsg_RegisterAck()
    {
        m_Session 	 = "";
    }

    void dump()
    {
        m_ackInfo.dump();
        std::cout << "m_Session:" << m_Session  		<< std::endl;
        for (std::vector<CSmsMsg_ServiceAgent>::iterator iter = m_Service.begin(); iter != m_Service.end(); ++iter)
        {
            iter->dump();
        }
    }
};

#endif /* _SmsMsg_H */
