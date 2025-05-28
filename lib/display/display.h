#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>
#include <avr/pgmspace.h>

#define SH1106_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void display_init(void);
void display_clear(void);
void display_pixel(int x, int y, uint8_t color);
void display_line(int x0, int y0, int x1, int y1, uint8_t color);
void display_char(int x, int y, char c);
void display_string(int x, int y, const char* str);
void display_number(int x, int y, int num);
void display_float(int x, int y, float num, uint8_t decimals);
void display_update(void);
uint8_t* display_get_buffer(void);

#endif // DISPLAY_H