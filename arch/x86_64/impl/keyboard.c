#include "keyboard.h"
#include "io.h"
#include "pic.h"
#include "shell.h"

static const unsigned char keyboard_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0,
    '*',
    0,
    ' ',
    0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0,
    0,
    0,
    '-',
    0,
    0,
    0,
    '+',
    0,
    0,
    0,
    0,
    0,
    0, 0, 0,
    0,
    0,
    0,
};

static const unsigned char keyboard_shift_map[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b',
    '\t',
    'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?', 0,
    '*',
    0,
    ' ',
    0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0,
    0,
    0,
    '-',
    0,
    0,
    0,
    '+',
    0,
    0,
    0,
    0,
    0,
    0, 0, 0,
    0,
    0,
    0,
};

static int shift_down = 0;
static int ctrl_down = 0;
static int extended_scancode = 0;

const char* keyboard_layout(void) {
    return "US basic";
}

void keyboard_handle_irq(void) {
    uint8_t scancode = inb(0x60);

    if (scancode == 0xE0) {
        extended_scancode = 1;
        pic_send_eoi(1);
        return;
    }

    if (extended_scancode) {
        if (scancode == 0x48) {
            shell_history_prev();
        } else if (scancode == 0x50) {
            shell_history_next();
        } else if (scancode == 0x4B) {
            shell_cursor_left();
        } else if (scancode == 0x4D) {
            shell_cursor_right();
        } else if (scancode == 0x47) {
            shell_cursor_home();
        } else if (scancode == 0x4F) {
            shell_cursor_end();
        } else if (scancode == 0x53) {
            shell_delete_char();
        }

        extended_scancode = 0;
        pic_send_eoi(1);
        return;
    }

    if (scancode == 0x2A || scancode == 0x36) {
        shift_down = 1;
        pic_send_eoi(1);
        return;
    }

    if (scancode == 0x1D) {
        ctrl_down = 1;
        pic_send_eoi(1);
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_down = 0;
        pic_send_eoi(1);
        return;
    }

    if (scancode == 0x9D) {
        ctrl_down = 0;
        pic_send_eoi(1);
        return;
    }

    if (scancode & 0x80) {
        pic_send_eoi(1);
        return;
    }

    if (ctrl_down && scancode == 0x2E) {
        shell_cancel_line();
        pic_send_eoi(1);
        return;
    }

    if (scancode < sizeof(keyboard_map)) {
        unsigned char c = shift_down ? keyboard_shift_map[scancode] : keyboard_map[scancode];
        if (c != 0) {
            shell_handle_char((char)c);
        }
    }

    pic_send_eoi(1);
}
