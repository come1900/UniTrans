/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * RecordTykdMediaUrl.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: RecordTykdMediaUrl.h 5884 2017-02-17 05:56:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-17 05:56:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/



#if !defined(_RecordTykdMediaUrl_H)
#define _RecordTykdMediaUrl_H

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

#include "RecFileHelper.h"

#include "Pusher.h"

#define def_max_ThreadNameLen 16

class CRecordTykdMediaUrl : public CPusher//CEZThread
{
public:

    EZTHREAD_BOOL Start(CEZObject * pObj, PusherSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, PusherSignalProc_t pProc);

    CRecordTykdMediaUrl();
    virtual ~CRecordTykdMediaUrl();

    int setFileSegTime(const int iFileSegTime);
    int setFileSegTime(const std::string &strFileSegTime);
    int setFileValidPeriod(const int iFileValidPeriod);
    int setFileValidPeriod(const std::string &strFileValidPeriod);
    int setFileListSize(const std::string &strFileListSize);

    int setRecValidPeriod(const int iRecValidPeriod);
    int setRecValidPeriod(const std::string &strRecValidPeriod);

    int getFileSegTime();
    int getFileValidPeriod();
    int getFileListSize();

    int getRecValidPeriod();

    int m_iFileHelperPeriod;//文件处理周期， 0 - 自动， 用iSegmentFileTime 此时两个文件周期会处理

    // Atatch
    int m_iType;          // Atatch
    std::string m_strUrl;           // Atatch
    std::string m_strAccount;       // Atatch
    std::string m_strPasswd;        // Atatch
    //std::string m_strVideoEncode;   // Atatch

    // 获取播放地址方法
    int m_iGetPlayUrlmethod;

#if defined(_Func_useRecFileHelper)
    // 分散在每个录像单元中
    CRecFileHelper *m_pRecFileHelper;
#endif //_Func_useRecFileHelper
    std::string m_strRecPath;        // 录像位置， 当赋值后，需要人为创建目录
    //time_t m_ttRecStartAt;
private:
    int m_iFileSegTime;//片段时长
    int m_iFileValidPeriod;//文件保留时长
    int m_iFileListSize;

    int m_iRecValidPeriod;//录制时长

    void doPull();
    virtual void onStoped();

    std::string m_strRecPathCreatDired;
};
#endif // !defined(_RecordTykdMediaUrl_H)

