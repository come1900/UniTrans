/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FunConsole.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FunConsole.h 5884 2014-03-26 02:48:32Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-03-26 02:48:32  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_FunConsole_H)
#define _FunConsole_H
#include <string>
#include <EZObject.h>

#include "ConsoleDef.h"

class CFunConsole: public CEZObject
{
public:

    CFunConsole(CEZObject *pObj, CONSOLE_CALLBACK pCallBack);
    virtual ~CFunConsole();

    int OnCommand(int argc, char * argv[]);
    // 1 --  user
    // 2 -- admin
    const std::string & HelperInfo(int iLevel=1);

    void FC_Ver(int argc, char * argv[]);

    void FC_SysReboot(int argc, char * argv[]);
    void FC_SysRestart(int argc, char * argv[]);

    void FC_Suicide(int argc, char * argv[]);

    void FC_Status(int argc, char * argv[]);
    void FC_Timer(int argc, char * argv[]);
    void FC_Thread(int argc, char * argv[]);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 功能
#if defined(FUNC_GEMINILOCK)

    void FC_gLock(int argc, char * argv[]);
#endif //FUNC_GEMINILOCK
#if defined(_FUN_FunSerial)
    void FC_Serial(int argc, char * argv[]);
#endif //_FUN_FunSerial

#if defined(USE_PUOVERHTTP)

    void FC_SendPuMsg(int argc, char * argv[]);
#endif //USE_PUOVERHTTP
#if defined(USE_CUOVERHTTP)

    void FC_SendCuMsg(int argc, char * argv[]);
#endif //USE_CUOVERHTTP
#if defined(USE_SC1ClientOverTcp)

    void FC_SendSC1Msg(int argc, char * argv[]);
#endif //USE_CUOVERHTTP
#ifdef USE_SMSOVERHTTP

    void FC_SmsSend(int argc, char * argv[]);
#endif //USE_SMSOVERHTTP

#ifdef _USE_DBAGENT_OCCI
    void FC_dbAgent(int argc, char * argv[]);
#endif // _USE_DBAGENT_OCCI

#ifdef _FUNC_PULL_STREAMMING_SERVER
    void FC_PullStreammingServer(int argc, char * argv[]);
#endif //_FUNC_PULL_STREAMMING_SERVER

#ifdef _FUNC_gMiniLicense
    void FC_gMiniLicense(int argc, char * argv[]);
#endif //_FUNC_gMiniLicense

#ifdef _FUNC_PandoraMatrix
    void FC_PandoraMatrix(int argc, char * argv[]);
#endif //_FUNC_PandoraMatrix
#ifdef _FUNC_PandoraNode
    void FC_PandoraNode(int argc, char * argv[]);
#endif //_FUNC_PandoraNode

#if defined(_FUNC_PullTykdMobileService) || defined(_FUNC_TykdMobileService)
    void FC_PushTykd(int argc, char * argv[]);
#endif //_FUNC_PullTykdMobileService
#ifdef _FUNC_LiveStatusServer
    void FC_PublishStatus(int argc, char * argv[]);
#endif //_FUNC_LiveStatusServer

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    void FunctionHelp(int argc, char * argv[]);

    void TimerProcLive();

private:
	
    std::string m_strHelper;

    CEZObject *m_pObj;
    CONSOLE_CALLBACK m_pCallBack;

    CEZTimer          m_TimerAutoFunc;          //保活功能定时器
    // 启动定时器， 现在放到构造函数中， 减少使用者的调用麻烦
    //使用者一般按需new， 不要放到构造函数中
    // 写成函数便于将来改造为供外部单独调用start
    int Start();
    
    bool m_bAuto_iDev_Status;
};

#endif // !defined(_FunConsole_H)

