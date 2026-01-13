#ifndef DISPLAY_DRIVER_BASE_H
#define DISPLAY_DRIVER_BASE_H

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