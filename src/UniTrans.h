/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * UniTrans.h - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: UniTrans.h 5884 2014-02-26 02:21:04Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2014-02-26 02:21:04 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _UniTrans_H
#define _UniTrans_H

#ifdef __cplusplus
extern "C"
{
#endif

int UniTransInitialize(int argc, char *argv[]);
int UniTransStart();
int UniTransDone();
int haveException(int ExcNo, char *pMsg);
int Term();

#ifdef __cplusplus
}
#endif

#endif //_UniTrans_H


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/* USAGE
	//--------------------------
	printf("orig:%s\n", pOrig);
	usret = ez_crc16(0x0, (unsigned char *)pOrig, strlen(pOrig));
	printf("ez_crc16:0x%0X\n", usret);

	//--------------------------
	printf("orig:%s\n", pOrig);
	uiret = ez_crc32((unsigned char *)pOrig, strlen(pOrig));
	printf("ez_crc32:0x%0X\n", uiret);
	printf("this crc32 alo is:%s\n", uiret==0x5CFCCFF6?"OK":"NOK"); 
*/
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
