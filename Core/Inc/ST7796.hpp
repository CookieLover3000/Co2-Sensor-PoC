#ifndef ST7796_H
#define ST7796_H

#include "DisplayDriverBase.hpp"
#include "lvgl.h"
#include "stm32wbxx_hal.h"
#include <cstdint>

// WARNING: Can only drive a single display because of static functions.

#define BUS_SPI1_POLL_TIMEOUT 0x1000U

namespace Drivers
{
namespace SPI
{

class ST7796 : public DisplayDriverBase
{
  public:
    ST7796(SPI_HandleTypeDef *hspi_ptr, TIM_HandleTypeDef *htim_ptr) : hspi(hspi_ptr), htim(htim_ptr) {}
    ~ST7796() {}

    virtual void lvglDisplayInit(void) override;
    virtual void setBrightness(uint8_t percentage) override;

  private:
    SPI_HandleTypeDef *hspi;
    TIM_HandleTypeDef *htim;

    lv_display_t *lcd_disp;

    static ST7796 *instance;

    volatile int lcd_bus_busy = 0;

    static constexpr uint16_t LCD_H_RES = 320;
    static constexpr uint16_t LCD_V_RES = 480;
    static constexpr size_t DRAW_BUF_SIZE = (LCD_H_RES * LCD_V_RES / 10 * 2);
    static uint8_t buf1[DRAW_BUF_SIZE] __attribute__((aligned(32)));
    static uint8_t buf2[DRAW_BUF_SIZE] __attribute__((aligned(32)));

    void init_backlight(void);
    static void lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi);
    int32_t lcd_io_init(void);
    static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
                             size_t param_size);
    static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                               size_t param_size);
};

} // namespace SPI
} // namespace Drivers

#endif /* ST7796_H */