#ifndef DISPLAY_H
#define DISPLAY_H

#include "lvgl.h"

#define LCD_H_RES             320
#define LCD_V_RES             480
#define BUS_SPI1_POLL_TIMEOUT 0x1000U

#define BYTES_PER_PIXEL       (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
#define BUFF_SIZE             (LCD_V_RES * 10 * BYTES_PER_PIXEL)

void display_initLvgl(void);

#endif /* DISPLAY_H */