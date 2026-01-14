#ifndef FT6336U_H
#define FT6336U_H

#include "I2CDevice.hpp"
#include "TouchDriverBase.hpp"
#include "cmsis_os2.h"
#include "stm32wbxx_hal.h"

extern "C"
{
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
}

namespace Drivers
{
namespace I2C
{
class FT6336U : I2CDevice, public TouchDriverBase
{
  public:
    FT6336U(I2C_HandleTypeDef *handle) : I2CDevice(handle) { instance = this; }
    ~FT6336U() {}
    virtual void init() override;
    virtual bool read() override;
    virtual void GetTouchData(TouchData *data) override;

    friend void ::HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

  private:
    osMutexId_t dataMutex;
    static FT6336U *instance;
    osThreadId_t touchTaskHandle;
    volatile uint8_t touch_detected = 0;
    void task();
    static void taskWrapper(void *argument);

    const osThreadAttr_t touchTask_attributes = {
        .name = "touchTask",
        .stack_size = 1024,
        .priority = (osPriority_t)osPriorityAboveNormal7,
    };

    TouchData point;

    void handleInterrupt();
};
} // namespace I2C
} // namespace Drivers

void FT6336U_Init(void);

#endif /* FT6336U_H */