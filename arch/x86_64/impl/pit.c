#include "pit.h"
#include "io.h"

static volatile uint64_t pit_ticks = 0;

void pit_init(uint32_t frequency) {
    if (frequency == 0) {
        frequency = 100;
    }

    uint32_t divisor = 1193182u / frequency;

    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void pit_on_tick(void) {
    pit_ticks++;
}

uint64_t pit_get_ticks(void) {
    return pit_ticks;
}
