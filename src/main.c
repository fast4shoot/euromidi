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
#include "events.h"
#include "mcp4728.h"
#include "midi.h"
#include "tick_timer.h"
#include "ui.h"

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
	ui_setup();
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
	
	tick_timer_setup();

	uint8_t counter = 0;
	uint8_t current_note = 0;
	uint8_t current_velocity = 0;
	event_t ev = {0, 0, 0};

	while(1) {
		bool play_note = false;
		bool stop_note = false;

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
					ui_next();
					break;
				case event_encoder_down:
					ui_prev();
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
			display_show_note(ev.a);
			play_note = false;
		} else if (stop_note) {
			display_show(' ', ' ', ' ');
			stop_note = false;
		}

		ui_update_display();
	}

	return 0;
}

