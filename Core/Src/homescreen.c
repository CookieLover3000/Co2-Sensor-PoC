#include "homescreen.h"
#include "cmsis_os2.h"
#include "custom_fonts.h"
#include <src/display/lv_display.h>
#include <src/font/lv_font.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_types.h>
#include <string.h>

typedef struct {
    lv_color_t co2_dangerous;
    lv_color_t co2_warning;
    lv_color_t co2_safe;
    lv_color_t temperature;
    lv_color_t humidity;

} HomescreenColors;

const HomescreenColors homescreen_status_colors = {.co2_dangerous =
                                                        LV_COLOR_MAKE(0xEC, 0x1C, 0x24),
                                                    .co2_warning = LV_COLOR_MAKE(0xF0, 0x5A, 0x28),
                                                    .co2_safe = LV_COLOR_MAKE(0x37, 0xB3, 0x4A),
                                                    .temperature = LV_COLOR_MAKE(0xF6, 0x92, 0x1E),
                                                    .humidity = LV_COLOR_MAKE(0x00, 0xAD, 0xEE)

};

typedef enum {
    CO2,
    TEMPERATURE,
    HUMIDITY
} Monitor;

Monitor main_widget_monitor = CO2;
Monitor upper_widget_monitor = TEMPERATURE;
Monitor lower_widget_monitor = HUMIDITY;

/* LVGL widgets */
static lv_obj_t *homescreen_screen = NULL;

// co2 init
typedef struct {
    lv_obj_t *arc;
    lv_obj_t *value_label;
    lv_obj_t *symbol_label;
} WidgetData;

static WidgetData main_widget = {.arc = NULL,
                                 .value_label = NULL,
                                 .symbol_label = NULL};

static WidgetData upper_widget = {.arc = NULL,
                                   .value_label = NULL,
                                   .symbol_label = NULL};

static WidgetData lower_widget = {.arc = NULL,
                                   .value_label = NULL,
                                   .symbol_label = NULL};

/* Function prototypes */
static void homescreen_init_main(void);
static void homescreen_init_upper(void);
static void homescreen_init_lower(void);

void homescreen_init(void)
{

    homescreen_screen = lv_screen_active();
    lv_obj_clear_flag(homescreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(homescreen_screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(homescreen_screen, LV_OPA_100, LV_PART_MAIN);

    homescreen_init_main();
    homescreen_init_upper();
    homescreen_init_lower();

    // homescreen_change_main_arc_anim("19.5");
}

static void homescreen_init_main(void)
{
    // values between 400-600 are safe. Above 600 is bad and above 1000 really bad.
    main_widget.arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(main_widget.arc, 213);
    lv_obj_set_height(main_widget.arc, 213);
    lv_obj_set_x(main_widget.arc, 66);
    lv_obj_set_y(main_widget.arc, -3);
    lv_obj_set_align(main_widget.arc, LV_ALIGN_LEFT_MID);
    lv_arc_set_range(main_widget.arc, 0, 100);
    lv_arc_set_value(main_widget.arc, 100);
    lv_arc_set_bg_angles(main_widget.arc, 0, 360);
    lv_obj_remove_flag(main_widget.arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(main_widget.arc, NULL, LV_PART_KNOB);

    main_widget.value_label = lv_label_create(main_widget.arc);
    lv_obj_set_width(main_widget.value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(main_widget.value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(main_widget.value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(main_widget.value_label, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(main_widget.value_label, &custom_font_montserrat_44,
                               LV_PART_MAIN);

    main_widget.symbol_label = lv_label_create(main_widget.arc);
    lv_obj_set_width(main_widget.symbol_label, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(main_widget.symbol_label, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(main_widget.symbol_label, 0);
    lv_obj_set_y(main_widget.symbol_label, -34);
    lv_obj_set_align(main_widget.symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_text_color(main_widget.symbol_label, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_opa(main_widget.symbol_label, 255, LV_PART_MAIN);
    lv_obj_set_style_text_font(main_widget.symbol_label, &lv_font_montserrat_22,
                               LV_PART_MAIN);

    switch (main_widget_monitor) {

    case CO2:
        lv_obj_set_style_arc_color(main_widget.arc,
                                   homescreen_status_colors.co2_dangerous, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(main_widget.arc,
                                   homescreen_status_colors.co2_dangerous, LV_PART_MAIN);
        lv_label_set_text(main_widget.value_label, "1000");
        lv_label_set_text(main_widget.symbol_label, "PPM");
        break;
    case TEMPERATURE:
        lv_obj_set_style_arc_color(main_widget.arc,
                                   homescreen_status_colors.temperature, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(main_widget.arc,
                                   homescreen_status_colors.temperature, LV_PART_MAIN);
        lv_label_set_text(main_widget.value_label, "19.5");
        lv_label_set_text(main_widget.symbol_label, "°C");
        break;
    case HUMIDITY:
        lv_obj_set_style_arc_color(main_widget.arc,
                                   homescreen_status_colors.humidity, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(main_widget.arc,
                                   homescreen_status_colors.humidity, LV_PART_MAIN);
        lv_label_set_text(main_widget.value_label, "46");
        lv_label_set_text(main_widget.symbol_label, "%");
        break;
    }
}

static void homescreen_init_upper(void)
{
    upper_widget.arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(upper_widget.arc, 134);
    lv_obj_set_height(upper_widget.arc, 134);
    lv_obj_set_x(upper_widget.arc, -66);
    lv_obj_set_y(upper_widget.arc, -77);
    lv_obj_set_align(upper_widget.arc, LV_ALIGN_RIGHT_MID);
    lv_arc_set_range(upper_widget.arc, 0, 100);
    lv_arc_set_value(upper_widget.arc, 100);
    lv_arc_set_bg_angles(upper_widget.arc, 0, 360);
    lv_obj_remove_flag(upper_widget.arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(upper_widget.arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(upper_widget.arc, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_width(upper_widget.arc, 8, LV_PART_INDICATOR);

    upper_widget.value_label = lv_label_create(upper_widget.arc);
    lv_obj_set_width(upper_widget.value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(upper_widget.value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(upper_widget.value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(upper_widget.value_label, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(upper_widget.value_label,
                               &custom_font_montserrat_34, LV_PART_MAIN);

    upper_widget.symbol_label = lv_label_create(upper_widget.arc);
    lv_obj_set_width(upper_widget.symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_height(upper_widget.symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_x(upper_widget.symbol_label, 0);
    lv_obj_set_y(upper_widget.symbol_label, -22);
    lv_obj_set_align(upper_widget.symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_text_color(upper_widget.symbol_label, lv_color_white(), 0);
    lv_obj_set_style_text_opa(upper_widget.symbol_label, 255, 0);
    lv_obj_set_style_text_font(upper_widget.symbol_label,
                               &custom_font_montserrat_18, LV_PART_MAIN);

    switch (upper_widget_monitor) {

    case CO2:
        lv_obj_set_style_arc_color(upper_widget.arc,
                                   homescreen_status_colors.co2_dangerous, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(upper_widget.arc,
                                   homescreen_status_colors.co2_dangerous, LV_PART_MAIN);
        lv_label_set_text(upper_widget.value_label, "1000");
        lv_label_set_text(upper_widget.symbol_label, "PPM");
        break;
    case TEMPERATURE:
        lv_obj_set_style_arc_color(upper_widget.arc,
                                   homescreen_status_colors.temperature, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(upper_widget.arc,
                                   homescreen_status_colors.temperature, LV_PART_MAIN);
        lv_label_set_text(upper_widget.value_label, "19.5");
        lv_label_set_text(upper_widget.symbol_label, "°C");
        break;
    case HUMIDITY:
        lv_obj_set_style_arc_color(upper_widget.arc,
                                   homescreen_status_colors.humidity, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(upper_widget.arc,
                                   homescreen_status_colors.humidity, LV_PART_MAIN);
        lv_label_set_text(upper_widget.value_label, "46");
        lv_label_set_text(upper_widget.symbol_label, "%");
        break;
    }
}

static void homescreen_init_lower(void)
{
    lower_widget.arc = lv_arc_create(homescreen_screen);
    lv_obj_set_width(lower_widget.arc, 134);
    lv_obj_set_height(lower_widget.arc, 134);
    lv_obj_set_x(lower_widget.arc, -66);
    lv_obj_set_y(lower_widget.arc, 77);
    lv_obj_set_align(lower_widget.arc, LV_ALIGN_RIGHT_MID);
    lv_arc_set_range(lower_widget.arc, 0, 100);
    lv_arc_set_value(lower_widget.arc, 100);
    lv_arc_set_bg_angles(lower_widget.arc, 0, 360);
    lv_obj_remove_flag(lower_widget.arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(lower_widget.arc, NULL, LV_PART_KNOB);
    lv_obj_set_style_arc_width(lower_widget.arc, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_width(lower_widget.arc, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.humidity,
                               LV_PART_INDICATOR);

    lower_widget.value_label = lv_label_create(lower_widget.arc);
    lv_obj_set_width(lower_widget.value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(lower_widget.value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(lower_widget.value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(lower_widget.value_label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(lower_widget.value_label, "46");
    lv_obj_set_style_text_font(lower_widget.value_label, &custom_font_montserrat_34,
                               LV_PART_MAIN);

    lower_widget.symbol_label = lv_label_create(lower_widget.arc);
    lv_obj_set_width(lower_widget.symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_height(lower_widget.symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_x(lower_widget.symbol_label, 0);
    lv_obj_set_y(lower_widget.symbol_label, -22);
    lv_obj_set_align(lower_widget.symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(lower_widget.symbol_label, "%");
    lv_obj_set_style_text_color(lower_widget.symbol_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_opa(lower_widget.symbol_label, 255, 0);
    lv_obj_set_style_text_font(lower_widget.symbol_label, &custom_font_montserrat_18,
                               LV_PART_MAIN);

    switch (lower_widget_monitor) {

    case CO2:
        lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.co2_dangerous,
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.co2_dangerous,
                                   LV_PART_MAIN);
        lv_label_set_text(lower_widget.value_label, "1000");
        lv_label_set_text(lower_widget.symbol_label, "PPM");
        break;
    case TEMPERATURE:
        lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.temperature,
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.temperature,
                                   LV_PART_MAIN);
        lv_label_set_text(lower_widget.value_label, "19.5");
        lv_label_set_text(lower_widget.symbol_label, "°C");
        break;
    case HUMIDITY:
        lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.humidity,
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(lower_widget.arc, homescreen_status_colors.humidity,
                                   LV_PART_MAIN);
        lv_label_set_text(lower_widget.value_label, "46");
        lv_label_set_text(lower_widget.symbol_label, "%");
        break;
    }
}

void homescreen_destroy(void)
{
    if (homescreen_screen)
        lv_obj_del(homescreen_screen);

    homescreen_screen = NULL;
    main_widget.arc = NULL;
    main_widget.value_label = NULL;
    upper_widget.arc = NULL;
    upper_widget.value_label = NULL;
    lower_widget.arc = NULL;
    lower_widget.value_label = NULL;
}