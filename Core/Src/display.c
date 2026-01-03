#include "display.h"
#include "./src/drivers/display/st7796/lv_st7796.h"
#include "cmsis_os.h"
#include "homescreen.h"
#include "main.h"

#include <src/misc/lv_color.h>
#include <stdint.h>

/* Private defines */
#define DRAW_BUF_SIZE         (LCD_H_RES * LCD_V_RES / 10 * 2) // *2 for 16-bit color depth

#define LCD_H_RES             320
#define LCD_V_RES             480
#define BUS_SPI1_POLL_TIMEOUT 0x1000U

#define BYTES_PER_PIXEL       (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
#define BUFF_SIZE             (LCD_V_RES * 10 * BYTES_PER_PIXEL)
/* End Private defines */

/* Private variables */
extern SPI_HandleTypeDef hspi1;
lv_display_t *lcd_disp;
static volatile int lcd_bus_busy = 0;

// lvgl draw buffers. Aligned to 32-byte boundary as this is apparently best practice for DMA.
static uint8_t buf1[DRAW_BUF_SIZE] __attribute__((aligned(32)));
static uint8_t buf2[DRAW_BUF_SIZE] __attribute__((aligned(32)));
/* End Private variables */

/* Function Prototypes */
void LVGL_Task(void *argument);
static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                         const uint8_t *param, size_t param_size);
static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                           size_t param_size);
/* End Function Prototypes */

/* Thread Handles */
osThreadId_t LvglTaskHandle;

const osThreadAttr_t LvglTaskHandle_attributes = {
    .name = "LvglTask", .priority = (osPriority_t)osPriorityAboveNormal6, .stack_size = 8192};
/* End Task Handles */

void display_initLvgl(void)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);

    LvglTaskHandle = osThreadNew(LVGL_Task, NULL, &LvglTaskHandle_attributes);

    if (LvglTaskHandle == NULL) {
        /* ERROR: The task was NOT created.
           Execution will never reach the StartLvglTask function. */
        Error_Handler();
    }
}

void lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi)
{
    /* CS high */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    lcd_bus_busy = 0;
    lv_display_flush_ready(lcd_disp);
}

/* Initialize LCD I/O bus, reset LCD */
static int32_t lcd_io_init(void)
{
    /* Register SPI Tx Complete Callback */
    HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_COMPLETE_CB_ID, lcd_color_transfer_ready_cb);

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
static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                         const uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    while (lcd_bus_busy)
        ; /* wait until previous transfer is finished */

    /* DCX low (command) */
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    /* CS low */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    /* send command */
    if (HAL_SPI_Transmit(&hspi1, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
        /* DCX high (data) */
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
        /* for short data blocks we use polling transfer */
        HAL_SPI_Transmit(&hspi1, (uint8_t *)param, (uint16_t)param_size, BUS_SPI1_POLL_TIMEOUT);
        /* CS high */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
    }
}

/* Platform-specific implementation of the LCD send color function. For better performance this
 * should use DMA transfer. In case of a DMA transfer a callback must be installed to notify LVGL
 * about the end of the transfer.
 */
static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                           size_t param_size)
{
    LV_UNUSED(disp);
    while (lcd_bus_busy)
        ; /* wait until previous transfer is finished */

    /* DCX low (command) */
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    /* CS low */
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    /* send command */
    if (HAL_SPI_Transmit(&hspi1, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
        /* DCX high (data) */
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
        /* for color data use DMA transfer */
        /* Set the SPI in 16-bit mode to match endianness */

        lcd_bus_busy = 1;
        if (HAL_SPI_Transmit_DMA(&hspi1, param, (uint16_t)param_size) != HAL_OK) {
            lcd_bus_busy = 0;
            HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        }
        /* NOTE: CS will be reset in the transfer ready callback */
    }
}

void LVGL_Task(void *argument)
{
    /* Initialize LVGL */
    lv_init();

    /* Initialize LCD I/O */
    if (lcd_io_init() != 0)
        return;

    /* Create the LVGL display object and the LCD display driver */
    lcd_disp =
        lv_st7796_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_BGR, lcd_send_cmd, lcd_send_color);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_90);

    lv_display_set_buffers(lcd_disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
    homescreen_init();

    for (;;) {
        /* The task running lv_timer_handler should have lower priority than that running
         * `lv_tick_inc` */
        lv_timer_handler();
        /* raise the task priority of LVGL and/or reduce the handler period can improve the
         * performance */
        osDelay(10);
    }
}