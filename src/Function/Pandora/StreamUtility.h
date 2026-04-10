/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StreamUtility.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: StreamUtility.h 5884 2016-12-09 09:11:20Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-12-09 09:11:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string>

#if !defined(_StreamUtility_H)
#define _StreamUtility_H

#define def_MAX_DeviceID_LEN 40// sha1sum (sizeof("965c940c4ab1eb45fe215a024977ca1b0d9de356")) //r-112A0000034_0_1-1488532643-2145888000.mp4

#define fixUrl_URL_SIGN_PARA_MARK "psstk"

class CStreamUtility// : public CEZObject
{
public:

    CStreamUtility();
    virtual ~CStreamUtility();

public:
    static int fixUrl(std::string &strDst, const std::string strUrl, const std::string strChkSum);

    static int CreateStreamUrl( \
                                const std::string &strName
                                , std::string &strPushRtmp
                                , std::string &strPushRtsp
                                , std::string &strPlayHls
                                , std::string &strPlayRtmp
                                , std::string &strPlayRtsp);

    // 0 -- ok
    // -1 -- para not valid
    // -2 -- url not valid
    // -10 -- ip not valid
    // -11 -- port not valid
    static int ez_url_valid(const char *);
    static int ez_url_valid(const std::string &strUrl);

    // Êý×ÖºÍ×ÖÄ¸Only
    // >0-valid, len
    // <=0 - not valid
    static int ez_devid_valid(const char *);

private:

};

#endif // !defined(_StreamUtility_H)
