/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 UCTECHIP Corporation.
 *
 * platform.h
 *
 *  Created on: May 19, 2020
 *      Author: viking <wj.yao@uctechip.com>
 */

#ifndef PLATFORM_UCTECHIP_WH64_PLATFORM_H_
#define PLATFORM_UCTECHIP_WH64_PLATFORM_H_

#include <sbi/sbi_const.h>

#define WH_HART_COUNT				2

#define WH_HART_STACK_SIZE			8192

#define WH_SYS_CLK				50000000

#define WH_CLINT_ADDR				0x32000000

#define WH_PLIC_ADDR				0xc0000000
#define WH_PLIC_NUM_SOURCES			15
#define WH_PLIC_NUM_PRIORITIES			7

#define WH_UART_BAUDRATE			115200

#endif /* PLATFORM_UCTECHIP_WH64_PLATFORM_H_ */
