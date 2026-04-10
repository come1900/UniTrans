/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecFileIdx.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecFileIdx.cpp 5884 2017-02-28 10:11:11Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-28 10:11:11  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

//#include <algorithm>
#include <errno.h>

#include "str_opr.h"
#include "StringOpr.h"
#include "ez_bit.h"
#include <ez_fs.h>
#include "ezParse.h"
#include "dtPst.h"

#include "../../UniTransDef.h"
#include "../../Logs.h"

//#include "StreamPusherTykd.h"
#include "FileStruct.h"
#include "RecFileIdx.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#define ARG_USED(x) (void)&x;

CRecFileIdx::CRecFileIdx()
{
}

CRecFileIdx::~CRecFileIdx()
{}

int CRecFileIdx::setFilePath(const std::string &strFilePath)
{
    if (strFilePath.length()<2)
    {
        return -1;
    }

    m_strFilePath = strFilePath;

    //ez_printf_debug("%s :%s\n", __FUNCTION__, m_strFilePath.c_str());

    return 0;
}

int CRecFileIdx::setPrefix(const std::string &strPrefix)
{
    if (strPrefix.length()<2)
    {
        return -1;
    }

    m_strPrefix = strPrefix;

    return 0;
}
//热 存储
int CRecFileIdx::setStoragePath(const std::string &strPath)
{
    if (strPath.length()<2)
    {
        return -1;
    }

    m_strStoragePath = strPath;

    return 0;
}

int CRecFileIdx::setStorageUrlPrefix(const std::string &strPrefix)
{
    if (strPrefix.length()<2)
    {
        return -1;
    }

    m_strStorageUrlPrefix = strPrefix;

    return 0;
}

// 冷存储
int CRecFileIdx::setCheapStoragePath(const std::string &strPath)
{
    if (strPath.length()<2)
    {
        return -1;
    }

    m_strCheapStoragePath = strPath;

    return 0;
}

int CRecFileIdx::setCheapStorageUrlPrefix(const std::string &strPrefix)
{
    if (strPrefix.length()<2)
    {
        return -1;
    }

    m_strCheapStorageUrlPrefix = strPrefix;

    return 0;
}

//读取qt  and snap file only
int CRecFileIdx::readDirectory(  \
                                 const std::string &strPath
                                 , const time_t startDt
                                 , const time_t endDt
                                 , const std::string &strUriPrefix
                                 , tdefList_CFileStruct &listRecordFile)
{
    int iret = 0;
    // 截图文件
    tdefList_CFileStruct __listRecordFileSnap;
    // 浓缩文件
    tdefList_CFileStruct __listRecordFileMotionFile;

    DIR * dirp = NULL;
    struct dirent * pDirent= NULL;

    do
    {

        dirp = opendir(strPath.c_str());
        if (NULL == dirp)
        {
            ez_printf_info("opendir %s failed! error_no: %s",strPath.c_str() , strerror(errno));
            iret = -2;
            break;
        }

        pDirent = (struct dirent *)malloc(sizeof(struct dirent));
        if(!pDirent)
        {
            ez_printf_info(("malloc error"));
            iret = -3;
            break;
        }

        struct dirent *pStResult = NULL;

        while (( 0== readdir_r(dirp,pDirent,&pStResult))
               && (pStResult != NULL))
        {
            if (pDirent->d_type == DT_DIR )
            {
                //ez_printf_info("dir      [%s]\n",pDirent->d_name);
                continue;
            }
            //else if(pDirent->d_type == DT_REG)
            else if(pDirent->d_type == DT_REG/*This is a regular file.*/
                        /*All applications must properly handle a return of DT_UNKNOWN.
                        There are no guarantees that the file type can be read from d_type. You should fall back to using stat to get the information you need.
                        */
                        || (pDirent->d_type == DT_UNKNOWN && NULL!=pStResult->d_name && strlen(pStResult->d_name)>=def_Min_MyRecFileNameLen) 
                        )
            {
                std::string strDevIDChnStream;
                time_t ttStartDT;
                time_t ttEndDT;
                int ipRet = CRecordUtility::parseFileName(pStResult->d_name, strDevIDChnStream, ttStartDT, ttEndDT);

                // 缓存抓图
                if (ipRet == EnRecordFileType_Snap)
                {
                    CFileStruct __fNode;
                    __fNode.strName = pStResult->d_name;
                    __fNode.startDt = ttStartDT;
                    __fNode.strDevIDChnStream = strDevIDChnStream;
                    //__fNode.endDt = ttEndDT;
                    //__fNode.iType = ipRet;
                    //__fNode.iSize = statbuf.st_size/1024;//KB为单位
                    //__fNode.strUri = strUriPrefix+pStResult->d_name;
                    __listRecordFileSnap.push_back(__fNode);
                }

                // 缓存浓缩
                if (ipRet == EnRecordFileType_MotionFile)
                {
                    CFileStruct __fNode;
                    __fNode.strName = pStResult->d_name;
                    __fNode.startDt = ttStartDT;
                    __fNode.strDevIDChnStream = strDevIDChnStream;
                    //__fNode.endDt = ttEndDT;
                    //__fNode.iType = ipRet;
                    //__fNode.iSize = statbuf.st_size/1024;//KB为单位
                    //__fNode.strUri = strUriPrefix+pStResult->d_name;
                    __listRecordFileMotionFile.push_back(__fNode);
                }

                if (ipRet != EnRecordFileType_QtPlay)
                {
                    continue;
                }

                if (!(startDt<=ttEndDT && endDt>=ttStartDT))
                {
                    continue;
                }

                struct stat statbuf;
                std::string __fullPathName = strPath + std::string("/") + pStResult->d_name;
                //ez_printf_info("malloc error", __fullPathName.c_str());
#if 0
                int iret_lstat = lstat(__fullPathName.c_str(), &statbuf);

                if (0!=iret_lstat)
                {
                    //memset(&statbuf, 0, sizeof (struct stat ));
                    statbuf.st_size = 0;
                }
#else
                int iretStat = stat(__fullPathName.c_str(), &statbuf);

                if (0 != iretStat)
                {
                    ez_printf_debug("iretStat:%d\n", iretStat);

                    continue;
                }
#endif
                //ez_printf_debug("iret_lstat(%d):[%s] [%ld]\n", iret_lstat, __fullPathName.c_str(), /*pDirent->d_reclen*/statbuf.st_size);

                CFileStruct __fNode;
                __fNode.strName = pStResult->d_name;
                __fNode.startDt = ttStartDT;
                __fNode.endDt = ttEndDT;
                __fNode.iType = ipRet;
                //__fNode.iSize = statbuf.st_size/1024;//KB为单位
                __fNode.iSize = statbuf.st_size;//Byte为单位
                __fNode.strUri = strUriPrefix+pStResult->d_name;
                __fNode.strDevIDChnStream = strDevIDChnStream;
                listRecordFile.push_back(__fNode);
            }
            else
            {
                ez_printf_debug("d_type:%ld\n", pDirent->d_type);
                continue;
            }
        }
    }
    while (0);

    if (dirp)
    {
        closedir(dirp);
    }
    if (pDirent)
    {
        free(pDirent);
    }

    //将抓图配置上
    for (tdefList_CFileStruct::iterator itSap = __listRecordFileSnap.begin(); itSap != __listRecordFileSnap.end(); itSap++)
    {
        for (tdefList_CFileStruct::iterator itFindF = listRecordFile.begin()
                ; itFindF != listRecordFile.end()
             ; itFindF++)
        {
            if (itFindF->strUriSnap.empty()// 未赋值
                && itSap->strDevIDChnStream == itFindF->strDevIDChnStream
                && itSap->startDt == itFindF->startDt)
            {
                itFindF->strUriSnap = strUriPrefix+itSap->strName;
            }
        }
    }

    //配置上MotionFile
    for (tdefList_CFileStruct::iterator itSap = __listRecordFileMotionFile.begin(); itSap != __listRecordFileMotionFile.end(); itSap++)
    {
        for (tdefList_CFileStruct::iterator itFindF = listRecordFile.begin()
                ; itFindF != listRecordFile.end()
             ; itFindF++)
        {
            if (itFindF->strUriMotionFile.empty()// 未赋值
                && itSap->strDevIDChnStream == itFindF->strDevIDChnStream
                && itSap->startDt == itFindF->startDt)
            {
                itFindF->strUriMotionFile = strUriPrefix+itSap->strName;
            }
        }
    }

    return iret;
}

int CRecFileIdx::findFile (  \
                             const std::string &strDevid
                             , const std::string &iChannel
                             , const std::string &iStream
                             , const time_t startDt
                             , const time_t endDt
                             , tdefList_CFileStruct &listRecordFile)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    const time_t __ttNow = time(NULL);
    const time_t __ttStart = startDt;
    const time_t __ttEnd = (endDt<__ttNow) ? endDt : __ttNow;

    if (__ttStart > __ttEnd)
    {
        ez_printf_error("para not valid:%ld, %ld\n", startDt, endDt);

        return -1;
    }

    // 只容许跨五天
    if (__ttEnd > (__ttStart+5*3600*24))
    {
        ez_printf_error("para not valid:%ld, %ld\n", startDt, endDt);

        return -2;
    }

    int iret = 0;

    std::string __strFileID = strDevid + "_" + (iChannel) + "_" + (iStream);
    ez_printf_debug("__strFileID:%s\n", __strFileID.c_str());

    // 处理跨天的查询
    std::list<std::string> __lstDt;

    for (time_t tt=__ttStart; tt<=__ttEnd; tt+=(3600*24))
    {
        time_t ttDt = tt;
        char __strdt[9]={'\0'};
        strftime(__strdt, sizeof(__strdt), "%Y%m%d\0",localtime(&ttDt));

        __lstDt.push_back(__strdt);
    }

    // 弥补因为小时的不一样
    {
        time_t __ttDt = __ttEnd;
        char __strdt[9]={'\0'};
        strftime(__strdt, sizeof(__strdt), "%Y%m%d\0",localtime(&__ttDt));

        if (__lstDt.empty()
            || (__lstDt.back() != std::string(__strdt))
           )
        {
            __lstDt.push_back(__strdt);
            //ez_printf_debug("corrective:%s\n", __strdt);
        }

    }//if (!__lstDt.empty())

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 二、 文件处理
    for (std::list<std::string>::const_iterator it = __lstDt.begin(); it != __lstDt.end() ; it++)
    {
        std::string __strDate = *it;
        ez_printf_debug("__strDate:[%s]\n", __strDate.c_str());

        // 1
        if (!m_strFilePath.empty())
        {
            std::string __strFilePathName = m_strFilePath + "/" + __strFileID + "/" + std::string(__strDate);
            std::string __strUriPrefix = m_strPrefix + "/" + __strFileID + "/" + std::string(__strDate) + "/";
            ez_printf_debug("__strFilePathName:%s\n", __strFilePathName.c_str());
            ez_printf_debug("__strUriPrefix:%s\n", __strUriPrefix.c_str());
            int iret_readDirectory = readDirectory(__strFilePathName, __ttStart, __ttEnd, __strUriPrefix, listRecordFile);
            ez_printf_debug("iret_readDirectory:%d, fileNum:%ld\n", iret_readDirectory, listRecordFile.size());
        }
        // 2
        if (!m_strStoragePath.empty())
        {
            std::string __strFilePathName = m_strStoragePath + "/" + __strFileID + "/" + std::string(__strDate);
            std::string __strUriPrefix = m_strStorageUrlPrefix + "/" + __strFileID + "/" + std::string(__strDate) + "/";
            ez_printf_debug("__strFilePathName:%s\n", __strFilePathName.c_str());
            ez_printf_debug("__strUriPrefix:%s\n", __strUriPrefix.c_str());
            int iret_readDirectory = readDirectory(__strFilePathName, __ttStart, __ttEnd, __strUriPrefix, listRecordFile);
            ez_printf_debug("iret_readDirectory:%d, fileNum:%ld\n", iret_readDirectory, listRecordFile.size());
        }
        // 3
        if (!m_strCheapStoragePath.empty())
        {
            std::string __strFilePathName = m_strCheapStoragePath + "/" + __strFileID + "/" + std::string(__strDate);
            std::string __strUriPrefix = m_strCheapStorageUrlPrefix + "/" + __strFileID + "/" + std::string(__strDate) + "/";
            ez_printf_debug("__strFilePathName:%s\n", __strFilePathName.c_str());
            ez_printf_debug("__strUriPrefix:%s\n", __strUriPrefix.c_str());
            int iret_readDirectory = readDirectory(__strFilePathName, __ttStart, __ttEnd, __strUriPrefix, listRecordFile);
            ez_printf_debug("iret_readDirectory:%d, fileNum:%ld\n", iret_readDirectory, listRecordFile.size());
        }
    }

    listRecordFile.sort(compare_CFileStruct);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // for debug, do not delete it
#if 0

    ez_printf_info_content("%2s %10s %10s %11s %44s\n", "T", "startDt", "endDt","Size", "Uri");
    ez_printf_info_content("---------------------------------------------------------------------------------\n");
    for (tdefList_CFileStruct::const_iterator it = listRecordFile.begin(); it != listRecordFile.end() ; it++)
    {
        if (EnRecordFileType_QtPlay == it->iType)
        {
            ez_printf_info_content("%2d %10ld %10ld %11ld %s\n", it->iType, it->startDt, it->endDt, it->iSize, it->strUri.c_str());
        }
    }
#endif

    return iret;
}

