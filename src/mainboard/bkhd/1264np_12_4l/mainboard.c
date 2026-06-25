/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <intelblocks/tcss.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <intelblocks/systemagent.h>
#include <soc/pcr_ids.h>
#include <delay.h>
#include "gpio.h"
#include "beep.h"

/*
 * Force the TCSS DisplayPort connect.
 *
 * The IOM firmware only sets IOM_PORT_STATUS PORT_IS_CONNECTED (and routes the
 * AUX/lanes for DP) when the processor commands it via the PMC IPC TCSS connect
 * - that handshake is coreboot's tcss_configure(), normally driven from the FSP
 * MultiPhaseSiInit callback. Our IoT FSP-S has no multi-phase-si-init entry, so
 * EnableMultiPhaseSiliconInit stays off and tcss_configure() never runs: the
 * fixed DP on TC port 0 is left unconnected and libgfxinit's detect-AUX times
 * out. Drive the same connect ourselves, before the GPU/display device init.
 */
static void bkhd_tcss_dp_connect(void *unused)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS))
		return;

	/* No AUX-bias pads on this board; only the TCSS DP-mode connect matters. */
	static const struct typec_aux_bias_pads pads[MAX_TYPE_C_PORTS] = { 0 };

	/* IOM_PORT_STATUS[0] = REGBAR(PID_IOM) + 0x160; bit31 = PORT_IS_CONNECTED. */
	printk(BIOS_ERR, "FENN IOM_PORT_STATUS[0] BEFORE = 0x%08x\n",
	       REGBAR32(PID_IOM, 0x160));

	printk(BIOS_DEBUG, "BKHD: driving tcss_configure (IoT FSP has no MultiPhaseSiInit)\n");
	tcss_configure(pads);

	/* coreboot does NOT connect the TCSS DP (proven: IOM stays 0x48 even after
	   a 15s wait here). The connect happens later, in the edk2 payload's DXE
	   phase - FennIomWatchDxe instruments it there. Just log the post-configure
	   state; the long wait only delayed the boot and is removed. */
	printk(BIOS_ERR, "FENN IOM_PORT_STATUS[0] AFTER tcss_configure = 0x%08x\n",
	       REGBAR32(PID_IOM, 0x160));
}
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, bkhd_tcss_dp_connect, NULL);

/*
 * FENN bracket read: log the IOM at the very LAST coreboot point (BS_PAYLOAD_BOOT
 * entry, right before jumping into the edk2 payload). Together with the
 * BS_DEV_INIT read (0x48) and the first edk2 image read (0x80000069), this pins
 * down whether coreboot-late or the edk2 payload connects the IOM.
 */
static void bkhd_iom_pre_payload(void *unused)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS))
		return;
	printk(BIOS_ERR, "FENN IOM_PORT_STATUS[0] PRE-PAYLOAD-JUMP = 0x%08x\n",
	       REGBAR32(PID_IOM, 0x160));
}
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, bkhd_iom_pre_payload, NULL);

/*
 * FENN fine trace: the IOM flips 0x48 -> 0x80000069 somewhere between BS_DEV_INIT
 * (0x48) and BS_PAYLOAD_BOOT (connected) - i.e. IN COREBOOT. Read it at every
 * boot state in that window to pin the exact phase (libgfxinit? a .final? a late
 * FSP/CSE step?). The label is passed as the BOOT_STATE arg.
 */
static void bkhd_iom_trace(void *label)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS))
		return;
	printk(BIOS_ERR, "FENN IOM_TRACE @%-18s = 0x%08x\n",
	       (const char *)label, REGBAR32(PID_IOM, 0x160));
}
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT,     BS_ON_EXIT,  bkhd_iom_trace, (void *)"DEV_INIT-EXIT");
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE,  BS_ON_ENTRY, bkhd_iom_trace, (void *)"POST_DEVICE-ENTRY");
BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE,  BS_ON_EXIT,  bkhd_iom_trace, (void *)"POST_DEVICE-EXIT");
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, bkhd_iom_trace, (void *)"WRITE_TABLES-ENTRY");
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, bkhd_iom_trace, (void *)"PAYLOAD_LOAD-ENTRY");
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT,  bkhd_iom_trace, (void *)"PAYLOAD_LOAD-EXIT");

/*
 * FENN: C hook called FROM libgfxinit (Ada, tigerlake/hw-gfx-gma-port_detect.adb)
 * to log IOM_PORT_STATUS[0] at each Type-C setup step, so we see exactly which
 * libgfxinit step (HPD enable / TC_Cold block / probe) connects the DP IOM.
 * Ada side: procedure Fenn_Iom_Trace (Tag : Word32); pragma Import (C, ...).
 */
void fenn_iom_trace(uint32_t tag);
void fenn_iom_trace(uint32_t tag)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS))
		return;
	printk(BIOS_ERR, "FENN libgfx-TC tag=%u IOM=0x%08x\n",
	       tag, REGBAR32(PID_IOM, 0x160));
}

/*
 * FENN: poll IOM PORT_IS_CONNECTED (bit31) for up to timeout_ms (100us steps).
 * Called from libgfxinit at the END of the TC ownership claim - this both
 * MEASURES the uC's async latency (how long after the FIA/ownership writes it
 * sets PORT_IS_CONNECTED) AND, by waiting until connected, lets the subsequent
 * detect-AUX reach the sink. Deterministic poll-until-ready, not a blind delay.
 */
void fenn_iom_wait(uint32_t timeout_ms);
void fenn_iom_wait(uint32_t timeout_ms)
{
	uint32_t i, ps = 0;
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS))
		return;
	for (i = 0; i < timeout_ms * 10; i++) {
		ps = REGBAR32(PID_IOM, 0x160);
		if (ps & (1u << 31))
			break;
		udelay(100);
	}
	printk(BIOS_ERR, "FENN IOM_WAIT: connected=%d after %u.%ums IOM=0x%08x\n",
	       !!(ps & (1u << 31)), i / 10, i % 10, ps);
}

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
