/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CHandlerGRestHttpdSocket.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CHandlerGRestHttpdSocket.cpp 5884 2019-11-22 11:26:39Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-11-22 11:26:39  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "HandlerGRestHttpdSocket.h"

CHandlerGRestHttpdSocket::CHandlerGRestHttpdSocket(CMaintainAgentLog *pLog) : SocketHandlerEp(pLog)
{
#if defined(_FUNC_OAuth20_GRestful)
    m_pCacheWithDb = NULL;
    m_pCacheKeyValue = NULL;
#endif
}

CHandlerGRestHttpdSocket::~CHandlerGRestHttpdSocket()
{
#if defined(_FUNC_OAuth20_GRestful)
    if (m_pCacheWithDb)
    {
        delete m_pCacheWithDb;
    }

    if (m_pCacheKeyValue)
    {
        delete m_pCacheKeyValue;
    }
#endif
}

#if defined(_FUNC_OAuth20_GRestful)
const std::string PairMark_AppKey = "AppKey__";
const std::string PairMark_Account = "Account__";
int CHandlerGRestHttpdSocket::setAppKey(const std::string strAppKey, const std::string strAppSecret)
{
    if (NULL == m_pCacheWithDb)
    {
        return -10001;
    }

    if (strAppKey.length() < 1)
    {
        return -10002;
    }

    if (strAppSecret.length() < 1)
    {
        return -10003;
    }

    return m_pCacheWithDb->addPair(PairMark_AppKey+strAppKey, strAppSecret);
}

int CHandlerGRestHttpdSocket::getAppKey(const std::string strAppKey, std::string &strAppSecret)
{
    if (NULL == m_pCacheWithDb)
    {
        return -10001;
    }

    if (strAppKey.length() < 1)
    {
        return -10002;
    }

    return m_pCacheWithDb->getPair(PairMark_AppKey+strAppKey, strAppSecret);
}

int CHandlerGRestHttpdSocket::cancelAppKey(const std::string &strAppKey)
{
    if (NULL == m_pCacheWithDb)
    {
        return -10001;
    }

    if (strAppKey.length() < 1)
    {
        return -10002;
    }

    return m_pCacheWithDb->delPair(PairMark_AppKey+strAppKey);
}

int CHandlerGRestHttpdSocket::setAccount(const std::string strAccount, const std::string strPasswd)
{
    if (NULL == m_pCacheWithDb)
    {
        return -10001;
    }

    if (strAccount.length() < 1)
    {
        return -10002;
    }

    if (strPasswd.length() < 1)
    {
        return -10003;
    }

    return m_pCacheWithDb->addPair(PairMark_Account+strAccount, strPasswd);
}

int CHandlerGRestHttpdSocket::getAccount(const std::string strAccount, std::string &strPasswd)
{
    if (NULL == m_pCacheWithDb)
    {
        return -10001;
    }

    if (strAccount.length() < 1)
    {
        return -10002;
    }

    return m_pCacheWithDb->getPair(PairMark_Account+strAccount, strPasswd);
}
#endif //_FUNC_OAuth20_GRestful

