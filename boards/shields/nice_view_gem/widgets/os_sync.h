#pragma once

#include <zephyr/sys/util.h>

// Spare bit in the HID LED indicator byte (bits 0-4 are NumLock/CapsLock/ScrollLock/Compose/Kana)
// used to tell the peripheral that CONFIG_NICE_VIEW_GEM_OS_LAYER is active on the central.
#define NICE_VIEW_GEM_OS_INDICATOR BIT(7)
