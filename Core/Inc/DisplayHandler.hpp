#include "DisplayScreenBase.hpp"
#include "DisplayDriverBase.hpp"
#include <homescreen.hpp>

namespace App
{
class DisplayHandler
{
  public:
    void init();
    void setScreen(UI::DisplayScreenBase &newScreen);
    void showHomescreen();

  private:
    static void taskWrapper(void *argument);
    void LVGLTask();
    UI::Homescreen homescreen;
    UI::DisplayScreenBase *currentScreen = nullptr;
};

} // namespace App