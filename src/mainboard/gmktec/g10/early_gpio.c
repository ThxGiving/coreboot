/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"

/*
 * TODO(G10): fill from the live dump:
 *   cat /sys/kernel/debug/gpio  +  work/g10/iomux-dump.py
 *   -> work/g10/gpio2coreboot.py
 * These are only the pads coreboot needs *before* ramstage (console UART, eSPI,
 * essential resets). Stub keeps the build sane.
 */
static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* eSPI CS# (Picasso default) - REVIEW against the G10 dump */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
