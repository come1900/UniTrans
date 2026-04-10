/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuClient.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: PuClient.h 5884 2019-05-15 02:25:26Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-05-15 02:25:26  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#if !defined(_PuClient_H)
#define _PuClient_H

#define PREFIX_CPuClient_THREAD_NAME "PuClient-"

// 
typedef enum __msg_PuClient_t {
    MSG_PuClient_OK = 0,		    	// 运行中
    MSG_PuClient_STOPED,		    	// 运行结束
}MSG_PuClient_T;

// 终端类型
typedef enum __msg_PuClient_Type_t { 
    MSG_PuClient_Type_Hik      = 2,		    	// 海康
    MSG_PuClient_Type_Dahua    = 3,		    	// 大华
    MSG_PuClient_Type_Xiongmai = 12,		   	// 雄迈
    MSG_PuClient_Type_Onvif    = 16,		   	// Onvif
    MSG_PuClient_Type_Undefined,		   	    // 
}MSG_PuClient_Type_T;

typedef enum __runStatus_PuClient_t {
    runStatus_PuClient_Ini = 0,		    	// 对象初始化
    runStatus_PuClient_Connecting,		    	// 正在连接
    runStatus_PuClient_Valid,		    	// 正常工作中
    runStatus_PuClient_Loss,		    	// 离线
}RunStatus_PuClient_T;

class CPuClient : public CEZThread
{
public:
    // msg, PuClient
    typedef TSignal2<int, void *> PuClientSignal_t;
    typedef PuClientSignal_t::SigProc PuClientSignalProc_t;

    virtual EZTHREAD_BOOL Start(CEZObject * pObj, PuClientSignalProc_t pProc);
    virtual EZTHREAD_BOOL Stop(CEZObject * pObj, PuClientSignalProc_t pProc);
    virtual void ThreadProc();
    virtual void onTask();
    virtual void onStoped();


    int setPuID(const std::string &strPuID);
    const std::string & getPuID ();

    // 获取硬盘信息
    virtual int getInfo_Storage(); 

    CPuClient();
    virtual ~CPuClient();

    PuClientSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;
    //信号的引用计数
    int m_iSigUser;

protected:
	MSG_PuClient_Type_T m_emType; // 类型
	std::string m_strHost;
	int m_iPort;
	std::string m_strAccount;
	std::string m_strPasswd;

int setRunStatus(RunStatus_PuClient_T runStatus);
RunStatus_PuClient_T getRunStatus();
private:
    CEZMutex m_MutexRunStatus;
	RunStatus_PuClient_T m_tRunStatus;

private:
    unsigned int m_uiRound; //循环次数
    std::string m_strPuID;  // 设备id
};

#endif // !defined(_PuClient_H)
