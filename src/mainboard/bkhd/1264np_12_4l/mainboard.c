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
 * Set up IT8625E EC temperature monitoring the way the stock firmware does -
 * three things the common env_ctrl driver does not do for this chip (env_ctrl
 * only configures the PECI host in THERMAL_PECI mode, but stock keeps TMPIN1 in
 * diode mode and routes it to PECI via the new temp-source register instead):
 *
 *  - Register 0x0c bits[5:4] (write 0x30) edge-trigger the temperature ADC.
 *    env_ctrl leaves 0x0c at 0x07, so without this the channels read -128 C.
 *  - The EC PECI host: the EXTEMP "GetTemp" registers 0x89/0x8a/0x8b/0x8c, the
 *    EXTEMP control 0x8e and the PECI interface select 0x0a. Without these the
 *    PECI transaction never runs and TMPIN1 is frozen at a fixed value that
 *    does not track the die at all.
 *  - The bank-2 temperature-source register 0x21d=0x04 (register 0x1d with the
 *    bank bits[7:5] of register 0x06 set to 2) routes TMPIN1 (CPU) to PECI.
 *
 * All verified live against the stock firmware: with all three, TMPIN1 reads
 * the die temperature (~coretemp) and tracks load; dropping the PECI host
 * freezes it, dropping 0x0c gives -128, dropping 0x21d reads a cool remote
 * diode. 0x06/0x0a/0x0b mirror the stock dump. EC HWM base = 0xa30 (devicetree
 * LDN4 io0); index/data ports are base+5 / base+6.
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
 * SmartGuardian curve from the selected preset. This runs in the mainboard
 * chip_ops->init (dev_initialize_chips), which is BEFORE the IT8625E device
 * .init (it8625e_init -> ite_ec_init) reads the same config struct and programs
 * the chip - so the patch lands. fan[0] = FAN1 = the one CPU_FAN header (J2G1).
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

	/*
	 * Wake on USB (CFR "wake_on_usb", default on). Set the per-port USB2/USB3
	 * wake-enable bitmaps now (BS_DEV_INIT) - the SoC copies them into GNVS
	 * u2we/u3we later at ACPI table generation, and the XHCI UWES method arms
	 * the wake from them. The devicetree leaves these at 0; override here.
	 */
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
 * Per-port Wake-on-LAN. Emit the PCIe-PME _PRW wake source on each i226 NIC
 * root port (1c.0/1c.3/1c.6/1d.0 = ACPI RP01/RP04/RP07/RP09) whose CFR
 * "Wake on LAN - LANx" option is enabled (default on, see cfr.c). Generated
 * here rather than statically in dsdt.asl so the CFR toggle can drop the _PRW
 * per port - without _PRW the OS registers no wake source for that NIC and a
 * magic packet cannot resume the box. GPE0_PME_B0 is the shared PCI Express
 * PME GPE (same idiom as soc/.../acpi/pch_glan.asl for the PCH LAN). The NIC's
 * own WoL (ethtool wol g, the igc default) must also be on.
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
 * Firmware boot watchdog (CFR "fw_watchdog", default Off). Arm the IT8625E
 * hardware watchdog just before the payload is launched, with the CFR-selected
 * timeout in seconds. If the payload / boot loader / OS hangs before the OS
 * watchdog daemon (it87_wdt via Proxmox watchdog-mux) opens /dev/watchdog0 and
 * takes over petting, the timer expires and the IT8625E hard-resets the board
 * (KRST) - the same reset action the in-tree it87_wdt driver uses (HW-proven to
 * reset this board). The WDT registers live in the SuperIO GPIO logical device:
 *   0x71 WDTCTRL = 0x00  (timer only; no keyboard/mouse/CIR/GP interrupt source)
 *   0x72 WDTCFG  = 0xC0  (bit7 TOV1 = count in seconds, bit6 KRST = system reset)
 *   0x73/0x74    = timeout value (LSB/MSB); writing it (re)loads and starts.
 * Arming this late (BS_PAYLOAD_BOOT) gives the whole timeout budget to the
 * payload + OS handover rather than spending it on coreboot's own boot.
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

	/*
	 * Cap the package C-state at C6. Deep package states (PC8/PC10) trigger
	 * SYS_PWROK resets on this board; C6 keeps the power saving but avoids
	 * the problematic deep states. Firmware-side, OS-independent. This hook
	 * runs after the SoC default (LIMIT_AUTO), so it cleanly overrides it
	 * without any SoC code changes.
	 */
	params->PkgCStateLimit = LIMIT_C6;

	/*
	 * Force CDCLK to 192 MHz (UPD value 0). FSP auto-selects the maximum
	 * (652.8 MHz) which is ~3.4x what i915 uses for this 1080p HDMI mode and
	 * appears to over-drive the display data path (scrambled, fast-scrolling
	 * scanout). i915 runs this exact board/mode at 192 MHz, so match it.
	 */
	params->CdClock = 0;
}
