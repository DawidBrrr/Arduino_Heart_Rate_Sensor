#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

#define SENSOR_ADC_CHANNEL 0

void adc_init(void);
uint16_t adc_read(uint8_t channel);
float adc_read_voltage(uint8_t channel);

#endif // ADC_H