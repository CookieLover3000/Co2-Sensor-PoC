#ifndef SCD40_H
#define SCD40_H

#include "SensorDriverBase.hpp"
#include "cmsis_os2.h"
#include "stm32wbxx_hal.h"
#include "stm32wbxx_hal_i2c.h"

#define WRITE_MEASUREMENT_COMMAND_SIZE 2
#define READ_MEASUREMENT_SIZE          2

namespace Drivers
{
namespace I2C
{
class SCD40 : public SensorDriverBase
{
  public:
    SCD40(I2C_HandleTypeDef *handler) : hi2c(handler){}
    ~SCD40() {}
    virtual void init() override;
    virtual SensorMessage read() override;
    virtual SensorType getType() override;

  private:
    osMessageQueueId_t queueHandle;
    const osMessageQueueAttr_t scd40Queue_attributes = {.name = "co2 Queue"};

    uint16_t scd40_address = (0x62 << 1);
    uint8_t start_periodic_measurement[WRITE_MEASUREMENT_COMMAND_SIZE] = {0x21, 0xb1};
    uint16_t read_measurement = 0xec05;

    I2C_HandleTypeDef *hi2c;

    uint8_t calculate_crc8(uint8_t *data, size_t length);
};
} // namespace I2C
} // namespace Drivers

#endif /* SCD40_H */