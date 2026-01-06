#include "homescreen_anim.h"
#include <src/lv_api_map_v8.h>
#include <src/misc/lv_types.h>

static lv_color_t oldColorCode = LV_COLOR_MAKE(0xFF, 0x00, 0x00);
static lv_color_t newColorCode = LV_COLOR_MAKE(0x00, 0xFF, 0x00);

static void arc_color_anim_cb(void *var, int32_t v)
{
    lv_color_t c = lv_color_mix(newColorCode, oldColorCode, v);

    lv_obj_set_style_arc_color((lv_obj_t *)var, c, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color((lv_obj_t *)var, c, LV_PART_MAIN);
}

static void opacity_anim_cb(void *var, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)var, v, 0);
}

static void fade_in_after_update_cb(lv_anim_t *a)
{
    lv_obj_t *label = (lv_obj_t *)a->var;
    const char *new_text = (const char *)a->user_data;

    lv_label_set_text(label, new_text);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, label);
    lv_anim_set_values(&anim, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&anim, LABEL_FADE_ANIM_DURATION_MS);
    lv_anim_set_exec_cb(&anim, opacity_anim_cb);
    lv_anim_start(&anim);
}

void homescreen_anim_change_arc_color(lv_obj_t *arc, lv_color_t old_color, lv_color_t new_color,
                                      uint32_t delay)
{
    oldColorCode = old_color;
    newColorCode = new_color;

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, arc);
    lv_anim_set_exec_cb(&anim, arc_color_anim_cb);
    lv_anim_set_values(&anim, 0, 255);
    lv_anim_set_delay(&anim, delay);
    lv_anim_set_duration(&anim, ARC_COLOR_CHANGE_ANIM_DURATION_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_linear);

    lv_anim_start(&anim);
}

void homescreen_anim_update_label_text_fade(lv_obj_t *obj, const char *text, uint32_t delay)
{
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_values(&anim, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_duration(&anim, LABEL_FADE_ANIM_DURATION_MS);
    lv_anim_set_delay(&anim, delay);
    lv_anim_set_exec_cb(&anim, opacity_anim_cb);
    lv_anim_set_user_data(&anim, (void *)text);
    lv_anim_set_ready_cb(&anim, fade_in_after_update_cb);

    lv_anim_start(&anim);
}