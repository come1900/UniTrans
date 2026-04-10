/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveStreammingServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveStreammingServer.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerLiveStreammingServer_H
#define _HttpServerLiveStreammingServer_H

//#include <HttpdSocket.h>
#include <RestfulHttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>

#include "../Interface/defGRest.h"
#include "../Interface/GRestHttpdSocket.h"

#include "../Stack/pss/PssStack.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveStreammingServer.h"
#include "../../Configs/ConfigPullTykdMobileService.h"
#endif //_USE_MODULES_EZCONFIG

class CHandlerLiveStreammingServer;
class CHttpServerLiveStreammingServer : public CGRestHttpdSocket
{
public:
    CHttpServerLiveStreammingServer(ISocketHandler& h);
    ~CHttpServerLiveStreammingServer();

    void Init();

    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    //void CreateHeader(size_t ContentLength=0, const std::string &matchRequest="");

    //void RetGeneralMsg(const int retCode=-1, const std::string &strMsg="", const std::string &matchRequest="");

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeaderComplete();
    virtual void OnUnknowRequest();

    std::string getMySessionId();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnHeader(const std::string& key,const std::string& value);
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    void RetGeneralMsg(const std::string &strCode, const std::string &strMsg, const std::string &retCode);

    void Update();

    size_t getMsgNumReceived();
    size_t getMsgNumSent();

private:
    std::string m_strAppKey  ;
    std::string m_strNonce   ;
    std::string m_strCurTime ;
    std::string m_strCheckSum;
    std::string m_strPssSid;

    std::string m_strBody;
    CHandlerLiveStreammingServer *m_pHandler;
    CConfigLiveStreammingServer *m_pPssCfg;
    CConfigPullTykdMobileService *m_pPullCfg;

    unsigned long m_ttOndata;
    unsigned long m_iTimeout;

    size_t m_iMsgCountGet;
    size_t m_iMsgCountSend;

    int onReq_Option( CPssStack &JsonData) ;
    int onReq_ManAppKeyCreate( CPssStack &JsonData);
    int onReq_ManAppKeyCancel( CPssStack &JsonData);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int onReq_ManConfigMod( CPssStack &JsonData);
#ifdef _FUNC_gMiniLicense
    int onReq_getGminiLicense( CPssStack &JsonData);
    int onReq_setGminiLicense( CPssStack &JsonData);
#endif //_FUNC_gMiniLicense

    int Do_reqPullTykd2HlsStart( CPssStack &JsonData)						;
    int Do_reqPullTykd2HlsStop( CPssStack &JsonData)						;

    int Do_reqPullTykd2RtmpStart( CPssStack &JsonData)						;
    int Do_reqPullTykd2RtmpStop( CPssStack &JsonData)						;

#if defined(_FUNC_LiveStreammingTykd2Rtsp)
    int Do_reqPullTykd2RtspStart( CPssStack &JsonData)						;
    int Do_reqPullTykd2RtspStop( CPssStack &JsonData)						;
#endif //_FUNC_LiveStreammingTykd2Rtsp

    int Do_reqPullStream2HlsStart( CPssStack &JsonData)						;
    int Do_reqPullStream2HlsStop( CPssStack &JsonData)						;

    int Do_reqPullStream2RtmpStart( CPssStack &JsonData)						;
    int Do_reqPullStream2RtmpStop( CPssStack &JsonData)						;
    int Do_reqPullStream2RtmpList( CPssStack &JsonData)						;

    int Do_reqPullStream2RtspStart( CPssStack &JsonData)						;
    int Do_reqPullStream2RtspStop( CPssStack &JsonData)						;

    int Do_reqPullStream2FlvStart( CPssStack &JsonData)						;
    int Do_reqPullStream2FlvStop( CPssStack &JsonData)						;
    //int Do_reqDeliveryStreamStart( CPssStack &JsonData) ;
    //int Do_reqDeliveryStreamStop( CPssStack &JsonData) ;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int chkValidDevChStreamPara(const std::string &strDevID, const std::string &strChannel, const std::string &strStream);

#if defined(_FUNC_LiveStreammingTykd2Hls) && !defined (def_Use_Unify_startPullStream_Interface)
    int onReq_NvpShareCam2WeChat( CPssStack &JsonData) ;
    int onReq_NvpShareCam2WeChatStop( CPssStack &JsonData) ;
    int onReq_NvpShareCam2WeChatQuery( CPssStack &JsonData) ;
#endif
private:
    //std::string m_strRetCode;
};

#endif // _HttpServerLiveStreammingServer_H

