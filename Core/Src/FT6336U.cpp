#include "FT6336U.hpp"
#include "cmsis_os2.h"
#include "main.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_def.h"
#include "stm32wbxx_nucleo.h" // Should be removed when switching to custom board.
#include <stdint.h>

#define FT6336U_ADDR  (0x38 << 1) // Standard I2C address

// Register Addresses
#define REG_TD_STATUS 0x02 // Number of touch points
#define REG_P1_XH     0x03 // Event Flag & X Position High
#define REG_P1_XL     0x04 // X Position Low
#define REG_P1_YH     0x05 // Touch ID & Y Position High
#define REG_P1_YL     0x06 // Y Position Low

using namespace Drivers;
using namespace I2C;

FT6336U *FT6336U::instance = nullptr;

void FT6336U::init()
{
    I2CDevice::initI2C();

    const osMutexAttr_t attr = {"TouchDataLock", osMutexRecursive | osMutexPrioInherit, NULL, 0};
    dataMutex = osMutexNew(&attr);

    touchTaskHandle = osThreadNew(FT6336U::taskWrapper, this, &touchTask_attributes);
}

bool FT6336U::read()
{
    uint8_t data[6];

    if (I2CTransmitRead(FT6336U_ADDR, REG_TD_STATUS, I2C_MEMADD_SIZE_8BIT, data, 6, 100, 0) != HAL_OK)
    {
        return false;
    }

    uint8_t touch_count = data[0] & 0x0F;

    if (touch_count > 0)
    {
        if (osMutexAcquire(dataMutex, 0) != osOK)
            return false;
        // Parse X and Y (Masking out event bits in high bytes, these contain event data that is unnecessary here
        // and will fuck up the calculation)
        point.x = ((data[1] & 0x0F) << 8) | data[2];
        point.y = ((data[3] & 0x0F) << 8) | data[4];
        point.status = 1;
    }
    else
    {
        point.status = 0;
    }
    osMutexRelease(dataMutex);
    return true;
}

// using this callback here is far from optimal, but it doesn't matter for this project so I will leave it like this.
// If you find this repository when searching for a FT6336U driver and have more than one GPIO Interrupt you should
// probably have this defined somewhere else.
extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == TOUCH_INT_Pin && FT6336U::instance != nullptr)
    {
        FT6336U::instance->handleInterrupt();
    }
}

void FT6336U::handleInterrupt()
{
    osThreadFlagsSet(touchTaskHandle, FT6336U_INT_RTOS_FLAG);
}

void FT6336U::taskWrapper(void *argument)
{
    FT6336U *handler = static_cast<FT6336U *>(argument);
    handler->task();
}

void FT6336U::task()
{
    HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_RESET);
    osDelay(20);
    HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_SET);
    osDelay(200); // Wait for the chip to wake up

    while (1)
    {
        uint32_t flags = osThreadFlagsWait(FT6336U_INT_RTOS_FLAG, osFlagsWaitAny, osWaitForever);

        if (flags == FT6336U_INT_RTOS_FLAG)
        {
            if (!read())
            {
                // TODO: Error handling
            }
            // Should be removed when switching to custom board.
            if (point.status == 1)
                BSP_LED_On(LED_BLUE);
            else if (point.status == 0)

                BSP_LED_Off(LED_BLUE);
        }
    }
}

void FT6336U::GetTouchData(TouchData *data)
{
    if (osMutexAcquire(dataMutex, 0) != osOK)
        return;
    if (data != nullptr)
    {
        *data = point;
    }
    osMutexRelease(dataMutex);
}
