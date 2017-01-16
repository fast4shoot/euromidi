#include "config.h"

#include "events.h"

static const uint8_t param_max[config_param_count] = {
	[config_param_channel] = 15,
	[config_param_division] = 11, // 1 2 3 4 6 8 12 16 24 32 48 96
	[config_param_base_note] = 127, // the entire MIDI range of notes
	[config_param_pitch_bend_range] = 96, // 48 being 0 range
	[config_param_out_a_src] = 154, // 0-120 7bit, 121 - 152 32 14bit, 153 pitch
	[config_param_out_a_polarity] = 1, // 0 uni, 1 bi
	[config_param_out_b_src] = 154,
	[config_param_out_b_polarity] = 1,
	[config_param_out_c_src] = 154,
	[config_param_out_c_polarity] = 1,
};

static const uint8_t division_val[] = {
	1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 96
};

static uint8_t config_value[config_param_count];

static void number_to_name(uint8_t number, uint8_t* name) {
	uint8_t hundreds = number / 100;
	number -= hundreds * 100;
	uint8_t tens = number / 10;
	number -= tens * 10;
	uint8_t ones = number;

	name[2] = '0' + ones;
	name[1] = hundreds > 0 || tens > 0 ? '0' + tens : ' ';
	name[0] = hundreds > 0 ? '0' + hundreds : ' ';
}

void config_setup() {
	// TODO: load config from EEPROM
	config_set(config_param_channel, 0); // channel 1
	config_set(config_param_division, 7); // sixteenth notes
	config_set(config_param_base_note, 24); // C0
	config_set(config_param_pitch_bend_range, 48); // C0
	config_set(config_param_out_a_src, 153); // cc
	config_set(config_param_out_a_polarity, 0);
	config_set(config_param_out_b_src, 122); // mod wheel
	config_set(config_param_out_b_polarity, 0);
	config_set(config_param_out_c_src, 128); // volume
	config_set(config_param_out_c_polarity, 0);
}

void config_save() {
	// TODO: save data to EEPROM
	// TODO: dostat Aleše na Akicon
}

uint8_t config_get_max(config_param_t param) {
	return param_max[param];
}

void config_get_name(config_param_t param, uint8_t value, uint8_t* name) {
	switch (param) {
		case config_param_channel:
			number_to_name(value + 1, name);
			break;
		case config_param_division:
			number_to_name(division_val[value], name);
			break;
		case config_param_out_a_src:
		case config_param_out_b_src:
		case config_param_out_c_src:
			if (value < 121) {
				number_to_name(value, name);
			} else if (value < 153) {
				number_to_name(value - 121, name);
				name[0] = 'C';
			} else if (value == 153) {
				name[0] = 'P'; name[1] = 'B'; name[2] = ' ';
			} else {
				name[0] = 'U'; name[1] = 'E'; name[2] = 'L';
			}
			break;
		case config_param_out_a_polarity:
		case config_param_out_b_polarity:
		case config_param_out_c_polarity:
			if (value == 0) {
				name[0] = 'U'; name[1] = 'N'; name[2] = 'I';
			} else {
				name[0] = 'B'; name[1] = 'I'; name[2] = ' ';
			}
			break;
		default:
			number_to_name(value, name);
	}
}

uint8_t config_get(config_param_t param) {
	return config_value[param];
}

void config_set(config_param_t param, uint8_t value) {
	config_value[param] = value;
	event_push((event_t){.id = event_config_changed, .a = param, .b = value});
}
