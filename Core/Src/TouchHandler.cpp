#include "TouchHandler.hpp"
#include "TouchDriverBase.hpp"
#include <lvgl.h>

using namespace App;

void TouchHandler::init()
{
    touchDriver->init();
}

void TouchHandler::lvglTouchPad_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    TouchHandler *self = (TouchHandler *)lv_indev_get_user_data(indev);

    Drivers::TouchData touch_point;

    self->touchDriver->GetTouchData(&touch_point);

    data->point.x = (lv_value_precise_t)touch_point.x;
    data->point.y = (lv_value_precise_t)touch_point.y;

    if (touch_point.status == 1)
    {
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

bool TouchHandler::isTouched()
{
    Drivers::TouchData data;
    touchDriver->GetTouchData(&data);

    if (data.status == 1)
        return true;
    return false;
}