/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigFaceWebs.cpp - brief
 *
 * Copyright (C) 2013 FaceMetro.com, All Rights Reserved.
 *
 * $Id: ConfigFaceWebs.cpp 5884 2013-06-19 10:14:01Z WuJunjie $
 *
 *  Notes:
 *     -
 *      for smartwebs
 *     -
 *
 *  Update:
 *     2013-06-19 10:14:01  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/



#include "ConfigFaceWebs.h"

template<>
void exchangeTable<ConfigFaceWebs>(CConfigExchange& xchg, CConfigTable& table, ConfigFaceWebs& config, int index, int app)
{
    xchg.exchange(table, "SPort"    , config.SPort,       0, 65535 ,80);
    xchg.exchange(table, "iRetries"    , config.iRetries,       0, 65535 ,5);

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 ,0);
    xchg.exchange(table,"PeerTimeout", config.PeerTimeout,  0, 65535 ,300);

    xchg.exchange(table, "DefaultPages"    , config.strDefaultPages    , "index.asp");
    xchg.exchange(table, "RootPath"    , config.strRootPath    , "./spages/");
}
