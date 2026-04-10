/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecFileHelper.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecFileHelper.h 5884 2017-02-28 10:11:07Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-28 10:11:07  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_RecFileHelper_H)
#define _RecFileHelper_H

#include <iostream>
#include <vector>
#include <list>

enum EnRecordFileType
{
    EnRecordFileType_Record = 1,  // 原始录像文件, tykdMobileServiceClient.cpp:859
    EnRecordFileType_QtPlay,
    EnRecordFileType_Snap,
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

class CRecFileHelper// : public CEZObject
{
public:
    CRecFileHelper();
    virtual ~CRecFileHelper();

    int setFilePath(const std::string &strFilePath);
    int setPrefix(const std::string &strPrefix);

    // 图片下载的目录
    int setFileDownloadPath(const std::string &strFilePath);

    // 维护文件索引
    int renew(int maskFun);
    int cutFiles(int maskFun);
	// 处理截图
    int makeSnap(const CRecFileNode *pFileNode, std::string &snapFile);
	// 处理快播文件
    int makeQtPlay(const CRecFileNode *pFileNode, std::string &snapFile);

    int addFile (const CRecFileNode &node);
    // 条件组合： 同查询
    int delFile (const CRecFileNode &node);
    int modFile (const CRecFileNode &node);
    // 条件组合：
    // 		strName
    // 		strDevID
    // 		strDevID, startDt, endDt
    size_t numFile (const CRecFileNode &node); // 返回数量
    size_t numFile (); // 返回数量

    int getFile ( \
                  const std::string &strDevid
                  , const time_t startDt
                  , const time_t endDt
                  , tdef_RecordList &listRecordFile); // 返回列表

    size_t dump(int mode);

private:

    int m_iRenewTimes;
    // 原始文件
    tdef_RecordList m_listRecordFile;

    // 播放文件
    tdef_RecordList m_listQtPlayFile;

    // 截图文件
    tdef_RecordList m_listSnapFile;

    std::string m_strFilePath;
    std::string m_strFileDownloadPath;
    std::string m_strPrefix;

    // 添加int iCallTimes, const time_t ttNow 为了区分后续调用的时候， 历史文件不再重复读取
    int readDir(const char *dir, const int depth, const int iCallTimes, const time_t ttNow, char *full_path);


    // return
    //   -1, filename too short
    //   -2, filename not valid
    //   -3, start dt not valid
    //   >0 filetype EnRecordFileType
    int parseFileName(const std::string &strFName, std::string &strDevIDChnStream, time_t &ttStartDT, time_t &ttEndDT);
};

#endif // !defined(_RecFileHelper_H)
