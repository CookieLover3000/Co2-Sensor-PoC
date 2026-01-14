#ifndef DISPLAY_DRIVER_BASE_H
#define DISPLAY_DRIVER_BASE_H

#include <cstdint>

namespace Drivers
{

class DisplayDriverBase
{
  public:
    virtual ~DisplayDriverBase() = default;
    virtual void lvglDisplayInit() = 0;
    virtual void setBrightness(uint8_t percentage) = 0;
};

} // namespace Drivers
#endif /* DISPLAY_DRIVER_BASE_H */