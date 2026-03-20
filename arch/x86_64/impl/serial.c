#include "serial.h"
#include "io.h"

#define COM1 0x3F8

static int serial_ready = 0;

static int serial_is_transmit_empty(void) {
    return inb(COM1 + 5) & 0x20;
}

void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);

    serial_ready = 1;
    serial_write_str("[serial] COM1 initialized\n");
}

void serial_write_char(char c) {
    if (!serial_ready) {
        return;
    }

    while (!serial_is_transmit_empty()) {
    }

    outb(COM1, (unsigned char)c);
}

void serial_write_str(const char* str) {
    if (!serial_ready) {
        return;
    }

    for (unsigned int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            serial_write_char('\r');
        }
        serial_write_char(str[i]);
    }
}
