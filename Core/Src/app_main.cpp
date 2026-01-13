#include "DisplayHandler.hpp"
#include "SCD40.hpp"
#include "ST7796.hpp"
#include "SensorHandler.hpp"
#include "main.h"
#include "stm32wbxx_hal_i2c.h"
#include <array>

extern "C" I2C_HandleTypeDef hi2c1;
extern "C" SPI_HandleTypeDef hspi1;
extern "C" TIM_HandleTypeDef htim16;

static Drivers::I2C::SCD40 co2HardwareDriver(&hi2c1);
static App::SensorDriver co2Wrapper = {.sensor = &co2HardwareDriver, .queueHandle = nullptr};

// 3. The array used by SensorHandler
static std::array<App::SensorDriver *, SENSOR_AMOUNT> sensors = {&co2Wrapper};

static App::SensorHandler sensorHandler(sensors);
static Drivers::SPI::ST7796 lcdDriver(&hspi1, &htim16);
static App::DisplayHandler displayHandler(&lcdDriver, sensorHandler);

extern "C"
{
void app_main()
{
    sensorHandler.init();
    displayHandler.init();
}
}