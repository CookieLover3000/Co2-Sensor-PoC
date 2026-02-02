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
    if (currentState == BOOT)
        return;

    if (touchHandler->isTouched())
    {
        display->setBrightness(settings.getMaxBrightness());
        brightnessTimer.start(settings.getDisplayOffDelay());
        isFading = false;
    }

    if (!isFading && brightnessTimer.isExpired())
    {
        isFading = true;
        fadeStartTime = osKernelGetTickCount();
        startBrightness = settings.getMaxBrightness();
    }

    if (!isFading)
        return;

    uint32_t elapsed = osKernelGetTickCount() - fadeStartTime;

    if (elapsed >= fadeDuration)
    {
        display->setBrightness(settings.getMinBrightness());
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

void DisplayHandler::lvglLongPressCb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_LONG_PRESSED)
    {

        lv_obj_t *touchedObj = lv_indev_get_active_obj();

        if (touchedObj != lv_screen_active())
            return;

        DisplayHandler *instance = (DisplayHandler *)lv_event_get_user_data(e);
        if (instance)
        {
            instance->currentScreen->handleLongPress();
        }
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
        return;
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

void DisplayHandler::stateMachine()
{
    if (currentScreen->shouldSwitch())
    {
        switch (currentState)
        {
        case BOOT:
            switchScreen(HOME);
            break;
        case HOME:
            switchScreen(SETTINGS);
        case SETTINGS:
            break;
        }
    }
}

void DisplayHandler::taskWrapper(void *argument)
{
    DisplayHandler *handler = static_cast<DisplayHandler *>(argument);
    handler->LVGLTask();
}
void DisplayHandler::LVGLTask()
{
    brightnessTimer.start(settings.getDisplayOffDelay());
    if (!display)
    {
        // TODO: Error handling
    }
    display->lvglDisplayInit();
    display->setBrightness(settings.getMaxBrightness());

    // Initialize the touch interface.
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchHandler->lvglTouchPad_cb);
    lv_indev_set_user_data(indev, touchHandler);
    lv_indev_set_long_press_time(indev, long_press_time);

    lv_indev_add_event_cb(indev, DisplayHandler::lvglLongPressCb, LV_EVENT_LONG_PRESSED, this);

    currentState = BOOT;
    currentScreen = &bootscreen;
    currentScreen->init();

    while (1)
    {
        stateMachine();

        updateBrightnessLogic();
        currentScreen->update();
        lv_timer_handler();

        osDelay(10);
    }
}