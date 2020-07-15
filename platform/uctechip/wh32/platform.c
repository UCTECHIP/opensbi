/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/sbi_hart.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>

/*
 * Include these files as needed.
 * See config.mk PLATFORM_xxx configuration parameters.
 */
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/uctechip-uart.h>
#include <sbi_utils/sys/clint.h>

#include "platform.h"


static int wh_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;
	fdt = sbi_scratch_thishart_arg1_ptr();
        
	plic_fdt_fixup(fdt, "riscv,plic0");

	return 0;
}

/* Get number of PMP regions for given HART. */
static u32 wh_pmp_region_count(u32 hartid)
{
        return 1;
}

/*
 * Get PMP regions details (namely: protection, base address, and size) for
 * a given HART.
 */
static int wh_pmp_region_info(u32 hartid, u32 index, ulong *prot,
                                 ulong *addr, ulong *log2size)
{
        int ret = 0;

        switch (index) {
        case 0:
                *prot     = PMP_R | PMP_W | PMP_X;
                *addr     = 0;
                *log2size = __riscv_xlen;
                break;
        default:
                ret = -1;
                break;
        };

        return ret;
}


static int wh_console_init(void)
{
	uart_init(WH_SYS_CLK, WH_UART_BAUDRATE);
	return 0;
}

static int wh_irqchip_init(bool cold_boot)
{
	u32 hartid = sbi_current_hartid();
	int ret;

	if (cold_boot) {
		ret = plic_cold_irqchip_init(WH_PLIC_ADDR,
					     WH_PLIC_NUM_SOURCES,
					     WH_HART_COUNT);
		if (ret)
			return ret;
	}

	return plic_warm_irqchip_init(hartid, 2 * hartid, 2 * hartid + 1);
}

static int wh_ipi_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = clint_cold_ipi_init(WH_CLINT_ADDR,
					  WH_HART_COUNT);
		if (ret)
			return ret;
	}

	return clint_warm_ipi_init();
}

static void wh_ipi_send(u32 target_hart)
{
	clint_ipi_send(target_hart);
}

static void wh_ipi_clear(u32 target_hart)
{
	clint_ipi_clear(target_hart);
}

static int wh_timer_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = clint_cold_timer_init(WH_CLINT_ADDR,
					    WH_HART_COUNT, TRUE);
		if (ret)
			return ret;
	}

	return clint_warm_timer_init();
}

static u64 wh_timer_value(void)
{
	return clint_timer_value();
}

static void wh_timer_event_start(u64 next_event)
{
	clint_timer_event_start(next_event);
}

static void wh_timer_event_stop(void)
{
	clint_timer_event_stop();
}

static int wh_system_down(u32 type)
{
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.final_init		= wh_final_init,

	.pmp_region_count = wh_pmp_region_count,
        .pmp_region_info  = wh_pmp_region_info,

	.console_putc		= uart_putchar,
	.console_getc		= uart_getchar,
	.console_init		= wh_console_init,

	.irqchip_init		= wh_irqchip_init,

	.ipi_send		= wh_ipi_send,
	.ipi_clear		= wh_ipi_clear,
	.ipi_init		= wh_ipi_init,

	.timer_value		= wh_timer_value,
	.timer_event_stop	= wh_timer_event_stop,
	.timer_event_start	= wh_timer_event_start,
	.timer_init		= wh_timer_init,

        .system_reboot          = wh_system_down,
        .system_shutdown        = wh_system_down

};
const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "WH",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= WH_HART_COUNT,
	.hart_stack_size	= WH_HART_STACK_SIZE,
	.disabled_hart_mask 	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
