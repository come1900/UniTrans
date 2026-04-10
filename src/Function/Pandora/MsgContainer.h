/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * MsgContainer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: MsgContainer.h 5884 2016-12-10 10:56:40Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-12-10 10:56:40  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include <string>
#include <vector>
#include <map>

#include <EZMutex.h>

#if !defined(_MsgContainer_H)
#define _MsgContainer_H

class CMsgNode
{
public:
    std::string MsgID;//DevID_CH_Msg: 812A930AACA_0_2

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

    //
    std::string strUrl; //请求Url
    std::string strData; // 请求数据
    

    long long llReqTime; //请求时间
    std::string strMsgSession; //会话Session
    long long llAckTime; //返回时间

    int iAskedTimes; //请求次数

    CMsgNode();
    virtual ~CMsgNode();
    void Initialize();

    bool operator==(const CMsgNode &t1)const;
    bool operator==(const std::string &theMsgID)const;
    CMsgNode & operator=(const CMsgNode &t1);

    void dump();
    void dump(std::string &strDump);

    int setMonitorMsg(const std::string &DevID, const int Channel, const int Msg);
    int setMonitorMsg(const std::string &DevID, const std::string &Channel, const std::string &Msg);

private:
    // 视频监控
    std::string DevID;
    int Channel;
    int Msg;
}
;//CMsgNode

class CMsgContainer// : public CEZObject
{
public:

    CMsgContainer();
    virtual ~CMsgContainer();

    size_t getCount();

    int addMsg(const CMsgNode &node);
    int addMsg(const std::string &strUrl, const std::string &strMsg);

    int delMsg(size_t idx);
	
    int modNodeMsg(size_t idx, const CMsgNode &node);
    // ret idx of vec
    int queryNodeMsg(const CMsgNode &node);
    int queryNodeMsg(const std::string &theMsgID);
    int queryNodeMsg(const std::string &DevID, const int chn, const int Msg);
    int queryNodeMsgByMsgSession(const std::string &MsgSession);

    int getMsg(const size_t idx, CMsgNode &node);
    int getMsg(const size_t idx, std::string &strUrl, std::string &strMsg);

private:
    std::vector<CMsgNode> *m_pvNodeMsg;
    CEZMutex m_MutexNodeMsg;
};

#endif // !defined(_MsgContainer_H)
