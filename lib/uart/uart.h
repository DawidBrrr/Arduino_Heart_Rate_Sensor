#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uart_init(void);
void uart_putchar(char c);
void uart_puts(const char *string);
void uart_print_int(int value);
void uart_print_float(float value);

#endif // UART_H