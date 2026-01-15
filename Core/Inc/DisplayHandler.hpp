#include "DisplayDriverBase.hpp"
#include "DisplayScreenBase.hpp"
#include "NonBlockingTimer.h"
#include "SensorHandler.hpp"
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
    void showHomescreen();
    void switchScreen(ScreenState newState);

  private:
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
    UI::DisplayScreenBase *currentScreen = nullptr;

    bool bootFinished = false;

    // display brightness control
    NonBlockingTimer brightnessTimer;
    struct DisplaySettings
    {
        uint8_t max_brightness = 100;       // percentage
        uint8_t min_brightness = 5;         // percentage
        uint32_t display_off_delay = 60000; // one minute
    };
    DisplaySettings settings;

    uint32_t fadeStartTime = 0;
    uint32_t fadeDuration = 4000;
    int startBrightness = 100;
    bool isFading = false;
};

} // namespace App