/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecFileHelper.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecFileHelper.cpp 5884 2017-02-28 10:11:11Z WuJunjie $
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

#include <algorithm>

#include "str_opr.h"
#include "StringOpr.h"
#include "ez_bit.h"

#include "str_opr.h"
#include <ez_fs.h>

#include "ezParse.h"
#include "StringOpr.h"
#include "dtPst.h"

#include "../../UniTransDef.h"
#include "../../Logs.h"

#include "RecFileHelper.h"

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
//#define def_MAX_REC_FILENAME_LEN (sizeof("01-112A0000034_00_01-2145888000-2145888000.mp4")) //r-112A0000034_0_1-1488532643-2145888000.mp4
// support meye
//#define def_MAX_REC_FILENAME_LEN (sizeof("01-010175010501420004_00_01-2145888000-2145888000.mp4")) //r-112A0000034_0_1-1488532643-2145888000.mp4
#define def_MAX_REC_FILENAME_LEN (sizeof("000001-ff8080816ad38e64016ad573311f0028_1_1-1558524981-2145888000.mp4")) //r-112A0000034_0_1-1488532643-2145888000.mp4
#define def_REC_FILE_ELEM_SPLITER "-"

//不处理非常旧的文件，加快处理速度
//#define _Fun_Do_Not_Proc_Very_Old_Files 1

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

CRecFileHelper::CRecFileHelper()
{
    m_strFilePath = "/home/wujj/html/rec/media_file";
    m_strPrefix = "http://202.102.101.133:18080/rec/media_file/";

    m_iRenewTimes = 0;
}

CRecFileHelper::~CRecFileHelper()
{}

int CRecFileHelper::setFileDownloadPath(const std::string &strFileDownloadPath)
{
    if (strFileDownloadPath.length()<2)
    {
        return -1;
    }

    m_strFileDownloadPath = strFileDownloadPath;

    return 0;
}

int CRecFileHelper::setFilePath(const std::string &strFilePath)
{
    if (strFilePath.length()<2)
    {
        return -1;
    }

    m_strFilePath = strFilePath;

    return 0;
}

int CRecFileHelper::setPrefix(const std::string &strPrefix)
{
    if (strPrefix.length()<2)
    {
        return -1;
    }

    m_strPrefix = strPrefix;

    return 0;
}

// 升序
// push_front
bool compare_RecFileNode(const CRecFileNode& first, const CRecFileNode& second)
{
    return first.startDt>second.startDt;
}

int CRecFileHelper::renew(int maskFun)
{
    char full_path[1024] = {'\0'};

    int iret = readDir(m_strFilePath.c_str(), 0, m_iRenewTimes, time(NULL), full_path);

    // 第一次排序，后续自动是按时间push back，因此无需再次排序
    if (m_iRenewTimes<1)
    {
        m_listRecordFile.sort(compare_RecFileNode);
    }
    else
    {}

    //
    if (maskFun != 0)
        for (tdef_RecordList::iterator ci = m_listRecordFile.begin(); ci != m_listRecordFile.end(); ++ci)
        {
            std::string __strFileName = "";
            int iret = 0;

            ci->iDoStatus = 1;//处理过

            if (BitGet(maskFun, 0))
            {
                //生成缩略图
                __strFileName = "";
                iret = makeSnap(&(*ci), __strFileName);
                if (iret>0)
                {
                    ci->strNameSnap = __strFileName;
                }
                else
                {
                    ci->iDoStatus = 0;//未处理过
                }
            }

            if (BitGet(maskFun, 1))
            {
                //生成快播文件
                __strFileName = "";
                iret = makeQtPlay(&(*ci), __strFileName);
                if (iret>0)
                {
                    ci->strNameQtFile = __strFileName;
                }
                else
                {
                    ci->iDoStatus = 0;//处理过
                }
            }
            //ez_printf_debug("__strFileName:%s\n", __strFileName.c_str());
        }

    m_iRenewTimes++;
    return iret;
}
#include "StreamPusherTykd.h"
// 删除旧文件
int CRecFileHelper::cutFiles(int maskFun)
{
    time_t ttNow = time(NULL);
    std::string strDevChnStreamID;
    int iFileSegTime;
    int iFileValidPeriod;

    //ez_printf_info("ttNow:%ld\n", ttNow);

    for (tdef_RecordList::iterator ci = m_listRecordFile.begin(); ci != m_listRecordFile.end(); ++ci)
    {
        if (ci->strName.length()<2
            || ttNow < ci->startDt+300)
        {
            //ez_printf_error("ttNow:%ld, startDt:%ld, strName:%s\n", ttNow, ci->startDt, ci->strName.c_str());
            continue;
        }

        int iret = g_StreamPusherTykd.getRecorderAttr(ci->strDevID, iFileSegTime, iFileValidPeriod);

        // unknow file, not in start
        if (0!=iret)
        {
            //ez_printf_error("Failed getRecorderAttr:%d\n", iret);
            continue;
        }

        //ez_printf_error("ttNow:%ld, startDt:%ld, iFileValidPeriod:%d\n", ttNow, ci->startDt, iFileValidPeriod);

        //old
        if (ttNow > ci->startDt+iFileValidPeriod)
        {
            //std::string __cmd = "";

            std::string __cmd = "rm -f " + ci->strPath + "/" + ci->strName;

            if (ci->strNameSnap.length()>2)
            {
                __cmd += " " + ci->strPath + "/" + ci->strNameSnap;
            }
            if (ci->strNameQtFile.length()>2)
            {
                __cmd += " " + ci->strPath + "/" + ci->strNameQtFile;
            }
            __cmd += " &";

            //std::cout << "+++++++++++++ Cmd:" << __cmd << std::endl;
            system(__cmd.c_str());

            // erash snap list
            tdef_RecordList::iterator itSnap = std::find (m_listSnapFile.begin(), m_listSnapFile.end(), ci->strNameSnap);
            // found
            if (itSnap != m_listSnapFile.end())
            {
                itSnap = m_listSnapFile.erase(itSnap);
            }

            // erash qtplay list
            tdef_RecordList::iterator itQtPlay = std::find (m_listQtPlayFile.begin(), m_listQtPlayFile.end(), ci->strNameQtFile);
            // found
            if (itQtPlay != m_listQtPlayFile.end())
            {
                itQtPlay = m_listQtPlayFile.erase(itQtPlay);
            }

            ez_printf_info("cutcut :%s\n", ci->strName.c_str());

            // erash rec list, 后删除主索引
            ci = m_listRecordFile.erase(ci);
        }

    }

    return 0;
}

// 文件大小?秒后不动， 则认为结束了
#define def_FileEndedTimeout 9
//#define def_PssixmanCmd "pssixman"
#define def_PssixmanCmd "/home/pss/bin/pssixman"
// 处理截图
// gpushman -i 1-112A0000034_0_1-1488793999-2145888000.mp4 -y -f image2 -t 0.001 1-112A0000034_0_1-1488793999-2145888000.mp4.jpg
int CRecFileHelper::makeSnap(const CRecFileNode *pFileNode, std::string &snapFile)
{
#if 0
    // 已经处理
    if (pFileNode->strNameSnap.length()>2)
    {
        return -2;
    }

    time_t ttNow = time(NULL);

    //正在录像 不处理
    if (ttNow <= pFileNode->startDt
        || (ttNow - pFileNode->startDt) > 5)
    {
        return -1;
    }
#endif
    const std::string strPath = pFileNode->strPath;
    const std::string strName = pFileNode->strName;

    int iret = 0;

    std::string snapFileName=strName;

    snapFileName = ezConvertToString(EnRecordFileType_Snap)
                   + std::string(def_REC_FILE_ELEM_SPLITER)
                   + pFileNode->strDevID
                   + std::string(def_REC_FILE_ELEM_SPLITER)
                   + ezConvertToString(pFileNode->startDt)
                   + std::string(def_REC_FILE_ELEM_SPLITER)
                   + def_MAX_TIME_T_VALUE_STR // 一开始就抓图 此时还不清楚enddt 这样写将文件名固定
                   //+ ezConvertToString(pFileNode->endDt)
                   + ".jpg"
                   ;

    // type
    //snapFileName.replace(snapFileName.begin(), snapFileName.begin()+1, ezConvertToString(EnRecordFileType_Snap)); //EnRecordFileType_Snap
    // ext name
    //snapFileName.replace(snapFileName.end()-3, snapFileName.end(),"jpg");

    tdef_RecordList::iterator itSnap = std::find (m_listSnapFile.begin(), m_listSnapFile.end(), snapFileName);

    // not found
    if (itSnap == m_listSnapFile.end())
    {
        //CRecFileNode node;
        //tdef_RecordList::iterator it = std::find (pFileList->begin(), pFileList->end(), node);
        std::string __cmd ;//= std::string(def_PssixmanCmd) + " -i " + strPath + "/" +strName +" -y -f image2 -t 0.001 " +  strPath + "/" + snapFileName;
        makeCmdStringSnapOfFile(strPath, strName, snapFileName, __cmd);
        int iret_system = system(__cmd.c_str());

        //ez_printf_debug("doCmd(%d):%s\n", iret_system, __cmd.c_str());
        ez_printf_info("doCmd(%d)\n", iret_system);
    }
    else // found
    {
        //tdef_RecordList::iterator itRecordFile = std::find (m_listRecordFile.begin(), m_listRecordFile.end(), snapFileName);
        snapFile = snapFileName;
        iret = 1;
    }

    return iret;
}

// todo
// 处理快播文件
// gpushman -i 1-112A0000034_0_1-1488793999-2145888000.mp4 -n -c copy -movflags +faststart 1-112A0000034_0_1-1488793999-2145888000.mp4.2.mp4
int CRecFileHelper::makeQtPlay(const CRecFileNode *pFileNode, std::string &snapFile)
{
    time_t ttNow = time(NULL);

    //该文件暂不需要处理
    if (ttNow <= pFileNode->endDt
        || (ttNow - pFileNode->endDt) < def_FileEndedTimeout) //
    {
        return -1;
    }

    // 已经处理
    if (pFileNode->strNameQtFile.length()>2)
    {
        return -2;
    }

    const std::string strPath = pFileNode->strPath;
    const std::string strName = pFileNode->strName;

    int iret = 0;

    std::string newFileName=strName;

    newFileName = ezConvertToString(EnRecordFileType_QtPlay)
                  + std::string(def_REC_FILE_ELEM_SPLITER)
                  + pFileNode->strDevID
                  + std::string(def_REC_FILE_ELEM_SPLITER)
                  + ezConvertToString(pFileNode->startDt)
                  + std::string(def_REC_FILE_ELEM_SPLITER)
                  + ezConvertToString(pFileNode->endDt)
                  + ".mp4"
                  ;
    //std::cout << "+++++++++++++ newFileName:" << newFileName << std::endl;

    // type
    //newFileName.replace(newFileName.begin(), newFileName.begin()+1, ezConvertToString(EnRecordFileType_Snap)); //EnRecordFileType_Snap
    // ext name
    //newFileName.replace(newFileName.end()-3, newFileName.end(),"jpg");

    tdef_RecordList::iterator itSnap = std::find (m_listQtPlayFile.begin(), m_listQtPlayFile.end(), newFileName);

    // not found
    if (itSnap == m_listQtPlayFile.end())
    {
        //CRecFileNode node;
        //tdef_RecordList::iterator it = std::find (pFileList->begin(), pFileList->end(), node);
        std::string __cmd = std::string(def_PssixmanCmd) + " -i " + strPath + "/" +strName +" -c copy -movflags +faststart " +  strPath + "/" + newFileName;
        __cmd += " &";

        //std::cout << "+++++++++++++ Cmd:" << __cmd << std::endl;
        //system(__cmd.c_str());
        int iret_system = system(__cmd.c_str());

        //ez_printf_info("doCmd(%d)\n", iret_system);
        ez_printf_debug("doCmd(%d):%s\n", iret_system, __cmd.c_str());
    }
    else // found
    {
        //tdef_RecordList::iterator itRecordFile = std::find (m_listRecordFile.begin(), m_listRecordFile.end(), newFileName);
        snapFile = newFileName;
        //std::cout << "+++++++++++++ found:" << newFileName << std::endl;
        iret = 1;
    }

    return iret;
}

//int CRecFileHelper::metabolism

// -1 -- 数据不全
// -2 -- 重名
int CRecFileHelper::addFile (const CRecFileNode &node)
{
#if 0
    if (!( node.strName.length()>0 \
           && node.strPath.length()>0 \
           && node.strDevID.length()>0 \
           && node.startDt>0 \
           && node.endDt>0 \
           && node.iSize>0 ))
#else

    if (!node.isDataValid())
#endif

    {
        return -1;
    }

    tdef_RecordList *pFileList = NULL;
    if (EnRecordFileType_Record == node.enFileType)
    {
        pFileList = &m_listRecordFile;
    }
    else if (EnRecordFileType_QtPlay == node.enFileType)
    {
        pFileList = &m_listQtPlayFile;
    }
    else if (EnRecordFileType_Snap == node.enFileType)
    {
        pFileList = &m_listSnapFile;
    }
    else
    {
        pFileList = NULL;
    }

    if (pFileList != NULL)
    {
        tdef_RecordList::iterator it = std::find (pFileList->begin(), pFileList->end(), node);

        // found
        if (it != pFileList->end())
        {
            // renew attrinfo
            if (it->iSize    != node.iSize)
            {
                *it = node;
            }

            return -2;
        }
        // 新的在前面
        //pFileList->push_back(node);
        pFileList->push_front(node);
    }

    return 0;
}

int CRecFileHelper::delFile (const CRecFileNode &node)
{
    return 0;
}
int CRecFileHelper::modFile (const CRecFileNode &node)
{
    return 0;
}
// 条件组合：
// 		strName
// 		strDevID
// 		strDevID, startDt, endDt
size_t CRecFileHelper::numFile (const CRecFileNode &node)
{
    return 0;
}

size_t CRecFileHelper::numFile ()
{
    return m_listRecordFile.size();
}

size_t CRecFileHelper::dump (int mode)
{
    if (m_listRecordFile.empty())
    {
        return 0;
    }

    m_listRecordFile.begin()->dump(0);

    //tdef_RecordList::iterator it;
    for (tdef_RecordList::iterator ci = m_listRecordFile.begin(); ci != m_listRecordFile.end(); ++ci)
    {
        ci->dump();
    }

    return m_listRecordFile.size();
}

int CRecFileHelper::getFile (  \
                            const std::string &strDevid
                            , const time_t startDt
                            , const time_t endDt
                            , tdef_RecordList &listRecordFile)
{
    if (m_listRecordFile.empty())
    {
        return -1;
    }

    listRecordFile.clear();

    DBG(
        dump(0);
        dump(1);
        printf("--- getFile ---\n");
        printf("+++++++++++ startDt:%lu, endDt:%lu\n", startDt, endDt);
    );
    for (tdef_RecordList::iterator ci = m_listRecordFile.begin(); ci != m_listRecordFile.end(); ++ci)
    {
        if (ci->strDevID==strDevid
            && (startDt<=ci->endDt && endDt>=ci->startDt)
            /*&& 1==ci->iDoStatus*/
            )
        {
#if 1
            if (ci->strNameQtFile.length() < 2)
            {
                ez_printf_debug("Pass:%s\n", ci->strName.c_str());
                continue;
            }
#endif
            listRecordFile.push_back(*ci);
            DBG(
                ci->dump();
            );
        }

    }

    return listRecordFile.size();
}

int CRecFileHelper::readDir(const char *dir, const int depth, const int iCallTimes, const time_t ttNow, char *full_path)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    int spaces = depth*4;

    ARG_USED(spaces);

    if((dp = opendir(dir)) == NULL)
    {
        //ez_printf_error("cannot open directory: %s\n", dir);
        //ez_creat_dir(dir);

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

            ///////////////////
#ifdef _Fun_Do_Not_Proc_Very_Old_Files
            // 过滤旧文件夹
            // 非第一次调用
            if (iCallTimes>0)
            {
                if (strlen(entry->d_name) == 8/*sizeof("20170413")*/
                    && entry->d_name[0] == '2'   			)// 2017....
                {
                    //ez_printf_info("bye, d_name:%s\n", entry->d_name);
                    //continue;

                    char fullDt[128] = {'\0'};
                    strcpy(fullDt, entry->d_name);
                    strcat(fullDt, " 23:59:59");

                    time_t ttFolder = string2date_time(fullDt, "%04d%02d%02d %02d:%02d:%02d");

                    //ez_printf_info("fullDt:%s, ttFolder:%llu\n", fullDt, ttFolder);

                    if (ttNow>ttFolder+3600) // 过后一个小时不再处理该文件夹
                    {
                        //ez_printf_info("bye the folder:%s, ttNow:%llu, ttFolder:%llu\n", entry->d_name, ttNow, ttFolder);
                        continue;
                    }
                    else
                    {
                        //ez_printf_info("proc folder:%s, ttNow:%llu, ttFolder:%llu\n", entry->d_name, ttNow, ttFolder);
                    }
                }
            }
#endif // _Fun_Do_Not_Proc_Very_Old_Files
            ///////////////////

            // if (depth<2) 列出层级
            readDir(entry->d_name, depth+1, iCallTimes, ttNow, full_path);
        }
        else
        {
            //printf("|| %s ", full_path);
            //            printf("%*s%s/%s : %ld\n",spaces,"", full_path, entry->d_name, statbuf.st_mtim.tv_sec);

            // 过滤旧文件
#ifdef _Fun_Do_Not_Proc_Very_Old_Files
            if (iCallTimes>0)
            {
                if (ttNow>statbuf.st_mtim.tv_sec+3600/*fresh interval*/)
                {
                    continue;
                }
            }
#endif // _Fun_Do_Not_Proc_Very_Old_Files

            CRecFileNode __node;
            __node.strName = entry->d_name;

            int iret = parseFileName(__node.strName, __node.strDevID, __node.startDt, __node.endDt);

            if (iret < 0)
            {
                //ez_printf_debug("parseFileName:%d -- %s\n", iret, __node.strName.c_str());
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
        }
    }

    chdir("..");

    full_path[strlen(full_path)-strlen(dir)-1] = '\0';

    closedir(dp);

    return 0;
}

// return
//   -1, filename too short
//   -2, filename not valid
//   -3, start dt not valid
//   >0 filetype EnRecordFileType
int CRecFileHelper::parseFileName(const std::string &strFName, std::string &strDevIDChnStream, time_t &ttStartDT, time_t &ttEndDT)
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
    ttStartDT = __ttStartDT>=def_MIN_TIME_T_VALUE_INT?__ttStartDT:def_MIN_TIME_T_VALUE_INT;
    ttEndDT = __ttEndDT>=def_MAX_TIME_T_VALUE_INT?__ttEndDT:def_MAX_TIME_T_VALUE_INT;

    return fileType;
}

