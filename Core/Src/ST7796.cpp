#include "ST7796.hpp"
#include "./src/drivers/display/st7796/lv_st7796.h"
#include "cmsis_os.h"
#include "lvgl.h"
#include "main.h"
#include <src/misc/lv_color.h>

#include <cstdint>

using namespace Drivers::SPI;

uint8_t ST7796::buf1[ST7796::DRAW_BUF_SIZE] __attribute__((aligned(32)));
uint8_t ST7796::buf2[ST7796::DRAW_BUF_SIZE] __attribute__((aligned(32)));
ST7796 *ST7796::instance = nullptr;

void ST7796::init_backlight()
{
    setBrightness(100);
    HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
}

void ST7796::lvglDisplayInit(void)
{
    instance = this;
    init_backlight();

    lv_init();
    lv_tick_set_cb(osKernelGetTickCount);

    /* Initialize LCD I/O */
    if (lcd_io_init() != 0)
        return;

    /* Create the LVGL display object and the LCD display driver */
    lcd_disp = lv_st7796_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_BGR, lcd_send_cmd, lcd_send_color);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_90);

    lv_display_set_buffers(lcd_disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
}

void ST7796::lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi)
{
    /* CS high */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    instance->lcd_bus_busy = 0;
    lv_display_flush_ready(instance->lcd_disp);
}

/* Initialize LCD I/O bus, reset LCD */
int32_t ST7796::lcd_io_init(void)
{
    /* Register SPI Tx Complete Callback */
    HAL_SPI_RegisterCallback(hspi, HAL_SPI_TX_COMPLETE_CB_ID, lcd_color_transfer_ready_cb);

    /* reset LCD */
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);

    return HAL_OK;
}

/* Platform-specific implementation of the LCD send command function. In general this should use
 * polling transfer. */
void ST7796::lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
                          size_t param_size)
{
    LV_UNUSED(disp);
    while (instance->lcd_bus_busy)
        ; /* wait until previous transfer is finished */

    /* DCX low (command) */
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    /* CS low */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    /* send command */
    if (HAL_SPI_Transmit(instance->hspi, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK)
    {
        /* DCX high (data) */
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
        /* for short data blocks we use polling transfer */
        HAL_SPI_Transmit(instance->hspi, (uint8_t *)param, (uint16_t)param_size, BUS_SPI1_POLL_TIMEOUT);
        /* CS high */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    }
}

/* Platform-specific implementation of the LCD send color function. For better performance this
 * should use DMA transfer. In case of a DMA transfer a callback must be installed to notify LVGL
 * about the end of the transfer.
 */
void ST7796::lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    while (instance->lcd_bus_busy)
        ; /* wait until previous transfer is finished */

    /* DCX low (command) */
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    /* CS low */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    /* send command */
    if (HAL_SPI_Transmit(instance->hspi, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK)
    {
        /* DCX high (data) */
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
        /* for color data use DMA transfer */
        /* Set the SPI in 16-bit mode to match endianness */

        instance->lcd_bus_busy = 1;
        if (HAL_SPI_Transmit_DMA(instance->hspi, param, (uint16_t)param_size) != HAL_OK)
        {
            instance->lcd_bus_busy = 0;
            HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        }
        /* NOTE: CS will be reset in the transfer ready callback */
    }
}

void ST7796::setBrightness(uint8_t percentage)
{
    // Clamp the value to 100%
    if (percentage > 100)
        percentage = 100;

    uint32_t arr = htim->Instance->ARR;

    uint32_t compare_value = (static_cast<uint32_t>(percentage) * (arr + 1)) / 100;

    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, compare_value);
}
