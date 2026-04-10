/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigConsoleOverTcp.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigConsoleOverTcp.cpp 5884 2013-06-07 11:28:21Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 11:28:21  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "../UniTransDef.h"

#include "ConfigConsoleOverTcp.h"

#if defined(_FUNC_PandoraMatrix)
    #define __def_ConsoleOverTcpPort 60024
#elif defined(_FUNC_PandoraNode)
    #define __def_ConsoleOverTcpPort 60025
#elif defined(_FUNC_LiveStreamming)
    #define __def_ConsoleOverTcpPort 60026
#elif defined(_FUNC_LiveRecord)
    #define __def_ConsoleOverTcpPort 60027
#elif defined(_FUNC_MinerMtService)
    #define __def_ConsoleOverTcpPort 60028
#else
    #define __def_ConsoleOverTcpPort 60023
#endif

template<> void exchangeTable<ConfigConsoleOverTcp>(CConfigExchange& xchg, CConfigTable& table, ConfigConsoleOverTcp& config, int index, int app)
{
    //xchg.exchange(table, "SPort"    , config.SPort,       HTTPD_MIN_PORT, 65535 , __def_ConsoleOverTcpPort);
    xchg.exchange(table, "SPort"    , config.SPort,       0, 65535 , __def_ConsoleOverTcpPort); // 0 - do not start

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"PeerTimeout", config.PeerTimeout,  0, 65535 ,300);
}
