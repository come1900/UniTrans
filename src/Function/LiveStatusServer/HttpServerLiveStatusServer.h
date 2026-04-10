/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerLiveStatusServer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerLiveStatusServer.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerLiveStatusServer_H
#define _HttpServerLiveStatusServer_H

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "../Stack/pss/PssStack.h"

#include "../../Configs/ConfigLiveStatusServer.h"

class CStreamContainer;
class CHandlerLiveStatusServer;
class CHttpServerLiveStatusServer : public HttpdSocket
{
public:
    CHttpServerLiveStatusServer(ISocketHandler& h);
    ~CHttpServerLiveStatusServer();

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
    //int m_iIsAuthedHost; // 服务器是否在白名单
    CHandlerLiveStatusServer *m_pHandler;
    //CConfigLiveStatusServer *m_pCfg;
    //CStreamContainer *m_pStreamContainer;

    std::string m_strAppKey  ;
    std::string m_strNonce   ;
    std::string m_strCurTime ;
    std::string m_strCheckSum;
    std::string m_strPssSid;

    std::string m_strBody;
    std::string m_requestPrefix;
    //std::string m_strShaChkSum;

    unsigned long m_ttOndata;
    unsigned long m_iTimeout;
    long long m_llTheReqTime;

    size_t m_iMsgCountGet;
    size_t m_iMsgCountSend;

    //测试接口
    int Do_TestIF( CPssStack &JsonData) ;

    // pss notify msg
    int Do_reqPssNotify( CPssNotifyMsg &PssNotifyMsg) ;

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

};

#endif // _HttpServerLiveStatusServer_H

