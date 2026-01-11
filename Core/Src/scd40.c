#include "scd40.h"
#include "cmsis_os2.h"
#include "main.h"
#include "stm32wbxx_hal_i2c.h"

#define WRITE_MEASUREMENT_COMMAND_SIZE 2
#define READ_MEASUREMENT_SIZE          2

extern I2C_HandleTypeDef hi2c1;

osMessageQueueId_t scd40QueueHandle;
const osMessageQueueAttr_t scd40Queue_attributes = {.name = "co2 Queue"};

osThreadId_t scd40TaskHandle;
const osThreadAttr_t Scd40TaskHandle_attributes = {
    .name = "scd40Task", .priority = (osPriority_t)osPriorityAboveNormal, .stack_size = 2048};

scd40Data_t scd40Data = {0};

static uint8_t calculate_crc8(uint8_t *data, size_t length);
static void scd40_task(void *arg);


void scd40_initSensor()
{
    scd40QueueHandle = osMessageQueueNew(10, sizeof(scd40Data_t), &scd40Queue_attributes);

    scd40TaskHandle = osThreadNew(scd40_task, NULL, &Scd40TaskHandle_attributes);

    if (scd40TaskHandle == NULL) {
        Error_Handler();
    }
}

// TODO: Change task to use interrupt and non blocking commands.
// As the blocking commands might interfere with the ui task.
static void scd40_task(void *arg)
{
    uint16_t scd40_address =
        (0x62 << 1); // bit shift 7 bit address to 8 bits as that is what the stm32 HAL expects.
    uint8_t start_periodic_measurement[READ_MEASUREMENT_SIZE] = {0x21, 0xb1};
    uint8_t read_measurement[READ_MEASUREMENT_SIZE] = {0xec, 0x05};

    HAL_I2C_Master_Transmit(&hi2c1, scd40_address, start_periodic_measurement,
                            WRITE_MEASUREMENT_COMMAND_SIZE, 100);

    osDelay(5000);

    while (1) {
        uint8_t raw_data[9];

        if (HAL_I2C_Master_Transmit(&hi2c1, scd40_address, read_measurement, READ_MEASUREMENT_SIZE,
                                    100) == HAL_OK) {
            osDelay(1);

            if (HAL_I2C_Master_Receive(&hi2c1, scd40_address, raw_data, 9, 100) == HAL_OK) {
                if (calculate_crc8(&raw_data[0], 2) == raw_data[2]) {
                    scd40Data.co2 = (raw_data[0] << 8) | raw_data[1];
                } else {
                    // TODO: Implement error handling
                }

                if (calculate_crc8(&raw_data[3], 2) == raw_data[5]) {
                    uint16_t temp_raw = (raw_data[3] << 8) | raw_data[4];
                    scd40Data.temperature = -45.0f + 175.0f * (float)temp_raw / 65536.0f;
                } else {
                    // TODO: Implement error handling
                }

                if (calculate_crc8(&raw_data[6], 2) == raw_data[8]) {
                    uint16_t humid_raw = (raw_data[6] << 8) | raw_data[7];
                    scd40Data.humidity = 100.0f * (float)humid_raw / 65536.0f;
                } else {
                    // TODO: Implement error handling
                }
            }
            osMessageQueuePut(scd40QueueHandle, &scd40Data, 0U, 0U);
        }

        osDelay(5000);
    }
}

static uint8_t calculate_crc8(uint8_t *data, size_t length)
{
    uint8_t crc = 0xFF;
    const uint8_t polynomial = 0x31;

    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];

        for (int j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ polynomial);
            else
                crc <<= 1;
        }
    }
    return crc;
}