#include "display.h"
#include "spi.h"
#include "font.h"
#include <avr/io.h>

static const uint8_t translate_led[] = {
	0x10, 0x20, 0x40,
	0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1, 0x80,
};

static spi_channel_t spi_channel;

void cmd(uint8_t addr, uint8_t val) {
	uint8_t data[] = {addr, val};
	spi_tx(spi_channel, data, 2);
}

void display_setup() {
	spi_channel = spi_setup_channel(1 << DISP_SS);

	cmd(0x9, 0x0); // no decode
	cmd(0xa, 0x2); // some intensity
	cmd(0xb, 0x4); // scan limit 5 digits (display + 2x leds)
	cmd(0xc, 0x1); // no shutdown
	cmd(0xf, 0x0); // no display test
	cmd(0x1, 0x1);
	display_show('E', 'R', 'R');
}

void display_show(uint8_t a, uint8_t b, uint8_t c) {
	static uint8_t disp_a = 0, disp_b = 0, disp_c = 0;
	if (disp_a == a
		&& disp_b == b
		&& disp_c == c) return;
	disp_a = a;
	disp_b = b;
	disp_c = c;

	cmd(1, font[a]);
	cmd(2, font[b]);
	cmd(3, font[c]);
}

void display_leds(uint8_t idx) {
	static uint8_t disp_idx = 0;
	if (disp_idx == idx) return;
	disp_idx = idx;
	if (idx < 3) {
		cmd(4, 0);
		cmd(5, translate_led[idx]);
	} else {
		cmd(4, translate_led[idx]);
		cmd(5, 0);
	}
}

void display_show_note(uint8_t note) {
	static const char* names[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "H "};
	const char* name = names[note % 12];
	uint8_t octave = note / 12;
	uint8_t octave_char = octave == 0 ? '-' : ('0' + octave - 1);
	display_show(name[0], name[1], octave_char);
}

void display_show_number(uint8_t number) {
	uint8_t ones = number % 10;
	number /= 10;
	uint8_t tens = number % 10;
	number /= 10;
	uint8_t hundreds = number % 10;
	
	display_show(hundreds + '0', tens + '0', ones + '0');
}
