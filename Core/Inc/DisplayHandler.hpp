#include "DisplayScreenBase.hpp"
#include "DisplayDriverBase.hpp"
#include "SensorHandler.hpp"
#include "homescreen.hpp"

namespace App
{
class DisplayHandler
{
  public:
    DisplayHandler(Drivers::DisplayDriverBase *drv, SensorHandler &sensor) : homescreen(sensor), driver(drv){}
    void init();
    void setScreen(UI::DisplayScreenBase &newScreen);
    void showHomescreen();

  private:
    static void taskWrapper(void *argument);
    void LVGLTask();
    UI::Homescreen homescreen;
    Drivers::DisplayDriverBase *driver;
    UI::DisplayScreenBase *currentScreen = nullptr;
};

} // namespace App