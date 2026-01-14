#ifndef NON_BLOCKING_TIMER_H
#define NON_BLOCKING_TIMER_H

#include "cmsis_os2.h"

class NonBlockingTimer
{
  public:
    void start(uint32_t duration_ms)
    {
        startTick = osKernelGetTickCount();
        period = duration_ms;
        active = true;
    }

    bool isExpired()
    {
        if (!active)
            return false;

        if ((osKernelGetTickCount() - startTick) >= period)
        {
            active = false;
            return true;
        }
        return false;
    }

  private:
    uint32_t startTick = 0;
    uint32_t period = 0;
    bool active = false;
};

#endif /* NON_BLOCKING_TIMER_H */