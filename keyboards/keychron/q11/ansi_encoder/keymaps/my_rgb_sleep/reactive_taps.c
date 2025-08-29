#include "quantum.h"
#include "transactions.h"
#include "timer.h"
#include "reactive_taps.h"

#ifndef RGB_MATRIX_LED_COUNT
#    error "reactive_taps.c requires RGB Matrix (RGB_MATRIX_ENABLE = yes)"
#endif

// --- State per LED ---
static bool     s_pressed[RGB_MATRIX_LED_COUNT];     // true while key is down
static uint32_t s_released_at[RGB_MATRIX_LED_COUNT]; // timestamp of last release (0 if never)

// RPC payload: {led, pressed}
typedef struct {
    uint8_t led_index;
    uint8_t pressed;    // 1 = key down, 0 = key up
} tap_payload_t;

// Your config.h should already include this in SPLIT_TRANSACTION_IDS_USER:
//   REACTIVE_TAP_EVENT
static void reactive_tap_handler(uint8_t i2t_size, const void *i2t_buf,
                                 uint8_t t2i_size, void *t2i_buf) {
    (void)t2i_size; (void)t2i_buf;
    if (i2t_size < sizeof(tap_payload_t)) return;
    const tap_payload_t *p = (const tap_payload_t *)i2t_buf;
    if (p->led_index >= RGB_MATRIX_LED_COUNT) return;

    if (p->pressed) {
        s_pressed[p->led_index]    = true;
        s_released_at[p->led_index] = 0;     // no decay while held
    } else {
        s_pressed[p->led_index]    = false;
        s_released_at[p->led_index] = timer_read32(); // decay starts now
    }
}

void reactive_taps_init(void) {
    transaction_register_rpc(REACTIVE_TAP_EVENT, reactive_tap_handler);
}

// Map key (row,col) to LED and update local + broadcast
static inline uint8_t map_key_to_led(uint8_t row, uint8_t col) {
    uint8_t led = g_led_config.matrix_co[row][col];
    return led; // add your nearest-LED fallback here if you want
}

void reactive_note_keyevent(uint8_t row, uint8_t col, bool pressed) {
    uint8_t led = map_key_to_led(row, col);
    if (led == NO_LED || led >= RGB_MATRIX_LED_COUNT) return;

    // local update
    if (pressed) {
        s_pressed[led] = true;
        s_released_at[led] = 0;
    } else {
        s_pressed[led] = false;
        s_released_at[led] = timer_read32();
    }

    // broadcast to other half
    tap_payload_t payload = { .led_index = led, .pressed = pressed ? 1 : 0 };
    (void)transaction_rpc_send(REACTIVE_TAP_EVENT, sizeof(payload), &payload);
}

// Convert elapsed -> heat 0..255; you can tune this to your REACTIVE_DECAY_MS
static inline uint8_t heat_from_elapsed(uint32_t elapsed_ms, uint16_t decay_ms) {
    if (elapsed_ms >= decay_ms) return 0;
    uint32_t inv = decay_ms - elapsed_ms;
    return (uint8_t)((inv * 255u) / decay_ms);
}

// Public: return “source heat” for LED (255 while held, then fades after release)
uint8_t reactive_source_heat(uint8_t led_index) {
    if (led_index >= RGB_MATRIX_LED_COUNT) return 0;
    if (s_pressed[led_index]) return 255;                         // full boost while down
    uint32_t t = s_released_at[led_index];
    if (!t) return 0;                                             // never pressed or still held on other half?
    return heat_from_elapsed(timer_elapsed32(t), 450 /*ms*/);     // match your REACTIVE_DECAY_MS
}
