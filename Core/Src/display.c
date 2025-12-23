#include "display.h"
#include "./src/drivers/display/st7796/lv_st7796.h"
#include "cmsis_os.h"
#include "main.h"

#include <stdint.h>

/* Private variables */
extern SPI_HandleTypeDef hspi1;
lv_display_t *lcd_disp;
volatile int lcd_bus_busy = 0;
/* End Private variables */

/* Function Prototypes */
void ui_init(lv_display_t *disp);
void LVGL_Task(void *argument);
static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                         const uint8_t *param, size_t param_size);
static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                           size_t param_size);
/* End Function Prototypes */

/* Thread Handles */
osThreadId_t LvglTaskHandle;

const osThreadAttr_t LvglTaskHandle_attributes = {
    .name = "LvglTask", .priority = (osPriority_t)osPriorityIdle, .stack_size = 2048};
/* End Task Handles */

void display_initLvgl(void)
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);

    LvglTaskHandle = osThreadNew(LVGL_Task, NULL, &LvglTaskHandle_attributes);
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
    /* Set the SPI in 8-bit mode */
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&hspi1);
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
    /* Set the SPI in 8-bit mode */
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    HAL_SPI_Init(&hspi1);
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
        hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
        HAL_SPI_Init(&hspi1);
        lcd_bus_busy = 1;
        HAL_SPI_Transmit_DMA(&hspi1, param, (uint16_t)param_size / 2);
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
        lv_st7796_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, lcd_send_cmd, lcd_send_color);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_90);

    /* Allocate draw buffers on the heap. In this example we use two partial buffers of 1/10th size
     * of the screen */
    lv_color_t *buf1 = NULL;
    // lv_color_t *buf2 = NULL;

    uint32_t buf_size = LCD_H_RES * LCD_V_RES / 10 *
                        lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

    buf1 = lv_malloc(buf_size);
    if (buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    // buf2 = lv_malloc(buf_size);
    // if (buf2 == NULL) {
    //     LV_LOG_ERROR("display buffer malloc failed");
    //     lv_free(buf1);
    //     return;
    // }
    lv_display_set_buffers(lcd_disp, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    ui_init(lcd_disp);

    for (;;) {
        /* The task running lv_timer_handler should have lower priority than that running
         * `lv_tick_inc` */
        lv_timer_handler();
        /* raise the task priority of LVGL and/or reduce the handler period can improve the
         * performance */
        osDelay(10);
    }
}

void ui_init(lv_display_t *disp)
{
    lv_obj_t *obj;

    /* set screen background to white */
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_100, 0);

    /* create label */
    obj = lv_label_create(scr);
    lv_obj_set_align(obj, LV_ALIGN_CENTER);
    lv_obj_set_height(obj, LV_SIZE_CONTENT);
    lv_obj_set_width(obj, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(obj, lv_color_black(), 0);
    lv_label_set_text(obj, "Hello World!");
}