/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigTouchEdge.cpp - Touch Edge Configuration Implementation
 *
 * Copyright (C) 2026 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigTouchEdge.cpp 1 2026-03-15 Create $
 *
 *  Explain:
 *     Configuration implementation for Touch Edge client
 *
 *  Update:
 *     2026-03-15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigTouchEdge.h"

template<> void exchangeTable<ConfigTouchEdge>(CConfigExchange& xchg, CConfigTable& table, ConfigTouchEdge& config, int index, int app)
{
    xchg.exchange(table, "IngressHost"    , config.strIngressHost    , "47.100.49.48");
    xchg.exchange(table, "IngressPort"    , config.iIngressPort    , 1, 65535, 54321);
    
    xchg.exchange(table, "EdgeId"    , config.strEdgeId    , def_Default_TouchEdge_Name);
    xchg.exchange(table, "EdgeKey"    , config.strEdgeKey    , "key001");
    xchg.exchange(table, "EdgeType"    , config.strEdgeType    , "touch");
    
    xchg.exchange(table, "ReconnectInterval", config.iReconnectInterval,  1, 3600, 30);
    xchg.exchange(table, "ReconnectMaxRetries", config.iReconnectMaxRetries,  -1, 1000, -1);
    xchg.exchange(table, "HeartbeatInterval", config.iHeartbeatInterval,  5, 3600, 30);
    xchg.exchange(table, "ConnectTimeout", config.iConnectTimeout,  1, 300, 10);
}
