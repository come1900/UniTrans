/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTykdMobileService.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTykdMobileService.cpp 5884 2016-11-02 09:21:05Z WuJunjie $
 *
 *  Explain:
 *     -
 *       Used By:
 *           LiveRecord
 *     -
 *
 *  Update:
 *     2016-11-02 09:21:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "ConfigTykdMobileService.h"

#include "../UniTransDef.h"

//#include "../Function/Interface/tykdWebService/TypedefTykdMobileService.h"
//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
#define StatusUploadWS_QQH "http://42.101.40.163:17071/cloudVideoMgr/cxf/StorageGatewayServices?wsdl"
#define StatusUploadWS_ALPHA "http://192.168.5.81:18086/storageGateway/cxf/StorageGatewayServices?wsdl"

#ifndef TYKD_URL_Endpoint
#define TYKD_URL_Endpoint     "http://www.189eyes.com:9000/cxf/MobileService"
#endif //TYKD_URL_Endpoint

#ifndef MY_PUSER_MAN_CMD
#define MY_PUSER_MAN_CMD "pssixman"
#endif//MY_PUSER_MAN_CMD

template<>
void exchangeTable<ConfigTykdMobileService>(CConfigExchange& xchg, CConfigTable& table, ConfigTykdMobileService& config, int index, int app)
{
#if defined(_SUPPORT_CFG_PSSIXMAN)
#if defined(LINUX)
    std::string strDefaultPushmanPathname = getenv("HOME") + ("/bin/" + std::string(MY_PUSER_MAN_CMD) );
    xchg.exchange(table, "PushmanFullName", config.strPushmanFullName, strDefaultPushmanPathname);
#else //LINUX
    xchg.exchange(table, "PushmanFullName", config.strPushmanFullName, MY_PUSER_MAN);
#endif //LINUX
#endif//_SUPPORT_CFG_PSSIXMAN

#if defined(_SUPPORT_CFG_TYKDWSURL)
    xchg.exchange(table, "TykdWsUrl", config.strTykdWsUrl, TYKD_URL_Endpoint);
#endif//_SUPPORT_CFG_TYKDWSURL

#if defined(_SUPPORT_CFG_StatusUploadWS)
    xchg.exchange(table, "StatusUploadWS", config.strStatusUploadWS, std::string(StatusUploadWS_QQH));
#endif//_SUPPORT_CFG_StatusUploadWS

    xchg.exchange(table,"GetPlayUrlmethod", config.iGetPlayUrlmethod,  0, 100, 0); 
    xchg.exchange(table, "HostRelative", config.strHostRelative, "");

    xchg.exchange(table, "04_EndP4VideoRs", config.strEndPoint4VideoRs, "http://122.227.101.27:60000/gbManager/cxf/rest/client/getPssServerPlayUrlHXReq");

    //0-copy, 1-libx264
    xchg.exchange(table, "05_DefaultVideoCodec", config.iDefaultVideoCodec,  0, 1, 0);
    // 0-audo(copy), 1-aac, 2-noaudio, 3-libfdk_aac
    xchg.exchange(table, "06_DefaultAudioCodec", config.iDefaultAudioCodec,  0, 3, 3);
}

