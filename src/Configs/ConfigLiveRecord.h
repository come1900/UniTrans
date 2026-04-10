/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveRecord.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveRecord.h 5884 2017-02-19 07:07:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-19 07:07:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __ConfigLiveRecord_H__
#define __ConfigLiveRecord_H__

#include "ConfigBase.h"

//!
typedef struct tagConfigLiveRecord
{
    std::string strIndexPath;//索引路径
    std::string strMediaPath;//录像文件路径

    int iListSize;//索引内文件个数

    int iDynamicIssueID;//动态ID
    // issue addr
    std::string strDefaultUrlPrefix;//分发url

    int iStreamPusherMaint;//刷新周期

    // bit0 - snap
    // bit1 - qtfile
    // bit2 - fileClean
    int iFuncMask;//功能掩码
    int iFun_MotionFile;//功能掩码- 视频浓缩

    // 参数传0 - 自动的时候以及默认值
    int iSegmentFileTime; // 文件片段时长
    int iFileValidPeriod;// 文件保存时长

    // 自动启动拉流， 上次的拉流流媒体开启后自动开启
    // 当成nvr的时候使用
    // 0 - 不启动
    // 1 - 启动
    int iStartWithCacheDB;

    // 2019-11-27 15:13:03
    //热 存储
#if 1
    //录像文件存储位置
    int iStoragePeriod; // 文件在此处存储时长- 秒
    std::string strStoragePath;//录像文件路径
    std::string strStorageUrlPrefix;//分发url
#endif

    // 冷存储
#if 1
    //录像文件存储位置
    int iCheapStoragePeriod; // 文件在此处存储时长- 秒
    std::string strCheapStoragePath;//录像文件路径
    std::string strCheapStorageUrlPrefix;//分发url
#endif

    int iFileHelperPeriod;//文件处理周期， 0 - 自动， 用iSegmentFileTime 此时两个文件周期会处理
}
ConfigLiveRecord;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigLiveRecord, 1, 4> CConfigLiveRecord;

#endif //__ConfigLiveRecord_H__
