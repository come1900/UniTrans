/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralConsoleOverTcp.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralConsoleOverTcp.cpp 0001 2012-04-28 09:58:37Z WuJunjie $
 *
 *  Explain:
 *     -self dbg platform-
 *
 *  Update:
 *     2012-04-28 09:58:37 WuJunjie Create
 *     2012-05-02 12:38:49 WuJunjie add login process, name=pwd is ok here
 *     2013-06-08 11:17:39 WuJunjie add st function
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <Utility.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

#include "../UniTransDef.h"
#include "str_opr.h"
#include <ezParse.h>
#include "EZThread.h"
#include "EZTimer.h"

#ifdef USE_ENOCEAN
#include "../../Device/EnOcean/enocean_mgr.h"
#endif

#ifdef _USE_MODULES_EZCONFIG
#include "../Configs/ConfigConsoleOverTcp.h"
#endif //_USE_MODULES_EZCONFIG

#include "../Console/FunConsole.h"

#include "GeneralConsoleOverTcp.h"
#include "GeneralAgentHandler.h"
#include "GeneralAgent.h"

#include "../Logs.h"

#include "GeneralSocketProcessor.h"

#ifdef EXAMPLE_THE_WEBSERVICE
#include "../WebService/AgentWebService.h"
#endif //
#include "LineProtocolCmd.h"

//#define DUMP_CONSOLE_MSG (pObj->*pCallBack)
#define DUMP_CONSOLE_MSG (this->tprintf)

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

#define NO_UGM_MODEL 1 //没有用户管理

static	CONSOLE_FUNC_NODE_T	m_tFuncTable_CMaintainConsoleOverTcp[] = {
            //Basic Function
            { LVL_SPRTR,"------",	NULL,				"CMaintainConsoleOverTcp -----------",},
            { LVL_USR, CMD_CONSOLE_HELP,		(CONSOLE_PROC)&CMaintainConsoleOverTcp::Helpme,		"help of CMaintainConsoleOverTcp",},
            { LVL_USR, CMD_CONSOLE_LOGOUT,	    (CONSOLE_PROC)&CMaintainConsoleOverTcp::iDev_Logout,		"Logout and disconnect me",},
            { LVL_USR, CMD_CONSOLE_BYE,	    (CONSOLE_PROC)&CMaintainConsoleOverTcp::iDev_Logout,		"Disconnect me",},

        };

#define CONSOLE_FUNC_NODE_ITEMS sizeof(m_tFuncTable_CMaintainConsoleOverTcp)/sizeof(CONSOLE_FUNC_NODE_T)

void CMaintainConsoleOverTcp::iDev_Logout(int argc, char * argv[])
{
    //m_iState = DBG_OFF;

    DUMP_CONSOLE_MSG(" exit now...\n");
    this->SetCloseAndDelete();
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CMaintainConsoleOverTcp::Helpme(int argc, char * argv[])
{
    unsigned int i;
    //	char *pszCmd0=GetCmd();

    for(i=0; i<CONSOLE_FUNC_NODE_ITEMS; i++)
    {
        if (m_tFuncTable_CMaintainConsoleOverTcp[i].level == LVL_SPRTR)
        {
            DUMP_CONSOLE_MSG("------------------%s\n",m_tFuncTable_CMaintainConsoleOverTcp[i].hlpStr);
            continue;
        }
        else if (m_tFuncTable_CMaintainConsoleOverTcp[i].level == LVL_NONE)
        {
            break;
        }

        // else
        DUMP_CONSOLE_MSG("%-9s         %s\n", m_tFuncTable_CMaintainConsoleOverTcp[i].cmdStr, m_tFuncTable_CMaintainConsoleOverTcp[i].hlpStr);
    }
    DUMP_CONSOLE_MSG("-----------------------------------------------------\n");
    DUMP_CONSOLE_MSG("`cmd -h` for details.\n");
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int CMaintainConsoleOverTcp::OnCommand(int argc, char * argv[])
{
    DBG(
        DUMP_CONSOLE_MSG("CMaintainConsoleOverTcp::OnCommand : %s>>>>>>>>>>>>> \n", argv[0]);
    );

    for (unsigned int i=0; i<CONSOLE_FUNC_NODE_ITEMS; i++)
    {
        if (strncmp(argv[0], m_tFuncTable_CMaintainConsoleOverTcp[i].cmdStr, strlen(argv[0])) == 0)
        {
            //(this->*m_tFuncTable_CMaintainConsoleOverTcp[i].proc)(argc, argv, pObj, pCallBack);
            (this->*m_tFuncTable_CMaintainConsoleOverTcp[i].proc)(argc, argv);

            return 0;
        }
    }

    return -1;
}


CMaintainConsoleOverTcp::CMaintainConsoleOverTcp(ISocketHandler& h, std::string strSocketName)
        :TcpSocket(h)
{
    SetLineProtocol();

    m_enLoginStatus = LoginStatusNotLogin;

    //SetSockName(strSocketName);

    m_strLastCmd = "";

    m_ttLastUpdate = 0;
    m_bAutoDumpUser = false;

    m_pFunConsole = NULL;

    m_ttLastOnData = 0;
}

void CMaintainConsoleOverTcp::OnAccept()
{
    m_ttLastOnData = time(NULL);

    m_pFunConsole = new CFunConsole(this, (CONSOLE_CALLBACK )&CMaintainConsoleOverTcp::tprintf);

    char strDT[GET_DATE_TIME_STRING_DEFAULT_STR_LEN];

    Send(" hostname : " + Utility::GetLocalHostname() + "\n");
    Send(" address  : " + Utility::GetLocalAddress() + "\n");

    tprintf(" datetime : %s\n", get_date_time_string(strDT, NULL));

    Send("Number of sockets in list : " + Utility::l2string(Handler().GetCount()) + "\n");

    tprintf("\n(none) login:");

    //LOG4CPLUS_INFO(LOG_SOLAR, "CMaintainConsoleOverTcp::OnAccept:" << GetRemoteAddress() << ":" << GetRemotePort());
    ez_printf_info("OnAccept:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}

void CMaintainConsoleOverTcp::OnDisconnect()
{
    if (m_pFunConsole)
    {
        delete m_pFunConsole;
    }

    ez_printf_info("OnDisconnect:%s:%d\n", GetRemoteAddress().c_str(), GetRemotePort());
}


CMaintainConsoleOverTcp::~CMaintainConsoleOverTcp()
{
    LOG4CPLUS_INFO(LOG_SOLAR, "CMaintainConsoleOverTcp::Destroy:" << GetRemoteAddress() << ":" << GetRemotePort());
}

void CMaintainConsoleOverTcp::tprintf(const char *format, ...)
{
    va_list ap;
    size_t n;
    char tmp[MAX_CONSOLE_SHOW_BUF];

    va_start(ap,format);
#ifdef _WIN32

    n = vsprintf(tmp,format,ap);
#else

    n = vsnprintf(tmp,MAX_CONSOLE_SHOW_BUF - 1,format,ap);
#endif

    va_end(ap);

    if (n>0)
    {
        SendBuf(tmp, n);
    }
}

void CMaintainConsoleOverTcp::OnLine(const std::string& InputLine)
{
    std::string CmdLine = InputLine;

    m_ttLastOnData = time(NULL);

    DBG(
        std::cout << CmdLine << std::endl << "m_enLoginStatus:";
        std::cout << (int)m_enLoginStatus << " | " <<std::endl;
    );


    if (m_enLoginStatus>LoginStatusWaitPwd)
    {
        if (CmdLine.size()<1)
        {
            CmdLine = m_strLastCmd;
        }
        else
        {
            m_strLastCmd = CmdLine;
        }
    }

    if (CmdLine.size() > 0)
    {
        if (LoginStatusNotLogin == m_enLoginStatus)
        {
            m_strUName = CmdLine;
            m_enLoginStatus = LoginStatusWaitPwd;
        }
        else if (LoginStatusWaitPwd == m_enLoginStatus)
        {
            m_strUPwd = CmdLine;
            ////////
            bool bAccountisOK = false;

            // 示例，相等即可
            if (m_strUName == m_strUPwd
                /*&& m_strUName=="yourPwd"*/)
            {
                bAccountisOK = true;
            }
            /////////////
            if (bAccountisOK)
            {
                m_enLoginStatus = LoginStatusLoginUser;
            }
            else
            {
                m_enLoginStatus = LoginStatusNotLogin;
            }
            //
        }
    }

    DBG(
        std::cout << "After:" << (int)m_enLoginStatus << " | " <<std::endl;
    );

    CezParse pa(CmdLine, " ");
    std::string cmd = pa.getWord();
    // 如果没有输入参数，则使用上次的命令
    if (cmd.size()>0)
    {
        m_iArgc = pa.Size();
        int ii;
        for (ii=0; ii<m_iArgc; ii++)
        {
            strncpy(m_strArgv[ii], pa.getWord(ii).c_str(), MAX_CONSOLE_ARGV_LEN);
            m_pArgv[ii] = &(m_strArgv[ii][0]);
        }

        std::string arg ;//= pa.getrest();
        DBG(
            std::cout << "cmd:" << cmd << std::endl;

            for (ii=0; ii<m_iArgc; ii++)
        {
            std::cout << "arg(" << ii <<"):" << m_strArgv[ii]
                << std::endl;
            }
        );
    }
    if (m_enLoginStatus < LoginStatusLoginUser)
    {
        // login
    }
    else if (CmdLine.size()<1)
    {
        tprintf("press h for help\n");
    }
    // doing...
    else
    {
        unsigned int iCmdUsed = 0;
        iCmdUsed = this->OnCommand(m_iArgc, m_pArgv/*, this, (CONSOLE_CALLBACK )&CMaintainConsoleOverTcp::tprintf*/);

        // used in m_FunConsole
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        //static CFunConsole m_FunConsole;
        if (m_pFunConsole)
        {
            iCmdUsed |= m_pFunConsole->OnCommand(m_iArgc, m_pArgv/*, this, (CONSOLE_CALLBACK )&CMaintainConsoleOverTcp::tprintf*/)>=0 ? 0x2 : 0x0;
        }
        //ez_trace("\nUnknow command:`%s`, 'help' show command.\n", m_dbParamTab[0]);
        if (iCmdUsed==0)
        {
            tprintf("\nUnknow command:`%s`, '%s' show command.\n", m_strArgv[0], CMD_CONSOLE_HELP);
        }
    }

    // 最后发送提示符
    if (LoginStatusNotLogin == m_enLoginStatus)
    {
        tprintf("\n(none) login:");
    }
    else if (LoginStatusWaitPwd == m_enLoginStatus)
    {
        tprintf("\nPassword:");
    }
    else
    {
        tprintf("\n[%s@%s]# "
#ifdef NO_UGM_MODEL
                //, m_szUserName
                , m_strUName.c_str()
#else
                //, /* m_cUgmUser.GetUser().*/u_name.c_str()
#endif
                , PROMPT_CONSOLE);
    }
}

void CMaintainConsoleOverTcp::Update()
{
    //printf("m_bAutoDumpPJ:%d m_bAutoDumpUser:%d\n", m_bAutoDumpPJ, m_bAutoDumpUser);
    int iDataTimeout = 30;
#ifdef _USE_MODULES_EZCONFIG
    CConfigConsoleOverTcp __cfg;
    __cfg.update();

    iDataTimeout = __cfg.getConfig().PeerTimeout;
#endif//

    //iDataTimeout = 10;
    if (time(NULL)-m_ttLastOnData>iDataTimeout)
    {
        tprintf("timed(%d) out waiting for input: auto-logout ...\n", iDataTimeout);

        SetCloseAndDelete();

        return;
    }

    // 防止过于频繁
    time_t ttNow = time(NULL);

    if (ttNow-m_ttLastUpdate<=2)
    {
        return;
    }

    m_ttLastUpdate = ttNow;

    if (m_bAutoDumpUser)
        DumpUserInfo("a");
}

void CMaintainConsoleOverTcp::DumpUserInfo(std::string home)
{
    tprintf("DumpUserInfo\n");
    tprintf("------------------------------------------------------------------------------\n");
}
