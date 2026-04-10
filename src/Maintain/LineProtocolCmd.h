/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * LineProtocolCmd.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: LineProtocolCmd.h 5884 2013-06-08 10:32:41Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-06-08 10:32:41  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _LineProtocolCmd_H
#define _LineProtocolCmd_H

typedef struct __cmd_list_t
{
    const char * pCmd; //
    const char * pCmdComment; //
    void    (*cmd_func)(int, int);

    const char * pAckCmd; //
    const char * pAckCmdComment; //
    void    (*ack_func)(int, int);

}
CmdList_T;


#endif // _LineProtocolCmd_H
