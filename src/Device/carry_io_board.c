/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * carry_io_board.cpp - _explain_
 *
 * Copyright (C) 2011 tiansu-china.com, All Rights Reserved.
 *
 * $Id: carry_io_board.cpp 5884 2013-01-20 08:18:17Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-01-20 08:18:17  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "carry_io_board.h"

int carry_makebuf_set_out(unsigned char *parray, int array_len, int addr, int ch, int status)
{
    if (status==1)
    {
        return carry_make_cmd(parray, array_len, 0x55, addr, 0x12, ch);
    }
    else if (status==0)// close
    {
        return carry_make_cmd(parray, array_len, 0x55, addr, 0x11, ch);
    }


    return -1;

}

int carry_make_cmd(unsigned char *parray,
                   int array_len,
                   unsigned char head,
                   unsigned char addr,
                   unsigned char cmd,
                   unsigned int data)
{
    if (array_len!=CARRY_BUF_LEN || parray==NULL)
    {
        return -1;
    }

    parray[0] = head ;// head
    parray[1] = addr;// board
    parray[2] = cmd; //cmd
    parray[3] = 0xf&(data>>3); //data
    parray[4] = 0xf&(data>>2);
    parray[5] = 0xf&(data>>1);
    parray[6] = 0xf&(data>>0);

    parray[7] = carry_make_sum(parray, 7);

    return 0;

}

unsigned char carry_make_sum(unsigned char *p_array, int array_len)
{
    unsigned char carry_sum;
    int ii;

    if (p_array==NULL || array_len<1)
    {
        return 0;
    }

    carry_sum = 0;
    for(ii=0;ii<array_len;ii++)
    {
        carry_sum+=p_array[ii];
    }

    return carry_sum;
}

void carry_show_array(unsigned char *p_array, int array_len)
{
    if (p_array==NULL || array_len<1)
    {
        return;
    }

    int ii;

    for(ii=0;ii<array_len;ii++)
    {
        //printf("0x%0x ", p_array[ii]);
        printf("%02x ", p_array[ii]);

    }
    printf("\n");

}

