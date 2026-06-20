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
 * ramstage) is already buffered in the cbmem console; fbcon_init() replays it
 * onto the screen the moment the framebuffer is available, so the whole boot
 * is shown - not just the tail. Subsequent characters are drawn live.
 */

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
