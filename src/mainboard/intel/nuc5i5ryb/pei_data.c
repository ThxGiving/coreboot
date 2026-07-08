/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

/*
 * NUC5i5RYB: two DDR3L SO-DIMM slots, one per memory channel.
 * SPD EEPROMs on the SMBus at 0x50 (channel 0) and 0x52 (channel 1).
 */
void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
	spdi->addresses[2] = 0x52;
}

/*
 * USB port configuration - FIRST PASS.
 * All ports enabled with over-current detection skipped, because the real
 * per-port OC-pin routing has not yet been derived from the NUC schematic
 * or a stock capture. This is safe (over-enabling + OC_SKIP does not damage
 * anything) but should be tightened once the actual USB routing is known.
 * TODO: real OC pins, per-port enable, USB2 port lengths.
 */
const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P1 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P2 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P3 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P4 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P5 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P6 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P7 */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* P8 */
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	{ 1, USB_OC_PIN_SKIP, 0 }, /* P1 */
	{ 1, USB_OC_PIN_SKIP, 0 }, /* P2 */
	{ 1, USB_OC_PIN_SKIP, 0 }, /* P3 */
	{ 1, USB_OC_PIN_SKIP, 0 }, /* P4 */
};
