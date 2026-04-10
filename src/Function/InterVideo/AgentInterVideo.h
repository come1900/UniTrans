/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentInterVideo.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentInterVideo.h 5884 2013-01-29 03:21:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _AgentInterVideo_H
#define _AgentInterVideo_H

#include "EZThread.h"

#include <time.h>
#include <vector>
#include <ListenSocket.h>

#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigInterVideo.h"
#include "../../Configs/ConfigGeneral.h"
#endif //_USE_MODULES_EZCONFIG

// 反应堆
#include "HandlerInterVideo.h"
#include "../../Maintain/GeneralAgentLog.h"

// 
#include "HttpServerInterVideo.h"

#define g_AgentInterVideo (*CAgentInterVideo::instance())

class CAgentInterVideo : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CAgentInterVideo);
    CAgentInterVideo();
    virtual ~CAgentInterVideo();
    ////////

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();
    void UseConfig();

    // 设置服务，
    void SetSerPort(unsigned short  port);
    void Connect();

    // 内部需要一定的时间自行处理
    // 调用之后， 10s之后再调用Connect，
    void Disconnect();

    void Run();
  	
    CHandlerInterVideo *GetHandler();

private:
    //AgentInterVideoCfg_T m_ConfigOfInterVideo;
    unsigned short m_sSerPort;
    CHandlerInterVideo *m_pHandlerInterVideo;
    CMaintainAgentLog m_Log;

    ListenSocket<CHttpServerInterVideo> *m_pCHttpServerInterVideo;

}
; //CAgentInterVideo
#endif //_AgentInterVideo_H
