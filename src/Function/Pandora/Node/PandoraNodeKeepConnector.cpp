/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PandoraNodeKeepConnector.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: PandoraNodeKeepConnector.cpp 5884 2013-12-18 03:38:04Z WuJunjie $
 *
 *  Notes:
 *     -
 *      支持多实例 
 *     -
 *
 *  Update:
 *     2013-12-18 03:38:04 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include <string>
#include <unistd.h> /* access */
#include <sstream>
#include <ez_util_platform.h>
#include <str_opr.h>
#include <ez_bit.h>
#include <sha1.h>
#include <ez_url_parser.h>
#include "StringOpr.h"
#include "ezUtilCPP.h"// ez_ZAB_encode

#include "../../../Logs.h"
#include "../../../Solar.h"

#ifdef _FUN_FunSerial
#include "../../GPorts/FunSerial.h"
#endif // _FUN_FunSerial

#include "../PandoraDef.h"
#include "shpcConfig.h"

#include "HandlerPandoraNode.h"
#include "PandoraNodeKeepConnector.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

//#define def_SimDataForVen 1//模拟数据

//事务处理状态
typedef enum en_icp_logic_status
{
    enAppLogicStatus_Begin  =  0,
    enAppLogicStatus_Logining  =  1,
    enAppLogicStatus_Logined  =  2,

    enAppLogicStatus_Heartbeating,
    enAppLogicStatus_Heartbeated,

    enAppLogicStatus_Exiting,
    enAppLogicStatus_End,
}enAppLogicStatus_PHKC;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

// 登录重登陆次数 超过则重建
// 有些设备需要重连
#define LOGIN_COUNT_DOWN 3

//////////////////////////////////////////////////
CPandoraNodeKeepConnector::CPandoraNodeKeepConnector(ISocketHandler& h, std::string strSocketName)
        :HTTPSocket(h)
        //,CEZThread(CPandoraNodeKeepConnector_THREAD_ATTR)
        //,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(2);

    SetDeleteByHandler();

    SetSockName(strSocketName);

    //DisableInputBuffer();

    m_tOndata = 0;

    m_enCurMsgStatus = enAppLogicStatus_Begin;

    m_tSndMsg = 0;//time(NULL);
    m_tOnMsg = 0;//time(NULL);
    m_tMsgTimeOut = TCP_MSGG_TIMEOUT;

    m_iSentPacket = 0;
    //m_strBody = "";
    m_pBodyBuf = new CezBuffer();
    m_strMatchRequest="";

    m_iStatusSended = 0;

    //统一管理
    m_pHandler = ((CHandlerPandoraNode *)(&(Handler())));

    m_pCfgPandoraNode = new CConfigPandoraNode;
    assert(m_pCfgPandoraNode);

    m_pCfgShp = new CConfigShp;
    assert(m_pCfgShp);

    m_strNodeId = "";

    m_strAppSecret  = "ebd69a6ccd6f81ef4e6c52e37761750b42f0fe50"; //get appSecret self

    //m_strTransferEncoding = APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB;
}

CPandoraNodeKeepConnector::~CPandoraNodeKeepConnector()
{
    delete m_pCfgPandoraNode;
    delete m_pCfgShp;

    if (m_pBodyBuf != NULL)
    {
        delete m_pBodyBuf;
    }
}

void CPandoraNodeKeepConnector::OnConnect()
{
    ez_printf_debug("::OnConnect() - %ld\n", time(NULL));

    //m_strShaChkSum = "043979497";// dbg dev huid

#ifdef _USE_MODULES_EZCONFIG

    m_pCfgPandoraNode->update();
    m_pCfgShp->update();

    m_strNodeId = m_pCfgPandoraNode->getConfig().strNodeName;

    if (m_strNodeId.length() < def_Min_Node_Name_Length)
    {
        m_strNodeId = "G1"+g_Solar.GetHuid();

        m_pCfgPandoraNode->getConfig().strNodeName = m_strNodeId;
        m_pCfgPandoraNode->commit();
    }
#else
    m_strNodeId = "G1"+g_Solar.GetHuid();
    //m_strNodeId = g_Solar.GetHuid();
#endif

    //m_strShaChkSum = g_Solar.GetHuid();

    if (m_pBodyBuf == NULL)
    {
        ez_printf_emerg("Low memory\n");
        SetCloseAndDelete();
        return;
    }
    m_pBodyBuf->SetIncreaseSize(1024);
}

void CPandoraNodeKeepConnector::OnDisconnect()
{
    ez_printf_info("::OnDisconnect() - %ld\n", time(NULL));
}

void CPandoraNodeKeepConnector::OnConnectTimeout()
{
    ez_printf_info("::OnConnectTimeout()\n");
}
#if 0
void CPandoraNodeKeepConnector::Renew()
{
    //m_strBody = "";
    //m_pBodyBuf->Reset();
    m_strMatchRequest="";
    Reset();
}
#endif
void CPandoraNodeKeepConnector::OnFirst()
{}
void CPandoraNodeKeepConnector::OnHeader(const std::string& key,const std::string& value)
{
    if (!strcasecmp(key.c_str(),"Match-request"))
    {
        m_strMatchRequest = value;
    }
    //needed by zab
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_CheckSum))
    {
        m_strCheckSum = value;

        DBG(ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_CheckSum, m_strCheckSum.c_str()););
    }
#if 0
    else if (!strcasecmp(key.c_str(),APP_AUTH_HTTP_HEAD_KEY_TransferEncoding))
    {
        m_strTransferEncoding = value;

        ez_printf_debug("%s:%s\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding, m_strTransferEncoding.c_str());
    }
#endif
    else
    {
        //HTTPSocket::OnHeader(key, value);
    }
}

void CPandoraNodeKeepConnector::OnHeaderComplete()
{
    m_pBodyBuf->Reset();
}

void CPandoraNodeKeepConnector::OnData(const char *p, size_t l)
{
    if (p==NULL || l<1)
    {
        ez_printf_error("Data not valid\n");
        return;
    }

#if 0
    //ez_printf_debug("%s\n", __FUNCTION__);

    if (m_pBodyBuf->Size()+l > def_Pandora_MaxMsgLen)
    {
        ez_printf_error("body(%lu), l(%lu) too long\n", m_strBody.length(), l);
        //std::string strBadboy = "msg too long.";

        Renew();

        return;
    }

    m_strBody += p;
#else

    DBG(ez_printf_debug("%s:BufL:%ld,OnDataL:%ld\n", __FUNCTION__, m_pBodyBuf->Size(), l););

    if (m_pBodyBuf->Size()+l > def_Pandora_MaxMsgLen)
    {
        ez_printf_error("body(%lu), l(%lu) too long\n", m_pBodyBuf->Size(), l);

        Reset();
        m_pBodyBuf->Reset();

        return;
    }

    m_pBodyBuf->Append((unsigned char *)p, l);
#endif

    m_tOndata = ez_sec_get_tick_count();
}

void CPandoraNodeKeepConnector::OnDataComplete()
{
    unsigned char theTail[1] = {'\0'};
    m_pBodyBuf->Append(theTail, 1);
    //std::string m_strBody;
    const char *pBody = NULL;//m_strBody.c_str();
    StlVecUnChar decBuff;

    //ez_printf_debug("m_strTransferEncoding:[%s]\n", m_strTransferEncoding.c_str());

    do
    {
        DBG(ez_printf_debug("Body(%d):*\n", m_pBodyBuf->Size()););

        DBG(ez_printf_debug("on %s.\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_ZAB););

        //m_strBody不赋值，则在后面统一检查长度的时候返回PssMsg_Value_Result_InvalidData
        //m_strBody.clear();

        if (m_strCheckSum.length() < 30/*40*/)
        {
            ez_printf_error("no CheckSum for eZAB\n");

            break;
        }

        StlVecUnChar encBuff(m_pBodyBuf->Buf(), m_pBodyBuf->Buf()+m_pBodyBuf->Size());
        //StlVecUnChar decBuff;

        std::string strSecret;
        CezUtilCPP::stringMergeAlternately(strSecret, m_strCheckSum, m_strAppSecret);

        int iret_dec = CezUtilCPP::ez_ZAB_decode(encBuff, decBuff, strSecret);
        if (0 != iret_dec)
        {
            //std::cout << "ez_ZAB_decode Failed:" << iret_dec << std::endl;
            ez_printf_error("ez_ZAB_decode Failed:%d\n", iret_dec);
            break;
        }

        if (decBuff.size()>def_HTTPD_MSGG_BodyMaxSize
            || strlen((char *)&decBuff[0]) > def_HTTPD_MSGG_BodyMaxSize)
        {
            ez_printf_error("decBuff.size()>def_HTTPD_MSGG_BodyMaxSize.\n");

            break;
        }

        //m_strBody = (char *)(&decBuff[0]);
        pBody = (char *)(&decBuff[0]);
        //}
        //else //APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null
        //{
        //    ez_printf_debug("%s.\n", APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_Null);

        //    m_strBody = (char *)m_pBodyBuf->Buf();//((const unsigned char*)m_pBodyBuf->Buf(), m_pBodyBuf->Size());
        //}

        //ez_printf_info("strBody(%d):\n%s\n", m_strBody.size(), m_strBody.c_str());
        DBG(ez_printf_debug("strBody(%d):*\n", decBuff.size()););
    }
    while(0);

    //const char *pBody = m_strBody.c_str();
    //char *pBody = (char *)m_pBodyBuf->Buf();

    do
    {
        if (NULL == pBody || *pBody == '\0')
        {
            ez_printf_error("pBody is NULL.\n");

            break ;
        }

        //ez_printf_debug("pBody:%s\n", pBody);

        CPqStack __CezlibsWebServiceIF;

        bool bRet = __CezlibsWebServiceIF.Parse(pBody);

        if (bRet == false)
        {
            ez_printf_error("Failed parse.\n");

            break ;
        }

        m_tOnMsg = ez_sec_get_tick_count();//time(NULL);

        DBG(ez_printf_debug("MatchRequest:%s\n", m_strMatchRequest.c_str()););

        if (m_strMatchRequest==def_Pandora_requestHeartbeat)
        {
            DBG(ez_printf_debug("Heartbeated.\n"););

            m_enCurMsgStatus = enAppLogicStatus_Heartbeated;
        }
        else if (m_strMatchRequest==def_Pandora_requestAuth)
        {
            onMatch_requestAuthACK(__CezlibsWebServiceIF);
        }
        else if (m_strMatchRequest==def_PandoraQ_MasReq_Publish_IO_Out)
        {
            Do_MatchReq_Hqtt1_Publish_IO_Out(__CezlibsWebServiceIF);
        }
        else if (m_strMatchRequest==def_PandoraQ_MasReq_Publish_HID_Send)
        {
            Do_MatchReq_Hqtt1_ShareHID(__CezlibsWebServiceIF);
        }
        else if (m_strMatchRequest==def_PandoraQ_MasReq_PubTaskStart)
        {
            Do_MatchAck_Hqtt1_PubTaskStart(__CezlibsWebServiceIF);
        }
        else if (m_strMatchRequest==def_PandoraQ_MasReq_ShpSet)
        {
            Do_MatchReq_PandoraQ_MasReq_ShpSet(__CezlibsWebServiceIF);
        }
        else if (m_strMatchRequest==def_PandoraQ_MasReq_ShpGet)
        {
            Do_MatchReq_PandoraQ_MasReq_ShpGet(__CezlibsWebServiceIF);
        }
        else if (m_strMatchRequest==def_PandoraQ_MasAck_PublishValue)
        {
            ez_printf_debug("MasAck_PublishValue.\n");
        }
        else
        {
            //ez_printf_error("Unknow MatchRequest:%s, Body:%s\n", m_strMatchRequest.c_str(), m_pBodyBuf->Buf());
        }
    }
    while(0);

    Reset(); // prepare for next request

    return;
}

std::string CPandoraNodeKeepConnector::getMySessionId()
{
    return m_sessionId;
}

void CPandoraNodeKeepConnector::Update()
{
    unsigned long ttNow = ez_sec_get_tick_count();//time(NULL);

    int iret = -1;
    ARG_USED(iret);

    if(!IsConnected())
    {
        return;
    }

    // 连接之后登录
    if (m_enCurMsgStatus<enAppLogicStatus_Logining)
    {
DBG(
        ez_printf_debug("Login ...\n");
);
        //初生
        m_tOnMsg = ez_sec_get_tick_count();//time(NULL);

        iret = ICP_requestAuth();

        return;
    }

    // 登录超时
    if (m_enCurMsgStatus==enAppLogicStatus_Logining && (ttNow-m_tOnMsg>m_tMsgTimeOut))
    {
        m_enCurMsgStatus=enAppLogicStatus_Begin;

        ez_printf_error("Login timeout, CountDown:%d\n"
                        , LOGIN_COUNT_DOWN-(m_iSentPacket/* - m_iRecvPacket*/)
                       );

        if (m_iSentPacket >= (/*m_iRecvPacket+*/LOGIN_COUNT_DOWN) )
        {
            // 多于10个包没有反馈也重启， 防止连接建立，但是没有反馈的情况
            // 在网络型信号机中发现此情况
            ez_printf_error("Login countdown.\n");
            SetCloseAndDelete();
        }

        return;
    }

    // 命令超时
    //if (m_enCurMsgStatus==enAppLogicStatus_Heartbeating && (ttNow-m_tOnMsg>m_tMsgTimeOut))
    if (m_enCurMsgStatus==enAppLogicStatus_Heartbeating && (ttNow-m_tSndMsg>m_tMsgTimeOut))
    {
        m_enCurMsgStatus = enAppLogicStatus_Logined;

        ez_printf_debug("Heartbeat timeout.\n");

        return;
    }


    // 心跳
    unsigned long hbPeriod = 1;
#ifdef _USE_MODULES_EZCONFIG

    hbPeriod = m_pCfgPandoraNode->getConfig().CheckPeriod;

    if (hbPeriod>(def_Pandora_ConnTimeout/2.0 - 0.5) )
    {
        ez_printf_warning("hbPeriod(%d) too long\n", hbPeriod);

        hbPeriod = (def_Pandora_ConnTimeout/2.0 - 0.5) ;
    }
#endif//

#if 1
    //ez_printf_debug("m_enCurMsgStatus:%d ttNow-m_tOnMsg:%ld, (hbPeriod*2):%ld\n", m_enCurMsgStatus, ttNow-m_tOnMsg, (hbPeriod*2));
    // 超过两个周期
    if (ttNow-m_tOnMsg > hbPeriod*2)
    {
        ez_printf_error("%d>%d, SetCloseAndDelete\n", (ttNow-m_tOnMsg), (hbPeriod*2));

        //m_enCurMsgStatus=enAppLogicStatus_Begin;

        // 遇到单片机之类的tcpserver;
        // 不返回断开包时的异常
        SetCloseAndDelete();
        // 需要在CAgentPandoraNode::Run()中重连配合

        return;
    }
#endif
#if 0
    if (m_enCurMsgStatus>=enAppLogicStatus_Logined
        && 0 == m_iStatusSended)
    {
        ICP_requestHeartbeat();
        m_iStatusSended = 1;
    }
#endif
    unsigned long tcNow = ez_sec_get_tick_count();
    static unsigned long __lastPublishValue = tcNow;//
    if (m_enCurMsgStatus>=enAppLogicStatus_Logined
        && tcNow-__lastPublishValue > 5)
    {
        Do_Req_Hqtt1_PublishValue();
        __lastPublishValue = tcNow;//
    }

    // 心跳置于最后
    if (m_enCurMsgStatus>=enAppLogicStatus_Logined
        && m_enCurMsgStatus != enAppLogicStatus_Heartbeating /*正在等心跳返回消息*/
        //&&(ttNow-m_tOnMsg)>(m_ttConnectTimeOut/2.0-0.5))
        &&(ttNow-m_tOnMsg)>hbPeriod/*(m_ttConnectTimeOut/2.0-0.5)*/)
    {
        //ez_printf_debug("KeepLive , ICP_requestHeartbeat\n");

        ICP_requestHeartbeat();
    }
#if 0
    //3个心跳周期后断开连接
    if (m_enCurMsgStatus>=enAppLogicStatus_Logined
        &&(ttNow-m_tOnMsg)>(3*hbPeriod))
    {
        ez_printf_error("%d>%d, SetCloseAndDelete\n", (ttNow-m_tOnMsg), (3*hbPeriod));

        SetCloseAndDelete();
    }
#endif
}

// http + json
int CPandoraNodeKeepConnector::SendMsg(const std::string &strUri, const std::string &strMsg, const std::string &matchRequest)
{
    if(!IsConnected())
    {
        return -1;
    }

    //////////////////////////////////////////
    // appkey 认证
    std::string strAppKey  = "701819701"; //get appSecret self
    //std::string strAppSecret  = "ebd69a6ccd6f81ef4e6c52e37761750b42f0fe50"; //get appSecret self
    std::string strNonce   = Utility::l2string((long)ez_sec_get_tick_count()) ;
    std::string strCurTime = Utility::l2string((long)time(NULL)) ;
    std::string strCheckSum = strNonce;

    strCheckSum += m_strAppSecret;
    strCheckSum += strCurTime;

    char dst[SHA1HashSizeDestBuffer/*SHA1HashSize*2+1*/] = {0};
    const char *src = strCheckSum.c_str();
    ARG_USED(src);
    ARG_USED(dst);

    //ez_printf_debug("ez_sha1_str:%s\n", src);
    int iret = ez_sha1_str(dst, sizeof(dst), src );
    ARG_USED(iret);

    strCheckSum = dst;

    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_AppKey, strAppKey );
    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_Nonce, strNonce);
    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_CurTime, strCurTime);
    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_CheckSum, strCheckSum);

    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_PssSid, m_strNodeId);
    m_strCheckSum = strCheckSum;

    StlVecUnChar oriBuff(strMsg.c_str(), strMsg.c_str()+strMsg.length()+1);
    StlVecUnChar encedBuff;

    // 提高安全性
#if 1
    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_TransferEncoding, APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_ZAB);

    std::string strSecret;
    CezUtilCPP::stringMergeAlternately(strSecret, strCheckSum, m_strAppSecret);

    int iret_enc = CezUtilCPP::ez_ZAB_encode(oriBuff, encedBuff, strSecret);
#else
    AddResponseHeader( APP_AUTH_HTTP_HEAD_KEY_TransferEncoding, APP_AUTH_HTTP_HEAD_KEY_TransferEncoding_eZAB);
    int iret_enc = CezUtilCPP::ez_ZAB_encode(oriBuff, encedBuff, strCheckSum);
#endif

    if (0 != iret_enc)
    {
        ez_printf_error("ez_ZAB_encode Failed:%d\n",  iret_enc);
        //std::cout << "ez_ZAB_encode Failed:" << iret_enc << std::endl;
        return -100;
    }
    std::string __strMsg((char *)(&(encedBuff[0])), encedBuff.size());

    DBG(ez_printf_debug("%s(%ld):*\n",  "SendMsg", strMsg.size()););
    DBG(ez_printf_debug("%s(%ld):*\n",  "Send", __strMsg.size()););

    //////////////////////////////////////////
    if (matchRequest.size()>0)
    {
        AddResponseHeader( "Match-request", matchRequest);
    }
    SetUrl(strUri);
    SetMethod("POST");
    SetHttpVersion( "HTTP/1.1" );
    AddResponseHeader( "Content-length", Utility::l2string((long)__strMsg.size()) );

    SendRequest();//head
    Send(__strMsg);

    m_tSndMsg = ez_sec_get_tick_count();//time(NULL);

    return 0;
}

bool CPandoraNodeKeepConnector::IsLogined()
{
    return m_enCurMsgStatus>=enAppLogicStatus_Logined;
}

bool CPandoraNodeKeepConnector::IsOnData()
{
    return m_tOndata>0;
}

int CPandoraNodeKeepConnector::ICP_requestAuth()
{
    CPqStack __smsStack;
    CSmsMsg_requestAuth Msg;
    const char *pUri = def_Pandora_requestAuth;
    std::string strNodetName = m_strNodeId;//"ezlibsSmartServer";

    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    Msg.m_strAccount  	    = "admin";
    Msg.m_strPassword 		= "passwd";
    Msg.m_strSessionID	    = "";

    Msg.m_LocalSystemInfo.m_strHostID    = strNodetName;//PandoraNode里配置
    Msg.m_LocalSystemInfo.m_strHostAlias    = __cfgGeneral.getConfig().strAlias; // __cfg.getConfig().strNodeName;
    Msg.m_LocalSystemInfo.m_strSerial      = __cfgGeneral.getConfig().strSerial; // __cfg.getConfig().strNodeName;
    Msg.m_LocalSystemInfo.m_strDescription = __cfgGeneral.getConfig().strDescription;
    Msg.m_LocalSystemInfo.m_strType        = __cfgGeneral.getConfig().strCode;
    Msg.m_LocalSystemInfo.m_strVersion        = g_Solar.GetVersion();
    Msg.m_LocalSystemInfo.m_strLocation = __cfgGeneral.getConfig().strLocation;
    Msg.m_LocalSystemInfo.m_strVendor= __cfgGeneral.getConfig().strVendor;
    Msg.m_LocalSystemInfo.m_strMac = g_Solar.GetHostMac();
    Msg.m_LocalSystemInfo.m_strLocalDate   = ezConvertToString(time(NULL));//"2013-11-08 16:55:53 +0800";

    for(int ii=0; ii<m_pCfgShp->getNumber(); ii++)
    {
        // 未使能
        if (1 != m_pCfgShp->getConfig(ii).bEnable)
        {
            continue;
        }

        if (def_Service_Protocol_Shp == m_pCfgShp->getConfig(ii).strFunction)
        {
            CMsgService __Service;

            CCacheKeyValue::bldCacheKey(__Service.m_strServiceID, def_Service_Protocol_Shp+strNodetName, m_pCfgShp->getConfig(ii).local_ip, m_pCfgShp->getConfig(ii).local_port);
            __Service.m_strDescription = m_pCfgShp->getConfig(ii).strDescription;

            __Service.m_strURI = std::string(def_Service_Protocol_Shp)
                                 +"://user:passwd@"
                                 +m_pCfgShp->getConfig(ii).local_ip
                                 +":"
                                 +ezConvertToString( m_pCfgShp->getConfig(ii).local_port )
                                 +"/"
                                 +( m_pCfgShp->getConfig(ii).strType.length()>1?m_pCfgShp->getConfig(ii).strType:std::string("tcp") )// tcp, udp
                                 +"?p=0"; // shp告知服务器分配端口

            __Service.m_strProtocol    = m_pCfgShp->getConfig(ii).strFunction; // shpt - shp tcp 待分配

            Msg.m_vLocalService.push_back(__Service);
        } // if

    }// for
#if 0 //示例用
    CMsgService __Service;

    __Service.m_strServiceID   = strNodetName + "_22";
    __Service.m_strDescription = "ssh management";

    __Service.m_strURI         = "shp://root:admin@127.0.0.1:22/tcp?p=0"; // shp告知服务器分配端口
    //    shp://127.0.0.1:22/mana?p=0
    //        shp :
    //        127.0.0.1: ip
    //        22:port
    //        mana: 管理用
    //        p=0：自动分配， 其他： 指定

    //__Service.m_strPort        = "12345";
    __Service.m_strProtocol    = def_Service_Protocol_Shp_Tcp; // shpt - shp tcp 待分配
    Msg.m_vLocalService.push_back(__Service);

    __Service.m_strServiceID   = strNodetName + "_80";
    __Service.m_strDescription = "http management";

    __Service.m_strURI         = "shp://root:admin@127.0.0.1:80/tcp?p=0"; // shp告知服务器分配端口
    __Service.m_strProtocol    = def_Service_Protocol_Shp_Tcp; // shpt - shp tcp 待分配
    Msg.m_vLocalService.push_back(__Service);
#endif // 示例用

    __smsStack.Encode(Msg);

    SendMsg(pUri, __smsStack.ToString());

    m_enCurMsgStatus = enAppLogicStatus_Logining;
    m_iSentPacket++;

    return 0;
}
int CPandoraNodeKeepConnector::onMatch_requestAuthACK(CPqStack &JsonData)
{
    int iret = 0;

    CSmsMsg_requestAuthACK MsgAuthACK;

    if (JsonData.Decode(MsgAuthACK))
    {
        DBG(
            MsgAuthACK.dump();
        );

        if (PssMsg_Value_Result_Success == MsgAuthACK.Result)
        {
            ez_printf_info("Succeeded Auth:%s\n", MsgAuthACK.m_strSessionID.c_str());

            time_t ttSvr ;
            ezConvertFromString(ttSvr, MsgAuthACK.m_LocalSystemInfo.m_strLocalDate);
            DBG(int iret = stime(&ttSvr);
            ez_printf_debug("stime(%ld):%d\n", ttSvr, iret););


            /////////////////
#if 0

            ez_printf_debug("ServiceSize:%ld\n", MsgAuthACK.m_vLocalService.size());

            CshpcConfig *pShpConfig = NULL;//new CshpcConfig;
            if (NULL != pShpConfig)
            {
                for (size_t ii=0; ii<MsgAuthACK.m_vLocalService.size(); ii++)
                {
                    if (def_Service_Protocol_Shp == MsgAuthACK.m_vLocalService[ii].m_strProtocol)
                    {
                        ez_printf_debug("%s:%s\n", def_Service_Protocol_Shp, MsgAuthACK.m_vLocalService[ii].m_strURI.c_str());

                        ez_parsed_url_t *parsed_url;
                        parsed_url = ez_parse_url(MsgAuthACK.m_vLocalService[ii].m_strURI.c_str());
                        if (parsed_url)
                        {
                            for(int kk=0; kk<m_pCfgShp->getNumber(); kk++)
                            {
                                if (1 == m_pCfgShp->getConfig(kk).bEnable
                                    && def_Service_Protocol_Shp == m_pCfgShp->getConfig(kk).strFunction)
                                {
                                    //CMsgService __Service;
                                    std::string strServiceID;

                                    CCacheKeyValue::bldCacheKey(strServiceID, def_Service_Protocol_Shp+m_strNodeId, m_pCfgShp->getConfig(kk).local_ip, m_pCfgShp->getConfig(kk).local_port);

                                    if (MsgAuthACK.m_vLocalService[ii].m_strServiceID  == strServiceID)
                                    {
                                        pShpConfig->server_addr = parsed_url->host;
                                        pShpConfig->server_port = atoi(parsed_url->port);
                                        pShpConfig->privilege_token = parsed_url->password;

                                        char *pPort = strstr(parsed_url->query, def_Shp_Tcp_Port_Query_Mark);
                                        int iPort = 0;
                                        if (pPort)
                                        {
                                            iPort = atoi(pPort+(strlen(def_Shp_Tcp_Port_Query_Mark)));
                                            ez_printf_debug("iPort(%d)\n", iPort);
                                        }

                                        if (iPort>0 && iPort<=65535)
                                        {
                                            CshpService shpService;
                                            shpService.Section      = MsgAuthACK.m_vLocalService[ii].m_strServiceID;
                                            shpService.type = ( m_pCfgShp->getConfig(kk).strType.length()>1?m_pCfgShp->getConfig(kk).strType:std::string("tcp") );
                                            shpService.local_ip         = m_pCfgShp->getConfig(kk).local_ip;//= 127.0.0.1
                                            shpService.local_port       = m_pCfgShp->getConfig(kk).local_port;//= 22
                                            shpService.remote_port      = iPort;//strPort;//= 16286
                                            pShpConfig->shpServices.push_back(shpService);
                                        }
                                        // 找到并设置完毕
                                        break;
                                    }//if
                                }
                            }

                        }
                    }
                    else
                    {
                        ez_printf_debug("Pass Protocol:%s\n", MsgAuthACK.m_vLocalService[ii].m_strProtocol.c_str());
                    }

                }

                pShpConfig->Save("/tmp/shpc_dbg.cfg");
                delete pShpConfig;
            }//if (NULL != pShpConfig)
            else //if (NULL == pShpConfig)
            {
                //ez_printf_error("memory not enough.\n");
            }
#endif
            /////////////////
            m_sessionId = MsgAuthACK.m_strSessionID;

            m_enCurMsgStatus = enAppLogicStatus_Logined;
        }
        else
        {
            ez_printf_error("Failed Auth:%s\n", MsgAuthACK.Result.c_str());
        }
    }
    else
    {
        ez_printf_error("__CezlibsWebServiceIF.Decode failed %s\n", m_pBodyBuf->Buf());
    }

    return iret;
}

int CPandoraNodeKeepConnector::ICP_requestHeartbeat()
{
    DBG(ez_printf_debug("%s.\n", __FUNCTION__););

    CPqStack __smsStack;
    const char *pUri = def_Pandora_requestHeartbeat;

    // 发送属性信息
    CStreamCapabilityMsg Msg;

    Msg.BandwidthIn        = 1024*100;   // 入口带宽 bps
    Msg.BandwidthOut       = 1024*1000 ; // 出口带宽 bps
    Msg.CpuMark            = 800;        // cpu评分，
    Msg.Memory             = 1024;       // 内存数量 MByte
    //Msg.StreamChannelTotal      = 200;        // 分发能力
    //Msg.StreamChannelTotal = m_pCfgPandoraNode->getConfig().iIpcChn;

    __smsStack.Encode(Msg);

    SendMsg(pUri, __smsStack.ToString());

    m_enCurMsgStatus = enAppLogicStatus_Heartbeating;

    return 0;
}

int CPandoraNodeKeepConnector::Do_MatchReq_Hqtt1_Publish_IO_Out(CPqStack &pqStackReq)
{
    //ez_printf_info("%s -- %s\n", __FUNCTION__, m_pBodyBuf->Buf());

    //CPqStack __stackReq;
    CPqStack __stackReqThis;

    CPqMsg_requestMsg __reqMsg;
    CPqMsg_requestMsgPub_IO_Out __reqMsgThis;


    CPqMsg_requestMsgAck __ackMsgNode;

    CAckMsgSimple __ackMsg;

    //char *pBody = (char *)m_pBodyBuf->Buf();
    //std::string m_strBody = (char *)m_pBodyBuf->Buf();
    do
    {
#if 0
        if (!__stackReq.Parse(pBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }
#endif
        if (!pqStackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
        ///////////////////////
        if (!__stackReqThis.Parse(__reqMsg.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }
        if (!__stackReqThis.Decode(__reqMsgThis))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

#ifdef _FUN_FunSerial

        int iCh = atoi(__reqMsgThis.m_strChannel.c_str()) - 1;
        int iMethod = atoi(__reqMsgThis.m_strMethod.c_str());

        ez_printf_info("%s(%d, %d)\n",  "pubIOOut", iCh, iMethod);

        int iSnded = g_FunSerial.pubIOOut(0, iCh, iMethod);
        ARG_USED(iSnded);

#endif // _FUN_FunSerial      nn

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while (0);

    CPqStack __stackNode;
    __stackNode.Encode(__ackMsg);

    __ackMsgNode.m_strDevID = __reqMsg.m_strDevID;
    __ackMsgNode.m_strSession = __reqMsg.m_strSession;
    __ackMsgNode.m_strMsgBody = __stackNode.ToString();

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(def_PandoraQ_MasReq_Publish_IO_Out, __stack.ToString(), def_PandoraQ_MasReq_Publish_IO_Out);

    return 0;
}

int CPandoraNodeKeepConnector::Do_MatchAck_Hqtt1_PubTaskStart(CPqStack &pqStackReq)
{
    //ez_printf_debug("%s -- [%s]\n", __FUNCTION__, m_pBodyBuf->Buf());

    //CPqStack __stackReq;
    CPqStack __stackReqThis;

    CPqMsg_requestMsg __reqMsg;
    CPqMsg_requestMsgTaskPub __reqMsgThis;


    CPqMsg_requestMsgAck __ackMsgNode;

    CAckMsgSimple __ackMsg;

    //char *pBody = (char *)m_pBodyBuf->Buf();
    //std::string m_strBody = (char *)m_pBodyBuf->Buf();

    do
    {
#if 0
        if (!__stackReq.Parse(pBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }
#endif
        if (!pqStackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
        ///////////////////////
        if (!__stackReqThis.Parse(__reqMsg.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }
        if (!__stackReqThis.Decode(__reqMsgThis))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        std::string strTask = __reqMsgThis.m_strTaskID;

        if (strTask == Pq_TaskID_trigReboot)
        {
            ez_printf_info("%s(%s)\n",  "onTask", strTask.c_str());
            system("reboot");
        }
        else if (strTask == Pq_TaskID_QZ_trigLock)
        {
            ez_printf_info("%s(%s)\n",  "onTask", strTask.c_str());
            unsigned char __sndData[] = {0xFD, 0x01, 0x02, 0x00, 0x00, 0xFB};
            g_FunSerial.Sendto_Modbus(def_SerialFunc_QZ1_IO1, (char *)__sndData, sizeof(__sndData));
            //g_FunSerial.Sendto(def_SerialFunc_QZ1_IO1, (char *)__sndData, sizeof(__sndData));

        }
        else if (strTask == Pq_TaskID_QZ_startSampling)
        {
            ez_printf_info("%s(%s)\n",  "onTask", strTask.c_str());
            unsigned char __sndData[] = {0xFD, 0x01, 0x01, 0x00, 0x00, 0xFB};
            g_FunSerial.Sendto_Modbus(def_SerialFunc_QZ1_IO1, (char *)__sndData, sizeof(__sndData));
        }
        else if (strTask == Pq_TaskID_QZ_stopSampling)
        {
            ez_printf_info("%s(%s)\n",  "onTask", strTask.c_str());
            unsigned char __sndData[] = {0xFD, 0x01, 0x03, 0x00, 0x00, 0xFB};
            g_FunSerial.Sendto_Modbus(def_SerialFunc_QZ1_IO1, (char *)__sndData, sizeof(__sndData));
        }
        else if (strTask == Pq_TaskID_QC_PureWaterStart
                 || strTask == Pq_TaskID_QC_CustomSolutionStart
                 || strTask == Pq_TaskID_QC_StandardizeStart
                )
        {
            ez_printf_info("%s(%s)\n",  "onTask", strTask.c_str());
            unsigned char __sndData[] = {0x01, 0x02, 0x01, 0x02, 0x00, 0x00, 0x01, 0x00, 0x05, 0x01};

            if (strTask == Pq_TaskID_QC_PureWaterStart)
                __sndData[6] = 0x01 ;//纯水
            else if (strTask == Pq_TaskID_QC_CustomSolutionStart)
                __sndData[6] = 0x02 ;//纯水
            else if (strTask == Pq_TaskID_QC_StandardizeStart)
                __sndData[6] = 0x03 ;//纯水

            std::list<CValueMsg>::const_iterator it = __reqMsgThis.Values.begin();
            for ( ; it != __reqMsgThis.Values.end() ; it++)
            {
                if (it->m_strType == PqMsg_Key_TaskID)
                {
                    unsigned short __TaskID;
                    ezConvertFromString(__TaskID, it->m_strValue);

                    __sndData[4] = (__TaskID & 0xff);
                    __sndData[5] = ( (__TaskID>>8) & 0xff );

                    ez_printf_debug("%s:%s(%d)\n",  PqMsg_Key_TaskID.c_str(), it->m_strValue.c_str(), __TaskID);
                }
                else if (it->m_strType == def_PqMsg_ValueKey_MotherLiquidConcentration)
                {
                    unsigned short __MotherLiquidConcentration;
                    ezConvertFromString(__MotherLiquidConcentration, it->m_strValue);
                    __sndData[7] = (__MotherLiquidConcentration & 0xff);
                    __sndData[8] = ( (__MotherLiquidConcentration>>8) & 0xff );
                    ez_printf_debug("%s:%s(%d)\n",  def_PqMsg_ValueKey_MotherLiquidConcentration, it->m_strValue.c_str(), __MotherLiquidConcentration);
                }
                else if (it->m_strType == def_PqMsg_ValueKey_DosingConcentration)
                {
                    unsigned short __DosingConcentration;
                    ezConvertFromString(__DosingConcentration, it->m_strValue);
                    __sndData[9] = (unsigned char)(__DosingConcentration & 0xff);
                    ez_printf_debug("%s:%s(%d, %d)\n",  def_PqMsg_ValueKey_DosingConcentration, it->m_strValue.c_str(), __DosingConcentration, __sndData[9]);
                }
                else
                {
                    ez_printf_debug("%s(%s)\n",  "Unknow", it->m_strValue.c_str());
                }
            }

            g_FunSerial.Sendto_Hdlc2(def_SerialFunc_COD_HDLC, (char *)__sndData, sizeof(__sndData));
        }
        else if (strTask == Pq_TaskID_trigReboot)
        {
            ez_printf_info("%s(%s)\n",  "onTask", strTask.c_str());
            system("reboot");
        }
        else
        {
            ez_printf_info("%s(%s)\n",  "Unknow Task", strTask.c_str());
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while (0);

    CPqStack __stackNode;
    __stackNode.Encode(__ackMsg);

    __ackMsgNode.m_strDevID = __reqMsg.m_strDevID;
    __ackMsgNode.m_strSession = __reqMsg.m_strSession;
    __ackMsgNode.m_strMsgBody = __stackNode.ToString();

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(def_PandoraQ_MasReq_PubTaskStart, __stack.ToString(), def_PandoraQ_MasReq_PubTaskStart);

    return 0;
}

int CPandoraNodeKeepConnector::Do_Req_Hqtt1_PublishValue()
{
    time_t ttNow = time(NULL);
    ttNow-=(def_SerialFunc_SerialComm_Period*3); // 两个周期还没有不传


    // 还没有数据
#if 0

    if (g_FunSerial.m_ttOnTempHum<=0
        && g_FunSerial.m_ttOnPh<=0)
    {
        ez_printf_debug("no data now.\n");

        //return 0;
    }
#endif

    CValueMsg Value;
    CPqMsg_requestMsgPubValue __reqMsg;

    __reqMsg.m_strDevID = m_strNodeId;

    //ez_printf_debug("%s\n", __FUNCTION__);
    //ez_printf_debug("+++++++++++%ld, %f\n", g_FunSerial.m_ttOnTempHum, g_FunSerial.m_fTemperature);

    // 温湿度一起采集
    if (g_FunSerial.m_ttOnTempHum > 0)
    {
        //ez_printf_debug("%s\n", __FUNCTION__);
        g_FunSerial.m_ttOnTempHum = 0;

        if (g_FunSerial.m_fTemperature > InvalidFloatPara)
        {
            //ez_printf_debug("%s\n", __FUNCTION__);
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fTemperature);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(1.23);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
        if (g_FunSerial.m_fHumidity > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Humidity;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fHumidity);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Humidity;
            Value.m_strValue = ezConvertToString(70.3);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
    }

    // 五参数
    if (g_FunSerial.m_ttOnPh > 0/*ttNow*/)
    {
        g_FunSerial.m_ttOnPh = 0;

        if (g_FunSerial.m_fpH > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_PH;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fpH);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fCond > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Conductivity;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fCond);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fDo > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_DissolvedOxygen;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fDo);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fTurb > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Turbidity;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fTurb);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fTemp > InvalidFloatPara)
        {
            Value.m_strChannel = "2";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fTemp);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "2";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(1.23);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
    }

    // DI DO
    if (g_FunSerial.m_ttOnDIO > 0)
    {
        g_FunSerial.m_ttOnDIO = 0;

        for (int ii=0; ii<4; ii++)
        {
            Value.m_strChannel = ezConvertToString(ii+1);
            Value.m_strType = def_PqMsg_ValueKey_DigitalInput;
            Value.m_strValue = ezConvertToString(BitGet(g_FunSerial.m_iDI, ii));
            __reqMsg.Values.push_back(Value);

            Value.m_strChannel = ezConvertToString(ii+1);
            Value.m_strType = def_PqMsg_ValueKey_DigitalOutput;
            Value.m_strValue = ezConvertToString(BitGet(g_FunSerial.m_iDO, ii));
            __reqMsg.Values.push_back(Value);
        }
    }

    // 气象
    if (g_FunSerial.m_ttOnWeather > 0)
    {
        g_FunSerial.m_ttOnWeather = 0;

        if (g_FunSerial.m_fWindSpeed > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_WindSpeed;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fWindSpeed);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_fWindDirection > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_WindDirection;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fWindDirection);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_fAirPressure > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AirPressure;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fAirPressure);
            __reqMsg.Values.push_back(Value);
        }
        // 温度 3
        if (g_FunSerial.m_fTemperature_3 > InvalidFloatPara)
        {
            Value.m_strChannel = "3";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fTemperature_3);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "3";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(1.235);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
        // 湿度3
        if (g_FunSerial.m_fHumidity_3 > InvalidFloatPara)
        {
            Value.m_strChannel = "3";
            Value.m_strType = def_PqMsg_ValueKey_Humidity;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fHumidity_3);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "3";
            Value.m_strType = def_PqMsg_ValueKey_Humidity;
            Value.m_strValue = ezConvertToString(3.4271);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
		///////////////////////////
        // 温度 3
        if (g_FunSerial.m_fTemperature_3 > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fTemperature_3);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Temperature;
            Value.m_strValue = ezConvertToString(1.235);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
        // 湿度3
        if (g_FunSerial.m_fHumidity_3 > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Humidity;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fHumidity_3);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Humidity;
            Value.m_strValue = ezConvertToString(3.4271);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
		
		///////////////////////////
        // 雨量
        if (g_FunSerial.m_fRainfall > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Rainfall;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fRainfall);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Rainfall;
            Value.m_strValue = ezConvertToString(0);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
    }

    // BBE
    if (g_FunSerial.m_ttOnBBE > 0)
    {
        g_FunSerial.m_ttOnBBE = 0;

        if (g_FunSerial.m_fAlgaeConBlue > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AlgaeConBlue;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fAlgaeConBlue);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fAlgaeConGreen > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AlgaeConGreen;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fAlgaeConGreen);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fAlgaeConDino > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AlgaeConDino;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fAlgaeConDino);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_fAlgaeConHide > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AlgaeConHide;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fAlgaeConHide);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AlgaeConHide;
            Value.m_strValue = ezConvertToString(9.796);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen

        if (g_FunSerial.m_fAlgaeConYellow > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AlgaeConYellow;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fAlgaeConYellow);
            __reqMsg.Values.push_back(Value);
        }
        // 透光率
        if (g_FunSerial.m_fTransmittance > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Transmittance;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fTransmittance);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Transmittance;
            Value.m_strValue = ezConvertToString(32.3);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
        // 深度
        if (g_FunSerial.m_fDepth > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Depth;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fDepth);
            __reqMsg.Values.push_back(Value);
        }
#if defined(def_SimDataForVen)
        else
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Depth;
            Value.m_strValue = ezConvertToString(0.80758);
            __reqMsg.Values.push_back(Value);
        }
#endif //def_SimDataForVen
    }

    // COD
    if (g_FunSerial.m_ttOnCod > 0)
    {
        g_FunSerial.m_ttOnCod = 0;
        //ez_printf_debug("%s\n", __FUNCTION__);

        if (g_FunSerial.m_fCod_Permanganate > InvalidFloatPara)
        {
            //ez_printf_debug("%s\n", __FUNCTION__);
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Permanganate;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fCod_Permanganate);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fCod_AmmoniaNitrogen > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_AmmoniaNitrogen;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fCod_AmmoniaNitrogen);
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fCod_Phosphorus > InvalidFloatPara)
        {
            char buff[32];
            snprintf(buff, sizeof(buff)-1, "%.2f", g_FunSerial.m_fCod_Phosphorus);

            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Phosphorus;
            //Value.m_strValue = ezConvertToString(g_FunSerial.m_fCod_Phosphorus);
            Value.m_strValue = buff;
            __reqMsg.Values.push_back(Value);
        }

        if (g_FunSerial.m_fCod_Nitrogen > InvalidFloatPara)
        {
            char buff[32];
            snprintf(buff, sizeof(buff)-1, "%.2f", g_FunSerial.m_fCod_Nitrogen);

            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Nitrogen;
            //Value.m_strValue = ezConvertToString(g_FunSerial.m_fCod_Nitrogen);
            Value.m_strValue = buff;
            __reqMsg.Values.push_back(Value);
        }

    }
    // 鱼毒性仪
    if (g_FunSerial.m_ttOnFishTaximeter1 > 0)
    {
        g_FunSerial.m_ttOnFishTaximeter1 = 0;
        //ez_printf_debug("%s\n", __FUNCTION__);

        const char *__pDataKey[def_FishTaximeter1_Data_Num]=
            {
                def_PqMsg_ValueKey_FishTaximeter1_Activated      ,
                def_PqMsg_ValueKey_FishTaximeter1_activated_sp   ,
                def_PqMsg_ValueKey_FishTaximeter1_TopCoverage    ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_1          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_2          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_3          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_4          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_5          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_6          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_7          ,
                def_PqMsg_ValueKey_FishTaximeter1_Row_8          ,
                def_PqMsg_ValueKey_FishTaximeter1_temperature    ,
                def_PqMsg_ValueKey_FishTaximeter1_flow           ,
                def_PqMsg_ValueKey_FishTaximeter1_Heat           ,
                def_PqMsg_ValueKey_FishTaximeter1_HeatWalt
            };

        for (size_t ii=0; ii<def_FishTaximeter1_Data_Num; ii++)
        {

            if (g_FunSerial.m_fFishTaximeter1_Data[ii] > InvalidFloatPara)
            {
                char buff[32];
                snprintf(buff, sizeof(buff)-1, "%.2f", g_FunSerial.m_fFishTaximeter1_Data[ii]);

                Value.m_strChannel = "1";
                Value.m_strType = __pDataKey[ii];
                //Value.m_strValue = ezConvertToString(g_FunSerial.m_fCod_Nitrogen);
                Value.m_strValue = buff;
                __reqMsg.Values.push_back(Value);
            }
        }

        if (g_FunSerial.m_iFishTaximeter1_Alarm_toxicity > InvalidIntPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_FishTaximeter1_Alarm_toxicity;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_iFishTaximeter1_Alarm_toxicity);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_iFishTaximeter1_Alarm_Hardware > InvalidIntPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_FishTaximeter1_Alarm_Hardware;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_iFishTaximeter1_Alarm_Hardware);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_iFishTaximeter1_Alarm_flow > InvalidIntPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_FishTaximeter1_Alarm_flow;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_iFishTaximeter1_Alarm_flow);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_iFishTaximeter1_Alarm_Odoor > InvalidIntPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_FishTaximeter1_Alarm_Odoor;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_iFishTaximeter1_Alarm_Odoor);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_iFishTaximeter1_Alarm_Level > InvalidIntPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_FishTaximeter1_Alarm_Level;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_iFishTaximeter1_Alarm_Level);
            __reqMsg.Values.push_back(Value);
        }
        if (g_FunSerial.m_iFishTaximeter1_Feeding > InvalidIntPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_FishTaximeter1_Feeding;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_iFishTaximeter1_Feeding);
            __reqMsg.Values.push_back(Value);
        }

    }

    // 噪音
    if (g_FunSerial.m_ttOnNoise > 0/*ttNow*/)
    {
        g_FunSerial.m_ttOnNoise = 0;

        if (g_FunSerial.m_fNoise > InvalidFloatPara)
        {
            Value.m_strChannel = "1";
            Value.m_strType = def_PqMsg_ValueKey_Noise1;
            Value.m_strValue = ezConvertToString(g_FunSerial.m_fNoise);
            __reqMsg.Values.push_back(Value);
        }
    }

    // def_PqMsg_ValueKey_LiveUrlSmoothRtmp
    {

        for(int ii=0; ii<m_pCfgShp->getNumber(); ii++)
        {
            // 未使能
            if (1 != m_pCfgShp->getConfig(ii).bEnable)
            {
                continue;
            }

            if (std::string(def_PqMsg_ValueKey_LiveUrlSmoothRtmp) == m_pCfgShp->getConfig(ii).strFunction)
            {
                Value.m_strChannel = ezConvertToString(ii+1);
                Value.m_strType = def_PqMsg_ValueKey_LiveUrlSmoothRtmp;
                Value.m_strValue = m_pCfgShp->getConfig(ii).local_ip;
                __reqMsg.Values.push_back(Value);
            }
            else if (std::string(def_PqMsg_ValueKey_LiveUrlClearRtmp) == m_pCfgShp->getConfig(ii).strFunction)
            {
                Value.m_strChannel = ezConvertToString(ii+1);
                Value.m_strType = def_PqMsg_ValueKey_LiveUrlClearRtmp;
                Value.m_strValue = m_pCfgShp->getConfig(ii).local_ip;
                __reqMsg.Values.push_back(Value);
            }

        }
    }

    if (__reqMsg.Values.size()>0)
    {
        CPqStack __stack;
        __stack.Encode(__reqMsg);
        SendMsg(def_PandoraQ_NodReq_PublishValue, __stack.ToString());

        ez_printf_debug("%s Len:%ld\n",  "Values", __stack.ToString().length());
        ez_printf_debug("%s :%s\n",  "Values", __stack.ToString().c_str());
    }
    else
    {
        //ez_printf_debug("Do nothing\n");
    }

    return 0;
}

int CPandoraNodeKeepConnector::Do_MatchReq_PandoraQ_MasReq_ShpSet(CPqStack &pqStackReq)
{
    //ez_printf_info("%s -- %s\n", __FUNCTION__, m_pBodyBuf->Buf());

    CPqStack __stackReqThis;

    CPqMsg_requestMsg __reqMsg;
    CPqMsg_requestMsgSetShpc __reqMsgThis;


    CPqMsg_requestMsgAck __ackMsgNode;

    CAckMsgSimple __ackMsg;

    CshpcConfig *pShpConfig = NULL;

    do
    {
        if (!pqStackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
        ///////////////////////
        if (!__stackReqThis.Parse(__reqMsg.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }

        if (!__stackReqThis.Decode(__reqMsgThis))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_debug("DevID : %s\n", __reqMsgThis.m_strDevID.c_str());

        std::string strFunc = __reqMsgThis.m_strFunc.length()>2 ? __reqMsgThis.m_strFunc : "/tmp/shpc_dbg.cfg";// 目前为文件

        pShpConfig = new CshpcConfig;
        if (NULL == pShpConfig)
        {
            ez_printf_error("Failed new CshpcConfig.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;

            break;
        }

        //if (NULL != pShpConfig)
        {

            pShpConfig->server_addr = __reqMsgThis.m_strsHost;
            pShpConfig->server_port = __reqMsgThis.m_strsPort;
            pShpConfig->privilege_token = __reqMsgThis.m_strsTocken;

            pShpConfig->tls_enable= __reqMsgThis.m_i_sTls;

            std::list<CShpc>::const_iterator it = __reqMsgThis.Shpcs.begin();
            for ( ; it != __reqMsgThis.Shpcs.end() ; it++)
            {
                CshpService shpService;

                if (it->m_strcIteam.length()>2)
                {
                    shpService.Section      = it->m_strcIteam;
                }
                else
                {
                    shpService.Section =
                        m_strNodeId  //设备编号
                        + it->m_strcHost
                        + ezConvertToString(it->m_strcPort)
                        + __reqMsgThis.m_strsHost
                        + ezConvertToString(__reqMsgThis.m_strsPort);

                    char dst[SHA1HashSizeDestBuffer] = {0};
                    int iret_sha1 = ez_sha1_str(dst, sizeof(dst), shpService.Section.c_str());

                    if (iret_sha1 >= 0)
                    {
                        shpService.Section      = dst;
                    }
                    else
                    {
                        ez_printf_error("failed to make Section use:%s\n", shpService.Section.c_str());
                    }

                }
                shpService.type = it->m_strcType;
                shpService.local_ip         = it->m_strcHost;//= 127.0.0.1
                shpService.local_port       = it->m_strcPort;//= 22
                shpService.remote_port      = it->m_strcRemontePort;//strPort;//= 16286

                shpService.use_encryption = it->m_iEncryption;//
                shpService.use_compression = it->m_iCompression;//

                pShpConfig->shpServices.push_back(shpService);
            }

            int iret_saveCfg = pShpConfig->Save(strFunc);

            if (iret_saveCfg < 0)
            {
                ez_printf_error("Error(%d) -- %s\n", iret_saveCfg, PssMsg_ResultMsg_PandoraNode_FailedToSaveCfgFile.c_str());

                __ackMsg.Result = PssMsg_Result_PandoraNode_FailedToSaveCfgFile;
                __ackMsg.Reason = PssMsg_ResultMsg_PandoraNode_FailedToSaveCfgFile;

                break;
            }
            else
            {
                ez_printf_debug("pShpConfig saved:%s\n", strFunc.c_str());
            }
        }

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while (0);

    if (pShpConfig)
    {
        delete pShpConfig;
        pShpConfig = NULL;
    }

    CPqStack __stackNode;
    __stackNode.Encode(__ackMsg);

    __ackMsgNode.m_strDevID = __reqMsg.m_strDevID;
    __ackMsgNode.m_strSession = __reqMsg.m_strSession;
    __ackMsgNode.m_strMsgBody = __stackNode.ToString();

    CPqStack __stack;
    //__stack.Encode(__ackMsg);
    __stack.Encode(__ackMsgNode);

    SendMsg(def_PandoraQ_NodReq_ShpSet, __stack.ToString(), def_PandoraQ_NodReq_ShpSet);

    return 0;
}

int CPandoraNodeKeepConnector::Do_MatchReq_PandoraQ_MasReq_ShpGet(CPqStack &pqStackReq)
{
    //ez_printf_info("%s -- %s\n", __FUNCTION__, m_pBodyBuf->Buf());

    CPqStack __stackReqThis;

    CPqMsg_requestMsg __reqMsg;//封装消息
    CPqMsg_requestMsgSetShpc __reqMsgThis; //本函数所需要处理的消息

    CPqMsg_requestMsgAck __ackMsgNode; //封装返回用

    CPqMsg_requestMsgSetShpcAck __ackMsg; // 本函数返回的消息

    CshpcConfig *pShpConfig = NULL;

    do
    {
        if (!pqStackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ///////////////////////
        if (!__stackReqThis.Parse(__reqMsg.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
        }

        if (!__stackReqThis.Decode(__reqMsgThis))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        ez_printf_debug("DevID : %s\n", __reqMsgThis.m_strDevID.c_str());
        ez_printf_debug("m_strFunc : %s\n", __reqMsgThis.m_strFunc.c_str());

        std::string strFunc = __reqMsgThis.m_strFunc.length()>2 ? __reqMsgThis.m_strFunc : "/tmp/shpc_dbg.cfg";// 目前为文件
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        pShpConfig = new CshpcConfig;
        if (NULL == pShpConfig)
        {
            ez_printf_error("Failed new CshpcConfig.\n");

            __ackMsg.Result = PssMsg_Value_Result_NotEnoughMem;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_NotEnoughMem;

            break;
        }

        int iretLoadini = pShpConfig->Load(strFunc);
        if (iretLoadini<0
                || false==pShpConfig->bCfgValid())
        {
            ez_printf_error("Config not valid.\n");

            __ackMsg.Result = PssMsg_Result_PandoraNode_NotFoundCfgFile;
            __ackMsg.Reason = PssMsg_ResultMsg_PandoraNode_NotFoundCfgFile;

            break;
        }

        __ackMsg.m_strDevID = __reqMsgThis.m_strDevID;
        __ackMsg.m_strFunc = __reqMsgThis.m_strFunc;

        __ackMsg.m_strsHost = pShpConfig->server_addr;
        __ackMsg.m_strsPort = pShpConfig->server_port;
        __ackMsg.m_strsTocken = pShpConfig->privilege_token;
        __ackMsg.m_i_sTls = pShpConfig->tls_enable;

        //std::list<CshpService> shpServices;
        for (std::list<CshpService>::const_iterator ci = pShpConfig->shpServices.begin() ; ci != pShpConfig->shpServices.end() ; ++ci)
        {
            CShpc __sc;
            __sc.m_strcIteam = ci->Section;
            __sc.m_strcType = ci->type;
            __sc.m_strcHost = ci->local_ip;
            __sc.m_strcPort = ci->local_port;
            __sc.m_strcRemontePort = ci->remote_port;

            if (ci->use_encryption==1 || ci->use_encryption==0)
                __sc.m_iEncryption = ci->use_encryption;
            if (ci->use_compression==1 || ci->use_compression==0)
                __sc.m_iCompression = ci->use_compression;

            __ackMsg.Shpcs.push_back(__sc);
        }

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while (0);

    if (pShpConfig)
    {
        delete pShpConfig;
        pShpConfig = NULL;
    }

    CPqStack __stackNode;
    __stackNode.Encode(__ackMsg);

    __ackMsgNode.m_strDevID = __reqMsg.m_strDevID;
    __ackMsgNode.m_strSession = __reqMsg.m_strSession;
    __ackMsgNode.m_strMsgBody = __stackNode.ToString();

    CPqStack __stack;
    //__stack.Encode(__ackMsg);
    __stack.Encode(__ackMsgNode);

    SendMsg(def_PandoraQ_NodReq_ShpGet, __stack.ToString(), def_PandoraQ_NodReq_ShpGet);

    return 0;
}
//
int CPandoraNodeKeepConnector::Do_MatchReq_Hqtt1_ShareHID(CPqStack &pqStackReq)
{
    //ez_printf_info("%s -- %s\n", __FUNCTION__, m_pBodyBuf->Buf());

    //CPqStack __stackReq;
    CPqStack __stackReqThis;

    CPqMsg_requestMsg __reqMsg;
    CPqMsg_requestMsgShareHID __reqMsgThis;


    CPqMsg_requestMsgAck __ackMsgNode;

    CAckMsgSimple __ackMsg;

    //char *pBody = (char *)m_pBodyBuf->Buf();
    //std::string m_strBody = (char *)m_pBodyBuf->Buf();
    do
    {
#if 0
        if (!__stackReq.Parse(pBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }
#endif
        if (!pqStackReq.Decode(__reqMsg))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
        ///////////////////////
        if (!__stackReqThis.Parse(__reqMsg.m_strMsgBody))
        {
            ez_printf_error("Failed Parse.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidDataFormat;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidDataFormat;

            break;
            //return -1;
        }
        if (!__stackReqThis.Decode(__reqMsgThis))
        {
            ez_printf_error("Failed Decode.\n");

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }
#ifndef _FUN_FunSerial
        ez_printf_error("Func not defined.\n");

        __ackMsg.Result = PssMsg_Value_Result_UnSupportedRequest;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_UnSupportedRequest + " Func not defined.";

        break;
#endif
        // 参数检查
        // int iDevId = atoi(__reqMsgThis.m_strIFNo.c_str());
        // if (0 == iDevId)
        // {
        //     iDevId = 1;
        // }

        // 没有必要支持太多设备 1-32足够
        unsigned char iIFId = (__reqMsgThis.m_strIFNo.length() > 0) ? (unsigned char)atoi(__reqMsgThis.m_strIFNo.c_str()) : 0;

        if (0 == iIFId)
        {
            iIFId = 1;
        }
        else if (iIFId > 32)
        {
            ez_printf_error("iIFId(%d) > 32\n", iIFId);

            __ackMsg.Result = PssMsg_Value_Result_InvalidData;
            __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

            break;
        }

        // 默认是键盘输入
        if (PqMsg_Key_ShareHID_CmdType_Keyboard != __reqMsgThis.m_strCmdType && PqMsg_Key_ShareHID_CmdType_Mouse != __reqMsgThis.m_strCmdType)
            __reqMsgThis.m_strCmdType = PqMsg_Key_ShareHID_CmdType_Keyboard;

        // 键盘输入
        if (PqMsg_Key_ShareHID_CmdType_Keyboard == __reqMsgThis.m_strCmdType)
        {
            if (__reqMsgThis.m_strUsualKey.length() > 0)
            {
                ez_printf_info("%s(%d, [%s](%d))\n", "Sendto_CH9329_String", iIFId, __reqMsgThis.m_strUsualKey.c_str(), __reqMsgThis.m_strUsualKey.length());

                int iSnded = g_FunSerial.Sendto_CH9329_String(iIFId, __reqMsgThis.m_strUsualKey.c_str(), __reqMsgThis.m_strUsualKey.length());
                ARG_USED(iSnded);
            }

            if (__reqMsgThis.m_strControlKey.length() > 0)
            {
                int iSnded = g_FunSerial.Sendto_CH9329_AsciiCtrl(iIFId, __reqMsgThis.m_strControlKey);
                ARG_USED(iSnded);
            }
        }
        // 只有键盘和鼠标， 因此这里是鼠标操作
        else // if (PqMsg_Key_ShareHID_CmdType_Mouse == __reqMsgThis.m_strCmdType)
        {
            unsigned char mouseKey = (__reqMsgThis.m_strMouseKey.length() > 0) ? atoi(__reqMsgThis.m_strMouseKey.c_str()) : 0;
            int __xMove = (__reqMsgThis.m_strMouseXMove.length() > 0) ? atoi(__reqMsgThis.m_strMouseXMove.c_str()) : 0;
            int __yMove = (__reqMsgThis.m_strMouseYMove.length() > 0) ? atoi(__reqMsgThis.m_strMouseYMove.c_str()) : 0;
            int __rollerMove = (__reqMsgThis.m_strMouseRollerMove.length() > 0) ? atoi(__reqMsgThis.m_strMouseRollerMove.c_str()) : 0;

            if(__xMove < -0x7f || __xMove > 0x7f
                ||__yMove < -0x7f || __yMove > 0x7f
                ||__rollerMove < -0x7f || __rollerMove > 0x7f)
            {
                ez_printf_error("invalid data:__xMove:%d, __yMove:%d, __rollerMove:%d\n", __xMove, __yMove, __rollerMove);

                __ackMsg.Result = PssMsg_Value_Result_InvalidData;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

                break;
            }

            // unsigned char mouseKey = (__reqMsgThis.m_strMouseKey.length() > 0) ? atoi(__reqMsgThis.m_strMouseKey.c_str()) : 0;
            unsigned char xMove = __xMove > 0 ? __xMove : 0xff + (__xMove + 1);                     // 255 是一个像素, -1表示一个像素
            unsigned char yMove = __yMove > 0 ? __yMove : 0xff + (__yMove + 1);                     // 255 是一个像素, -1表示一个像素
            unsigned char rollerMove = __rollerMove > 0 ? __rollerMove : 0xff + (__rollerMove + 1); // 255 是一个像素, -1表示一个像素

            int iSnded = g_FunSerial.Sendto_CH9329_mouseKey(iIFId,
                                                            mouseKey,
                                                            xMove,
                                                            yMove,
                                                            rollerMove);

            if (iSnded <= 0)
            {
                ez_printf_error("failed iSnded(%d)\n", iSnded);

                __ackMsg.Result = PssMsg_Value_Result_InvalidData;
                __ackMsg.Reason = PssMsg_Value_ResultMsg_InvalidData;

                break;
            }
        }
        // else //

        __ackMsg.Result = PssMsg_Value_Result_Success;
        __ackMsg.Reason = PssMsg_Value_ResultMsg_Success;
    }
    while (0);

    CPqStack __stackNode;
    __stackNode.Encode(__ackMsg);

    __ackMsgNode.m_strDevID = __reqMsg.m_strDevID;
    __ackMsgNode.m_strSession = __reqMsg.m_strSession;
    __ackMsgNode.m_strMsgBody = __stackNode.ToString();

    CPqStack __stack;
    __stack.Encode(__ackMsg);

    SendMsg(def_PandoraQ_NodReq_Publish_HID_Send, __stack.ToString(), def_PandoraQ_MasReq_Publish_HID_Send);

    return 0;
}
