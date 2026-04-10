/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * StreamUtility.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: StreamUtility.cpp 5884 2016-12-09 09:11:23Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2016-12-09 09:11:23  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "str_opr.h"
#include "ez_bit.h"
#include "sha1.h"
#include "StringOpr.h"
#include "ez_util_platform.h"
#include "str_opr.h"
#include <ez_url_parser.h>

#include "StreamUtility.h"

#include <string.h>
#define def_Max_Url_Len 2083 //IE, https://www.cnblogs.com/cuihongyu3503319/p/5892257.html
#define def_Min_Url_Len 2 //IE, https://www.cnblogs.com/cuihongyu3503319/p/5892257.html

CStreamUtility::CStreamUtility()
{}

CStreamUtility::~CStreamUtility()
{}

int CStreamUtility::fixUrl(std::string &strDst, const std::string strUrl, const std::string strChkSum)
{
    if (strUrl.length()<5)
    {
        strDst = "";
        return -1;
    }

    std::string strShaChkSum = strChkSum;

    strShaChkSum += strUrl;

    char dst[SHA1HashSize*2+1] = {0};
    int iret = ez_sha1_str(dst, sizeof(dst), strShaChkSum.c_str());
    if (iret>=0)
    {
        //strDst = strUrl+"?";
        strDst = strUrl + std::string("?") + std::string(fixUrl_URL_SIGN_PARA_MARK) + std::string("=") + dst;
        //strDst += "=";
        //strDst += dst;

        return 0;
    }
    else
    {
        strDst = strUrl;
        return -2;
    }

    return -100;
}

int CStreamUtility::ez_url_valid(const char *p_url)
{
    int iret=0;
    ez_parsed_url_t * parsed_url =NULL;

    if (!p_url || !*p_url)
    {
        return -1;
    }

    int url_len = strlen(p_url);

    if (url_len>def_Max_Url_Len || url_len < def_Min_Url_Len)
    {
        return -2;
    }
	
    do
    {
        parsed_url = ez_parse_url(p_url);

        if (NULL==parsed_url)
        {
            iret = -10;
            break;
        }

        if (NULL==parsed_url->host)
        {
            iret = -11;
            break;
        }

        struct in_addr in_addr_ip;

        int result = inet_pton(AF_INET, parsed_url->host, &(/*sa.sin_addr*/in_addr_ip));

        if (result == 0 || in_addr_ip.s_addr==0/*0.0.0.0*/)
        {
            iret = -12;
            break;
        }

        // 采用默认端口的port is null
        if (parsed_url->port && atoi(parsed_url->port)==0)
        {
            iret = -13;
            break;
        }
    }
    while(0);

    if (parsed_url)
        ez_parsed_url_free(parsed_url);

    return iret;
}

int CStreamUtility::ez_url_valid(const std::string &strUrl)
{
    if (strUrl.empty())
    {
        return -100;
    }

    if (strUrl.length()>def_Max_Url_Len || strUrl.length()<def_Min_Url_Len)
    {
        return -101;
    }

    return ez_url_valid(strUrl.c_str());
}
// 数字和字母Only
// >0-valid, len
// <=0 - not valid
int CStreamUtility::ez_devid_valid(const char *p_devid)
{
    if (!p_devid || !*p_devid)
    {
        return 0;
    }

    int ii=0;
    while ( (*(p_devid+ii)) )
    {
        if ( \
             ( \
               !(isalpha(*(p_devid+ii)))
               && !(isdigit (*(p_devid+ii)))
             )
             || ii>def_MAX_DeviceID_LEN
           )
        {
            return (-1*ii);
        }

        ii ++;
    };

    return ii;
}

