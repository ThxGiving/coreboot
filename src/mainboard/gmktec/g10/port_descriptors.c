/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <types.h>

/*
 * TODO(G10): the logical-lane ranges and clk_req lines below are a best-effort
 * mapping from the live PCI topology (lspci):
 *   NVMe  MAP1202   @ 00:01.5  x4   (fn 5)
 *   RTL8125 2.5GbE  @ 00:01.3  x1   (fn 3)
 *   RTL8822CE WiFi  @ 00:01.2  x1   (fn 2)
 * The physical lane numbers + CLK_REQ assignments are board routing and MUST
 * be verified against the schematic - wrong lanes mean the device won't train.
 */
static const fsp_dxio_descriptor g10_dxio_descriptors[] = {
	{ /* NVMe SSD (00:01.5, x4) */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 0,
		.end_logical_lane = 3,
		.device_number = 1,
		.function_number = 5,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5,
	},
	{ /* RTL8125 2.5GbE LAN (00:01.3, x1) */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 4,
		.end_logical_lane = 4,
		.device_number = 1,
		.function_number = 3,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1,
	},
	{ /* RTL8822CE WiFi (00:01.2, x1) */
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 5,
		.end_logical_lane = 5,
		.device_number = 1,
		.function_number = 2,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
	},
};

/* TODO(G10): the G10 exposes HDMI + DP + USB-C(DP). Verify aux/hdp indices. */
static const fsp_ddi_descriptor g10_ddi_descriptors[] = {
	{ /* DDI0 - HDMI */
		.connector_type = HDMI,
		.aux_index = AUX1,
		.hdp_index = HDP1,
	},
	{ /* DDI1 - DP */
		.connector_type = DP,
		.aux_index = AUX2,
		.hdp_index = HDP2,
	},
	{ /* DDI2 - DP (USB-C) */
		.connector_type = DP,
		.aux_index = AUX3,
		.hdp_index = HDP3,
	},
};

void mainboard_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					size_t *dxio_num,
					const fsp_ddi_descriptor **ddi_descs,
					size_t *ddi_num)
{
	*dxio_descs = g10_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(g10_dxio_descriptors);
	*ddi_descs = g10_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(g10_ddi_descriptors);
}
