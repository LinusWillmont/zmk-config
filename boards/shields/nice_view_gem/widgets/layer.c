#include <zephyr/kernel.h>
#include "layer.h"
#include "../assets/custom_fonts.h"

LV_IMG_DECLARE(tux);

void draw_layer_status(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &pixel_operator_mono, LV_TEXT_ALIGN_CENTER);

    char text[10] = {};

    if (state->layer_label == NULL) {
        sprintf(text, "Layer %i", state->layer_index);
    } else {
        strncpy(text, state->layer_label, 9);
        to_uppercase(text);
    }

    canvas_draw_text(canvas, 0, 146 + BUFFER_OFFSET_BOTTOM, 68, &label_dsc, text);

    if (state->os_layer_active) {
        lv_draw_image_dsc_t img_dsc;
        lv_draw_image_dsc_init(&img_dsc);
        canvas_draw_img(canvas, 54, 124 + BUFFER_OFFSET_BOTTOM, &tux, &img_dsc);
    }
}