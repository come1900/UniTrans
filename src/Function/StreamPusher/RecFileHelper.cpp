/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecFileHelper.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecFileHelper.cpp 5884 2020-01-09 11:33:45Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2020-01-09 11:33:45  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <unistd.h>

#include <str_opr.h>
#include <ez_bit.h>
#include <ez_fs.h>
#include <StringOpr.h>

#include <ezParse.h>
#include <dtPst.h>

#include "../../UniTransDef.h"
#include "../../Logs.h"

#include "RecordTykdMediaUrl.h"

#include "RecFileHelper.h"

//#define _DEBUG_THIS
#include <ez_def_devel_debug.h>

CRecFileHelper::CRecFileHelper(CRecordTykdMediaUrl *pRecordTykdMediaUrl):CEZThread("CRecFileHelper", THREAD_PRIORITY_DEFAULT)
{
    m_pcfgLiveRecord = NULL;

    m_pRecordTykdMediaUrl = pRecordTykdMediaUrl;
    assert(m_pRecordTykdMediaUrl);

    m_iFunMask = 0;
}

CRecFileHelper::~CRecFileHelper()
{}

EZTHREAD_BOOL CRecFileHelper::Start()
{
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    // 全局变量
    //static CConfigLiveRecord __cfgRF;
    //__cfgRF.update();
    m_pcfgLiveRecord = new CConfigLiveRecord;
    assert(m_pcfgLiveRecord);

    m_pcfgLiveRecord->update();

    m_iFunMask = m_pcfgLiveRecord->getConfig().iFuncMask;

    int ret = CreateThread();

    return ret;
}

EZTHREAD_BOOL CRecFileHelper::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    int iret = fileReorganize(em_fileReorganizeMode_Immediately);
    ARG_USED(iret);

    return EZTHREAD_BOOL_TRUE;
}
void CRecFileHelper::ThreadProc()
{
    while (m_bLoop)
    {
        int iret = fileReorganize(em_fileReorganizeMode_Auto);
        ARG_USED(iret);

        if (BitGet(m_iFunMask, 2))
        {
            int iret = fileClean(0);
            ARG_USED(iret);
        }

        //ez_printf_debug("pd:%d, fs:%d\n", m_pRecordTykdMediaUrl->m_iFileHelperPeriod, m_pRecordTykdMediaUrl->getFileSegTime());

        if (m_pRecordTykdMediaUrl->m_iFileHelperPeriod>10)
            sleep(m_pRecordTykdMediaUrl->m_iFileHelperPeriod);
        else
            sleep(m_pRecordTykdMediaUrl->getFileSegTime());
    }
}

int CRecFileHelper::makeSnap(const CFileStruct *pFileNode, const std::string &strFilePath, const std::string &objFilePath, std::string &objFilename)
{
    const std::string strPath = strFilePath;//pFileNode->strPath;
    const std::string strName = pFileNode->strName;

    int iret = 0;

    std::stringstream __newFileName;

    __newFileName \
    << EnRecordFileType_Snap
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->strDevIDChnStream
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->startDt
    << def_REC_FILE_ELEM_SPLITER
    << def_MAX_TIME_T_VALUE_STR // 一开始就抓图 此时还不清楚enddt 这样写将文件名固定
    << ".jpg"
    ;

    objFilename = __newFileName.str();

    std::string __cmd ;
    makeCmdStringSnapOfFile(strPath, strName, objFilePath, __newFileName.str(), __cmd);

    int iret_system = system(__cmd.c_str());

    ez_printf_debug("Done(%d)\n", iret_system);
    //ez_printf_debug("Done(%d):%s\n", iret_system, __cmd.c_str());

    //
    std::string __strFullPathNameObj = objFilePath + "/" + objFilename;
    // 文件转码失败则直接move为新
    int iret_fileExist = ez_check_file_exist(__strFullPathNameObj.c_str());

    if (iret_fileExist != 1)
    {
        ez_printf_error("ez_check_file_exist:(%d):%s\n", iret_fileExist, __strFullPathNameObj.c_str());
        iret = -100;
    }

    return iret;
}

// 处理快播文件
int CRecFileHelper::makeQtPlay(const CFileStruct *pFileNode, const std::string &strFilePath, const std::string &objFilePath, std::string &objFilename)
{
    const std::string strPath = strFilePath;//pFileNode->strPath;
    const std::string strName = pFileNode->strName;

    int iret = 0;

    std::stringstream __newFileName;
    __newFileName
    << EnRecordFileType_QtPlay
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->strDevIDChnStream
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->startDt
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->endDt
    << ".mp4"
    ;

    objFilename = __newFileName.str();

    std::string __cmd ;
    makeCmdStringQtPlayOfFile(strFilePath, strName, objFilePath, __newFileName.str(), __cmd);

    int iret_system = system(__cmd.c_str());

    ez_printf_debug("Done(%d)\n", iret_system);
    //ez_printf_debug("Done(%d):%s\n", iret_system, __cmd.c_str());

    return iret;
}

// 处理Motion 文件
int CRecFileHelper::makeMotionFile(const CFileStruct *pFileNode, const std::string &strFilePath, const std::string &objFilePath, std::string &objFilename)
{
    const std::string strPath = strFilePath;//pFileNode->strPath;
    const std::string strName = pFileNode->strName;

    int iret = 0;

    std::stringstream __newFileName;
    __newFileName
    << EnRecordFileType_MotionFile
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->strDevIDChnStream
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->startDt
    << def_REC_FILE_ELEM_SPLITER
    << pFileNode->endDt
    << ".mp4"
    ;

    objFilename = __newFileName.str();

    std::string __cmd ;

    //__newFileName << "pssixman -i" 2-FirstFloorDevel_1_1-1582074682-1582074981.mp4 -vf "select=gt(scene\,0.002),setpts=N/(25*TB)" -c:a copy -movflags +faststart -y << __newFileName.str();

    iret = makeCmdStringMotionFileOfFile(strFilePath, strName, objFilePath, __newFileName.str(), __cmd);
    if (iret < 0)
    {
        return -1;
    }

    int iret_system = system(__cmd.c_str());
    ez_printf_debug("Done(%d)\n", iret_system);
    //ez_printf_debug("makeMotionFile(%d):%s\n", iret_system, __cmd.c_str());

    return iret;
}


int CRecFileHelper::fileReorganize(EM_FileReorganizeMode_T iMode)
{
    int iret = 0;

    do
    {
        // 一、 文件发现
        tdefList_CFileStruct listRecordFile;
        std::string __strFilePathName;
        std::string __strUriPrefix;
        time_t __ttNow = time(NULL);

        if (m_pRecordTykdMediaUrl->m_strRecPath.empty())
        {
            ez_printf_error("m_strRecPath.empty()\n");
            iret = -100;

            break;
        }

        __strFilePathName = m_pRecordTykdMediaUrl->m_strRecPath;

        //ez_printf_debug("readDirectory:(%s, %ld, %ld)\n", __strFilePathName.c_str(), 0, __ttNow);
        //
        int iret_readDirectory = CRecordUtility::readDirectory(__strFilePathName, 0/*startDt*/, __ttNow/*endDt*/, __strUriPrefix, listRecordFile);
        if (iret_readDirectory < 0)
        {
            ez_printf_error("Error, iret_readDirectory:%d\n", iret_readDirectory);
            iret = -101;

            break;
        }

        if (listRecordFile.empty())
        {
            ez_printf_debug("listRecordFile.empty()\n");
            iret = -101;

            break;
        }

        listRecordFile.sort(compare_CFileStruct);

        //ez_printf_debug("iret_readDirectory:%d, fileNum:%ld\n", iret_readDirectory, listRecordFile.size());
        //CRecordUtility::dump_listRecordFile(listRecordFile, 1);

        // 二、 文件处理
        for (tdefList_CFileStruct::const_iterator it = listRecordFile.begin(); it != listRecordFile.end() ; it++)
        {
            //ez_printf_info_content("%2d %10ld %10ld %11ld %s\n", it->iType, it->startDt, it->endDt, it->iSize, it->strName.c_str());

            // 跳过最新的正在录制的文件
            if ( (em_fileReorganizeMode_Immediately != iMode) // 立即处理的不跳过
                && it == listRecordFile.begin()
                && (__ttNow < it->endDt+m_pRecordTykdMediaUrl->getFileSegTime())
               )
            {
                // ez_printf_debug("Skip:%s\n", it->strName.c_str());

                continue;
            }// if

            // 跳过非录像文件
            if (it->iType != EnRecordFileType_Record)
            {
                ez_printf_debug("Skip:%s\n", it->strName.c_str());

                continue;
            }// if

            // 跳过过小(录像异常)的录像文件
            if (it->iSize <= 97008)
            {
                //std::string __cmd = "rm -f " +__strFilePathName + "/" +it->strName;

                //int iret_system = system(__cmd.c_str());

                //ez_printf_debug("rm Short file:(%d):%s\n", iret_system, __cmd.c_str());

                std::string __strFullPathName = __strFilePathName + "/" + it->strName;

                int iret_system = remove
                                      (__strFullPathName.c_str());

                ez_printf_debug("Short file rm:(%d):%s\n", iret_system, __strFullPathName.c_str());

                continue;
            }// if

            // 针对文件的开始日期计
            time_t __ttFileStartDt = it->startDt;
            char __strFileStartDt[9];//={'\0'};
            strftime(__strFileStartDt, sizeof(__strFileStartDt), "%Y%m%d\0",localtime(&__ttFileStartDt));

            // 处理后的文件存放目录
            std::string strObjFilePath;

            // 旧方式不再支持
            //if (m_pRecordTykdMediaUrl->m_strRecPath.empty())
            //{
            //    strObjFilePath = __strFilePathName;
            //    // 旧方式不需要创建目录
            //}
            //else

            // 创建处理后的文件夹

            strObjFilePath = __strFilePathName + "/" + std::string(__strFileStartDt);

#if 0
            if (m_strObjFilePath != strObjFilePath)
            {
                int iretCreatDir = ez_creat_dir(strObjFilePath.c_str());

                if (iretCreatDir < 0)
                {
                    ez_printf_error("Failed ez_creat_dir(%d):%s\n", iretCreatDir, strObjFilePath.c_str());
                    continue;
                }
                else
                {
                    // 避免重复创建此目录
                    m_strObjFilePath = strObjFilePath;

                    //ez_printf_debug("ez_creat_dir(%d):%s\n", iretCreatDir, strObjFilePath.c_str());
                }
            }
#else
            // 文件夹不存在则创建
            int iret_fileExist_strObjFilePath = ez_check_file_exist(strObjFilePath.c_str());
            if (iret_fileExist_strObjFilePath != 1)
            {
                int iretCreatDir = ez_creat_dir(strObjFilePath.c_str());

                if (iretCreatDir < 0)
                {
                    ez_printf_error("Failed creat_dir(%d):%s\n", iretCreatDir, strObjFilePath.c_str());
                    continue;
                }
                else
                {
                    // 避免重复创建此目录
                    //m_strObjFilePath = strObjFilePath;

                    //ez_printf_debug("ez_creat_dir(%d):%s\n", iretCreatDir, strObjFilePath.c_str());
                }
            }
#endif
            int maskDo = 0;

            if (BitGet(m_iFunMask, 0))
            {
                //生成缩略图
                std::string __strFileName ;
                int iret = makeSnap(&(*it), __strFilePathName, strObjFilePath, __strFileName);

                if (iret >= 0)
                {
                    maskDo = BitSet(maskDo, 0);
                    //ci->strNameSnap = __strFileName;
                }

                // 考试院项目专属
#if defined(_FUNC_KaoShi__) //_FUNC_do_not_use_now
                else
                {
                    //ez_printf_error("Failed makeSnap(%d):%s\n", iret, __strFilePathName.c_str());
                    usleep(50*1000);
                    // try again
                    int iret_again = makeSnap(&(*it), __strFilePathName, strObjFilePath, __strFileName);
                    if (iret_again >= 0)
                    {
                        maskDo = BitSet(maskDo, 0);
                    }
                    else
                    {
                        ez_printf_error("Failed makeSnap(%d):%s\n", iret, __strFilePathName.c_str());
                    }
                }
#endif //_FUNC_KaoShi__
            }

            std::string strFileNameQt = "";
            if (BitGet(m_iFunMask, 1))
            {
                //视频文件二次处理
                int iret = makeQtPlay(&(*it), __strFilePathName, strObjFilePath, strFileNameQt);

                if (iret>=0)
                {
                    maskDo = BitSet(maskDo, 1);
                    //ci->strNameQtFile = __strFileName;
                }
                else
                {
                    ez_printf_error("Failed makeQtPlay(%d):%s\n", iret, __strFilePathName.c_str());
                }
            }

            // 动态视频浓缩
            if (m_pcfgLiveRecord
                && 1 == m_pcfgLiveRecord->getConfig().iFun_MotionFile)
            {
                //
                std::string __strFileName ;
                int iret = makeMotionFile(&(*it), __strFilePathName, strObjFilePath, __strFileName);

                if (iret>=0)
                {
                    maskDo = BitSet(maskDo, 2);
                }
                else
                {
                    ez_printf_error("Failed makeMotionFile(%d):%s\n", iret, __strFilePathName.c_str());
                }
            }

            // 处理完成
            if (maskDo != 0)
            {
                std::string __strFullPathName = __strFilePathName + "/" + it->strName;
                std::string __strFullPathNameObj = strObjFilePath + "/" + strFileNameQt;

                // 文件转码失败则直接move为新
                int iret_fileExist = ez_check_file_exist(__strFullPathNameObj.c_str());
                //ez_printf_debug("ez_check_file_exist:(%d):%s\n", iret_fileExist, __strFullPathNameObj.c_str());

                if (iret_fileExist != 1)
                {
                    ez_printf_error("file Not exist(%d):%s\n", iret_fileExist, __strFullPathNameObj.c_str());

                    int iret_system = rename(__strFullPathName.c_str(), __strFullPathNameObj.c_str());

                    ez_printf_debug("rename:(%d):[%s] -> [%s]\n", iret_system, __strFullPathName.c_str(), __strFullPathNameObj.c_str());
                }
                else
                {
                    ez_printf_debug("file exist(%d):%s\n", iret_fileExist, __strFullPathNameObj.c_str());

                    int iret_system = remove(__strFullPathName.c_str());

                    ez_printf_debug("rm:(%d):%s\n", iret_system, __strFullPathName.c_str());
                }

            }
        }// for
    }
    while (0);

    return iret;
}

int CRecFileHelper::fileClean(int iMode)
{
    ez_printf_info("On:%s\n", __FUNCTION__);

    time_t ttNow = time(NULL);
    time_t __ttFileClean = ttNow-m_pRecordTykdMediaUrl->getFileValidPeriod();
    // 清理上一天的文件夹
    time_t __ttFileCleanLastday = __ttFileClean-3600*24;

    char __strFileClean[9];//={'\0'};
    char __strFileCleanLastday[9];//={'\0'};
    strftime(__strFileClean, sizeof(__strFileClean), "%Y%m%d\0",localtime(&__ttFileClean));
    strftime(__strFileCleanLastday, sizeof(__strFileCleanLastday), "%Y%m%d\0",localtime(&__ttFileCleanLastday));

    // ez_printf_debug(" -- FileValidPeriod:%d, __strFileClean:%s, __strFileCleanLastday:%s\n"
    //                , m_pRecordTykdMediaUrl->getFileValidPeriod()
    //                , __strFileClean
    //                , __strFileCleanLastday);

    do
    {

        // 一、 文件发现
        tdefList_CFileStruct listRecordFileLastday;
        std::string __strFilePathNameLastday;
        std::string __strUriPrefixLastday;
        int iret_readDirectoryLastday = -99999;

        __strFilePathNameLastday = m_pRecordTykdMediaUrl->m_strRecPath + std::string("/") + std::string(__strFileCleanLastday);

        // 先判断是否存在
        int _fileExist_strFilePathNameLastday = ez_check_file_exist(__strFilePathNameLastday.c_str());

        if(m_strFullPathNameRemoved != __strFilePathNameLastday
                && 1 == _fileExist_strFilePathNameLastday)
        {
            //
            iret_readDirectoryLastday = CRecordUtility::readDirectory(__strFilePathNameLastday, 0/*startDt*/, __ttFileClean/*endDt*/, __strUriPrefixLastday, listRecordFileLastday);
            if (iret_readDirectoryLastday < 0)
            {
                ez_printf_error("Error, iret_readDirectory:%d\n", iret_readDirectoryLastday);
                //break;
            }

            //listRecordFileLastday.sort(compare_CFileStruct);
            ez_printf_debug("iret_readDirectoryLastday:%d, fileNum:%ld:--%s\n", iret_readDirectoryLastday, listRecordFileLastday.size(), __strFilePathNameLastday.c_str());
            //CRecordUtility::dump_listRecordFile(listRecordFileLastday, 1);
        }

        tdefList_CFileStruct listRecordFile;
        std::string __strFilePathName;
        std::string __strUriPrefix;
        int iret_readDirectory = -99999;
        {
            __strFilePathName = m_pRecordTykdMediaUrl->m_strRecPath + std::string("/") + std::string(__strFileClean);
            //
            iret_readDirectory = CRecordUtility::readDirectory(__strFilePathName, 0/*startDt*/, __ttFileClean/*endDt*/, __strUriPrefix, listRecordFile);
            if (iret_readDirectory < 0)
            {
                // ez_printf_error("Error, readDirectory:%d\n", iret_readDirectory);
                //break;
            }
            else
            {
                ez_printf_debug("iret_readDirectory:%d, fileNum:%ld:--%s\n", iret_readDirectory, listRecordFile.size(), __strFilePathName.c_str());
            }

            //listRecordFile.sort(compare_CFileStruct);
            
            //CRecordUtility::dump_listRecordFile(listRecordFile, 1);
        }

        // 二、 文件清理
        // Today
        if (iret_readDirectory>=0
            && !(listRecordFile.empty()))
        {
            for (tdefList_CFileStruct::const_iterator it = listRecordFile.begin(); it != listRecordFile.end() ; it++)
            {
                //std::string __cmd = "rm -f " +__strFilePathName + "/" +it->strName;

                //int iret_system = system(__cmd.c_str());

                std::string __strFullPathName = __strFilePathName + "/" + it->strName;

                int iret_system = remove
                                      (__strFullPathName.c_str());

                ez_printf_debug("rm:(%d):%s\n", iret_system, __strFullPathName.c_str());
            }// for
        }

        // Lastday
        if (iret_readDirectoryLastday>=0
            && !(listRecordFileLastday.empty()))
        {
            for (tdefList_CFileStruct::const_iterator it = listRecordFileLastday.begin(); it != listRecordFileLastday.end() ; it++)
            {
                //std::string __cmd = "rm -f " +__strFilePathNameLastday + "/" +it->strName;

                //int iret_system = system(__cmd.c_str());

                std::string __strFullPathName = __strFilePathNameLastday + "/" + it->strName;

                int iret_system = remove
                                      (__strFullPathName.c_str());

                ez_printf_debug("rm:(%d):%s\n", iret_system, __strFullPathName.c_str());

            }// for
        }

        // dir of Lastday
        if (1 == _fileExist_strFilePathNameLastday 
                && listRecordFileLastday.empty())
        {
            // clean empty dir
            int iret_system = remove
                                  (__strFilePathNameLastday.c_str());


            if( 0==iret_system)
            {
                ez_printf_debug("remove:(%d):%s\n", iret_system, __strFilePathNameLastday.c_str());

                m_strFullPathNameRemoved = __strFilePathNameLastday;
            }
            else
            {
                ez_printf_error("Failed remove:(%d):%s\n", iret_system, __strFilePathNameLastday.c_str());
            }// remove

        }


    }
    while (0);

    return 0;
}

