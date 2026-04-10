/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileList.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileList.h 5884 2018-03-19 11:25:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-19 11:25:44  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "FileNode.h"

#if !defined(_FileList_H)
#define _FileList_H

typedef std::list<CFileNode> CFileList_t;
	
#ifdef __cplusplus
extern "C" {
#endif

int dumpFileList(CFileList_t *pFileList);

#ifdef __cplusplus
}
#endif

#endif // !defined(_FileList_H)