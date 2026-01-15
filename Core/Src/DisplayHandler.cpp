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

void DisplayHandler::updateBrightnessLogic()
{
    if (touchHandler->isTouched())
    {
        display->setBrightness(settings.max_brightness);
        brightnessTimer.start(settings.display_off_delay);
        isFading = false;
    }

    if (!isFading && brightnessTimer.isExpired())
    {
        isFading = true;
        fadeStartTime = osKernelGetTickCount();
        startBrightness = settings.max_brightness;
    }

    if (!isFading)
        return;

    uint32_t elapsed = osKernelGetTickCount() - fadeStartTime;

    if (elapsed >= fadeDuration)
    {
        display->setBrightness(settings.min_brightness);
        isFading = false;
    }
    else
    {
        // Start + (End - Start) * (Elapsed / Total)
        float progress = (float)elapsed / (float)fadeDuration;
        int currentBrightness = startBrightness + (int)((0 - startBrightness) * progress);

        display->setBrightness(currentBrightness);
    }
}

void DisplayHandler::switchScreen(ScreenState newState)
{
    UI::DisplayScreenBase *nextScreen = nullptr;

    switch (newState)
    {
    case BOOT:
        nextScreen = &bootscreen;
        break;
    case HOME:
        nextScreen = &homescreen;
        break;
    case SETTINGS:
        break;
    }

    if (nextScreen)
    {
        nextScreen->init();
    }

    if (currentScreen)
    {
        currentScreen->destroy();
    }

    currentState = newState;
    currentScreen = nextScreen;
}

void DisplayHandler::taskWrapper(void *argument)
{
    DisplayHandler *handler = static_cast<DisplayHandler *>(argument);
    handler->LVGLTask();
}
void DisplayHandler::LVGLTask()
{
    brightnessTimer.start(settings.display_off_delay);
    if (!display)
    {
        // TODO: Error handling
    }
    display->lvglDisplayInit();
    display->setBrightness(settings.max_brightness);

    // Initialize the touch interface.
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchHandler->lvglTouchPad_cb);
    lv_indev_set_user_data(indev, touchHandler);

    currentState = BOOT;
    currentScreen = &bootscreen;
    currentScreen->init();

    while (1)
    {
        if (currentScreen->shouldSwitch())
        {
            if (currentState == BOOT)
            {
                switchScreen(HOME);
            }
        }

        updateBrightnessLogic();
        currentScreen->update();
        lv_timer_handler();

        osDelay(10);
    }
}