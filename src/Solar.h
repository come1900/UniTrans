/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Solar.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Solar.h 5884 2012-05-16 09:17:46Z WuJunjie $
 *
 *  Explain:
 *     -
 *      一般主调类
 *     -
 *
 *  Update:
 *     2012-05-16 09:17:46   WuJunjie  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __Solar_H__
#define __Solar_H__

#include "EZThread.h"
#include "EZTimer.h"

//class CgMiniLicense;
class CSolar : public CEZObject
{
public:
    PATTERN_SINGLETON_DECLARE(CSolar);

    CSolar();
    ~CSolar();

    int Initialize(int argc, char * argv[]);

    int Start();
    int Stop(int mod=-1/*all*/);

    int Done();

    // kill(1)
    int Term();

    int Reboot();
    int Shutdown();

    int Restart();
    int Restore();

    void haveException(int ExcNo=0, char *pMsg = NULL);
    void Cleanup(int iMode=0);

    const std::string &GetVersion();
//#ifdef _FUNC_gMiniLicense
    const std::string &GetHuid();
    const std::string &GetHostID();
//#endif //_FUNC_gMiniLicense
    const char *GetCopyRights();
    const char *GetDesc();
    unsigned long GetRunPeriod();
    unsigned long GetRunPeriodMs();
    time_t GetStartTime();
    const std::string &GetHostIP();
    const std::string &GetHostMac();

    // 自增接口
    long GetCounter();

//#ifdef _USE_MODULES_EZCONFIG
#if 0
    const std::string &GetConfigFilePath();
#endif //_USE_MODULES_EZCONFIG
private:
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 新增
    // 解析输入参数
    int parseArg(int argc, char *argv[]);
    void GetBuildDate(int &year, int &month_, int &day);

#ifdef _USE_MODULES_EZCONFIG
    std::string m_strConfigFile;
    std::string m_strConfigFileDuplicate;

    std::string m_strConfigFilePath;
#endif //_USE_MODULES_EZCONFIG

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    int InitializeLogs(int log_ctrl);

    void ShowVersion();

    int chkRuntimeEnvironment(int mode=-1/*All*/);
    int iniRuntimeEnvironment(int mode=-1/*All*/);
    int setRuntimeEnvironment(int mode=-1/*All*/);

    std::string m_strHostIp; // 只记录第一个IP地址
    std::string m_strHostMac; // 只记录第一个

    // 版本字符串
    std::string m_strVer;
    // 启动时间
    unsigned int m_iRunPeriod;

    time_t m_ttStartTime;


    CEZMutex m_mutexMark;

#if defined(_FUNC_gMiniLicense)
    //CgMiniLicense         *m_pCgMiniLicense;          //保活功能定时器
    void OnGminiLicenseMsg(int iCMD, char *pPacket, int iCh, time_t time);
#endif // _FUNC_gMiniLicense
    // 普通回调
    void OnMsgCallback(int iMsg, void *pData);

    std::string m_strHuid;// 
    std::string m_strHostID;// 

    int m_iRunDaemon;
    int m_log_ctrl;

    int m_iServiceRunning;

    // 便捷的自增接口
    CEZMutex m_mutexCounter;
    long m_iCounter;
};

#define g_Solar (*CSolar::instance())

#endif //__Solar_H__
