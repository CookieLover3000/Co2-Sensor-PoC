#ifndef HOMESCREEN_ANIM_H
#define HOMESCREEN_ANIM_H

#include "lvgl.h"

#define LABEL_FADE_ANIM_DURATION_MS 800
#define ARC_COLOR_CHANGE_ANIM_DURATION_MS 5000
#define ARC_COLOR_SWAP_DELAY 1000

void homescreen_anim_change_arc_color(lv_obj_t *arc, lv_color_t old_color, lv_color_t new_color, uint32_t delay);
void homescreen_anim_update_label_text_fade(lv_obj_t * obj, const char * text, uint32_t delay);

#endif /* HOMESCREEN_ANIM_H */