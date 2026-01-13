#ifndef SENSOR_DRIVER_BASE_H
#define SENSOR_DRIVER_BASE_H

#include <cstdint>

namespace Drivers
{

// tagged union
enum class SensorType
{
    SCD40,
    SHT40,
};

struct SCD40Data
{
    uint16_t co2;
    float temperature;
    float humidity;
};

struct SHT40Data
{
    float temperature;
    float humidity;
};

struct SensorMessage
{
    SensorType type;
    union {
        SCD40Data scd40;
        SHT40Data sht40;
    } payload;
};

class SensorDriverBase
{
  public:
    virtual ~SensorDriverBase() = default;
    virtual void init() = 0;
    virtual SensorMessage read() = 0;
    virtual SensorType getType() = 0;
};
} // namespace Drivers
#endif /* SENSOR_DRIVER_BASE_H */