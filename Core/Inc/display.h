#ifndef DISPLAY_H
#define DISPLAY_H
#ifdef __cplusplus
extern "C" {
#endif
#include "lvgl.h"

void display_initLvgl(void);
void display_setbacklight_brightness(uint8_t percentage);
int32_t lcd_io_init(void);
void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size,
                         const uint8_t *param, size_t param_size);
void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                           size_t param_size);
#ifdef __cplusplus
}
#endif
#endif /* DISPLAY_H */