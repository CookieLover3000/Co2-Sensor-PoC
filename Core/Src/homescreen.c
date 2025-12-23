#include "homescreen.h"

static lv_obj_t *homescreen_screen = NULL;

static lv_obj_t *homescreen_co2_arc = NULL;
lv_obj_t *homescreen_co2_value_label = NULL;
static lv_obj_t *homescreen_co2_symbol_label = NULL;

static lv_obj_t *homescreen_temp_arc = NULL;
lv_obj_t *homescreen_temp_value_label = NULL;
lv_obj_t *homescreen_temp_symbol_label = NULL;

static lv_obj_t *homescreen_humidity_arc = NULL;
lv_obj_t *homescreen_humidity_value_label = NULL;
lv_obj_t *homescreen_humidity_symbol_label = NULL;

void homescreen_init_co2(void);
void homescreen_init_temp(void);
void homescreen_init_humidity(void);

void homescreen_init(void)
{
    homescreen_screen = lv_screen_active();
    lv_obj_clear_flag(homescreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(homescreen_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(homescreen_screen, LV_OPA_100, 0);

    homescreen_init_co2();
    homescreen_init_temp();
    homescreen_init_humidity();
}

void homescreen_init_co2(void)
{
    homescreen_co2_arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(homescreen_co2_arc, 200);
    lv_obj_set_height(homescreen_co2_arc, 200);
    lv_obj_set_x(homescreen_co2_arc, 60);
    lv_obj_set_y(homescreen_co2_arc, 0);
    lv_obj_set_align(homescreen_co2_arc, LV_ALIGN_LEFT_MID);
    lv_arc_set_range(homescreen_co2_arc, 0, 100);
    lv_arc_set_value(homescreen_co2_arc, 100);
    lv_arc_set_bg_angles(homescreen_co2_arc, 0, 360);
    lv_obj_remove_flag(homescreen_co2_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(homescreen_co2_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(homescreen_co2_arc, lv_color_hex(0x00FF0F), LV_PART_INDICATOR | LV_STATE_DEFAULT);

    homescreen_co2_value_label = lv_label_create(homescreen_co2_arc);
    lv_obj_set_width(homescreen_co2_value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_co2_value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(homescreen_co2_value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(homescreen_co2_value_label, lv_color_white(), 0);
    lv_label_set_text(homescreen_co2_value_label, "1000");
    lv_obj_set_style_text_font(homescreen_co2_value_label, &lv_font_montserrat_24, 0);
    // lv_obj_set_style_text_font(homescreen_co2_value_label, &lv_font_montserrat_44, LV_PART_MAIN | LV_STATE_DEFAULT);

    homescreen_co2_symbol_label = lv_label_create(homescreen_co2_arc);
    lv_obj_set_width(homescreen_co2_symbol_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(homescreen_co2_symbol_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(homescreen_co2_symbol_label, 0);
    lv_obj_set_y(homescreen_co2_symbol_label, -30);
    lv_obj_set_align(homescreen_co2_symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(homescreen_co2_symbol_label, "PPM");
    lv_obj_set_style_text_color(homescreen_co2_symbol_label, lv_color_hex(0x00FF0F),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(homescreen_co2_symbol_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void homescreen_init_temp()
{
    homescreen_temp_arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(homescreen_temp_arc, 150);
    lv_obj_set_height(homescreen_temp_arc, 150);
    lv_obj_set_x(homescreen_temp_arc, -60);
    lv_obj_set_y(homescreen_temp_arc, -80);
    lv_obj_set_align(homescreen_temp_arc, LV_ALIGN_RIGHT_MID);
    lv_arc_set_range(homescreen_temp_arc, 0, 100);
    lv_arc_set_value(homescreen_temp_arc, 100);
    lv_arc_set_bg_angles(homescreen_temp_arc, 0, 360);
    lv_obj_remove_flag(homescreen_temp_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(homescreen_temp_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(homescreen_temp_arc, lv_color_hex(0x008DFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);

    homescreen_temp_value_label = lv_label_create(homescreen_temp_arc);
    lv_obj_set_width(homescreen_temp_value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_temp_value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(homescreen_temp_value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(homescreen_temp_value_label, lv_color_white(), 0);
    lv_label_set_text(homescreen_temp_value_label, "28");
    // lv_obj_set_style_text_font(homescreen_temp_value_label, &lv_font_montserrat_44, LV_PART_MAIN | LV_STATE_DEFAULT);

    homescreen_temp_symbol_label = lv_label_create(homescreen_temp_arc);
    lv_obj_set_width(homescreen_temp_symbol_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(homescreen_temp_symbol_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(homescreen_temp_symbol_label, 0);
    lv_obj_set_y(homescreen_temp_symbol_label, -20);
    lv_obj_set_align(homescreen_temp_symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(homescreen_temp_symbol_label, "°C");
    lv_obj_set_style_text_color(homescreen_temp_symbol_label, lv_color_hex(0xFF9700),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(homescreen_temp_symbol_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void homescreen_init_humidity(void)
{
    homescreen_humidity_arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(homescreen_humidity_arc, 150);
    lv_obj_set_height(homescreen_humidity_arc, 150);
    lv_obj_set_x(homescreen_humidity_arc, -60);
    lv_obj_set_y(homescreen_humidity_arc, 80);
    lv_obj_set_align(homescreen_humidity_arc, LV_ALIGN_RIGHT_MID);
    lv_arc_set_range(homescreen_humidity_arc, 0, 100);
    lv_arc_set_value(homescreen_humidity_arc, 100);
    lv_arc_set_bg_angles(homescreen_humidity_arc, 0, 360);
    lv_obj_remove_flag(homescreen_humidity_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(homescreen_humidity_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(homescreen_humidity_arc, lv_color_hex(0xFF9700), LV_PART_INDICATOR | LV_STATE_DEFAULT);

    homescreen_humidity_value_label = lv_label_create(homescreen_humidity_arc);
    lv_obj_set_width(homescreen_humidity_value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_humidity_value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(homescreen_humidity_value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(homescreen_humidity_value_label, lv_color_white(), 0);
    lv_label_set_text(homescreen_humidity_value_label, "46");
    // lv_obj_set_style_text_font(homescreen_humidity_value_label, &lv_font_montserrat_44, LV_PART_MAIN | LV_STATE_DEFAULT);

    homescreen_humidity_symbol_label = lv_label_create(homescreen_humidity_arc);
    lv_obj_set_width(homescreen_humidity_symbol_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(homescreen_humidity_symbol_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(homescreen_humidity_symbol_label, 0);
    lv_obj_set_y(homescreen_humidity_symbol_label, -20);
    lv_obj_set_align(homescreen_humidity_symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(homescreen_humidity_symbol_label, "%");
    lv_obj_set_style_text_color(homescreen_humidity_symbol_label, lv_color_hex(0xFF9700),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(homescreen_humidity_symbol_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void homescreen_destroy(void)
{
    if (homescreen_screen)
        lv_obj_del(homescreen_screen);

    homescreen_screen = NULL;
    homescreen_co2_arc = NULL;
    homescreen_co2_value_label = NULL;
    homescreen_temp_arc = NULL;
    homescreen_temp_value_label = NULL;
    homescreen_humidity_arc = NULL;
    homescreen_humidity_value_label = NULL;
}
