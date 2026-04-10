/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileTailer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileTailer.cpp 5884 2019-09-20 02:01:08Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-09-20 02:01:08  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>/*sleep*/
#include <ez_fs.h>

#include "../../Logs.h"

#include "FileTailer.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

#if defined(_useSingletonPattern_FileTailer)
PATTERN_SINGLETON_IMPLEMENT(CFileTailer);
#endif //_useSingletonPattern_FileTailer
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CFileTailer::CFileTailer(std::string strFilePathName) :CEZThread("CFileTailer", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    //printf("CFileTailer Enter--------\n");

    m_strFilePathName = strFilePathName;
}

CFileTailer::~CFileTailer()
{
    //printf("CFileTailer Leave--------\n");

}

EZTHREAD_BOOL CFileTailer::Start(CEZObject * pObj, FileTailerSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    // 检查启动条件
    if (m_strFilePathName.length() < 1)
    {
        ez_printf_error("FilePathName not valid\n");
        return EZTHREAD_BOOL_FALSE;
    }

    int isFileExist = ez_check_file_exist(m_strFilePathName.c_str());
    if (isFileExist != 1)
    {
        ez_printf_error("%s not exist\n", m_strFilePathName.c_str());
        return EZTHREAD_BOOL_FALSE;
    }

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        ez_printf_error("attach error\n");
        return EZTHREAD_BOOL_FALSE;
    }

    if (m_iUser==0)
    {
        CreateThread();

        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        bRet = EZTHREAD_BOOL_TRUE;
    }
    m_iUser++;

    //ez_printf_debug("m_iUser:%d\n", m_iUser);

    return bRet;

}
EZTHREAD_BOOL CFileTailer::Stop(CEZObject * pObj, FileTailerSignalProc_t pProc)
{
    ez_printf_debug("CDevHDiskReader::Stop\n");

    EZTHREAD_BOOL bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
    CEZLock __lock(m_MutexSigBuffer);
    m_iUser--;

    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
    }
    else
    {
        // error
        __trip;
    }

    return bRet;
}

#define __MAX_fileLine_BYTES 1024
void CFileTailer::ThreadProc()
{
    int fileLineLen = 0;

    // 文件名已经构造
    std::string strCmd = "tail -F " + m_strFilePathName;
    FILE *pp = NULL;

    while (m_bLoop)
    {
#if 0
        sleep(1);
        const char *fileLine = "192.168.9.190 | - | [2019-09-24T09:01:33+08:00] | 200 | 0.188 | 0.188 | GET | jssfks.pq.ezlibs.com:10272 | /pq/list_PqDevice.php | 494 | 7613 | \"-\" | \"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.90 Safari/537.36\"";
        fileLineLen = strlen(fileLine);
#else

        if (pp == NULL)
        {
            pp = popen(strCmd.c_str(), "r");
        }

        if (pp == NULL)
        {
            sleep(10);
            continue;
        }

        char fileLine[__MAX_fileLine_BYTES] = {0};

        if (fgets(fileLine, sizeof(fileLine)-1, pp) != NULL)
        {
            fileLineLen = strlen(fileLine);

            if (fileLine[fileLineLen - 1] == '\n')
            {
                fileLine[fileLineLen - 1] = '\0';
            }
            //printf("::[%s]\n", fileLineLen);
        }
        else
        {
            fileLineLen = 0;
        }
#endif

        if (fileLineLen > 0)
        {
            // 回调
            m_SigBuffer(fileLineLen, fileLine);
        }//fileLineLen > 0
    }//while
}

