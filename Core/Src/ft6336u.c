#include "ft6336u.h"
#include "cmsis_os2.h"
#include "main.h"
#include "stm32wbxx_nucleo.h"
#include <stdio.h>

#include <stdint.h>

extern I2C_HandleTypeDef hi2c1;

#define FT6336U_ADDR  (0x38 << 1)

// Register Addresses
#define REG_TD_STATUS 0x02 // Number of touch points
#define REG_P1_XH     0x03 // Event Flag & X Position High
#define REG_P1_XL     0x04 // X Position Low
#define REG_P1_YH     0x05 // Touch ID & Y Position High
#define REG_P1_YL     0x06 // Y Position Low

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint8_t status; // 0 = Pressed, 1 = Released
} TouchPoint;

osThreadId_t touchTaskHandle;
const osThreadAttr_t touchTask_attributes = {
    .name = "touchTask",
    .stack_size = 2048,
    .priority = (osPriority_t)osPriorityNormal,
};

void StartTouchTask(void *argument);

void FT6336U_Init(void)
{
    touchTaskHandle = osThreadNew(StartTouchTask, NULL, &touchTask_attributes);
}

uint8_t FT6336U_ReadTouch(TouchPoint *tp)
{
    uint8_t data[6];

    if (HAL_I2C_Mem_Read(&hi2c1, FT6336U_ADDR, REG_TD_STATUS, I2C_MEMADD_SIZE_8BIT, data, 6, 100) != HAL_OK)
    {
        return 0;
    }

    uint8_t touch_count = data[0] & 0x0F;
    if (touch_count > 0)
    {
        // Parse X and Y (Masking out event bits in high bytes, these contain event data that is unnecessary here)
        tp->x = ((data[1] & 0x0F) << 8) | data[2];
        tp->y = ((data[3] & 0x0F) << 8) | data[4];
        tp->status = 1;
        return 1;
    }

    return 0;
}

volatile uint8_t touch_detected = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == TOUCH_INT_Pin)
    {
        osThreadFlagsSet(touchTaskHandle, FT6336U_INT_RTOS_FLAG);
    }
}
int touched = 0;
void StartTouchTask(void *argument)
{
    TouchPoint point;

    // Reset the controller
    HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_RESET);
    osDelay(20);
    HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_SET);
    osDelay(200); // Wait for chip to wake up

    while (1)
    {
        uint32_t flags = osThreadFlagsWait(FT6336U_INT_RTOS_FLAG, osFlagsWaitAny, osWaitForever);

        if (flags == FT6336U_INT_RTOS_FLAG)
        {
            if (FT6336U_ReadTouch(&point))
            {
                BSP_LED_Toggle(LED_BLUE);
            }
        }
    }
}