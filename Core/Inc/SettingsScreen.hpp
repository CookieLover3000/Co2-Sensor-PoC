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

    /* LVGL objects */
    struct
    {
        lv_obj_t *arc;
        lv_obj_t *roller;
    } Widget_t;

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

    BrightnessSettings_t brightnessSettings = {};
    DoneButton_t done = {};

    lv_obj_t *settingsscreen_screen = nullptr;

    /* end LVGL objects */

    /* variables */
    Settings::DisplaySettings settings;

    bool screenSwitch = false;
    /* end variables */

    /* private functions */
    void initBrightnessControls(void);
    void initButtons(void);
    void checkBrightnessSlider(void);
    // callbacks
    static void onDonePressed(lv_event_t *e);
    static void onBrightnessButtonPressed(lv_event_t *e);
    /* end private functions */
};
} // namespace UI

#endif /* SETTINGS_SCREEN_H */