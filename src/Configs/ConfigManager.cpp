/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigManager.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigManager.cpp 5884 2012-07-02 09:17:36Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-07-02 09:17:36  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigManager.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int CConfigManager::SetDefaultConfig(int iConfigType)
{
    int iRet = 0;
    if ((iConfigType > CFG_IDX_ALL) || (iConfigType < CFG_IDX_General))
    {
        //tracepoint();
        __trip;
        return CONFIG_APPLY_FILE_ERROR;
    }

    switch(iConfigType)
    {
        case CFG_IDX_General:
        {
            //��ͨ����
            iRet |= recallConfig(CFG_NAME_General);

            break;
        }
#if defined(_USE_DBAGENT_OCCI)
        case CFG_IDX_Database:
        {
            //
            iRet |= recallConfig(CFG_NAME_Database);

            break;
        }
#endif //_USE_DBAGENT_OCCI
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETSERVER)
        case CFG_IDX_TcpSocketServer:
        {
            //TCP������
            iRet |= recallConfig(CFG_NAME_TcpSocketServer);

            break;
        }
#endif //EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR)
        case CFG_IDX_TcpSocketConnector:
        {
            //TCP�ͻ�������
            iRet |= recallConfig(CFG_NAME_TcpSocketConnector);

            break;
        }
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#if defined(_FUNC_CONSOLE_OVERTCP)
        case CFG_IDX_ConsoleOverTcp:
        {
            //�����ն�
            iRet |= recallConfig(CFG_NAME_ConsoleOverTcp);

            break;
        }
#endif //_FUNC_CONSOLE_OVERTCP
#if defined(EXAMPLE_THE_WEBSERVICE)
        case CFG_IDX_WebService:
        {
            iRet |= recallConfig(CFG_NAME_WebService);

            break;
        }
#endif //EXAMPLE_THE_WEBSERVICE
#if defined(USE_PUBSERVICE)
        case CFG_IDX_PubService:
        {
            iRet |= recallConfig(CFG_NAME_PubService);

            break;
        }
#endif //USE_PUBSERVICE
#if defined(USE_SMSOVERHTTP)
        case CFG_IDX_SmsOverHttp:
        {
            iRet |= recallConfig(CFG_NAME_SmsOverHttp);

            break;
        }
#endif //USE_SMSOVERHTTP
#if defined(USE_InterUtcs_YiHeTech)
        case CFG_IDX_InterUtcs_YiHeTech:
        {
            iRet |= recallConfig(CFG_NAME_InterUtcs_YiHeTech);

            break;
        }
#endif //USE_InterUtcs_YiHeTech

#if defined(USE_ASSOVERHTTP)
        case CFG_IDX_AssOverHttp:
        {
            iRet |= recallConfig(CFG_NAME_AssOverHttp);

            break;
        }
#endif //USE_ASSOVERHTTP
#if defined(USE_PUOVERHTTP)
        case CFG_IDX_PuOverHttp:
        {
            iRet |= recallConfig(CFG_NAME_PuOverHttp);

            break;
        }
#endif //USE_PUOVERHTTP
#if defined(USE_CUOVERHTTP)
        case CFG_IDX_CuOverHttp:
        {
            iRet |= recallConfig(CFG_NAME_CuOverHttp);

            break;
        }
#endif //USE_CUOVERHTTP
#if defined(FUNC_KnxIpCu)
        case CFG_IDX_KnxIpCu:
        {
            iRet |= recallConfig(CFG_NAME_KnxIpCu);

            break;
        }
#endif //FUNC_KnxIpCu
#if defined(FUNC_KnxIpGw)
        case CFG_IDX_KnxIpGw:
        {
            iRet |= recallConfig(CFG_NAME_KnxIpGw);

            break;
        }
#endif //FUNC_KnxIpGw
#if defined(XP_FaceWebs)
        case CFG_IDX_FaceWebs:
        {
            iRet |= recallConfig(CFG_NAME_FaceWebs);

            break;
        }
#endif //XP_FaceWebs
#if defined(FUNC_GEMINILOCK)
        case CFG_IDX_GeminiLock:
        {
            iRet |= recallConfig(CFG_NAME_GeminiLock);

            break;
        }
#endif

#if defined(_USE_InterThings_gLock)
        case CFG_IDX_InterThings_gLock:
        {
            iRet |= recallConfig(CFG_NAME_InterThings_gLock);

            break;
        }
#endif //_USE_InterThings_gLock

#if defined(_DEV_NetSerialTCPSOCKETCONNECTOR)
        case CFG_IDX_NetSerialTcpCli:
        {
            iRet |= recallConfig(CFG_NAME_NetSerialTcpCli);

            break;
        }
#endif
#if defined(_DEV_SERIAL)
        case CFG_IDX_Serial:
        {
            iRet |= recallConfig(CFG_NAME_Serial);

            break;
        }
#endif//_DEV_SERIAL
#if defined(_FUNC_Shp)
        case CFG_IDX_Shp:
        {
            iRet |= recallConfig(CFG_NAME_Shp);

            break;
        }
#endif//_FUNC_Shp
#if defined(_FUNC_PULL_STREAMMING_SERVER)
        case CFG_IDX_PullStreammingServer:
        {
            iRet |= recallConfig(CFG_NAME_PullStreammingServer);

            break;
        }
#endif
#if defined(_FUNC_LiveRecord)
        case CFG_IDX_LiveRecord:
        {
            iRet |= recallConfig(CFG_NAME_LiveRecord);
            break;
        }
#endif
#if defined(_FUNC_LiveRecordServer)
        case CFG_IDX_LiveRecordServer:
        {
            iRet |= recallConfig(CFG_NAME_LiveRecordServer);
            break;
        }
#endif
#if defined(_FUNC_LiveStatusServer)
        case CFG_IDX_LiveStatusServer:
        {
            iRet |= recallConfig(CFG_NAME_LiveStatusServer);
            break;
        }
#endif //_FUNC_LiveStatusServer
#if defined(_FUNC_LiveCastServer)
        case CFG_IDX_LiveCast:
        {
            iRet |= recallConfig(CFG_NAME_LiveCast);
            break;
        }
        case CFG_IDX_LiveCastServer:
        {
            iRet |= recallConfig(CFG_NAME_LiveCastServer);
            break;
        }
#endif
#if defined(_FUNC_InterVideo)
        case CFG_IDX_InterVideo:
        {
            iRet |= recallConfig(CFG_NAME_InterVideo);
            break;
        }
#endif
#if defined(_FUNC_KeyManagementService)
        case CFG_IDX_KeyManagementService:
        {
            iRet |= recallConfig(CFG_NAME_KeyManagementService);
            break;
        }
#endif

#if defined(_FUNC_NvpRegisterServer)
        case CFG_IDX_NvpRegisterServer:
        {
            iRet |= recallConfig(CFG_NAME_NvpRegisterServer);
            break;
        }
#endif //_FUNC_NvpRegisterServer
#if defined(_FUNC_StreamPusher)
        case CFG_IDX_StreamPusher:
        {
            iRet |= recallConfig(CFG_NAME_StreamPusher);
            break;
        }
#endif //_FUNC_StreamPusher
#if defined(_FUNC_MinerMtService)
        case CFG_IDX_MinerMtService:
        {
            iRet |= recallConfig(CFG_NAME_MinerMtService);
            break;
        }
#endif //_FUNC_MinerMtService
#if defined(_FUNC_Crawler)
        case CFG_IDX_Crawler:
        {
            iRet |= recallConfig(CFG_NAME_Crawler);
            break;
        }
#endif //_FUNC_Crawler
#if defined(_FUNC_TouchIngress)
        case CFG_IDX_TouchIngress:
        {
            iRet |= recallConfig(CFG_NAME_TouchIngress);
            break;
        }
#endif //_FUNC_TouchIngress
#if defined(_FUNC_TouchEdge)
        case CFG_IDX_TouchEdge:
        {
            iRet |= recallConfig(CFG_NAME_TouchEdge);
            break;
        }
#endif //_FUNC_TouchEdge

        case CFG_IDX_ALL:
        {
            //�ָ�ȫ��,�����
            ez_trace("Recall all \n");
            iRet |= recallConfigAll();
            break;
        }
        default:
        {
            ez_trace("###########default \n");
            break;
        }
    }

    return iRet;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

PATTERN_SINGLETON_IMPLEMENT(CConfigManager);

int CConfigManager::initialize(std::string mfile, std::string sfile)
{
    CConfigMan::initialize(mfile, sfile);

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // ����ͨ������
    setupConfig(CFG_NAME_General, CFG_IDX_General, m_configGeneral);

#if defined(_USE_DBAGENT_OCCI)
    setupConfig(CFG_NAME_Database, CFG_IDX_Database, m_configDatabase);
#endif //_USE_DBAGENT_OCCI
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETSERVER)
    setupConfig(CFG_NAME_TcpSocketServer, CFG_IDX_TcpSocketServer, m_configTcpSocketServer);
#endif //EXAMPLE_GENERALAGENTTCPSOCKETSERVER

#if defined(EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR)
    setupConfig(CFG_NAME_TcpSocketConnector, CFG_IDX_TcpSocketConnector, m_configTcpSocketConnector);
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR

#if defined(_FUNC_CONSOLE_OVERTCP)
    setupConfig(CFG_NAME_ConsoleOverTcp, CFG_IDX_ConsoleOverTcp, m_configConsoleOverTcp);
#endif

#if defined(EXAMPLE_THE_WEBSERVICE)
    setupConfig(CFG_NAME_WebService, CFG_IDX_WebService, m_configWebService);
#endif
#if defined(USE_PUBSERVICE)
    setupConfig(CFG_NAME_PubService, CFG_IDX_PubService, m_configPubService);
#endif
#if defined(USE_SMSOVERHTTP)
    setupConfig(CFG_NAME_SmsOverHttp, CFG_IDX_SmsOverHttp, m_configSmsOverHttp);
#endif
#if defined(USE_InterUtcs_YiHeTech)
    setupConfig(CFG_NAME_InterUtcs_YiHeTech, CFG_IDX_InterUtcs_YiHeTech, m_configInterUtcs_YiHeTech);
#endif
#if defined(USE_ASSOVERHTTP)
    setupConfig(CFG_NAME_AssOverHttp, CFG_IDX_AssOverHttp, m_configAssOverHttp);
#endif
#if defined(USE_PUOVERHTTP)
    setupConfig(CFG_NAME_PuOverHttp, CFG_IDX_PuOverHttp, m_configPuOverHttp);
#endif
#if defined(USE_CUOVERHTTP)
    setupConfig(CFG_NAME_CuOverHttp, CFG_IDX_CuOverHttp, m_configCuOverHttp);
#endif
#if defined(FUNC_KnxIpCu)
    setupConfig(CFG_NAME_KnxIpCu, CFG_IDX_KnxIpCu, m_configKnxIpCu);
#endif //FUNC_KnxIpCu
#if defined(FUNC_KnxIpGw)
    setupConfig(CFG_NAME_KnxIpGw, CFG_IDX_KnxIpGw, m_configKnxIpGw);
#endif //FUNC_KnxIpGw
#if defined(XP_FaceWebs)
    setupConfig(CFG_NAME_FaceWebs, CFG_IDX_FaceWebs, m_configFaceWebs);
#endif //XP_FaceWebs

#if defined(FUNC_GEMINILOCK)
    setupConfig(CFG_NAME_GeminiLock, CFG_IDX_GeminiLock, m_configGeminiLock);
#endif //FUNC_GEMINILOCK
#if defined(_USE_InterThings_gLock)
    setupConfig(CFG_NAME_InterThings_gLock, CFG_IDX_InterThings_gLock, m_configInterThings_gLock);
#endif //_USE_InterThings_gLock

#if defined(_DEV_NetSerialTCPSOCKETCONNECTOR)
    setupConfig(CFG_NAME_NetSerialTcpCli, CFG_IDX_NetSerialTcpCli, m_configNetSerialTcpCli);
#endif //_DEV_NetSerialTCPSOCKETCONNECTOR
#if defined(_DEV_SERIAL)
    setupConfig(CFG_NAME_Serial, CFG_IDX_Serial, m_configSerial);
#endif //_DEV_SERIAL
#if defined(_FUNC_Shp)
    setupConfig(CFG_NAME_Shp, CFG_IDX_Shp, m_configShp);
#endif //_FUNC_Shp

//#if defined(_FUNC_PULL_STREAMMING_SERVER)
//    setupConfig(CFG_NAME_PullStreammingServer, CFG_IDX_PullStreammingServer, m_configPullStreammingServer);
//#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_PandoraMatrix)
    setupConfig(CFG_NAME_PandoraMatrix, CFG_IDX_PandoraMatrix, m_configPandoraMatrix);
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_PandoraNode)
    setupConfig(CFG_NAME_PandoraNode, CFG_IDX_PandoraNode, m_configPandoraNode);
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_PullTykdMobileService)
    setupConfig(CFG_NAME_PullTykdMobileService, CFG_IDX_PullTykdMobileService, m_configPullTykdMobileService);
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_TykdMobileService)
    setupConfig(CFG_NAME_TykdMobileService, CFG_IDX_TykdMobileService, m_configTykdMobileService);
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_LiveStreammingServer)
    setupConfig(CFG_NAME_LiveStreammingServer, CFG_IDX_LiveStreammingServer, m_configLiveStreammingServer);
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_LiveRecord)
    setupConfig(CFG_NAME_LiveRecord, CFG_IDX_LiveRecord, m_configLiveRecord);
#endif //_FUNC_LiveRecord
#if defined(_FUNC_LiveRecordServer)
    setupConfig(CFG_NAME_LiveRecordServer, CFG_IDX_LiveRecordServer, m_configLiveRecordServer);
#endif //_FUNC_LiveRecordServer
#if defined(_FUNC_LiveStatusServer)
    setupConfig(CFG_NAME_LiveStatusServer, CFG_IDX_LiveStatusServer, m_configLiveStatusServer);
#endif //_FUNC_LiveStatusServer
#if defined(_FUNC_LiveCastServer)
    setupConfig(CFG_NAME_LiveCast, CFG_IDX_LiveCast, m_configLiveCast);
    setupConfig(CFG_NAME_LiveCastServer, CFG_IDX_LiveCastServer, m_configLiveCastServer);
#endif //_FUNC_LiveCastServer
#if defined(_FUNC_InterVideo)
    setupConfig(CFG_NAME_InterVideo, CFG_IDX_InterVideo, m_configInterVideo);
#endif //_FUNC_InterVideo
#if defined(_FUNC_KeyManagementService)
    setupConfig(CFG_NAME_KeyManagementService, CFG_IDX_KeyManagementService, m_configKeyManagementService);
#endif //_FUNC_KeyManagementService

#if defined(_FUNC_NvpRegisterServer)
    setupConfig(CFG_NAME_NvpRegisterServer, CFG_IDX_NvpRegisterServer, m_configNvpRegisterServer);
#endif //_FUNC_NvpRegisterServer
#if defined(_FUNC_StreamPusher)
    setupConfig(CFG_NAME_StreamPusher, CFG_IDX_StreamPusher, m_configStreamPusher);
#endif //_FUNC_StreamPusher
#if defined(_FUNC_MinerMtService)
    setupConfig(CFG_NAME_MinerMtService, CFG_IDX_MinerMtService, m_configMinerMtService);
#endif //_FUNC_MinerMtService
#if defined(_FUNC_Crawler)
    setupConfig(CFG_NAME_Crawler, CFG_IDX_Crawler, m_configCrawler);
#endif //_FUNC_Crawler
#if defined(_FUNC_TouchIngress)
    setupConfig(CFG_NAME_TouchIngress, CFG_IDX_TouchIngress, m_configTouchIngress);
#endif //_FUNC_TouchIngress
#if defined(_FUNC_TouchEdge)
    setupConfig(CFG_NAME_TouchEdge, CFG_IDX_TouchEdge, m_configTouchEdge);
#endif //_FUNC_TouchEdge
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // ��ʼ�����, ����һ������
    CConfigMan::saveFile();

    return 0;
}

