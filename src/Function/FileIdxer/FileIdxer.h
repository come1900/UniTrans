/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileIdxer.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileIdxer.h 5884 2018-03-15 04:25:09Z WuJunjie $
 *
 *  Explain:
 *     -
 *      独立管理， 因此不回调信号
 *     -
 *
 *  Update:
 *     2018-03-15 04:25:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __FileIdxer_H__
#define __FileIdxer_H__

#include <vector>

#include "EZThread.h"
#include "EZSignals.h"
#include "ezBuffer.h"

#include "FileList.h"

//#define g_FileIdxer (*CFileIdxer::instance())

class CFileIdxer : public CEZThread
{
public:
    //PATTERN_SINGLETON_DECLARE(CFileIdxer);

    int AddPath(const std::string &strPath);

    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();

    void ThreadProc();

    CFileIdxer();
    virtual ~CFileIdxer();

private:
    // 添加int iCallTimes, const time_t ttNow 为了区分后续调用的时候， 历史文件不再重复读取
    int readDir(const char *dir, const int depth, const int iCallTimes, const time_t ttNow, char *full_path);
    //std::string m_strFilePath;
    int m_iRenewTimes;

    int idxExistFiles();
    int idxNotify();

    CEZMutex m_MutexFilePath;
    std::vector<std::string > m_vFilePath;

    CEZMutex m_MutexFileList;
    CFileList_t *m_pFileList;
};

#endif //__FileIdxer_H__
