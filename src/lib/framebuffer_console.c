/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Framebuffer console - show coreboot's boot progress on the linear
 * framebuffer once graphics are up. See <console/framebuffer.h>.
 *
 * Three runtime modes (board exposes the "fb_console" option via CFR):
 *   0 OFF      - nothing is drawn.
 *   1 LOG      - the full console log, using a compact 8x16 monospace bitmap
 *                font (Terminus). Lines "page": they fill the screen top to
 *                bottom, and once full the screen clears and printing restarts
 *                at the top - O(1) per line, no slow scroll on the
 *                write-combining framebuffer.
 *   2 PROGRESS - a clean centred progress bar with a short phase label drawn
 *                inside it; no log spam.
 */

#include <console/framebuffer.h>
#include <console/cbmem_console.h>
#include <boot/coreboot_tables.h>
#include <commonlib/coreboot_tables.h>
#include <bootstate.h>
#include <option.h>
#include <string.h>
#include <types.h>

#include "fbcon_font.h"		/* fbcon_font[95][FBCON_FH], FBCON_FW, FBCON_FH */

#define CELL_W		FBCON_FW	/* 8 */
#define CELL_H		FBCON_FH	/* 16 */

#define BAR_H		6		/* LOG-mode bottom bar height */
#define BAR_GAP		2

/* Accent / track colours. */
#define BAR_R 0x30
#define BAR_G 0x80
#define BAR_B 0xff
#define TRK_R 0x20
#define TRK_G 0x20
#define TRK_B 0x20

enum fbcon_mode {
	FBM_OFF = 0,
	FBM_LOG = 1,
	FBM_PROGRESS = 2,
};

static struct {
	uint8_t *fb;		/* framebuffer base */
	uint32_t width;
	uint32_t height;
	uint32_t stride;	/* bytes per scanline */
	uint32_t bpp;		/* bytes per pixel */
	bool is32;		/* fast path */
	uint8_t r_pos, g_pos, b_pos;
	uint32_t fg;		/* packed white */
	uint32_t cols, rows;	/* character grid (LOG mode) */
	uint32_t cur_col, cur_row;
	int mode;
	bool ready;
} st;

static inline uint32_t pack(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint32_t)r << st.r_pos) | ((uint32_t)g << st.g_pos) |
	       ((uint32_t)b << st.b_pos);
}

static inline void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
	uint8_t *p = st.fb + (size_t)y * st.stride + (size_t)x * st.bpp;
	if (st.is32) {
		*(uint32_t *)p = color;
		return;
	}
	for (uint32_t i = 0; i < st.bpp; i++)
		p[i] = (color >> (i * 8)) & 0xff;
}

static void fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
	for (uint32_t j = 0; j < h; j++) {
		uint8_t *row = st.fb + (size_t)(y + j) * st.stride + (size_t)x * st.bpp;
		if (st.is32) {
			uint32_t *p = (uint32_t *)row;
			for (uint32_t i = 0; i < w; i++)
				p[i] = color;
		} else {
			for (uint32_t i = 0; i < w; i++)
				for (uint32_t b = 0; b < st.bpp; b++)
					row[i * st.bpp + b] = (color >> (b * 8)) & 0xff;
		}
	}
}

/*
 * Blit one glyph at pixel coordinates. When 'opaque', unset pixels are painted
 * black (so it clears its cell); when not, only set pixels are drawn (so text
 * can sit on top of the progress bar).
 */
static void blit_glyph(unsigned char c, uint32_t px, uint32_t py, bool opaque)
{
	if (c < 32 || c > 126)
		c = ' ';
	const uint8_t *g = fbcon_font[c - 32];

	for (uint32_t y = 0; y < CELL_H; y++) {
		uint8_t bits = g[y];
		for (uint32_t x = 0; x < CELL_W; x++) {
			if (bits & (0x80 >> x))
				put_pixel(px + x, py + y, st.fg);
			else if (opaque)
				put_pixel(px + x, py + y, 0);
		}
	}
}

/* Draw a scaled, white, transparent string (for the progress-mode label). */
static void draw_label(uint32_t px, uint32_t py, const char *s, uint32_t scale)
{
	for (; *s; s++, px += CELL_W * scale) {
		unsigned char c = *s;
		if (c < 32 || c > 126)
			c = ' ';
		const uint8_t *g = fbcon_font[c - 32];
		for (uint32_t y = 0; y < CELL_H; y++) {
			uint8_t bits = g[y];
			for (uint32_t x = 0; x < CELL_W; x++) {
				if (!(bits & (0x80 >> x)))
					continue;
				for (uint32_t sy = 0; sy < scale; sy++)
					for (uint32_t sx = 0; sx < scale; sx++)
						put_pixel(px + x * scale + sx,
							  py + y * scale + sy, st.fg);
			}
		}
	}
}

/* ---- LOG mode ---------------------------------------------------------- */

static void newline(void)
{
	st.cur_col = 0;
	if (++st.cur_row >= st.rows) {
		fill_rect(0, 0, st.width, st.rows * CELL_H, 0);
		st.cur_row = 0;
	}
}

void fbcon_tx_byte(unsigned char c)
{
	if (!st.ready || st.mode != FBM_LOG)
		return;

	switch (c) {
	case '\r':
		st.cur_col = 0;
		return;
	case '\n':
		newline();
		return;
	case '\t':
		st.cur_col = (st.cur_col + 8) & ~7u;
		if (st.cur_col >= st.cols)
			newline();
		return;
	default:
		break;
	}

	if (c < 32 || c > 126)
		return;

	blit_glyph(c, st.cur_col * CELL_W, st.cur_row * CELL_H, true);
	if (++st.cur_col >= st.cols)
		newline();
}

/* ---- PROGRESS mode ----------------------------------------------------- */

static void status_show(const char *label, unsigned int permille)
{
	const uint32_t scale = 2;
	uint32_t bw = st.width * 3 / 5;			/* 60% wide */
	uint32_t bx = (st.width - bw) / 2;
	uint32_t bh = CELL_H / 2;			/* thin bar */
	uint32_t by = st.height * 90 / 100;		/* near the bottom */

	/* bar: white fill (matches the coreboot logo) on a dark track */
	fill_rect(bx, by, bw, bh, pack(TRK_R, TRK_G, TRK_B));
	fill_rect(bx, by, (uint64_t)bw * permille / 1000, bh, st.fg);

	/* stage label: scaled white text on a thin dark band just above the bar */
	uint32_t th = CELL_H * scale;
	uint32_t bandy = by - th - 14;
	fill_rect(bx, bandy - 6, bw, th + 12, pack(0x10, 0x10, 0x10));
	uint32_t tw = strlen(label) * CELL_W * scale;
	uint32_t tx = (tw < bw) ? (st.width - tw) / 2 : bx;
	draw_label(tx, bandy, label, scale);
}

/* ---- common progress entry point --------------------------------------- */

void fbcon_progress(unsigned int permille)
{
	if (!st.ready)
		return;
	if (permille > 1000)
		permille = 1000;
	if (st.mode == FBM_PROGRESS) {
		status_show("", permille);
		return;
	}
	/* LOG mode: thin bar along the very bottom. */
	uint32_t y = st.height - BAR_H;
	uint32_t w = (uint64_t)st.width * permille / 1000;
	fill_rect(0, y, st.width, BAR_H, 0);
	fill_rect(0, y, w, BAR_H, st.fg);
}

static void phase(const char *label, unsigned int permille)
{
	if (!st.ready)
		return;
	if (st.mode == FBM_PROGRESS)
		status_show(label, permille);
	else if (st.mode == FBM_LOG)
		fbcon_progress(permille);
}

/* ---- LOG-mode early-log replay ----------------------------------------- */

static uint8_t log_at(const uint8_t *buf, uint32_t s1s, uint32_t s1l,
		      uint32_t s2s, uint32_t i)
{
	if (i < s1l)
		return buf[s1s + i];
	return buf[s2s + (i - s1l)];
}

static void replay_backlog(void)
{
	const uint8_t *buf;
	uint32_t size, cursor;
	int overflow;

	cbmem_console_get(&buf, &size, &cursor, &overflow);
	if (!buf || !size)
		return;

	uint32_t s1s = 0, s1l = cursor, s2s = 0;
	if (overflow) {
		s1s = cursor;
		s1l = size - cursor;
		s2s = 0;
	}
	uint32_t total = s1l + (overflow ? cursor : 0);
	if (!total)
		return;

	uint32_t start = 0, seen = 0;
	for (uint32_t i = total; i > 0; i--) {
		if (log_at(buf, s1s, s1l, s2s, i - 1) == '\n') {
			if (++seen >= st.rows) {
				start = i;
				break;
			}
		}
	}

	for (uint32_t i = start; i < total; i++)
		fbcon_tx_byte(log_at(buf, s1s, s1l, s2s, i));
}

/* ---- init -------------------------------------------------------------- */

void fbcon_init(void)
{
	if (st.ready)
		return;

	st.mode = get_uint_option("fb_console", FBM_PROGRESS);
	if (st.mode == FBM_OFF)
		return;

	/*
	 * Be defensive about the framebuffer, whatever set it up (libgfxinit,
	 * FSP GOP, VGA, ...): bail safely on anything we cannot draw into. With
	 * no usable framebuffer we simply stay inert and the boot and the
	 * payload's own graphics proceed unaffected.
	 */
	const struct lb_framebuffer *fb = get_lb_framebuffer();
	if (!fb || !fb->physical_address)
		return;
	if (fb->bits_per_pixel < 16 || fb->bits_per_pixel > 32)
		return;
	if (!fb->x_resolution || !fb->y_resolution)
		return;
	if (fb->bytes_per_line < fb->x_resolution * ((fb->bits_per_pixel + 7u) / 8u))
		return;

	st.fb = (uint8_t *)(uintptr_t)fb->physical_address;
	st.width = fb->x_resolution;
	st.height = fb->y_resolution;
	st.stride = fb->bytes_per_line;
	st.bpp = (fb->bits_per_pixel + 7) / 8;
	st.is32 = (st.bpp == 4);
	st.r_pos = fb->red_mask_pos;
	st.g_pos = fb->green_mask_pos;
	st.b_pos = fb->blue_mask_pos;
	st.fg = pack(0xff, 0xff, 0xff);

	st.cols = st.width / CELL_W;
	st.rows = (st.height - BAR_H - BAR_GAP) / CELL_H;
	if (!st.cols || !st.rows)
		return;
	st.cur_col = 0;
	st.cur_row = 0;

	/* LOG needs a clean background. PROGRESS leaves a bootsplash (if built)
	   in place and only paints its bar near the bottom. */
	if (st.mode == FBM_LOG || !(CONFIG(BOOTSPLASH) || CONFIG(BMP_LOGO)))
		fill_rect(0, 0, st.width, st.height, 0);
	st.ready = true;

	if (st.mode == FBM_LOG)
		replay_backlog();
}

/* --- boot-state wiring -------------------------------------------------- */
/* Name the payload coreboot hands control to, for the final step. */
static const char *payload_label(void)
{
	if (CONFIG(PAYLOAD_EDK2))
		return "Starting edk2 (UEFI)";
	if (CONFIG(PAYLOAD_SEABIOS))
		return "Starting SeaBIOS";
	if (CONFIG(PAYLOAD_LINUXBOOT))
		return "Starting LinuxBoot";
	if (CONFIG(PAYLOAD_GRUB2))
		return "Starting GRUB2";
	if (CONFIG(PAYLOAD_DEPTHCHARGE))
		return "Starting depthcharge";
	return "Starting payload";
}

/*
 * Progress steps. The framebuffer only becomes drawable once graphics are up
 * (device init), so the first step is at BS_POST_DEVICE; from there we tap the
 * entry and exit of the remaining boot states for a finer, more steady climb.
 */
static void bs_devices(void *unused)	{ fbcon_init(); phase("Initializing devices", 300); }
static void bs_finalize(void *unused)	{ phase("Finalizing hardware", 440); }
static void bs_tables(void *unused)	{ phase("Writing boot tables", 580); }
static void bs_tables_done(void *unused){ phase("Preparing ACPI", 700); }
static void bs_load(void *unused)	{ phase("Loading payload", 830); }
static void bs_load_done(void *unused)	{ phase("Payload ready", 940); }
static void bs_boot(void *unused)	{ phase(payload_label(), 1000); }

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE,     BS_ON_ENTRY, bs_devices,     NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_ENTRY, bs_finalize,    NULL);
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES,    BS_ON_ENTRY, bs_tables,      NULL);
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES,    BS_ON_EXIT,  bs_tables_done, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD,    BS_ON_ENTRY, bs_load,        NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD,    BS_ON_EXIT,  bs_load_done,   NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT,    BS_ON_ENTRY, bs_boot,        NULL);
