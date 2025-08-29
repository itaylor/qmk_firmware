// keyboards/keychron/q11/ansi_encoder/keymaps/my_rgb_sleep/rgb_matrix_user.inc

// set your measured LED indices:
#ifndef LEFT_SPACE_LED
#  define LEFT_SPACE_LED  41
#endif
#ifndef RIGHT_SPACE_LED
#  define RIGHT_SPACE_LED 82
#endif

RGB_MATRIX_EFFECT(MY_SPACE_BREATHE)   // <— no semicolon here
// ---- Declarations (effect name)

RGB_MATRIX_EFFECT(MY_TWO_TONE_REACTIVE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#include "split_rgb_sync.h"
#include "reactive_taps.h"


bool MY_SPACE_BREATHE(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Time & brightness
    uint32_t t    = synced_timer_read32();
    uint8_t  wave = sin8(t / 20);
    uint8_t  val  = 12 + scale8(wave, 180);   // 12..192 (dim white)
    RGB      c    = hsv_to_rgb((HSV){0, 0, val});

    // Clear only this chunk
    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }

    // Set only if LED is in this chunk (prevents later chunks from wiping it)
    if (LEFT_SPACE_LED  >= led_min && LEFT_SPACE_LED  < led_max) {
        rgb_matrix_set_color(LEFT_SPACE_LED,  c.r, c.g, c.b);
    }
    if (RIGHT_SPACE_LED >= led_min && RIGHT_SPACE_LED < led_max) {
        rgb_matrix_set_color(RIGHT_SPACE_LED, c.r, c.g, c.b);
    }

    return rgb_matrix_check_finished_leds(led_max);
}

// ======= CONFIGURE YOUR TWO SETS =======
// Example placeholders; replace with your LED indices for each group.
static const uint8_t SET_PRIMARY[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17,     19,
    20, 21, 22, 23,             27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80,     82, 83, 84, 85,
};
static const uint8_t SET_SECONDARY[] = { 
    18, //W
    24, //A
    25, //S
    26, //D
    81, //UP
    86, //LEFT
    87, //DOWN
    88, //RIGHT
}; 


static inline bool in_list(uint8_t led, const uint8_t *list, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) if (list[i] == led) return true;
    return false;
}

// ======= Reactive parameters (tweak to taste) =======
#define REACTIVE_DECAY_MS   450   // how long a key’s boost lasts
#define BOOST_MAX           100   // max extra brightness added (0..255)
#define BASE_V_CAP          155   // headroom for reactive pop
#define R1_SQ               36    // distance^2 thresholds in LED coord space (~units of 1)
#define R2_SQ               100
#define R3_SQ               196

// Compute distance^2 between LED i and j in LED coordinate space
static inline uint16_t dist2_leds(uint8_t i, uint8_t j) {
    int16_t dx = (int16_t)g_led_config.point[i].x - (int16_t)g_led_config.point[j].x;
    int16_t dy = (int16_t)g_led_config.point[i].y - (int16_t)g_led_config.point[j].y;
    return (uint16_t)(dx * dx + dy * dy);
}

// Convert elapsed ms -> heat 0..255 (linear fade)
static inline uint8_t heat_from_elapsed(uint32_t elapsed_ms) {
    if (elapsed_ms >= REACTIVE_DECAY_MS) return 0;
    // Map [0..REACTIVE_DECAY_MS) -> [255..~0]
    uint32_t inv = REACTIVE_DECAY_MS - elapsed_ms;
    // scale to 0..255 without float
    return (uint8_t)((inv * 255u) / REACTIVE_DECAY_MS);
}

static uint8_t compute_boost_for_led(uint8_t target) {
    uint8_t best = 0;
    for (uint8_t j = 0; j < RGB_MATRIX_LED_COUNT; j++) {
        uint8_t heat = reactive_source_heat(j);  // 0..255 (255 while held, fades after release)
        if (!heat) continue;

        uint16_t d2 = dist2_leds(target, j);

        uint8_t proximity = 0;
        if (d2 <= R1_SQ)       proximity = 255;
        else if (d2 <= R2_SQ)  proximity = 170;
        else if (d2 <= R3_SQ)  proximity = 85;
        else                   proximity = 0;

        uint8_t contrib = scale8(heat, proximity);  // 0..255
        if (contrib > best) best = contrib;
        if (best == 255) break;
    }
    return best;
}

bool MY_TWO_TONE_REACTIVE(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Base colors
    HSV prim = rgb_matrix_config.hsv; // same source as SOLID_COLOR
    bool primary_is_white = (prim.s == 0 || prim.v == 0);
    HSV sec  = primary_is_white ? (HSV){170, 255, prim.v}   // blue if primary is white
                                : (HSV){0,   0,   prim.v};  // otherwise white

    // Draw nothing outside our chunk
    for (uint8_t i = led_min; i < led_max; i++) {
        // Determine which set (if any) this LED belongs to
        bool is_primary   = in_list(i, SET_PRIMARY,   sizeof(SET_PRIMARY));
        bool is_secondary = (!is_primary) && in_list(i, SET_SECONDARY, sizeof(SET_SECONDARY));

        if (!is_primary && !is_secondary) {
            rgb_matrix_set_color(i, 0, 0, 0);
            continue;
        }

         HSV base = is_primary ? prim : sec;

        // Headroom: clamp base V so we can add boost visibly
        uint8_t v_base = base.v > BASE_V_CAP ? BASE_V_CAP : base.v;

        // Reactive brightness boost (0..255 -> 0..BOOST_MAX)
        uint8_t boost = compute_boost_for_led(i);
        uint8_t add   = scale8(boost, BOOST_MAX);

        // Final V with headroom
        uint8_t v = qadd8(v_base, add);

        RGB rgb = hsv_to_rgb((HSV){ base.h, base.s, v });
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);

    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif