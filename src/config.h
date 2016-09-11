#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/// Enum of all possible parameters
/// Each one of these can have up to 256 values,
/// starting with zero and going up to the value
/// returned by config_get_max()
enum {
	config_param_channel = 0,
	config_param_division = 1, // 1 2 3 4 6 8 12 16 24 32 48 96
	config_param_out_a = 2, // 0-120 7bit, 121 - 152 32 14bit, 153 pitch
	config_param_out_b = 3, // ditto
	config_param_out_c = 4, // ditto

	config_param_max = 4, // highest param values
	config_param_count = 5, // total number of params
};

typedef uint8_t config_param_t;

void config_setup();

void config_save();

/// Gets the maximum value of a parameter
uint8_t config_get_max(config_param_t param);

/// Gets a three letter name of a parameter's value
/// that's suitable for displaying to the user
void config_get_name(config_param_t param, uint8_t value, uint8_t* name);

uint8_t config_get(config_param_t param);

void config_set(config_param_t param, uint8_t value);

#endif
