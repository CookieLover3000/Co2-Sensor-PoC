#ifndef DISPLAY_DRIVER_BASE_H
#define DISPLAY_DRIVER_BASE_H

#include "stm32wbxx_hal.h"

namespace Drivers
{

class DisplayDriverBase
{
  public:
    virtual ~DisplayDriverBase() = default;
    virtual void lvglDisplayInit() = 0;
};

} // namespace Drivers
#endif /* DISPLAY_DRIVER_BASE_H */