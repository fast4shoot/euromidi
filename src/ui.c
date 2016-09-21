#include "ui.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "config.h"
#include "display.h"

static config_param_t selection;
static bool selected;
static uint8_t value;
static uint8_t value_max;

void ui_setup() {
	selection = 0;
	selected = false;	
}

void ui_next() {
	if (!selected) {
		if (selection == config_param_max) selection = 0;
		else selection++;
		value = config_get(selection);
	} else {
		if (value == value_max) value = 0;
		else value++;
	}
}

void ui_prev() {
	if (!selected) {
		if (selection == 0) selection = config_param_max;
		else selection--;
		value = config_get(selection);
	} else {
		if (value == 0) value = value_max;
		else value--;
	}
}

void ui_button() {
	if (!selected) {
		selected = true;
		value_max = config_get_max(selection);
	} else {
		selected = false;
		config_set(selection, value);
	}
}

void ui_update_display() {
	uint8_t name[3] = "ERR";
	config_get_name(selection, value, name);
	display_show(name[0], name[1], name[2]);
	display_leds(1 << selection);
}
