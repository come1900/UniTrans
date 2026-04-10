/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileList.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileList.cpp 5884 2018-03-19 11:25:47Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-19 11:25:47  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "FileList.h"

int dumpFileList(CFileList_t *pFileList)
{
	if (NULL == pFileList)
	{
		return -1;
	}

    CFileList_t::iterator it = pFileList->begin();
    if (it != pFileList->end())
    {
        it->dump(0);

        for ( ; it != pFileList->end() ; it++)
        {
            it->dump();
        }
    }
    
    return 0;
}
