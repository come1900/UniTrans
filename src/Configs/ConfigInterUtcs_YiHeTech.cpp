/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigInterUtcs_YiHeTech.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigInterUtcs_YiHeTech.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigInterUtcs_YiHeTech.h"

template<>
void exchangeTable<ConfigInterUtcs_YiHeTech>(CConfigExchange& xchg, CConfigTable& table, ConfigInterUtcs_YiHeTech& config, int index, int app)
{
    xchg.exchange(table, "InterUtcs_YiHeTechPort"    , config.iInterUtcs_YiHeTechPort, 0, 65535 ,60088);
    xchg.exchange(table, "ConnTimeout"    , config.ConnTimeout, 0, 65535 ,5);

    xchg.exchange(table, "getSignalControlerPara",      config.strURIgetSignalControlerPara     , "/yh1/getSignalControlerPara");
    xchg.exchange(table, "getCrossPara", config.strURIgetCrossPara, "/yh1/getCrossPara");
    xchg.exchange(table, "getCrossStatus",  config.strURIgetCrossStatus , "/yh1/getCrossStatus");
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}
