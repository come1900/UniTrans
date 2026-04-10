/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * ConfigStreamPusher.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: ConfigStreamPusher.h 5884 2021-03-17 09:59:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2021-03-17 09:59:14  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_ConfigStreamPusher_H)
#define _ConfigStreamPusher_H

#include "ConfigBase.h"
//!
typedef struct tagConfigStreamPusher
{
    int iPushInterval; // 重试间隔， 0 默认， >10 用这个参数， 1-10 备用
}
ConfigStreamPusher;

//1 -结构数目
//4 -观察者最大数目
typedef TConfig<ConfigStreamPusher, 1, 4> CConfigStreamPusher;

#endif // !defined(_ConfigStreamPusher_H)
