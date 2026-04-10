/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StreamContainer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: StreamContainer.cpp 5884 2016-10-13 11:23:31Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-10-13 11:23:31  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "assert.h"

#include "EZLock.h"
#include "Utility.h"

#include "StreamContainer.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// CStreamNode
CStreamNode::CStreamNode()
{
    Initialize();
}
CStreamNode::~CStreamNode()
{}
void CStreamNode::Initialize()
{
    StreamID = "";//DevID_CH_Stream: 812A930AACA_0_2
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

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    hx_UrlHls = "";
    hx_UrlRtmp = "";
    hx_UrlRtsp = "";
    hx_urlFreshTime = 0;
    hx_urlFreshTimes = 0;

    hx_UrlHlsStatus = "";
    hx_UrlRtmpStatus = "";
    hx_UrlRtspStatus = "";
    hx_statusFreshTime = 0;
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //pWorker = NULL;
    //pData = NULL;
    strUrl = "";
    strData = "";
    llReqTime = 0; //请求时间
    strMsgSession = ""; //会话Session
    //llAckTime = 0; //返回时间

    // 视频监控
    DevID = "";
    Channel = 0;
    Stream = 0;

    iAskedTimes = 0;
}

void CStreamNode::dump()
{}
void CStreamNode::dump(std::string &strDump)
{}

bool CStreamNode::operator==(const CStreamNode &t1)const
{
    return (StreamID == t1.StreamID);
}
bool CStreamNode::operator==(const std::string &theStreamID)const
{
    return (StreamID == theStreamID);
}
CStreamNode & CStreamNode::operator=(const CStreamNode &t1)
{
    StreamID = t1.StreamID;
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

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 采用直接拉流的方式
    hx_UrlHls           = t1.hx_UrlHls          ;
    hx_UrlRtmp          = t1.hx_UrlRtmp         ;
    hx_UrlRtsp          = t1.hx_UrlRtsp         ;
    hx_urlFreshTime     = t1.hx_urlFreshTime    ;
    hx_urlFreshTimes    = t1.hx_urlFreshTimes   ;

    hx_UrlHlsStatus     = t1.hx_UrlHlsStatus    ;
    hx_UrlRtmpStatus    = t1.hx_UrlRtmpStatus   ;
    hx_UrlRtspStatus    = t1.hx_UrlRtspStatus   ;
    hx_statusFreshTime  = t1.hx_statusFreshTime ;
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    strData    = t1.strData   ;
    strUrl    = t1.strUrl;
    llReqTime = t1.llReqTime; //请求时间
    strMsgSession = t1.strMsgSession; //会话Session
    //llAckTime = t1.llAckTime; //返回时间

    DevID    = t1.DevID   ;
    Channel  = t1.Channel ;
    Stream   = t1.Stream  ;

    iAskedTimes   = t1.iAskedTimes  ;

    return *this;
}

int CStreamNode::setMonitorStream(const std::string &theDevID, const int theChannel, const int theStream)
{
    DevID = theDevID;
    Channel = theChannel;
    Stream = theStream;

    StreamID = theDevID+"_"+Utility::l2string(theChannel)+"_"+Utility::l2string(theStream);

    return 0;
}
int CStreamNode::setMonitorStream(const std::string &DevID, const std::string &Channel, const std::string &Stream)
{
    return setMonitorStream(DevID, atoi(Channel.c_str()), atoi(Stream.c_str()));
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// CStreamContainer
CStreamContainer::CStreamContainer()
{
    m_pvNodeStream = new std::vector<CStreamNode>;

    assert(m_pvNodeStream);
}

CStreamContainer::~CStreamContainer()
{
    if (m_pvNodeStream)
    {
        delete m_pvNodeStream;
    }
}

size_t CStreamContainer::getCount()
{
    return m_pvNodeStream->size();
}

int CStreamContainer::addNodeStream(const CStreamNode &node)
{
    if (queryNodeStream(node)>=0)
    {
        return -1;
    }

    CEZLock __lock(m_MutexNodeStream);
    m_pvNodeStream->push_back(node);

    return 0;
}

int CStreamContainer::delNodeStream(size_t idx)
{
    if (idx>=m_pvNodeStream->size())
    {
        return -1;
    }

    CEZLock __lock(m_MutexNodeStream);
    m_pvNodeStream->erase(m_pvNodeStream->begin() + idx);

    return 0;
}
int CStreamContainer::modNodeStream(size_t idx, const CStreamNode &node)
{
    if (idx>=m_pvNodeStream->size())
    {
        return -1;
    }

    CEZLock __lock(m_MutexNodeStream);
    (*m_pvNodeStream)[idx] = node;

    return 0;
}
// ret idx of vec
int CStreamContainer::queryNodeStream(const CStreamNode &node)
{
    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii] == node)
        {
            return ii;
        }
    }

    return -1;
}

int CStreamContainer::queryNodeStream(const std::string &theStreamID)
{
    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii] == theStreamID)
        {
            return ii;
        }
    }

    return -1;
}

int CStreamContainer::queryNodeStreamByMsgSession(const std::string &theMsgSession)
{
    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii].strMsgSession == theMsgSession)
        {
            return ii;
        }
    }

    return -1;
}


//
int CStreamContainer::queryNodeStream(const std::string &DevID, const int chn, const int stream)
{
    CStreamNode __node;
    __node.setMonitorStream(DevID, chn, stream);

    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii] == __node)
        {
            return ii;
        }
    }

    return -1;
}
int CStreamContainer::getNodeStream(size_t idx, CStreamNode &node)
{
    if (idx>=m_pvNodeStream->size())
    {
        return -1;
    }

    node = (*m_pvNodeStream)[idx];

    return idx;
}
int CStreamContainer::getNodeStream(const std::string &theStreamID, CStreamNode &node)
{
    int idx = -1;
    idx = queryNodeStream(theStreamID);

    if (idx >= 0)
    {
        node = (*m_pvNodeStream)[idx];
    }

    return idx;
}

int CStreamContainer::getNodeStream(const std::string &DevID, const int chn, const int stream, CStreamNode &node)
{
    int idx = -1;

    idx = queryNodeStream(DevID, chn, stream);

    if (idx >= 0)
    {
        node = (*m_pvNodeStream)[idx];
    }

    return idx;
}

int CStreamContainer::getNodeStream(const std::string &DevID, const std::string &chn, const std::string &stream, CStreamNode &node)
{
    return getNodeStream(DevID, atoi(chn.c_str()), atoi(stream.c_str()), node);
}

size_t CStreamContainer::numNode()
{
    return m_pvNodeStream->size();
}

size_t CStreamContainer::numHls()
{

    size_t num = 0;

    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii].UrlHls.size()>0
            && (*m_pvNodeStream)[ii].UrlHls != URL_PROCESSING_STARTING)
        {
            num++;
        }
    }

    return num;
}

int CStreamContainer::getNodeStreamByMsgSession(const std::string &theMsgSession, CStreamNode &node)
{
    int idx = -1;
    idx = queryNodeStreamByMsgSession(theMsgSession);

    if (idx >= 0)
    {
        node = (*m_pvNodeStream)[idx];
    }

    return idx;
}

size_t CStreamContainer::numRtmp()
{

    size_t num = 0;

    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii].UrlRtmp.size()>0
            && (*m_pvNodeStream)[ii].UrlRtmp != URL_PROCESSING_STARTING)
        {
            num++;
        }
    }

    return num;
}

size_t CStreamContainer::numRtsp()
{

    size_t num = 0;

    for (size_t ii=0; ii<m_pvNodeStream->size(); ii++)
    {
        if ((*m_pvNodeStream)[ii].UrlRtsp.size()>0
            && (*m_pvNodeStream)[ii].UrlRtsp != URL_PROCESSING_STARTING)
        {
            num++;
        }
    }

    return num;
}

