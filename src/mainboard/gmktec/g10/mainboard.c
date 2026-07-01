/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <device/device.h>
#include <amdblocks/acpi.h>
#include <types.h>
#include "gpio.h"

/* TODO: recheck IRQ tables for the G10. (Copied Picasso-generic from amd/mandolin.) */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	8,		16 },
	{ PIRQ_B,	10,		17 },
	{ PIRQ_C,	11,		18 },
	{ PIRQ_D,	12,		19 },
	{ PIRQ_SCI,	ACPI_SCI_IRQ,	ACPI_SCI_IRQ },
	{ PIRQ_SATA,	PIRQ_NC,	19 },
	{ PIRQ_GPIO,	 7,		 7 },
	{ PIRQ_I2C2,	 6,		 6 },
	{ PIRQ_I2C3,	14,		14 },
	{ PIRQ_UART0,	 4,		 4 },
	{ PIRQ_UART1,	 3,		 3 },
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void mainboard_init(void *chip_info)
{
	mainboard_program_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
