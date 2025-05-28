#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

#define I2C_SUCCESS 0
#define I2C_ERROR_START 1
#define I2C_ERROR_SLAW 2
#define I2C_ERROR_DATA 3

void i2c_init(void);
uint8_t i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_write_byte(uint8_t address, uint8_t data);
uint8_t i2c_write_buffer(uint8_t address, uint8_t* buffer, uint8_t length);

#endif // I2C_H