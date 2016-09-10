#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include "spi.h"
#include "i2c.h"
#include "display.h"
#include "encoder.h"
#include "mcp4728.h"
#include "events.h"
#include "midi.h"
#include "tick_timer.h"


ISR(PCINT1_vect) {
	encoder_update();
}

void setup() {
	sei();
	spi_setup();
	i2c_setup();
	encoder_init();
	display_setup();
	midi_setup();
	midi_set_channel(0);
	tick_timer_setup();

	SELECTION_DDR |= SELECTION_PORT_BASE | (SELECTION_PORT_BASE<<1) | (SELECTION_PORT_BASE<<2) | (SELECTION_PORT_BASE<<3);
	SELECTION_PORT |= SELECTION_PORT_BASE;
}

void line(uint8_t val) {
	uint8_t ones = val % 10;
	val /= 10;
	uint8_t tens = val % 10;
	val /= 10;
	uint8_t hundreds = val % 10;
	
	display_show(hundreds + '0', tens + '0', ones + '0');
}

void display_note(uint8_t note) {
	static const char* names[] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "H "};
	const char* name = names[note % 12];
	uint8_t octave = note / 12;
	uint8_t octave_char = octave == 0 ? '-' : ('0' + octave - 1);
	display_show(name[0], name[1], octave_char);
}

uint8_t to_hex(uint8_t x) {
	x &= 0xf;
	if (x < 10) return '0' + x;
	else return 'A' + x;
}

int main(void) {
	setup();

	{
		uint8_t cmd = MCP4728_CMD_WRITE_VREF | 0x0f;
		i2c_tx(DAC_ADDR, &cmd, 1);
		while (!i2c_ready());
		cmd = MCP4728_CMD_WRITE_GAIN | 0x0f;
		i2c_tx(DAC_ADDR, &cmd, 1);
	}

	_delay_ms(10);

	uint8_t selection = 0;
	uint8_t counter = 0;
	uint8_t current_note = 0;
	uint8_t current_velocity = 0;
	event_t ev = {0, 0, 0};

	while(1) {

		bool play_note = false;
		bool stop_note = false;
		bool move_selection = false;

		while (event_peek(&ev)) {
			switch (ev.id) {
				case event_tick:
					counter++;
					break;
				case event_note_on:
					current_note = ev.a;
					current_velocity = ev.b;
					play_note = true; 
					break;
				case event_note_off:
					if (ev.a == current_note) {
						stop_note = true;
					}
					break;
				case event_encoder_up: 
					selection++;
					selection %= 4;
					move_selection = true;
					break;
				case event_encoder_down:
					selection--;
					selection = (selection + 4) % 4;
					move_selection = true;
					break;
				default: break;
			}
		}
		
		uint8_t top = (counter & 0xf0) >> 4;
		uint8_t bottom = ((counter & 0x0f) << 4) | ((counter & 0xf0) >> 4);
		uint8_t data[8] = {
			(current_note & 0x78) >> 3,
			((current_note & 0x07) << 5) | ((current_note & 0x7c) >> 2),
			(current_velocity & 0x78) >> 3,
			((current_velocity & 0x07) << 5) | ((current_velocity & 0x7c) >> 2),
			top, bottom, top, bottom};
		i2c_tx(DAC_ADDR, data, 8);

		if (play_note) {
			display_note(ev.a);
			play_note = false;
		} else if (stop_note) {
			display_show(' ', ' ', ' ');
			stop_note = false;
		}

		if (move_selection) {
			move_selection = false;
			SELECTION_PORT &= ~(SELECTION_PORT_BASE | (SELECTION_PORT_BASE<<1) | (SELECTION_PORT_BASE<<2) | (SELECTION_PORT_BASE<<3));
			SELECTION_PORT |= (SELECTION_PORT_BASE << selection);
		}
	}

	return 0;
}

