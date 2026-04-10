/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * CHandlerGRestHttpdSocket.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: CHandlerGRestHttpdSocket.h 5884 2019-11-22 11:26:54Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-11-22 11:26:54  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <EZThread.h>

#include <SocketHandlerEp.h>
#include <TcpSocket.h>
#include "../UniService/DefUniService.h"
#include "../../UniTransDef.h"
#include "../../DBAgent/CacheKeyValue.h"

#include "../../Maintain/GeneralAgentLog.h"

#if !defined(_HandlerGRestHttpdSocket_H)
#define _HandlerGRestHttpdSocket_H

//#define _FUNC_OAuth20_GRestful
class CHandlerGRestHttpdSocket : public SocketHandlerEp,  public CEZObject
{
public:

    CHandlerGRestHttpdSocket(CMaintainAgentLog *pLog=NULL);
    virtual ~CHandlerGRestHttpdSocket();

#if defined(_FUNC_OAuth20_GRestful)
public:
    int setAppKey(const std::string strAppKey, const std::string strAppSecret);
    int getAppKey(const std::string strAppKey, std::string &strAppSecret);
    int cancelAppKey(const std::string &strAppKey);

    int setAccount(const std::string strAccount, const std::string strPasswd);
    int getAccount(const std::string strAccount, std::string &strPasswd);
    // 节点信息收集, 使用数据库持久化
    CCacheKeyValue *m_pCacheWithDb;
    // 数据采集用, 不使用数据库持久化
    CCacheKeyValue *m_pCacheKeyValue;
#endif //_FUNC_OAuth20_GRestful
private:

};

#endif // !defined(_HandlerGRestHttpdSocket_H)
