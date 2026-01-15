#ifndef BOOTSCREEN_H
#define BOOTSCREEN_H

#include "DisplayScreenBase.hpp"
#include "NonBlockingTimer.h"
#include <lvgl.h>
namespace UI
{

class Bootscreen : public DisplayScreenBase
{
  public:
    ~Bootscreen();

    virtual void init(void) override;
    virtual void update(void) override;
    virtual void destroy(void) override;
    virtual bool shouldSwitch(void) override;

  private:
    lv_obj_t *bootscreen_screen = NULL;
    lv_obj_t *boot_text = NULL;
    NonBlockingTimer timer;
    static constexpr uint32_t bootDuration = 6000; // Sensors take about 5000 ms to initialize.
    bool done;
};

} // namespace UI
#endif /* BOOTSCREEN_H */