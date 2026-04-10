/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * MsgContainer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: MsgContainer.cpp 5884 2016-12-10 10:56:45Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-12-10 10:56:45  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "assert.h"

#include "EZLock.h"
#include "Utility.h"

#include "MsgContainer.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// CMsgNode
CMsgNode::CMsgNode()
{
    Initialize();
}
CMsgNode::~CMsgNode()
{}
void CMsgNode::Initialize()
{
    MsgID = "";//DevID_CH_Msg: 812A930AACA_0_2
    strNodeName = "";

    PushUrlRtmp  = "";
    PushUrlRtsp  = "";
    UrlHls = "";//http://202.102.101.134:18080/hls/81165769460_0_2.m3u8?s=1eb860e156a9b2dec79045383423803e107f7550
    UrlRtmp = "";//rtmp://202.102.101.134:19090/live/81165769460_0_2?s=9060dd2eeb12e8c30339ed750ecc96374b70dbe2
    UrlRtsp = "";//rtsp://202.102.101.133:18554/812A9311631_0_2.sdp?s=3490c18229765f23d5fd3425c975b659854d9e18
    urlFreshTime = 0;
    urlFreshTimes = 0;

    UrlHlsStatus = "";
    UrlRtmpStatus = "";
    UrlRtspStatus = "";
    statusFreshTime = 0;

    strUrl = "";
    strData = "";
    llReqTime = 0; //请求时间
    strMsgSession = ""; //会话Session
    //llAckTime = 0; //返回时间

    // 视频监控
    DevID = "";
    Channel = 0;
    Msg = 0;

    iAskedTimes = 0;
}

void CMsgNode::dump()
{}
void CMsgNode::dump(std::string &strDump)
{}

bool CMsgNode::operator==(const CMsgNode &t1)const
{
    return (MsgID == t1.MsgID);
}
bool CMsgNode::operator==(const std::string &theMsgID)const
{
    return (MsgID == theMsgID);
}
CMsgNode & CMsgNode::operator=(const CMsgNode &t1)
{
    MsgID = t1.MsgID;
    strNodeName  = t1.strNodeName ;

    PushUrlRtmp  = t1.PushUrlRtmp ;
    PushUrlRtsp  = t1.PushUrlRtsp ;
    UrlHls   = t1.UrlHls  ;
    UrlRtmp  = t1.UrlRtmp ;
    UrlRtsp  = t1.UrlRtsp ;
    urlFreshTime = t1.urlFreshTime;
    urlFreshTimes = t1.urlFreshTimes;

    UrlHlsStatus = t1.UrlHlsStatus;
    UrlRtmpStatus = t1.UrlRtmpStatus;
    UrlRtspStatus = t1.UrlRtspStatus;
    statusFreshTime = t1.statusFreshTime;


    strData    = t1.strData   ;
    strUrl    = t1.strUrl;
    llReqTime = t1.llReqTime; //请求时间
    strMsgSession = t1.strMsgSession; //会话Session
    //llAckTime = t1.llAckTime; //返回时间

    DevID    = t1.DevID   ;
    Channel  = t1.Channel ;
    Msg   = t1.Msg  ;

    iAskedTimes   = t1.iAskedTimes  ;

    return *this;
}

int CMsgNode::setMonitorMsg(const std::string &theDevID, const int theChannel, const int theMsg)
{
    DevID = theDevID;
    Channel = theChannel;
    Msg = theMsg;

    MsgID = theDevID+"_"+Utility::l2string(theChannel)+"_"+Utility::l2string(theMsg);

    return 0;
}
int CMsgNode::setMonitorMsg(const std::string &DevID, const std::string &Channel, const std::string &Msg)
{
    return setMonitorMsg(DevID, atoi(Channel.c_str()), atoi(Msg.c_str()));
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// CMsgContainer
CMsgContainer::CMsgContainer()
{
    m_pvNodeMsg = new std::vector<CMsgNode>;

    assert(m_pvNodeMsg);
}

CMsgContainer::~CMsgContainer()
{
    if (m_pvNodeMsg)
    {
        delete m_pvNodeMsg;
    }
}

size_t CMsgContainer::getCount()
{
    return m_pvNodeMsg->size();
}

int CMsgContainer::addMsg(const CMsgNode &node)
{
    if (queryNodeMsg(node)>=0)
    {
        return -1;
    }

    CEZLock __lock(m_MutexNodeMsg);
    m_pvNodeMsg->push_back(node);

    return 0;
}

int CMsgContainer::addMsg(const std::string &strUrl, const std::string &strMsg)
{
    CMsgNode __theNode;

    __theNode.strUrl = strUrl;
    __theNode.strData = strMsg;

    return addMsg(__theNode);
}

int CMsgContainer::delMsg(size_t idx)
{
    if (idx>=m_pvNodeMsg->size())
    {
        return -1;
    }

    CEZLock __lock(m_MutexNodeMsg);
    m_pvNodeMsg->erase(m_pvNodeMsg->begin() + idx);

    return 0;
}
int CMsgContainer::modNodeMsg(size_t idx, const CMsgNode &node)
{
    if (idx>=m_pvNodeMsg->size())
    {
        return -1;
    }

    CEZLock __lock(m_MutexNodeMsg);
    (*m_pvNodeMsg)[idx] = node;

    return 0;
}
// ret idx of vec
int CMsgContainer::queryNodeMsg(const CMsgNode &node)
{
    for (size_t ii=0; ii<m_pvNodeMsg->size(); ii++)
    {
        if ((*m_pvNodeMsg)[ii] == node)
        {
            return ii;
        }
    }

    return -1;
}

int CMsgContainer::queryNodeMsg(const std::string &theMsgID)
{
    for (size_t ii=0; ii<m_pvNodeMsg->size(); ii++)
    {
        if ((*m_pvNodeMsg)[ii] == theMsgID)
        {
            return ii;
        }
    }

    return -1;
}

int CMsgContainer::queryNodeMsgByMsgSession(const std::string &theMsgSession)
{
    for (size_t ii=0; ii<m_pvNodeMsg->size(); ii++)
    {
        if ((*m_pvNodeMsg)[ii].strMsgSession == theMsgSession)
        {
            return ii;
        }
    }

    return -1;
}


//
int CMsgContainer::queryNodeMsg(const std::string &DevID, const int chn, const int Msg)
{
    CMsgNode __node;
    __node.setMonitorMsg(DevID, chn, Msg);

    for (size_t ii=0; ii<m_pvNodeMsg->size(); ii++)
    {
        if ((*m_pvNodeMsg)[ii] == __node)
        {
            return ii;
        }
    }

    return -1;
}

int CMsgContainer::getMsg(const size_t idx, CMsgNode &node)
{
    if (idx>=m_pvNodeMsg->size())
    {
        return -1;
    }

    node = (*m_pvNodeMsg)[idx];

    return idx;
}

int CMsgContainer::getMsg(const size_t idx, std::string &strUrl, std::string &strMsg)
{
    CMsgNode __node;
	
    int iret = getMsg(idx, __node);

    if (iret >= 0)
    {
        strUrl = __node.strUrl;
        strMsg = __node.strData;
    }

    return iret;
}

