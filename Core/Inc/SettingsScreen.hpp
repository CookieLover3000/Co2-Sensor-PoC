#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "DisplayScreenBase.hpp"
#include "DisplaySettings.hpp"

#include <lv/lv.hpp>

#include <array>
#include <cstdint>
#include <optional>

namespace UI
{
class SettingsScreen : public DisplayScreenBase
{
  public:
    ~SettingsScreen() {}
    virtual void init(void) override;
    virtual void load(void) override;
    virtual void update(void) override;
    virtual void destroy(void) override;
    virtual bool shouldSwitch(void) override;
    virtual void handleLongPress(void) override;

  private:
    /* variables */
    Settings::DisplaySettings settings;
    bool screenSwitch = false;
    /* end variables */

    /* button callback stuff */

    // Amount of buttons that can be used to set the delay before the display turns off.
    static constexpr uint8_t brightness_button_amount = 5;
    // Values display time in minutes.
    static constexpr std::array<uint8_t, brightness_button_amount> brightness_buttons_values = {1, 2, 5, 10, 15};

    uint8_t brightnessButtonCbDataArray[brightness_button_amount];

    lv::Style brightness_button_style_selected;
    lv::Style brightness_button_style_unselected;

    // needs to be saved to NVS
    uint8_t selected_button_index = 0;

    /* end button callback stuff */

    struct BrightnessDurrButton_t
    {
        lv::Button button;
        lv::Label label;
    };

    struct BrightnessSettings_t
    {
        lv::Box container;
        lv::Box panel; // I think
        lv::Label title;
        lv::Slider slider;
        std::array<BrightnessDurrButton_t, brightness_button_amount> buttons = {};
    };

    // brightness slider is in percentage.
    static constexpr int32_t slider_min_value = 0;
    static constexpr int32_t slider_max_value = 100;

    struct DoneButton_t
    {
        lv::Button button;
        lv::Label label;
    };

    struct BrightnessMenuButton_t
    {
        lv::Button button;
        lv::Label label;
    };

    struct Widget_t
    {
        lv::Arc arc;
        lv::Roller roller;
        Settings::DisplaySettings::WidgetType type;
        Settings::DisplaySettings::Monitor monitor;
        static constexpr lv::Color active_color = LV_COLOR_MAKE(0x00, 0xAD, 0xEE);
    };

    Widget_t mainWidget = {
        .arc = {},
        .roller = {},
        .type = Settings::DisplaySettings::MAIN,
        .monitor = settings.getMainMonitor(),
    };

    Widget_t upperWidget = {
        .arc = {},
        .roller = {},
        .type = Settings::DisplaySettings::UPPER,
        .monitor = settings.getUpperMonitor(),
    };

    Widget_t lowerWidget = {
        .arc = {},
        .roller = {},
        .type = Settings::DisplaySettings::LOWER,
        .monitor = settings.getLowerMonitor(),
    };

    BrightnessSettings_t brightnessSettings = {};
    DoneButton_t done = {};
    BrightnessMenuButton_t brightnessMenu = {};

    std::optional<lv::Screen> settingsscreen_screen;

    /* end LVGL objects */

    /* private functions */
    void initBrightnessControls(void);
    void initButtons(void);
    void checkBrightnessSlider(void);
    void toggleBrightnessControls(void);
    void initWidget(Widget_t *widget);
    void checkRollerValue(Widget_t *widget);
    // callbacks
    void onDonePressed();
    void onBrightnessMenuPressed();
    void onBrightnessButtonPressed(lv::Event e);
    /* end private functions */
};
} // namespace UI

#endif /* SETTINGS_SCREEN_H */