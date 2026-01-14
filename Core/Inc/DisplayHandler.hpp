#include "DisplayDriverBase.hpp"
#include "DisplayScreenBase.hpp"
#include "SensorHandler.hpp"
#include "homescreen.hpp"
#include <TouchHandler.hpp>

namespace App
{
class DisplayHandler
{
  public:
    DisplayHandler(Drivers::DisplayDriverBase *drv, SensorHandler &sensor, TouchHandler *touch)
        : touchHandler(touch), homescreen(sensor), driver(drv)
    {
    }
    void init();
    void setScreen(UI::DisplayScreenBase &newScreen);
    void showHomescreen();

  private:
    static void taskWrapper(void *argument);
    void LVGLTask();

    TouchHandler *touchHandler;
    UI::Homescreen homescreen;
    Drivers::DisplayDriverBase *driver;
    UI::DisplayScreenBase *currentScreen = nullptr;
    osThreadId_t LvglTaskHandle;

    const osThreadAttr_t LvglTaskHandle_attributes = {
        .name = "LvglTask",
        .stack_size = 8192,
        .priority = (osPriority_t)osPriorityAboveNormal6,
    };
};

} // namespace App