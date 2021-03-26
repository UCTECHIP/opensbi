/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>

#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/uctechip-uart.h>
#include <sbi_utils/sys/clint.h>

#define PLATFORM_PLIC_ADDR		0x3c000000
#define PLATFORM_PLIC_NUM_SOURCES	15
#define PLATFORM_HART_COUNT		2
#define PLATFORM_CLINT_ADDR		0x32000000
#define PLATFORM_UART_INPUT_FREQ	50000000
#define PLATFORM_UART_BAUDRATE		115200

static struct plic_data plic = {
	.addr = PLATFORM_PLIC_ADDR,
	.num_src = PLATFORM_PLIC_NUM_SOURCES,
};

static struct clint_data clint = {
	.addr = PLATFORM_CLINT_ADDR,
	.first_hartid = 0,
	.hart_count = PLATFORM_HART_COUNT,
	.has_64bit_mmio = TRUE,
};

static int platform_early_init(bool cold_boot)
{
	return 0;
}

static int platform_final_init(bool cold_boot)
{
	return 0;
}

static int platform_console_init(void)
{
	uart_init(PLATFORM_UART_INPUT_FREQ,
		PLATFORM_UART_BAUDRATE);
	return 0;
}

static void platform_console_putc(char ch)
{
	uart_putchar(ch);
}

static int platform_console_getc(void)
{
	return uart_getchar();
}

static int platform_irqchip_init(bool cold_boot)
{
	u32 hartid = current_hartid();
	int ret;

	if (cold_boot) {
		ret = plic_cold_irqchip_init(&plic);
		if (ret)
			return ret;
	}

	return plic_warm_irqchip_init(&plic, 2 * hartid, 2 * hartid + 1);
}

static int platform_ipi_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = clint_cold_ipi_init(&clint);
		if (ret)
			return ret;
	}

	return clint_warm_ipi_init();
}

static void platform_ipi_send(u32 target_hart)
{
	clint_ipi_send(target_hart);
}

static void platform_ipi_clear(u32 target_hart)
{
	clint_ipi_clear(target_hart);
}

static int platform_timer_init(bool cold_boot)
{
	int ret;

	if (cold_boot) {
		ret = clint_cold_timer_init(&clint, NULL);
		if (ret)
			return ret;
	}

	return clint_warm_timer_init();
}

static u64 platform_timer_value(void)
{
	return clint_timer_value();
}

static void platform_timer_event_start(u64 next_event)
{
	clint_timer_event_start(next_event);
}

static void platform_timer_event_stop(void)
{
	clint_timer_event_stop();
}

static int platform_system_reset_check(u32 type, u32 reason)
{
	return 0;
}

static void platform_system_reset(u32 type, u32 reason)
{
}

const struct sbi_platform_operations platform_ops = {
	.early_init		= platform_early_init,
	.final_init		= platform_final_init,
	.console_putc		= platform_console_putc,
	.console_getc		= platform_console_getc,
	.console_init		= platform_console_init,
	.irqchip_init		= platform_irqchip_init,
	.ipi_send		= platform_ipi_send,
	.ipi_clear		= platform_ipi_clear,
	.ipi_init		= platform_ipi_init,
	.timer_value		= platform_timer_value,
	.timer_event_stop	= platform_timer_event_stop,
	.timer_event_start	= platform_timer_event_start,
	.timer_init		= platform_timer_init,
	.system_reset_check	= platform_system_reset_check,
	.system_reset		= platform_system_reset
};
const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x00),
	.name			= "wh-processor",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= PLATFORM_HART_COUNT,
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
