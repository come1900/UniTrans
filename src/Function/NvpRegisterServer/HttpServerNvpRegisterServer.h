/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerNvpRegisterServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerNvpRegisterServer.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerNvpRegisterServer_H
#define _HttpServerNvpRegisterServer_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "../Stack/pss/PssStack.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigNvpRegisterServer.h"
#endif //_USE_MODULES_EZCONFIG
class CStreamContainer;
class CHandlerNvpRegisterServer;
class CHttpServerNvpRegisterServer : public HttpdSocket, public CEZObject
{
public:
    CHttpServerNvpRegisterServer(ISocketHandler& h);
    ~CHttpServerNvpRegisterServer();

    void Init();

    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    void CreateHeader(size_t ContentLength=0, const std::string &matchRequest="");

    void RetGeneralMsg(const int retCode=-1, const std::string &strMsg="", const std::string &matchRequest="");
    void RetGeneralMsg(const std::string &strCode, const std::string &strMsg);

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeaderComplete();

    std::string getMySessionId();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnHeader(const std::string& key,const std::string& value);
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int SendMsg(const std::string &strMsg, const std::string &matchRequest="");

    void Update();

    // 注册处理回调
    void OnRegistered(int iCode, void *pData);

private:
    int m_iIsAuthedHost; // 服务器是否在白名单
    CConfigNvpRegisterServer *m_pCfg;
    CStreamContainer *m_pStreamContainer;

    std::string m_strAppKey  ;
    std::string m_strNonce   ;
    std::string m_strCurTime ;
    std::string m_strCheckSum;
    std::string m_strPssSid;

    std::string m_strSetCookie;

    std::string m_strBody;
    std::string m_requestPrefix;
    std::string m_strShaChkSum;

    unsigned long m_ttOndata;
    unsigned long m_iTimeout;
    long long m_llTheReqTime;
#if 0
    //size_t m_iMsgCountGet;
    //size_t m_iMsgCountSend;

    int m_iCfgRandChannelID;
    std::string m_strPubPushRTMP;//= DEFAULT_RTMP_URL;
    std::string m_strPubPushRTSP;//= DEFAULT_RTSP_URL;

    std::string m_strUrlPrefixHLS ;//= DEFAULT_HLS_URL_HEADER;
    std::string m_strUrlPrefixRTMP ;//= DEFAULT_RTMP_URL;
    std::string m_strUrlPrefixRTSP ;//= DEFAULT_RTSP_URL;

    int sendDeviceCtrlMsg(char *ip,
                          const int port,
                          const char *operate_id,
                          const char *value,
                          const u_char value_len,
                          const int32_t session_id);
#endif
    //测试接口
    int Do_TestIF(const std::string &strReq) ;

    // 单机模式
    int onReq_NvpTRegister(const std::string &strReq);
    int onReq_NvpTermRegister(const std::string &strReq);
    int onReq_NvpTermInfo(const std::string &strReq);
	
    // 账号管理
    int onReq_NvpAccountCreate(const std::string &strReq);
    int onReq_NvpAccountMod( const std::string &strReq) ;

    CHandlerNvpRegisterServer *m_pHandler;
    CStreamContainer *m_pContainerChannel;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    //std::string m_strProcMsgTocken;
    unsigned long m_ttOnProcMsg;
    //CStreamMsg m_StreamProcMsg;
    std::string m_strStreamID;

};

#endif // _HttpServerNvpRegisterServer_H

