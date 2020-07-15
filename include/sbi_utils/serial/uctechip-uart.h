/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 UCTECHIP Corporation.
 *
 * uctechip-uart.h
 *
 *  Created on: May 19, 2020
 *      Author: viking <wj.yao@uctechip.com>
 */

#ifndef INCLUDE_SBI_UTILS_SERIAL_UCTECHIP_UART_H_
#define INCLUDE_SBI_UTILS_SERIAL_UCTECHIP_UART_H_

#include <sbi/sbi_types.h>


void uart_init(int core_freq, int baud_rate);

void uart_putchar(char ch);

int uart_getchar(void);

#endif /* INCLUDE_SBI_UTILS_SERIAL_UCTECHIP_UART_H_ */
