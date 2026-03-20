#include "print.h"
#include "io.h"
#include "serial.h"

static const size_t NUM_COLS = 80;
static const size_t NUM_ROWS = 25;

struct Char {
    uint8_t character;
    uint8_t color;
};

static struct Char* const buffer = (struct Char*) 0xb8000;
static size_t col = 0;
static size_t row = 0;
static uint8_t color = PRINT_COLOR_WHITE | (PRINT_COLOR_BLACK << 4);
static int serial_mirror_enabled = 0;

static void update_cursor(void) {
    uint16_t position = (uint16_t)(row * NUM_COLS + col);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

static void clear_row(size_t row_index) {
    struct Char empty = {
        .character = ' ',
        .color = color,
    };

    for (size_t current_col = 0; current_col < NUM_COLS; current_col++) {
        buffer[current_col + (NUM_COLS * row_index)] = empty;
    }
}

void print_enable_serial_mirror(void) {
    serial_mirror_enabled = 1;
}

void print_disable_serial_mirror(void) {
    serial_mirror_enabled = 0;
}

void print_clear(void) {
    col = 0;
    row = 0;

    for (size_t current_row = 0; current_row < NUM_ROWS; current_row++) {
        clear_row(current_row);
    }

    update_cursor();
}

static void print_newline(void) {
    col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    for (size_t current_row = 1; current_row < NUM_ROWS; current_row++) {
        for (size_t current_col = 0; current_col < NUM_COLS; current_col++) {
            buffer[current_col + (NUM_COLS * (current_row - 1))] =
                buffer[current_col + (NUM_COLS * current_row)];
        }
    }

    clear_row(NUM_ROWS - 1);
    update_cursor();
}

void print_char(char character) {
    if (serial_mirror_enabled) {
        serial_write_char(character);
    }

    if (character == '\n') {
        print_newline();
        return;
    }

    if (character == '\r') {
        col = 0;
        update_cursor();
        return;
    }

    if (character == '\t') {
        for (int i = 0; i < 4; i++) {
            print_char(' ');
        }
        return;
    }

    if (character == '\b') {
        print_backspace();
        return;
    }

    if (col >= NUM_COLS) {
        print_newline();
    }

    buffer[col + (NUM_COLS * row)] = (struct Char) {
        .character = (uint8_t) character,
        .color = color,
    };

    col++;
    update_cursor();
}

void print_str(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        print_char(str[i]);
    }
}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground | (background << 4);
}

void print_backspace(void) {
    if (col > 0) {
        col--;
    } else if (row > 0) {
        row--;
        col = NUM_COLS - 1;
    } else {
        return;
    }

    buffer[col + (NUM_COLS * row)] = (struct Char) {
        .character = ' ',
        .color = color,
    };

    update_cursor();
}

void print_move_cursor_left(void) {
    if (col > 0) {
        col--;
    } else if (row > 0) {
        row--;
        col = NUM_COLS - 1;
    } else {
        return;
    }

    update_cursor();
}

void print_move_cursor_right(void) {
    if (col < NUM_COLS - 1) {
        col++;
    } else if (row < NUM_ROWS - 1) {
        row++;
        col = 0;
    } else {
        return;
    }

    update_cursor();
}

void print_hex64(uint64_t value) {
    static const char* hex = "0123456789ABCDEF";
    print_str("0x");

    for (int shift = 60; shift >= 0; shift -= 4) {
        print_char(hex[(value >> shift) & 0xF]);
    }
}

void print_dec(uint64_t value) {
    char buffer[21];
    size_t i = 0;

    if (value == 0) {
        print_char('0');
        return;
    }

    while (value > 0 && i < sizeof(buffer)) {
        buffer[i++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (i > 0) {
        print_char(buffer[--i]);
    }
}
