/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentTouchIngress.cpp - Touch Ingress Agent
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentTouchIngress.cpp 1 2026-03-15 Create $
 *
 *  Explain:
 *     Touch Ingress Agent - integrates touch_ingress as a component
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "AgentTouchIngress.h"

#include <unistd.h>
#include "../../../Logs.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigManager.h"
#endif //_USE_MODULES_EZCONFIG

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

PATTERN_SINGLETON_IMPLEMENT(CAgentTouchIngress);

CAgentTouchIngress::CAgentTouchIngress():CEZThread("CAgentTouchIngress", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2)
{
    m_iUser = 0;
    m_Config.Port = 54321;
    m_Config.iConnTimeOut = 5;

    m_pFunRegisterSvr = new CFunRegisterSvr();
    assert(m_pFunRegisterSvr);
}

CAgentTouchIngress::~CAgentTouchIngress()
{
    m_Config.Port = 0;
    m_Config.iConnTimeOut = 0;

    if (m_pFunRegisterSvr)
    {
        delete m_pFunRegisterSvr;
        m_pFunRegisterSvr = NULL;
    }
}

CFunRegisterSvr * CAgentTouchIngress::GetFunRegisterSvr()
{
    return m_pFunRegisterSvr;
}

void CAgentTouchIngress::SetPort(unsigned short port)
{
    m_Config.Port = port;
}

void CAgentTouchIngress::SetProtocol(const std::string& protocol)
{
    m_Config.strProtocol = protocol;
}

void CAgentTouchIngress::SetPathPrefix(const std::string& pathPrefix)
{
    m_Config.strPathPrefix = pathPrefix;
}

EZTHREAD_BOOL CAgentTouchIngress::Start(CEZObject * pObj, TouchIngressSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    int iret = 0;
    iret = m_SigBuffer.Attach(pObj, pProc);
    if( iret < 0)
    {
        ez_printf_error("Attach error(%d)\n", iret);
        return bRet;
    }

    if (m_iUser == 0)
    {
        SetConfig();

        // 打印关键配置信息
        ez_printf_info("=== Touch Ingress Configuration ===\n");
        ez_printf_info(" Host: %s\n", m_Config.strHost.c_str());
        ez_printf_info(" Port: %d\n", m_Config.Port);
        ez_printf_info(" Protocol: %s\n", m_Config.strProtocol.c_str());
        ez_printf_info(" PathPrefix: %s\n", m_Config.strPathPrefix.c_str());
        ez_printf_info(" Timeout: %d sec\n", m_Config.iConnTimeOut);
        ez_printf_info("===================================\n");

        // 启动 FunRegisterSvr (使用配置中的 WebSocket 参数)
        m_pFunRegisterSvr->Start(m_Config.Port, m_Config.strProtocol.c_str(), m_Config.strPathPrefix.c_str());

        int ret = CreateThread();
        ARG_USED(ret);

        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        bRet = EZTHREAD_BOOL_TRUE;
        ez_printf_debug("no need, CreateThread, have %d users.\n", m_iUser);
    }

    m_iUser++;
    DBG(ez_printf_debug("m_iUser:%d\n", m_iUser););

    return bRet;
}

EZTHREAD_BOOL CAgentTouchIngress::Stop(CEZObject * pObj, TouchIngressSignalProc_t pProc)
{
    ez_printf_debug("%s\n", __FUNCTION__);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    if(m_iUser > 0)
    {
        m_iUser--;
    }

    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        ez_printf_error("Detach failed.\n");
    }

    if (m_iUser == 0)
    {
        // 停止 FunRegisterSvr
        if (m_pFunRegisterSvr)
        {
            m_pFunRegisterSvr->Stop();
        }

        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            ez_printf_error("DestroyThread failed.\n");
        }
        else
        {
            ez_printf_debug("DestroyThread OK.\n");
        }
    }
    else
    {
        ez_printf_debug("no need, DestroyThread, have %d users.\n", m_iUser);
    }

    return bRet;
}

void CAgentTouchIngress::SetConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigTouchIngress __cfg;
    __cfg.update();

    m_Config.strHost = __cfg.getConfig().strHost;
    m_Config.Port = __cfg.getConfig().iPort > 0 ? __cfg.getConfig().iPort : 54321;
    m_Config.strProtocol = __cfg.getConfig().strProtocol;
    m_Config.strPathPrefix = __cfg.getConfig().strPathPrefix;
    m_Config.iConnTimeOut = __cfg.getConfig().PeerTimeout > 0 ? __cfg.getConfig().PeerTimeout : 5;
    m_Config.EdgeReportMode = __cfg.getConfig().EdgeReportMode;
#else
    // 默认配置
    m_Config.strHost = "0.0.0.0";
    m_Config.Port = 54321;
    m_Config.strProtocol = "come.1";
    m_Config.strPathPrefix = "/come";
    m_Config.iConnTimeOut = 5;
    m_Config.EdgeReportMode = 1;  // 默认：上报所有已连接的 Edge
#endif

    // 将配置同步到 FunRegisterSvr
    if (m_pFunRegisterSvr) {
        m_pFunRegisterSvr->SetEdgeReportMode(m_Config.EdgeReportMode);
    }
}

void CAgentTouchIngress::ThreadProc()
{
    while (m_bLoop)
    {
        // 主循环，目前不需要做额外处理
        sleep(1);
    }
}
