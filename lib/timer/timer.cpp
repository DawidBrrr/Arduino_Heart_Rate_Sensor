#include "timer.h"

volatile unsigned long millis_counter = 0;

void timer0_init(void) {
    // Configure Timer0 for 1ms interrupts
    TCCR0A = (1 << WGM01);              // CTC mode
    TCCR0B = (1 << CS01) | (1 << CS00); // prescaler 64
    OCR0A = 249;                        // for 1ms at 16MHz: (16MHz/64)/1000Hz - 1
    TIMSK0 = (1 << OCIE0A);            // Enable compare match interrupt
}

ISR(TIMER0_COMPA_vect) {
    millis_counter++;
}

unsigned long millis(void) {
    unsigned long m;
    uint8_t oldSREG = SREG;   // Save interrupt state
    cli();                     // Disable interrupts
    m = millis_counter;        // Get counter value
    SREG = oldSREG;           // Restore interrupt state
    return m;
}

void delay_ms(unsigned int ms) {
    unsigned long start = millis();
    while (millis() - start < ms) {
        // Non-blocking delay
        asm volatile("nop");
    }
}