#ifndef SPI_H
#define SPI_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t mask;
} spi_channel_t;

void spi_setup();
spi_channel_t spi_setup_channel(uint8_t channel);
bool spi_tx(spi_channel_t channel, const void* data, uint8_t len);
bool spi_ready();

#endif
