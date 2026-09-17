#include <zephyr/kernel.h>
#include "profile.h"

#define PROFILE_COUNT CONFIG_NICE_VIEW_GEM_PROFILE_COUNT
#define PROFILE_PITCH 7
#define PROFILE_Y (129 + BUFFER_OFFSET_BOTTOM)
#define PROFILE_X0 ((68 - (PROFILE_COUNT * PROFILE_PITCH - 4)) / 2)

static void draw_inactive_profiles(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_rect_dsc_t fg_dsc;
    init_rect_dsc(&fg_dsc, LVGL_FOREGROUND);

    // same look as the original 31x3 "profiles" image: four corner pixels per slot
    for (int i = 0; i < PROFILE_COUNT; i++) {
        int x = PROFILE_X0 + i * PROFILE_PITCH;
        canvas_draw_rect(canvas, x, PROFILE_Y, 1, 1, &fg_dsc);
        canvas_draw_rect(canvas, x + 2, PROFILE_Y, 1, 1, &fg_dsc);
        canvas_draw_rect(canvas, x, PROFILE_Y + 2, 1, 1, &fg_dsc);
        canvas_draw_rect(canvas, x + 2, PROFILE_Y + 2, 1, 1, &fg_dsc);
    }
}

static void draw_active_profile(lv_obj_t *canvas, const struct status_state *state) {
    if (state->active_profile_index >= PROFILE_COUNT) {
        return;
    }
    lv_draw_rect_dsc_t fg_dsc;
    init_rect_dsc(&fg_dsc, LVGL_FOREGROUND);

    int x = PROFILE_X0 + state->active_profile_index * PROFILE_PITCH;
    canvas_draw_rect(canvas, x, PROFILE_Y, 3, 3, &fg_dsc);
}

void draw_profile_status(lv_obj_t *canvas, const struct status_state *state) {
    draw_inactive_profiles(canvas, state);
    draw_active_profile(canvas, state);
}
