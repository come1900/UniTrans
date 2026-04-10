/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Logs.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Logs.h 5884 2012-08-13 01:54:12Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-08-13 01:54:12  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __LOGS_H__
#define __LOGS_H__

#if defined(EXAMPLE_LOG4CPP)

#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/loggingmacros.h>
#include <iomanip>
using namespace log4cplus;
using namespace log4cplus::helpers;

// 日志所属模块
//#define  LOG_ROOT  Logger::getRoot()
#define  LOG_SOLAR   Logger::getInstance(LOG4CPLUS_TEXT("SOLAR"))
#define  LOG_MAT   Logger::getInstance(LOG4CPLUS_TEXT("MAT"))
// general agent
#define  LOG_PORTS   Logger::getInstance(LOG4CPLUS_TEXT("PORTS"))
// tshome webservice
#define  LOG_WEBSERVICES  Logger::getInstance(LOG4CPLUS_TEXT("WEBS"))
#define  LOG_OF_SWEBS  Logger::getInstance(LOG4CPLUS_TEXT("SWEB"))
#define  LOG_OF_SMS  Logger::getInstance(LOG4CPLUS_TEXT("SMS"))
#else//EXAMPLE_LOG4CPP

#define LOG4CPLUS_INFO(x, y)
#define LOG4CPLUS_ERROR(x, y)
#define LOG4CPLUS_DEBUG(x, y)

#endif//EXAMPLE_LOG4CPP
//日志等级
/*
        LOG4CPLUS_DEBUG(LOG_IPC, "debug log" << i);
        LOG4CPLUS_INFO(LOG_IPC, "This is a int: " << 1000);
        LOG4CPLUS_WARN(LOG_IPC, "This is the THIRD log message...");
        LOG4CPLUS_ERROR(LOG_IPC, "This is the FOURTH log message...");
        LOG4CPLUS_FATAL(LOG_IPC, "This is the FOURTH log message... %d");
*/
#endif //__LOGS_H__
