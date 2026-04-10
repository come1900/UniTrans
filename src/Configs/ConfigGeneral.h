/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigGeneral.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigGeneral.h 5884 2012-07-02 09:15:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __CONFIGGENERAL_H__
#define __CONFIGGENERAL_H__

#include "ConfigBase.h"
//!	普通配置
typedef struct tagCONFIG_GENERAL
{
    int iLocalNo;               // 编号
    std::string strCode;        // 代号,  型号
    std::string strProductID;   //产品编号
    std::string strHostID;      //本机序列号
    std::string strAlias;       //机器名
    std::string strVendor;      // 客户
    std::string strLocation;    //位置
    std::string strDescription; //描述
    std::string strSerial;      // lic or serial 序列号
    std::string strVersion;     //版本
    // 日志文件路径 需要事前创建
    std::string strLogFilePath;
    // 日志文件名 如果长度为0， 则输出到屏幕
    std::string strLogFileName;
    int maxFileSize;
    int maxBackupIndex;
    int levelLog;

    int iLanguage;
    int iLanguageSupport;

    // 运行数据文件路径  比如数据库等
    std::string strDataFilePath;
    std::string strPid;
} CONFIG_GENERAL;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<CONFIG_GENERAL, 1, 4> CConfigGeneral;

#endif //__CONFIGGENERAL_H__
