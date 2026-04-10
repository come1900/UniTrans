/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigPandoraNode.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigPandoraNode.cpp 5884 2013-06-07 10:19:04Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-07 10:19:04  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "ConfigPandoraNode.h"
#include "../Function/Pandora/PandoraDef.h"

template<> void exchangeTable<ConfigPandoraNode>(CConfigExchange& xchg, CConfigTable& table, ConfigPandoraNode& config, int index, int app)
{
    xchg.exchange(table, "NodeName"    , config.strNodeName    , def_Default_Node_Name); // !<3, will use huid

    //xchg.exchange(table, "AppKey"    , config.AppKey, "AppKey");
    xchg.exchange(table, "AppKey"    , config.AppKey, "701819701");
    xchg.exchange(table, "AppSecret"    , config.AppSecret, "AppSecret");
#if defined(_VEND_rainMan)
    xchg.exchange(table, "MatrixHost"    , config.strMatrixHost    , "rainman.pq.ezlibs.com");
#elif defined(_VEND_sinVision)
    xchg.exchange(table, "MatrixHost"    , config.strMatrixHost    , "sinvision.pq.ezlibs.com");
#elif defined(_VEND_VSA)
    xchg.exchange(table, "MatrixHost"    , config.strMatrixHost    , "vsa.pq.ezlibs.com");
#else
    xchg.exchange(table, "MatrixHost"    , config.strMatrixHost    , def_Pandora_MatrixHost);
#endif
    xchg.exchange(table, "MatrixPort"    , config.iMatrixPort    , 0, 65535 , def_Pandora_MatrixPort);

    xchg.exchange(table,"CheckPeriod", config.CheckPeriod,  0, 65535 , (int)(def_Pandora_ConnTimeout/2.0 - 0.5));

    xchg.exchange(table, "IpcChn"    , config.iIpcChn    , 0, 65535 , 1);

    xchg.exchange(table, "MaxConnSecs"    , config.iMaxConnSecs    , 0, 2678400/*3600*24*31*/, 15*60);// Ä¬ÈÏ15·ÖÖÓ
}

