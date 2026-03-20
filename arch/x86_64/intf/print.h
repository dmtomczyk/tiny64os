#pragma once

#include <stddef.h>
#include <stdint.h>

enum {
    PRINT_COLOR_BLACK = 0,
    PRINT_COLOR_BLUE = 1,
    PRINT_COLOR_GREEN = 2,
    PRINT_COLOR_CYAN = 3,
    PRINT_COLOR_RED = 4,
    PRINT_COLOR_MAGENTA = 5,
    PRINT_COLOR_BROWN = 6,
    PRINT_COLOR_LIGHT_GRAY = 7,
    PRINT_COLOR_DARK_GRAY = 8,
    PRINT_COLOR_LIGHT_BLUE = 9,
    PRINT_COLOR_LIGHT_GREEN = 10,
    PRINT_COLOR_LIGHT_CYAN = 11,
    PRINT_COLOR_LIGHT_RED = 12,
    PRINT_COLOR_PINK = 13,
    PRINT_COLOR_YELLOW = 14,
    PRINT_COLOR_WHITE = 15,
};

void print_clear(void);
void print_char(char character);
void print_str(const char* string);
void print_set_color(uint8_t foreground, uint8_t background);
void print_backspace(void);
void print_move_cursor_left(void);
void print_move_cursor_right(void);
void print_hex64(uint64_t value);
void print_dec(uint64_t value);
void print_enable_serial_mirror(void);
void print_disable_serial_mirror(void);
