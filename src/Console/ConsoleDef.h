/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConsoleDef.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConsoleDef.h 5884 2014-04-01 09:05:51Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-04-01 09:05:51  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __ConsoleDef_H__
#define __ConsoleDef_H__

typedef const int           int_def;
//int_def SYS_MAX_PASSWD_LEN = 6;
//int_def SYS_MAX_USERNAME_LEN = 6;
//int_def SYS_MAX_GROUPNAME_LEN = 6;

#define  ASCII_CR		0x0d
#define  ASCII_LF		0x0a
#define  ASCII_BS		0x08
#define  ASCII_BELL		0x07
#define  ASCII_TAB		0x09
#define  ASCII_XON		0x11
#define  ASCII_XOFF		0x13
#define  ASCII_ESC		0x1B
#define  ASCII_DEL		0x7F
#define	 ASCII_BACK		0x08
//#define PROMPT_CONSOLE  "UniTrans"
//#define PROMPT_CONSOLE_OVER_TCP "[tcp@UniTrans]$ "

#ifndef _PROJECT_NAME
#define _PROJECT_NAME  "UniTrans"
#endif// _PROJECT_NAME

#define DEFAULT_USER_OF_CONSOLE  _PROJECT_NAME
// 提示符
#define PROMPT_CONSOLE  _PROJECT_NAME
#define PROMPT_CONSOLE_OVER_TCP "[tcp@"##_PROJECT_NAME##"]$ "

#define TOKEN_Console_01 "========================================================="
#define TOKEN_Console_02 "---------------------------------------------------------"

enum _con_stat
{
    DBG_OFF = 0,	//控制台关闭状态
    DBG_USRNAME,	//用户名输入状态
    DBG_PASSWD,		//密码输入状态
    DBG_LINE,		//命令行状态
};
enum _lvl_t
{
    LVL_NONE = 0,
    LVL_SPRTR,	//分割器 用来说明此类帮助
    LVL_USR,
    LVL_SYS,
    LVL_HID,//隐藏
};

class CEZObject;
// 回调函数
typedef void (CEZObject::*CONSOLE_CALLBACK)(const char *format, ...);
// 处理函数
//typedef void (CEZObject::*CONSOLE_PROC)(int argc, char * argv[], CEZObject *pObj, CONSOLE_CALLBACK pCallBack);
// 我们在构造函数中统一赋值callback了， 所以简化如下
typedef void (CEZObject::*CONSOLE_PROC)(int argc, char * argv[]);
//命令列表
typedef struct __console_func_node_t
{
    int			level;		//等级
    const char		*cmdStr;
    CONSOLE_PROC		proc;
    const char		*hlpStr;
}
CONSOLE_FUNC_NODE_T;

// 输入字符最大字节
#define MAX_LINELEN_CONSOLE 1024
// 最大参数项(argc, argv)
#define MAX_CONSOLE_ARGV_NUM 20
#define MAX_CONSOLE_ARGV_LEN 40

// 打印信息缓存
#define MAX_CONSOLE_SHOW_BUF 4096

// 定义为此数值的时候，认为是内部调用
#define INTERNAL_CALL_OF_ARGC 10000

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define CMD_CONSOLE_HELP  "help"
#define CMD_CONSOLE_LOGOUT  "logout"
#define CMD_CONSOLE_BYE  "bye"
#define CMD_CONSOLE_VERSION  "ver"

#endif// __ConsoleDef_H__
