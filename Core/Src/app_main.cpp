#include "DisplayHandler.hpp"
#include "main.h"
#include "scd40.h"
#include "stm32wbxx_hal_i2c.h"

extern "C" I2C_HandleTypeDef hi2c1;
extern "C" SPI_HandleTypeDef hspi1;
extern "C" TIM_HandleTypeDef htim16;

static App::DisplayHandler display;

extern "C"
{
void app_main()
{
    scd40_initSensor();
    display.init();
}
}