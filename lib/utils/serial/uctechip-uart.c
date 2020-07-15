/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 UCTECHIP Corporation.
 *
 * uctechip_uart.c
 *
 *  Created on: May 19, 2020
 *      Author: viking <wj.yao@uctechip.com>
 */


#include <sbi_utils/serial/uctechip-uart.h>

#define MMIO_ADDR               0x10000000
#define UART_CTRL_ADDR          MMIO_ADDR + 0x10


#define UART_IS                 0x0   // uart interrupt status register
#define UART_IE                 0x1   // uart interrupt enable register
#define UART_CON                0x2   // uart control register
#define UART_DATA               0x3   // uart data register
#define UART_BPRL               0x4   // uart boud rate register low 8 bit
#define UART_BPRH               0x5   // uart boud rate register high 4 bit
#define UART_ECR                0x5   // ETU counter-value register


// control register
#define UART_CON_FLUSH          (1<<4)  //flush the receive fifo
#define UART_CON_TRS            (1<<3)  //transmit/receive select 0:receive 1:transmit
#define UART_CON_ODD_EN         (1<<1)  //ODD parity enable


// interrupt status register
#define UART_IS_ECNT0           (1<<7)  //ETU timer overflow flag
#define UART_IS_FIFO_NE         (1<<6)  //FIFO Not Empty flag
#define UART_IS_FIFO_OV         (1<<3)  //FIFO overflow error
#define UART_IS_TXEND           (1<<2)  //transmit finish flag
#define UART_IS_TRE             (1<<1)  //transmit or receive parity error flag


#define UART_REG(offset)        (*((volatile char*)(UART_CTRL_ADDR+offset)))


void uart_init(int core_freq, int baud_rate)
{
    int br;
    char br_l,br_h;

    // close the interrupt
    UART_REG(UART_IE) = 0x00;

    // set the baud rate
    //TODO get CORE_FREQ from dtb
    br = core_freq / baud_rate;
    br_l = (char)(br % 256);
    br_h = (char)(br / 256);
    UART_REG(UART_BPRL) = br_l;
    UART_REG(UART_BPRH) = (UART_REG(UART_BPRH) & 0xF0) | br_h;
    UART_REG(UART_CON) &= ~UART_CON_ODD_EN;     // even parity
}



void uart_putchar(char ch)
{
    UART_REG(UART_CON) |= UART_CON_TRS;
    UART_REG(UART_DATA) = ch;

    while(!(UART_REG(UART_IS) & UART_IS_TXEND))
    	;
    UART_REG(UART_IS) &= ~UART_IS_TXEND;

    UART_REG(UART_ECR) = (UART_REG(UART_ECR) & 0x0F) | 0x50;      //set UART_ECR with 3
    while(!(UART_REG(UART_IS) & UART_IS_ECNT0))      //the etu didn't arrive default value(3)
    	;
    UART_REG(UART_IS) &= ~UART_IS_ECNT0;
}



int uart_getchar(void)
{
    UART_REG(UART_CON) &= ~UART_CON_TRS;        //select receive mode

    if(UART_REG(UART_IS) & UART_IS_TRE)
        UART_REG(UART_IS) &= ~UART_IS_TRE;
    if(UART_REG(UART_IS)&UART_IS_FIFO_OV)
    	UART_REG(UART_CON) |= UART_CON_FLUSH;

    if(UART_REG(UART_IS) & UART_IS_FIFO_NE)
    	return UART_REG(UART_DATA);
    return -1;
} 
