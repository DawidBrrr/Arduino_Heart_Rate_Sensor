#include "display.h"

static uint8_t display_buffer[1024]; // 128x64/8 = 1024 bytes

// Complete 5x8 font for ASCII characters 32-126
static const uint8_t font5x8[][5] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32 (space)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 33 !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34 "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // 35 #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // 36 $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37 %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 38 &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 39 '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // 40 (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // 41 )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // 42 *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // 43 +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 44 ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45 -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 46 .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47 /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 48 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 49 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 50 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 51 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 52 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 54 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 55 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 57 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 58 :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 59 ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // 60 <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61 =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // 62 >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 63 ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // 64 @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 65 A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 66 B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 67 C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 68 D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 69 E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // 70 F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // 71 G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 72 H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 73 I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 74 J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 75 K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 76 L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 77 M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 78 N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 79 O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 80 P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 81 Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 82 R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 83 S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 84 T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 85 U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 86 V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 87 W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88 X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 89 Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 90 Z
};

static uint8_t display_command(uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd}; // 0x00 for command mode
    return i2c_write_buffer(SH1106_I2C_ADDRESS, buffer, 2);
}

static uint8_t display_data(uint8_t data) {
    uint8_t buffer[2] = {0x40, data}; // 0x40 for data mode
    return i2c_write_buffer(SH1106_I2C_ADDRESS, buffer, 2);
}

void display_init(void) {
    _delay_ms(100);
    
    display_command(0xAE); // Display off
    display_command(0xD5); // Set display clock
    display_command(0x80);
    display_command(0xA8); // Set multiplex
    display_command(0x3F);
    display_command(0xD3); // Set display offset
    display_command(0x00);
    display_command(0x40); // Set start line
    display_command(0x8D); // Charge pump
    display_command(0x14);
    display_command(0x20); // Memory mode
    display_command(0x00);
    display_command(0xA1); // Segment remap
    display_command(0xC8); // COM scan direction
    display_command(0xDA); // COM pins
    display_command(0x12);
    display_command(0x81); // Set contrast
    display_command(0xCF);
    display_command(0xD9); // Set precharge
    display_command(0xF1);
    display_command(0xDB); // Set VCOM detect
    display_command(0x40);
    display_command(0xA4); // Display all on resume
    display_command(0xA6); // Normal display
    display_command(0xAF); // Display on
    
    display_clear();
    display_update();
}

void display_clear(void) {
    memset(display_buffer, 0, 1024);
}

void display_pixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        if (color) {
            display_buffer[x + (y / 8) * SCREEN_WIDTH] |= (1 << (y % 8));
        } else {
            display_buffer[x + (y / 8) * SCREEN_WIDTH] &= ~(1 << (y % 8));
        }
    }
}

void display_line(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        display_pixel(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void display_char(int x, int y, char c) {
    if (c < 32 || c > 90) return; // Only printable characters
    
    uint8_t index = c - 32;
    
    for (int i = 0; i < 5; i++) {
        uint8_t line = pgm_read_byte(&font5x8[index][i]);
        for (int j = 0; j < 8; j++) {
            if (line & (1 << j)) {
                display_pixel(x + i, y + j, 1);
            }
        }
    }
}

void display_string(int x, int y, const char* str) {
    while (*str) {
        display_char(x, y, *str);
        x += 6;
        str++;
    }
}

void display_number(int x, int y, int num) {
    char buffer[12];
    sprintf(buffer, "%d", num);
    display_string(x, y, buffer);
}

void display_float(int x, int y, float num, uint8_t decimals) {
    char buffer[20];
    if (decimals == 1) {
        sprintf(buffer, "%.1f", num);
    } else if (decimals == 2) {
        sprintf(buffer, "%.2f", num);
    } else {
        sprintf(buffer, "%.0f", num);
    }
    display_string(x, y, buffer);
}

void display_update(void) {
    for (int page = 0; page < 8; page++) {
        display_command(0xB0 + page); // Set page
        display_command(0x02); // Set column low nibble (SH1106 specific)
        display_command(0x10); // Set column high nibble
        
        for (int col = 0; col < 128; col++) {
            display_data(display_buffer[col + page * 128]);
        }
    }
}

uint8_t* display_get_buffer(void) {
    return display_buffer;
}