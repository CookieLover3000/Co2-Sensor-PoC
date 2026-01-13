#include "homescreen.hpp"
#include "SensorHandler.hpp"
#include "cmsis_os2.h"
#include "custom_fonts.h"
#include "homescreen_anim.h"

#include <src/core/lv_obj_style.h>
#include <src/display/lv_display.h>
#include <src/font/lv_font.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_types.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

using namespace UI;

#define WIDGET_AMOUNT                 3
#define CO2_DANGEROUS_VALUE           1000
#define CO2_WARNING_VALUE             750
#define CO2_SENSOR_QUEUE_HANDLE_INDEX 0

Homescreen::~Homescreen()
{
    destroy();
}

void Homescreen::init(void)
{
    homescreen_screen = lv_screen_active();
    lv_obj_clear_flag(homescreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(homescreen_screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(homescreen_screen, LV_OPA_100, LV_PART_MAIN);

    init_widget(&main_widget);
    init_widget(&upper_widget);
    init_widget(&lower_widget);
}

void Homescreen::init_widget(Widget_t *widget)
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

    switch (widget->type)
    {
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
        lv_obj_set_style_arc_width(widget->arc, 8, LV_PART_MAIN);
        lv_obj_set_style_arc_width(widget->arc, 8, LV_PART_INDICATOR);
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
        lv_obj_set_style_arc_width(widget->arc, 8, LV_PART_MAIN);
        lv_obj_set_style_arc_width(widget->arc, 8, LV_PART_INDICATOR);
        lv_obj_set_x(widget->arc, -66);
        lv_obj_set_y(widget->arc, 77);
        lv_obj_set_x(widget->symbol_label, 0);
        lv_obj_set_y(widget->symbol_label, -22);
    }

    switch (widget->monitor)
    {

    case CO2:
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.co2_dangerous, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.co2_dangerous, LV_PART_MAIN);

        lv_label_set_text(widget->value_label, "1000");

        lv_label_set_text(widget->symbol_label, "PPM");

        break;

    case TEMPERATURE:
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.temperature, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.temperature, LV_PART_MAIN);
        lv_label_set_text(widget->value_label, "19.5");
        lv_label_set_text(widget->symbol_label, "°C");
        break;

    case HUMIDITY:
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.humidity, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(widget->arc, homescreen_status_colors.humidity, LV_PART_MAIN);
        lv_label_set_text(widget->value_label, "46");
        lv_label_set_text(widget->symbol_label, "%");
        break;
    }
}

void Homescreen::update_widget_label(Widget_t *widget, const char *co2, const char *temp, const char *hum,
                                     uint16_t co2_value)
{
    if (widget == NULL || widget->value_label == NULL)
        return;

    switch (widget->monitor)
    {
    case CO2:
        lv_label_set_text(widget->value_label, co2);
        if (co2_value >= CO2_DANGEROUS_VALUE &&
            !lv_color_eq(widget->active_color, homescreen_status_colors.co2_dangerous))
        {
            homescreen_anim_change_arc_color(widget->arc, widget->active_color, homescreen_status_colors.co2_dangerous,
                                             0);
            widget->active_color = homescreen_status_colors.co2_dangerous;
        }
        else if (co2_value >= CO2_WARNING_VALUE &&
                 !lv_color_eq(widget->active_color, homescreen_status_colors.co2_warning))
        {
            homescreen_anim_change_arc_color(widget->arc, widget->active_color, homescreen_status_colors.co2_warning,
                                             0);
            widget->active_color = homescreen_status_colors.co2_warning;
        }
        else if (co2_value < CO2_WARNING_VALUE && !lv_color_eq(widget->active_color, homescreen_status_colors.co2_safe))
        {
            homescreen_anim_change_arc_color(widget->arc, widget->active_color, homescreen_status_colors.co2_safe, 0);
            widget->active_color = homescreen_status_colors.co2_safe;
        }

        break;
    case TEMPERATURE:
        lv_label_set_text(widget->value_label, temp);
        break;
    case HUMIDITY:
        lv_label_set_text(widget->value_label, hum);
        break;
    }
}

extern osMessageQueueId_t handle;

void Homescreen::update()
{
    Drivers::SCD40Data received_data = {0};
    char co2_string[6];
    char temperature_string[12];
    char humidity_string[10];

    osMessageQueueId_t queueHandle = sensor.getQueueHandle(CO2_SENSOR_QUEUE_HANDLE_INDEX);

    if (queueHandle == NULL)
        return;

    if (osMessageQueueGet(queueHandle, &received_data, NULL, 0) == osOK)
    {

        uint16_t temp_int = (uint16_t)received_data.temperature;
        uint16_t temp_dec = (uint16_t)((received_data.temperature - temp_int) * 10);
        uint16_t humid_int = (uint16_t)received_data.humidity;

        snprintf(co2_string, sizeof(co2_string), "%u", received_data.co2);
        snprintf(temperature_string, sizeof(temperature_string), "%d.%d", temp_int, temp_dec);
        snprintf(humidity_string, sizeof(humidity_string), "%u", humid_int);

        Widget_t *widgets[] = {&main_widget, &upper_widget, &lower_widget};

        for (uint8_t i = 0; i < WIDGET_AMOUNT; i++)
        {
            update_widget_label(widgets[i], co2_string, temperature_string, humidity_string, received_data.co2);
        }
    }
}

__attribute__((unused)) void Homescreen::change_widget(Widget_t *widget, Monitor new_monitor)
{
    lv_color_t old_color = widget->active_color;
    widget->monitor = new_monitor;

    switch (new_monitor)
    {
    case CO2:
        homescreen_anim_update_label_text_fade(widget->value_label, "1000", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "PPM", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.co2_dangerous,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
        break;
    case TEMPERATURE:
        homescreen_anim_update_label_text_fade(widget->value_label, "19.5", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "°C", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.temperature,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
        break;
    case HUMIDITY:
        homescreen_anim_update_label_text_fade(widget->value_label, "46", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "%", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.humidity,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
    }
}

void Homescreen::destroy(void)
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