#include "SettingsScreen.hpp"
#include "custom_fonts.h"
#include <DisplaySettings.hpp>
#include <cstdint>
#include <lv/lv.hpp>
#include <stdio.h>

using namespace UI;

void SettingsScreen::init()
{
    // init screen
    settingsscreen_screen.emplace();

    settingsscreen_screen->remove_flag(LV_OBJ_FLAG_SCROLLABLE).bg_color(lv::colors::black(), LV_PART_MAIN);

    initWidget(&mainWidget);
    initWidget(&upperWidget);
    initWidget(&lowerWidget);
    initBrightnessControls();
    initButtons();
}

void SettingsScreen::initBrightnessControls()
{
    brightness_button_style_selected.bg_color(lv::rgb(0x0076FF));
    brightness_button_style_unselected.bg_color(lv::colors::black());

    // Container
    brightnessSettings.container = lv::Box::create(*settingsscreen_screen)
                                       .remove_all_styles()
                                       .width(480)
                                       .height(115)
                                       .x(0)
                                       .y(-103)
                                       .align(LV_ALIGN_CENTER)
                                       .add_flag(LV_OBJ_FLAG_HIDDEN);

    brightnessSettings.panel = lv::Box::create(brightnessSettings.container)
                                   .width(480)
                                   .height(116)
                                   .align(LV_ALIGN_CENTER)
                                   .remove_flag(LV_OBJ_FLAG_SCROLLABLE)
                                   .bg_color(lv::rgb(0x000000), LV_PART_MAIN)
                                   .bg_opa(255, LV_PART_MAIN)
                                   .border_color(lv::rgb(0xFFFFFF), LV_PART_MAIN)
                                   .border_opa(255, LV_PART_MAIN)
                                   .border_width(2, LV_PART_MAIN)
                                   .blend_mode(LV_BLEND_MODE_NORMAL, LV_PART_MAIN)
                                   .opa(200, LV_PART_MAIN);

    // Title
    brightnessSettings.title = lv::Label::create(brightnessSettings.container)
                                   .width(LV_SIZE_CONTENT)
                                   .height(LV_SIZE_CONTENT)
                                   .x(0)
                                   .y(-40)
                                   .align(LV_ALIGN_CENTER)
                                   .text("BRIGHTNESS")
                                   .text_color(lv::rgb(0xFFFFFF), LV_PART_MAIN)
                                   .opa(255, LV_PART_MAIN)
                                   .text_font(&custom_font_montserrat_22, LV_PART_MAIN);

    // Slider
    brightnessSettings.slider = lv::Slider::create(brightnessSettings.container)
                                    .mode(LV_SLIDER_MODE_RANGE)
                                    .range(slider_min_value, slider_max_value)
                                    .value(static_cast<int32_t>(settings.getMaxBrightness())) // without animation
                                    .width(400)
                                    .height(10)
                                    .x(0)
                                    .y(-10)
                                    .align(LV_ALIGN_CENTER);

    if (lv_slider_get_mode(brightnessSettings.slider) == LV_SLIDER_MODE_RANGE)
    {
        brightnessSettings.slider.left_value(static_cast<int32_t>(settings.getMinBrightness()));
    }

    // Buttons
    int16_t x_pos = -140;

    for (uint8_t i = 0; i < brightnessSettings.buttons.size(); i++)
    {
        auto &button = brightnessSettings.buttons[i];
        button.button = lv::Button::create(brightnessSettings.container)
                            .width(60)
                            .height(30)
                            .x(x_pos)
                            .y(20)
                            .align(LV_ALIGN_CENTER)
                            .remove_flag(LV_OBJ_FLAG_SCROLLABLE);

        lv::Style &style =
            (i == selected_button_index) ? brightness_button_style_selected : brightness_button_style_unselected;
        button.button.add_style(style, LV_PART_MAIN);

        char buffer[8];
        snprintf(buffer, sizeof(buffer), "%d Min", brightness_buttons_values[i]);

        button.label = lv::Label::create(button.button)
                           .width(LV_SIZE_CONTENT)
                           .height(LV_SIZE_CONTENT)
                           .align(LV_ALIGN_CENTER)
                           .text(buffer)
                           .text_font(&custom_font_montserrat_16, LV_PART_MAIN);

        brightnessButtonCbDataArray[i] = i;
        button.button.user_data(&brightnessButtonCbDataArray[i])
            .on_click<&UI::SettingsScreen::onBrightnessButtonPressed>(this);

        x_pos += 70;
    }
}

void SettingsScreen::initWidget(Widget_t *widget)
{

    widget->arc = lv::Arc::create(*settingsscreen_screen)
                      .bg_opa(0, LV_PART_KNOB)
                      .indicator_color(Widget_t::active_color)
                      .indicator_width(10)
                      .bg_angles(0, 360)
                      .value(100)
                      .remove_flag(LV_OBJ_FLAG_CLICKABLE);

    widget->roller = lv::Roller::create(widget->arc)
                         .options("CO2\n°C\nRH", LV_ROLLER_MODE_NORMAL)
                         .align(LV_ALIGN_CENTER)
                         .text_font(&custom_font_montserrat_26, LV_PART_MAIN)
                         .bg_color(lv::rgb(0x000000), LV_PART_MAIN)
                         .bg_opa(255, LV_PART_MAIN)
                         .bg_color(lv::rgb(0x000000), LV_PART_SELECTED)
                         .bg_opa(255, LV_PART_SELECTED);

    uint16_t selected_index = settings.getMonitor(widget->type);
    widget->roller.selected(selected_index);

    switch (widget->type)
    {
    case Settings::DisplaySettings::CO2:
        widget->arc.width(213).height(213).x(66).y(-3).align(LV_ALIGN_LEFT_MID).arc_width(10);

        widget->roller.width(180).height(40);
        break;
    case Settings::DisplaySettings::TEMPERATURE:
        widget->arc.width(134).height(134).x(-66).y(-77).align(LV_ALIGN_RIGHT_MID).arc_width(8);

        widget->roller.width(100).height(40);
        break;
    case Settings::DisplaySettings::HUMIDITY:
        widget->arc.width(134).height(134).x(-66).y(77).align(LV_ALIGN_RIGHT_MID).arc_width(8);

        widget->roller.width(100).height(40);
        break;
    }
}

void SettingsScreen::initButtons(void)
{
    done.button = lv::Button::create(*settingsscreen_screen)
                      .width(70)
                      .height(40)
                      .x(201)
                      .y(134)
                      .align(LV_ALIGN_CENTER)
                      .add_flag(LV_OBJ_FLAG_SCROLL_ON_FOCUS)
                      .remove_flag(LV_OBJ_FLAG_SCROLLABLE)
                      .on_click<&UI::SettingsScreen::onDonePressed>(this);

    done.label = lv::Label::create(done.button)
                     .width(LV_SIZE_CONTENT)
                     .height(LV_SIZE_CONTENT)
                     .align(LV_ALIGN_CENTER)
                     .text("Done")
                     .text_font(&custom_font_montserrat_22, LV_PART_MAIN);

    brightnessMenu.button = lv::Button::create(*settingsscreen_screen)
                                .width(70)
                                .height(40)
                                .x(-201)
                                .y(134)
                                .align(LV_ALIGN_CENTER)
                                .add_flag(LV_OBJ_FLAG_SCROLL_ON_FOCUS)
                                .remove_flag(LV_OBJ_FLAG_SCROLLABLE)
                                .on_click<&UI::SettingsScreen::onBrightnessMenuPressed>(this);

    brightnessMenu.label = lv::Label::create(brightnessMenu.button)
                               .width(LV_SIZE_CONTENT)
                               .height(LV_SIZE_CONTENT)
                               .align(LV_ALIGN_CENTER)
                               .text("🔆")
                               .text_font(&brightness_symbol_22, LV_PART_MAIN);
}

void SettingsScreen::toggleBrightnessControls()
{
    if (brightnessSettings.container.has_flag(LV_OBJ_FLAG_HIDDEN))
    {
        brightnessSettings.container.remove_flag(LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        brightnessSettings.container.add_flag(LV_OBJ_FLAG_HIDDEN);
    }
}

void SettingsScreen::load()
{
    settingsscreen_screen->load();
}

void SettingsScreen::update()
{
    // polling instead of callback because this is quick enough and I'm lazy.
    checkBrightnessSlider();
    checkRollerValue(&mainWidget);
    checkRollerValue(&upperWidget);
    checkRollerValue(&lowerWidget);
}

bool SettingsScreen::shouldSwitch()
{
    bool ret = screenSwitch;
    if (screenSwitch)
        screenSwitch = false;
    return ret;
}

void SettingsScreen::checkRollerValue(Widget_t *widget)
{
    uint16_t selected_index = widget->roller.selected();

    switch (selected_index)
    {
    case 0:
        settings.setMonitor(widget->type, Settings::DisplaySettings::CO2);
        break;
    case 1:
        settings.setMonitor(widget->type, Settings::DisplaySettings::TEMPERATURE);
        break;
    case 2:
        settings.setMonitor(widget->type, Settings::DisplaySettings::HUMIDITY);
        break;
    default:
        break;
    }
}

void SettingsScreen::checkBrightnessSlider()
{
    int32_t min_value = brightnessSettings.slider.left_value();
    int32_t max_value = brightnessSettings.slider.value();
    settings.setMinBrightness((uint8_t)min_value);
    settings.setMaxBrightness((uint8_t)max_value);
}

void SettingsScreen::destroy()
{
    screenSwitch = false;

    settingsscreen_screen->del();
}

void SettingsScreen::handleLongPress()
{
    return;
}

void SettingsScreen::onDonePressed()
{
    screenSwitch = true;
}

void SettingsScreen::onBrightnessMenuPressed()
{
    toggleBrightnessControls();
}

void SettingsScreen::onBrightnessButtonPressed(lv::Event e)
{
    auto clicked_btn = e.target();

    auto *data = static_cast<uint8_t *>(clicked_btn.get_user_data());

    uint8_t index = *data;

    selected_button_index = index;

    for (auto &button : brightnessSettings.buttons)
    {
        button.button.remove_style(brightness_button_style_selected, LV_PART_MAIN)
            .add_style(brightness_button_style_unselected, LV_PART_MAIN);
    }
    clicked_btn.remove_style(brightness_button_style_unselected, LV_PART_MAIN)
        .add_style(brightness_button_style_selected, LV_PART_MAIN);

    uint32_t ms = brightness_buttons_values[index] * 60000;
    settings.setDisplayOffDelay(ms);
}
