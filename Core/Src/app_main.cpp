#include "DisplayHandler.hpp"
#include "main.h"
#include "scd40.h"
#include "stm32wbxx_hal_i2c.h"
#include "ST7796.hpp"

extern "C" I2C_HandleTypeDef hi2c1;
extern "C" SPI_HandleTypeDef hspi1;
extern "C" TIM_HandleTypeDef htim16;

static Drivers::SPI::ST7796 lcdDriver(&hspi1, &htim16);
static App::DisplayHandler displayHandler(&lcdDriver);

extern "C"
{
void app_main()
{
    scd40_initSensor();
    displayHandler.init();
}
}