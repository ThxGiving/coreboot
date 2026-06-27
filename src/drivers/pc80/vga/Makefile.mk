## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_ARCH_X86),y)

romstage-$(CONFIG_ROMSTAGE_VGA) += vga_io.c
romstage-$(CONFIG_ROMSTAGE_VGA) += vga_palette.c
romstage-$(CONFIG_ROMSTAGE_VGA) += vga_font_8x16.c
romstage-$(CONFIG_ROMSTAGE_VGA) += vga.c

ramstage-$(CONFIG_VGA) += vga_io.c
ramstage-$(CONFIG_VGA) += vga_palette.c
ramstage-$(CONFIG_VGA) += vga_font_8x16.c
ramstage-$(CONFIG_VGA) += vga.c

# The framebuffer console reuses this 8x16 font (sorted/deduped by the build).
ramstage-$(CONFIG_CONSOLE_FRAMEBUFFER) += vga_font_8x16.c

endif
