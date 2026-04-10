/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Pusher.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Pusher.h 5884 2016-05-25 09:08:36Z WuJunjie $
 *
 *  Explain:
 *     -拉流体-
 *
 *  Update:
 *     2016-05-25 09:08:36  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_Pusher_H)
#define _Pusher_H

#include <map>

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include <dtPst.h> /* libdtPst */

#include "../Pandora/PandoraDef.h"

// 未列举的消息暂不需要
typedef enum __msg_Pusher_t {
    MSG_Pusher_OK = 0,		    	// HLS转码中
    MSG_Pusher_STOPED,		    	// HLS转码结束
    MSG_Pusher_Stream2Hls_OK,		    	// 第三方链接转码
    MSG_Pusher_Stream2Hls_STOPED,		    	// 第三方链接转码结束
    MSG_Pusher_Tykd2Rtsp_OK,		    	// RTSP中
    MSG_Pusher_Tykd2Rtsp_STOPED,		    	// RTSP结束

    MSG_Pusher_Tykd2Record_OK,		    	// Rec中
    MSG_Pusher_Tykd2Record_STOPED,		    	// Rec结束
}MSG_Pusher_T;

typedef enum __em_terminal_status_t {
    em_terminal_status_online = 1,
    em_terminal_status_offline,
    em_terminal_status_DevNotExist,
    em_terminal_status_unknown,      // 不在系统内
}EM_TERMINAL_STATUS_T;

#define PREFIX_CPusher_THREAD_NAME "Pusher-"
#define PREFIX_CPusherTykd2Rtsp_THREAD_NAME "PusherTykd2Rtsp-"
#define PREFIX_CPusherTykd2Hls_THREAD_NAME "PusherTykd2Hls-"
#define PREFIX_CPusherStream2Hls_THREAD_NAME "PusherStream2Hls-"
#define PREFIX_CRecordTykdMediaUrl_THREAD_NAME "Recorder-"
#define PREFIX_CRecFileHelper_THREAD_NAME "RecFileHelper-"
#define URL_SIGN_PARA_MARK "s="


//记录运行的时间点的个数
#define REC_ROUND_TIME_NUM 5
// 参数个数
#define def_Max_DoPush_Arg_Num 64
// chkLifeCycle返回值
typedef enum __Pusher_LifeCycle_t {
    Pusher_LifeCycle_T_OK = 0, // 正常
    Pusher_LifeCycle_T_TimeOut, // 超期，一般应该被停止， 去除
    Pusher_LifeCycle_T_NotStarted, // 未开启，一般应该被停止， 去除
}Pusher_LifeCycle_T;

// 运行状态用于判断是否有效， 可播放与否等
// PushStatus 返回值
#if 0
typedef enum __Pusher_PushStatus_t {
    Pusher_PushStatus_T_Ini = 0, // 初始状态
    Pusher_PushStatus_T_Starting, // 开始
    Pusher_PushStatus_T_Pushing, // 正常工作
}Pusher_PushStatus_T;
#endif
// 获取播放地址接口定义
typedef enum __em_GetPlayUrlmethod_t {
    em_GetPlayUrlmethod_default_value = 0,
    em_GetPlayUrlmethod_getPlayUrlHX,
    em_GetPlayUrlmethod_getPssServerPlayUrlHXReq,
    em_GetPlayUrlmethod_getPssStoragePlayUrlHXReq,
}EM_GetPlayUrlmethod_T;

#define PushTimeout_SECOND_Max 12 // 拉流最大超时时间

class CStreamPusherTykd;
class CPusher : public CEZThread
{
public:
    // msg, Pusher
    typedef TSignal2<int, void *> PusherSignal_t;
    typedef PusherSignal_t::SigProc PusherSignalProc_t;

    virtual EZTHREAD_BOOL Start(CEZObject * pObj, PusherSignalProc_t pProc);
    virtual EZTHREAD_BOOL Stop(CEZObject * pObj, PusherSignalProc_t pProc);
    virtual void ThreadProc();
    virtual void doPull();
    virtual void onStoped();

    CPusher();
    virtual ~CPusher();

    // 设置转发ID
    // iEncryptID， 0 - 不加密id, 1- 加密id
    virtual int setIdDst(const std::string &strPullUrl, const int iEncryptID=EncryptID_Encrypt);
    virtual int setTykdAccount (const std::string &strPusherID);
    virtual int setTykdPasswd (const std::string &strPusherID);
    virtual const std::string & getIdDst();
    virtual const std::string & getTykdAccount ();
    virtual const std::string & getTykdPasswd ();
    virtual const std::string & getUrlDst ();// CPusherTykd2Rtsp 在用
    virtual const std::string & getUrlDstFix();
    std::string m_strIdDst;//分发ID
    std::string m_strIdDstOri;//分发ID 变换前
    std::string m_strTykdAccount;// 天翼看店内的用户名
    std::string m_strTykdPasswd;// 天翼看店内的密码
    std::string m_strUrlDst; // 分发 url
    std::string m_strUrlDstFix; // 分发 url

    int m_iVideoEncode ; /* 编码选项 0-copy, 1-libx264, 2-libx265*/
    int m_iAudioEncode ; /* 编码选项 0-audo(copy), 1-aac, 2-noaudio, 3-libfdk_aac*/

    int m_iVideoFps; /* 编码选项 帧率>=0 0--自动*/
    int m_iVideoWidth; /* 编码选项 宽度>=0, 0--系统自定, 1-640, 2-704, 3-800 */
    int m_iVideoHeight; /* 编码选项 高度， 暂时不用 */
    ////////////////////////////////

    // 功能配置
    // 播放地址使用
    int m_iUsePlayUrl_Local;
    int m_iUsePlayUrl_Direct;
    int m_iUsePlayUrl_Relay;
//#if defined (_FUNC_LiveStreamming_BackMediaSource) 
    // 这里作为基本数据， 由接口来控制是不是具备功能
    std::string m_strPlayUrl_Src; // 视频源播放地址
//#endif // _FUNC_LiveStreamming_BackMediaSource
private:
    // 请求次数
    int m_iCountReq;

    // 最近请求时间点
    unsigned long m_ulReqPoint;
    // 启动时间点
    unsigned long m_ulStartPoint;
    unsigned long m_ulStartPushPoint; /* 开始取流时间点 */

    int m_iValidPeriod; /* 有效期 */

    int m_iAutoLiveSecond; /* 自动的时候的生存期*/
public:
    // 
    int setAutoLiveSecond(int iAutoLiveSecond);
    int getAutoLiveSecond();
    // 
    int setValidPeriod(int iValidPeriod);
    int setValidPeriod(const std::string &strValidPeriod);
    int getValidPeriod();
    // 开始
    int onStart();
    // 请求调用
    // 返回请求次数
    int onReq();
	
	// 流处理开始和结束
    int PushStart();
    int PushEnd();
    unsigned long getPushLast();// 持续时长
    //Pusher_PushStatus_T PushStatus();
    int m_iPushTimeout_SECOND_Max;
    // 检查生存期
    // ret:
    //        0 - 正常
    //        1 - 超期
    //        2 - 未开始
    int chkLifeCycle();
    unsigned long secOfStarted();//已经启动了?秒
    unsigned long secOfStart();//启动的时间点
    ////////////////////////////////

    //应该一起设置， 故而统一接口
    int setDevChnStreamID(const std::string &strTykdDevID, const std::string strChannel, const std::string strStream);

    const std::string & getTykdDevID ();
    const std::string &getChanel();
    const std::string &getStream();
    const std::string &getDevChnStreamID();
	
    std::string m_strTykdDevID;// 天翼看店内的设备id
    std::string m_iChanel; // 通道号
    std::string m_iStream; // 码流 1主码流  2子码流

    std::string m_strDevChnStreamID; // 设备id_通道_码流 用于标识一个码流

    PusherSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;
    //信号的引用计数
    int m_iSigUser;

    int getChildPid(); //子进程pid
    unsigned int getRound(); //循环次数
    unsigned long long getRoundTimePoints();//运行时间点
    int m_iChildPid; //子进程pid
    unsigned int m_uiRound; //循环次数
    unsigned long long m_ullRoundTimePoints[REC_ROUND_TIME_NUM];

    EM_TERMINAL_STATUS_T getDevStatus(); 
    int setDevStatus(EM_TERMINAL_STATUS_T st); 
private:
    EM_TERMINAL_STATUS_T m_emDevStatus; //设备状态

private:
    // 选择turnip的功能  
    //解决有些流媒体服务器不能访问某些turn的公网ip问题
    static std::string m_strHostRelative;
    static std::string m_strFixedhost; // 所有都替换成这个 优先检查
    static std::map<std::string, std::string> *m_pMapHostRelative; // 配置方式2

public: // 可以在CStreamPusherTykd 中调用
    void setHostRelative(const std::string &strHostRelative);

    // 视频源平台接口地址
    std::string m_strEndPoint4VideoRs;
protected:
    // 0 -- 完成修正
    // 1 --没有配置， 不需要修正
    // 2 -- 没有对应Host 配置, 不需要修正
    // 3 --  配置数据不恰当, 不需要修正
    // -1 -- 修正失败
    int fixRelayPlayUrl(std::string &strDst, const std::string strUrl);

    // 播放地址检查
    // 无效或者不可用的地址直接置空
    // 只有一个地址的时候， 通过strRelayPlayUrl传递
    //int chkPlayUrlHX(std::string &strLocalPlayUrl, std::string &strDirectPlayUrl);

public: // 可以在CStreamPusherTykd 中调用
    void setTykdWsUrl(const std::string &strTykdWsUrl);

protected:
    std::string m_strTykdWsUrl;

    CStreamPusherTykd * m_pPusherManager; // 通过obj 赋值
};

//typedef std::vector<CPusher *> def_PusherContainer;
typedef std::list<CPusher *> def_PusherContainer;

#endif // !defined(_Pusher_H)

