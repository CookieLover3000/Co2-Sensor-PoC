
#include "SensorHandler.hpp"
#include "SensorDriverBase.hpp"
#include "main.h"

using namespace App;

osMessageQueueId_t SensorHandler::getQueueHandle(size_t index) const
{
    if (index < SENSOR_AMOUNT && sensors[index] != nullptr)
    {
        return sensors[index]->queueHandle;
    }
    return nullptr; // TODO: Error handling
}

void SensorHandler::init()
{
    for (auto *var : sensors)
    {
        if (var == nullptr || var->sensor == nullptr || var->queueHandle != nullptr)
        {
            continue;
        }

        switch (var->sensor->getType())
        {
        case Drivers::SensorType::SCD40:
            var->queueHandle = osMessageQueueNew(10, sizeof(Drivers::SCD40Data), NULL);
            if (var->queueHandle == nullptr)
            {
                Error_Handler();
            }
            break;
        case Drivers::SensorType::SHT40:
            var->queueHandle = osMessageQueueNew(10, sizeof(Drivers::SHT40Data), NULL);
            if (var->queueHandle == nullptr)
            {
                Error_Handler();
            }
            break;
        default:
            // TODO: Error handling
            break;
        }
        var->sensor->init();
    }
    SensorTaskHandle = osThreadNew(taskWrapper, this, &SensorTaskHandle_attributes);
}

void SensorHandler::sensorTask()
{
    osDelay(5000);
    while (1)
    {
        for (auto *var : sensors)
        {
            if (var != nullptr && var->sensor != nullptr)
            {
                Drivers::SensorMessage msg;
                var->sensor->read(&msg);

                processData(msg, var->queueHandle);
            }
            osDelay(1000);
        }
    }
    osDelay(5000);
}

void SensorHandler::taskWrapper(void *argument)
{
    SensorHandler *handler = static_cast<SensorHandler *>(argument);
    handler->sensorTask();
}

void SensorHandler::processData(const Drivers::SensorMessage &msg, osMessageQueueId_t queueHandle)
{
    switch (msg.type)
    {

    case Drivers::SensorType::SCD40:
        osMessageQueuePut(queueHandle, &msg.payload.scd40, 0U, 0U);
        break;

    case Drivers::SensorType::SHT40:
        osMessageQueuePut(queueHandle, &msg.payload.sht40, 0U, 0U);
        break;
    }
}