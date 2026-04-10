/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecFileHelper.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecFileHelper.h 5884 2020-01-09 11:33:40Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2020-01-09 11:33:40  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_RecFileHelper_H)
#define _RecFileHelper_H

#include <iostream>
#include <vector>
#include <list>

#include "EZThread.h"

#include "FileStruct.h"

#include "../../Configs/ConfigLiveRecord.h"

typedef enum __em_fileReorganizeMode_t {
    em_fileReorganizeMode_Auto =  0,
    em_fileReorganizeMode_Immediately,
}EM_FileReorganizeMode_T;

class CRecordTykdMediaUrl;
class CRecFileHelper : public CEZThread
{
public:
    EZTHREAD_BOOL Start();
    EZTHREAD_BOOL Stop();
    void ThreadProc();

    CRecFileHelper(CRecordTykdMediaUrl *pRecordTykdMediaUrl);
    virtual ~CRecFileHelper();

private:
    // 截图
    int makeSnap(const CFileStruct *pFileNode, const std::string &strFilePath, const std::string &objFilePath, std::string &objFilename);
    // 快播文件
    int makeQtPlay(const CFileStruct *pFileNode, const std::string &strFilePath, const std::string &objFilePath, std::string &objFilename);
    // 浓缩动态文件
    int makeMotionFile(const CFileStruct *pFileNode, const std::string &strFilePath, const std::string &objFilePath, std::string &objFilename);
    // 整理
    int fileReorganize(EM_FileReorganizeMode_T iMode);

private:
    // 清理
    int fileClean(int iMode);

    CConfigLiveRecord *m_pcfgLiveRecord;
    CRecordTykdMediaUrl *m_pRecordTykdMediaUrl;
    int m_iFunMask;
    // 记录已经创建的目录
    //std::string m_strObjFilePath;

    // 已经清理的目录， 不需要维护
    std::string m_strFullPathNameRemoved;
};

#endif // !defined(_RecFileHelper_H)
