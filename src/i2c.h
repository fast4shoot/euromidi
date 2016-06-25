#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>

void i2c_setup();

/// Returns true on success
bool i2c_tx(uint8_t addr, const void* data, uint8_t len);

bool i2c_ready();

#endif
