/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

/* CPU package power-limit overrides (W), read by the SoC power_limit driver;
   devicetree 15/20 W is the fallback, "SoC default" (0) reverts to silicon TDP. */
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

/* Framebuffer-console boot screen; only present when the optional
   CONSOLE_FRAMEBUFFER feature is built in. */
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

/* Per-port Wake-on-LAN for the four i226-V NICs (RP01/04/07/09).
   mainboard_fill_ssdt() emits the ACPI _PRW per enabled port; the NIC's own
   WoL (ethtool wol g) must also be on. */
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
 * SaGv (System Agent Geyserville) DRAM frequency switching, read by
 * romstage_fsp_params.c via get_uint_option("sagv", ...). Default Enabled
 * (dynamic): the earlier instability was a specific DDR5 module brand, not the
 * board, so no fixed point is forced. Enum: Disabled=0 .. FixedPoint3=4,
 * Enabled=5.
 */
static const struct sm_object sagv = SM_DECLARE_ENUM({
	.opt_name	= "sagv",
	.ui_name	= "SaGv (DRAM frequency switching)",
	.ui_helptext	= "System Agent Geyserville. Enabled (default) allows dynamic DRAM "
			  "frequency switching. On marginal RAM you can pin a Fixed Point "
			  "(3 = top frequency, full bandwidth) or force Disabled (lowest "
			  "point) for maximum margin.",
	.default_value	= 5,	/* SaGv_Enabled */
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled (lowest point)",	0 },
				{ "Fixed Point 0 (low)",	1 },
				{ "Fixed Point 1",		2 },
				{ "Fixed Point 2",		3 },
				{ "Fixed Point 3 (top)",	4 },
				{ "Enabled (dynamic, default)",	5 },
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

/* CPU-fan profile: picks an IT8625E SmartGuardian curve for FAN1 (CPU_FAN).
   mainboard_init() patches FAN1.smart from the preset; devicetree "Normal" is
   the baseline. */
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

/* Firmware boot watchdog: arms the IT8625E HW watchdog before the payload
   (mainboard.c BS_PAYLOAD_BOOT hook). Default Off (opt-in; too short a timeout
   reset-loops a slow boot). Covers the window until the OS watchdog takes over. */
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
