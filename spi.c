#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "spi.h"

typedef enum {
	spi_state_ready,
	spi_state_running
} spi_state_t;

static uint8_t tx_buffer[SPI_TX_BUFSIZE];
static uint8_t tx_buffer_len;
static uint8_t tx_buffer_pos;
static uint8_t tx_ss;
static volatile spi_state_t spi_state;

void spi_setup() {
	DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
	SPCR = (1 << SPIE) | (1 << SPE) | (1 << DORD) | (1 << MSTR) | (1 << SPR1);
	spi_state = spi_state_ready;
}

bool spi_tx(uint8_t ss, const void* data, uint8_t len) {
	if (spi_state == spi_state_running) return false;

	spi_state = spi_state_running;
	tx_ss = ss;

	SPI_SS_PORT &= ~tx_ss;

	// fill tx buffer
	memcpy(tx_buffer, data, len);
	tx_buffer_len = len;
	tx_buffer_pos = 1;

	// initiate the transfer by sending the first octet
	SPDR = tx_buffer[0];

	return true;
}

ISR(SPI_STC_vect) {
	if (tx_buffer_pos == tx_buffer_len) {
		// done
		spi_state = spi_state_ready;
		SPI_SS_PORT |= tx_ss;
	} else {
		SPDR = tx_buffer[tx_buffer_pos++];
	}
}

bool spi_ready() {
	return spi_state == spi_state_ready;
}
