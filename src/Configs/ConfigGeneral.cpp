/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigGeneral.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigGeneral.cpp 5884 2012-07-02 09:15:19Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../UniTransDef.h"
#include "ConfigGeneral.h"

#ifndef _VERSION
#define _VERSION "1.0.0"
#endif//_VERSION
#ifndef _LOG_FILENAME
#define _LOG_FILENAME "log-uniTrans.log"
#endif

template<>
void exchangeTable<CONFIG_GENERAL>(CConfigExchange& xchg, CConfigTable& table, CONFIG_GENERAL& config, int index, int app)
{
    //(CConfigTable& table, CConfigKey key, int& value, int min, int max, int dflt)
    xchg.exchange(table,"01_No",	   config.iLocalNo,	     0, 998 ,8		);

#if defined(_VEND_rainMan)
    xchg.exchange(table,"02_Alias", config.strAlias, "rainMan@pq.ezlibs.com");
    xchg.exchange(table,"03_Code", config.strCode, "R1");
    xchg.exchange(table,"04_ProductID", config.strProductID, "P987654321");
    xchg.exchange(table,"05_HostID", config.strHostID, "");//会在solor中初始化
    xchg.exchange(table,"06_Vendor", config.strVendor, "jsict.com");
    xchg.exchange(table,"07_Location", config.strLocation, "32.0427897449,118.7831210043");//金陵饭店
    xchg.exchange(table,"08_Description", config.strDescription, "rainman.ezlibs.com");
#elif defined(_VEND_sinVision)
    xchg.exchange(table,"02_Alias", config.strAlias, "sinvision@pq.ezlibs.com");
    xchg.exchange(table,"03_Code", config.strCode, "c6");
    xchg.exchange(table,"04_ProductID", config.strProductID, "P987654321");
    xchg.exchange(table,"05_HostID", config.strHostID, "");//会在solor中初始化
    xchg.exchange(table,"06_Vendor", config.strVendor, "sinvision.pq.ezlibs.com");
    xchg.exchange(table,"07_Location", config.strLocation, "39.9227897449,116.4631210043");//北京
    xchg.exchange(table,"08_Description", config.strDescription, "c6");
#elif defined(_VEND_VSA)
    xchg.exchange(table,"02_Alias", config.strAlias, "vsa@pq.ezlibs.com");
    xchg.exchange(table,"03_Code", config.strCode, "G1");
    xchg.exchange(table,"04_ProductID", config.strProductID, "P987654321");
    xchg.exchange(table,"05_HostID", config.strHostID, "");//会在solor中初始化
    xchg.exchange(table,"06_Vendor", config.strVendor, "vsa.pq.ezlibs.com");
    xchg.exchange(table,"07_Location", config.strLocation, "30.2711800000,120.1632800000");//杭州武林广场
    xchg.exchange(table,"08_Description", config.strDescription, "vsa");
#else
    xchg.exchange(table,"02_Alias", config.strAlias, def_Default_Alias);
    xchg.exchange(table,"03_Code", config.strCode, def_Default_Code);
    xchg.exchange(table,"04_ProductID", config.strProductID, "P987654321");
    xchg.exchange(table,"05_HostID", config.strHostID, "");//会在solor中初始化
    xchg.exchange(table,"06_Vendor", config.strVendor, "ezlibs.com");
    xchg.exchange(table,"07_Location", config.strLocation, "32.0427897449,118.7831210043");//金陵饭店
    xchg.exchange(table,"08_Description", config.strDescription, "cosmos");
#endif

#if defined(_FUNC_gMiniLicense)

    xchg.exchange(table,"09_Serial", config.strSerial, def_Default_SerialNumber);// 第二版
#else

    xchg.exchange(table,"09_Serial", config.strSerial, "08620543482132792993272592000");
#endif//_FUNC_gMiniLicense

    xchg.exchange(table,"10_Version", config.strVersion, _VERSION);

#if defined(_EZLOG_Embed)///////////////////////////////////////////////////////////////////////////////////_EZLOG_Embed
#if defined(LINUX)
    std::string __logPath = std::string("/tmp/logs");
    xchg.exchange(table,"11_maxLogSize", config.maxFileSize,  0, 5*1024*1024 ,4*1024);
#else //LINUX
    std::string __logPath = std::string("./logs");
    xchg.exchange(table,"11_maxLogSize", config.maxFileSize,  0, 5*1024*1024 ,5*1024*1024);
#endif //LINUX

    xchg.exchange(table,"12_maxLogFile", config.maxBackupIndex,  0, 8 ,2);
    xchg.exchange(table,"13_levelLog", config.levelLog,  0, 100 ,36);
#else ///////////////////////////////////////////////////////////////////////////////////_EZLOG_Embed

#if defined(LINUX)
    std::string __logPath = getenv("HOME") + std::string("/logs");
#else //LINUX
    std::string __logPath = std::string("./logs");
#endif //LINUX

    xchg.exchange(table,"11_maxLogSize", config.maxFileSize,  0, 50*1024*1024 ,9*1024*1024);
    xchg.exchange(table,"12_maxLogFile", config.maxBackupIndex,  0, 10 ,6);
    xchg.exchange(table,"13_levelLog", config.levelLog,  0, 100 ,36);
#endif///////////////////////////////////////////////////////////////////////////////////_EZLOG_Embed
    xchg.exchange(table,"14_LogFilePath", config.strLogFilePath, __logPath);
    xchg.exchange(table,"15_LogFileName", config.strLogFileName, _LOG_FILENAME);


    char * __dataPath = NULL;
#if defined(LINUX)
    __dataPath = getcwd(NULL, 0);
#endif //LINUX
    xchg.exchange(table,"16_DataFilePath", config.strDataFilePath, ((NULL==__dataPath) ? __logPath.c_str() : __dataPath));
    xchg.exchange(table,"17_Pid", config.strPid, (_PROJECT_NAME".pid"));

    xchg.exchange(table, "91_Language"    , config.iLanguage    , 0,0xffff, 1); // 0 - auto(by user's), 1 - chn 2 - eng
    xchg.exchange(table, "92_LanguageSupport"    , config.iLanguageSupport    , 0 , 0xffff, 0xffff); //mask
}

