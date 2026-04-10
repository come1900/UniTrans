/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentTouchEdge.cpp - Touch Edge Agent
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentTouchEdge.cpp 1 2026-03-15 Create $
 *
 *  Explain:
 *     Touch Edge Agent - integrates touch_edge as a component
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "AgentTouchEdge.h"

#include <unistd.h>
#include "../../../Logs.h"
#include "../../../Solar.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigManager.h"
#endif //_USE_MODULES_EZCONFIG

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

PATTERN_SINGLETON_IMPLEMENT(CAgentTouchEdge);

CAgentTouchEdge::CAgentTouchEdge():CEZThread("CAgentTouchEdge", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2)
{
    m_iUser = 0;
    m_Config.IngressHost = "127.0.0.1";
    m_Config.IngressPort = 54321;
    m_Config.EdgeId = "device001";
    m_Config.EdgeKey = "key001";
    m_Config.EdgeType = "touch";
    m_Config.iConnTimeOut = 5;

    m_pFunRegisterCli = new CFunRegisterCli(m_Config.EdgeId, m_Config.EdgeKey, m_Config.EdgeType);
    assert(m_pFunRegisterCli);

    m_pCfgTouchEdge = new CConfigTouchEdge;
    assert(m_pCfgTouchEdge);   
}

CAgentTouchEdge::~CAgentTouchEdge()
{
    if (m_pFunRegisterCli)
    {
        delete m_pFunRegisterCli;
        m_pFunRegisterCli = NULL;
    }

    delete m_pCfgTouchEdge;
}

CFunRegisterCli * CAgentTouchEdge::GetFunRegisterCli()
{
    return m_pFunRegisterCli;
}

void CAgentTouchEdge::SetIngressHost(const std::string& host)
{
    m_Config.IngressHost = host;
}

void CAgentTouchEdge::SetIngressPort(unsigned short port)
{
    m_Config.IngressPort = port;
}

void CAgentTouchEdge::SetEdgeId(const std::string& edgeId)
{
    m_Config.EdgeId = edgeId;
}

void CAgentTouchEdge::SetEdgeKey(const std::string& edgeKey)
{
    m_Config.EdgeKey = edgeKey;
}

void CAgentTouchEdge::SetEdgeType(const std::string& edgeType)
{
    m_Config.EdgeType = edgeType;
}

EZTHREAD_BOOL CAgentTouchEdge::Start(CEZObject * pObj, TouchEdgeSignalProc_t pProc)
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

#ifdef _USE_MODULES_EZCONFIG

        m_pCfgTouchEdge->update();

        m_Config.EdgeId = m_pCfgTouchEdge->getConfig().strEdgeId;

        if (m_Config.EdgeId.length() < def_Min_TouchEdge_Name_Length)
        {
            m_Config.EdgeId = "G1"+g_Solar.GetHuid();

            m_pCfgTouchEdge->getConfig().strEdgeId = m_Config.EdgeId;
            m_pCfgTouchEdge->commit();
            ez_printf_info(" strEdgeId: %s\n", m_pCfgTouchEdge->getConfig().strEdgeId.c_str());
        }

        ez_printf_info(" EdgeId: %s\n", m_Config.EdgeId.c_str());

#else
        m_Config.EdgeId = "G1"+g_Solar.GetHuid();
        //m_strNodeId = g_Solar.GetHuid();
        ez_printf_info(" EdgeId: %s\n", m_Config.EdgeId.c_str());
#endif

        // 打印关键配置信息
        ez_printf_info("=== Touch Edge Configuration ===\n");
        ez_printf_info(" IngressHost: %s\n", m_Config.IngressHost.c_str());
        ez_printf_info(" IngressPort: %d\n", m_Config.IngressPort);
        ez_printf_info(" EdgeId: %s\n", m_Config.EdgeId.c_str());
        ez_printf_info(" EdgeType: %s\n", m_Config.EdgeType.c_str());
        ez_printf_info(" Timeout: %d sec\n", m_Config.iConnTimeOut);
        ez_printf_info("================================\n");

        // 更新 FunRegisterCli 的设备参数（使用配置加载后的值）
        m_pFunRegisterCli->UpdateEdgeParams(m_Config.EdgeId, m_Config.EdgeKey, m_Config.EdgeType);

        // 启动 FunRegisterCli
        m_pFunRegisterCli->Start(m_Config.IngressHost.c_str(), m_Config.IngressPort);

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

EZTHREAD_BOOL CAgentTouchEdge::Stop(CEZObject * pObj, TouchEdgeSignalProc_t pProc)
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
        // 停止 FunRegisterCli
        if (m_pFunRegisterCli)
        {
            m_pFunRegisterCli->Stop();
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

void CAgentTouchEdge::SetConfig()
{
#ifdef _USE_MODULES_EZCONFIG
    CConfigTouchEdge __cfg;
    __cfg.update();

    m_Config.IngressHost = __cfg.getConfig().strIngressHost;
    m_Config.IngressPort = __cfg.getConfig().iIngressPort > 0 ? __cfg.getConfig().iIngressPort : 54321;
    m_Config.EdgeId = __cfg.getConfig().strEdgeId;
    m_Config.EdgeKey = __cfg.getConfig().strEdgeKey;
    m_Config.EdgeType = __cfg.getConfig().strEdgeType;
    m_Config.iConnTimeOut = __cfg.getConfig().iConnectTimeout > 0 ? __cfg.getConfig().iConnectTimeout : 5;
#else
    // 默认配置
    m_Config.IngressHost = "127.0.0.1";
    m_Config.IngressPort = 54321;
    m_Config.EdgeId = "device001";
    m_Config.EdgeKey = "key001";
    m_Config.EdgeType = "touch";
    m_Config.iConnTimeOut = 5;
#endif
}

void CAgentTouchEdge::ThreadProc()
{
    while (m_bLoop)
    {
        // 主循环，目前不需要做额外处理
        sleep(1);
    }
}
