/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileStruct.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileStruct.h 5884 2019-11-30 08:59:52Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-11-30 08:59:52  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_FileStruct_H)
#define _FileStruct_H
#include <time.h>
#include <string>
#include <list>

#define def_Min_MyRecFileNameLen 25//"1--1591672637-2145888000.mp4"

class CFileStruct// : public CEZObject
{
public:

    CFileStruct();
    virtual ~CFileStruct();

    std::string strName;// 文件名
    time_t startDt;
    time_t endDt;
    int iType;//文件大小
    size_t iSize;//文件大小
    std::string strUri;// 链接

    std::string strNameSnap;// 抓图文件名
    std::string strUriSnap;//抓图 链接
    std::string strDevIDChnStream;// 文件名

    // 1型号文件 正在录像的
    std::string strNameCache;// 文件名
    time_t startDtCache;
    time_t endDtCache;

    //std::string strNameSnap;// 文件名
    std::string strNameMotionFile;// 抓图文件名
    std::string strUriMotionFile;//抓图 链接

private:

};

typedef std::list<CFileStruct> tdefList_CFileStruct;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
class CFileStructUri : public CFileStruct
{
public:

    CFileStructUri();
    virtual ~CFileStructUri();

    //std::string strUri;// 文件名
private:

};
typedef std::list<CFileStructUri> tdefList_CFileStructUri;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
enum EnRecordFileType
{
    EnRecordFileType_Record = 1,  // 原始录像文件, tykdMobileServiceClient.cpp:859
    EnRecordFileType_QtPlay = 2,
    EnRecordFileType_Snap = 3, // 抓图
    EnRecordFileType_MotionFile = 4,// 浓缩 RemovePartsWithoutMotion
    EnRecordFileType_MAX,
};

class CRecFileNode
{
public:
    std::string strName;// 名字 名字为索引
    std::string strPath;// 不包括域名， 前面直接拼域名，后拼接名字即是全路径
    std::string strDevID;// devid_channel_stream
    time_t startDt;
    time_t endDt;
    int iSize;//文件大小

    time_t freshDt;//数据从文件系统中的刷新时间，证明文件系统中还有此文件

    int enFileType;// EnRecordFileType 文件类型
    std::string strNameSnap;// 截图文件名
    std::string strNameQtFile;// 支持快速播放的文件

    int iDoStatus;//状态-- 0-暂时不能被查， 1-可以用

    CRecFileNode()
    {
        strName = "";// 名字 名字为索引
        strPath = "";// 不包括域名， 前面直接拼域名，后拼接名字即是全路径
        strDevID = "";// devid_channel_stream
        startDt = 0;
        endDt = 0;
        iSize = -1;//文件大小

        freshDt = 0;

        // 这两个不参与 == , = valid等判断
        strNameSnap = "";// 截图文件名
        strNameQtFile = "";// 支持快速播放的文件
        iDoStatus = 0;

        enFileType = EnRecordFileType_MAX;
    }

    bool operator==(const CRecFileNode& obj) const;
    bool operator==(const std::string & strName) const;

    CRecFileNode & operator=(const CRecFileNode &tt);

    void dump(int mode=1) const;

    //
    bool isDataValid() const
    {
        return strName.length()>0 \
               && strPath.length()>0 \
               && strDevID.length()>0 \
               && startDt>0 \
               && endDt>0 \
               && iSize>0 \
               ;
    }
};
typedef std::list<CRecFileNode> tdef_RecordList;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define def_MAX_REC_FILENAME_LEN 72//(sizeof("1-965c940c4ab1eb45fe215a024977ca1b0d9de356_1_1-1580248377-2145888000.mp4")) 
#define def_REC_FILE_ELEM_SPLITER "-"

class CRecordUtility// : public CEZObject
{
public:
    CRecordUtility();
    virtual ~CRecordUtility();

public:
    static int parseFileName(const std::string &strFName, std::string &strDevIDChnStream, time_t &ttStartDT, time_t &ttEndDT);
    static void dump_listRecordFile(const tdefList_CFileStruct listRecordFile, int iMode=0);
    //返回有用的文件
    static int readDirectory ( \
                  const std::string &strPath
                  , const time_t startDt
                  , const time_t endDt
                  , const std::string &strUriPrefix
                  , tdefList_CFileStruct &listRecordFile); // 返回列表

private:

};

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifdef __cplusplus
extern "C"
{
#endif

bool compare_CFileStruct(const CFileStruct& first, const CFileStruct& second);
bool compare_CFileStructUri(const CFileStructUri& first, const CFileStructUri& second);
bool compare_RecFileNode(const CRecFileNode& first, const CRecFileNode& second);

#ifdef __cplusplus
}
#endif//
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#endif // !defined(_FileStruct_H)
