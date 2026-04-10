/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTouchIngress.cpp - Touch Ingress Configuration Implementation
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTouchIngress.cpp 1 2026-03-15 Create $
 *
 *  Explain:
 *     Configuration implementation for Touch Ingress server
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigTouchIngress.h"

template<> void exchangeTable<ConfigTouchIngress>(CConfigExchange& xchg, CConfigTable& table, ConfigTouchIngress& config, int index, int app)
{
    xchg.exchange(table, "Host"    , config.strHost    , "0.0.0.0");
    xchg.exchange(table, "Port"    , config.iPort    , 1, 65535, 54321);
    
    // WebSocket 配置
    xchg.exchange(table, "Protocol"    , config.strProtocol    , "come.1");
    xchg.exchange(table, "PathPrefix"    , config.strPathPrefix    , "/come");
    
    xchg.exchange(table, "CheckPeriod", config.CheckPeriod,  0, 86400, 30);
    xchg.exchange(table, "PeerTimeout", config.PeerTimeout,  5, 3600, 60);
    xchg.exchange(table, "MaxConnections", config.iMaxConnections,  0, 10000, 1000);
    xchg.exchange(table, "EdgeReportMode", config.EdgeReportMode,  0, 2, 1);
}
