/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FaceWebs.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: FaceWebs.h 5884 2013-07-01 04:32:32Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-07-01 04:32:32 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _FaceWebs_H
#define _FaceWebs_H 1

#include "EZThread.h"
#include "EZTimer.h"

#define g_FaceWebs (*CFaceWebs::instance())

class CFaceWebs : public CEZThread
{
public:
    PATTERN_SINGLETON_DECLARE(CFaceWebs);
	
	EZTHREAD_BOOL Start();
	EZTHREAD_BOOL Stop();
	void ThreadProc();
private:
	CFaceWebs();
	virtual ~CFaceWebs();
    // ÐÂÔö
#ifdef XP_FaceWebs

    int StartSmartWebs();
    int StopSmartWebs();
#endif//XP_FaceWebs

 };
#endif /* _FaceWebs_H */
