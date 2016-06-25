#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "spi.h"

typedef enum {
	spi_state_ready,
	spi_state_running
} spi_state_t;

typedef struct {
	uint8_t end; // index just past the last byte of this transaction
	spi_channel_t channel;	
} transaction_t;

static transaction_t transactions[SPI_TRANSACTION_BUFSIZE];
static uint8_t transaction_begin; // index of the current transaction
static uint8_t transaction_end; // index just past the last transaction

static uint8_t tx_buffer[SPI_TX_BUFSIZE];
static uint8_t tx_buffer_pos;

static volatile spi_state_t spi_state;

void spi_setup() {
	DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
	SPCR = (1 << SPIE) | (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	spi_state = spi_state_ready;
}

spi_channel_t spi_setup_channel(uint8_t mask) {
	SPI_SS_DDR |= mask;
	SPI_SS_PORT |= mask;

	transaction_begin = SPI_TRANSACTION_BUFSIZE - 1;
	transaction_end = 0;
	tx_buffer_pos = 0;

	transactions[transaction_begin] = (transaction_t){
		.end = 0,
	};

	return (spi_channel_t){mask};
}

bool spi_tx(spi_channel_t channel, const void* data, uint8_t len) {
	// put data into the data buffer
	uint8_t last_transaction_idx = (transaction_end - 1 + SPI_TRANSACTION_BUFSIZE) % SPI_TRANSACTION_BUFSIZE;
	transaction_t last_transaction = transactions[last_transaction_idx];

	uint8_t data_begin = last_transaction.end;
	uint8_t data_end = (last_transaction.end + len) % SPI_TX_BUFSIZE;

	if (data_end < data_begin) {
		// data wraps around in the buffer
		memcpy(tx_buffer + data_begin, data, SPI_TX_BUFSIZE - data_begin);
		memcpy(tx_buffer, ((const uint8_t*) data) + SPI_TX_BUFSIZE - data_begin, data_end);
	} else {
		memcpy(tx_buffer + data_begin, data, len);
	}

	// generate a transaction
	transactions[transaction_end] = (transaction_t){
		.end = data_end,
		.channel = channel,
	};

	cli();
	transaction_end = (transaction_end + 1) % SPI_TRANSACTION_BUFSIZE;
	if (spi_state == spi_state_ready) {
		//we need to initiate the transfer
		spi_state = spi_state_running;
		SPI_SS_PORT &= ~channel.mask;
		transaction_begin = (transaction_begin + 1) % SPI_TRANSACTION_BUFSIZE;
		tx_buffer_pos = (data_begin + 1) % SPI_TX_BUFSIZE;
		SPDR = tx_buffer[data_begin];
	}
	sei();

	return true;
}

ISR(SPI_STC_vect) {
	transaction_t current_transaction = transactions[transaction_begin];

	if (tx_buffer_pos == current_transaction.end) {
		// this transaction is done
		SPI_SS_PORT |= current_transaction.channel.mask;
		uint8_t next_transaction_idx = (transaction_begin + 1) % SPI_TRANSACTION_BUFSIZE;
		if (next_transaction_idx != transaction_end) {
			// there still is at least one transaction in the queue
			transaction_t next_transaction = transactions[next_transaction_idx];
			transaction_begin = next_transaction_idx;
			SPI_SS_PORT &= ~next_transaction.channel.mask;
			SPDR = tx_buffer[tx_buffer_pos];
			tx_buffer_pos = (tx_buffer_pos + 1) % SPI_TX_BUFSIZE;
		} else {
			// there are no more transactions
			spi_state = spi_state_ready;
		}
	} else {
		// this transaction is not yet done, transmit the next byte
		SPDR = tx_buffer[tx_buffer_pos];
		tx_buffer_pos = (tx_buffer_pos + 1) % SPI_TX_BUFSIZE;
	}
}

bool spi_ready() {
	return spi_state == spi_state_ready;
}
