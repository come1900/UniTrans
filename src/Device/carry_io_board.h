/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * carry_io_board.h - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: carry_io_board.h 5884 2013-01-20 08:17:01Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-20 08:17:01  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef _CARRY_IO_BOARD_H
#define _CARRY_IO_BOARD_H


#define CARRY_BUF_LEN 8

#ifdef __cplusplus
extern "C"
{
#endif

    unsigned char carry_make_sum(unsigned char *p_array, int array_len);
    int carry_make_cmd(unsigned char *parray,
                       int array_len,
                       unsigned char head,
                       unsigned char addr,
                       unsigned char cmd,
                       unsigned int data);

    //单路开关
    int carry_makebuf_set_out(unsigned char *parray /*缓存*/
                              , int array_len /*缓存长度*/
                              , int addr /*板子地址*/
                              , int ch /*通道 >=1*/
                              , int status /*开1关0*/);

    void carry_show_array(unsigned char *p_array, int array_len);


#ifdef __cplusplus
}
#endif

#endif //_CARRY_IO_BOARD_H
