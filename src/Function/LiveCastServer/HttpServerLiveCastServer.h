/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveCastServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveCastServer.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerLiveCastServer_H
#define _HttpServerLiveCastServer_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "../Stack/pss/PssStack.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveCastServer.h"
#endif //_USE_MODULES_EZCONFIG
#if defined(_USE_MODULES_LocalDataBase)
#include "../../DBAgent/DbLiveCast.h"
#endif // _USE_MODULES_LocalDataBase

class CStreamContainer;
class CHandlerLiveCastServer;
class CHttpServerLiveCastServer : public HttpdSocket
{
public:
    CHttpServerLiveCastServer(ISocketHandler& h);
    ~CHttpServerLiveCastServer();

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

    size_t getMsgNumReceived();
    size_t getMsgNumSent();

private:
    int m_iIsAuthedHost; // 服务器是否在白名单
    CConfigLiveCastServer *m_pCfg;
    CStreamContainer *m_pStreamContainer;

#if defined(_USE_MODULES_LocalDataBase)
    CDbLiveCast *m_dbaLiveRecord;
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

    //测试接口
    int Do_TestIF( CPssStack &JsonData) ;

    int CreateStreamUrl( \
		const std::string &strName
		, std::string &strPushRtmp
		, std::string &strPushRtsp
		, std::string &strPlayHls
		, std::string &strPlayRtmp
		, std::string &strPlayRtsp);

    // 单机模式
    int onReq_pssChannelCreate_Single(const std::string &strReq);
    int onReq_pssChannelCancel_Single(const std::string &strReq);
    int onReq_pssChannelQuery_Single(const std::string &strReq);
    int onReq_pssChannelList_Single(const std::string &strReq);

    CHandlerLiveCastServer *m_pHandler;
    CStreamContainer *m_pContainerChannel;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    //std::string m_strProcMsgTocken;
    unsigned long m_ttOnProcMsg;
    //CStreamMsg m_StreamProcMsg;
    std::string m_strStreamID;

};

#endif // _HttpServerLiveCastServer_H

