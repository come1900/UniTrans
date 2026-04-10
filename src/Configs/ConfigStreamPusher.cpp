/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigStreamPusher.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigStreamPusher.cpp 5884 2021-03-17 10:05:19Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-03-17 10:05:19  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigStreamPusher.h"

template<>
void exchangeTable<ConfigStreamPusher>(CConfigExchange& xchg, CConfigTable& table, ConfigStreamPusher& config, int index, int app)
{
    xchg.exchange(table, "01_PushInterval"    , config.iPushInterval, 0, 65535 ,0);
}
