/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralConsoleOverTcp.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralConsoleOverTcp.h 0001 2012-04-28 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -self dbg platform-
 *
 *  Update:
 *     2012-04-28 09:58:37 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _GeneralConsoleOverTcp_H
#define _GeneralConsoleOverTcp_H

#include "../Console/ConsoleDef.h"
#include <EZObject.h>

#include <TcpSocket.h>
#include <ISocketHandler.h>

#define LoginStatusNotlogin

enum    EnLoginStatus
{
    LoginStatusNotLogin =  0,
    LoginStatusWaitPwd,
    LoginStatusLoginUser,
    LoginStatusLoginAdmin,
};

class CFunConsole;

class CMaintainConsoleOverTcp : public TcpSocket, public CEZObject
{
public:
    CMaintainConsoleOverTcp(ISocketHandler&, std::string strSocketName="GeneralConsoleOverTcp");
    ~CMaintainConsoleOverTcp();
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // ¹¤¾ß
    /// Dump the help message
    void Helpme(int argc, char * argv[]);
    /// logout
    void iDev_Logout(int argc, char * argv[]);

    void OnAccept();
    void OnLine(const std::string& );

    void Update();

    virtual void OnDisconnect();

private:
    void tprintf(const char *format, ...);
    int OnCommand(int argc, char * argv[]);

    void DumpUserInfo(std::string home);
    void Kick(std::string userName);

    EnLoginStatus m_enLoginStatus;
    std::string m_strUName;
    std::string m_strUPwd;

    std::string m_strLastCmd;

    time_t m_ttLastUpdate;
    bool m_bAutoDumpUser;

    int m_iArgc;
    char m_strArgv[MAX_CONSOLE_ARGV_NUM+2][MAX_CONSOLE_ARGV_LEN+1];

    char *m_pArgv[MAX_CONSOLE_ARGV_NUM+2];

    CFunConsole *m_pFunConsole;

    time_t m_ttLastOnData;
};

#endif // _CommonConsoleOverTcp_H
