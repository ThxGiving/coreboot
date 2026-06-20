/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

/*
 * CPU package power-limit overrides, in watts; 0 keeps the SoC/FSP default.
 * Read at runtime by src/soc/intel/common/block/power_limit/power_limit.c via
 * get_uint_option(); requires an option backend (USE_UEFI_VARIABLE_STORE).
 * Lowering PL1 also smooths current transients (di/dt).
 */
static const struct sm_object cpu_pl1 = SM_DECLARE_ENUM({
	.opt_name	= "tdp_pl1_override",
	.ui_name	= "CPU PL1 sustained power",
	.ui_helptext	= "Sustained package power limit in watts. Lower runs cooler and "
			  "smooths current spikes; higher allows more sustained turbo. "
			  "Default keeps the SoC value.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Default",	0  },
				{ "6 W",	6  },
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
	.ui_helptext	= "Short-term burst package power limit in watts. Default keeps the "
			  "SoC value. Automatically clamped to be at least PL1.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Default",	0  },
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
 * What the framebuffer console shows during boot. Read by the generic
 * framebuffer console via get_uint_option("fb_console", ...). The boot log is
 * always still available via cbmem -c and the serial console regardless.
 */
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
		&boot_beep,
		&fb_console,
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
