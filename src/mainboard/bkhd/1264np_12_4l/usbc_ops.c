/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Static USB-C/TCSS ops for the board's FIXED DisplayPort wired through the
 * Type-C (Dekel) PHY (DP_TC1, no real USB-C socket, no PD controller).
 *
 * On a normal USB-C board a PD controller / EC negotiates DP-alt-mode and the
 * EC's usbc_ops drive the PMC IPC TCSS connect, which makes the IOM firmware
 * "connect" the port (set PORT_IS_CONNECTED, route AUX/orientation). Our DP is
 * hard-wired, so nothing ever triggers that connect and the TC PHY's AUX never
 * reaches the sink (libgfxinit detect-AUX times out).
 *
 * This provides a static usbc_ops that declares our fixed DP on TC port 0 as
 * always connected, so coreboot's tcss_configure_dp_mode() issues the PMC IPC
 * connect and the IOM configures the port. Values from the live i915 working
 * state: orientation NOT flipped (polarity 0); 4-lane DP (pin assignment C).
 */

#include <types.h>
#include <stdbool.h>
#include <device/usbc_mux.h>
#include <intelblocks/tcss.h>

#define BKHD_DP_TC_PORT		0

static int bkhd_get_mux_info(int port, struct usbc_mux_info *info)
{
	if (port != BKHD_DP_TC_PORT)
		return -1;

	info->dp = true;
	info->usb = false;
	info->cable = false;
	info->polarity = false;		/* orientation not flipped (i915 read) */
	info->hpd_lvl = true;
	info->hpd_irq = false;
	info->ufp = false;
	info->dbg_acc = false;
	info->dp_pin_mode = MODE_DP_PIN_C;	/* 4-lane DP-only */

	return 0;
}

static int bkhd_wait_for_connection(long timeout_ms)
{
	/* Fixed DP on TC port 0 is always "connected". */
	return BIT(BKHD_DP_TC_PORT);
}

static int bkhd_enter_dp_mode(int port)
{
	return 0;
}

static int bkhd_wait_for_dp_mode_entry(int port, long timeout_ms)
{
	return 0;
}

static int bkhd_wait_for_hpd(int port, long timeout_ms)
{
	return 0;
}

static const struct usbc_ops bkhd_usbc_ops = {
	.mux_ops = {
		.get_mux_info = bkhd_get_mux_info,
	},
	.dp_ops = {
		.wait_for_connection = bkhd_wait_for_connection,
		.enter_dp_mode = bkhd_enter_dp_mode,
		.wait_for_dp_mode_entry = bkhd_wait_for_dp_mode_entry,
		.wait_for_hpd = bkhd_wait_for_hpd,
	},
};

const struct usbc_ops *usbc_get_ops(void)
{
	return &bkhd_usbc_ops;
}
