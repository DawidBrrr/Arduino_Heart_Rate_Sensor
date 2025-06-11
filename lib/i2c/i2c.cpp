#include "i2c.h"

void i2c_init(void) {
    TWSR = 0x00;    // No prescaler
    TWBR = 0x0C;    // 400kHz at 16MHz: (16000000/400000-16)/2 = 12 (0x0C)
    TWCR = (1<<TWEN);
}

uint8_t i2c_start(void) {
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    
    if ((TWSR & 0xF8) != 0x08 && (TWSR & 0xF8) != 0x10) {
        return I2C_ERROR_START;
    }
    return I2C_SUCCESS;
}

void i2c_stop(void) {
    TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
    while (TWCR & (1<<TWSTO)); // Wait for stop condition to be executed
}

uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    
    if ((TWSR & 0xF8) != 0x18 && (TWSR & 0xF8) != 0x28) {
        return I2C_ERROR_DATA;
    }
    return I2C_SUCCESS;
}

uint8_t i2c_write_byte(uint8_t address, uint8_t data) {
    if (i2c_start() != I2C_SUCCESS) return I2C_ERROR_START;
    if (i2c_write(address << 1) != I2C_SUCCESS) {
        i2c_stop();
        return I2C_ERROR_SLAW;
    }
    if (i2c_write(data) != I2C_SUCCESS) {
        i2c_stop();
        return I2C_ERROR_DATA;
    }
    i2c_stop();
    return I2C_SUCCESS;
}

uint8_t i2c_write_buffer(uint8_t address, uint8_t* buffer, uint8_t length) {
    if (i2c_start() != I2C_SUCCESS) return I2C_ERROR_START;
    if (i2c_write(address << 1) != I2C_SUCCESS) {
        i2c_stop();
        return I2C_ERROR_SLAW;
    }
    
    for (uint8_t i = 0; i < length; i++) {
        if (i2c_write(buffer[i]) != I2C_SUCCESS) {
            i2c_stop();
            return I2C_ERROR_DATA;
        }
    }
    
    i2c_stop();
    return I2C_SUCCESS;
}