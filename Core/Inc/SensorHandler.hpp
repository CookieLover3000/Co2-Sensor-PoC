#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include "SensorDriverBase.hpp"
#include "cmsis_os2.h"
#include <array>

#define SENSOR_AMOUNT 1

namespace App
{
struct SensorDriver
{
    Drivers::SensorDriverBase *sensor;
    osMessageQueueId_t queueHandle;
};

class SensorHandler
{

  public:
    SensorHandler(std::array<SensorDriver *, SENSOR_AMOUNT> s) { sensors = s; }
    void init();
    osMessageQueueId_t getQueueHandle(size_t index) const;

  private:
    std::array<SensorDriver *, SENSOR_AMOUNT> sensors;
    void processData(const Drivers::SensorMessage &msg, osMessageQueueId_t queue);
    void sensorTask();
    static void taskWrapper(void *argument);

    osThreadId_t SensorTaskHandle;
    const osThreadAttr_t SensorTaskHandle_attributes = {
        .name = "SensorTask",
        .stack_size = 1024 * SENSOR_AMOUNT,
        .priority = (osPriority_t)osPriorityAboveNormal,
    };
};
} // namespace App

#endif /* SENSOR_HANDLER_H */