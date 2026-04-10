/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecFileIdx.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecFileIdx.h 5884 2017-02-28 10:11:07Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-28 10:11:07  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_RecFileIdx_H)
#define _RecFileIdx_H

#include <iostream>
#include <vector>
#include <list>

#include "FileStruct.h"

class CRecFileIdx// : public CEZObject
{
public:
    CRecFileIdx();
    virtual ~CRecFileIdx();

    int setFilePath(const std::string &strFilePath);
    int setPrefix(const std::string &strPrefix);

    int setStoragePath(const std::string &strPath);
    int setStorageUrlPrefix(const std::string &strPrefix);
    int setCheapStoragePath(const std::string &strPath);
    int setCheapStorageUrlPrefix(const std::string &strPrefix);

    int findFile ( \
                  const std::string &strDevid
                  , const std::string &iChannel
                  , const std::string &iStream
                  , const time_t startDt
                  , const time_t endDt
                  , tdefList_CFileStruct &listRecordFile); // 返回列表

private:
    // 录像地址
    std::string m_strFilePath;
    std::string m_strPrefix;
    //热 存储
    std::string m_strStoragePath;//录像文件路径
    std::string m_strStorageUrlPrefix;//分发url
    // 冷存储
    std::string m_strCheapStoragePath;//录像文件路径
    std::string m_strCheapStorageUrlPrefix;//分发url

    //返回有用的文件
    int readDirectory ( \
                  const std::string &strPath
                  , const time_t startDt
                  , const time_t endDt
                  , const std::string &strUriPrefix
                  , tdefList_CFileStruct &listRecordFile); // 返回列表
};

#endif // !defined(_RecFileIdx_H)
