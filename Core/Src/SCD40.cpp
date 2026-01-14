#include "SCD40.hpp"
#include "SensorDriverBase.hpp"
#include "main.h"
#include "stm32wbxx_hal_i2c.h"

using namespace Drivers;
using namespace Drivers::I2C;

void SCD40::init()
{
    I2CDevice::initI2C();
    I2CTransmit(scd40_address, start_periodic_measurement, WRITE_MEASUREMENT_COMMAND_SIZE, 100, osWaitForever);
}

// SCD40.cpp
bool SCD40::read(SensorMessage *msg)
{
    uint8_t raw_data[9];
    bool success = true;

    // Set message type.
    // SensorMessage msg;
    msg->type = SensorType::SCD40;

    if (I2CTransmitRead(scd40_address, read_measurement, I2C_MEMADD_SIZE_16BIT, raw_data, 9, 100, 0) != HAL_OK)

    {
        return false;
    }

    if (calculate_crc8(&raw_data[0], 2) == raw_data[2])
    {
        msg->payload.scd40.co2 = (raw_data[0] << 8) | raw_data[1];
    }
    else
    {
        success = false;
    }

    if (calculate_crc8(&raw_data[3], 2) == raw_data[5])
    {
        uint16_t temp_raw = (raw_data[3] << 8) | raw_data[4];
        msg->payload.scd40.temperature = -45.0f + 175.0f * (float)temp_raw / 65536.0f;
    }
    else
    {
        success = false;
    }

    if (calculate_crc8(&raw_data[6], 2) == raw_data[8])
    {
        uint16_t humid_raw = (raw_data[6] << 8) | raw_data[7];
        msg->payload.scd40.humidity = 100.0f * (float)humid_raw / 65536.0f;
    }
    else
    {
        success = false;
    }

    return success;
}

SensorType SCD40::getType()
{
    return SensorType::SCD40;
}

uint8_t SCD40::calculate_crc8(uint8_t *data, size_t length)
{
    uint8_t crc = 0xFF;
    const uint8_t polynomial = 0x31;

    for (size_t i = 0; i < length; ++i)
    {
        crc ^= data[i];

        for (int j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ polynomial);
            else
                crc <<= 1;
        }
    }
    return crc;
}