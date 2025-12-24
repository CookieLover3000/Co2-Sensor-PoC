#include "homescreen.h"
#include <src/display/lv_display.h>
#include <src/misc/lv_color.h>

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

static void homescreen_init_co2(void);
static void homescreen_init_temp(void);
static void homescreen_init_humidity(void);

void homescreen_init(void)
{

    homescreen_screen = lv_screen_active();
    lv_obj_clear_flag(homescreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(homescreen_screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(homescreen_screen, LV_OPA_100, LV_PART_MAIN);

    homescreen_init_co2();
    homescreen_init_temp();
    homescreen_init_humidity();
}

static void homescreen_init_co2(void)
{
    // values between 400-600 are safe. Above 600 is bad and above 1000 really bad.
    homescreen_co2_arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(homescreen_co2_arc, 213);
    lv_obj_set_height(homescreen_co2_arc, 213);
    lv_obj_set_x(homescreen_co2_arc, 66);
    lv_obj_set_y(homescreen_co2_arc, -3);
    lv_obj_set_align(homescreen_co2_arc, LV_ALIGN_LEFT_MID);
    lv_arc_set_range(homescreen_co2_arc, 0, 100);
    lv_arc_set_value(homescreen_co2_arc, 100);
    lv_arc_set_bg_angles(homescreen_co2_arc, 0, 360);
    lv_obj_remove_flag(homescreen_co2_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(homescreen_co2_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_color(homescreen_co2_arc, lv_color_hex(0xEC1C24), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(homescreen_co2_arc, lv_color_hex(0xEC1C24), LV_PART_MAIN);

    homescreen_co2_value_label = lv_label_create(homescreen_co2_arc);
    lv_obj_set_width(homescreen_co2_value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_co2_value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(homescreen_co2_value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(homescreen_co2_value_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(homescreen_co2_value_label, "1000");
    lv_obj_set_style_text_font(homescreen_co2_value_label, &lv_font_montserrat_44, LV_PART_MAIN);

    homescreen_co2_symbol_label = lv_label_create(homescreen_co2_arc);
    lv_obj_set_width(homescreen_co2_symbol_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(homescreen_co2_symbol_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(homescreen_co2_symbol_label, 0);
    lv_obj_set_y(homescreen_co2_symbol_label, -34);
    lv_obj_set_align(homescreen_co2_symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(homescreen_co2_symbol_label, "PPM");
    lv_obj_set_style_text_color(homescreen_co2_symbol_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_opa(homescreen_co2_symbol_label, 255, LV_PART_MAIN);
    lv_obj_set_style_text_font(homescreen_co2_symbol_label, &lv_font_montserrat_18, LV_PART_MAIN);
}

static void homescreen_init_temp(void)
{
    homescreen_temp_arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(homescreen_temp_arc, 134);
    lv_obj_set_height(homescreen_temp_arc, 134);
    lv_obj_set_x(homescreen_temp_arc, -66);
    lv_obj_set_y(homescreen_temp_arc, -77);
    lv_obj_set_align(homescreen_temp_arc, LV_ALIGN_RIGHT_MID);
    lv_arc_set_range(homescreen_temp_arc, 0, 100);
    lv_arc_set_value(homescreen_temp_arc, 100);
    lv_arc_set_bg_angles(homescreen_temp_arc, 0, 360);
    lv_obj_remove_flag(homescreen_temp_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(homescreen_temp_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(homescreen_temp_arc, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_width(homescreen_temp_arc, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(homescreen_temp_arc, lv_color_hex(0xF6921E), LV_PART_INDICATOR);

    homescreen_temp_value_label = lv_label_create(homescreen_temp_arc);
    lv_obj_set_width(homescreen_temp_value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_temp_value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(homescreen_temp_value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(homescreen_temp_value_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(homescreen_temp_value_label, "19.5");
    lv_obj_set_style_text_font(homescreen_temp_value_label, &lv_font_montserrat_34, LV_PART_MAIN);

    homescreen_temp_symbol_label = lv_label_create(homescreen_temp_arc);
    lv_obj_set_width(homescreen_temp_symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_temp_symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_x(homescreen_temp_symbol_label, 0);
    lv_obj_set_y(homescreen_temp_symbol_label, -22);
    lv_obj_set_align(homescreen_temp_symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(homescreen_temp_symbol_label, "°C");
    lv_obj_set_style_text_color(homescreen_temp_symbol_label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(homescreen_temp_symbol_label, 255, 0);
    lv_obj_set_style_text_font(homescreen_temp_symbol_label, &lv_font_montserrat_14, LV_PART_MAIN);
}

static void homescreen_init_humidity(void)
{
    homescreen_humidity_arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(homescreen_humidity_arc, 134);
    lv_obj_set_height(homescreen_humidity_arc, 134);
    lv_obj_set_x(homescreen_humidity_arc, -66);
    lv_obj_set_y(homescreen_humidity_arc, 77);
    lv_obj_set_align(homescreen_humidity_arc, LV_ALIGN_RIGHT_MID);
    lv_arc_set_range(homescreen_humidity_arc, 0, 100);
    lv_arc_set_value(homescreen_humidity_arc, 100);
    lv_arc_set_bg_angles(homescreen_humidity_arc, 0, 360);
    lv_obj_remove_flag(homescreen_humidity_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(homescreen_humidity_arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(homescreen_humidity_arc, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_width(homescreen_humidity_arc, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(homescreen_humidity_arc, lv_color_hex(0x00ADEE), LV_PART_INDICATOR);

    homescreen_humidity_value_label = lv_label_create(homescreen_humidity_arc);
    lv_obj_set_width(homescreen_humidity_value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_humidity_value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(homescreen_humidity_value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(homescreen_humidity_value_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(homescreen_humidity_value_label, "46");
    lv_obj_set_style_text_font(homescreen_humidity_value_label, &lv_font_montserrat_34,
                               LV_PART_MAIN);

    homescreen_humidity_symbol_label = lv_label_create(homescreen_humidity_arc);
    lv_obj_set_width(homescreen_humidity_symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_height(homescreen_humidity_symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_x(homescreen_humidity_symbol_label, 0);
    lv_obj_set_y(homescreen_humidity_symbol_label, -22);
    lv_obj_set_align(homescreen_humidity_symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(homescreen_humidity_symbol_label, "%");
    lv_obj_set_style_text_color(homescreen_humidity_symbol_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_opa(homescreen_humidity_symbol_label, 255, 0);
    lv_obj_set_style_text_font(homescreen_humidity_symbol_label, &lv_font_montserrat_14,
                               LV_PART_MAIN);
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