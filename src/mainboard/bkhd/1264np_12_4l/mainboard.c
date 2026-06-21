/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <soc/ramstage.h>
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

static void mainboard_init(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void mainboard_final(void *chip_info)
{
	/* env_ctrl has configured the EC by now; start the temp-diode ADC. */
	it8625e_start_temp_adc();

	/* Signal a successful boot with the front-panel chime (if enabled). */
	mainboard_boot_beep();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.final = mainboard_final,
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
