#include "SettingsScreen.hpp"
#include "custom_fonts.h"
#include <DisplaySettings.hpp>
#include <cstdint>
#include <lvgl.h>
#include <src/misc/lv_anim.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_event.h>
#include <src/misc/lv_types.h>
#include <src/widgets/slider/lv_slider.h>
#include <stdio.h>

using namespace UI;

SettingsScreen::brightnessButtonCbData SettingsScreen::brightnessButtonCbDataArray[brightness_button_amount];
lv_style_t SettingsScreen::brightness_button_style_selected;
lv_style_t SettingsScreen::brightness_button_style_unselected;

void SettingsScreen::init()
{
    // screen
    settingsscreen_screen = lv_obj_create(NULL);
    lv_obj_remove_flag(settingsscreen_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(settingsscreen_screen, lv_color_black(), LV_PART_MAIN);

    lv_screen_load(settingsscreen_screen); // very important, don't forget again next time...

    initBrightnessControls();
    initButtons();
}

void SettingsScreen::initBrightnessControls()
{
    lv_style_set_bg_color(&brightness_button_style_selected, lv_color_hex(0x0076FF));
    lv_style_set_bg_color(&brightness_button_style_unselected, lv_color_black());

    // Container
    brightnessSettings.container = lv_obj_create(settingsscreen_screen);
    lv_obj_remove_style_all(brightnessSettings.container);
    lv_obj_set_width(brightnessSettings.container, 480);
    lv_obj_set_height(brightnessSettings.container, 115);
    lv_obj_set_x(brightnessSettings.container, 0);
    lv_obj_set_y(brightnessSettings.container, -103);
    lv_obj_set_align(brightnessSettings.container, LV_ALIGN_CENTER);

    brightnessSettings.panel = lv_obj_create(brightnessSettings.container);
    lv_obj_set_width(brightnessSettings.panel, 480);
    lv_obj_set_height(brightnessSettings.panel, 116);
    lv_obj_set_align(brightnessSettings.panel, LV_ALIGN_CENTER);
    lv_obj_remove_flag(brightnessSettings.panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(brightnessSettings.panel, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(brightnessSettings.panel, 255, LV_PART_MAIN);
    lv_obj_set_style_border_color(brightnessSettings.panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_border_opa(brightnessSettings.panel, 255, LV_PART_MAIN);
    lv_obj_set_style_border_width(brightnessSettings.panel, 2, LV_PART_MAIN);
    lv_obj_set_style_blend_mode(brightnessSettings.panel, LV_BLEND_MODE_NORMAL, LV_PART_MAIN);
    lv_obj_set_style_opa(brightnessSettings.panel, 200, LV_PART_MAIN);

    // Title
    brightnessSettings.title = lv_label_create(brightnessSettings.container);
    lv_obj_set_width(brightnessSettings.title, LV_SIZE_CONTENT);
    lv_obj_set_height(brightnessSettings.title, LV_SIZE_CONTENT);
    lv_obj_set_x(brightnessSettings.title, 0);
    lv_obj_set_y(brightnessSettings.title, -40);
    lv_obj_set_align(brightnessSettings.title, LV_ALIGN_CENTER);
    lv_label_set_text(brightnessSettings.title, "BRIGHTNESS");
    lv_obj_set_style_text_color(brightnessSettings.title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_opa(brightnessSettings.title, 255, LV_PART_MAIN);
    lv_obj_set_style_text_font(brightnessSettings.title, &custom_font_montserrat_22, LV_PART_MAIN);

    // Slider
    brightnessSettings.slider = lv_slider_create(brightnessSettings.container);
    lv_slider_set_mode(brightnessSettings.slider, LV_SLIDER_MODE_RANGE);
    lv_slider_set_range(brightnessSettings.slider, slider_min_value, slider_max_value);
    lv_slider_set_value(brightnessSettings.slider, (int32_t)settings.getMaxBrightness(), LV_ANIM_OFF);
    if (lv_slider_get_mode(brightnessSettings.slider) == LV_SLIDER_MODE_RANGE)
        lv_slider_set_left_value(brightnessSettings.slider, (int32_t)settings.getMinBrightness(), LV_ANIM_OFF);
    lv_obj_set_width(brightnessSettings.slider, 400);
    lv_obj_set_height(brightnessSettings.slider, 10);

    lv_obj_set_x(brightnessSettings.slider, 0);
    lv_obj_set_y(brightnessSettings.slider, -10);
    lv_obj_set_align(brightnessSettings.slider, LV_ALIGN_CENTER);

    // Buttons
    int16_t x_pos = -140;

    for (size_t i = 0; i < brightnessSettings.buttons.size(); i++)
    {
        auto &button = brightnessSettings.buttons[i];
        button.button = lv_btn_create(brightnessSettings.container);
        lv_obj_set_width(button.button, 60);
        lv_obj_set_height(button.button, 30);
        lv_obj_set_x(button.button, x_pos);
        lv_obj_set_y(button.button, 20);
        lv_obj_set_align(button.button, LV_ALIGN_CENTER);
        lv_obj_remove_flag(button.button, LV_OBJ_FLAG_SCROLLABLE);

        lv_style_t *style =
            (i == selected_button_index) ? &brightness_button_style_selected : &brightness_button_style_unselected;
        lv_obj_add_style(button.button, style, LV_PART_MAIN);

        char buffer[8];
        snprintf(buffer, sizeof(buffer), "%d Min", brightness_buttons_values[i]);

        button.label = lv_label_create(button.button);
        lv_obj_set_width(button.label, LV_SIZE_CONTENT);
        lv_obj_set_height(button.label, LV_SIZE_CONTENT);
        lv_obj_set_align(button.label, LV_ALIGN_CENTER);
        lv_label_set_text(button.label, buffer);
        lv_obj_set_style_text_font(button.label, &custom_font_montserrat_16, LV_PART_MAIN);

        brightnessButtonCbDataArray[i] = {this, i};
        lv_obj_add_event_cb(button.button, onBrightnessButtonPressed, LV_EVENT_CLICKED,
                            &brightnessButtonCbDataArray[i]);

        x_pos += 70;
    }
}

void SettingsScreen::initButtons(void)
{
    done.button = lv_btn_create(settingsscreen_screen);
    lv_obj_set_width(done.button, 70);
    lv_obj_set_height(done.button, 40);
    lv_obj_set_x(done.button, 201);
    lv_obj_set_y(done.button, 134);
    lv_obj_set_align(done.button, LV_ALIGN_CENTER);
    lv_obj_add_flag(done.button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(done.button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(done.button, onDonePressed, LV_EVENT_CLICKED, this);

    done.label = lv_label_create(done.button);
    lv_obj_set_width(done.label, LV_SIZE_CONTENT);
    lv_obj_set_height(done.label, LV_SIZE_CONTENT);
    lv_obj_set_align(done.label, LV_ALIGN_CENTER);
    lv_label_set_text(done.label, "Done");
    lv_obj_set_style_text_font(done.label, &custom_font_montserrat_22, LV_PART_MAIN);
}

void SettingsScreen::update()
{
    // polling instead of callback because this is quick enough.
    checkBrightnessSlider();
}

bool SettingsScreen::shouldSwitch()
{
    return screenSwitch;
}

void SettingsScreen::checkBrightnessSlider()
{
    int32_t min_value = lv_slider_get_left_value(brightnessSettings.slider);
    int32_t max_value = lv_slider_get_value(brightnessSettings.slider);
    settings.setMinBrightness((uint8_t)min_value);
    settings.setMaxBrightness((uint8_t)max_value);
}

void SettingsScreen::destroy()
{
    screenSwitch = false;

    if (settingsscreen_screen)
        lv_obj_del_async(settingsscreen_screen);

    brightnessSettings.container = NULL;
    brightnessSettings.panel = NULL;
    brightnessSettings.title = NULL;
    brightnessSettings.slider = NULL;
    for (auto &button : brightnessSettings.buttons)
    {
        button.button = NULL;
        button.label = NULL;
    }

    done.button = NULL;
    done.label = NULL;
}

void SettingsScreen::handleLongPress()
{
    return;
}

void SettingsScreen::onDonePressed(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code != LV_EVENT_CLICKED)
    {
        return;
    }

    SettingsScreen *instance = (SettingsScreen *)lv_event_get_user_data(e);
    if (instance)
    {
        instance->screenSwitch = true;
    }
}

void SettingsScreen::onBrightnessButtonPressed(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *clicked_btn = static_cast<lv_obj_t *>(lv_event_get_target(e));

    if (code != LV_EVENT_CLICKED)
        return;

    brightnessButtonCbData *data = static_cast<brightnessButtonCbData *>(lv_event_get_user_data(e));

    SettingsScreen *instance = data->instance;
    uint8_t index = data->index;
    if (instance == nullptr)
        return;

    instance->selected_button_index = index;

    for (auto &button : instance->brightnessSettings.buttons)
    {
        lv_obj_remove_style(button.button, &brightness_button_style_selected, LV_PART_MAIN);
        lv_obj_add_style(button.button, &brightness_button_style_unselected, LV_PART_MAIN);
    }

    lv_obj_remove_style(clicked_btn, &brightness_button_style_unselected, LV_PART_MAIN);
    lv_obj_add_style(clicked_btn, &brightness_button_style_selected, LV_PART_MAIN);

    uint32_t ms = brightness_buttons_values[index] * 60000;
    instance->settings.setDisplayOffDelay(ms);
}
