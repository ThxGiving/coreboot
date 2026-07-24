/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <console/console.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>
#include <superio/ite/it8625e/it8625e.h>

#define UART_DEV PNP_DEV(0x2e, IT8625E_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8625E_GPIO)

/*
 * User GPIO header (GPIO1..8) -> IT8625E Simple I/O pins GP22/23/36/37/56/57/
 * 60/61. Set as inputs (safe default); the OS gpio-it87 driver (Simple I/O
 * base 0xa00, LDN 0x07) reconfigures them at runtime.
 */
static const u8 user_gpios[] = { 22, 23, 36, 37, 56, 57, 60, 61 };

void bootblock_mainboard_early_init(void)
{
	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);

	for (unsigned int i = 0; i < ARRAY_SIZE(user_gpios); i++)
		ite_gpio_setup(GPIO_DEV, user_gpios[i], ITE_GPIO_INPUT,
			       ITE_GPIO_SIMPLE_IO_MODE, ITE_GPIO_CONTROL_DEFAULT);
}
