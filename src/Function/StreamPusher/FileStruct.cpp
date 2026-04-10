/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileStruct.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileStruct.cpp 5884 2019-11-30 09:01:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-11-30 09:01:14  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "FileStruct.h"

CFileStruct::CFileStruct()
{
    //strName = "";// 名字 名字为索引
    startDt = 0;
    endDt = 0;
    iType = 0;//文件类型
    iSize = 0;//文件大小

    //strUriSnap = "";//抓图 链接
    //strDevIDChnStream = "";// 文件名
}

CFileStruct::~CFileStruct()
{}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CFileStructUri::CFileStructUri()
{}
CFileStructUri::~CFileStructUri()
{}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
bool compare_CFileStruct(const CFileStruct& first, const CFileStruct& second)
{
    return first.startDt>second.startDt;
}

bool compare_CFileStructUri(const CFileStructUri& first, const CFileStructUri& second)
{
    return first.startDt>second.startDt;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
bool CRecFileNode::operator==(const CRecFileNode& obj) const
{
    return this->strName==obj.strName;
}

bool CRecFileNode::operator==(const std::string & strName) const
{
    return this->strName==strName;
}

CRecFileNode & CRecFileNode::operator=(const CRecFileNode &tt)
{
    strName  = tt.strName     ;// 名字 名字为索引
    strPath  = tt.strPath     ;// 不包括域名， 前面直接拼域名，后拼接名字即是全路径
    strDevID = tt.strDevID    ;// devid_channel_stream
    startDt  = tt.startDt     ;
    endDt    = tt.endDt       ;
    iSize    = tt.iSize       ;//文件大小
    freshDt  = tt.freshDt     ;//数据从文件系统中的刷新时间，证明文件系统中还有此文件

    strNameSnap = tt.strNameSnap;// 截图文件名
    strNameQtFile = tt.strNameQtFile;// 支持快速播放的文件
    iDoStatus = tt.iDoStatus;//

    enFileType = tt.enFileType;

    return *this;
}

void CRecFileNode::dump(int mode) const
{
    if (mode==0)
    {
        printf("%38s %10s, %10s, %10s, %15s %s\n"
               , "Name"
               , "startDt"
               , "endDt"
               , "iSize"
               , "DevID"
               , "strNameSnap"
              );
    }
    else
    {
        //std::cout << strName << std::endl;
        printf("%38s %10lu, %10lu, %10d, %15s %s\n"
               , strName.c_str()
               , startDt
               , endDt
               , iSize
               , strDevID.c_str()
               , strNameSnap.c_str()
              );
    }
    //std::cout << PssMsg_Key_Session << ":" << m_strSession << std::endl;
}

// 升序
// push_front
bool compare_RecFileNode(const CRecFileNode& first, const CRecFileNode& second)
{
    return first.startDt>second.startDt;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "str_opr.h"
#include "ez_bit.h"
#include "sha1.h"
#include "StringOpr.h"
#include "ez_util_platform.h"
#include "str_opr.h"
#include <ez_url_parser.h>
#include "ezParse.h"
#include "StringOpr.h"
#include "dtPst.h"

#include "../../UniTransDef.h"
#include "../../Logs.h"

// return
//   -1, filename too short
//   -2, filename not valid
//   -3, start dt not valid
//   >0 filetype EnRecordFileType
int CRecordUtility::parseFileName(const std::string &strFName, std::string &strDevIDChnStream, time_t &ttStartDT, time_t &ttEndDT)
{
    if (strFName.length()>def_MAX_REC_FILENAME_LEN
        ||strFName.length()<2)
    {
        return -1;
    }

    CezParse __parser(strFName, def_REC_FILE_ELEM_SPLITER);

#if 0

    char strLeft[def_MAX_REC_FILENAME_LEN+1];
    char strRight[def_MAX_REC_FILENAME_LEN+1];
    int iret = split_str(
                   strFName.c_str(),
                   strLeft,
                   strRight,
                   def_REC_FILE_ELEM_SPLITER,
                   0);
#endif
    // 0-112A0000034_0_1-1488528570-1488528570.mp4
    if (__parser.Size() != 4)
    {
        return -2;
    }

    time_t __ttStartDT;
    ezConvertFromString(__ttStartDT, __parser.getWord(2));
    time_t __ttEndDT;
    ezConvertFromString(__ttEndDT, __parser.getWord(3));

    if (__ttStartDT<1456761600/*2017-03-01 00:00:00*/)
    {
        return -3;
    }

    //if (__parser.getWord(1).length()<15/*strlen 112A0000034_0_1*/)
    if (__parser.getWord(1).length()<5/*strlen 4_0_1*/) // 长度不管。。。
    {
        //printf("strLeft:%s\n", strLeft);
        return -4;
    }

    int fileType = __parser.getValue(0);
    if (fileType<EnRecordFileType_Record || fileType>=EnRecordFileType_MAX)
    {
        return -5;
    }

    strDevIDChnStream = __parser.getWord(1);
    ttStartDT = __ttStartDT>def_MIN_TIME_T_VALUE_INT?__ttStartDT:def_MIN_TIME_T_VALUE_INT;
    ttEndDT = __ttEndDT<def_MAX_TIME_T_VALUE_INT?__ttEndDT:def_MAX_TIME_T_VALUE_INT;

    return fileType;
}

void CRecordUtility::dump_listRecordFile(const tdefList_CFileStruct listRecordFile, int iMode)
{
    if (listRecordFile.empty() )
    {
        return;
    }

    if (1==iMode)
    {
        ez_printf_info_content("%2s %10s %10s %11s %s\n", "T", "startDt", "endDt","Size", "Name");
        ez_printf_info_content("---------------------------------------------------------------------------------\n");
        for (tdefList_CFileStruct::const_iterator it = listRecordFile.begin(); it != listRecordFile.end() ; it++)
        {
            ez_printf_info_content("%2d %10ld %10ld %11ld %s\n", it->iType, it->startDt, it->endDt, it->iSize, it->strName.c_str());
        }
    }

}

int CRecordUtility::readDirectory(  \
                                    const std::string &strPath
                                    , const time_t startDt
                                    , const time_t endDt
                                    , const std::string &strUriPrefix
                                    , tdefList_CFileStruct &listRecordFile)
{
    //ez_printf_debug("%s:(%s, %ld, %ld)\n", __FUNCTION__, strPath.c_str(), startDt, endDt);

    int iret = 0;
    DIR * dirp = NULL;
    struct dirent * pDirent= NULL;

    do
    {
        dirp = opendir(strPath.c_str());
        if (NULL == dirp)
        {
            //LOG_ERROR(("opendir %s failed! error_no: %s",pDirname , strerror(errno)));
            //ez_printf_error("opendir %s failed! error_no(%d): %s\n",strPath.c_str(), errno, strerror(errno));
            iret = -2;
            break;
        }

        pDirent = (struct dirent *)malloc(sizeof(struct dirent));
        if(!pDirent)
        {
            ez_printf_error(("malloc error"));
            iret = -3;
            break;
        }

        struct dirent *pStResult = NULL;

        while (( 0== readdir_r(dirp, pDirent, &pStResult))
               && (pStResult != NULL) )
        {
            if (pDirent->d_type == DT_DIR )
            {
                //ez_printf_debug("\t[%s]\n", pDirent->d_name);
                continue;
            }
            else if(pDirent->d_type == DT_REG/*This is a regular file.*/
                        /*All applications must properly handle a return of DT_UNKNOWN.
                        There are no guarantees that the file type can be read from d_type. You should fall back to using stat to get the information you need.
                        */
                        || (pDirent->d_type == DT_UNKNOWN && NULL!=pStResult->d_name && strlen(pStResult->d_name)>=def_Min_MyRecFileNameLen/*skip . .. dir name*/) 
                        )
            {
                std::string strDevIDChnStream;
                time_t ttStartDT;
                time_t ttEndDT;

                //ez_printf_debug("\t[%s]\n", pDirent->d_name);

                int ipRet = CRecordUtility::parseFileName(pStResult->d_name, strDevIDChnStream, ttStartDT, ttEndDT);

                // 文件名过滤
                if (ipRet<EnRecordFileType_Record || ipRet>=EnRecordFileType_MAX)
                {
                    ez_printf_debug("ipRet:%d\n", ipRet);

                    continue;
                }

                struct stat statbuf;
                const std::string __strFPName = strPath + std::string("/") + pStResult->d_name;
                int iretStat = stat(__strFPName.c_str(), &statbuf);

                if (0 != iretStat)
                {
                    ez_printf_debug("iretStat:%d\n", iretStat);

                    continue;
                }

                if (ttEndDT >= def_MAX_TIME_T_VALUE_INT)
                    ttEndDT = statbuf.st_mtime;

                //时间过滤
                if (!(startDt<=ttEndDT && endDt>=ttStartDT))
                {
                    //ez_printf_debug("file     [%s] [%ld]\n", pDirent->d_name, /*pDirent->d_reclen*/statbuf.st_size);

                    continue;
                }

                //大小过滤
#if 0
                if (statbuf.st_size <= 97008)
                {
                    continue;
                }
#endif
                //ez_printf_info("file     [%s] [%ld]\n", pDirent->d_name, /*pDirent->d_reclen*/statbuf.st_size);

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

    return iret;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

