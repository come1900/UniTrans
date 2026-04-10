/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * gMiniLicense.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: gMiniLicense.h 5884 2015-02-01 10:37:29Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2015-02-01 10:37:29  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_gMiniLicense_H)
#define _gMiniLicense_H

#include "gmini_license.h"

#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"

// 未列举的消息暂不需要
typedef enum __msg_gmini_license_t {
    MSG_GMINI_LICENSE_NOTREADY = 0,		    	// 数据不全
    MSG_GMINI_LICENSE_OK,		    	        // 授权正常
    MSG_GMINI_LICENSE_LICENSE_DATA_ERROR,		// 授权码无效
    MSG_GMINI_LICENSE_OBJECT_ERROR,		        // 授权对象（设备）异常
    MSG_GMINI_LICENSE_TIMEOUT,	        // XX功能失效， 功能见消息体
    MSG_GMINI_LICENSE_FUNC_OUT_AUTH,	        // XX功能失效， 功能见消息体
}MSG_GMINI_LICENSE_T;

#define g_gMiniLicense (*CgMiniLicense::instance())
class CgMiniLicense : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CgMiniLicense);

    // cmd, partition, len, time
    typedef TSignal4<int, char *, int, time_t> gMiniLicenseSignal_t;
    typedef gMiniLicenseSignal_t::SigProc gMiniLicenseSignalProc_t;

    EZTHREAD_BOOL Start(CEZObject * pObj, gMiniLicenseSignalProc_t pProc);
    EZTHREAD_BOOL Stop(CEZObject * pObj, gMiniLicenseSignalProc_t pProc);

    void ThreadProc();

    CgMiniLicense();
    virtual ~CgMiniLicense();
    // 运行时长
    unsigned long long getLicenseUptime();
    time_t getValidDate();
    const char * const getValidDateStr();
    char getLicVers();

    // use config
    int useLicense();
    // use para
    int setLicense(std::string strLic);

    const std::string &getLicense();
    int Dump(std::string &strDump);

private:
    //int OnUEventData(char *pData, int len);

    gMiniLicenseSignal_t m_SigBuffer;
    CEZMutex m_MutexSigBuffer;

    //计数
    int				m_iUser;

    char m_cGMINI_LIC_V;
    unsigned long long m_llValidSeconds;//for GMINI_LIC_V1
    time_t m_ttValidDate;//for GMINI_LIC_V2
    char m_strValidDate[20];//for GMINI_LIC_V2
    std::string m_strGLic;
};
#endif // !defined(_gMiniLicense_H)



