#include "shell.h"
#include "pit.h"
#include "print.h"

#define SHELL_BUFFER_SIZE 128
#define SHELL_HISTORY_SIZE 8
#define PIT_HZ 100

static const char* PROMPT = "tiny64os> ";

static char line_buffer[SHELL_BUFFER_SIZE];
static unsigned int line_length = 0;
static unsigned int cursor_pos = 0;
static char history[SHELL_HISTORY_SIZE][SHELL_BUFFER_SIZE];
static unsigned int history_count = 0;
static int history_index = -1;

static void shell_prompt(void) {
    print_str(PROMPT);
}

static int shell_str_equals(const char* a, const char* b) {
    unsigned int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

static int shell_str_starts_with(const char* s, const char* prefix) {
    unsigned int i = 0;
    while (prefix[i] != '\0') {
        if (s[i] != prefix[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

static void shell_redraw_line(void) {
    print_char('\r');
    shell_prompt();

    for (unsigned int i = 0; i < line_length; i++) {
        print_char(line_buffer[i]);
    }

    print_char(' ');
    print_char('\r');
    shell_prompt();

    for (unsigned int i = 0; i < cursor_pos; i++) {
        print_char(line_buffer[i]);
    }
}

static void shell_set_line(const char* text) {
    unsigned int i = 0;
    while (text[i] != '\0' && i + 1 < SHELL_BUFFER_SIZE) {
        line_buffer[i] = text[i];
        i++;
    }

    line_length = i;
    cursor_pos = i;
    line_buffer[line_length] = '\0';
    shell_redraw_line();
}

static void shell_store_history(const char* text) {
    if (text[0] == '\0') {
        return;
    }

    if (history_count > 0 && shell_str_equals(history[history_count - 1], text)) {
        return;
    }

    if (history_count < SHELL_HISTORY_SIZE) {
        unsigned int dst = history_count++;
        for (unsigned int i = 0; i < SHELL_BUFFER_SIZE; i++) {
            history[dst][i] = text[i];
            if (text[i] == '\0') {
                break;
            }
        }
        return;
    }

    for (unsigned int entry = 1; entry < SHELL_HISTORY_SIZE; entry++) {
        for (unsigned int i = 0; i < SHELL_BUFFER_SIZE; i++) {
            history[entry - 1][i] = history[entry][i];
        }
    }

    for (unsigned int i = 0; i < SHELL_BUFFER_SIZE; i++) {
        history[SHELL_HISTORY_SIZE - 1][i] = text[i];
        if (text[i] == '\0') {
            break;
        }
    }
}

static void shell_show_help(void) {
    print_str("Commands:\n");
    print_str("  help     - show this help text\n");
    print_str("  about    - show information about tiny64os\n");
    print_str("  version  - show the current version\n");
    print_str("  uptime   - show uptime based on PIT ticks\n");
    print_str("  ticks    - show PIT tick count\n");
    print_str("  echo ... - print text back to the console\n");
    print_str("  clear    - clear the screen\n");
    print_str("  cls      - alias for clear\n");
    print_str("  reboot   - reboot the machine\n");
    print_str("  panic    - trigger an invalid opcode exception\n");
    print_str("Editing: Left/Right move, Home/End jump, Del deletes, Up/Down recall history.\n");
}

static void shell_show_about(void) {
    print_str("tiny64os is a small educational x86_64 operating system.\n");
    print_str("Current features: GRUB boot, VGA console, IDT, PIT, keyboard, shell, and serial boot logs.\n");
}

static void shell_show_version(void) {
    print_str("tiny64os version 0.1.0-dev\n");
}

static void shell_show_ticks(void) {
    print_str("ticks: ");
    print_dec(pit_get_ticks());
    print_char('\n');
}

static void shell_show_uptime(void) {
    uint64_t ticks = pit_get_ticks();
    uint64_t seconds = ticks / PIT_HZ;
    uint64_t rem = ticks % PIT_HZ;

    print_str("uptime: ");
    print_dec(seconds);
    print_char('.');
    if (((rem * 100) / PIT_HZ) < 10) {
        print_char('0');
    }
    print_dec((rem * 100) / PIT_HZ);
    print_str(" seconds\n");
}

static void shell_echo(const char* text) {
    print_str(text);
    print_char('\n');
}

static void shell_reboot(void) {
    print_str("Rebooting...\n");
    __asm__ volatile ("cli");
    __asm__ volatile ("outb %0, $0x64" : : "a"((unsigned char)0xFE));

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

static void shell_trigger_panic(void) {
    print_str("Triggering invalid opcode exception...\n");
    __asm__ volatile ("ud2");
}

static void shell_execute_command(void) {
    if (shell_str_equals(line_buffer, "help")) {
        shell_show_help();
        return;
    }

    if (shell_str_equals(line_buffer, "about")) {
        shell_show_about();
        return;
    }

    if (shell_str_equals(line_buffer, "version")) {
        shell_show_version();
        return;
    }

    if (shell_str_equals(line_buffer, "clear") || shell_str_equals(line_buffer, "cls")) {
        print_clear();
        return;
    }

    if (shell_str_equals(line_buffer, "ticks")) {
        shell_show_ticks();
        return;
    }

    if (shell_str_equals(line_buffer, "uptime")) {
        shell_show_uptime();
        return;
    }

    if (shell_str_starts_with(line_buffer, "echo ")) {
        shell_echo(line_buffer + 5);
        return;
    }

    if (shell_str_equals(line_buffer, "echo")) {
        print_char('\n');
        return;
    }

    if (shell_str_equals(line_buffer, "reboot")) {
        shell_reboot();
        return;
    }

    if (shell_str_equals(line_buffer, "panic")) {
        shell_trigger_panic();
        return;
    }

    print_str("Unknown command: ");
    print_str(line_buffer);
    print_char('\n');
    print_str("Type 'help' to list commands.\n");
}

void shell_init(void) {
    line_length = 0;
    cursor_pos = 0;
    line_buffer[0] = '\0';
    history_index = -1;

    print_clear();
    print_str("tiny64os ready.\n");
    print_str("Type 'help' for commands.\n\n");
    shell_prompt();
}

void shell_history_prev(void) {
    if (history_count == 0) {
        return;
    }

    if (history_index < 0) {
        history_index = (int)history_count - 1;
    } else if (history_index > 0) {
        history_index--;
    }

    shell_set_line(history[history_index]);
}

void shell_history_next(void) {
    if (history_count == 0) {
        return;
    }

    if (history_index < 0) {
        return;
    }

    if ((unsigned int)history_index + 1 < history_count) {
        history_index++;
        shell_set_line(history[history_index]);
        return;
    }

    history_index = -1;
    shell_set_line("");
}

void shell_cursor_left(void) {
    if (cursor_pos == 0) {
        return;
    }

    cursor_pos--;
    print_move_cursor_left();
}

void shell_cursor_right(void) {
    if (cursor_pos >= line_length) {
        return;
    }

    cursor_pos++;
    print_move_cursor_right();
}

void shell_cursor_home(void) {
    while (cursor_pos > 0) {
        shell_cursor_left();
    }
}

void shell_cursor_end(void) {
    while (cursor_pos < line_length) {
        shell_cursor_right();
    }
}

void shell_delete_char(void) {
    if (cursor_pos >= line_length) {
        return;
    }

    for (unsigned int i = cursor_pos; i < line_length; i++) {
        line_buffer[i] = line_buffer[i + 1];
    }

    line_length--;
    shell_redraw_line();
}

void shell_cancel_line(void) {
    print_str("^C\n");
    line_length = 0;
    cursor_pos = 0;
    line_buffer[0] = '\0';
    history_index = -1;
    shell_prompt();
}

void shell_handle_char(char c) {
    if (c == '\r') {
        return;
    }

    if (c == '\b') {
        if (cursor_pos > 0 && line_length > 0) {
            for (unsigned int i = cursor_pos - 1; i < line_length; i++) {
                line_buffer[i] = line_buffer[i + 1];
            }
            cursor_pos--;
            line_length--;
            shell_redraw_line();
        }
        history_index = -1;
        return;
    }

    if (c == '\n') {
        line_buffer[line_length] = '\0';
        print_char('\n');

        if (line_length != 0) {
            shell_store_history(line_buffer);
            shell_execute_command();
        }

        line_length = 0;
        cursor_pos = 0;
        line_buffer[0] = '\0';
        history_index = -1;
        shell_prompt();
        return;
    }

    if (line_length + 1 >= SHELL_BUFFER_SIZE) {
        return;
    }

    for (unsigned int i = line_length + 1; i > cursor_pos; i--) {
        line_buffer[i] = line_buffer[i - 1];
    }

    line_buffer[cursor_pos] = c;
    line_length++;
    cursor_pos++;
    line_buffer[line_length] = '\0';
    history_index = -1;
    shell_redraw_line();
}
