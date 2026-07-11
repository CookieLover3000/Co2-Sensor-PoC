#ifndef BOOTSCREEN_H
#define BOOTSCREEN_H

#include "DisplayScreenBase.hpp"
#include "NonBlockingTimer.h"
#include <lv/lv.hpp>
#include <optional>

namespace UI
{

class Bootscreen : public DisplayScreenBase
{
  public:
    ~Bootscreen();

    virtual void init(void) override;
    virtual void load(void) override;
    virtual void update(void) override;
    virtual void destroy(void) override;
    virtual bool shouldSwitch(void) override;
    virtual void handleLongPress(void) override;

  private:
    std::optional<lv::Screen> bootscreen_screen;
    lv::Label boot_text;
    NonBlockingTimer timer;
    static constexpr uint32_t bootDuration = 6000; // Sensors take about 5000 ms to initialize.
    bool done;
};

} // namespace UI
#endif /* BOOTSCREEN_H */