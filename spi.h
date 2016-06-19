#ifndef SPI_H
#define SPI_H

#include <stdbool.h>
#include <stdint.h>

void spi_setup();
bool spi_tx(uint8_t ss, const void* data, uint8_t len);
bool spi_ready();

#endif
