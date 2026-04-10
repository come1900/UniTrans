/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveRecordServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveRecordServer.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerLiveRecordServer_H
#define _HttpServerLiveRecordServer_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "../Interface/defGRest.h"
#include "../Interface/GRestHttpdSocket.h"

#include "../Stack/pss/PssStack.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveRecordServer.h"
#endif //_USE_MODULES_EZCONFIG
#if defined(_USE_MODULES_LocalDataBase)
#include "../../DBAgent/DbLiveRecord.h"
#endif // _USE_MODULES_LocalDataBase

class CStreamContainer;
class CHandlerLiveRecordServer;
class CHttpServerLiveRecordServer : public CGRestHttpdSocket// : public HttpdSocket
{
public:
    CHttpServerLiveRecordServer(ISocketHandler& h);
    ~CHttpServerLiveRecordServer();

    void Init();

    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    //void CreateHeader(size_t ContentLength, const std::string &matchRequest, const std::string &retCode="200");
    //int SendMsg(const std::string &strMsg, const std::string &matchRequest="", const std::string &retCode="200");
    void RetGeneralMsg(const std::string &strCode, const std::string &strMsg, const std::string &retCode="200");

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeaderComplete();

    std::string getMySessionId();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnHeader(const std::string& key,const std::string& value);
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


    void Update();

    size_t getMsgNumReceived();
    size_t getMsgNumSent();

private:
    int m_iIsAuthedHost; // 服务器是否在白名单
    CHandlerLiveRecordServer *m_pHandler;
    CConfigLiveRecordServer *m_pCfg;
    CStreamContainer *m_pStreamContainer;
#if defined(_USE_MODULES_LocalDataBase)
    CDbLiveRecord *m_dbaLiveRecord;
#endif //_USE_MODULES_LocalDataBase

    std::string m_strAppKey  ;
    std::string m_strNonce   ;
    std::string m_strCurTime ;
    std::string m_strCheckSum;
    std::string m_strPssSid;

    std::string m_strBody;
    std::string m_requestPrefix;
    std::string m_strShaChkSum;

    unsigned long m_ttOndata;
    unsigned long m_iTimeout;
    long long m_llTheReqTime;

    size_t m_iMsgCountGet;
    size_t m_iMsgCountSend;

    int sendDeviceCtrlMsg(char *ip,
                          const int port,
                          const char *operate_id,
                          const char *value,
                          const u_char value_len,
                          const int32_t session_id);

    //测试接口不鉴权
    //virtual int Do_reqHello( CPssStack &JsonData) ;
    //测试接口鉴权
    //virtual int Do_reqEcho( CPssStack &JsonData) ;

//#if defined(_FUNC_liveRecordTykd)
    int Do_reqLiveRecordTykdStart( CPssStack &JsonData);
    int Do_reqLiveRecordTykdStop( CPssStack &JsonData);
    int Do_reqLiveRecordTykdQuery( CPssStack &JsonData);
    int Do_reqLiveRecordTykdDel( CPssStack &JsonData);
//#endif //#if defined(_FUNC_liveRecordTykd)

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int Do_reqLiveRecordStreamStart( CPssStack &JsonData);
    int Do_reqLiveRecordStreamStop( CPssStack &JsonData);
    int Do_reqLiveRecordStreamQuery( CPssStack &JsonData);
    int Do_reqLiveRecordStreamDel( CPssStack &JsonData);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int onReq_ManAppKeyCreate( CPssStack &JsonData);
    int onReq_ManAppKeyCancel( CPssStack &JsonData);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int onReq_ManConfigMod( CPssStack &JsonData);
};

#endif // _HttpServerLiveRecordServer_H

