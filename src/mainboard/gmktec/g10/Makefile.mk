# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += early_gpio.c

ramstage-y += gpio.c
ramstage-y += port_descriptors.c

# OEM AGESA PSP Customization Block extracted from the stock G10 firmware
# (SO-DIMM board -> AGESA reads SPD at runtime, so this is reuse-as-is).
APCB_SOURCES = src/mainboard/gmktec/g10/APCB_g10.bin
