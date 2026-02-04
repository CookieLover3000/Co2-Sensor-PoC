#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include "DisplayDriverBase.hpp"
#include "DisplayScreenBase.hpp"
#include "DisplaySettings.hpp"
#include "NonBlockingTimer.h"
#include "SensorHandler.hpp"
#include "SettingsScreen.hpp"
#include "TouchHandler.hpp"
#include "bootscreen.hpp"
#include "homescreen.hpp"
#include <cstdint>

namespace App
{
class DisplayHandler
{
  public:
    enum ScreenState
    {
        BOOT,
        HOME,
        SETTINGS,
    };

    DisplayHandler(Drivers::DisplayDriverBase *drv, SensorHandler &sensor, TouchHandler *touch)
        : touchHandler(touch), display(drv), homescreen(sensor)
    {
    }
    void init();
    void setScreen(UI::DisplayScreenBase &newScreen);
    void switchScreen(ScreenState newState);
    void stateMachine();

  private:
    static void lvglLongPressCb(lv_event_t *e);
    static void taskWrapper(void *argument);
    void LVGLTask();
    void updateBrightnessLogic();

    TouchHandler *touchHandler;
    Drivers::DisplayDriverBase *display;

    osThreadId_t LvglTaskHandle;
    const osThreadAttr_t LvglTaskHandle_attributes = {
        .name = "LvglTask",
        .stack_size = 8192,
        .priority = (osPriority_t)osPriorityAboveNormal6,
    };

    // state machine
    ScreenState currentState;
    UI::Homescreen homescreen;
    UI::Bootscreen bootscreen;
    UI::SettingsScreen settingsscreen;
    UI::DisplayScreenBase *currentScreen = nullptr;

    bool bootFinished = false;

    // display brightness control
    NonBlockingTimer brightnessTimer;
    Settings::DisplaySettings settings;
    // struct DisplaySettings
    // {
    //     uint8_t max_brightness = 100;       // percentage
    //     uint8_t min_brightness = 5;         // percentage
    //     uint32_t display_off_delay = 60000; // one minute
    // };
    // DisplaySettings settings;

    static constexpr uint16_t long_press_time = 1000;

    static constexpr uint32_t fadeDuration = 4000;
    uint32_t fadeStartTime = 0;
    int startBrightness = 100;
    bool isFading = false;
};

} // namespace App

#endif /* DISPLAY_HANDLER_H */