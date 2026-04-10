/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileIdxer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileIdxer.cpp 5884 2018-03-15 04:25:18Z WuJunjie $
 *
 *  Explain:
 *     -
 *      独立管理， 因此不回调信号
 *     -
 *
 *  Update:
 *     2018-03-15 04:25:18  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include <algorithm>

#include "ez_system_api.h"
#include "../../Logs.h"
#include "../../Solar.h"

#include "EZThread.h"
#include "EZTimer.h"

#include "FileIdxer.h"

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


//PATTERN_SINGLETON_IMPLEMENT(CFileIdxer);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CFileIdxer::CFileIdxer() :CEZThread("CFileIdxer", THREAD_PRIORITY_DEFAULT)
{
    //std::string m_strFilePath;
    m_iRenewTimes = 0;

    m_pFileList = NULL;
}

CFileIdxer::~CFileIdxer()
{
    if (m_pFileList==NULL)
    {
        delete m_pFileList ;
    }

}

EZTHREAD_BOOL CFileIdxer::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    if (NULL == m_pFileList)
    {
        m_pFileList = new CFileList_t;
        assert(m_pFileList);
    }

    int ret = CreateThread();

    ARG_USED(ret);

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CFileIdxer::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;

}

#define MAX_LEN_BYTES 1024

void CFileIdxer::ThreadProc()
{
    idxExistFiles();

    idxNotify();

}

// m_pSerial will be modify
int CFileIdxer::AddPath(const std::string &strPath)
{
    if (0 == strPath.size())
    {
        return -1;
    }

    CEZLock _lock(m_MutexFilePath);

    m_vFilePath.push_back(strPath);

    //ez_printf_info("Add:%s\n", strPath.c_str());

    return -1;
}

int CFileIdxer::readDir(const char *dir, const int depth, const int iCallTimes, const time_t ttNow, char *full_path)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    int spaces = depth*4;

    ARG_USED(spaces);

    if((dp = opendir(dir)) == NULL)
    {
        ez_printf_error("cannot open directory: %s\n", dir);

        return -1;
    }

    chdir(dir);

    if (full_path[0]!='\0')
        strcat(full_path, "/");

    strcat(full_path, dir);

    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name,&statbuf);

        if(S_ISDIR(statbuf.st_mode))
        {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 ||
               strcmp("..",entry->d_name) == 0)
                continue;

            //            printf("%*s%s/\n",spaces,"",entry->d_name);

            //strcat(full_path, "/");
            //strcat(full_path, entry->d_name);
            /* Recurse at a new indent level */


            // if (depth<2) 列出层级
            readDir(entry->d_name, depth+1, iCallTimes, ttNow, full_path);
        }
        else
        {
            //printf("|| %s ", full_path);
            printf("----%*s%s/%s : %ld\n", spaces ,"", full_path, entry->d_name, statbuf.st_mtim.tv_sec);
            // todo, interface add
            CFileNode __flieNode;
            __flieNode.strName = entry->d_name;
            __flieNode.strPath = full_path;
            __flieNode.mTime = statbuf.st_mtim.tv_sec;
            __flieNode.iSize = statbuf.st_size;
            m_pFileList->push_back(__flieNode);

#if 0

            CRecFileNode __node;
            __node.strName = entry->d_name;

            int iret = -1;//parseFileName(__node.strName, __node.strDevID, __node.startDt, __node.endDt);

            if (iret < 0)
            {
                ez_printf_debug("parseFileName:%d -- %s\n", iret, __node.strName.c_str());
            }
            else
            {
                __node.enFileType = iret;

                //__node.strName = entry->d_name;
                __node.strPath = full_path;
                //__node.strDevID = "0";
                //__node.startDt = 0;//从文件名取
                if (__node.endDt>=def_MAX_TIME_T_VALUE_INT)
                {
                    __node.endDt = statbuf.st_mtim.tv_sec;
                }
                __node.iSize = statbuf.st_size;
                __node.freshDt = time(NULL);

                addFile(__node);
            }
#endif

        }
    }

    chdir("..");

    full_path[strlen(full_path)-strlen(dir)-1] = '\0';

    closedir(dp);

    return 0;
}


int CFileIdxer::idxExistFiles()
{
    std::vector<std::string>::iterator it = m_vFilePath.begin();
    for ( ; it != m_vFilePath.end() ; it++)
    {
        char full_path[1024] = {'\0'};
        std::string __strFilePath = *it;

        ez_printf_info("strFilePath:%s\n", __strFilePath.c_str());

        int iret = readDir(__strFilePath.c_str(), 0, m_iRenewTimes, time(NULL), full_path);
        ARG_USED(iret);

    }

    return 0;
}

int CFileIdxer::idxNotify()
{
    if (0 == m_vFilePath.size())
    {
        ez_printf_error("FilePath not valid.\n");

        return -1;
    }

    std::string strCmd="inotifywait -mrq --timefmt '%Y%m%d%H%M%S' --format  '%T&&%w%f&&%e' --event create,move,delete";

    std::vector<std::string>::iterator it = m_vFilePath.begin();
    for ( ; it != m_vFilePath.end() ; it++)
    {
        strCmd+=(" " + *it);
    }

    ez_printf_debug("Cmd:[%s]\n", strCmd.c_str());

    FILE *pp = popen(strCmd.c_str(), "r"); //建立管道

    if (!pp)
    {
        return -2;
    }

    char tmp[MAX_LEN_BYTES]; //设置一个合适的长度，以存储每一行输出

    while (m_bLoop
           && fgets(tmp, sizeof(tmp)-1, pp) != NULL)
    {
        if (tmp[strlen(tmp) - 1] == '\n')
        {
            tmp[strlen(tmp) - 1] = '\0'; //去除换行符
        }

        ez_printf_info("::[%s]\n", tmp);

        dumpFileList(m_pFileList);
    }

    pclose(pp); //关闭管道

    return 0;
}

