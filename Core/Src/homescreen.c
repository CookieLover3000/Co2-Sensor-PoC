#include "homescreen.h"
#include "cmsis_os2.h"
#include "custom_fonts.h"
#include "homescreen_anim.h"
#include "scd40.h"

#include <src/display/lv_display.h>
#include <src/font/lv_font.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_types.h>
#include <string.h>

#define WIDGET_AMOUNT 3

extern osMessageQueueId_t scd40QueueHandle;

typedef struct {
    const lv_color_t co2_dangerous;
    const lv_color_t co2_warning;
    const lv_color_t co2_safe;
    const lv_color_t temperature;
    const lv_color_t humidity;

} HomescreenColors;

static const HomescreenColors homescreen_status_colors = {
    .co2_dangerous = LV_COLOR_MAKE(0xEC, 0x1C, 0x24),
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

typedef enum {
    MAIN,
    UPPER,
    LOWER
} WidgetType;

/* LVGL widgets */
static lv_obj_t *homescreen_screen = NULL;

// co2 init
typedef struct {
    lv_obj_t *arc;
    lv_obj_t *value_label;
    lv_obj_t *symbol_label;
    WidgetType type;
    Monitor monitor;
    lv_color_t active_color;
} Widget_t;

static Widget_t main_widget = {.arc = NULL,
                               .value_label = NULL,
                               .symbol_label = NULL,
                               .type = MAIN,
                               .monitor = CO2,
                               .active_color = homescreen_status_colors.co2_dangerous};
static Widget_t upper_widget = {.arc = NULL,
                                .value_label = NULL,
                                .symbol_label = NULL,
                                .type = UPPER,
                                .monitor = TEMPERATURE,
                                .active_color = homescreen_status_colors.temperature};
static Widget_t lower_widget = {.arc = NULL,
                                .value_label = NULL,
                                .symbol_label = NULL,
                                .type = LOWER,
                                .monitor = HUMIDITY,
                                .active_color = homescreen_status_colors.humidity};

/* Function prototypes */
static void homescreen_init_widget(Widget_t *widget);
static void update_widget_label(Widget_t *widget, const char *co2, const char *temp,
                                const char *hum);
// static void homescreen_change_widget(WidgetData *widget, Monitor new_monitor);

void homescreen_init(void)
{
    homescreen_screen = lv_screen_active();
    lv_obj_clear_flag(homescreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(homescreen_screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(homescreen_screen, LV_OPA_100, LV_PART_MAIN);

    homescreen_init_widget(&main_widget);
    homescreen_init_widget(&upper_widget);
    homescreen_init_widget(&lower_widget);
}

static void homescreen_init_widget(Widget_t *widget)
{
    // values between 400-600 are safe. Above 600 is bad and above 1000 really bad.
    widget->arc = lv_arc_create(homescreen_screen);

    lv_arc_set_range(widget->arc, 0, 100);
    lv_arc_set_value(widget->arc, 100);
    lv_arc_set_bg_angles(widget->arc, 0, 360);
    lv_obj_remove_flag(widget->arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(widget->arc, NULL, LV_PART_KNOB);

    widget->value_label = lv_label_create(widget->arc);
    lv_obj_set_width(widget->value_label, LV_SIZE_CONTENT);
    lv_obj_set_height(widget->value_label, LV_SIZE_CONTENT);
    lv_obj_set_align(widget->value_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(widget->value_label, lv_color_white(), LV_PART_MAIN);

    widget->symbol_label = lv_label_create(widget->arc);
    lv_obj_set_width(widget->symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_height(widget->symbol_label, LV_SIZE_CONTENT);
    lv_obj_set_align(widget->symbol_label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_text_color(widget->symbol_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_opa(widget->symbol_label, 255, LV_PART_MAIN);

    switch (widget->type) {
    case MAIN:
        lv_obj_set_align(widget->arc, LV_ALIGN_LEFT_MID);
        lv_obj_set_style_text_font(widget->value_label, &custom_font_montserrat_44, LV_PART_MAIN);
        lv_obj_set_style_text_font(widget->symbol_label, &custom_font_montserrat_22, LV_PART_MAIN);
        lv_obj_set_width(widget->arc, 213);
        lv_obj_set_height(widget->arc, 213);
        lv_obj_set_x(widget->arc, 66);
        lv_obj_set_y(widget->arc, -3);
        lv_obj_set_x(widget->symbol_label, 0);
        lv_obj_set_y(widget->symbol_label, -34);
        break;
    case UPPER:
        lv_obj_set_align(widget->arc, LV_ALIGN_RIGHT_MID);
        lv_obj_set_style_text_font(widget->value_label, &custom_font_montserrat_34, LV_PART_MAIN);
        lv_obj_set_style_text_font(widget->symbol_label, &custom_font_montserrat_18, LV_PART_MAIN);
        lv_obj_set_width(widget->arc, 134);
        lv_obj_set_height(widget->arc, 134);
        lv_obj_set_x(widget->arc, -66);
        lv_obj_set_y(widget->arc, -77);
        lv_obj_set_x(widget->symbol_label, 0);
        lv_obj_set_y(widget->symbol_label, -22);
        break;
    case LOWER:
        lv_obj_set_align(widget->arc, LV_ALIGN_RIGHT_MID);
        lv_obj_set_style_text_font(widget->value_label, &custom_font_montserrat_34, LV_PART_MAIN);
        lv_obj_set_style_text_font(widget->symbol_label, &custom_font_montserrat_18, LV_PART_MAIN);
        lv_obj_set_width(widget->arc, 134);
        lv_obj_set_height(widget->arc, 134);
        lv_obj_set_x(widget->arc, -66);
        lv_obj_set_y(widget->arc, 77);
        lv_obj_set_x(widget->symbol_label, 0);
        lv_obj_set_y(widget->symbol_label, -22);
    }

    switch (widget->monitor) {

    case CO2:
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.co2_dangerous,
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.co2_dangerous,
                                   LV_PART_MAIN);

        lv_label_set_text(widget->value_label, "1000");

        lv_label_set_text(widget->symbol_label, "PPM");

        break;

    case TEMPERATURE:
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.temperature,
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.temperature, LV_PART_MAIN);
        lv_label_set_text(widget->value_label, "19.5");
        lv_label_set_text(widget->symbol_label, "°C");
        break;

    case HUMIDITY:
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.humidity,
                                   LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.humidity, LV_PART_MAIN);
        lv_label_set_text(widget->value_label, "46");
        lv_label_set_text(widget->symbol_label, "%");
        break;
    }
}

static void update_widget_label(Widget_t *widget, const char *co2, const char *temp,
                                const char *hum)
{
    if (widget == NULL || widget->value_label == NULL)
        return;

    switch (widget->monitor) {
    case CO2:
        lv_label_set_text(widget->value_label, co2);
        break;
    case TEMPERATURE:
        lv_label_set_text(widget->value_label, temp);
        break;
    case HUMIDITY:
        lv_label_set_text(widget->value_label, hum);
        break;
    }
}

void homescreen_update_sensor_values()
{
    scd40Data_t receivedData = {0};
    char co2_string[6];
    char temperature_string[12];
    char humidity_string[10];

    if (scd40QueueHandle == NULL)
        return;

    if (osMessageQueueGet(scd40QueueHandle, &receivedData, NULL, 0) == osOK) {

        uint16_t temp_int = (uint16_t)receivedData.temperature;
        uint16_t temp_dec = (uint16_t)((receivedData.temperature - temp_int) * 10);
        uint16_t humid_int = (uint16_t)receivedData.humidity;

        snprintf(co2_string, sizeof(co2_string), "%u", receivedData.co2);
        snprintf(temperature_string, sizeof(temperature_string), "%d.%d", temp_int, temp_dec);
        snprintf(humidity_string, sizeof(humidity_string), "%u", humid_int);

        Widget_t *widgets[] = {&main_widget, &upper_widget, &lower_widget};

        for (uint8_t i = 0; i < WIDGET_AMOUNT; i++) {
            update_widget_label(widgets[i], co2_string, temperature_string, humidity_string);
        }
    }
}

static void homescreen_change_widget(Widget_t *widget, Monitor new_monitor)
{
    lv_color_t old_color = widget->active_color;
    widget->monitor = new_monitor;

    switch (new_monitor) {
    case CO2:
        homescreen_anim_update_label_text_fade(widget->value_label, "1000", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "PPM", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color,
                                         homescreen_status_colors.co2_dangerous,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
        break;
    case TEMPERATURE:
        homescreen_anim_update_label_text_fade(widget->value_label, "19.5", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "°C", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color,
                                         homescreen_status_colors.temperature,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
        break;
    case HUMIDITY:
        homescreen_anim_update_label_text_fade(widget->value_label, "46", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "%", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.humidity,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
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