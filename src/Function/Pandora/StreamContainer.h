/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StreamContainer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: StreamContainer.h 5884 2016-10-13 11:23:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-10-13 11:23:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string>
#include <vector>
#include <map>

#include <EZMutex.h>

#if !defined(_StreamContainer_H)
#define _StreamContainer_H

#define URL_PROCESSING_STARTING "starting"
#define URL_PROCESSING_STOPING "stoping"

class CStreamNode
{
public:
    std::string StreamID;//DevID_CH_Stream: 812A930AACA_0_2

    std::string strNodeName; // 处理节点名

    // 推流Url
    std::string PushUrlRtmp;//rtmp://202.102.101.134:19090/man/81165769460_0_2?s=9060dd2eeb12e8c30339ed750ecc96374b70dbe2
    std::string PushUrlRtsp;//rtsp://202.102.101.133:18554/812A9311631_0_2.sdp?s=3490c18229765f23d5fd3425c975b659854d9e18

    // 播放Url
    std::string UrlHls;//http://202.102.101.134:18080/hls/81165769460_0_2.m3u8?s=1eb860e156a9b2dec79045383423803e107f7550
    std::string UrlRtmp;//rtmp://202.102.101.134:19090/live/81165769460_0_2?s=9060dd2eeb12e8c30339ed750ecc96374b70dbe2
    std::string UrlRtsp;//rtsp://202.102.101.133:18554/812A9311631_0_2.sdp?s=3490c18229765f23d5fd3425c975b659854d9e18
    long long urlFreshTime;// Url更新时间
    int urlFreshTimes; //请求创建的次数

    std::string UrlHlsStatus;
    std::string UrlRtmpStatus;
    std::string UrlRtspStatus;
    long long statusFreshTime;// Status更新时间

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // hx播放Url
    std::string hx_UrlHls;//http://202.102.101.134:18080/hls/81165769460_0_2.m3u8?s=1eb860e156a9b2dec79045383423803e107f7550
    std::string hx_UrlRtmp;//rtmp://202.102.101.134:19090/live/81165769460_0_2?s=9060dd2eeb12e8c30339ed750ecc96374b70dbe2
    std::string hx_UrlRtsp;//rtsp://202.102.101.133:18554/812A9311631_0_2.sdp?s=3490c18229765f23d5fd3425c975b659854d9e18
    long long hx_urlFreshTime;// Url更新时间
    int hx_urlFreshTimes; //请求创建的次数

    std::string hx_UrlHlsStatus;
    std::string hx_UrlRtmpStatus;
    std::string hx_UrlRtspStatus;
    long long hx_statusFreshTime;// Status更新时间
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    //
    std::string strUrl; //请求Url
    std::string strData; // 请求数据
    long long llReqTime; //请求时间
    std::string strMsgSession; //会话Session
    long long llAckTime; //返回时间

    int iAskedTimes; //请求次数

    CStreamNode();
    virtual ~CStreamNode();
    void Initialize();

    bool operator==(const CStreamNode &t1)const;
    bool operator==(const std::string &theStreamID)const;
    CStreamNode & operator=(const CStreamNode &t1);

    void dump();
    void dump(std::string &strDump);

    int setMonitorStream(const std::string &DevID, const int Channel, const int Stream);
    int setMonitorStream(const std::string &DevID, const std::string &Channel, const std::string &Stream);

private:
    // 视频监控
    std::string DevID;
    int Channel;
    int Stream;
}
;//CStreamNode

class CStreamContainer// : public CEZObject
{
public:

    CStreamContainer();
    virtual ~CStreamContainer();

    size_t getCount();

    int addNodeStream(const CStreamNode &node);
    int delNodeStream(size_t idx);
    int modNodeStream(size_t idx, const CStreamNode &node);
    // ret idx of vec
    int queryNodeStream(const CStreamNode &node);
    int queryNodeStream(const std::string &theStreamID);
    int queryNodeStream(const std::string &DevID, const int chn, const int stream);
    int queryNodeStreamByMsgSession(const std::string &MsgSession);

    int getNodeStream(size_t idx, CStreamNode &node);
    int getNodeStream(const std::string &theStreamID, CStreamNode &node);
    int getNodeStream(const std::string &DevID, const int chn, const int stream, CStreamNode &node);
    int getNodeStream(const std::string &DevID, const std::string &chn, const std::string &stream, CStreamNode &node);

    int getNodeStreamByMsgSession(const std::string &MsgSession, CStreamNode &node);

    size_t numNode();
    size_t numHls();
    size_t numRtmp();
    size_t numRtsp();

private:
    std::vector<CStreamNode> *m_pvNodeStream;
    CEZMutex m_MutexNodeStream;
};

#endif // !defined(_StreamContainer_H)
