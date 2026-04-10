/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * LocalConsole.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: LocalConsole.cpp 5884 2013-12-26 03:28:44Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-12-26 03:28:44 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <iostream>
#include <string.h>
#include <stdarg.h>

#include "../UniTransDef.h"
#include "../Solar.h"
#include "FunConsole.h"
#include "LocalConsole.h"

#ifndef _DEBUG_THIS
//  #define _DEBUG_THIS
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

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define NO_UGM_MODEL 1 //没有用户管理

static	CONSOLE_FUNC_NODE_T	m_tFuncTable_CLocalConsole[] = {
            //Basic Function
            { LVL_SPRTR,"------",	NULL,				"LocalConsole ----------------------",},
            { LVL_USR, CMD_CONSOLE_HELP,		(CONSOLE_PROC)&CLocalConsole::Helpme,		"help of CLocalConsole",},
            { LVL_USR, CMD_CONSOLE_LOGOUT,	    (CONSOLE_PROC)&CLocalConsole::iDev_Logout,		CMD_CONSOLE_LOGOUT,},

        };

#define CONSOLE_FUNC_NODE_ITEMS sizeof(m_tFuncTable_CLocalConsole)/sizeof(CONSOLE_FUNC_NODE_T)

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

void CLocalConsole::iDev_Logout(int argc, char * argv[])
{
    m_iState = DBG_OFF;
    //m_cUgmUser.Logout(LoginAs_Net);
    ez_trace("iDev_Logout.\n");
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CLocalConsole::Helpme(int argc, char * argv[])
{
    unsigned int i;
    //	char *pszCmd0=GetCmd();

    for(i=0; i<CONSOLE_FUNC_NODE_ITEMS; i++)
    {
        if (m_tFuncTable_CLocalConsole[i].level == LVL_SPRTR)
        {
            ez_trace("------------------%s\n",m_tFuncTable_CLocalConsole[i].hlpStr);
            continue;
        }

        // else
        ez_trace("%-9s         %s\n", m_tFuncTable_CLocalConsole[i].cmdStr, m_tFuncTable_CLocalConsole[i].hlpStr);
    }
    ez_trace("-----------------------------------------------------\n");
    ez_trace("`cmd -h` for details.\n");
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
PATTERN_SINGLETON_IMPLEMENT(CLocalConsole);

void CLocalConsole::ThreadProc()
{
    //	char buf[MAX_LineLen];
    unsigned char ch = 0;

    while (m_bLoop)
    {
        fflush(stdout);

        ch = (char)getchar();
        if(ch)
        {
            DBG(
                ez_tracepoint();
                ez_trace("0x%0x\n", ch);
            )
            OnData(&ch, 1);
        }
    }
}

CLocalConsole::CLocalConsole() : CEZThread("LocalConsole", THREAD_PRIORITY_COM, 1024)/*, m_cUgmUser(CUgmManage::instance())*//*, m_Timer("Console")*/
{
    //    m_bShowContinue = EZTHREAD_BOOL_FALSE;
    m_iState = DBG_OFF;
    m_iWordPosition  = 0;


    m_pFunConsole = NULL;//new CFunConsole;
}

CLocalConsole::~CLocalConsole()
{
    if (m_pFunConsole)
    {
        delete m_pFunConsole;
    }
}

EZTHREAD_BOOL CLocalConsole::Start()
{
    DBG(
        ez_trace("CLocalConsole::Start()>>>>>>>>>>>>>>>>>\n");
    );
    //m_pFunConsole->Start();

    CreateThread();
    m_pFunConsole = new CFunConsole(this, (CONSOLE_CALLBACK )&CLocalConsole::OnShow);
    //assert(m_pFunConsole);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CLocalConsole::Stop()
{
    DestroyThread();
    return EZTHREAD_BOOL_TRUE;
}

void CLocalConsole::OnShow(const char *format, ...)
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

    ARG_USED(n);

    va_end(ap);

    fprintf(stdout, tmp);
}


/// 取到数据的调用
///
/// \param [in] pCaps 数据指针
/// \param [in] iLength 数据长度
/// \retval 0  获取成功。
/// \retval -1  获取失败。
void CLocalConsole::OnData(void *pDat, int iLength)
{
    char  l_dbData[2];
    EZTHREAD_BOOL l_bRet;
    char *l_pdbData = (char *)pDat;

    while(iLength--)
    {
        l_dbData[0] = *l_pdbData++;

        //        if(m_bShowContinue)
        //        {
        //            if(l_dbData[0] == 'q' || l_dbData[0] == 'Q') // 'q'为退出键
        //            {
        //                m_bShowContinue = EZTHREAD_BOOL_FALSE;
        //                ez_trace("\nDEBUG>");
        //            }
        //            else
        //            {
        //                //(this->*m_pCurProc)(0, NULL);	// ?
        //                if(m_bShowContinue)
        //                {
        //                    ez_trace("press 'Q' to quit, any other key to continue ...\n\n");
        //                }
        //                else
        //                {
        //                    ez_trace("\nDEBUG>");
        //                }
        //            }
        //            continue;
        //        }
        switch (l_dbData[0])
        {
            case ASCII_LF:
            case ASCII_CR:
            switch(m_iState)
            {
                case DBG_OFF:
                {
                    ez_trace("\f\n(none) login:");
                    m_iState = DBG_USRNAME;
                    m_iWordPosition = 0;
                    break;
                }

                case DBG_USRNAME:
                {
                    m_szData[m_iWordPosition++] = '\0';
                    //                    memcpy(m_szUserName, m_szData, SYS_MAX_USERNAME_LEN);
                    m_strUserName = m_szData;
                    //m_szUserName[SYS_MAX_USERNAME_LEN] = '\0';
                    ez_trace("\nPassword:");
                    m_iWordPosition = 0;
                    m_iState = DBG_PASSWD;

                    break;
                }
                case DBG_PASSWD:
                {
                    m_szData[m_iWordPosition++] = '\0';
                    //
                    // 调试用户作为网络用户登录，确保本地登录只有一个用户
                    //
#ifdef NO_UGM_MODEL

                    l_bRet = EZTHREAD_BOOL_TRUE;

                    //if (m_szUserName[0] == '\0')
                    if (0 == m_strUserName.size())
                    {
                        //strcpy(m_szUserName, DEFAULT_USER_OF_CONSOLE);
                        m_strUserName = DEFAULT_USER_OF_CONSOLE;
                    }
#else
                    //l_bRet = m_cUgmUser.Login(m_szUserName, m_szData, LoginAs_Net);
                    l_bRet = m_cUgmUser.Login(m_strUserName.c_str(), m_szData, LoginAs_Net);
#endif
                    //std::string u_name = m_szUserName;//m_cUgmUser.Login(m_szUserName, m_szData, LoginAs_Net);

                    if (EZTHREAD_BOOL_FALSE == l_bRet)
                    {
                        ez_trace("\nUser not valid!");
                        ez_trace("\n(none) login:");
                        m_iWordPosition = 0;
                        m_iState = DBG_USRNAME;
                    }
                    else
                    {
                        m_iWordPosition = 0;
                        m_iState = DBG_LINE;
                        ez_trace("\n[%s@%s]# "
#ifdef NO_UGM_MODEL
                              //, m_szUserName
                              , m_strUserName.c_str()
#else
                              //, /* m_cUgmUser.GetUser().*/u_name.c_str()
#endif
                              , PROMPT_CONSOLE);
                    }
                    break;
                }
                case DBG_LINE:
                {
                    if (m_iWordPosition > 0)
                    {
                        m_szData[m_iWordPosition++] = '\0';
                        l_dbData[0] = ASCII_CR;
                        l_dbData[1] = 0;
                        ez_trace(&l_dbData[0]);
                        l_dbData[0] = ASCII_LF;
                        l_dbData[1] = 0;
                        ez_trace(&l_dbData[0]);
                        StrParse();
                        m_iWordPosition = 0;
                    }
                    //                    if(m_bShowContinue)
                    //                    {
                    //                        ez_trace("	press 'Q' to quit, any other key to continue ...\n\n");
                    //                    }
                    //                    else
                    //                    {
                    ez_trace("\n[%s@%s]# "
#ifdef NO_UGM_MODEL
                          //, m_szUserName
                          , m_strUserName.c_str()
#else
                          //, /* m_cUgmUser.GetUser().*/u_name.c_str()
#endif
                          , PROMPT_CONSOLE);
                    //                    }
                    break;

                }
                break;
            }

            case ASCII_BACK:
            {
                if (m_iWordPosition)
                {
                    ez_trace("\b \b");
                    m_iWordPosition--;
                }
                break;
            }

            default:
            {
                m_szData[m_iWordPosition++] = l_dbData[0];
                if(m_iState == DBG_PASSWD)
                {
                    l_dbData[0] = '*';
                }
                l_dbData[1] = 0;
                ez_trace(&l_dbData[0], 1);
                if (m_iWordPosition >= MAX_LINELEN_CONSOLE)
                {
                    m_iWordPosition = 0;
                }
            }
        }
    }
    return;
}

/*	$FXN :
==	======================================================================
==	$DSC :	解析在命令行输入的字符串，并把解析出来的命令和参数都放在m_dbParamTab变量中
==	$ARG :
==		 :
==	$RET :
==	$MOD :
==	======================================================================
*/
void CLocalConsole::StrParse()
{
    int i = -1;
    int	j = 0;
    int k = 0;

    m_iParams = 0;
    while (m_szData[k])
    {
        if (k >= MAX_LINELEN_CONSOLE)
        {
            break;
        }
        if (m_szData[k] == ' ')
        {
            if (i >= 0)
                m_dbParamTab[i][j] = '\0';
            j = 0;
            k ++;
            continue;
        }
        if (j == 0)
        {
            if (++i >= 16)
                break;
            m_iParams = i;
        }
        m_dbParamTab[i][j++] = m_szData[k];
		
        if (j >= MAX_CONSOLE_ARGV_LEN)
        {
            while (m_szData[k])
            {
                if (k >= MAX_LINELEN_CONSOLE)
                {
                    break;
                }
                if (m_szData[k] == ' ')
                {
                    break;
                }
                k ++;
            }
        }
        k ++;
    }
    m_dbParamTab[i][j] = '\0';

    if (strlen(m_dbParamTab[0]) < 3)
    {
        //ez_trace("\nUnknow command:%s\n", m_dbParamTab[0]);
        ez_trace("\nUnknow command:`%s`, '%s' show command.\n", m_dbParamTab[0], CMD_CONSOLE_HELP);
        return;
    }

    // get argc and argv
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#if 0 // 目前挂掉， 还需查bug( 2014-04-04 15:53:19 )。。。。
    // 如果本次输入为空， 使用上次的命令及参数
    char *pCmd = GetCmd();
    if (NULL == pCmd)
    {
        // 无需解析参数
    }
    else
    {
        m_iArgc = 1;
        m_pArgv[0] = m_dbParamTab[0];
        m_pArgv[1] = pCmd;
#if 0
        // 外面已经判断，无需再次判断
        if (NULL == m_pArgv[1])
        {
            DBG(
                ez_trace("no cmd.\n");
            );
        }
        else
#endif

        {
            m_iArgc++;

            for (int ii=0; ii<=MAX_CONSOLE_ARGV_NUM; ii++)
            {
                m_pArgv[ii+2] = GetArg(ii);
                if (NULL == m_pArgv[ii+2])
                {
                    break;
                }
                else
                {
                    //ez_trace("m_pArgv[%d] = %s\n", ii+2, m_pArgv[ii+2]);
                    m_iArgc++;
                }
            }
        }
    }

#else
    m_iArgc = 1;
    m_pArgv[0] = m_dbParamTab[0];
    m_pArgv[1] = GetCmd();
    // 外面已经判断，无需再次判断
    if (NULL == m_pArgv[1])
    {
        DBG(
            ez_trace("no cmd.\n");
        );
    }
    else
    {
        m_iArgc++;

        for (int ii=0; ii<=MAX_CONSOLE_ARGV_NUM; ii++)
        {
            m_pArgv[ii+2] = GetArg(ii);
            if (NULL == m_pArgv[ii+2])
            {
                break;
            }
            else
            {
                //ez_trace("m_pArgv[%d] = %s\n", ii+2, m_pArgv[ii+2]);
                m_iArgc++;
            }
        }
    }
#endif

    // use in local
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    unsigned int iCmdUsed = 0;
    for (unsigned int ii=0; ii<CONSOLE_FUNC_NODE_ITEMS; ii++)
    {
        //	ez_trace("i->%d, s1->%s, s2->%s, l->%d\n", i, m_dbParamTab[0], m_sDbgTbl[i].cmdStr, strlen(m_dbParamTab[0]));
        if (strncmp(m_dbParamTab[0], m_tFuncTable_CLocalConsole[ii].cmdStr, strlen(m_dbParamTab[0])) == 0)
        {
            //(this->*m_tFuncTable_CLocalConsole[ii].proc)(m_iArgc, m_pArgv, this, (CONSOLE_CALLBACK )&CLocalConsole::OnShow);
            (this->*m_tFuncTable_CLocalConsole[ii].proc)(m_iArgc, m_pArgv);

            iCmdUsed |=0x1;

            break;
        }
    }

    // used in m_FunConsole
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //static CFunConsole m_FunConsole;
    if (m_pFunConsole)
    {
        //iCmdUsed |= m_pFunConsole->OnCommand(m_iArgc, m_pArgv, this, (CONSOLE_CALLBACK )&CLocalConsole::OnShow)>=0 ? 0x2 : 0x0;
        iCmdUsed |= m_pFunConsole->OnCommand(m_iArgc, m_pArgv)>=0 ? 0x2 : 0x0;
    }
    //ez_trace("\nUnknow command:`%s`, 'help' show command.\n", m_dbParamTab[0]);
    if (iCmdUsed==0)
    {
        ez_trace("\nUnknow command:`%s`, '%s' show command.\n", m_dbParamTab[0], CMD_CONSOLE_HELP);
    }
}

/// 取命令接口
///
/// \retval 0  获取成功。
/// \retval -1  获取失败。
char * CLocalConsole::GetCmd()
{
    char *str = m_dbParamTab[1];

    if (m_iParams < 1)
    {
        return 0;
    }
    if (str[0] != '-' && str[0] != '/')
    {
        return m_dbParamTab[1];
    }
    return &m_dbParamTab[1][1];
}

/// 取命令参数
///
/// \param [in] pos 参数位置
/// \retval *  获取成功。
/// \retval NULL  获取失败。
char * CLocalConsole::GetArg(int pos)
{
    if (m_iParams < (pos + 2))
    {
        return 0;
    }
    return m_dbParamTab[pos + 2];
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
