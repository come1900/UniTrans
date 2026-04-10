/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * HttpServerPandoraMatrix.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: HttpServerPandoraMatrix.h 5884 2013-01-29 03:20:53Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:20:53  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _HttpServerPandoraMatrix_H
#define _HttpServerPandoraMatrix_H

#include <ezBuffer.h>

#include <HttpdSocket.h>
#include <SocketHandler.h>
#include <ListenSocket.h>
#include <StdoutLog.h>

#include "../../Interface/defGRest.h"
#include "../../Interface/GRestHttpdSocket.h"

//#include "../../Stack/SmsStack.h"
//#define def_MyServiceStack_Of_Pandora CSmsStack

//#include "../../Stack/pss/PssStack.h"
#include "../../Stack/pq/PqStack.h"

#include "../StreamContainer.h"
#include "../PandoraDef.h"

//#define def_MyServiceStack_Of_Pandora CPssStack // in def

class CHandlerPandoraMatrix;
//class CConfigPandoraMatrix;

class CHttpServerPandoraMatrix : public CGRestHttpdSocket// : public HttpdSocket
{
public:
    CHttpServerPandoraMatrix(ISocketHandler& h);
    ~CHttpServerPandoraMatrix();

    void Init();

    void Exec();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    //void CreateHeader(size_t ContentLength=0, const std::string &matchRequest="");

    //void RetGeneralMsg();
    //void RetGeneralMsg(const int retCode=-1, const std::string &strMsg="", const std::string &matchRequest="");

    virtual void OnData(const char *,size_t);
    virtual void OnDataComplete();
    virtual void OnHeader(const std::string& key,const std::string& value);
    virtual void OnHeaderComplete();
    virtual void OnUnknowRequest();

    virtual bool ReadyNode();

    std::string getMySessionId();

    const std::string &getGroupID() const;
    const std::string &getNodeID() const;
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    virtual void OnAccept();
    virtual void OnDisconnect();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    void Update();

    size_t getMsgNumReceived();
    size_t getMsgNumSent();

    //int parseStr2Stack(std::string &strMsg, CPssStack &stackMsg);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int Do_MatchReq(const std::string &strUrl, const std::string &strReq);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 2018-08-01 8:49:59
#if defined(_FUNC_LiveInclusion)
    int On_Req_Psso_DeviceAdd( CPqStack &JsonData);
    int On_Req_Psso_DeviceCancel( CPqStack &JsonData);
    int On_Req_Psso_ValueGet( CPqStack &JsonData);
    int On_Req_Psso_ValueSet( CPqStack &JsonData);
#endif //#if defined(_FUNC_LiveInclusion)

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int On_Req_Hqtt1_Pub_IO_Out();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //  用户命令
    int On_Req_Hqtt1_Pub_ShareHID();
    //node返回
    int On_Req_Hqtt1_Pub_ShareHIDAck();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int On_Req_Hqtt1_Get_Value( CPqStack &JsonData) ;

    int On_Req_PandoraQ_Req_GetValueList( CPqStack &JsonData) ;
    int On_Req_PandoraQ_Req_SetValueList( CPqStack &JsonData) ;

    int On_Req_PandoraQ_Req_SetPlanSchedule(CPqStack &JsonData);
    int On_Req_PandoraQ_Req_GetPlanSchedule(CPqStack &JsonData);
    int On_Req_PandoraQ_Req_PubTaskStart   (CPqStack &JsonData);
    int On_Req_PandoraQ_Req_PubTaskStop    (CPqStack &JsonData);
    int On_Req_PandoraQ_Req_GetTaskList  (CPqStack &JsonData);
    int On_Req_PandoraQ_Req_GetTaskDetails  (CPqStack &JsonData);

    int On_Req_PandoraQ_Req_GetDeviceList  (CPqStack &JsonData);

    int On_Req_PandoraQ_Req_SetShpc (CPqStack &JsonData);
    int On_Req_PandoraQ_NodReq_SetShpc(CPqStack &JsonData);//Nod2Ma
    int On_Req_PandoraQ_Req_GetShpc (CPqStack &JsonData);
    int On_Req_PandoraQ_NodReq_GetShpc(CPqStack &JsonData);//Nod2Ma
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    int On_Req_PandoraQ_Req_Pub_Value( CPqStack &JsonData) ;
    //数据采集上报
    int On_NodReq_Hqtt1_Pub_Value( CPqStack &JsonData) ;

    // task执行结果
    int On_NodReq_Hqtt1_PubTaskStart( CPqStack &JsonData) ;

private:
    int m_iIsAuthedHost; // 服务器是否在白名单
    CConfigPandoraMatrix *m_pPssCfg;
    std::string m_strTransferEncoding; // body

    // APP AUTH
    std::string m_strAppKey  ;
    std::string m_strNonce   ;
    std::string m_strCurTime ;
    std::string m_strCheckSum;
    std::string m_strPssSid;
    std::string m_strAppSecret;

    // 目前是 AppKey
    std::string m_strGroupID;
    std::string m_strNodeID;

    //添加values的首部关键字
    std::string m_strKeyFrontOfValues;

    CezBuffer *m_pBodyBuf;

    std::string m_requestPrefix;
    std::string m_sessionId;
    //int reportId;

    unsigned long m_ttOndata;
    // 本实例自用
    unsigned long m_iTimeout;

    size_t m_iMsgCountGet;
    size_t m_iMsgCountSend;

    // 长连接， 单独实现
    CEZMutex m_MutexSendMsg;
    int SendMsg(const std::string &strMsg, const std::string &matchRequest);
    void RetGeneralMsg(const std::string &strCode, const std::string &strMsg);

    // get a new
    std::string generateSessionId();

    //测试接口
    int Do_reqHello( CPqStack &JsonData) ;
    int Do_reqEcho(const std::string &strReq);

    int onReq_ManAppKeyCreate( CPqStack &JsonData);
    int onReq_ManAppKeyCancel( CPqStack &JsonData);

    //4.1	鉴权
    int On_requestAuth( CPqStack &JsonData);
    //4.3	心跳
    int On_requestHeartbeat( CPqStack &JsonData);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CHandlerPandoraMatrix *m_pHandlerPandoraMatrix;
};

#endif // _HttpServerPandoraMatrix_H

