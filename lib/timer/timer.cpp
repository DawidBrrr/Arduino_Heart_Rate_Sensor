#include "timer.h"
#include <util/delay.h>

volatile unsigned long millis_counter = 0;

void timer0_init(void) {
    TCCR0A = (1<<WGM01); // CTC mode
    TCCR0B = (1<<CS01) | (1<<CS00); // prescaler 64
    OCR0A = 249; // for 1ms interrupt at 16MHz
    TIMSK0 = (1<<OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
    millis_counter++;
}

unsigned long millis(void) {
    unsigned long m;
    cli();
    m = millis_counter;
    sei();
    return m;
}

void delay_ms(unsigned int ms) {
    while (ms--) {
        _delay_ms(1);
    }
}