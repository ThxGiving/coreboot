/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CONSOLE_FRAMEBUFFER_H_
#define _CONSOLE_FRAMEBUFFER_H_

#include <stdint.h>
#include <types.h>

/*
 * Framebuffer console.
 *
 * Brings the coreboot console log onto the linear framebuffer once graphics
 * have been initialised. The early log (bootblock, romstage, pre-graphics
 * ramstage) is already buffered in the cbmem console; when fbcon_init() runs
 * (after graphics init) it replays the most recent screenful of that log so the
 * screen immediately shows where boot is, and every byte after that is drawn
 * live. A thin boot-progress bar is shown along the bottom edge.
 */

/*
 * Console bitmap font. Glyphs are row-major, one byte per scanline with the
 * most-significant bit leftmost, so the width is limited to 8 pixels; the height
 * is the number of bytes per glyph. 'glyphs' holds 'num_chars' entries starting
 * at character code 'first_char'.
 *
 * fbcon_font() returns the active font; it is __weak and defaults to coreboot's
 * in-tree 8x16 VGA font, so a board (or a future Kconfig-selected provider) can
 * swap in a different console font just by overriding it.
 */
struct fbcon_font {
	uint8_t width;		/* glyph width in pixels, 1..8 */
	uint8_t height;		/* glyph height in pixels = bytes per glyph */
	uint8_t first_char;	/* character code of glyphs[0] */
	uint16_t num_chars;
	const uint8_t *glyphs;	/* num_chars * height bytes */
};

const struct fbcon_font *fbcon_font(void);

/* Latch framebuffer geometry and replay the buffered early log. Called once
   from a BS_POST_DEVICE boot-state hook (after graphics init). */
void fbcon_init(void);

/* Render one byte at the cursor. No-op until fbcon_init() has run. */
void fbcon_tx_byte(unsigned char c);

/* Advance the bottom progress bar to permille (0..1000) of the screen width. */
void fbcon_progress(unsigned int permille);

#if CONFIG(CONSOLE_FRAMEBUFFER) && ENV_RAMSTAGE
static inline void __fbcon_tx_byte(u8 data)	{ fbcon_tx_byte(data); }
#else
static inline void __fbcon_tx_byte(u8 data)	{}
#endif

#endif /* _CONSOLE_FRAMEBUFFER_H_ */
