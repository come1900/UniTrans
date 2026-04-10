/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigDatabase.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigDatabase.h 5884 2014-10-20 08:59:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-20 08:59:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef __ConfigDatabase_H__
#define __ConfigDatabase_H__

#include "ConfigBase.h"
//!	
typedef struct tagConfigDatabase
{
    int iEnable;// 0-不用， 1 - 使用
    std::string strName;
    std::string strPasswd;

    std::string strDBString;
}
ConfigDatabase;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigDatabase, 1, 4> CConfigDatabase;

#endif //__ConfigDatabase_H__

