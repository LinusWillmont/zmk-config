#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/keymap.h>
#include <zmk/split/central.h>

#include "os_sync.h"

#if CONFIG_NICE_VIEW_GEM_OS_LAYER >= 0

// ZMK itself forwards the raw host LED state to peripherals (on host LED reports and endpoint
// changes). We piggyback on the same channel: shortly after any of those events, and on layer
// changes, re-send the indicators with the OS bit OR'ed in. A slow periodic refresh catches a
// peripheral that rebooted or reconnected, since the central gets no event for that.

#define OS_SYNC_SETTLE_MS 50
#define OS_SYNC_REFRESH_S 10

static void os_sync_send(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(os_sync_work, os_sync_send);

static void os_sync_send(struct k_work *work) {
    zmk_hid_indicators_t indicators =
        zmk_hid_indicators_get_current_profile() & ~NICE_VIEW_GEM_OS_INDICATOR;
    if (zmk_keymap_layer_active(zmk_keymap_layer_index_to_id(CONFIG_NICE_VIEW_GEM_OS_LAYER))) {
        indicators |= NICE_VIEW_GEM_OS_INDICATOR;
    }
    int err = zmk_split_central_update_hid_indicator(indicators);
    if (err && err != -ENODEV) {
        LOG_DBG("os sync send failed: %d", err);
    }
    k_work_reschedule(&os_sync_work, K_SECONDS(OS_SYNC_REFRESH_S));
}

static int os_sync_listener(const zmk_event_t *eh) {
    k_work_reschedule(&os_sync_work, K_MSEC(OS_SYNC_SETTLE_MS));
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(os_sync, os_sync_listener);
ZMK_SUBSCRIPTION(os_sync, zmk_layer_state_changed);
ZMK_SUBSCRIPTION(os_sync, zmk_hid_indicators_changed);
ZMK_SUBSCRIPTION(os_sync, zmk_endpoint_changed);

static int os_sync_init(void) {
    k_work_schedule(&os_sync_work, K_SECONDS(2));
    return 0;
}

SYS_INIT(os_sync_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // CONFIG_NICE_VIEW_GEM_OS_LAYER >= 0
