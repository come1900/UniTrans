/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * LocalConsole.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: LocalConsole.h 5884 2013-12-26 03:23:28Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 03:23:28 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __LocalConsole_H__
#define __LocalConsole_H__

#include "EZThread.h"
#include "ConsoleDef.h"

class CFunConsole;
class CLocalConsole : public CEZThread
{
public:
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 工具
    /// Dump the help message
    void Helpme(int argc, char * argv[]);
    /// logout
    void iDev_Logout(int argc, char * argv[]);


    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 工具内部使用
    PATTERN_SINGLETON_DECLARE(CLocalConsole);
    CLocalConsole();
    ~CLocalConsole();
    void ThreadProc();
    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    void OnShow(const char *format, ...);

private:
    void OnData(void *pDat, int iLength);
    char * GetArg(int pos);
    char * GetCmd();
    void StrParse();

    //EZTHREAD_BOOL m_bShowContinue;
    //当前的调试功能
    int		m_iState;
    int		m_iWordPosition;
    char	m_szData[MAX_LINELEN_CONSOLE];
    //char	m_szUserName[SYS_MAX_USERNAME_LEN+1];
    std::string m_strUserName;
    //命令行参数数量
    int		m_iParams;
    char	m_dbParamTab[MAX_CONSOLE_ARGV_NUM][MAX_CONSOLE_ARGV_LEN];

    int m_iArgc;
    char *m_pArgv[MAX_CONSOLE_ARGV_NUM+2];

    CFunConsole *m_pFunConsole;
};

#define g_LocalConsole (*CLocalConsole::instance())

#endif// __LocalConsole_H__
