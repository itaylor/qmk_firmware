#include QMK_KEYBOARD_H
#include "transactions.h"
#include "print.h"
#include "deferred_exec.h"
#include "split_rgb_sync.h"
#include "reactive_taps.h"


enum layers{
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
};
enum custom_keycodes {
    KC_ENC_L_TOGGLE = SAFE_RANGE,  // place this on the LEFT encoder button in your keymap
    KC_ENC_L_CCW,
    KC_ENC_L_CW
};

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_91_ansi(
        KC_ENC_L_TOGGLE,  KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RM_VALD,   RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  KC_INS,   KC_DEL,   KC_MUTE,
        _______,  KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,    KC_EQL,   KC_BSPC,            KC_PGUP,
        _______,  KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,    KC_RBRC,  KC_BSLS,            KC_PGDN,
        _______,  KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,             KC_HOME,
        _______,  KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,  KC_UP,
        _______,  KC_LCTL,  KC_LOPT,  KC_LCMD,  KC_LCMD,         KC_SPC,                        KC_SPC,             KC_RCMD,  MO(MAC_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_91_ansi(
        RM_TOGG,  _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   _______,  _______,  RM_TOGG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,            _______,
        _______,  RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,            _______,
        _______,  _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,   _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,            _______,  _______,  _______,  _______,   _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,  _______,
        _______,  _______,  _______,  _______,  _______,            _______,                       _______,            _______,  _______,    _______,  _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_91_ansi(
        KC_ENC_L_TOGGLE,  KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   KC_INS,   KC_DEL,   KC_MUTE,
        _______,  KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,    KC_EQL,   KC_BSPC,            KC_PGUP,
        _______,  KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,      KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,    KC_RBRC,  KC_BSLS,            KC_PGDN,
        _______,  KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,      KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,             KC_HOME,
        _______,  KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,      KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,  KC_UP,
        _______,  KC_LCTL,  KC_LWIN,  KC_LALT,  MO(WIN_FN),         KC_SPC,                        KC_SPC,             KC_RALT,  MO(WIN_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_91_ansi(
        RM_TOGG,  _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FLXP,  RM_VALD,   RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  _______,  _______,  RM_TOGG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,            _______,
        _______,  RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,   _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,            _______,
        _______,  _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,   _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,            _______,  _______,  _______,  _______,   _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,  _______,
        _______,  _______,  _______,  _______,  _______,            _______,                       _______,            _______,  _______,    _______,  _______,  _______,  _______),
};

static const uint8_t kColors[][3] = {
    { HSV_PINK },        // pink
    { HSV_RED },         // red
    { HSV_ORANGE },      // orange
    { HSV_GOLDENROD },   // yellow-orange
    { HSV_GOLD },        // yellow
    { HSV_YELLOW },      // yellow
    { HSV_CHARTREUSE },  // yellow-green
    { HSV_SPRINGGREEN }, // green-cyan
    { HSV_GREEN },       // green
    { HSV_TEAL },        // teal
    { HSV_TURQUOISE },   // cyan-teal
    { HSV_CYAN },        // cyan
    { HSV_AZURE },       // cyan-blue
    { HSV_BLUE },        // blue
    { HSV_PURPLE },      
    { HSV_MAGENTA },     // magenta
    
    { HSV_CORAL },       // pink-orange highlight
    // neutrals/utility at the end
    { HSV_WHITE },       
};
#define COLOR_COUNT (sizeof(kColors)/sizeof(kColors[0]))


#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [MAC_BASE] = { ENCODER_CCW_CW(KC_ENC_L_CCW, KC_ENC_L_CW), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [MAC_FN]   = { ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(RM_VALD, RM_VALU) },
    [WIN_BASE] = { ENCODER_CCW_CW(KC_ENC_L_CCW, KC_ENC_L_CW), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [WIN_FN]   = { ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(RM_VALD, RM_VALU) }
};
#endif // ENCODER_MAP_ENABLE

#include "print.h"
#include "timer.h"
#include <stdarg.h>
#include <stdio.h>

// ---------------- Tunables ----------------
#define LOG_BUF_SIZE   2048   // total bytes
#define LOG_FLUSH_MS   60000  // flush interval (60s)
// ------------------------------------------

static char     log_buf[LOG_BUF_SIZE];
static uint16_t log_head = 0;
static uint16_t log_tail = 0;


// Append formatted text into ring
void flog(const char *fmt, ...) {
    char tmp[128];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(tmp, sizeof tmp, fmt, ap);
    va_end(ap);
    if (n <= 0) return;
    if (n >= (int)sizeof tmp) n = sizeof tmp - 1;

    for (int i = 0; i < n; i++) {
        log_buf[log_head] = tmp[i];
        log_head = (log_head + 1) % LOG_BUF_SIZE;
        if (log_head == log_tail) {  // overwrite oldest
            log_tail = (log_tail + 1) % LOG_BUF_SIZE;
        }
    }
}


// Flush everything in buffer to console
static void log_flush(void) {
    while (log_tail != log_head) {
        uprintf("%c", log_buf[log_tail]);
        log_tail = (log_tail + 1) % LOG_BUF_SIZE;
    }
    uprintf("\n"); // newline after flush
}

// Timer callback to flush every LOG_FLUSH_MS
static uint32_t log_flush_cb(uint32_t t, void *cb_arg) {
    (void)t; (void)cb_arg;
    log_flush();
    return LOG_FLUSH_MS;
}


#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)


// Apply the visual state locally
void apply_sleep_visual(bool s) {
    rgb_matrix_enable_noeeprom();
    if (s) {
        flog("set sleep %s\n", is_keyboard_master() ? "m" : "s");

        // Sleep effect
        // rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_MY_SPACE_BREATHE);
        // rgb_matrix_sethsv_noeeprom(0, 0, 128); // sets global V above 0
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_MY_SPACE_BREATHE);
    } else {
        flog("set awake %s\n", is_keyboard_master() ? "m" : "s");

        // Awake look
        rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_MY_TWO_TONE_REACTIVE);
        rgb_matrix_sethsv_noeeprom(170, 255, 255); // cyan-ish

        // rgb_matrix_sethsv_noeeprom(170, 255, 255); // cyan-ish
    }
}


// Receiver: runs on the other half
static void rpc_sleep_handler(uint8_t inlen, const void *in, uint8_t outlen, void *out) {
    (void)outlen; (void)out;
    flog("RPC sleep handler: inlen=%u\n", inlen);
    if (inlen >= 1) {
        uint8_t v = *(const uint8_t*)in;
        flog("RPC sleep byte=%u\n", v);
        apply_sleep_visual(v != 0);
    }
}

void keyboard_post_init_user(void) {
    flog("init_kb");
    transaction_register_rpc(RPC_SYNC_SLEEP, rpc_sleep_handler);
    rgb_sync_init();
    apply_sleep_visual(false);
    reactive_taps_init();
    defer_exec(LOG_FLUSH_MS, log_flush_cb, NULL);
    uprintf("RPC reg %s\n", is_keyboard_master() ? "m" : "s");
}

// Helper: one-way send (no reply expected)
static inline bool send_sleep_flag(bool s) {
    flog ("RPC Noop\n");
    return true;
    // uint8_t v = s ? 1 : 0;
    // bool ok = transaction_rpc_send(RPC_SYNC_SLEEP, 1, &v);
    // flog("RPC %u -> %s\n", v, ok ? "y" : "n");
    // return ok;
}
bool suspended = false;

// Override KB-level hooks so nothing upstream undoes us.
void suspend_power_down_kb(void) {
    if (!suspended) {
        suspended = true;
        flog("%s down\n", is_keyboard_master() ? "m" : "s" );
        apply_sleep_visual(true);     // change locally (this half)
        send_sleep_flag(true);        // tell the other half
    }
}

void suspend_wakeup_init_kb(void) {
    if (suspended) {
        flog("%s up\n", is_keyboard_master() ? "m" : "s" );
        apply_sleep_visual(false);    // change locally
        send_sleep_flag(false);       // tell the other half
        suspended = false;
    }
}

static uint8_t g_color_index = 0;

static void apply_current_color_noeeprom(void) {
    rgb_matrix_sethsv_noeeprom(kColors[g_color_index][0],
                               kColors[g_color_index][1],
                               kColors[g_color_index][2]);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_ENC_L_TOGGLE:
            if (record->event.pressed) {
                if (rgb_matrix_is_enabled()) {
                    rgb_matrix_disable_noeeprom();   // off now, state not saved
                } else {
                    rgb_matrix_enable_noeeprom();    // on now, state not saved
                    apply_current_color_noeeprom();  // re-apply the last color
                }
            }
            return false; // don’t let anything else see this key
        case KC_ENC_L_CW:
            if (record->event.pressed) {
                g_color_index = (g_color_index + 1) % COLOR_COUNT;   // rainbow forward
                apply_current_color_noeeprom();
            }
            return false;
        case KC_ENC_L_CCW:
            if (record->event.pressed) {
                g_color_index = (g_color_index + COLOR_COUNT - 1) % COLOR_COUNT; // reverse
                apply_current_color_noeeprom();
            }
            return false;
    }
    reactive_note_keyevent(record->event.key.row, record->event.key.col,
                           record->event.pressed);
    return true;
}
