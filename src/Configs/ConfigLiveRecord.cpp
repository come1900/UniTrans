/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigLiveRecord.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigLiveRecord.cpp 5884 2017-02-19 07:07:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-02-19 07:07:22  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigLiveRecord.h"

#include "../UniTransDef.h"
#include "../Function/Pandora/PandoraDef.h"
#include "../Solar.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
#define DEFAULT_RECORD_ISUE_URL "http://" "61.160.149.160" ":18080/rec/media_file"

template<>
void exchangeTable<ConfigLiveRecord>(CConfigExchange& xchg, CConfigTable& table, ConfigLiveRecord& config, int index, int app)
{
    std::string __defaultUrlPrefix = DEFAULT_RECORD_ISUE_URL;

    if (g_Solar.GetHostIP().length() >= sizeof("0.0.0.0") )
    {
        //__defaultUrlPrefix = "http://" + g_Solar.GetHostIP() + ":18080/rec/media_file";
        __defaultUrlPrefix = "http://" + g_Solar.GetHostIP() + ":18080";
    }

#if defined(LINUX)
    std::string strDefaultPushmanPathname = getenv("HOME") + std::string("/html/rec" );
#else //LINUX
    std::string strDefaultPushmanPathname = "/home/pss/html/rec";
#endif //LINUX

    xchg.exchange(table, "strIndexPath", config.strIndexPath, strDefaultPushmanPathname);
    xchg.exchange(table, "strMediaPath", config.strMediaPath, strDefaultPushmanPathname+std::string("/media_file"));

    xchg.exchange(table,"iListSize", config.iListSize,  1, 65535, 5);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //录像的id默认不要加密
    xchg.exchange(table,"iDynamicIssueID", config.iDynamicIssueID,  0, 1, 0);

    xchg.exchange(table, "IsueUrlPrefix", config.strDefaultUrlPrefix, __defaultUrlPrefix+"/rec/media_file");

    xchg.exchange(table,"StreamPusherMaint", config.iStreamPusherMaint,  5, 120, 5);

    //xchg.exchange(table,"iFuncMask", config.iFuncMask,  0, 0xff, 0x1);

    xchg.exchange(table,"01_SegmentFileTime", config.iSegmentFileTime, def_MinFileSegTime, def_MaxFileSegTime, def_MinFileSegTime);
    // 94608000 = 3 years
    xchg.exchange(table,"02_FileValidPeriod", config.iFileValidPeriod, def_MinFileValidPeriod, def_MaxFileValidPeriod, def_MinFileValidPeriod);

#if defined(_PROD_LiveRecordWone) 
    xchg.exchange(table,"iFuncMask", config.iFuncMask,  0, 0xff, 0x3);//snap qt
    xchg.exchange(table,"03_StartWithCacheDB", config.iStartWithCacheDB,  0, 1, 1);
#elif defined(_Vendor_TaizhouVC) //泰州视频云
    xchg.exchange(table,"iFuncMask", config.iFuncMask,  0, 0xff, 0x3);//snap qt
    xchg.exchange(table,"03_StartWithCacheDB", config.iStartWithCacheDB,  0, 1, 1);
#else
    xchg.exchange(table,"iFuncMask", config.iFuncMask,  0, 0xff, 0x7);// snap, qt, clean - 1, 2, 4
    xchg.exchange(table,"03_StartWithCacheDB", config.iStartWithCacheDB,  0, 1, 0);
#endif //_PROD_LiveRecordWone

    // 2019-11-27 15:13:03
    xchg.exchange(table, "04_StoragePeriod", config.iStoragePeriod, 0, 0x7fffffff, 0);
    xchg.exchange(table, "05_StoragePath", config.strStoragePath, strDefaultPushmanPathname+std::string("/storage"));
    xchg.exchange(table, "06_StorageUrlPrefix", config.strStorageUrlPrefix, __defaultUrlPrefix+"/rec/storage");

    xchg.exchange(table, "07_CheapStoragePeriod", config.iCheapStoragePeriod, 0, 0x7fffffff, 0);
    xchg.exchange(table, "08_CheapStoragePath", config.strCheapStoragePath, strDefaultPushmanPathname+std::string("/cheap_storage"));
    xchg.exchange(table, "09_CheapStorageUrlPrefix", config.strCheapStorageUrlPrefix, __defaultUrlPrefix+"/rec/cheap_storage");

    //2020-02-19 8:23:49
    xchg.exchange(table,"10_MotionFile", config.iFun_MotionFile,  0, 1, 0);// 视频浓缩功能
    xchg.exchange(table,"11_FileHelperPeriod", config.iFileHelperPeriod,  0, 3600, 0);
}

