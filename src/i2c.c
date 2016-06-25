#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>
#include "i2c.h"

#define PRESCALER 1
#define TWBR_VAL ((((F_CPU / F_I2C) / PRESCALER) - 16 ) / 2)

#if PRESCALER != 1
#error "PRESCALER has to be 1"
#endif

typedef enum {
	i2c_state_ready,
	i2c_state_running
} i2c_state_t;

static uint8_t tx_buffer[I2C_TX_BUFSIZE];
static uint8_t tx_buffer_len;
static uint8_t tx_buffer_pos;
static volatile i2c_state_t i2c_state;

void i2c_setup()
{
	TWBR = (uint8_t)TWBR_VAL;
	i2c_state = i2c_state_ready;
}

bool i2c_tx(uint8_t addr, const void* data, uint8_t len)
{
	if (i2c_state == i2c_state_running) return false;

	i2c_state = i2c_state_running;
	TWCR = 0;
	// initiate start
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	
	// do something useful while waiting
	memcpy(tx_buffer, data, len);
	tx_buffer_len = len;
	tx_buffer_pos = 0;
	
	// wait for confirmation
	while( !(TWCR & (1<<TWINT)) );

	if ((TWSR & 0xF8) != TW_START) return false;

	// load the address
	TWDR = addr;
	// initiate transfer
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);

	return true;
}

ISR(TWI_vect)
{
	if (tx_buffer_len == tx_buffer_pos) {
		// transmit STOP condition
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
		i2c_state = i2c_state_ready;
	} else {
		// transmit next octet
		TWDR = tx_buffer[tx_buffer_pos++];
		// start transmission of data
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
	}
}

bool i2c_ready() {
	return i2c_state == i2c_state_ready;
}
