/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PusherStream2Hls.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PusherStream2Hls.h 5884 2017-05-19 09:06:13Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-05-19 09:06:13  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_PusherStream2Hls_H)
#define _PusherStream2Hls_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "Pusher.h"

typedef enum __em_PushFunc_t {
    em_PushFunc_RTMP =  0,	// 第0位为1表示RTMP推流
    em_PushFunc_RTSP,	// 第0位为1表示RTSP推流
}EM_PushFunc_T;

class CPusherStream2Hls : public CPusher//CEZThread
{
public:
    // start 运行前检查
    int checkValid();
    EZTHREAD_BOOL Start(CEZObject * pObj, PusherSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, PusherSignalProc_t pProc);

    CPusherStream2Hls();
    virtual ~CPusherStream2Hls();

    const std::string &getUrlHls ();
    const std::string &getUrlHlsFix ();
    const std::string &getUrlHlss ();
    const std::string &getUrlHlssFix ();
    const std::string &getUrlRtmp ();
    const std::string &getUrlRtmpFix ();
    const std::string &getUrlRtsp ();
    const std::string &getUrlRtspFix ();
    const std::string &getUrlFlv ();
    const std::string &getUrlFlvFix ();
    const std::string &getUrlDash ();
    const std::string &getUrlDashFix ();


    const std::string &getTitle()      ;
    const std::string &getMemo ()      ;
    const std::string &getAddr ()      ;
    const std::string &getEndDt()       ;
	
    // 记录数据用
    std::string m_strTitle      ;
    std::string m_strMemo       ;
    std::string m_strAddr       ;
    std::string m_strStartDt       ;
    std::string m_strEndDt       ;

    //特殊参数
    std::string m_strID    ; /* 设备ID  */
    std::string m_strType    ; /* 流类型，代号如下：1-流媒体      2-海康IPC     3-大华IPC     4-全球眼   */
    std::string m_strUrl     ; /* 链路url，协议://ip:port，例如：rtsp://192.168.5.66:554                 */
    std::string m_strAccount ; /* 账号、用户名                                                           */
    std::string m_strPasswd  ; /* 密码                                                                   */
    std::string m_strChnnel ; /* 通道号, ID号                                                           */
    std::string m_strSubtype ; /* 码流类型，主码流为0（即subtype=0），辅码流为1（即subtype=1），以此类推 */
    //char       *pPlayUrl   /* 码流类型，主码流为0（即subtype=0），辅码流为1（即subtype=1），以此类推 */
    unsigned int m_uPushFunc; // 推流协议, 按位bit0-rtmp, bit1-rtsp
    // 1-启用，其他不启用
    int m_iPushedByTheFront; //前端推流，比如南通中创执法项目 DH-IPC-HFW8841E-ZRL-IRA-LED
private:
    // 启动时间点
    //unsigned long m_ulStartPoint;

private:
    void doPull();
    virtual void onStoped();

    std::string m_strUrlHls;
    std::string m_strUrlHlsFix;
    std::string m_strUrlHlss;
    std::string m_strUrlHlssFix;
    std::string m_strUrlRtmp ;
    std::string m_strUrlRtmpFix;
    std::string m_strUrlRtsp ;
    std::string m_strUrlRtspFix;
    std::string m_strUrlFlv;
    std::string m_strUrlFlvFix;
    std::string m_strUrlDash;
    std::string m_strUrlDashFix;

    std::string m_strShaChkSum;

    //std::string m_strTykdWsUrl;
};
#endif // !defined(_PusherStream2Hls_H)

