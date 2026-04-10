/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * WebServiceMsg.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: WebServiceMsg.h 5884 2013-01-29 03:20:29Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:29  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _WebServiceMsg_H
#define _WebServiceMsg_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

//
// 简单返回消息
//
class CCommonSimpleMsg
{
public:
	//0:表示成功
    //1：失败原因参见Reason
    //2：HTTP 请求错误，失败原因
    //参见Reason
    std::string result;
    std::string reason;

    CCommonSimpleMsg()
    {
        result = "";
        reason = "";
    }

    void dump()
    {
        std::cout << "result:" << result << std::endl;
        std::cout << "reason:" << reason << std::endl;
    }
};
//
// 返回带有session的消息
//
class CAuthResponse : public CCommonSimpleMsg
{
public:
    std::string sessionId;
    CAuthResponse()
    {
        sessionId = "";
    }
    void dump()
    {
        std::cout << "sessionId:" << sessionId << std::endl;
        CCommonSimpleMsg::dump();
    }
};

//IPC - 
// 注册接口
//
class CIPC_requestRegister
{
public:
    std::string authUser 		;//string 用户名
    std::string authPassword 	;//string 用户密码

    //CIPC_requestAuth()
    CIPC_requestRegister()
    {
        authUser 	 = "";
        authPassword = "";
    }
    void dump()
    {
        std::cout << "authUser    :" << authUser   << std::endl;
        std::cout << "authPassword:" << authPassword << std::endl;
    }
};

//
// WEB子系统认证接口
//
class CIPC_requestAuth
{
public:
    std::string authUser 		;//string 用户名
    std::string authPassword 	;//string 用户密码

    CIPC_requestAuth()
    {
        authUser 	 = "";
        authPassword = "";
    }
    void dump()
    {
        std::cout << "authUser    :" << authUser   << std::endl;
        std::cout << "authPassword:" << authPassword << std::endl;
    }
};

//
//主机认证接口
//
class CIPC_requestAuthByHost
{
public:
    std::string sessionId 		;//string WEB子系统的sessionId
    std::string homeIdStr 		;//string 登录主机的homeId
    std::string userName 		;//string 家庭用户名
    std::string password 		;//string 家庭用户密码

    CIPC_requestAuthByHost()
    {
        sessionId 	 = "";
        homeIdStr 	 = "";
        userName 	 = "";
        password 	 = "";
    }
    void dump()
    {
        std::cout << "sessionId    :" 	<< sessionId   	<< std::endl;
        std::cout << "homeIdStr	   :" 	<< homeIdStr 	<< std::endl;
        std::cout << "userName     :" 	<< userName   	<< std::endl;
        std::cout << "password	   :" 	<< password 	<< std::endl;
    }
};

//
// 返回带有session的消息
//
class CAuthHostResponse : public CCommonSimpleMsg
{
public:
    std::string sessionId2;
    CAuthHostResponse()
    {
        sessionId2 = "";
    }
    void dump()
    {
        std::cout << "sessionId2:" << sessionId2 << std::endl;
        CCommonSimpleMsg::dump();
    }
};

//
//WEB子系统向连辅子系统注册接口地址的接口
//
class CIPC_registerURL
{
public:
    std::string sessionId ;//String 
    std::string baseURL ;//String 
    CIPC_registerURL()
    {
        sessionId = "" ;
        baseURL = "" ;
    }
    void dump()
    {
        cout << "sessionId :" <<  sessionId  << endl;
        cout << "baseURL :" <<  baseURL  << endl;
    }
}
;

//
//
//
class CIPC_registerURL_ACK : public CCommonSimpleMsg
{
}
;

class CIPC_heartbeat
{
public:
    std::string sessionId ;//String 
    CIPC_heartbeat()
    {
        sessionId = "" ;
    }
    void dump()
    {
        cout << "sessionId :" <<  sessionId  << endl;
    }
}
;

//
//
//
class CIPC_heartbeat_ACK : public CCommonSimpleMsg
{
}
;


class CIPC_stopHostNetwork
{
public:
    std::string sessionId ;//String 
    std::string homeIdStr ;//String 
    std::string username ;//String 
    CIPC_stopHostNetwork()
    {
        sessionId = "" ;
        homeIdStr = "" ;
        username = "" ;
    }
    void dump()
    {
        cout << "sessionId :" <<  sessionId  << endl;
        cout << "homeIdStr :" <<  homeIdStr  << endl;
        cout << "username :" <<  username  << endl;
    }
}
;

//
//
//
class CIPC_stopHostNetwork_ACK : public CCommonSimpleMsg
{
}
;

class CIPC_checkUpdate
{
public:
    std::string sessionId ;//String 
    std::string homeIdStr ;//String 
    std::string lastTime ;//String 
    std::string aesKey ;//String 
    std::string queryFrom ;//String 
    std::string product ;//String 
    CIPC_checkUpdate()
    {
        sessionId = "" ;
        homeIdStr = "" ;
        lastTime = "" ;
        aesKey = "" ;
        queryFrom = "" ;
        product = "" ;
    }
    void dump()
    {
        cout << "sessionId :" <<  sessionId  << endl;
        cout << "homeIdStr :" <<  homeIdStr  << endl;
        cout << "lastTime :" <<  lastTime  << endl;
        cout << "aesKey :" <<  aesKey  << endl;
        cout << "queryFrom :" <<  queryFrom  << endl;
        cout << "product :" <<  product  << endl;
    }
}
;

//
//
//
class CIPC_checkUpdate_ACK : public CCommonSimpleMsg
{
public:
    std::string cfgForce;
    std::string sessionId ;//String 
    std::string homeIdStr ;//String 
    std::string cfgUrl ;//String 
    std::string softForce ;//String 
    std::string queryFrom ;//String 
    std::string softUrl ;//String 
    std::string md5 ;//String 
    CIPC_checkUpdate_ACK()
    {
        sessionId = "" ;
        homeIdStr = "" ;
        cfgForce = "" ;
        cfgUrl = "" ;
        queryFrom = "" ;
        softForce = "" ;
        softUrl = "" ;
        md5 = "" ;
    }
    void dump()
    {
        cout << "cfgForce:" << cfgForce << std::endl;
        cout << "sessionId :" <<  sessionId  << endl;
        cout << "homeIdStr :" <<  homeIdStr  << endl;
        cout << "cfgForce :" <<  cfgForce  << endl;
        cout << "softForce :" <<  softForce  << endl;
        cout << "queryFrom :" <<  queryFrom  << endl;
        cout << "softUrl :" <<  softUrl  << endl;
        cout << "md5 :" <<  md5  << endl;
        CCommonSimpleMsg::dump();
    }
}
;

class CIPC_notifyUpdate
{
public:
    std::string sessionId ;//String 
    std::string homeIdStr ;//String 
    CIPC_notifyUpdate()
    {
        sessionId = "" ;
        homeIdStr = "" ;
    }
    void dump()
    {
        cout << "sessionId :" <<  sessionId  << endl;
        cout << "homeIdStr :" <<  homeIdStr  << endl;
    }
}
;

//
//
//
class CIPC_notifyUpdate_ACK : public CCommonSimpleMsg
{
}
;

class CIPC_reportDeviceStateList_operate
{
public:
    std::string dataPoint 		;//string 数据点ID
    std::string dataLen 	;//string 数据值的字符串长度
    std::string data 	;//String 实际的数据值

    
    CIPC_reportDeviceStateList_operate()
	{
		dataPoint	   = "";
		dataLen   = "";
		data  = "";
	}
	void dump()
	{
		cout << "dataPoint :" <<  dataPoint  << endl;
		cout << "dataLen :" <<	dataLen  << endl;
		cout << "data :" <<  data  << endl;
	
	}

}
;
class CIPC_reportDeviceStateList
{
public:
    std::string homeIdStr ;//String 
    
    std::vector<CIPC_reportDeviceStateList_operate> m_operate;
    CIPC_reportDeviceStateList()
    {
        homeIdStr = "" ;
    }
    void dump()
    {
        cout << "homeIdStr :" <<  homeIdStr  << endl;
	}
}
;

class CIPC_reportDeviceStateList_ACK : public CCommonSimpleMsg
{
}
;

class CIPC_getDeviceStateList_operate
{
public:
    std::string dataPoint 		;//string 数据点ID
    
    CIPC_getDeviceStateList_operate()
    {
        dataPoint      = "";
    }
    void dump()
    {
        cout << "dataPoint :" <<  dataPoint  << endl;

	}
}
;


//
// 获取设备状态接口
//
class CIPC_getDeviceStateList
{
public:
    std::string sessionId ;//String
    std::string sessionId2 ;//String
    std::string homeIdStr ;//String 
    
    std::vector<CIPC_getDeviceStateList_operate> m_operate;
    CIPC_getDeviceStateList()
    {
        sessionId = "" ;
        sessionId2 = "" ;
        homeIdStr = "" ;
    }
    void dump()
    {
        cout << "sessionId :" <<  sessionId  << endl;
        cout << "sessionId2 :" <<  sessionId2  << endl;
        cout << "homeIdStr :" <<  homeIdStr  << endl;
	}
}
;


class CIPC_getDeviceStateList_ACK_operate
{
public:
    std::string dataPoint 		;//string 数据点ID
    std::string dataLen 	;//string 数据值的字符串长度
    std::string data 	;//String 实际的数据值
    
    CIPC_getDeviceStateList_ACK_operate()
    {
        dataPoint      = "";
        dataLen   = "";
        data  = "";
    }
    void dump()
    {
        cout << "dataPoint :" <<  dataPoint  << endl;
        cout << "dataLen :" <<  dataLen  << endl;
        cout << "data :" <<  data  << endl;

	}
}
;
class CIPC_getDeviceStateList_ACK : public CCommonSimpleMsg
{
public:
    std::vector<CIPC_getDeviceStateList_ACK_operate> m_operate;

    CIPC_getDeviceStateList_ACK()
    {
        result 	= "";
        reason 	= "";
        m_operate.clear();
    }
    void dump()
    {}
}
;

//
// 控制命令接口 的operate数据
//
class CIPC_controlDevice_operate
{
public:
    std::string dataPoint 		;//String 
    std::string dataLen 	;//String 
    std::string data 	;//string 
    CIPC_controlDevice_operate()
    {
        dataPoint      = "";
        dataLen  = "";
        data    = "";
    }
    void dump()
    {
        cout << "dataPoint :" <<  dataPoint  << endl;
        cout << "dataLen :" <<  dataLen  << endl;
        cout << "data :" <<  data  << endl;
	}
}
;


//
// 控制命令接口的返回消息 - operate 数据
//
class CIPC_controlDevice_ACK_Operate
{
public:
    std::string dataPoint 		;//String 
    std::string dataLen 	;//String 
    std::string data 	;//string 
    CIPC_controlDevice_ACK_Operate()
    {
        dataPoint  = "";
        dataLen  = "";
        data    = "";
    }
    void dump()
    {}
}
;

//
// 控制命令接口
//
class CIPC_controlDevice
{
public:
    std::string sessionId 		;//String 
    std::string sessionId2 		;//String 
    std::string homeIdStr ;//String 
    std::vector<CIPC_controlDevice_operate> m_operate;
    CIPC_controlDevice()
    {
        sessionId = "";
        sessionId2 = "";
        homeIdStr = "";
        m_operate.clear();
    }
    void dump()
    {}
}
;


//
// 控制命令接口的返回消息
//
class CIPC_controlDevice_ACK : public CCommonSimpleMsg
{
public:
    std::vector<CIPC_controlDevice_ACK_Operate> m_operate;
    //Operate 操作控制组
    CIPC_controlDevice_ACK()
    {
    	result = "";
		reason = "";
        m_operate.clear();
    }
    void dump()
    {}
}
;
#endif /* _WebServiceMsg_H */
