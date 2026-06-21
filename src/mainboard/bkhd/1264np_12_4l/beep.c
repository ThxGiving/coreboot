/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <delay.h>
#include <gpio.h>
#include <option.h>
#include <console/console.h>
#include "beep.h"

/*
 * The front-panel buzzer is a PASSIVE piezo on GPP_H4. This was determined
 * empirically by sweeping the SoC GPIOs from the OS (libgpiod) and bit-banging
 * each output pad until the buzzer sounded - GPP_H4 (gpiochip line 164,
 * pin GPPC_H_4) is the one. A passive buzzer has no internal oscillator, so it
 * must be toggled at the audio frequency to make a tone.
 *
 * Note: this board does NOT use the legacy PC speaker (PIT + port 0x61). Its
 * SPKR native function lives on GPP_B14, but that pad is wired to the SATA
 * activity LED here, not to the buzzer - so the PC-speaker path stays silent.
 * The stock AMI firmware also drives the buzzer via this GPIO, not via SPKR.
 */
static const struct pad_config buzzer_pad[] = {
	PAD_CFG_GPO(GPP_H4, 0, DEEP),	/* passive buzzer, idle low */
};

/*
 * Ascending arpeggio = "all systems go". Deliberately bright and rising so it
 * can never be mistaken for an error tone. { frequency_hz, duration_ms }.
 */
static const struct {
	unsigned int freq_hz;
	unsigned int dur_ms;
} boot_chime[] = {
	{  784,  90 },	/* G5 */
	{  988,  90 },	/* B5 */
	{ 1175,  90 },	/* D6 */
	{ 1568, 150 },	/* G6 - resolve high = positive */
};

/* Bit-bang a square wave on the buzzer pad at freq_hz for dur_ms. */
static void play_tone(unsigned int freq_hz, unsigned int dur_ms)
{
	const unsigned int half_us = 1000000u / freq_hz / 2u;
	const unsigned int cycles = (dur_ms * 1000u) / (half_us * 2u);

	for (unsigned int i = 0; i < cycles; i++) {
		gpio_set(GPP_H4, 1);
		udelay(half_us);
		gpio_set(GPP_H4, 0);
		udelay(half_us);
	}
}

void mainboard_boot_beep(void)
{
	/*
	 * Runtime toggle from the BIOS setup / CFR ("Boot beep"); the build-time
	 * BEEP_ON_BOOT is just the default when no option store is present.
	 */
	if (!get_uint_option("boot_beep", CONFIG(BEEP_ON_BOOT))) {
		printk(BIOS_INFO, "BEEP: boot beep disabled by option\n");
		return;
	}

	gpio_configure_pads(buzzer_pad, ARRAY_SIZE(buzzer_pad));

	printk(BIOS_INFO, "BEEP: playing boot chime on GPP_H4\n");
	for (size_t i = 0; i < ARRAY_SIZE(boot_chime); i++) {
		play_tone(boot_chime[i].freq_hz, boot_chime[i].dur_ms);
		mdelay(30);	/* short gap between notes for articulation */
	}

	gpio_set(GPP_H4, 0);	/* leave the buzzer silent */
}
