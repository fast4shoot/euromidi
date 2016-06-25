#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include "midi.h"
#include "midi_spec.h"
#include "events.h"

#define BAUD MIDI_BAUD
#include <util/setbaud.h>

static volatile uint8_t channel;

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void midi_setup() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0);   // Enable RX and RX interrupt
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit data
}

void midi_set_channel(uint8_t ch) {
	channel = ch;
}

ISR(USART_RX_vect) {
	static uint8_t msg[3] = {0};
	static uint8_t msg_len = 0;
	static bool accept_data = false;

	uint8_t received = UDR0;
	if (MIDI_IS_REALTIME(received)) {
		// real time message
		event_t event = {.id = received};
		event_push(event);
	} else if (MIDI_IS_STATUS(received)) { 
		// status byte
		msg[0] = received; // replace current status

		// accept only data only for interesting channel mode messages
		uint8_t masked = MIDI_GET_MASKED_MSG(msg[0]);
		accept_data = MIDI_IS_CHANNEL_MODE(msg[0])
				&& MIDI_GET_CHANNEL(msg[0]) == channel
				&& (
					masked == MIDI_NOTE_OFF
					|| masked == MIDI_NOTE_ON
					|| masked == MIDI_CONTROL_CHANGE
					|| masked == MIDI_PITCH_BEND
				);
		msg_len = 1;
	} else if(accept_data) {
		// data byte that we care about
		msg[msg_len++] = received;
		if (msg_len == 3) {
			msg_len = 1; // keep running-status
			event_t event = {.id = MIDI_GET_MASKED_MSG(msg[0]), .a = msg[1], .b = msg[2]};
			event_push(event);
		}
	}
}

