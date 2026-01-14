#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include "cmsis_os2.h"
#include "stm32wbxx_hal.h"

namespace Drivers
{
namespace I2C
{

class I2CDevice
{
  public:
    I2CDevice(I2C_HandleTypeDef *handle) : hi2c(handle) {}

  protected:
    I2C_HandleTypeDef *hi2c;
    static osMutexId_t I2CMutex;
    /**
     * This function should be called in the init function of all classes that inherit I2CDevice. You can call it from
     * that class using the following code:
     * I2CDevice::initI2C();
     */
    void initI2C();

    /**
     * @brief  Read an amount of data in blocking mode from a specific memory address while mutex protected.
     * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
     *                the configuration information for the specified I2C.
     * @param  DevAddress Target device address: The device 7 bits address value
     *         in datasheet must be shifted to the left before calling the interface
     * @param  MemAddress Internal memory address
     * @param  MemAddSize Size of internal memory address
     * @param  pData Pointer to data buffer
     * @param  Size Amount of data to be sent
     * @param  Timeout Timeout duration
     * @param MutexTimeout Mutex Timeout duration
     * @retval HAL status
     */
    HAL_StatusTypeDef I2CTransmitRead(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData,
                                      uint16_t Size, uint32_t Timeout, uint32_t MutexTimeout);
    HAL_StatusTypeDef I2CTransmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout,
                                  uint32_t MutexTimeout);
};
} // namespace I2C
} // namespace Drivers

#endif /* I2C_DEVICE_H */