#pragma once

#include <stdint.h>

struct interrupt_frame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
};

void isr_exception_handler(uint64_t vector, uint64_t error_code, struct interrupt_frame* frame);
void irq0_handler(void);
void irq1_handler(void);
