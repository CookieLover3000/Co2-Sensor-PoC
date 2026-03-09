#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "DisplayScreenBase.hpp"
#include "DisplaySettings.hpp"
#include <array>
#include <lvgl.h>
#include <src/misc/lv_types.h>
#include <stdint.h>

namespace UI
{
class SettingsScreen : public DisplayScreenBase
{
  public:
    ~SettingsScreen() {}
    virtual void init(void) override;
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

    // necessary because callback needs to be static and as such can only access static variables
    struct brightnessButtonCbData
    {
        SettingsScreen *instance;
        size_t index;
    };

    static brightnessButtonCbData brightnessButtonCbDataArray[brightness_button_amount];

    static lv_style_t brightness_button_style_selected;
    static lv_style_t brightness_button_style_unselected;

    // needs to be saved to NVS
    size_t selected_button_index = 0;

    /* end button callback stuff */

    struct BrightnessDurrButton_t
    {
        lv_obj_t *button = nullptr;
        lv_obj_t *label = nullptr;
    };

    struct BrightnessSettings_t
    {
        lv_obj_t *container = nullptr;
        lv_obj_t *panel = nullptr;
        lv_obj_t *title = nullptr;
        lv_obj_t *slider = nullptr;
        std::array<BrightnessDurrButton_t, brightness_button_amount> buttons = {};
    };

    // brightness slider is in percentage.
    static constexpr int32_t slider_min_value = 0;
    static constexpr int32_t slider_max_value = 100;

    struct DoneButton_t
    {
        lv_obj_t *button = nullptr;
        lv_obj_t *label = nullptr;
    };

    struct BrightnessMenuButton_t
    {
        lv_obj_t *button = nullptr;
        lv_obj_t *label = nullptr;
    };

    struct Widget_t
    {
        lv_obj_t *arc;
        lv_obj_t *roller;
        Settings::DisplaySettings::WidgetType type;
        Settings::DisplaySettings::Monitor monitor;
        static constexpr lv_color_t active_color = LV_COLOR_MAKE(0x00, 0xAD, 0xEE);
    };

    Widget_t mainWidget = {
        .arc = NULL,
        .roller = NULL,
        .type = Settings::DisplaySettings::MAIN,
        .monitor = settings.getMainMonitor(),
    };

    Widget_t upperWidget = {
        .arc = NULL,
        .roller = NULL,
        .type = Settings::DisplaySettings::UPPER,
        .monitor = settings.getUpperMonitor(),
    };

    Widget_t lowerWidget = {
        .arc = NULL,
        .roller = NULL,
        .type = Settings::DisplaySettings::LOWER,
        .monitor = settings.getLowerMonitor(),
    };

    BrightnessSettings_t brightnessSettings = {};
    DoneButton_t done = {};
    BrightnessMenuButton_t brightnessMenu = {};

    lv_obj_t *settingsscreen_screen = nullptr;

    /* end LVGL objects */

    /* private functions */
    void initBrightnessControls(void);
    void initButtons(void);
    void checkBrightnessSlider(void);
    void toggleBrightnessControls(void);
    void initWidget(Widget_t *widget);
    void checkRollerValue(Widget_t *widget);
    // callbacks
    static void onDonePressed(lv_event_t *e);
    static void onBrightnessMenuPressed(lv_event_t *e);
    static void onBrightnessButtonPressed(lv_event_t *e);
    /* end private functions */
};
} // namespace UI

#endif /* SETTINGS_SCREEN_H */