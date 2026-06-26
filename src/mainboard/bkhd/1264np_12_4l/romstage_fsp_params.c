/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fsp/api.h>
#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	static const struct mb_cfg ddr5_mem_config = {
		.UserBd = BOARD_TYPE_MOBILE,
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.LpDdrDqDqsReTraining = 1,
	};

	const struct mem_spd dimm_module_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
		},
	};

	const bool half_populated = false;

	memcfg_init(memupd, &ddr5_mem_config, &dimm_module_spd_info, half_populated);

	/*
	 * CFR override for SaGv. This runs after the SoC has set m_cfg->SaGv from
	 * the devicetree (soc/.../romstage/fsp_params.c calls us last), so this
	 * wins. Default = SaGv_FixedPoint3 (4) - the top frequency point pinned,
	 * chosen for stability on marginal RAM. See cfr.c "sagv".
	 */
	memupd->FspmConfig.SaGv = get_uint_option("sagv", 4 /* SaGv_FixedPoint3 */);
}
