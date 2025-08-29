#pragma once
#include <stdbool.h>
#include <stdint.h>

void reactive_taps_init(void);                                  // call once (e.g., keyboard_post_init_user)
void reactive_note_keyevent(uint8_t row, uint8_t col, bool pressed);

// For the effect loop:
uint8_t reactive_source_heat(uint8_t led_index);                // 0..255 (255 while held, then fades after release)
