#include "encoder.h"

#include <avr/io.h>
#include <stdbool.h>
#include "events.h"

enum encoder_state_enum {
	encoder_state_n, encoder_state_a, encoder_state_b
};

typedef uint8_t encoder_state_t;

static uint8_t last_port;
static encoder_state_t state;

void encoder_init() {
	PCICR |= (1 << ENC_PCIE);
	ENC_PCMSK |= (1 << ENC_A_PCINT) | (1 << ENC_B_PCINT) | (1 << ENC_BTN_PCINT);
	last_port = 0;
	state = encoder_state_n;
}

void encoder_update() {
	uint8_t port = ENC_PIN;
	bool a_change = (port & (1 << ENC_A)) != (last_port & (1 << ENC_A));
	bool b_change = (port & (1 << ENC_B)) != (last_port & (1 << ENC_B));
	bool btn_change = (port & (1 << ENC_BTN)) != (last_port & (1 << ENC_BTN));
	bool a = !!(port & (1 << ENC_A));
	bool b = !!(port & (1 << ENC_B));
	bool btn = !!(port & (1 << ENC_BTN));
	last_port = port;

	switch (state) {
		case encoder_state_n:
			if (a_change && !a && b) state = encoder_state_a;
			else if (b_change && !b && a) state = encoder_state_b;
			break;

		case encoder_state_a:
			if (a_change && a) {
			   	state = encoder_state_n;
			} else if (b_change && !b) {
				state = encoder_state_n;
				event_push((event_t){.id = event_encoder_down});
			}
			break;

		case encoder_state_b:
			if (b_change && b) {
				state = encoder_state_n;
			} else if (a_change && !a) {
				state = encoder_state_n;
				event_push((event_t){.id = event_encoder_up});
			}
			break;
	}

	if (btn_change && btn) {
		event_push((event_t){.id = event_encoder_press});
	}
}
