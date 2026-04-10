/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PuClientDahua.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: PuClientDahua.h 5884 2019-05-16 05:16:41Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-05-16 05:16:41  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <unistd.h>
#include "../../Logs.h"
#include "../../Solar.h"

#include "PuClient.h"

#include "dhnetsdk.h"

#if !defined(_PuClientDahua_H)
#define _PuClientDahua_H

class CPuClientDahua : public CPuClient
{
public:

    CPuClientDahua();
    virtual ~CPuClientDahua();

    virtual EZTHREAD_BOOL Start(CEZObject * pObj, PuClientSignalProc_t pProc);
    virtual void onTask();

private:
    int doLogin();
    LLONG m_lLoginId;
    int doHeartbeat();
    int doHARDDISK_STATE();
};

#endif // !defined(_PuClientDahua_H)
