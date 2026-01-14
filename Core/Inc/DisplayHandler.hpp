#include "DisplayDriverBase.hpp"
#include "DisplayScreenBase.hpp"
#include "NonBlockingTimer.h"
#include "SensorHandler.hpp"
#include "TouchHandler.hpp"
#include "homescreen.hpp"
#include <cstdint>

namespace App
{
class DisplayHandler
{
  public:
    DisplayHandler(Drivers::DisplayDriverBase *drv, SensorHandler &sensor, TouchHandler *touch)
        : touchHandler(touch), homescreen(sensor), display(drv)
    {
    }
    void init();
    void setScreen(UI::DisplayScreenBase &newScreen);
    void showHomescreen();

  private:
    static void taskWrapper(void *argument);
    void LVGLTask();
    void updateBrightnessLogic();

    TouchHandler *touchHandler;
    UI::Homescreen homescreen;
    NonBlockingTimer timer;
    Drivers::DisplayDriverBase *display;
    UI::DisplayScreenBase *currentScreen = nullptr;
    osThreadId_t LvglTaskHandle;

    const osThreadAttr_t LvglTaskHandle_attributes = {
        .name = "LvglTask",
        .stack_size = 8192,
        .priority = (osPriority_t)osPriorityAboveNormal6,
    };

    // display brightness control
    struct DisplaySettings
    {
        uint8_t max_brightness = 100;       // percentage
        uint8_t min_brightness = 0;         // percentage
        uint32_t display_off_delay = 60000; // one minute
    };
    DisplaySettings settings;

    uint32_t fadeStartTime = 0;
    uint32_t fadeDuration = 4000;
    int startBrightness = 100;
    bool isFading = false;
};

} // namespace App