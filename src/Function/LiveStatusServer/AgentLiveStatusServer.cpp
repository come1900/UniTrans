/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentLiveStatusServer.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentLiveStatusServer.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <str_opr.h> /*time_iso8601_to_timet*/
#include <ezParse.h>

#include "../../Logs.h"
#ifdef _USE_MODULES_EZCONFIG
#include "../../Configs/ConfigLiveStatusServer.h"
#endif //_USE_MODULES_EZCONFIG

//#include "TypedefTykdMobileService.h" /* defined in libdtPst1Cif */
//#include "tykdMobileServiceClient.h" /* defined in libdtPst1Cif */

#include "AgentLiveStatusServer.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

// 有任务队列
// 100ms， 利于尽快返回-- 用的是异步机制
//#define AgentLiveStatusServer_CHK_SOCKET_TIMEOUT_US 1000*1000

#define def_ParseTS_Req_OnFileTailer 1

PATTERN_SINGLETON_IMPLEMENT(CAgentLiveStatusServer);
void CAgentLiveStatusServer::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerLiveStatusServer->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerLiveStatusServer->GetCount())
    {
        sleep(1);
        __fline;
        printf("idle.\n");
        return;
    }

    //    m_pHandlerLiveStatusServer->Select(0, AgentLiveStatusServer_CHK_SOCKET_TIMEOUT_US);
    m_pHandlerLiveStatusServer->Select(1, 0);
    m_pHandlerLiveStatusServer->Update();
}

CAgentLiveStatusServer::CAgentLiveStatusServer():CEZThread("CAgentLiveStatusServer", THREAD_PRIORITY_DEFAULT, 10)
{
    m_pCHttpServerLiveStatusServer = NULL;

    m_ConfigOfLiveStatusServer.HttpServicePort = 60090; // 如果不启用，则设置为0
    m_pCfgLiveStatusServer = new CConfigLiveStatusServer;
    assert(m_pCfgLiveStatusServer);

    m_pFileTailer_NgxAccessLog = NULL;
}

CAgentLiveStatusServer::~CAgentLiveStatusServer()
{
    m_ConfigOfLiveStatusServer.HttpServicePort = 0;

    // handler里面自管理
    //m_pCHttpServerLiveStatusServer = NULL;

    if (m_pHandlerLiveStatusServer)
    {
        delete m_pHandlerLiveStatusServer;
        m_pHandlerLiveStatusServer = NULL;
    }

    if (m_pCfgLiveStatusServer)
    {
        delete m_pCfgLiveStatusServer;
        m_pCfgLiveStatusServer = NULL;
    }

    if (m_pFileTailer_NgxAccessLog)
    {
        delete m_pFileTailer_NgxAccessLog;
        m_pFileTailer_NgxAccessLog = NULL;
    }
}

CHandlerLiveStatusServer * CAgentLiveStatusServer::GetHandler()
{
    return m_pHandlerLiveStatusServer;
}

void CAgentLiveStatusServer::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfLiveStatusServer.HttpServicePort = port;
}

void CAgentLiveStatusServer::Connect()
{
    if (m_ConfigOfLiveStatusServer.HttpServicePort > 0)
    {
        DBG(
            __fline;
            printf("new m_pCHttpServerLiveStatusServer: %d\n", m_ConfigOfLiveStatusServer.HttpServicePort);
        );

        m_pCHttpServerLiveStatusServer = new ListenSocket<CHttpServerLiveStatusServer>(*m_pHandlerLiveStatusServer);

        if (m_pCHttpServerLiveStatusServer)
        {
            if (m_pCHttpServerLiveStatusServer->Bind(m_ConfigOfLiveStatusServer.HttpServicePort) < 0)
            {
                ez_printf_error("LiveStatusServer->Bind :%d error\n", m_ConfigOfLiveStatusServer.HttpServicePort);
            }
            else
            {
                m_pHandlerLiveStatusServer->SetTimeout();
                m_pHandlerLiveStatusServer->Add(m_pCHttpServerLiveStatusServer);
                // DBG_CODE(
                //__fline;
                //printf("LiveStatusServer\t\tReady:%d\n", m_ConfigOfLiveStatusServer.HttpServicePort);
                ez_printf_info_content("%25s:%d\n", "LiveStatusServer", m_ConfigOfLiveStatusServer.HttpServicePort);
                // );
                //LOG4CPLUS_INFO(LOG_WEBSERVICES, "LiveStatusServer     Ready:"<<m_ConfigOfLiveStatusServer.HttpServicePort);
            }
        }
    }
    else
    {
        ez_printf_error("Error, m_pCHttpServerLiveStatusServer: %d\n", m_ConfigOfLiveStatusServer.HttpServicePort);
    }
}

void CAgentLiveStatusServer::Disconnect()
{}

EZTHREAD_BOOL CAgentLiveStatusServer::Start()
{
    // 已经启动
    if (m_bLoop)
    {
        return EZTHREAD_BOOL_TRUE;
    }

    m_pHandlerLiveStatusServer = new CHandlerLiveStatusServer(&m_Log);
    assert(m_pHandlerLiveStatusServer);

    // 初始化一些变量
    m_pHandlerLiveStatusServer->Start();

    m_pCfgLiveStatusServer->update();
    SetGeneralHttpAgent((unsigned short)(m_pCfgLiveStatusServer->getConfig().iPort));

    // 调试的时候使用默认参数
    //SetDefaultConfig();

    Connect();

    int ret = CreateThread();

    ARG_USED(ret);

    if (NULL == m_pFileTailer_NgxAccessLog)
    {
        m_pFileTailer_NgxAccessLog = new CFileTailer(m_pCfgLiveStatusServer->getConfig().strNgxAccessLog.c_str()/*"/home/pss/log/access.log"*/);
        assert(m_pFileTailer_NgxAccessLog);

        EZTHREAD_BOOL __bStart = m_pFileTailer_NgxAccessLog->Start(this, (CFileTailer::FileTailerSignalProc_t)&CAgentLiveStatusServer::OnFileTailer);

        if (EZTHREAD_BOOL_FALSE == __bStart)
        {
            ez_printf_notice("Start CFileTailer failed, skip this function::%s\n", m_pCfgLiveStatusServer->getConfig().strNgxAccessLog.c_str());
        }
        else
        {
            //ez_printf_debug("Start CFileTailer(%d):%s\n", __bStart, m_pCfgLiveStatusServer->getConfig().strNgxAccessLog.c_str());
        }
    }

    return EZTHREAD_BOOL_TRUE;
}

EZTHREAD_BOOL CAgentLiveStatusServer::Stop()
{
    if(m_bLoop)
    {
        m_bLoop = EZTHREAD_BOOL_FALSE;
        DestroyThread();
    }

    return EZTHREAD_BOOL_TRUE;
}

void CAgentLiveStatusServer::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}

void CAgentLiveStatusServer::OnFileTailer(int iCMD, char *pLine)
{
    if (NULL==pLine
        || *pLine == '\0')
    {
        ez_printf_error("Empty line!\n");
        return;
    }
DBG(
    ez_printf_debug("CALLBACK - %s:%d[%s]\n", __FUNCTION__, iCMD, pLine);
);
    do
    {
        char *__pBegin = strstr(pLine, "/hls/");
        char *__pEnd = strstr(pLine, ".m3u8");
#if defined(def_ParseTS_Req_OnFileTailer)
        if (__pEnd==NULL)
        {
            __pEnd = strstr(pLine, ".ts");
        }
#endif //#if defined(def_ParseTS_Req_OnFileTailer)

        if (__pBegin==NULL
            ||__pEnd==NULL
            ||__pEnd-(__pBegin+5)<1)/*5 is /hls/*/
        {
            //ez_printf_debug("strReqResource not hls req, skip it\n");
            break;
        }

        std::string _strStreamID ;/*sizeof(/hls/)*/
        _strStreamID.assign(__pBegin+5, __pEnd-(__pBegin+5));/*5 is /hls/*/

#if defined(def_ParseTS_Req_OnFileTailer)
        std::size_t found = _strStreamID.find('-');
        if (found!=std::string::npos)
        {
            _strStreamID = _strStreamID.substr(0, found);
        }
#endif //#if defined(def_ParseTS_Req_OnFileTailer)

DBG(
        ez_printf_debug("_strStreamID:[%s]\n", _strStreamID.c_str());
);
        // update req
        if (_strStreamID.size() > 0)
        {
            std::string __strDstIKey;

            CCacheKeyValue::bldCacheKey(__strDstIKey, _strStreamID, PssNotifyMsg_Key_Notify_PlayTTime);

            time_t __PlayTTime = time(NULL);
            GetHandler()->m_pCacheKeyValue->modPair(__strDstIKey, __PlayTTime, modPair_If_Not_Exist_Then_Add);

DBG(
            ez_printf_debug("Fresh:[%s:%ld]\n", __strDstIKey.c_str(), __PlayTTime);
);
        }

    }
    while (0);
}

