#include "quantum.h"
#include "transactions.h"
#include "timer.h"

#ifndef RGB_SYNC_RESYNC_MS
#    define RGB_SYNC_RESYNC_MS 5000u
#endif

typedef struct {
    uint32_t master_time_ms;
} rgb_sync_payload_t;

static int32_t  g_time_offset  = 0;  // slave: master_time - local_time
static uint32_t g_last_sync_at = 0;

static void rgb_sync_rpc_handler(uint8_t i2t_size,
                                 const void *i2t_buf,
                                 uint8_t t2i_size,
                                 void *t2i_buf) {
    (void)t2i_size; (void)t2i_buf; // we don't return data

    if (i2t_size < sizeof(rgb_sync_payload_t)) return;
    const rgb_sync_payload_t *p = (const rgb_sync_payload_t *)i2t_buf;

    uint32_t now = timer_read32();
    g_time_offset  = (int32_t)(p->master_time_ms - now);
    g_last_sync_at = now;
}

void rgb_sync_now(void) {
    if (!is_keyboard_master()) return;  // we only broadcast from the master
    rgb_sync_payload_t payload = { .master_time_ms = timer_read32() };
    (void)transaction_rpc_send(RGB_SYNC_TIME, sizeof(payload), &payload);
}

static inline void rgb_sync_maybe_resync(void) {
    if (!is_keyboard_master()) return;
    if (timer_elapsed32(g_last_sync_at) >= RGB_SYNC_RESYNC_MS) {
        rgb_sync_now();
    }
}

void housekeeping_task_user(void) {
    rgb_sync_maybe_resync();
}

void rgb_sync_init(void) {
    // Register our handler under the ID inserted by SPLIT_TRANSACTION_IDS_USER
    transaction_register_rpc(RGB_SYNC_TIME, rgb_sync_rpc_handler);

    if (is_keyboard_master()) {
        wait_ms(20);  // let split transport come up
        rgb_sync_now();
    }
}

uint32_t synced_timer_read32(void) {
    if (is_keyboard_master()) {
        return timer_read32();
    } else {
        return (uint32_t)((int32_t)timer_read32() + g_time_offset);
    }
}
