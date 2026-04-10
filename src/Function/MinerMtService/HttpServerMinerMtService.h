/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerMinerMtService.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: HttpServerMinerMtService.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerMinerMtService_H
#define _HttpServerMinerMtService_H
#include <ezBuffer.h>

// #include <HttpdSocket.h>
#include <RestfulHttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>
#include "../Interface/defGRest.h"
#include "../Interface/GRestHttpdSocket.h"

#include "../Stack/promQL/promQLStack.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigMinerMtService.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG
//class CStreamContainer;
class CHandlerMinerMtService;
class CHttpServerMinerMtService : public CGRestHttpdSocket
{
public:
    CHttpServerMinerMtService(ISocketHandler& h);
    ~CHttpServerMinerMtService();

    void Init();

    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    void CreateHeader(size_t ContentLength=0, const std::string &matchRequest="");

    // void RetGeneralMsg(const int retCode=-1, const std::string &strMsg="", const std::string &matchRequest="");
    void RetGeneralMsg(const std::string &strCode, const std::string &strMsg);

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeaderComplete();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnHeader(const std::string& key,const std::string& value);
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // int SendMsg(const std::string &strMsg, const std::string &matchRequest="");

    void Update();

private:
    int m_iIsAuthedHost; // 服务器是否在白名单
    CConfigMinerMtService *m_pCfgMinerMtService;
    CHandlerMinerMtService *m_pHandler;

    std::string m_strAppKey  ;
    std::string m_strNonce   ;
    std::string m_strCurTime ;
    std::string m_strCheckSum;
    std::string m_strUniSid;

    CezBuffer *m_pBodyBuf;
    std::string m_requestPrefix;
    std::string m_strShaChkSum;

    unsigned long m_ttOndata;
    unsigned long m_iTimeout;
    long long m_llTheReqTime;

    //测试接口
    //int Do_TestIF( CpromQLStack &JsonData) ;
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 获取主机检测结果
    int On_Req_MinerMtService_mt_query_PromQlValue( CpromQLStack &restStack, const std::string strValueType);

    // 开启检测
    int On_Req_MinerMtService_mt_dialing_ipcam_start( CpromQLStack &restStack);
     // 检测结果查询
    int On_Req_MinerMtService_mt_query_ipcam_dialing( CpromQLStack &restStack);
     // 检测结果查询
    int On_Req_MinerMtService_mt_cut_ipcam_dialing( CpromQLStack &restStack);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    unsigned long m_ttOnProcMsg;
    std::string m_strStreamID;
};

#endif // _HttpServerMinerMtService_H

