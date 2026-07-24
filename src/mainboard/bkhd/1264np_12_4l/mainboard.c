/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <acpi/acpigen.h>
#include <option.h>
#include <soc/gpe.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <static.h>
#include <superio/ite/it8625e/it8625e.h>
#include <superio/ite/it8625e/chip.h>
#include "gpio.h"
#include "beep.h"

/*
 * Program the IT8625E EC like stock: edge-trigger the temp ADC (0x0c=0x30),
 * enable the EC PECI host (EXTEMP GetTemp 0x89-0x8e) and route TMPIN1 (CPU)
 * to PECI via bank-2 reg 0x21d=0x04. env_ctrl does none of this, so without
 * it TMPIN1 reads -128/frozen. EC HWM base 0xa30, index/data = base+5/base+6.
 */
#define EC_HWM_BASE	0xa30
static void it8625e_start_temp_adc(void)
{
	const u16 idx = EC_HWM_BASE + 5;	/* 0xa35 - HWM index */
	const u16 data = EC_HWM_BASE + 6;	/* 0xa36 - HWM data  */
	static const u8 init[][2] = {
		{ 0x06, 0x02 }, { 0x0a, 0x64 }, { 0x0b, 0x08 }, { 0x0c, 0x30 },
		/* EC PECI host (EXTEMP GetTemp) - without this the PECI read never
		   runs and TMPIN1 is frozen instead of reading the die temp. */
		{ 0x89, 0x30 }, { 0x8a, 0x01 }, { 0x8b, 0x02 }, { 0x8c, 0x01 },
		{ 0x8e, 0xe0 },
	};

	for (size_t i = 0; i < ARRAY_SIZE(init); i++) {
		outb(init[i][0], idx);
		outb(init[i][1], data);
	}

	/* TMPIN1 (CPU) source: bank-2 register 0x21d = 0x04 (PECI, like stock). */
	outb(0x06, idx); outb(0x42, data);	/* select bank 2 (2 << 5 | 0x02) */
	outb(0x1d, idx); outb(0x04, data);	/* register 0x1d = 0x04          */
	outb(0x06, idx); outb(0x02, data);	/* back to bank 0                */
}

/*
 * CPU-fan profile (CFR "fan_profile", default Normal). Patch the IT8625E FAN1
 * (CPU_FAN, J2G1) SmartGuardian curve before the IT8625E .init reads it.
 */
static void bkhd_apply_fan_profile(void)
{
	const struct device *ec = dev_find_slot_pnp(0x2e, IT8625E_EC);
	if (!ec || !ec->chip_info)
		return;
	struct superio_ite_it8625e_config *sio = ec->chip_info;
	struct ite_ec_fan_smartconfig *s = &sio->ec.fan[0].smart;

	/* { tmp_off, tmp_start, tmp_full, tmp_delta, pwm_start, slope } */
	static const struct { u8 off, start, full, delta, pwm, slope; } prof[] = {
		[0] = { 50, 58, 90, 4, 25,  6 },	/* Silent      */
		[1] = { 45, 52, 85, 3, 30,  8 },	/* Normal      */
		[2] = { 40, 45, 80, 2, 40, 12 },	/* Performance */
	};
	unsigned int p = get_uint_option("fan_profile", 1);
	if (p >= ARRAY_SIZE(prof))
		p = 1;

	sio->ec.fan[0].mode = FAN_SMART_AUTOMATIC;
	s->tmpin     = 1;	/* regulate by CPU temp (TMPIN1) */
	s->tmp_off   = prof[p].off;
	s->tmp_start = prof[p].start;
	s->tmp_full  = prof[p].full;
	s->tmp_delta = prof[p].delta;
	s->pwm_start = prof[p].pwm;
	s->slope     = prof[p].slope;
	printk(BIOS_INFO, "bkhd: CPU fan profile %u (off %u/start %u/full %u C)\n",
	       p, s->tmp_off, s->tmp_start, s->tmp_full);
}

static void mainboard_init(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	/* Wake on USB (CFR "wake_on_usb", default on): set the per-port wake
	   bitmaps (devicetree leaves 0); SoC copies them to GNVS u2we/u3we. */
	config_t *cfg = (config_t *)config_of_soc();
	const uint16_t usb_wake = get_uint_option("wake_on_usb", 1) ? 0xffff : 0;
	cfg->usb2_wake_enable_bitmap = usb_wake;
	cfg->usb3_wake_enable_bitmap = usb_wake;

	bkhd_apply_fan_profile();
}

static void mainboard_final(void *chip_info)
{
	/* env_ctrl has configured the EC by now; start the temp-diode ADC. */
	it8625e_start_temp_adc();

	/* Signal a successful boot with the front-panel chime (if enabled). */
	mainboard_boot_beep();
}

/*
 * Per-port Wake-on-LAN (CFR "wol_lanX", default on): emit the PCIe-PME _PRW
 * on each i226 NIC root port (RP01/04/07/09). Done here, not in dsdt.asl, so
 * the CFR toggle can drop it per port. The NIC's own WoL must also be on.
 */
static void mainboard_fill_ssdt(const struct device *dev)
{
	static const struct {
		const char *scope;
		const char *opt;
	} wol_ports[] = {
		{ "\\_SB.PCI0.RP01", "wol_lan1" },	/* 00:1c.0 - LAN1 */
		{ "\\_SB.PCI0.RP04", "wol_lan2" },	/* 00:1c.3 - LAN2 */
		{ "\\_SB.PCI0.RP07", "wol_lan3" },	/* 00:1c.6 - LAN3 */
		{ "\\_SB.PCI0.RP09", "wol_lan4" },	/* 00:1d.0 - LAN4 */
	};

	for (size_t i = 0; i < ARRAY_SIZE(wol_ports); i++) {
		if (!get_uint_option(wol_ports[i].opt, 1))
			continue;
		acpigen_write_scope(wol_ports[i].scope);
		acpigen_write_name_integer("_S0W", 3);
		acpigen_write_PRW(GPE0_PME_B0, 4);
		acpigen_write_method("_DSW", 3);
		acpigen_write_method_end();
		acpigen_write_scope_end();
	}
}

/*
 * Firmware boot watchdog (CFR "fw_watchdog", default Off, seconds). Arm the
 * IT8625E HW watchdog at BS_PAYLOAD_BOOT; if the payload/OS hangs before Linux
 * it87_wdt opens /dev/watchdog0, it expires and hard-resets the board (KRST).
 * WDT registers live in the SuperIO GPIO LDN (see writes below).
 */
#define SIO_IDX	0x2e	/* SuperIO config index port */
#define SIO_DAT	0x2f	/* SuperIO config data port  */

static void sio_wdt_write(u8 reg, u8 val)
{
	outb(reg, SIO_IDX);
	outb(val, SIO_DAT);
}

static void bkhd_arm_fw_watchdog(void *unused)
{
	unsigned int secs = get_uint_option("fw_watchdog", 0);
	if (!secs)
		return;
	if (secs > 0xffff)
		secs = 0xffff;

	/* Enter the IT8625E config state (the ITE magic key sequence on 0x2e). */
	outb(0x87, SIO_IDX);
	outb(0x01, SIO_IDX);
	outb(0x55, SIO_IDX);
	outb(0x55, SIO_IDX);			/* 0x2e port uses 0x55 here */

	sio_wdt_write(0x07, IT8625E_GPIO);	/* select GPIO LDN (holds WDT) */
	sio_wdt_write(0x71, 0x00);		/* WDTCTRL: timer only, no IRQ src */
	sio_wdt_write(0x72, 0xC0);		/* WDTCFG: seconds (TOV1) + KRST  */
	sio_wdt_write(0x74, (secs >> 8) & 0xff);	/* timeout MSB */
	sio_wdt_write(0x73, secs & 0xff);		/* timeout LSB -> load+start */

	sio_wdt_write(0x02, 0x02);		/* exit config state (CC reg) */

	printk(BIOS_INFO, "bkhd: armed IT8625E firmware watchdog, %u s to reset\n",
	       secs);
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, bkhd_arm_fw_watchdog, NULL);

static void mainboard_enable(struct device *dev)
{
	if (dev->ops)
		dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.final = mainboard_final,
	.enable_dev = mainboard_enable,
};

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/* Disabling DMI ASPM, fixes wonky NVME */
	params->PchLegacyIoLowLatency = 1;

	/* CDCLK = 192 MHz (UPD 0). FSP auto picks 652.8 MHz (~3.4x what i915 uses)
	   which over-drives the HDMI data path -> scrambled scanout. Match i915. */
	params->CdClock = 0;
}
