#include "DisplayHandler.hpp"
#include "DisplayScreenBase.hpp"

#include "cmsis_os.h"
#include "display.h"
// #include "DisplayDriverBase.hpp"
#include "main.h"
#include <homescreen.hpp>

/* Thread Handles */
osThreadId_t LvglTaskHandle;

const osThreadAttr_t LvglTaskHandle_attributes = {
    .name = "LvglTask",
    .stack_size = 8192,
    .priority = (osPriority_t)osPriorityAboveNormal6,
};
/* End Task Handles */

using namespace App;

void DisplayHandler::init()
{
    // if (driver)
    // {
    //     driver->lvglDisplayInit();
    // }

    LvglTaskHandle = osThreadNew(DisplayHandler::taskWrapper, this, &LvglTaskHandle_attributes);

    if (LvglTaskHandle == NULL)
    {
        /* ERROR: The task was NOT created.
           Execution will never reach the StartLvglTask function. */
        Error_Handler();
    }
}

void DisplayHandler::setScreen(UI::DisplayScreenBase &newScreen)
{
    if (currentScreen)
        currentScreen->destroy();
    currentScreen = &newScreen;
    currentScreen->init();
}

void DisplayHandler::taskWrapper(void *argument)
{
    DisplayHandler *handler = static_cast<DisplayHandler *>(argument);
    handler->LVGLTask();
}
void DisplayHandler::LVGLTask()
{
    /* Initialize LVGL */
    display_initLvgl();
    homescreen.init();

    for (;;)
    {
        homescreen.update();
        /* The task running lv_timer_handler should have lower priority than that running
         * `lv_tick_inc` */
        lv_timer_handler();

        osDelay(10);
    }
}