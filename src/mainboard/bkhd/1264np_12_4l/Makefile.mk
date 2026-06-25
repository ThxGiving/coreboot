/* SPDX-License-Identifier: GPL-2.0-or-later */

bootblock-y += bootblock.c

romstage-y += romstage_fsp_params.c

ramstage-y += mainboard.c
ramstage-y += beep.c
ramstage-$(CONFIG_TCSS_HAS_USBC_OPS) += usbc_ops.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
