#pragma once
#include <stdint.h>

void rgb_sync_init(void);     // call once at startup (both halves OK)
void rgb_sync_now(void);      // optional manual resync
uint32_t synced_timer_read32(void);  // use instead of timer_read32() in effects