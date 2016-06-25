#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include "spi.h"
#include "i2c.h"
#include "display.h"
#include "mcp4728.h"

typedef enum {
	encoder_state_n, encoder_state_a, encoder_state_b
} encoder_state_t;

volatile uint8_t counter = 0;
volatile encoder_state_t encoder_state = encoder_state_n;

ISR(PCINT2_vect) {
	static uint8_t last_port = 0;
	uint8_t port = ENC_PIN;
	bool a_change = (port & (1 << ENC_A)) != (last_port & (1 << ENC_A));
	bool b_change = (port & (1 << ENC_B)) != (last_port & (1 << ENC_B));
	bool a = !!(port & (1 << ENC_A));
	bool b = !!(port & (1 << ENC_B));
	last_port = port;

	switch (encoder_state) {
		case encoder_state_n:
			if (a_change && !a && b) encoder_state = encoder_state_a;
			else if (b_change && !b && a) encoder_state = encoder_state_b;
			break;

		case encoder_state_a:
			if (a_change && a) encoder_state = encoder_state_n;
			else if (b_change && !b){ encoder_state = encoder_state_n; counter++; }
			break;

		case encoder_state_b:
			if (b_change && b) encoder_state = encoder_state_n;
			else if (a_change && !a) { encoder_state = encoder_state_n; counter--; }
			break;
	}
}

void encoder_setup() {
	PCICR |= (1 << ENC_PCIE);
	PCMSK2 |= (1 << ENC_A_PCINT) | (1 << ENC_B_PCINT);
}


void setup() {
	sei();
	spi_setup();
	i2c_setup();
	encoder_setup();
	display_setup();

	_delay_ms(10);
}

void line(uint8_t val) {
	uint8_t ones = val % 10;
	val /= 10;
	uint8_t tens = val % 10;
	val /= 10;
	uint8_t hundreds = val % 10;
	
	display_show(hundreds + '0', tens + '0', ones + '0');
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

	line(127);
	while(1) {
		line(counter);

		_delay_ms(10);
		uint8_t top = (counter & 0xf0) >> 4;
		uint8_t bottom = ((counter & 0x0f) << 4) | ((counter & 0xf0) >> 4);
		uint8_t data[8] = {top, bottom, top, bottom, top, bottom, top, bottom};
		i2c_tx(DAC_ADDR, data, 8);
	}

	return 0;
}

