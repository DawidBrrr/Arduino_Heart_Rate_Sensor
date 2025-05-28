#include "uart.h"
#include <stdio.h>

void uart_init(void) {
    UBRR0H = 0;
    UBRR0L = 8; // 115200 baud at 16MHz
    UCSR0B = (1<<TXEN0) | (1<<RXEN0);
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_putchar(char c) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;
}

void uart_puts(const char* str) {
    while (*str) {
        uart_putchar(*str++);
    }
}

void uart_print_int(int value) {
    char buffer[12];
    sprintf(buffer, "%d", value);
    uart_puts(buffer);
}

void uart_print_float(float value) {
    char buffer[20];
    sprintf(buffer, "%.2f", value);
    uart_puts(buffer);
}