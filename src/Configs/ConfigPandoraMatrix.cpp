/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPandoraMatrix.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPandoraMatrix.cpp 5884 2012-09-07 10:46:00Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-09-07 10:46:00  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigPandoraMatrix.h"

#include "../Function/Pandora/PandoraDef.h"

template<>
void exchangeTable<ConfigPandoraMatrix>(CConfigExchange& xchg, CConfigTable& table, ConfigPandoraMatrix& config, int index, int app)
{
    xchg.exchange(table, "PandoraMatrixPort"    , config.iPandoraMatrixPort, 0, 65535 , def_Pandora_MatrixPort);
    xchg.exchange(table, "udpPort"    , config.iPandoraMatrixPortUdp, 0, 65535 , def_Pandora_MatrixPort);
    xchg.exchange(table, "ConnTimeout"    , config.ConnTimeout, 0, 65535 ,def_Pandora_ConnTimeout);

    xchg.exchange(table, "AuthType"    , config.iAuth, 0, 0xfffffff, 0x1); //bit1_AppKey

    xchg.exchange(table, "CleanValuesWhenDisconnect"    , config.iCleanValuesWhenDisconnect, 0, 1, def_CleanValuesWhenDisconnect); //
}

