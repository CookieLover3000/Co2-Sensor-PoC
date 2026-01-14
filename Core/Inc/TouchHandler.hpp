#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include "TouchDriverBase.hpp"

#include <lvgl.h>

namespace App
{
class TouchHandler
{
  public:
    TouchHandler(Drivers::TouchDriverBase *driver) : touchDriver(driver) {}
    void init();
    // At the moment lvgl polls this callback, which is a lot slower than the event driven driver.
    // It should be fine for this project, but if nog it might be necessary to modify the driver/callback to buffer the
    // data instead.
    // It might also be necessary to invert the touch coördinates if the display is inverted (I don't remember)
    static void lvglTouchPad_cb(lv_indev_t *indev, lv_indev_data_t *data);
    // polling based, seems to work fine for now. Might also need to buffer it.
    bool isTouched();

  private:
    Drivers::TouchDriverBase *touchDriver;
};
} // namespace App

#endif /* TOUCH_HANDLER_H */