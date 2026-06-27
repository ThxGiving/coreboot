/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

/*
 * CPU package power-limit overrides, in watts. Read at runtime by
 * src/soc/intel/common/block/power_limit/power_limit.c via get_uint_option(),
 * with the devicetree values (15 W PL1 / 20 W PL2) as the fallback. So when the
 * option is left unset the board runs at 15/20 W; picking a wattage overrides
 * it; picking "SoC default" (0) reverts to the Alder Lake-N silicon TDP (~6 W
 * PL1, ~7.5 W PL2). Requires an option backend (USE_UEFI_VARIABLE_STORE).
 * Lowering PL1 also smooths current transients (di/dt).
 */
static const struct sm_object cpu_pl1 = SM_DECLARE_ENUM({
	.opt_name	= "tdp_pl1_override",
	.ui_name	= "CPU PL1 sustained power",
	.ui_helptext	= "Sustained package power limit in watts. This board's firmware "
			  "default is 15 W - raised above the 6 W Alder Lake-N silicon "
			  "minimum for better sustained performance. Lower runs cooler "
			  "and quieter; higher allows more sustained turbo. \"SoC "
			  "default\" reverts to the 6 W silicon value.",
	.default_value	= 15,
	.values		= (const struct sm_enum_value[]) {
				{ "SoC default (6 W)",	0  },
				{ "10 W",	10 },
				{ "12 W",	12 },
				{ "15 W",	15 },
				{ "20 W",	20 },
				{ "25 W",	25 },
				SM_ENUM_VALUE_END		},
});

static const struct sm_object cpu_pl2 = SM_DECLARE_ENUM({
	.opt_name	= "tdp_pl2_override",
	.ui_name	= "CPU PL2 burst power",
	.ui_helptext	= "Short-term burst package power limit in watts. This board's "
			  "firmware default is 20 W. \"SoC default\" reverts to about "
			  "1.25x the silicon TDP (~7.5 W). Automatically clamped to be "
			  "at least PL1.",
	.default_value	= 20,
	.values		= (const struct sm_enum_value[]) {
				{ "SoC default (7.5 W)",	0  },
				{ "10 W",	10 },
				{ "15 W",	15 },
				{ "20 W",	20 },
				{ "25 W",	25 },
				{ "30 W",	30 },
				{ "35 W",	35 },
				SM_ENUM_VALUE_END		},
});

/*
 * Runtime on/off for the front-panel "boot OK" chime. Read by beep.c via
 * get_uint_option("boot_beep", ...); requires an option backend
 * (USE_UEFI_VARIABLE_STORE). Build-time default is CONFIG_BEEP_ON_BOOT.
 */
static const struct sm_object boot_beep = SM_DECLARE_BOOL({
	.opt_name	= "boot_beep",
	.ui_name	= "Boot beep",
	.ui_helptext	= "Play a short ascending chime over the front-panel buzzer when "
			  "boot completes. Useful as a headless \"all OK\" signal.",
	.default_value	= true,
});

/*
 * What the framebuffer console shows during boot - only when the separate,
 * optional generic framebuffer console feature is built in (CONSOLE_FRAMEBUFFER,
 * github.com/ThxGiving/coreboot-framebuffer-console). The board does not depend
 * on it; this option only appears when it is enabled. Read by that module via
 * get_uint_option("fb_console", ...). The boot log is always available via
 * cbmem -c and the serial console regardless.
 */
#if CONFIG(CONSOLE_FRAMEBUFFER)
static const struct sm_object fb_console = SM_DECLARE_ENUM({
	.opt_name	= "fb_console",
	.ui_name	= "Boot screen",
	.ui_helptext	= "What coreboot draws on the display while booting. Progress "
			  "shows a clean bar with the current phase (over the splash, "
			  "if any); Console log shows the full boot log; Off draws "
			  "nothing. The log stays available via cbmem and serial.",
	.default_value	= 2,
	.values		= (const struct sm_enum_value[]) {
				{ "Off",		0 },
				{ "Console log",	1 },
				{ "Progress bar",	2 },
				SM_ENUM_VALUE_END		},
});
#endif

/*
 * Per-port Wake-on-LAN for the four discrete Intel i226-V NICs (PCIe root
 * ports 1c.0/1c.3/1c.6/1d.0 = ACPI RP01/RP04/RP07/RP09). Read at runtime by
 * mainboard_fill_ssdt() in mainboard.c, which emits the ACPI _PRW wake source
 * for each enabled port. With _PRW present the OS can arm a Wake-on-LAN magic
 * packet to resume from S5/sleep; the NIC's own WoL (ethtool wol g, the igc
 * default) must also be on. Requires an option backend (USE_UEFI_VARIABLE_STORE).
 */
static const struct sm_object wol_lan1 = SM_DECLARE_BOOL({
	.opt_name	= "wol_lan1",
	.ui_name	= "Wake on LAN - LAN1 (1c.0)",
	.ui_helptext	= "Allow a Wake-on-LAN magic packet on LAN1 (the i226 NIC at "
			  "PCIe 1c.0) to resume the system from soft-off (S5) or sleep.",
	.default_value	= true,
});

static const struct sm_object wol_lan2 = SM_DECLARE_BOOL({
	.opt_name	= "wol_lan2",
	.ui_name	= "Wake on LAN - LAN2 (1c.3)",
	.ui_helptext	= "Allow a Wake-on-LAN magic packet on LAN2 (the i226 NIC at "
			  "PCIe 1c.3) to resume the system from soft-off (S5) or sleep.",
	.default_value	= true,
});

static const struct sm_object wol_lan3 = SM_DECLARE_BOOL({
	.opt_name	= "wol_lan3",
	.ui_name	= "Wake on LAN - LAN3 (1c.6)",
	.ui_helptext	= "Allow a Wake-on-LAN magic packet on LAN3 (the i226 NIC at "
			  "PCIe 1c.6) to resume the system from soft-off (S5) or sleep.",
	.default_value	= true,
});

static const struct sm_object wol_lan4 = SM_DECLARE_BOOL({
	.opt_name	= "wol_lan4",
	.ui_name	= "Wake on LAN - LAN4 (1d.0)",
	.ui_helptext	= "Allow a Wake-on-LAN magic packet on LAN4 (the i226 NIC at "
			  "PCIe 1d.0) to resume the system from soft-off (S5) or sleep.",
	.default_value	= true,
});

/*
 * SaGv (System Agent Geyserville) - dynamic DRAM frequency/voltage switching.
 * Read at runtime by romstage_fsp_params.c (mainboard_memory_init_params) via
 * get_uint_option("sagv", ...), overriding the devicetree value. The board
 * default is Fixed Point 3: the top frequency point pinned, full bandwidth with
 * NO runtime transitions - chosen for stability on marginal RAM (random
 * corruption/segfaults were seen with full dynamic switching). "Enabled" allows
 * all switching; "Disabled" forces the lowest point. Requires an option backend.
 * Values match the SoC enum (SaGv_Disabled=0 .. SaGv_FixedPoint3=4, Enabled=5).
 */
static const struct sm_object sagv = SM_DECLARE_ENUM({
	.opt_name	= "sagv",
	.ui_name	= "SaGv (DRAM frequency switching)",
	.ui_helptext	= "System Agent Geyserville. Fixed Point 3 (default) pins the top "
			  "DRAM frequency: full bandwidth, no runtime transitions - the "
			  "stable choice on marginal RAM. Enabled allows dynamic switching; "
			  "Disabled forces the lowest point. If you see random instability, "
			  "keep Fixed Point 3 (or try Disabled for maximum margin).",
	.default_value	= 4,	/* SaGv_FixedPoint3 */
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled (lowest point)",	0 },
				{ "Fixed Point 0 (low)",	1 },
				{ "Fixed Point 1",		2 },
				{ "Fixed Point 2",		3 },
				{ "Fixed Point 3 (top, default)", 4 },
				{ "Enabled (dynamic switching)", 5 },
				SM_ENUM_VALUE_END		},
});

/*
 * Wake on USB. Whether USB device activity / remote-wakeup can resume the
 * system from sleep (S3). Sets the per-port USB2/USB3 wake-enable bitmaps,
 * applied at runtime by mainboard_init() in mainboard.c (-> GNVS u2we/u3we ->
 * the XHCI UWES ACPI method). Default on (all ports).
 */
static const struct sm_object wake_on_usb = SM_DECLARE_BOOL({
	.opt_name	= "wake_on_usb",
	.ui_name	= "Wake on USB",
	.ui_helptext	= "Allow a USB device (e.g. a keyboard's remote wakeup) to resume "
			  "the system from sleep. Sets the USB2/USB3 port wake-enable "
			  "bitmaps. Disable to ignore USB wake events.",
	.default_value	= true,
});

/*
 * CPU-fan profile. Picks one of three IT8625E SmartGuardian curves for the
 * single CPU_FAN header (FAN1 / pwm1). Read at runtime by mainboard_init() in
 * mainboard.c, which patches the IT8625E FAN1.smart config (off/start/full
 * temperature, start duty and slope) before the env_ctrl driver programs the
 * chip. The devicetree carries the "Normal" curve as the baseline; this option
 * overrides it. Requires an option backend (USE_UEFI_VARIABLE_STORE).
 *   Silent      - fan stays off longest, ramps gently (quietest, runs warmer)
 *   Normal      - the tuned default (off 45C, gentle to full at 85C)
 *   Performance - fan on early and stronger (coolest, audible sooner)
 */
static const struct sm_object fan_profile = SM_DECLARE_ENUM({
	.opt_name	= "fan_profile",
	.ui_name	= "CPU fan profile",
	.ui_helptext	= "SmartGuardian curve for the CPU fan. Silent keeps the fan off "
			  "longest and ramps gently (quietest, lets the CPU run warmer); "
			  "Normal is the tuned default; Performance spins the fan up "
			  "earlier and harder (coolest, but audible sooner). The N100 is "
			  "fanless-cool at idle, so Normal is quiet already.",
	.default_value	= 1,	/* Normal */
	.values		= (const struct sm_enum_value[]) {
				{ "Silent",		0 },
				{ "Normal (default)",	1 },
				{ "Performance",	2 },
				SM_ENUM_VALUE_END		},
});

/*
 * Firmware boot watchdog. Arms the IT8625E hardware watchdog late in coreboot
 * (just before the payload is launched) with the selected timeout. If the
 * payload, boot loader or OS hangs before the OS watchdog daemon takes over
 * (Proxmox watchdog-mux opening /dev/watchdog0 = it87_wdt), the IT8625E resets
 * the board (KRST), so a wedged headless box self-recovers. Once the OS driver
 * opens the watchdog it re-arms with its own timeout and keeps petting, so this
 * only covers the pre-OS-daemon window. Default Off (opt in deliberately - too
 * short a timeout against a slow boot would reset-loop). Read at runtime by the
 * BS_PAYLOAD_BOOT hook in mainboard.c. Requires an option backend.
 */
static const struct sm_object fw_watchdog = SM_DECLARE_ENUM({
	.opt_name	= "fw_watchdog",
	.ui_name	= "Firmware boot watchdog",
	.ui_helptext	= "Arm the IT8625E hardware watchdog before launching the payload. "
			  "If the boot or OS hangs before the OS watchdog (it87_wdt / "
			  "Proxmox watchdog-mux) takes over, the board hard-resets and "
			  "retries. Pick a timeout comfortably longer than your normal "
			  "boot-to-watchdog-daemon time. Off disables it (the OS watchdog "
			  "still works independently).",
	.default_value	= 0,	/* Off */
	.values		= (const struct sm_enum_value[]) {
				{ "Off",	0   },
				{ "60 s",	60  },
				{ "120 s",	120 },
				{ "180 s",	180 },
				{ "300 s",	300 },
				SM_ENUM_VALUE_END		},
});

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&igd_enabled,
		&igd_dvmt,
		&igd_aperture,
		&legacy_8254_timer,
		&me_state,
		&me_state_counter,
		&vtd,
		&s0ix_enable,
		&sagv,
		&boot_beep,
#if CONFIG(CONSOLE_FRAMEBUFFER)
		&fb_console,
#endif
		NULL
	},
};

static struct sm_obj_form pcie = {
	.ui_name = "PCIe",
	.obj_list = (const struct sm_object *[]) {
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		&pciexp_speed,
		NULL
	},
};

static struct sm_obj_form security = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]) {
		&intel_tme,
		&bios_lock,
		&disable_heci1_at_pre_boot,
		&pkg_power_limit_lock,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&cpu_pl1,
		&cpu_pl2,
		&power_on_after_fail,
		&wol_lan1,
		&wol_lan2,
		&wol_lan3,
		&wol_lan4,
		&wake_on_usb,
		&fan_profile,
		&fw_watchdog,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&pcie,
	&security,
	&power,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
