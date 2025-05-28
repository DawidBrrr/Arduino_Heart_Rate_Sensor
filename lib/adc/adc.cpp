#include "adc.h"

void adc_init(void) {
    ADMUX = (1<<REFS0); // AVCC reference (5V)
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // Enable, prescaler 128
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
    ADCSRA |= (1<<ADSC);
    while (ADCSRA & (1<<ADSC));
    return ADC;
}

float adc_read_voltage(uint8_t channel) {
    uint16_t raw = adc_read(channel);
    return (raw * 5.0) / 1023.0; // Convert to voltage (0-5V)
}