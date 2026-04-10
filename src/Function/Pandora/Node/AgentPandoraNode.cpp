/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * AgentPandoraNode.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: AgentPandoraNode.cpp 5884 2013-01-29 03:21:27Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-29 03:21:27  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TypeDefPandoraNode.h"

#include "../../../Logs.h"

#ifdef _USE_MODULES_EZCONFIG
#include "../../../Configs/ConfigPandoraNode.h"
#endif //_USE_MODULES_EZCONFIG

#include "AgentPandoraNode.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

PATTERN_SINGLETON_IMPLEMENT(CAgentPandoraNode);

CAgentPandoraNode::CAgentPandoraNode():CEZThread("CAgentPandoraNode", THREAD_PRIORITY_DEFAULT), m_SigBuffer(2)
        //CEZThread("CAgentPandoraNode", THREAD_PRIORITY_DEFAULT, 10), m_SigBuffer(0x02/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    m_ConfigOfPandoraNode.HttpServicePort = 60088; // 如果不启用，则设置为0

    m_pHandlerPandoraNode = new CHandlerPandoraNode(&m_Log);
    assert(m_pHandlerPandoraNode);

    ///////////////////////////////////////////////
}
CAgentPandoraNode::~CAgentPandoraNode()
{
    m_ConfigOfPandoraNode.HttpServicePort = 0;
    // handler里面自管理
    m_pCPandoraNodeKeepConnector = NULL;

    if (m_pHandlerPandoraNode)
    {
        delete m_pHandlerPandoraNode;
    }
}

CHandlerPandoraNode * CAgentPandoraNode::GetHandler()
{
    return m_pHandlerPandoraNode;
}

void CAgentPandoraNode::SetGeneralHttpAgent(unsigned short  port)
{
    m_ConfigOfPandoraNode.HttpServicePort = port;
}

void CAgentPandoraNode::ConnectAll()
{
    {
        CPandoraNodeKeepConnector *_pConnector
        = new CPandoraNodeKeepConnector(*m_pHandlerPandoraNode, m_ConfigOfPandoraNode.strServerName);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfPandoraNode.strHost.c_str(), m_ConfigOfPandoraNode.Port);
            if (bRet)
            {
                ez_printf_debug("%s--%s:%d Opened\n"
                                , m_ConfigOfPandoraNode.strServerName.c_str()
                                , m_ConfigOfPandoraNode.strHost.c_str()
                                , m_ConfigOfPandoraNode.Port);

                m_pHandlerPandoraNode->Add(_pConnector);
            }
            else
            {
                ez_printf_error("Failed Open(%s):%s:%d\n"
                                , m_ConfigOfPandoraNode.strServerName.c_str()
                                , m_ConfigOfPandoraNode.strHost.c_str()
                                , m_ConfigOfPandoraNode.Port);
            }
        }
        else
        {
            ez_printf_error("Not enough mem.\n");
        }
    }

    //多连接
#if 0
    // 多客户端的情况
    if (0)
    {
        CPandoraNodeKeepConnector *_pConnector
        = new CPandoraNodeKeepConnector(*m_pHandlerPandoraNode, m_ConfigOfPandoraNode.strServerName+"Internet");
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfPandoraNode.strHost.c_str(), m_ConfigOfPandoraNode.Port);
            if (bRet)
            {
                printf("PandoraNodeConnector:%s:%d Open\n"
                       , m_ConfigOfPandoraNode.strHost.c_str()
                       , m_ConfigOfPandoraNode.Port);

                m_pHandlerPandoraNode->Add(_pConnector);
            }
            else
            {
                __trip;
                // 失败处理
            }
        }
        else
        {
            __trip;
            //失败处理
        }
    }

    // 多客户端的情况
    std::string strName = "Ci";
    //for (int ii=0; ii<50; ii++)
    for (int ii=0; ii<0; ii++)
    {
        CPandoraNodeKeepConnector *_pConnector
        = new CPandoraNodeKeepConnector(*m_pHandlerPandoraNode, strName/*m_ConfigOfPandoraNode.strServerName+"Internet"*/);
        if (_pConnector)
        {
            bool bRet = _pConnector->Open(m_ConfigOfPandoraNode.strHost.c_str(), m_ConfigOfPandoraNode.Port);
            if (bRet)
            {
                ez_printf_info("PandoraNodeConnector:%s:%d Open\n"
                               , m_ConfigOfPandoraNode.strHost.c_str()
                               , m_ConfigOfPandoraNode.Port);

                m_pHandlerPandoraNode->Add(_pConnector);
            }
            else
            {
                __trip;
                // 失败处理
            }
        }
        else
        {
            __trip;
            //失败处理
        }

        strName += "Ci";

    }
#endif //多连接
}

void CAgentPandoraNode::DisconnectAll()
{}

EZTHREAD_BOOL CAgentPandoraNode::Start(CEZObject * pObj, PandoraNodeSignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    int iret=0;
    iret = m_SigBuffer.Attach(pObj, pProc);
    if( iret < 0)
    {
        //重复调用
        // ez_printf_error("Attach error(%d)\n", iret);
        return bRet;
    }
    else
    {
        // ez_printf_info("Attach OK(%d)\n", iret);
    }

    if (m_iUser==0)
    {

        SetConfig();

        ConnectAll();

        // 初始化一些变量
        m_pHandlerPandoraNode->Start();

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

EZTHREAD_BOOL CAgentPandoraNode::Stop(CEZObject * pObj, PandoraNodeSignalProc_t pProc)
{
    ez_printf_debug("%s\n", __FUNCTION__);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iUser>0)
    {
        m_iUser--;
    }

    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
        // m_iUser--;
        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        ez_printf_error("Detach failed.\n");
    }

    if (m_iUser==0)
    {
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

void CAgentPandoraNode::SetConfig()
{
#ifdef _USE_MODULES_EZCONFIG

    CConfigPandoraNode __cfg;
    __cfg.update();

    m_ConfigOfPandoraNode.IsUse = 1;
    m_ConfigOfPandoraNode.Port = __cfg.getConfig().iMatrixPort;
    m_ConfigOfPandoraNode.strHost = __cfg.getConfig().strMatrixHost;

    // will find by name
    m_ConfigOfPandoraNode.strServerName = CPandoraNodeKeepConnector_SOCKET_NAME;

    m_ConfigOfPandoraNode.iConnTimeOut = 5;//超时时间
    m_ConfigOfPandoraNode.iConnTimes = 5; //连接尝试次数（保留）

    m_ConfigOfPandoraNode.iInterval = 5; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    m_ConfigOfPandoraNode.bEnableRecon  = 1;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    m_ConfigOfPandoraNode.strUName = "root";
    m_ConfigOfPandoraNode.strUPwd = "admin";
#else

    m_ConfigOfPandoraNode.IsUse = 1;
    m_ConfigOfPandoraNode.Port = DEFAULT_SMS_HOST_PORT;
    m_ConfigOfPandoraNode.strHost = DEFAULT_SMS_HOST;

    // will find by name
    m_ConfigOfPandoraNode.strServerName = CPandoraNodeKeepConnector_SOCKET_NAME;

    m_ConfigOfPandoraNode.iConnTimeOut = 5;//超时时间
    m_ConfigOfPandoraNode.iConnTimes = 5; //连接尝试次数（保留）

    m_ConfigOfPandoraNode.iInterval = 5; //[in] 重连间隔，单位毫秒，参数默认值为30秒
    m_ConfigOfPandoraNode.bEnableRecon  = 1;//[in] 是否重连，0-不重连，1-重连，参数默认值为1

    m_ConfigOfPandoraNode.strUName = "root";
    m_ConfigOfPandoraNode.strUPwd = "admin";

#endif
}

void CAgentPandoraNode::ThreadProc()
{
    while (m_bLoop)
    {
        Run();
    } //while (m_bLoop)
}
void CAgentPandoraNode::Run()
{
    static int iFirstRun = 0;

    if (1==iFirstRun)
    {
        m_pHandlerPandoraNode->Select(1,0);
        iFirstRun  = 0;
    }

    if (!m_pHandlerPandoraNode->GetCount())
    {
        sleep(1);

        // 配合login失败计数， 实现重连接机制。
        // 在连接某些单片机系统或者localhost的时候防止异常的处理
        ez_printf_info("Connect ...\n");
        ConnectAll();
    }
    else
    {
        m_pHandlerPandoraNode->Select(1,0);
    }

    m_pHandlerPandoraNode->Update();

    static MSG_PandoraNodeData_T __msg;

    __msg.iConnTimes = m_pHandlerPandoraNode->getTryConnTimes();

    //ez_printf_debug("__msg.iConnTimes:%d\n", __msg.iConnTimes);

    if (__msg.iConnTimes > 1)
    {
        __msg.ttLastConnTime = time(NULL);

        if (__msg.ttFirstConnTime == 0)
        {
            __msg.ttFirstConnTime  = __msg.ttLastConnTime;
        }

        //ez_printf_debug("__msg.iConnTimes:%d\n", __msg.iConnTimes);

        if (m_pHandlerPandoraNode)
        {
            if (m_pHandlerPandoraNode->getCfgPandoraNode())
            {
                if (m_pHandlerPandoraNode->getCfgPandoraNode()->getConfig().iMaxConnSecs <= 0)
                {
                    // do nothing
                    return;
                }
				
                ez_printf_debug("iMaxConnSecs:%d\n", m_pHandlerPandoraNode->getCfgPandoraNode()->getConfig().iMaxConnSecs);

                //大于次数， 报告尝试失败
                if (m_pHandlerPandoraNode->getCfgPandoraNode()->getConfig().iMaxConnSecs > 0
                    && __msg.ttFirstConnTime >0
                    && ((__msg.ttLastConnTime-__msg.ttFirstConnTime) >= m_pHandlerPandoraNode->getCfgPandoraNode()->getConfig().iMaxConnSecs))
                {
                    m_SigBuffer(MSG_PANDORANODE_T_CONNECT_TRY_FAILED, &__msg);
                }
                else
                {
                    m_SigBuffer(MSG_PANDORANODE_T_CONNECT_TRYING, &__msg);
                }
            }
            else
            {
                ez_printf_error("m_pHandlerPandoraNode->getCfgPandoraNode() not valid.\n");
            }
        }
        else
        {
            ez_printf_error("m_pHandlerPandoraNode not valid\n");
        }
    }// if
    else
    {
        // 连接成功
        __msg.ttFirstConnTime = 0;
    }
}

