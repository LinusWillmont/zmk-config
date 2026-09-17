#pragma once

#include <lvgl.h>
#include "util.h"
#include "screen_peripheral.h"

void draw_animation(lv_obj_t *canvas);

// Swap the gem for Tux (and back). No-op if already showing the requested art.
void set_os_art(lv_obj_t *parent, bool os);
