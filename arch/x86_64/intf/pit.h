#pragma once

#include <stdint.h>

void pit_init(uint32_t frequency);
void pit_on_tick(void);
uint64_t pit_get_ticks(void);
