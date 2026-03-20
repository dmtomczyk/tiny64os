#pragma once

void shell_init(void);
void shell_handle_char(char c);
void shell_history_prev(void);
void shell_history_next(void);
void shell_cursor_left(void);
void shell_cursor_right(void);
void shell_cursor_home(void);
void shell_cursor_end(void);
void shell_delete_char(void);
void shell_cancel_line(void);
