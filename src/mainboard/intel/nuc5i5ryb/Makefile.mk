# SPDX-License-Identifier: GPL-2.0-or-later

romstage-y += gpio.c
romstage-y += pei_data.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += pei_data.c
ramstage-y += mainboard.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_DRIVERS_OPTION_CFR_ENABLED) += cfr.c
ramstage-$(CONFIG_TPM2_PTT_ACPI_START) += ptt.c
