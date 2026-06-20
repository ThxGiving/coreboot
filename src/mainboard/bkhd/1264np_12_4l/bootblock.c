/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <console/console.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/common/ite_gpio.h>
#include <superio/ite/it8625e/it8625e.h>

#define UART_DEV PNP_DEV(0x2e, IT8625E_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8625E_GPIO)

/*
 * User GPIO header (2x5, 2.0 mm pitch). The board manual labels the eight
 * usable pins GPIO1..GPIO8; they map to the IT8625E Simple I/O pins below
 * (verified against the stock BIOS "GPIO for SIO" setup page):
 *
 *   GPIO1=GP22  GPIO2=GP23  GPIO3=GP36  GPIO4=GP37
 *   GPIO5=GP56  GPIO6=GP57  GPIO7=GP60  GPIO8=GP61
 *
 * Route them to Simple I/O mode as inputs (safe power-on default, no
 * contention). Direction and level are then fully controlled at runtime
 * from the OS: the mainline gpio-it87 driver exposes the pins as a gpiochip
 * at Simple I/O base 0xa00 (devicetree LDN 0x07), so they can be
 * reconfigured any time without reflashing.
 */
static const u8 user_gpios[] = { 22, 23, 36, 37, 56, 57, 60, 61 };

void bootblock_mainboard_early_init(void)
{
	ite_enable_serial(UART_DEV, CONFIG_TTYS0_BASE);

	for (unsigned int i = 0; i < ARRAY_SIZE(user_gpios); i++)
		ite_gpio_setup(GPIO_DEV, user_gpios[i], ITE_GPIO_INPUT,
			       ITE_GPIO_SIMPLE_IO_MODE, ITE_GPIO_CONTROL_DEFAULT);
}
