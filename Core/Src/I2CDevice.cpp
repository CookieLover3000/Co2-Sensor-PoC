#include "I2CDevice.hpp"
#include "cmsis_os2.h"
#include "stm32wbxx_hal_def.h"

using namespace Drivers::I2C;

osMutexId_t I2CDevice::I2CMutex = nullptr;

void I2CDevice::initI2C()
{
    if (I2CMutex == nullptr)
    {
        const osMutexAttr_t attr = {"I2C_Lock", osMutexRecursive | osMutexPrioInherit, NULL, 0};
        I2CMutex = osMutexNew(&attr);
    }
}

HAL_StatusTypeDef I2CDevice::I2CTransmitRead(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize,
                                             uint8_t *pData, uint16_t Size, uint32_t Timeout, uint32_t MutexTimeout)
{
    if (osMutexAcquire(I2CMutex, MutexTimeout) != osOK)
        return HAL_BUSY;

    HAL_StatusTypeDef I2CStatus = HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);

    osMutexRelease(I2CMutex);

    return I2CStatus;
}

HAL_StatusTypeDef I2CDevice::I2CTransmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout,
                                         uint32_t MutexTimeout)
{
    if (osMutexAcquire(I2CMutex, MutexTimeout) != osOK)
        return HAL_BUSY;

    HAL_StatusTypeDef I2CStatus = HAL_I2C_Master_Transmit(hi2c, DevAddress, pData, Size, Timeout);

    osMutexRelease(I2CMutex);

    return I2CStatus;
}
