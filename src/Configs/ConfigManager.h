/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigManager.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigManager.h 5884 2012-07-02 09:15:02Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-06-25 10:17:36
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include "ConfigMan.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "ConfigGeneral.h"

#if defined(_USE_DBAGENT_OCCI)
#include "ConfigDatabase.h"
#endif //_USE_DBAGENT_OCCI
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETSERVER)
#include "ConfigTcpSocketServer.h"
#endif //EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR)
#include "ConfigTcpSocketConnector.h"
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#if defined(_FUNC_CONSOLE_OVERTCP)
#include "ConfigConsoleOverTcp.h"
#endif //_FUNC_CONSOLE_OVERTCP
#if defined(EXAMPLE_THE_WEBSERVICE)
#include "ConfigWebService.h"
#endif //EXAMPLE_THE_WEBSERVICE
#if defined(USE_PUBSERVICE)
#include "ConfigPubService.h"
#endif //USE_PUBSERVICE
#if defined(USE_SMSOVERHTTP)
#include "ConfigSmsOverHttp.h"
#endif //USE_SMSOVERHTTP
#if defined(USE_InterUtcs_YiHeTech)
#include "ConfigInterUtcs_YiHeTech.h"
#endif //USE_InterUtcs_YiHeTech
#if defined(USE_ASSOVERHTTP)
#include "ConfigAssOverHttp.h"
#endif //USE_ASSOVERHTTP
#if defined(USE_PUOVERHTTP)
#include "ConfigPuOverHttp.h"
#endif //USE_PUOVERHTTP
#if defined(USE_CUOVERHTTP)
#include "ConfigCuOverHttp.h"
#endif //USE_CUOVERHTTP
#if defined(FUNC_KnxIpCu)
#include "ConfigKnxIpCu.h"
#endif //FUNC_KnxIpCu
#if defined(FUNC_KnxIpGw)
#include "ConfigKnxIpGw.h"
#endif //FUNC_KnxIpGw
#if defined(XP_FaceWebs)
#include "ConfigFaceWebs.h"
#endif //XP_FaceWebs

#if defined(FUNC_GEMINILOCK)
#include "ConfigGeminiLock.h"
#endif
#if defined(_USE_InterThings_gLock)
#include "ConfigInterThings_gLock.h"
#endif //_USE_InterThings_gLock

#if defined(_DEV_NetSerialTCPSOCKETCONNECTOR)
#include "ConfigNetSerialTcpCli.h"
#endif
#if defined(_DEV_SERIAL)
#include "ConfigSerial.h"
#endif // _DEV_Serial
#if defined(_FUNC_Shp)
#include "ConfigShp.h"
#endif // _FUNC_Shp

//#if defined(_FUNC_PULL_STREAMMING_SERVER)
//#include "ConfigPullStreammingServer.h"
//#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_PandoraMatrix)
#include "ConfigPandoraMatrix.h"
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_PandoraNode)
#include "ConfigPandoraNode.h"
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_PullTykdMobileService)
#include "ConfigPullTykdMobileService.h"
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_TykdMobileService) //��������
#include "ConfigTykdMobileService.h"
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_LiveStreammingServer)
#include "ConfigLiveStreammingServer.h"
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_LiveRecord)
#include "ConfigLiveRecord.h"
#endif //_FUNC_LiveRecord
#if defined(_FUNC_LiveRecordServer)
#include "ConfigLiveRecordServer.h"
#endif //_FUNC_LiveRecordServer
#if defined(_FUNC_LiveStatusServer)
#include "ConfigLiveStatusServer.h"
#endif //_FUNC_LiveStatusServer
#if defined(_FUNC_LiveCastServer)
#include "ConfigLiveCastServer.h"
#include "ConfigLiveCast.h"
#endif //_FUNC_LiveCastServer
#if defined(_FUNC_InterVideo)
#include "ConfigInterVideo.h"
#endif //_FUNC_InterVideo
#if defined(_FUNC_KeyManagementService)
#include "ConfigKeyManagementService.h"
#endif //_FUNC_KeyManagementService

#if defined(_FUNC_NvpRegisterServer)
#include "ConfigNvpRegisterServer.h"
#endif //_FUNC_NvpRegisterServer

#if defined(_FUNC_StreamPusher)
#include "ConfigStreamPusher.h"
#endif //_FUNC_StreamPusher
#if defined(_FUNC_MinerMtService)
#include "ConfigMinerMtService.h"
#endif //_FUNC_MinerMtService
#if defined(_FUNC_Crawler)
#include "ConfigCrawler.h"
#endif //_FUNC_Crawler
#if defined(_FUNC_TouchIngress)
#include "ConfigTouchIngress.h"
#endif //_FUNC_TouchIngress
#if defined(_FUNC_TouchEdge)
#include "ConfigTouchEdge.h"
#endif //_FUNC_TouchEdge

typedef enum __cfg_index_t {
    CFG_IDX_General =  0,	// ��ͨ
    CFG_IDX_Database,	// ���ݿ�
    CFG_IDX_TcpSocketServer,				// TcpSocketServer ����
    CFG_IDX_TcpSocketConnector,				// TcpSocketConnector ����
    CFG_IDX_ConsoleOverTcp,				// ConsoleOverTcp ����
    CFG_IDX_WebService,				//
    CFG_IDX_PubService,				//
    CFG_IDX_SmsOverHttp,				//
    CFG_IDX_InterUtcs_YiHeTech,				//
    CFG_IDX_AssOverHttp,				//Automation Service Server
    CFG_IDX_PuOverHttp,				//
    CFG_IDX_CuOverHttp,				//
    CFG_IDX_KnxIpCu,				// ����knx����
    CFG_IDX_KnxIpGw,				// ����knx����
    CFG_IDX_FaceWebs,				// web����
    CFG_IDX_GeminiLock,				// ˫����
    CFG_IDX_InterThings_gLock,		// ������-˫����
    CFG_IDX_NetSerialTcpCli,		// ���紮��
    CFG_IDX_Serial,                 //
    CFG_IDX_Shp,                    // // share port of ezlibs
    //CFG_IDX_PullStreammingServer,		// ����������
    CFG_IDX_PandoraMatrix,		// ����������
    CFG_IDX_PandoraNode,		// ����������
    CFG_IDX_PullTykdMobileService,		// ����������
    CFG_IDX_TykdMobileService,		// ����������
    CFG_IDX_LiveStreammingServer,		// ����������
    CFG_IDX_LiveCast,		// ֱ��
    CFG_IDX_LiveCastServer,		// ֱ��
    CFG_IDX_LiveRecord,		// ����������
    CFG_IDX_LiveRecordServer,		// ¼��
    CFG_IDX_LiveStatusServer,		// ����״̬
    CFG_IDX_InterVideo,		// ¼��
    CFG_IDX_KeyManagementService,		// ��Կ��������
    CFG_IDX_NvpRegisterServer, // ע��
    CFG_IDX_StreamPusher, // ������
    CFG_IDX_MinerMtService, // ά���ɼ�����
    CFG_IDX_Crawler, // ά���ɼ�����
    CFG_IDX_TouchIngress, // Touch Ingress
    CFG_IDX_TouchEdge, // Touch Edge
    CFG_IDX_ALL,			// ʵ��û���������ã� ���ڻָ�ȫ������
}CFG_INDEX;

#define CFG_NAME_General "General"
#define CFG_NAME_Database "Database"
#define CFG_NAME_TcpSocketServer "TcpSocketServer"
#define CFG_NAME_TcpSocketConnector "TcpSocketConnector"
#define CFG_NAME_ConsoleOverTcp "ConsoleOverTcp"
#define CFG_NAME_WebService "WebService"
#define CFG_NAME_PubService "PubService"
#define CFG_NAME_SmsOverHttp "SmsOverHttp"
#define CFG_NAME_InterUtcs_YiHeTech "InterUtcs_YiHeTech"
#define CFG_NAME_AssOverHttp "AssOverHttp"
#define CFG_NAME_PuOverHttp "PuOverHttp"
#define CFG_NAME_CuOverHttp "CuOverHttp"
#define CFG_NAME_KnxIpCu "KnxIpCu"
#define CFG_NAME_KnxIpGw "KnxIpGw"
#define CFG_NAME_FaceWebs "FaceWebs"
#define CFG_NAME_GeminiLock "GeminiLock"
#define CFG_NAME_InterThings_gLock "InterThings_gLock"
#define CFG_NAME_NetSerialTcpCli "NetSerialTcpCli"
#define CFG_NAME_Serial "Serial"
#define CFG_NAME_Shp    "Shp"
//#define CFG_NAME_PullStreammingServer "PullStreammingServer"
#define CFG_NAME_PandoraMatrix "PandoraMatrix"
#define CFG_NAME_PandoraNode "PandoraNode"
#define CFG_NAME_PullTykdMobileService "PullTykdMobileService"
#define CFG_NAME_TykdMobileService "TykdMobileService"
#define CFG_NAME_LiveStreammingServer "LiveStreammingServer"
#define CFG_NAME_LiveRecordServer "LiveRecordServer"
#define CFG_NAME_LiveRecord "LiveRecord"
#define CFG_NAME_LiveStatusServer "LiveStatusServer"
#define CFG_NAME_LiveCastServer "LiveCastServer"
#define CFG_NAME_LiveCast "LiveCast"
#define CFG_NAME_InterVideo "InterVideo"
#define CFG_NAME_KeyManagementService "KeyManagementService"
#define CFG_NAME_NvpRegisterServer "NvpRegisterServer"
#define CFG_NAME_StreamPusher "StreamPusher"
#define CFG_NAME_MinerMtService "MinerMtService"
#define CFG_NAME_Crawler "Crawler"
#define CFG_NAME_TouchIngress "TouchIngress"
#define CFG_NAME_TouchEdge "TouchEdge"
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

class CConfigManager : public CConfigMan
{
public:
    PATTERN_SINGLETON_DECLARE(CConfigManager);

    //! ��ʼ��
    // ��������ʹ��Ĭ�����֣���Ϊ���ļ���ʽ
    // ��һ������ʹ�ø����֣�Ϊ���ļ���ʽ
    // ����������ʹ�ø����֣�Ϊ˫�ļ���ʽ
    int initialize(std::string mfile="", std::string sfile="");

    //! ����Ĭ�����ã���GUI���������ģ�����
    int SetDefaultConfig(int iConfigType);

private:
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    //����������
    CConfigGeneral		m_configGeneral;	/*!< ��ͨ���� */

#if defined(_USE_DBAGENT_OCCI)
    CConfigDatabase		m_configDatabase;	/*!< ��ͨ���� */
#endif //_USE_DBAGENT_OCCI
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETSERVER)
    CConfigTcpSocketServer			m_configTcpSocketServer;	/*!< dsd */
#endif //EXAMPLE_GENERALAGENTTCPSOCKETSERVER
#if defined(EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR)
    CConfigTcpSocketConnector			m_configTcpSocketConnector;	/*!< dsd */
#endif //EXAMPLE_GENERALAGENTTCPSOCKETCONNECTOR
#if defined(_FUNC_CONSOLE_OVERTCP)
    CConfigConsoleOverTcp			m_configConsoleOverTcp;	/*!< dsd */
#endif
#if defined(EXAMPLE_THE_WEBSERVICE)
    CConfigWebService			m_configWebService;	/*!< WebService */
#endif
#if defined(USE_PUBSERVICE)
    CConfigPubService			m_configPubService;	/*!< PubService */
#endif
#if defined(USE_SMSOVERHTTP)
    CConfigSmsOverHttp			m_configSmsOverHttp;	/*!< SmsOverHttp */
#endif
#if defined(USE_InterUtcs_YiHeTech)
    CConfigInterUtcs_YiHeTech			m_configInterUtcs_YiHeTech;	/*!< InterUtcs_YiHeTech */
#endif //USE_InterUtcs_YiHeTech

#if defined(USE_ASSOVERHTTP)
    CConfigAssOverHttp			m_configAssOverHttp;	    /*!< s */
#endif
#if defined(USE_PUOVERHTTP)
    CConfigPuOverHttp			m_configPuOverHttp;	/*!< s */
#endif
#if defined(USE_CUOVERHTTP)
    CConfigCuOverHttp			m_configCuOverHttp;	/*!< s */
#endif

#if defined(FUNC_KnxIpCu)
    CConfigKnxIpCu			m_configKnxIpCu;	/*!< s */
#endif //FUNC_KnxIpCu
#if defined(FUNC_KnxIpGw)
    CConfigKnxIpGw			m_configKnxIpGw;	/*!< s */
#endif //FUNC_KnxIpGw
#if defined(XP_FaceWebs)
    CConfigFaceWebs			m_configFaceWebs;	/*!< s */
#endif //XP_FaceWebs
#if defined(FUNC_GEMINILOCK)
    CConfigGeminiLock			m_configGeminiLock;	/*!< s */
#endif
#if defined(_USE_InterThings_gLock)
    CConfigInterThings_gLock			m_configInterThings_gLock;	/*!< s */
#endif //_USE_InterThings_gLock

#if defined(_DEV_NetSerialTCPSOCKETCONNECTOR)
    CConfigNetSerialTcpCli			m_configNetSerialTcpCli;	/*!< s */
#endif
#if defined(_DEV_SERIAL)
    CConfigSerial m_configSerial;
#endif//_DEV_SERIAL
#if defined(_FUNC_Shp)
    CConfigShp m_configShp;
#endif // _FUNC_Shp
//#if defined(_FUNC_PULL_STREAMMING_SERVER)
//    CConfigPullStreammingServer			m_configPullStreammingServer;	/*!< s */
//#endif
#if defined(_FUNC_PandoraMatrix)
    CConfigPandoraMatrix			m_configPandoraMatrix;	/*!< s */
#endif
#if defined(_FUNC_PandoraNode)
    CConfigPandoraNode			m_configPandoraNode;	/*!< s */
#endif
#if defined(_FUNC_PullTykdMobileService)
    CConfigPullTykdMobileService			m_configPullTykdMobileService;	/*!< s */
#endif
#if defined(_FUNC_TykdMobileService)
    CConfigTykdMobileService			m_configTykdMobileService;	/*!< s */
#endif
#if defined(_FUNC_LiveStreammingServer)
    CConfigLiveStreammingServer			m_configLiveStreammingServer;	/*!< s */
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_LiveRecord)
    CConfigLiveRecord			m_configLiveRecord;	/*!< s */
#endif //_FUNC_PULL_STREAMMING_SERVER
#if defined(_FUNC_LiveRecordServer)
    CConfigLiveRecordServer			m_configLiveRecordServer;	/*!< s */
#endif //_FUNC_LiveRecordServer
#if defined(_FUNC_LiveStatusServer)
    CConfigLiveStatusServer			m_configLiveStatusServer;	/*!< s */
#endif //_FUNC_LiveStatusServer

#if defined(_FUNC_LiveCastServer)
    CConfigLiveCastServer			m_configLiveCastServer;	/*!< s */
    CConfigLiveCast      			m_configLiveCast ;	/*!< s */
#endif //_FUNC_LiveCastServer
#if defined(_FUNC_InterVideo)
    CConfigInterVideo			m_configInterVideo;	/*!< s */
#endif //_FUNC_InterVideo
#if defined(_FUNC_KeyManagementService)
    CConfigKeyManagementService			m_configKeyManagementService;	/*!< s */
#endif //_FUNC_KeyManagementService
// "NvpRegisterServer"
#if defined(_FUNC_NvpRegisterServer)
    CConfigNvpRegisterServer			m_configNvpRegisterServer;	/*!< s */
#endif //_FUNC_NvpRegisterServer
#if defined(_FUNC_StreamPusher)
    CConfigStreamPusher			m_configStreamPusher;	/*!< s */
#endif //_FUNC_StreamPusher
#if defined(_FUNC_MinerMtService)
    CConfigMinerMtService			m_configMinerMtService;	/*!< s */
#endif //_FUNC_MinerMtService
#if defined(_FUNC_Crawler)
    CConfigCrawler			m_configCrawler;	/*!< s */
#endif //_FUNC_Crawler
#if defined(_FUNC_TouchIngress)
    CConfigTouchIngress			m_configTouchIngress;	/*!< s */
#endif //_FUNC_TouchIngress
#if defined(_FUNC_TouchEdge)
    CConfigTouchEdge			m_configTouchEdge;	/*!< s */
#endif //_FUNC_TouchEdge
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

private:
    CConfigManager()
    {}
    ;
    ~CConfigManager()
    {}
    ;
};

#define g_Config (*CConfigManager::instance())

#endif //__CONFIG_MANAGER_H__
