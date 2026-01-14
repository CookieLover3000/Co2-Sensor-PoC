#include "DisplayHandler.hpp"
#include "DisplayDriverBase.hpp"
#include "DisplayScreenBase.hpp"
#include "ST7796.hpp"
#include "TouchHandler.hpp"
#include "cmsis_os.h"
#include "homescreen.hpp"
#include "main.h"
#include <lvgl.h>

/* Thread Handles */

/* End Task Handles */

using namespace App;

void DisplayHandler::init()
{

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
    if (driver)
    {
        driver->lvglDisplayInit();
    }

    // Initialize the touch interface.
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchHandler->lvglTouchPad_cb);
    lv_indev_set_user_data(indev, touchHandler);

    homescreen.init();

    for (;;)
    {
        homescreen.update();
        lv_timer_handler();

        osDelay(10);
    }
}