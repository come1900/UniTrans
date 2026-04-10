/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PandoraNodeKeepConnector.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PandoraNodeKeepConnector.h 5884 2012-07-10 04:10:20Z WuJunjie $
 *
 *  Explain:
 *     -
 *      http 连接器，
 *      保持连接，断开后自动再次连接
 *     -
 *
 *  Update:
 *     2012-07-10 04:10:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _PandoraNodeKeepConnector_H
#define _PandoraNodeKeepConnector_H

#include <ezBuffer.h>

#include <HTTPSocket.h>
//#include <ISocketHandler.h>
#include "../../../UniTransDef.h"

#include "../../Stack/pq/PqStack.h"

#include "../../UniService/DefUniService.h"
#include "../PandoraDef.h"

#include "../../../Configs/ConfigGeneral.h"
#include "../../../Configs/ConfigPandoraNode.h"
#include "../../../Configs/ConfigShp.h"

//#include "../StreamContainer.h"

#define CPandoraNodeKeepConnector_SOCKET_NAME "PandoraNode"
class CHandlerPandoraNode;


class CPandoraNodeKeepConnector : public HTTPSocket//,public CEZThread
{
public:
    CPandoraNodeKeepConnector(ISocketHandler&, std::string strSocketName=CPandoraNodeKeepConnector_SOCKET_NAME);
    ~CPandoraNodeKeepConnector();

    /////////////////////////////
    // 加入命令解析
    virtual void OnConnect();
    virtual void OnDisconnect();
    virtual void OnConnectTimeout();
    /////////////////////////////
		
    std::string getMySessionId();
    int SendMsg(const std::string &strUri, const std::string &strMsg, const std::string &matchRequest="");

    void Update();
    bool IsLogined();
    bool IsOnData();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 登录
    int ICP_requestAuth();
    int onMatch_requestAuthACK(CPqStack &JsonData);
    // 心跳
    int ICP_requestHeartbeat();


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //int Do_MatchReq_Hqtt1_Publish(const std::string &strReq);

    int Do_MatchReq_Hqtt1_Publish_IO_Out(CPqStack &pqStackReq);
    int Do_MatchAck_Hqtt1_PubTaskStart(CPqStack &pqStackReq);
    int Do_MatchReq_PandoraQ_MasReq_ShpSet(CPqStack &pqStackReq);

    int Do_MatchReq_PandoraQ_MasReq_ShpGet(CPqStack &pqStackReq);

    int Do_MatchReq_Hqtt1_ShareHID(CPqStack &pqStackReq);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 采集类
    int Do_Req_Hqtt1_PublishValue();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    

    void OnFirst();
    void OnHeader(const std::string& key,const std::string& value);
    void OnHeaderComplete();
    void OnData(const char *buf, size_t sz);
    void OnDataComplete();

private:
    // 重置变量
    //void Renew();

    std::string m_sessionId;

    unsigned long m_tOndata;///< last Time in seconds when this socket was on data

    // 当前消息状态-- 状态机
    int m_enCurMsgStatus;

    // 发送接收都算, 用来标记最后消息
    unsigned long m_tSndMsg;
    unsigned long m_tOnMsg;
    unsigned long m_tMsgTimeOut; // 命令超时时间

    //std::string m_strShaChkSum;
    std::string m_strCheckSum; // 从服务器返回， 用户数据
    std::string m_strAppSecret;
    std::string m_strNodeId;
    std::string m_strSerial;

    bool m_bHaveMatchRequest;
    std::string m_strMatchRequest;

    unsigned int m_iSentPacket;

    //std::string m_strBody;
    CezBuffer *m_pBodyBuf;

    //std::string m_strTransferEncoding; // body

    // 心跳、能力信息是否已发送
    int m_iStatusSended;

    CHandlerPandoraNode *m_pHandler;
    CConfigPandoraNode *m_pCfgPandoraNode;
    CConfigShp *m_pCfgShp;
};

#endif // _PandoraNodeKeepConnector_H
