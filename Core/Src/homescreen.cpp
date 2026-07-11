#include "homescreen.hpp"
#include "SensorHandler.hpp"
#include "cmsis_os2.h"
#include "custom_fonts.h"
#include "homescreen_anim.h"

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
    // init screen
    homescreen_screen.emplace();

    homescreen_screen->remove_flag(LV_OBJ_FLAG_SCROLLABLE).bg_color(lv_color_black()).bg_opa(LV_OPA_100);

    while (!getSensorData())
        osDelay(10);

    init_widget(&main_widget);
    init_widget(&upper_widget);
    init_widget(&lower_widget);
}

void Homescreen::init_widget(Widget_t *widget)
{
    // values between 400-600 are good. Above 750 are fine and above 1000 is bad.
    widget->arc = lv::Arc::create(*homescreen_screen)
                      .range(0, 100)
                      .value(100)
                      .bg_angles(0, 360)
                      .remove_flag(LV_OBJ_FLAG_CLICKABLE)
                      .remove_style(nullptr, LV_PART_KNOB);

    widget->value_label = lv::Label::create(widget->arc)
                              .width(LV_SIZE_CONTENT)
                              .height(LV_SIZE_CONTENT)
                              .align(LV_ALIGN_CENTER)
                              .text_color(lv_color_white(), LV_PART_MAIN);

    widget->symbol_label = lv::Label::create(widget->arc)
                               .width(LV_SIZE_CONTENT)
                               .height(LV_SIZE_CONTENT)
                               .align(LV_ALIGN_BOTTOM_MID)
                               .text_color(lv_color_white(), LV_PART_MAIN)
                               .opa(255, LV_PART_MAIN);

    switch (widget->type)
    {
    case Settings::DisplaySettings::MAIN:
        widget->arc.align(LV_ALIGN_LEFT_MID);
        widget->value_label.text_font(&custom_font_montserrat_44, LV_PART_MAIN);
        widget->symbol_label.text_font(&custom_font_montserrat_22, LV_PART_MAIN);
        widget->arc.width(213);
        widget->arc.height(213);
        widget->arc.x(66);
        widget->arc.y(-3);
        widget->symbol_label.x(0);
        widget->symbol_label.y(-34);
        break;
    case Settings::DisplaySettings::UPPER:
        widget->arc.align(LV_ALIGN_RIGHT_MID);
        widget->value_label.text_font(&custom_font_montserrat_34, LV_PART_MAIN);
        widget->symbol_label.text_font(&custom_font_montserrat_18, LV_PART_MAIN);
        widget->arc.width(134);
        widget->arc.height(134);
        widget->arc.arc_width(8);
        widget->arc.indicator_width(8);
        widget->arc.x(-66);
        widget->arc.y(-77);
        widget->symbol_label.x(0);
        widget->symbol_label.y(-22);
        break;
    case Settings::DisplaySettings::LOWER:
        widget->arc.align(LV_ALIGN_RIGHT_MID);
        widget->value_label.text_font(&custom_font_montserrat_34, LV_PART_MAIN);
        widget->symbol_label.text_font(&custom_font_montserrat_18, LV_PART_MAIN);
        widget->arc.width(134);
        widget->arc.height(134);
        widget->arc.arc_width(8);
        widget->arc.indicator_width(8);
        widget->arc.x(-66);
        widget->arc.y(77);
        widget->symbol_label.x(0);
        widget->symbol_label.y(-22);
    }

    update_widget_monitor(widget);
}

void Homescreen::update_widget_monitor(Widget_t *widget)
{
    widget->monitor = settings.getMonitor(widget->type);

    // I don't like how I did this, but it works.
    // uint16_t temp_int = (uint16_t)sensorData.temperature;
    uint16_t temp_int = static_cast<uint16_t>(sensorData.temperature);
    uint16_t temp_dec = static_cast<uint16_t>((sensorData.temperature - temp_int) * 10);
    uint16_t humid_int = static_cast<uint16_t>(sensorData.humidity);

    char co2_string[6];
    char temperature_string[12];
    char humidity_string[10];

    snprintf(co2_string, sizeof(co2_string), "%u", sensorData.co2);
    snprintf(temperature_string, sizeof(temperature_string), "%d.%d", temp_int, temp_dec);
    snprintf(humidity_string, sizeof(humidity_string), "%u", humid_int);

    switch (widget->monitor)
    {

    case Settings::DisplaySettings::CO2:
        // I don't like how I did this, but it works.
        if (sensorData.co2 >= CO2_DANGEROUS_VALUE)
        {
            widget->arc.indicator_arc_color(homescreen_status_colors.co2_dangerous);
            widget->arc.arc_color(homescreen_status_colors.co2_dangerous);
            widget->active_color = homescreen_status_colors.co2_dangerous;
        }
        else if (sensorData.co2 >= CO2_WARNING_VALUE && sensorData.co2 < CO2_DANGEROUS_VALUE)
        {
            widget->arc.indicator_arc_color(homescreen_status_colors.co2_warning);
            widget->arc.arc_color(homescreen_status_colors.co2_warning);
            widget->active_color = homescreen_status_colors.co2_warning;
        }
        else
        {
            widget->arc.indicator_arc_color(homescreen_status_colors.co2_safe);
            widget->arc.arc_color(homescreen_status_colors.co2_safe);
            widget->active_color = homescreen_status_colors.co2_safe;
        }

        widget->value_label.text(co2_string);
        widget->symbol_label.text("PPM");

        break;

    case Settings::DisplaySettings::TEMPERATURE:

        widget->arc.indicator_arc_color(homescreen_status_colors.temperature);
        widget->arc.arc_color(homescreen_status_colors.temperature);
        widget->value_label.text(temperature_string);
        widget->symbol_label.text("°C");
        break;

    case Settings::DisplaySettings::HUMIDITY:
        widget->arc.indicator_arc_color(homescreen_status_colors.humidity);
        widget->arc.arc_color(homescreen_status_colors.humidity);
        widget->value_label.text(humidity_string);
        widget->symbol_label.text("%");
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
    case Settings::DisplaySettings::CO2:
        widget->value_label.text(co2);
        if (co2_value >= CO2_DANGEROUS_VALUE &&
            !lv_color_eq(widget->active_color, homescreen_status_colors.co2_dangerous))
        {
            homescreen_anim_change_arc_color(widget->arc, widget->active_color, homescreen_status_colors.co2_dangerous,
                                             0);
            widget->active_color = homescreen_status_colors.co2_dangerous;
        }
        else if (co2_value >= CO2_WARNING_VALUE && co2_value < CO2_DANGEROUS_VALUE &&
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
    case Settings::DisplaySettings::TEMPERATURE:
        widget->value_label.text(temp);
        break;
    case Settings::DisplaySettings::HUMIDITY:
        widget->value_label.text(hum);
        break;
    }
}

bool Homescreen::getSensorData()
{
    osMessageQueueId_t queueHandle = sensor.getQueueHandle(CO2_SENSOR_QUEUE_HANDLE_INDEX);

    Drivers::SCD40Data received_data = {0, 0, 0};
    if (osMessageQueueGet(queueHandle, &received_data, NULL, 0) == osOK)
    {
        sensorData = received_data;
        return true;
    }
    return false;
}

void Homescreen::load()
{
    update_widget_monitor(&main_widget);
    update_widget_monitor(&upper_widget);
    update_widget_monitor(&lower_widget);

    homescreen_screen->load();
}

void Homescreen::update()
{
    char co2_string[6];
    char temperature_string[12];
    char humidity_string[10];

    osMessageQueueId_t queueHandle = sensor.getQueueHandle(CO2_SENSOR_QUEUE_HANDLE_INDEX);

    if (queueHandle == NULL)
        return;

    if (getSensorData())
    {
        uint16_t temp_int = (uint16_t)sensorData.temperature;
        uint16_t temp_dec = (uint16_t)((sensorData.temperature - temp_int) * 10);
        uint16_t humid_int = (uint16_t)sensorData.humidity;

        snprintf(co2_string, sizeof(co2_string), "%u", sensorData.co2);
        snprintf(temperature_string, sizeof(temperature_string), "%d.%d", temp_int, temp_dec);
        snprintf(humidity_string, sizeof(humidity_string), "%u", humid_int);

        Widget_t *widgets[] = {&main_widget, &upper_widget, &lower_widget};

        for (uint8_t i = 0; i < WIDGET_AMOUNT; i++)
        {
            update_widget_label(widgets[i], co2_string, temperature_string, humidity_string, sensorData.co2);
        }
    }
}

[[maybe_unused]]
void Homescreen::change_widget(Widget_t *widget, Settings::DisplaySettings::Monitor new_monitor)
{
    lv_color_t old_color = widget->active_color;
    widget->monitor = new_monitor;

    switch (new_monitor)
    {
    case Settings::DisplaySettings::CO2:
        homescreen_anim_update_label_text_fade(widget->value_label, "1000", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "PPM", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.co2_dangerous,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
        break;
    case Settings::DisplaySettings::TEMPERATURE:
        homescreen_anim_update_label_text_fade(widget->value_label, "19.5", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "°C", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.temperature,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
        break;
    case Settings::DisplaySettings::HUMIDITY:
        homescreen_anim_update_label_text_fade(widget->value_label, "46", 0);
        homescreen_anim_update_label_text_fade(widget->symbol_label, "%", 0);
        homescreen_anim_change_arc_color(widget->arc, old_color, homescreen_status_colors.humidity,
                                         LABEL_FADE_ANIM_DURATION_MS + ARC_COLOR_SWAP_DELAY);
    }
}

bool Homescreen::shouldSwitch()
{
    bool ret = screenSwitch;
    if (screenSwitch)
        screenSwitch = false;
    return ret;
}

void Homescreen::handleLongPress()
{
    screenSwitch = true;
}

void Homescreen::destroy(void)
{
    screenSwitch = false;

    homescreen_screen->del();
}