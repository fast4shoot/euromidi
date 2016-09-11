#include "ui.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "display.h"

typedef enum {
	selection_channel = 0,
	selection_division = 1,
	selection_out_a = 2,
	selection_out_b = 3,
	selection_out_c = 4,

	selection_max = 4, // a 'sentinel' value
} selection_t;

static selection_t selection;
static bool selected;

void ui_setup() {
	SELECTION_DDR |= (7 << SELECTION_PORT_BASE);
	selection = 0;
	selected = false;	
}

void ui_next() {
	if (!selected) {
		if (selection == selection_max) selection = 0;
		else selection++;
	}
}

void ui_prev() {
	if (!selected) {
		if (selection == 0) selection = selection_max;
		else selection--;
	}
}

void ui_button() {
	if (!selected) {
		selected = true;
	}
}

void ui_update_display() {
	cli();
	uint8_t port = SELECTION_PORT;
	port &= ~(7 << SELECTION_PORT_BASE);
	port |= selection << SELECTION_PORT_BASE;
	SELECTION_PORT = port;
	sei();
}
