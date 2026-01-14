#ifndef TOUCH_DRIVER_BASE_H
#define TOUCH_DRIVER_BASE_H

#include <stdint.h>

namespace Drivers
{
struct TouchData
{
    uint16_t x;
    uint16_t y;
    uint8_t status; // 1 = Pressed, 0 = Released
};

class TouchDriverBase
{
  public:
    virtual void init() = 0;
    virtual bool read() = 0;
    virtual void GetTouchData(TouchData *data) = 0;
};
} // namespace Drivers

#endif /* TOUCH_DRIVER_BASE_H */