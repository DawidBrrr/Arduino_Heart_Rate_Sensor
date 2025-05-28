#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

void timer0_init(void);
unsigned long millis(void);
void delay_ms(unsigned int ms);

#endif // TIMER_H
